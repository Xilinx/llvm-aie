//===- CodeGenFormat.cpp - Format Generator Generator
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//

#include "CodeGenFormat.h"
#include "CodeGenInstruction.h"
#include "CodeGenTarget.h"
#include "SubtargetFeatureInfo.h"
#include "Types.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/PassInstrumentation.h"
#include "llvm/IR/Value.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TableGen/DirectiveEmitter.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

using namespace llvm;

void CodeGenFormat::run(raw_ostream &o) {
  CodeGenTarget Target(Records);
  const std::string CurrentNamespace = Target.getName().str();

  std::vector<Record *> CodeGenFormatRecords =
      Records.getAllDerivedDefinitions("CodeGenFormat");
  unsigned Size = CodeGenFormatRecords.size();

  if (Size > 1)
    errs() << "Invalid number of CodeGenFormat instantiations: "
           << "There must be only one Record deriving it.\n";
  else if (Size == 0)
    errs() << "Invalid number of CodeGenFormat instantiations: "
           << "The class CodeGenFormat must be derived once.\n";
  if (Size != 1)
    exit(EXIT_FAILURE);

  const std::string FormatClassEmitted("MCFormatDesc");

  TGTargetSlots Slots(CurrentNamespace, Records.getClass("InstSlot"));
  std::vector<Record *> InstRecords =
      Records.getAllDerivedDefinitions("Instruction");
  std::vector<Record *> SlotRecords =
      Records.getAllDerivedDefinitions("InstSlot");

  // Register all Slot based Record into the data structure
  for (const Record *Slot : SlotRecords)
    Slots.addSlot(Slot);
  // Finalizing the slots, preparing them for the emission
  Slots.finalizeSlots();

  // For little-endian instruction bit encodings, reverse the bit order
  Target.reverseBitsForLittleEndianEncoding();

  // Instructions are going to be ordered as they are in
  // CodeGenEmitter
  ArrayRef<const CodeGenInstruction *> NumberedInstructions =
      Target.getInstructionsByEnumValue();

  // Our main container of Formats
  std::vector<TGInstrLayout> InstFormats;
  // Contains only multislot pseudo inst.
  std::vector<TGInstrLayout> PseudoInstFormats;

  for (const CodeGenInstruction *CGI : NumberedInstructions) {
    Record *R = CGI->TheDef;

    if ((R->getValueAsString("Namespace") == "TargetOpcode") ||
        (!R->getValue("Inst") && !R->getValue("isMultiSlotPseudo")))
      continue;

    // Build a tree of fields based on the encoding infos of the current
    // instruction
    TGInstrLayout IL(CGI, Slots);
    // For debugging purposes:
    // IL.dump(std::cout);
    InstFormats.push_back(IL);

    if (IL.hasMultipleSlotOptions())
      PseudoInstFormats.push_back(IL);
  }

  // Populate slot map for multislot pseudo inst.
  for (TGInstrLayout &PseudoInst : PseudoInstFormats)
    PseudoInst.addAlternateInstInMultiSlotPseudo(InstFormats);

  assert(Slots.size() != 0 && "no Slot detected");

  o << "#ifdef GET_FORMATS_SLOTKINDS\n"
       "#undef GET_FORMATS_SLOTKINDS\n\n";
  Slots.emitTargetSlotKindEnum(o);
  o << "#endif // GET_FORMATS_SLOTKINDS\n\n";

  o << "#ifdef GET_FORMATS_HEADER\n"
       "#undef GET_FORMATS_HEADER\n\n";
  Slots.emitTargetSlotsDeclaration(o);
  o << "#endif // GET_FORMATS_HEADER\n\n";

  o << "#ifdef GET_FORMATS_CLASS_DEF\n"
       "#undef GET_FORMATS_CLASS_DEF\n\n";
  Slots.emitTargetSlotKindClass(o);
  Slots.emitTargetSlotClass(o);
  o << "#endif // GET_FORMATS_CLASS_DEF\n\n";

  if (InstFormats.size() > 0) {
    o << "#ifdef GET_FORMATS_INFO\n"
      << "#undef GET_FORMATS_INFO\n"
      << "enum GenFormatInfo {\n"
      << "  NB_FORMATS_EMITTED = " << InstFormats.size() << "\n"
      << "};\n"
      << "#endif // GET_FORMATS_INFO\n\n";
  }

  o << "#ifdef GET_FORMATS_SLOTS_DEFS\n"
    << "#undef GET_FORMATS_SLOTS_DEFS\n\n";
  Slots.emitSlotsInfoInstantiation(o, TGInstrLayout::getNOPSlotMapper());
  o << "#endif // GET_FORMATS_SLOTS_DEFS\n\n";

  o << "#ifdef GET_FORMATS_SLOTINFOS_MAPPING\n"
       "#undef GET_FORMATS_SLOTINFOS_MAPPING\n";
  Slots.emitTargetSlotMapping(o);
  o << "#endif // GET_FORMATS_SLOTINFOS_MAPPING\n\n";

  o << "#ifdef GET_OPCODE_FORMATS_INDEX_FUNC\n"
    << "#undef GET_OPCODE_FORMATS_INDEX_FUNC\n";
  o << "std::optional<unsigned int> " << Target.getName().str()
    << "MCFormats::getFormatDescIndex";
  o << "(unsigned int Opcode) const {\n";
  o << "  switch (Opcode) {\n";
  o << "  default:\n";
  o << "    return std::nullopt;\n";
  for (const TGInstrLayout &Inst : InstFormats)
    Inst.emitOpcodeFormatIndex(o);
  o << "  }\n}\n";
  o << "#endif // GET_OPCODE_FORMATS_INDEX_FUNC\n\n";

  o << "#ifdef GET_ALTERNATE_INST_OPCODE_FUNC\n"
    << "#undef GET_ALTERNATE_INST_OPCODE_FUNC\n";

  if (!PseudoInstFormats.empty()) {
    o << "static std::vector<unsigned int> const AlternateInsts[] = {\n";
    for (const auto &Inst : PseudoInstFormats) {
      Inst.emitAlternateInstsOpcodeSet(o);
      if (Inst.getInstrName() != PseudoInstFormats.back().getInstrName())
        o << ", \n";
    }
    o << "\n};\n\n";
  }

  o << "const std::vector<unsigned int> *" << Target.getName().str()
    << "MCFormats::getAlternateInstsOpcode";
  o << "(unsigned int Opcode) const {\n";
  o << "  switch (Opcode) {\n";
  o << "  default:\n";
  o << "    return nullptr;\n";
  for (unsigned int i = 0; i < PseudoInstFormats.size(); i++)
    PseudoInstFormats[i].emitAlternateInstsOpcode(o, i);
  o << "  }\n}\n";
  o << "#endif // GET_ALTERNATE_INST_OPCODE_FUNC\n\n";

  if (InstFormats.size() > 0 && Slots.size() > 0) {
    o << "#ifdef GET_FORMATS_FORMATS_DEFS\n"
      << "#undef GET_FORMATS_FORMATS_DEFS\n\n";

    ConstTable FieldsHierarchy("MCFormatField", "FieldsHierarchyTables");

    unsigned BaseIndex = 0;
    for (const TGInstrLayout &Inst : InstFormats)
      Inst.emitFlatTree(FieldsHierarchy, BaseIndex);
    FieldsHierarchy.finish();

    o << FieldsHierarchy;

    {
      ConstTable OpFields("const MCFormatField *", "OpFields");
      ConstTable FieldRanges("const MCFormatField * const *", "FieldRanges");
      ConstTable Formats(FormatClassEmitted, "Formats");
      for (const TGInstrLayout &Inst : InstFormats) {
        Inst.emitFormat(FieldsHierarchy, Formats, OpFields, FieldRanges);
        Formats.next();
      }

      Formats.finish();
      OpFields.finish();
      FieldRanges.mark("Sentinel");
      FieldRanges << OpFields.refNext();
      FieldRanges.finish();

      o << OpFields;
      o << FieldRanges;
      o << Formats;
      o << "#endif // GET_FORMATS_FORMATS_DEFS\n\n";
    }

    o << "#ifdef GET_FORMATS_PACKETS_TABLE\n"
      << "#undef GET_FORMATS_PACKETS_TABLE\n\n";

    // Introduce an intermediate container for the Packets which allow us
    // to sort them (as the base container can't be sorted as TGInstLayout
    // isn't Copy-assignable).
    std::vector<const TGInstrLayout *> Packets;

    for (const TGInstrLayout &Inst : InstFormats)
      if (Inst.isPacketFormat())
        Packets.push_back(&Inst);

    // Sort the Packets by size
    std::sort(Packets.begin(), Packets.end(),
              [](const TGInstrLayout *Packet0, const TGInstrLayout *Packet1) {
                return Packet0->getSize() < Packet1->getSize();
              });
    {
      // Create two flat tables, one holding all slot ranges
      // consecutively, the other with the formats,
      // referencing the ranges
      ConstTable SlotData("MCSlotKind", "FormatSlotData");
      ConstTable FormatData("VLIWFormat", "FormatData");

      // Emit each Packet in Size order
      for (const TGInstrLayout *Packet : Packets)
        Packet->emitPacketEntry(FormatData, SlotData);

      // Terminate both tables
      SlotData.finish();
      // Add a sentinel, (The 0 opcode is used by the getFormat lookup)
      FormatData.mark("Sentinel");
      FormatData << "{\n  0, nullptr, {nullptr, nullptr}, 0}\n";
      FormatData.finish();

      o << SlotData << FormatData
        << "static const PacketFormats Formats {FormatData};\n\n";

      o << "#endif // GET_FORMATS_PACKETS_TABLE\n\n";
    }
  }
}

