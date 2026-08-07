//===- AIEVariableInstrItineraryEmitter.cpp - Variable Instr Itinerary info
// generator ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This TableGen backend generates table-driven schedule class selection based
// on operand register classes. It produces:
// 1. A flat table of operand register class requirements
// 2. A table of schedule class variants with ArrayRefs into the flat table
// 3. A function for itinerary lookup, getSchedClass().
//
//===----------------------------------------------------------------------===//

#include "Common/CodeGenInstruction.h"
#include "Common/CodeGenTarget.h"
#include "ConstTable.h"
#include "llvm/Support/Debug.h"
#include "llvm/TableGen/Error.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TGTimer.h"
#include "llvm/TableGen/TableGenBackend.h"

using namespace llvm;

#define DEBUG_TYPE "aie-alternate-itinerary-emitter"

namespace {

// Structure to hold the parsed information for each instruction variant.
struct SchedVariantData {
  // The itinerary/sched class name (e.g., "II_ADD_NC_mv_add_ri_mSRF2IFlags").
  StringRef ItineraryName;
  // List of (OpIdx, RegClassName) pairs for this variant.
  std::vector<std::pair<unsigned, StringRef>> OperandRCs;
};

// Structure to hold all variants for a single instruction.
struct InstrVariantData {
  // The instruction name.
  StringRef InstrName;
  // All variants for this instruction.
  std::vector<SchedVariantData> Variants;
};

class AIEVariableInstrItineraryEmitter {
  // All instructions with their variant data, for table generation.
  std::vector<InstrVariantData> InstrVariants;

public:
  AIEVariableInstrItineraryEmitter(const RecordKeeper &R);
  void run(raw_ostream &OS);

private:
  void emitTables(raw_ostream &OS);
  void emitInterface(raw_ostream &OS);

  const RecordKeeper &Records;
  CodeGenTarget Target;
  std::string CurrentNamespace;
};

} // namespace

AIEVariableInstrItineraryEmitter::AIEVariableInstrItineraryEmitter(
    const RecordKeeper &R)
    : Records(R), Target(R) {
  CurrentNamespace = Target.getName().str();
}

void AIEVariableInstrItineraryEmitter::emitTables(raw_ostream &OS) {
  // Note: OperandRCRequirement, SchedVariantInfo, InstrVariantInfo, and
  // VarItinInterface are defined in AIEBaseInstrInfo.h.

  // Table 1: Flat table of operand register class requirements.
  ConstTable OpReqTable("llvm::OperandRCRequirement", "OperandRCRequirements");

  // Table 2: Schedule class variants with ArrayRefs into OpReqTable.
  ConstTable VariantsTable("SchedVariantInfo", "SchedVariants");

  // Table 3: Instruction to variants mapping with ArrayRefs into
  // VariantsTable.
  ConstTable InstrTable("InstrVariantInfo", "InstrVariantInfos");

  for (const InstrVariantData &InstrData : InstrVariants) {
    VariantsTable.mark(InstrData.InstrName.str().c_str());

    for (const SchedVariantData &Variant : InstrData.Variants) {
      OpReqTable.mark(Variant.ItineraryName.str().c_str());

      for (const auto &[OpIdx, RegClassName] : Variant.OperandRCs) {
        OpReqTable << "  {" << OpIdx << ", &" << CurrentNamespace
                   << "::" << RegClassName.str() << "RegClass}";
        OpReqTable.next();
      }

      VariantsTable << "  {" << CurrentNamespace
                    << "::Sched::" << Variant.ItineraryName.str() << ", "
                    << OpReqTable.arrayRef() << "}";
      VariantsTable.next();
    }

    InstrTable << "  {" << CurrentNamespace << "::" << InstrData.InstrName.str()
               << ", " << VariantsTable.arrayRef() << "}";
    InstrTable.next();
  }

  // Finalize and emit the tables.
  OS << OpReqTable.finish();
  OS << VariantsTable.finish();
  OS << InstrTable.finish();
}

void AIEVariableInstrItineraryEmitter::emitInterface(raw_ostream &OS) {
  OS << "// Interface object providing access to the variant tables.\n";
  OS << "static const VarItinInterface " << CurrentNamespace
     << "VarItinInterfaceImpl = {\n";
  OS << "  InstrVariantInfos\n";
  OS << "};\n\n";
  OS << "VarItinInterface " << CurrentNamespace
     << "InstrInfo::getVarItinInterface() const {\n";
  OS << "  return " << CurrentNamespace << "VarItinInterfaceImpl;\n";
  OS << "}\n";
}

void AIEVariableInstrItineraryEmitter::run(raw_ostream &OS) {
  TGTimer &Timer = Records.getTimer();
  Timer.startTimer("Process definitions");

  ArrayRef<const CodeGenInstruction *> NumberedInstructions =
      Target.getInstructions();

  for (const CodeGenInstruction *CGI : NumberedInstructions) {
    const Record *R = CGI->TheDef;
    if ((R->getValueAsString("Namespace") == "TargetOpcode") ||
        (!R->getValue("Inst") && !R->getValue("isMultiSlotPseudo")))
      continue;

    std::vector<const Record *> AltItinary =
        R->getValueAsListOfDefs("ItineraryRegPairs");
    if (AltItinary.empty())
      continue;

    InstrVariantData InstrData;
    InstrData.InstrName = R->getName();

    for (const Record *AltItinerary : AltItinary) {
      SchedVariantData Variant;
      Variant.ItineraryName =
          AltItinerary->getValueAsDef("Itinerary")->getName();

      std::vector<const Record *> OpRegType =
          AltItinerary->getValueAsListOfDefs("RegTypeList");

      auto CheckUnique = [&Variant](unsigned OpIdx) {
        return !std::any_of(
            Variant.OperandRCs.begin(), Variant.OperandRCs.end(),
            [OpIdx](const auto &Operand) { return Operand.first == OpIdx; });
      };

      for (const Record *RegType : OpRegType) {
        const unsigned OpIdx = RegType->getValueAsInt("OperandIdx");
        assert(CheckUnique(OpIdx) && "OperandIdx must be unique");
        const StringRef RegClassName =
            RegType->getValueAsDef("RegClass")->getName();
        Variant.OperandRCs.emplace_back(OpIdx, RegClassName);
      }

      assert(!Variant.OperandRCs.empty() && "OperandRCs cannot be empty");
      InstrData.Variants.push_back(std::move(Variant));
    }

    InstrVariants.push_back(std::move(InstrData));
  }

  // Sort InstrVariants by opcode for binary search.
  std::sort(InstrVariants.begin(), InstrVariants.end(),
            [&](const InstrVariantData &A, const InstrVariantData &B) {
              const CodeGenInstruction &IA =
                  Target.getInstruction(Records.getDef(A.InstrName));
              const CodeGenInstruction &IB =
                  Target.getInstruction(Records.getDef(B.InstrName));
              return IA.EnumVal < IB.EnumVal;
            });

  Timer.startTimer(
      "Emit Instruction with Alternate Itinerary based on RegClass used");
  emitSourceFileHeader("Instruction itinerary based on RegClass", OS);

  emitTables(OS);
  emitInterface(OS);
}

static TableGen::Emitter::OptClass<AIEVariableInstrItineraryEmitter>
    X("gen-aie-alternate-itinerary-emitter",
      "Generate scheduling info for instructions with alternate itineraries "
      "based on register types");