// Retrieve the number of consecutive bits (from posBit) that are part of the
// same "variable" (described by "VarName")
// NOTE0: if the bit at posbit isn't a variable bit, then we simply return 0
// NOTE1: the counting is made in descending order (i.e from bit n-1, left one,
//        to bit 0)
unsigned CodeGenFormat::getVariableBits(const std::string &VarName,
                                        BitsInit *BI, unsigned posBit) {
  assert(BI && "BI pointer must be non-null");
  assert(posBit < BI->getNumBits() && "posBit out of range");

  unsigned counter = 0;
  for (int i = static_cast<int>(posBit); i >= 0; i--, counter++) {
    VarBitInit *VBI = nullptr;
    VarInit *VI = nullptr;
    VBI = dyn_cast<VarBitInit>(BI->getBit(i));
    if (VBI)
      VI = dyn_cast<VarInit>(VBI->getBitVar());
    if (VI) {
      // if the name is different, stop counting
      if (VI->getName() != VarName)
        break;
    } else
      break;
  }
  return counter;
}

// Retrieve the number of consecutive bits (from posBit) that are part of the
// same chunck of Fixed bits (placed into the OutChunck)
unsigned CodeGenFormat::getFixedBits(std::string &OutChunck, BitsInit *BI,
                                     unsigned posBit) {
  assert(BI && "BI pointer must be non-null");
  assert(posBit < BI->getNumBits() && "posBit out of range");

  OutChunck = "";
  unsigned counter = 0;
  for (int i = static_cast<int>(posBit); i >= 0; i--, counter++) {
    BitInit *bitInit = nullptr;
    bitInit = dyn_cast<BitInit>(BI->getBit(i));
    if (bitInit) {
      bool value = bitInit->getValue();
      OutChunck += (value) ? "1" : "0";
    } else
      break;
  }
  return counter;
}

TGInstrLayout::TGInstrLayout(const CodeGenInstruction *const CGI,
                             const TGTargetSlots &Slots)
    : CGI(CGI), SlotsRegistry(Slots), IsComposite(false),
      IsMultipleSlotOptions(false), IsSlotNOP(false), InstrID(EmissionID++) {

  // resolveIsMultipleSlotOptions is required to select Arttibute to keep track
  // of.
  resolveIsMultipleSlotOptions();

  // Default name of Instruction Attribute in TableGen record
  std::string InstrAttribute =
      IsMultipleSlotOptions ? "isMultiSlotPseudo" : "Inst";

  assert(CGI && "null pointer as CodeGenInstruction argument");
  assert(CGI->TheDef && "CodeGenInstruction doesn't own any Record reference");
  assert(CGI->TheDef->getValue(InstrAttribute) &&
         "Expected Instruction Definition in the Record");
  assert(Slots.isFinalized() && "Slots Pool must be finalized");

  Record *BaseRecord = CGI->TheDef;
  InstrName = BaseRecord->getName().str();
  Target = BaseRecord->getValueAsString("Namespace").str();
  Size = BaseRecord->getValueAsBitsInit(InstrAttribute)->getNumBits();

  InstField = std::make_shared<TGFieldLayout>(
      CGI, InstrAttribute, nullptr, Size, TGFieldLayout::FieldType::Variable);

  if (IsMultipleSlotOptions) {
    std::vector<Record *> SlotRecords =
        CGI->TheDef->getValueAsListOfDefs("materializableInto");
    assert(!SlotRecords.empty() &&
           "Expected Instructions for multi slot Pseudo instructions");
    resolveIsComposite();
    return;
  }

  BitsInit *BI = nullptr;
  unsigned HierarchyLevel = 0;

  // Find the super Class where the definition of "Inst" begin
  // This "Level" in the hierarchy will be stored in the HierarchyLevel
  for (auto &SuperClass : BaseRecord->getSuperClasses()) {
    if (SuperClass.first->getValue(InstrAttribute)) {
      BI = SuperClass.first->getValueAsBitsInit(InstrAttribute);
      break;
    }
    ++HierarchyLevel;
  }

  assert(BI && "No Bits Init found from the base Instr Attribute");

  // Recursively search of every fields definition
  // Recursion will begin with the "Inst" field
  InstField->resolveFieldsDefInHierarchy(InstrAttribute, HierarchyLevel,
                                         InstField);

  // Fields definition must be resolved before making the offsets resolution
  for (auto *Field : leaves()) {
    Field->resolveFieldsGlobalOffsets();
  }

  // Here, the order matters: IsComposite has to be resolved first as we use
  // this property to resolve IsSlot
  resolveIsComposite();
  resolveIsSlot();
  resolveMCOperandNumber();
  resolveIsSlotNOP();
}

TGInstrLayout::NOPSlotMap TGInstrLayout::NOPMapper;
unsigned TGInstrLayout::EmissionID = 0;

unsigned TGInstrLayout::getSize() const { return InstField->getSize(); }

void TGInstrLayout::dump(std::ostream &stream) const {
  stream << InstrName << " format decomposition:" << std::endl;
  ;
  InstField->dump(stream);
  stream << std::endl;
}

void TGInstrLayout::resolveIsComposite() {
  Record *BaseRecord = CGI->TheDef;
  if (BaseRecord->getValue("isComposite"))
    IsComposite = BaseRecord->getValueAsBit("isComposite");
}

void TGInstrLayout::resolveIsMultipleSlotOptions() {
  Record *BaseRecord = CGI->TheDef;
  if (BaseRecord->getValue("isMultiSlotPseudo"))
    IsMultipleSlotOptions =
        BaseRecord->getValueAsBitsInit("isMultiSlotPseudo")->getBit(0);
}

void TGInstrLayout::addAlternateInstInMultiSlotPseudo(
    const std::vector<TGInstrLayout> &InstFormats) {

  std::vector<Record *> AltInsts =
      CGI->TheDef->getValueAsListOfDefs("materializableInto");

  std::set<const TGTargetSlot *> SupportedSlot;
  for (Record *AltInst : AltInsts) {
    bool Found = false;
    for (const TGInstrLayout &Inst : InstFormats) {
      if (Inst.InstrName == AltInst->getName()) {

        const std::vector<TGFieldLayout *> SlotsVec(Inst.slots().begin(),
                                                    Inst.slots().end());
        assert(SlotsVec.size() == 1 && "Real Instr should have only one slot");
        for (auto &SlotField : SlotsVec) {
          const TGTargetSlot *Slot = SlotField->SlotClass;
          if (Slot) {
            // Add alternate instr only if no other instr with same slot exist
            if (SupportedSlot.find(Slot) == SupportedSlot.end()) {
              addAlternateInsts(Target + "::" + Inst.InstrName);
              SupportedSlot.insert(Slot);
            } else
              llvm_unreachable(
                  "An instr. with same slot already added to supported list");
          } else
            llvm_unreachable(
                "MulitSlot Pseudo Inst. with an Real Inst having UNKNOWN slot");
        }
        Found = true;
        break;
      }
    }
    if (!Found)
      llvm_unreachable("Instruction for MulitSlot Pseudo Inst. not found");
  }
}

void TGInstrLayout::resolveIsSlot() {
  // We're dealing with a Packet Format
  if (IsComposite) {
    // For all the leaves that are not fixed bits
    for (auto *Field : leaves()) {
      if (Field->isFixedBits())
        continue;

      unsigned OpIdx;
      if (CGI->Operands.hasOperandNamed(Field->Label, OpIdx)) {
        // find by Record
        TGTargetSlots::const_iterator SlotIt =
            SlotsRegistry.find(CGI->Operands[OpIdx].Rec);
        if (SlotIt != SlotsRegistry.cend())
          Field->setSlot(SlotIt->second);
        else {
          dbgs() << "Operand " << CGI->Operands[OpIdx].Rec->getName()
                 << " skipped when resolving Slot information for the "
                    "composite record "
                 << CGI->TheDef->getName() << "\n";
        }
      }
    }
  } else {
    // Slot of the BaseRecord
    Record *SlotRecord = CGI->TheDef->getValueAsDef("Slot");
    // If it's the default slot, then abort the procedure
    if (SlotRecord == SlotsRegistry.getDefaultSlot()->first)
      return;
    // Entry of the SlotRecord in the slot pool
    TGTargetSlots::const_iterator SlotPtr = SlotsRegistry.find(SlotRecord);

    // If the slot on which the Record is pointing isn't in the slot pool
    // This should not happened as the Backend has to crash when an invalid
    // slot is detected.
    if (SlotPtr == SlotsRegistry.cend()) {
      dbgs() << "error: Record " << InstrName
             << " pointing to an invalid "
                "(unregistered) slot: "
             << SlotRecord->getName() << "\n";
      exit(EXIT_FAILURE);
    }

    // Intermediate variables for the sake of clarity
    const std::string &SlotName = SlotPtr->second.getInstanceName();
    const std::string &LabelToFind = SlotPtr->second.getLabelToFind();

    // Lambda returning true if a valid slot is evaluated
    auto IsValidSlot = [&](const TGFieldLayout &FL) -> bool {
      return (FL.getLabel() == LabelToFind &&
              FL.getSize() == SlotPtr->second.getSlotSize());
    };

    // Defining an iterator over the fields with the predicate defined above.
    // NOTE: the Mode specified here isn't important as when we will encounter
    // the good slot field, we're gonna register it and then stop the research.
    // So, there is no point defining what we're going to do after the
    // predicate returns true...
    iterator_range<TGFieldIterator> IsValidSlotRange = make_range(
        TGFieldIterator(InstField.get(), TGFieldIterator::Mode::StopTraversal,
                        IsValidSlot),
        TGFieldIterator(nullptr));

    bool SlotResolved = false;
    for (auto *Field : IsValidSlotRange) {
      Field->setSlot(SlotPtr->second);
      // Make sure we really enter into that loop
      SlotResolved = true;
      break;
    }

    if (!SlotResolved) {
      dbgs() << "error: " << SlotName << " not resolved for instruction "
             << InstrName << "\n"
             << "NOTE: occurs when trying to find the field with label \""
             << LabelToFind
             << "\" in the format encoding of the instruction\n\n";
      exit(EXIT_FAILURE);
    }
  }
}

void TGInstrLayout::resolveMCOperandNumber() {
  // Property:
  // A Field which represents an MCOperand is always a leaf of the tree
  for (auto *Field : leaves()) {
    if (Field->isFixedBits())
      continue;

    // If the operand matches by name, reference according to that
    // operand number. Non-matching operands are assumed to be in
    // order.
    unsigned OpIdx;
    if (CGI->Operands.hasOperandNamed(Field->Label, OpIdx)) {
      // Get the machine operand number for the indicated operand.
      OpIdx = CGI->Operands[OpIdx].MIOperandNo;
      assert(!CGI->Operands.isFlatOperandNotEmitted(OpIdx) &&
             "Explicitly used operand also marked as not emitted!");
      Field->setMCOperandIndex(OpIdx);
    }
  }
}

void TGInstrLayout::resolveIsSlotNOP() {
  Record *BaseRecord = CGI->TheDef;
  IsSlotNOP = BaseRecord->getValueAsBit("isSlotNOP");
  // If the instruction is a NOP representative of the slot
  // (isPacketFormat is an extra check...)
  if (isSlotNOP() && !isPacketFormat()) {
    const Record *R = BaseRecord->getValueAsDef("Slot");
    if (SlotsRegistry.getDefaultSlot()->first == R) {
      errs() << "error: " << InstrName
             << " has been defined as the NOP "
                "representative of the default slot.\n";
      exit(EXIT_FAILURE);
    }
    NOPMapper.insert({R, InstrName});
  }
}

iterator_range<TGFieldIterator> TGInstrLayout::fields() const {
  return make_range(TGFieldIterator(InstField.get()), TGFieldIterator(nullptr));
}

iterator_range<TGFieldIterator> TGInstrLayout::leaves() const {
  // Create a predicate returning true iff the field is a terminal (i.e. leaf)
  auto TerminalPolicy = [](const TGFieldLayout &FL) -> bool {
    return FL.isLeaf();
  };

  // Build an iterator on the terminals performing a full traversal
  return make_range(TGFieldIterator(InstField.get(),
                                    TGFieldIterator::Mode::FullTraversal,
                                    TerminalPolicy),
                    TGFieldIterator(nullptr));
}

iterator_range<TGFieldIterator> TGInstrLayout::slots() const {
  // Create a predicate returning true iff the field is describing a slot
  auto SlotPolicy = [](const TGFieldLayout &FL) -> bool { return FL.isSlot(); };

  return make_range(TGFieldIterator(InstField.get(),
                                    TGFieldIterator::Mode::StopTraversal,
                                    SlotPolicy),
                    TGFieldIterator(nullptr));
}

iterator_range<TGFieldIterator> TGInstrLayout::operands() const {
  // This predicate returns true iff the field owns a valid MCOperand index
  auto OperandPolicy = [](const TGFieldLayout &FL) -> bool {
    return FL.isLeaf() && FL.getMCOperandIndex() != -1;
  };

  return make_range(TGFieldIterator(InstField.get(),
                                    TGFieldIterator::Mode::FullTraversal,
                                    OperandPolicy),
                    TGFieldIterator(nullptr));
}

void TGInstrLayout::emitFlatTree(ConstTable &FieldsHierarchy,
                                 unsigned &FieldIndex) const {
  const std::string GenSlotKindName = SlotsRegistry.GenSlotKindName;
  for (TGFieldLayout *Field : fields()) {
    Field->EmissionID = FieldIndex++;
  }
  FieldsHierarchy << "    // " << Target << "::" << InstrName
                  << " - Index : " << std::to_string(InstrID)
                  << " - FieldIndex : " << FieldsHierarchy.mark() << "\n";
  const char *Bracket = "{ ";
  for (const TGFieldLayout *Field : fields()) {
    FieldsHierarchy << Bracket;
    Bracket = "     { ";

    // Emitting ParentID
    if (Field->getParent() == nullptr)
      FieldsHierarchy << "-1, ";
    else
      FieldsHierarchy << Field->getParent()->EmissionID << ", ";

    if (Field->isFixedBits()) {
      FieldsHierarchy
          << "MCFormatField::MCFormatFieldType::FixedBits, nullptr, ";
    } else {
      FieldsHierarchy << "MCFormatField::MCFormatFieldType::Variable, " << '"'
                      << Field->getLabel() << "\",";
    }

    if (Field->isGlobalPositionKnown())
      FieldsHierarchy << " MCFormatField::GlobalOffsets{ "
                      << Field->GlobalOffsets.LeftOffset << ", "
                      << Field->GlobalOffsets.RightOffset << " }, ";
    else
      FieldsHierarchy << " /*LocInfos=*/std::nullopt,";

    const std::string TargetKindName = Target + SlotsRegistry.GenSlotKindName;
    const TGTargetSlot *SlotInfo = Field->getSlot();
    const TGTargetSlots::RecordSlot *DefaultSlotInfo =
        SlotsRegistry.getDefaultSlot();

    FieldsHierarchy << TargetKindName << '(' << TargetKindName << "::";
    if (SlotInfo)
      FieldsHierarchy << SlotInfo->getEnumerationString();
    else
      FieldsHierarchy << DefaultSlotInfo->second.getEnumerationString();
    FieldsHierarchy << ") }";
    FieldsHierarchy.next();
  }
}

void TGInstrLayout::emitAlternateInstsOpcodeSet(raw_ostream &o) const {
  assert(AlternateInsts.size() &&
         "AlternateInsts cannot be empty for multi slot pseudo instr");
  o << "    // " << Target << "::" << InstrName << "\n";
  o << "    { ";
  for (const auto &AltInstr : AlternateInsts) {
    o << AltInstr;
    if (AltInstr != AlternateInsts.back())
      o << ", ";
  }
  o << " }";
}

void TGInstrLayout::emitAlternateInstsOpcode(raw_ostream &o,
                                             unsigned int index) const {
  assert(AlternateInsts.size() &&
         "AlternateInsts cannot be empty for multi slot pseudo instr");
  o << "  case " << Target << "::" << InstrName << ":\n"
    << "    return &AlternateInsts[" << std::to_string(index) << "];\n";
}

void TGInstrLayout::emitOpcodeFormatIndex(raw_ostream &o) const {
  o << "  case " << Target << "::" << InstrName << ":\n"
    << "    return " << std::to_string(InstrID) << ";\n";
}

void TGInstrLayout::emitFormat(ConstTable &FieldsHierarchy, ConstTable &o,
                               ConstTable &OpFields,
                               ConstTable &FieldRanges) const {
  o << "    // " << Target << "::" << InstrName
    << " - Index : " << std::to_string(InstrID) << "\n"
    << "    {\n"
    << "      " << /*"llvm" << "::" << Namespace << "::" <<*/ InstrName
    << " /* Opcode */,\n"
    << "      " << (IsComposite ? "true" : "false") << " /* isComposite */,\n"
    << "      " << (IsMultipleSlotOptions ? "true" : "false")
    << " /* hasMultipleSlotOptions */,\n"
    << "      "
    << "/* Slots - Fields mapper */\n"
    << "      {";

  const std::vector<TGFieldLayout *> SlotsVec(slots().begin(), slots().end());
  const std::string TargetClassName = Target + SlotsRegistry.GenSlotKindName;

  for (auto &SlotField : SlotsVec) {
    o << "{ ";
    o << TargetClassName << "::" << SlotField->SlotClass->getEnumerationString()
      << ", ";
    o << FieldsHierarchy.absRef(SlotField->EmissionID);
    o << " }";
    if (&SlotField != &SlotsVec.back())
      o << ", ";
  }
  o << "},\n";

  // Group every field by their operand index (if they have one)
  // i.e. { 0 : {field0, field1}, 1 : {field2}}...
  std::map<unsigned, SmallVector<TGFieldLayout *>> FieldOpIndexMap;
  // Keep track of the maximum operand index
  unsigned MaxIdx = 0;
  for (auto *OperandField : operands()) {
    unsigned Idx = OperandField->getMCOperandIndex();
    FieldOpIndexMap[Idx].push_back(OperandField);
    MaxIdx = std::max(MaxIdx, Idx);
  }

  o << "      /* MCOperand - Slots mapper */\n";
  OpFields.mark(InstrName.c_str());
  FieldRanges.mark(InstrName.c_str());
  for (unsigned Idx = 0; Idx <= MaxIdx; Idx++) {
    FieldRanges << OpFields.ref(Idx);
    FieldRanges.next();
    auto It = FieldOpIndexMap.find(Idx);
    if (It != FieldOpIndexMap.end()) {
      for (auto &MO : It->second) {
        OpFields << "  " << FieldsHierarchy.absRef(MO->EmissionID);
        OpFields.next();
      }
    }
  }
  o << "      {" << FieldRanges.ref(0) << ", " << MaxIdx + 1 << "},\n";

  auto *RootField = *fields().begin();
  o << "      " << FieldsHierarchy.absRef(RootField->EmissionID) << "\n"
    << "    }";
}

void TGInstrLayout::emitPacketEntry(ConstTable &Packets,
                                    ConstTable &SlotData) const {
  const std::vector<TGFieldLayout *> SlotsVec(slots().begin(), slots().end());
  // Some instructions (DUMMY96, UNKNOWN128...) are indicated as Composite but
  // they don't define a Packet Format... In that case, we don't emit anything.
  if (SlotsVec.empty())
    return;

  Packets << "{\n"
          << "  " << Target << "::" << getInstrName() << ",\n"
          << "  \"" << getInstrName() << "\",\n";

  const std::string TargetSlotKindName = Target + SlotsRegistry.GenSlotKindName;

  SlotData << "// " << getInstrName() << " : " << SlotData.mark() << "\n";
  uint64_t Bits = 0;
  for (auto &Slot : SlotsVec) {
    Bits |= uint64_t(1) << Slot->getSlot()->getNumSlot();
    SlotData << TargetSlotKindName
             << "::" << Slot->getSlot()->getEnumerationString();
    SlotData.next();
  }
  Packets << "  { " << SlotData.ref(0) << "," << SlotData.refNext() << "},\n";
  Packets << "  " << std::hex << std::showbase << Bits << std::dec << "},\n";
}

/// Returns true whether each all of the bits are not complete
static bool areAllBitsNotComplete(BitsInit *BI) {
  unsigned i = 0, k = 0;
  unsigned e = BI->getNumBits();
  for (; i != e; ++i) {
    if (!BI->getBit(i)->isComplete())
      k++;
  }
  return k == e;
}

void TGFieldLayout::resolveFieldsDefInBaseRecord(
    const std::string &LabelToFind, const TGFieldLayoutPtr &BaseFieldPtr) {
  const Record *const BaseRecord = CGI->TheDef;
  BitsInit *BI = nullptr;

  if (BaseRecord->getValue(LabelToFind))
    BI = BaseRecord->getValueAsBitsInit(LabelToFind);

  if (!BI) {
    // We will not find any Record defining this fields
    DefRecord = nullptr;
    return;
  }
  DefRecord = BaseRecord;

  VarBitInit *VBI = nullptr;
  VarInit *VI = nullptr;
  BitInit *BtI = nullptr;

  for (int j = BI->getNumBits() - 1; j >= 0;) {
    VBI = nullptr;
    VI = nullptr;
    BtI = nullptr;
    std::string Componentlabel;

    if ((VBI = dyn_cast<VarBitInit>(BI->getBit(j)))) {
      VI = dyn_cast<VarInit>(VBI->getBitVar());
      if (VI)
        Componentlabel = VI->getName().str();
    } else {
      BtI = dyn_cast<BitInit>(BI->getBit(j));
    }

    if (VI) {
      // See if they are others "variable" bits to group in the current
      // Variable.
      // NOTE: getVariableBits should always return >= 0 value in this context
      // as we know that there is at least one bit inside so varsize >= 1
      unsigned VarSize = CodeGenFormat::getVariableBits(Componentlabel, BI, j);

      std::shared_ptr<TGFieldLayout> const &FL =
          BaseFieldPtr->addSubField(std::make_shared<TGFieldLayout>(
              CGI, Componentlabel, BaseFieldPtr, VarSize,
              TGFieldLayout::FieldType::Variable));

      // Recursive research of the new variable found
      // Still in the Base-Record
      FL->resolveFieldsDefInBaseRecord(Componentlabel, FL);
      j -= VarSize; // we need also to record this into a proper data structure
    } else if (BtI) {
      // let's try to see if there are other constant bits behind the current
      // one for the same reason, varsize >= 1, preventing us to be in an
      // infinite loop
      unsigned BitFieldSize =
          CodeGenFormat::getFixedBits(Componentlabel, BI, j);

      BaseFieldPtr->addSubField(std::make_shared<TGFieldLayout>(
          CGI, Componentlabel, BaseFieldPtr, BitFieldSize,
          TGFieldLayout::FieldType::FixedBits));
      j -= BitFieldSize;
    } else {
      j--;
    }
  }
}

void TGFieldLayout::resolveFieldsDefInHierarchy(
    const std::string &LabelToFind, unsigned HierarchyLevel,
    const TGFieldLayoutPtr &BaseFieldPtr) {
  const Record *const BaseRecord = CGI->TheDef;

  ArrayRef<std::pair<Record *, SMRange>> Hierarchy =
      BaseRecord->getSuperClasses();
  BitsInit *BI = nullptr;

  // if we are out of range in the Hierarchy, stop recursion...
  if (HierarchyLevel >= Hierarchy.size()) {
    // ...but before, let's check if the label is defined in the base record
    // as it could be the case.
    // For example:
    // def I64_ST_LNG : AIE_i64_mv<(ins st_slot:$st, lng_slot:$lng),
    //                             "i64_st_lng", "$st, $lng"> {
    //    bits<20> st;
    //    bits<28> lng;
    //    let alu_st = {0b1, st};
    //    let mv_all = {0b000, lng};
    // }
    // Here, the labels "alu_st" and "mv_all" are only defined in the base
    // record I64_ST_LNG
    return resolveFieldsDefInBaseRecord(LabelToFind, BaseFieldPtr);
  }
  // HierarchyLevel < Hierarchy.size() => HierarchyLevel Still valid
  else {
    const Record *CurrentLevelRecord = Hierarchy[HierarchyLevel].first;
    // if the label we are looking for isn't defined at this HierarchyLevel
    if (!CurrentLevelRecord->getValue(LabelToFind))
      // let's try on the upper level (if it is not defined there, the recursion
      // will be stopped by the first if statement)
      return resolveFieldsDefInHierarchy(LabelToFind, HierarchyLevel + 1,
                                         BaseFieldPtr);
    // Otherwise, the label is defined but it may not provide additionnal
    // information
    BI = CurrentLevelRecord->getValueAsBitsInit(LabelToFind);

    // If the field is defined but gives not extra information in the current
    // level
    if (areAllBitsNotComplete(BI))
      // then stop the execution of this one and request the same thing to the
      // upper level
      return resolveFieldsDefInHierarchy(LabelToFind, HierarchyLevel + 1,
                                         BaseFieldPtr);

    // areAllBitsNotComplete(BI) returned false
    // => The field we're looking for is defined, at least partially, at this
    // level
    DefRecord = CurrentLevelRecord;
  }

  VarBitInit *VBI = nullptr;
  VarInit *VI = nullptr;
  BitInit *BtI = nullptr;

  for (int j = BI->getNumBits() - 1; j >= 0;) {
    VBI = nullptr;
    VI = nullptr;
    BtI = nullptr;
    std::string Componentlabel;

    if ((VBI = dyn_cast<VarBitInit>(BI->getBit(j)))) {
      VI = dyn_cast<VarInit>(VBI->getBitVar());
      if (VI)
        Componentlabel = VI->getName().str();
    } else {
      BtI = dyn_cast<BitInit>(BI->getBit(j));
    }

    if (VI) {
      // See if they are others "variable" bits to group in the current
      // Variable.
      // NOTE: getVariableBits should always return >= 0 value in this context
      // as we know that there is at least one bit inside so varsize >= 1
      unsigned VarSize = CodeGenFormat::getVariableBits(Componentlabel, BI, j);

      std::shared_ptr<TGFieldLayout> const &FL =
          BaseFieldPtr->addSubField(std::make_shared<TGFieldLayout>(
              CGI, Componentlabel, BaseFieldPtr, VarSize,
              TGFieldLayout::FieldType::Variable));

      // Recursive research of the new variable found
      FL->resolveFieldsDefInHierarchy(Componentlabel, HierarchyLevel + 1, FL);
      j -= VarSize; // we need also to record this into a proper data structure
    } else if (BtI) {
      // let's try to see if there are other constant bits behind the current
      // one for the same reason, varsize >= 1, preventing us to be in an
      // infinite loop
      unsigned BitFieldSize =
          CodeGenFormat::getFixedBits(Componentlabel, BI, j);

      BaseFieldPtr->addSubField(std::make_shared<TGFieldLayout>(
          CGI, Componentlabel, BaseFieldPtr, BitFieldSize,
          TGFieldLayout::FieldType::FixedBits));
      j -= BitFieldSize;
    } else {
      j--;
    }
  }
}

void TGFieldLayout::resolveFieldsGlobalOffsets() {
  // GlobalOffsets are the positions of a given field in the global scope
  // (full encoding scope). This scope is bootstrapped by the Inst field,
  // which is the base field to override when defining a specific encoding
  // in TableGen.
  //
  // NOTE: we could performed the Global Offset resolution when building
  // the field tree (in resolveFieldsDef()) but it will add more complexity
  // into the recursive descent function. Thus, it has been prefered of
  // letting this detail on the side and dedicate another pass on this task.

  // We avoid re-computing the same nodes
  // Every nodes of the tree are resolved only once
  if (Parent && !Parent->isGlobalPositionKnown())
    Parent->resolveFieldsGlobalOffsets();

  // If it has a parent, compute the left GlobalOffsets based on the parent
  // left global offset and the (local) relative offset of the child in the
  // parent
  if (Parent)
    GlobalOffsets.LeftOffset = Parent->GlobalOffsets.LeftOffset +
                               Parent->computeRelativeChildOffset(this);
  // Otherwise, it must be the root of the tree where the left Offset is always
  // 0
  else
    GlobalOffsets.LeftOffset = 0;

  GlobalOffsets.RightOffset = GlobalOffsets.LeftOffset + this->getSize() - 1;
}

unsigned TGTargetSlots::SlotID = 0;

bool TGTargetSlots::addSlot(const Record *const R) {
  // If the instance is already finalized
  if (IsFinalized) {
    errs() << "Trying to add the slot " << R->getName()
           << " in a slot pool already finalized\n";
    exit(EXIT_FAILURE);
  }

  // Make sure the current Record is deriving from the Base Slot class
  if (!R->hasDirectSuperClass(BaseSlotClass))
    return false;

  // Instanciate the slot based on the TableGen Record
  TGTargetSlot CurrentSlot(R);

  // Check the current SlotName is unique, i.e. we should have no
  // entries in the Slots container having the same SlotName.
  // NOTE: this verification prevents us also to have multiples
  // same enumeration strings
  TGTargetSlots::const_iterator DuplicateSlot =
      findBySlotName(CurrentSlot.SlotName);

  if (DuplicateSlot != Slots.end()) {
    errs() << "Record Slot " << CurrentSlot.getInstanceName() << " and Slot "
           << DuplicateSlot->second.getInstanceName()
           << " have the same slot name (" << CurrentSlot.SlotName << ")\n"
           << "Use a different SlotName to solve this ambiguous case.\n";
    exit(EXIT_FAILURE);
  }

  // If we're adding a default slot but we already have one => error
  if (CurrentSlot.isDefaultSlot() && getDefaultSlot()) {
    errs() << "Multiples default slots defined: \n"
           << "- " << getDefaultSlot()->second.getInstanceName() << '\n'
           << "- " << CurrentSlot.getInstanceName() << '\n'
           << "One default slot (at most) must be defined\n";
    exit(EXIT_FAILURE);
  }

  const std::string SlotNamespace = CurrentSlot.getNamespace();

  if (SlotNamespace != Target) {
    dbgs() << "Conflicting namespaces between the Target (" << Target
           << ") and the current Slot (" << SlotNamespace << ").\n";
    dbgs() << "The slot (" << CurrentSlot.getInstanceName()
           << ") will not be added to the current Pool.";
    return false;
  }

  // let's find if the record is already in the Slots container
  // (integrity check). Normally, this won't happened.
  TGTargetSlots::const_iterator SlotPtr = find(R);

  // If it's the case, abort the adding procedure
  if (SlotPtr != Slots.end()) {
    errs() << "Record " << R->getName() << " already registered in the pool\n";
    exit(EXIT_FAILURE);
  }

  // If it's not the case, append the Pair in it
  Slots.push_back({R, CurrentSlot});
  return true;
}

void TGTargetSlots::finalizeSlots() {
  // Check if the default Slot exists.
  // Otherwise, create one.
  // NOTE: it is not mandatory to have a default slot defined, an artificial
  // one will be created in that case (in order to uniformize the slot logic)
  if (getDefaultSlot() == nullptr) {
    TGTargetSlot DefaultSlot(Target);
    // check if no other valid slot is labeled as the name "default"
    TGTargetSlots::const_iterator SlotPtr =
        findBySlotName(DefaultSlot.SlotName);
    // if this is not the case
    if (SlotPtr == Slots.end())
      // We're adding an extra entry in the Slots container.
      // NOTE: it's safe to add a nullptr reference here as the default slot
      // record will never be dereferenced.
      Slots.push_back({nullptr, DefaultSlot});
    else {
      errs() << "Valid slot (" << SlotPtr->second.getInstanceName()
             << ") already has \"unknown\" as SlotName: put this one as"
                " default or write explicitely another default slot record.";
      exit(EXIT_FAILURE);
    }
  }

  // Give an ID for each slot
  for (RecordSlot &Slot : Slots)
    if (Slot.second.isDefaultSlot())
      Slot.second.setNumSlot(-1);
    else
      Slot.second.setNumSlot(SlotID++);

  // Sort the slot container by ID
  std::sort(Slots.begin(), Slots.end(),
            [](const RecordSlot &Rs0, const RecordSlot &Rs1) {
              return Rs0.second.getNumSlot() < Rs1.second.getNumSlot();
            });

  IsFinalized = true;
}

void TGTargetSlots::emitTargetSlotKindClass(raw_ostream &o) const {
  assert(IsFinalized && "Internal vector needs to be finalized (i.e. sorted)");

  std::string TargetEnumName = Target + GenSlotKindName;

  o << "class " << Target << GenSlotKindName << ": public MC" << GenSlotKindName
    << " {\n"
    << "public:\n";

  // 1st Ctor - Initialization to default slot
  o << "  constexpr " << TargetEnumName << "()\n"
    << "    : MC" << GenSlotKindName << "()\n"
    << "  {\n  }\n\n";

  if (Slots.size() > 1) {
    // 2nd Ctor - Initilization by SlotKind if valid
    // We check in this constructor
    o << "  constexpr " << TargetEnumName << '(' << "int"
      << " Kind)\n"
      << "    : MC" << GenSlotKindName
      << "((Kind >= "
      // Default slot is always at index 0
      << Slots[1].second.getEnumerationString()
      << " && Kind <= " << Slots.back().second.getEnumerationString()
      << ") ? Kind : SLOT_UNKNOWN)\n"
      << "  {\n  }\n";
  }

  o << "};\n\n"; // class closing bracket
}

void TGTargetSlots::emitTargetSlotMapping(raw_ostream &o) const {

  o << "const MCSlotInfo *" << Target << "MCFormats::getSlotInfo";
  o << "(const MCSlotKind Kind) const {\n";
  o << "  switch (Kind) {\n";
  o << "  default:\n";
  o << "    return nullptr;\n";

  const std::string EnumCstPreamble = Target + GenSlotKindName + "::";
  for (const RecordSlot &Slot : Slots) {
    const TGTargetSlot &TS = Slot.second;
    // Skip the emission of the default slot
    if (TS.isDefaultSlot())
      // falls in the "default" case
      continue;
    o << "  case " << EnumCstPreamble << TS.getEnumerationString() << ":\n"
      << "    "
      << "return &" << TS.getInstanceName() << ";\n";
  }
  o << "  }\n";
  o << "}\n";
}

void TGTargetSlots::emitTargetSlotsDeclaration(raw_ostream &o) const {
  assert(IsFinalized && "Internal vector needs to be finalized (i.e. sorted)");

  const std::string TargetClassName = Target + GenSlotInfoName;
  o << "class " << TargetClassName << ";\n";
  for (const RecordSlot &Slot : Slots) {
    // Skip the emission of the default slot
    if (Slot.second.isDefaultSlot())
      continue;
    o << "extern const " << TargetClassName << " "
      << Slot.second.getInstanceName() << ";\n";
  }
}

void TGTargetSlots::emitSlotsInfoInstantiation(
    raw_ostream &o, const TGInstrLayout::NOPSlotMap &SlotMapper) const {
  assert(IsFinalized && "Internal vector needs to be finalized (i.e. sorted)");

  using SlotBits = uint64_t;
  const std::string TargetClassName = Target + GenSlotInfoName;
  const std::string TargetEnumName = Target + GenSlotKindName;

  for (const RecordSlot &Slot : Slots) {
    std::string NOPName;
    auto SlotMap = SlotMapper.find(Slot.first);

    if (SlotMap == SlotMapper.end())
      NOPName = "0";
    else
      NOPName = Target + "::" + SlotMap->second;

    const TGTargetSlot &TS = Slot.second;
    // Skip the emission of the default slot
    if (TS.isDefaultSlot())
      continue;
    o << "constexpr const " << TargetClassName << " " << TS.getInstanceName()
      << " = {\n"
      << "  " << TargetEnumName << "::" << TS.getEnumerationString() << ",\n"
      << "  \"" << TS.getSlotName() << "\",\n"
      << "  " << TS.getSlotSize()
      << ",\n"
      // Right now, we're using the slot num as SlotSet
      << "  " << (SlotBits(1) << TS.getNumSlot()) << ",\n"
      << "  " << NOPName << "\n"
      << "};"
      << "\n";
  }
}

void TGTargetSlots::emitTargetSlotClass(raw_ostream &o) const {
  const std::string TargetClassName = Target + GenSlotInfoName;
  const std::string TargetEnumName = Target + GenSlotKindName;

  o << "class " << TargetClassName << " : public MC" << GenSlotInfoName << "\n"
    << "{\n"
    << "  const " << TargetEnumName << " Kind;\n"
    << "public:\n"
    << "  constexpr " << TargetClassName << "(const " << TargetEnumName
    << " Kind, "
    << "const char* SlotName, "
    << "unsigned Size, "
    << "SlotBits SlotSet, "
    << "unsigned NopOpc)\n"
    << "    : MC" << GenSlotInfoName
    << "(SlotName, Size, SlotSet, NopOpc), Kind(Kind)\n"
    << "  {\n  }\n\n"
    << "  const " << TargetEnumName
    << " &getSlotKind() const { return Kind; }\n"
    << "};\n\n";
}

TGFieldIterator::TGFieldIterator(
    TGFieldLayout *InitField, Mode Mode,
    const std::function<bool(const TGFieldLayout &)> &Predicate)
    : TraversalMode(Mode), FunctorPolicy(Predicate) {
  bool HasToStop = false;

  if (InitField) {
    InternalStack.push(InitField);
    do {
      HasToStop = !iterateOverFields();
      if (HasToStop)
        break;
    } while (!InternalStack.empty());
  }

  if (!HasToStop && InternalStack.empty())
    FieldCurrent = nullptr;
}

inline bool TGFieldIterator::iterateOverFields() {
  FieldCurrent = InternalStack.top();
  InternalStack.pop();

  // If we are using the FullTraversal mode or the predicate is false
  if (TraversalMode == Mode::FullTraversal || (!FunctorPolicy(*FieldCurrent)))
    // Load all the subfields in the reverse order as we're storing them into
    // a stack (LIFO)
    for (reverse_iterator it = FieldCurrent->LayoutEntries.rbegin();
         it != FieldCurrent->LayoutEntries.rend(); ++it)
      InternalStack.push(it->get());
  // If the functor returns true, then we have to stop
  // The iterator will then point on this first element
  return !FunctorPolicy(*FieldCurrent);
}

TGFieldIterator &TGFieldIterator::operator++() {
  bool HasToStop = false;

  // If FieldCurrent is the last field
  if (FieldCurrent && InternalStack.empty())
    // then put it at null
    FieldCurrent = nullptr;

  while (!InternalStack.empty()) {
    HasToStop = !iterateOverFields();
    if (HasToStop)
      break;
  }

  // if the stack is now empty but no valid element has been found
  if (!HasToStop && InternalStack.empty())
    FieldCurrent = nullptr;

  return *this;
}

TGFieldLayout *TGFieldIterator::operator*() const { return FieldCurrent; }

bool TGFieldIterator::operator==(const TGFieldIterator &Other) const {
  return FieldCurrent == Other.FieldCurrent;
}

static TableGen::Emitter::OptClass<CodeGenFormat>
    X("gen-instr-format", "Instruction Format Emitter");
