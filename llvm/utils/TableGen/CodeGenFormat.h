//===- CodeGenFormat.h - Format Generator Generator
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//
//
// CodeGenFormat uses the descriptions of instructions and their fields to
// construct tables which provides information about specific field of the
// encoding and slot information. This Backend has been primarily designed for
// AIE but could be also used for other VLIW architecture owning slot-based
// composite instructions and in the need of additional information about the
// encoding.
//
//===----------------------------------------------------------------------===//

// READ: https://llvm.org/docs/TableGen/BackGuide.html

#ifndef LLVM_UTILS_TABLEGEN_CODEGENFORMAT_H
#define LLVM_UTILS_TABLEGEN_CODEGENFORMAT_H

#include "CodeGenInstruction.h"
#include "CodeGenTarget.h"
#include "SubtargetFeatureInfo.h"
#include "Types.h"
#include "llvm/ADT/APInt.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TableGen/DirectiveEmitter.h"
#include "llvm/TableGen/Record.h"
#include "llvm/TableGen/TableGenBackend.h"
#include <bitset>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace llvm {

class TGTargetSlots;
class TGInstrLayout;
class TGFieldLayout;
class TGFieldIterator;

// Helper class to create flat tables and some corresponding utilities
class ConstTable {
  std::stringstream Text;
  std::string Name;
  unsigned Mark = 0;
  unsigned Size = 0;

public:
  ConstTable(std::string Type, std::string Name) : Name(Name) {
    Text << "static " << Type << " const " << Name << "[] = {\n";
  }
  const std::stringstream &text() const { return Text; }
  std::stringstream &text() { return Text; }

  // Mark the start of a block of items
  unsigned mark(const char *Comment = nullptr) {
    if (Comment) {
      Text << "// " << Comment << " " << Size << "\n";
    }
    Mark = Size;
    return Mark;
  }

  // Make a reference to entry Idx, relative to the start of the table
  std::string absRef(unsigned Idx) const {
    return "&" + Name + "[" + std::to_string(Idx) + "]";
  }
  // Make a reference to entry Idx relative to the last marked block
  std::string ref(unsigned Idx) const { return absRef(Mark + Idx); }
  // Make a reference to the next entry
  std::string refNext() const { return absRef(Size); }

  // Move to the next entry
  void next() {
    Text << ",\n";
    Size++;
  }
  void finish() { Text << "};\n\n"; }
};

template <typename T> ConstTable &operator<<(ConstTable &Table, T Item) {
  Table.text() << Item;
  return Table;
}

inline raw_ostream &operator<<(raw_ostream &O, const ConstTable &Table) {
  O << Table.text().str();
  return O;
}

/// Main class of CodeGenFormat, a TableGen Backend, allowing us to generate:
/// - Format information (is it a composite instruction?).
/// - Slots information (name, positions, size, ...) for each Sub and Composite
///   instructions.
/// - MCOperands information (positions and size).
class CodeGenFormat {
  RecordKeeper &Records;

public:
  CodeGenFormat(RecordKeeper &R) : Records(R) {}

  void run(raw_ostream &o);

  static unsigned getVariableBits(const std::string &VarName, BitsInit *BI,
                                  unsigned posBit);
  static unsigned getFixedBits(std::string &OutChunck, BitsInit *BI,
                               unsigned posBit);
};

/// Main class abstracting a CodeGenInstruction (CGI).
/// Based on the CGI, the class builds a hierarchical representation of the
/// different fields of the instruction's encoding.
class TGInstrLayout {
public:
  using NOPSlotMap = std::unordered_map<const Record *, const std::string>;

private:
  const CodeGenInstruction *const CGI;
  const TGTargetSlots &SlotsRegistry;
  /// Name of the instruction
  std::string InstrName;
  /// Name of the current Target of the instruction
  std::string Target;
  /// A reference on the first field of the encoding ("Inst")
  std::shared_ptr<TGFieldLayout> InstField;
  /// Size of the instruction in Bits
  unsigned Size;
  /// Is it describing a composite VLIW instruction? (Packet Format)
  bool IsComposite;
  /// Is it a MultipleSlotOptions instruction?
  bool IsMultipleSlotOptions;
  /// Possible instructions pseudo could expand to
  /// Vector is used to preserve the sequence of Inst. mentioned in TableGen
  /// This will indirectly help in giving priority to a specific inst/slot
  std::vector<std::string> AlternateInsts;
  /// Is it the NOP instruction of a particular slot?
  bool IsSlotNOP;
  /// Mapping Container between Slots and their respective NOP instruction
  static NOPSlotMap NOPMapper;
  /// Pool of ID used for the emission
  static unsigned EmissionID;
  /// Specific ID of the instance
  unsigned InstrID;

public:
  TGInstrLayout(const CodeGenInstruction *const CGI,
                const TGTargetSlots &Slots);

  const std::shared_ptr<TGFieldLayout> &getInstField() const {
    return InstField;
  }

  /// Returns an iterator range ranging over all the encoding fields of the
  /// tree. The traversal of the tree is made in a pre-order way.
  iterator_range<TGFieldIterator> fields() const;
  /// Returns a leaf field iterator range (ranging only over the different
  /// leaves of the tree).
  iterator_range<TGFieldIterator> leaves() const;
  /// Returns a slot iterator range (ranging over the fields representing a
  /// slot only).
  iterator_range<TGFieldIterator> slots() const;
  /// Returns an iterator range, ranging on every field defining a MCOperand
  iterator_range<TGFieldIterator> operands() const;

  /// Returns the NOP/Slot Mapper static object
  static const NOPSlotMap &getNOPSlotMapper() { return NOPMapper; }

  unsigned getSize() const;
  const std::string &getInstrName() const { return InstrName; }
  bool isPacketFormat() const { return IsComposite; }
  bool hasMultipleSlotOptions() const { return IsMultipleSlotOptions; }
  bool isSlotNOP() const { return IsSlotNOP; }
  void dump(std::ostream &) const;
  std::vector<std::string> getAlternateInsts() const { return AlternateInsts; }
  void addAlternateInsts(const std::string Instr) {
    assert(IsMultipleSlotOptions &&
           "AlternateInst only possible for Pseudo Instr");
    auto it = std::find(AlternateInsts.begin(), AlternateInsts.end(), Instr);
    if (it == AlternateInsts.end())
      AlternateInsts.push_back(Instr);
    else
      llvm_unreachable("Alternate Insts already exist");
  }

  /// Emissions methods

  /// Emit a flat representation of the encoding hierarchy.
  void emitFlatTree(ConstTable &o, unsigned &BaseIndex) const;
  /// Emit the Format entries.
  void emitFormat(ConstTable &FieldHierarcht, ConstTable &Formats,
                  ConstTable &OpFields, ConstTable &FieldRanges) const;
  /// Emit a case table to get InstrID based of InstrName.
  void emitOpcodeFormatIndex(raw_ostream &o) const;
  /// Emit a set array to get AlternateInsts refs based of
  /// InstrName/PseudoOpcode
  void emitAlternateInstsOpcodeSet(raw_ostream &o) const;
  /// Emit a case table to get AlternateInsts based of InstrName/PseudoOpcode
  void emitAlternateInstsOpcode(raw_ostream &o, unsigned int index) const;
  /// Emit the Packet-Format table, used in the FormatSelector.
  void emitPacketEntry(ConstTable &FormatData, ConstTable &SlotData) const;

  void addAlternateInstInMultiSlotPseudo(
      const std::vector<TGInstrLayout> &InstFormats);

private:
  /// Check the value of the IsComposite TableGen attribute and report it into
  /// the class.
  void resolveIsComposite();

  /// Check the value of the hasMultipleSlotOptions TableGen attribute & report
  /// it into the class.
  void resolveIsMultipleSlotOptions();

  /// Mark every Field representing a Slot with its according slot.
  void resolveIsSlot();

  /// This method intends to bind on each "leaf field" the index of the
  /// correspondant MCOperand if there is one.
  void resolveMCOperandNumber();

  /// Add an entry in the NOPMapper if the current instruction is the NOP
  /// representative of a slot.
  void resolveIsSlotNOP();
};

/// This class intends to represent a 1-to-1 abstraction to the InstSlot class
/// in TableGen.
class TGTargetSlot {
  // This Friendship allow TGTargetSlots to set NumSlot at post-construction
  // time of the Slot.
  friend class TGTargetSlots;
  // The instance's name (C++ sources will take the same as in TableGen)
  std::string InstanceName;
  // The slot name attribute in TableGen ("alu" for alu_slot...)
  std::string SlotName;
  // The namespace of the current instruction
  std::string Namespace;
  // primarely used when emitting the C++ enumeration constant
  std::string EnumerationString;
  // Name of the field to find in the hierarchy
  std::string FieldToFind;
  // Unique number attributed (in the pool) for the slot.
  // It is used to generate a unique "SlotSet".
  int NumSlot;
  // Size of the Slot, in bits.
  // The storage of the size is made using an integer value as the size could
  // be negative. In this case, the slot pool (TGTargetSlots) will consider
  // this slot invalid and the Backend will crash. So, when performing the
  // emission, there will be no cases where the negative value wrapped to an
  // (high) unsigned one.
  int SlotSize;
  // Boolean indicating if the slot is a default one, meaning this is a
  // placeholder slot, no semantic is attached to it.
  bool IsDefaultSlot;
  // Name of the Instr representing the NOP of the slot
  std::string NOPInstrName;

public:
  // Ctor allowing to build a slot based on the TableGen Record
  TGTargetSlot(const Record *const SlotRecord)
      : InstanceName(SlotRecord->getName()),
        SlotName(SlotRecord->getValueAsString("SlotName")),
        Namespace(SlotRecord->getValueAsString("Namespace")),
        FieldToFind(SlotRecord->getValueAsString("FieldToFind")),
        // NumSlot is defined as -1 for now.
        // When the pool of slots will be finalized, then the final ID will be
        // attributed.
        NumSlot(-1), SlotSize(SlotRecord->getValueAsInt("SlotSize")),
        IsDefaultSlot(SlotRecord->getValueAsBit("isDefaultSlot")) {
    // We check here if the Slotname is valid, i.e. non empty.
    if (SlotName.empty()) {
      errs() << "Empty Slotname for Record " << InstanceName << "\n";
      exit(EXIT_FAILURE);
    }

    bool IsSizeValid = SlotSize > 0 || (SlotSize == 0 && IsDefaultSlot);
    if (!IsSizeValid) {
      errs() << "Invalid size of the slot " << InstanceName << "\n";
      exit(EXIT_FAILURE);
    }

    // From the SlotName, we derive the Enumeration string and the field to
    // find (if empty)
    if (FieldToFind.empty())
      FieldToFind = SlotName + "_base";

    if (!IsDefaultSlot) {
      EnumerationString = SlotName;
      // EnumerationString = capitalize(EnumerationString)
      // This is just cosmetic...
      std::transform(EnumerationString.begin(), EnumerationString.end(),
                     EnumerationString.begin(), ::toupper);

      EnumerationString = Namespace + "_SLOT_" + EnumerationString;
    } else {
      // a default slot has always the enumeration string "SLOT_UNKNOWN"
      EnumerationString = "SLOT_UNKNOWN";
    }
  }

  // This constructor builds a default slot
  TGTargetSlot(const std::string Namespace)
      : InstanceName("unknown_slot"), SlotName("unknown"), Namespace(Namespace),
        EnumerationString("SLOT_UNKNOWN"), FieldToFind(""), NumSlot(-1),
        SlotSize(0), IsDefaultSlot(true) {}

  const std::string &getInstanceName() const { return InstanceName; }

  const std::string &getSlotName() const { return SlotName; }

  const std::string &getNamespace() const { return Namespace; }

  int getNumSlot() const { return NumSlot; }

  const std::string &getLabelToFind() const { return FieldToFind; }

  unsigned getSlotSize() const { return static_cast<unsigned>(SlotSize); }

  const std::string &getEnumerationString() const { return EnumerationString; }
  bool isDefaultSlot() const { return IsDefaultSlot; }

private:
  void setNumSlot(int SlotID) { NumSlot = SlotID; }
  void setNopInst(const std::string &NOPName) { NOPInstrName = NOPName; }
};

/// This class intends to register all the detected slots, including the
/// default slot (unknown_slot) which may be explicit or implicit (if not
/// needed).
class TGTargetSlots {
public:
  using RecordSlot = std::pair<const Record *, TGTargetSlot>;
  using const_iterator = SmallVector<RecordSlot>::const_iterator;

  /// Name of the class/enum generated enumerating the different kinds of slot
  /// of the target
  const std::string GenSlotKindName = "SlotKind";
  /// Name of the class generated owning the informations (size...) of every
  /// slot of the target
  const std::string GenSlotInfoName = "SlotInfo";

private:
  /// Current Namespace
  std::string Target;
  /// Record representing the Base Slot class in TableGen
  const Record *const BaseSlotClass;
  /// List of pair <Record, Slot> parsed in TableGen
  SmallVector<RecordSlot> Slots;
  /// ID's of the Slots part of this instance
  /// The default slot will have -1 as ID
  /// Valid slots will have >= 0 IDs attributed
  static unsigned SlotID;
  /// Indicates if the Slots could be emitted or not
  bool IsFinalized;

public:
  TGTargetSlots(const std::string &TargetNS, const Record *const BaseClass)
      : BaseSlotClass(BaseClass), IsFinalized(false) {
    assert(BaseSlotClass && "null-pointer argument");
    Target = TargetNS;
  }

  /// Add the Slot (if legal) in the Slot pool
  bool addSlot(const Record *const R);

  /// This method performs a finalization of the slot pool. This finalization
  /// is composed of 2 stages: the attribution of a unique ID for each slot and
  /// then the sorting of the internal container based on their ID, in order to
  /// prepare the emission. It should be called only once all the detected
  /// slots have been added into the main Slots instance.
  void finalizeSlots();

  inline unsigned size() const { return Slots.size(); }
  inline bool isFinalized() const { return IsFinalized; }

  /// Find a SlotRecord by TableGen Record
  const_iterator find(const Record *const R) const {
    return std::find_if(Slots.begin(), Slots.end(),
                        [&R](const RecordSlot &RS) { return RS.first == R; });
  }

  /// Find a SlotRecord using "Label" as a SlotName
  const_iterator findBySlotName(const std::string &Label) const {
    return std::find_if(
        Slots.begin(), Slots.end(),
        [&Label](const RecordSlot &RS) { return RS.second.SlotName == Label; });
  }

  const_iterator cbegin() const { return Slots.begin(); }

  const_iterator cend() const { return Slots.end(); }

  const TGTargetSlots::RecordSlot *getDefaultSlot() const {
    // Find the default slot in the pool
    const_iterator DefaultSlotIt =
        std::find_if(Slots.begin(), Slots.end(), [](const RecordSlot &RS) {
          return RS.second.isDefaultSlot();
        });

    if (DefaultSlotIt == Slots.end())
      return nullptr;
    return DefaultSlotIt;
  }

  /// Emit the "SlotKind" enumeration for the target
  void emitTargetSlotKindEnum(raw_ostream &o) const {
    assert(IsFinalized &&
           "Internal vector needs to be finalized (i.e. sorted)");

    for (auto *SlotIt = Slots.begin(); SlotIt != Slots.end(); ++SlotIt) {
      const RecordSlot &Slot = *SlotIt;
      if (Slot.second.isDefaultSlot())
        continue;
      o << "  " << Slot.second.getEnumerationString() << ", \n";
    }
  }

  /// Emit the "SlotKind" class
  void emitTargetSlotKindClass(raw_ostream &o) const;

  /// Emit the correspondence code between the slot kinds and their slot infos
  void emitTargetSlotMapping(raw_ostream &o) const;

  /// Emit the "SlotInfo" class, specific to the "SlotKind"
  void emitTargetSlotClass(raw_ostream &o) const;

  /// Emit the series of Slot declarations
  void emitTargetSlotsDeclaration(raw_ostream &o) const;

  /// Emit the initialization code of all "SlotInfo" instances
  void emitSlotsInfoInstantiation(raw_ostream &o,
                                  const TGInstrLayout::NOPSlotMap &) const;
};

/// Main Class abstracting the concept of encoding fields
class TGFieldLayout {
  friend class TGInstrLayout;
  friend class TGFieldIterator;

public:
  using iterator = TGFieldIterator;
  using TGFieldLayoutPtr = std::shared_ptr<TGFieldLayout>;

  enum class FieldType { Variable, FixedBits };

private:
  const CodeGenInstruction *const CGI;
  /// Name of the TableGen Field or the value of a bitset
  std::string Label;
  /// Parent reference
  std::shared_ptr<TGFieldLayout> Parent;
  /// TableGen Record defining the current Field Layout
  const Record *DefRecord;
  /// Size (in bits) of the field
  unsigned Size;
  /// Type of the field
  FieldType Type;
  /// Offsets (left and right) of the field in the instruction encoding scope
  struct {
    int LeftOffset;
    int RightOffset;
  } GlobalOffsets;
  /// Vector containing all the sub-fields of the current field
  std::vector<std::shared_ptr<TGFieldLayout>> LayoutEntries;
  /// Operand Number of the future MI/MCInst linked to this field.
  int MIOpIdx = -1;
  /// Emission ID, which is set lately in the process when emitting the "flat"
  /// tree.
  unsigned EmissionID;
  /// Pointer on the Slot, matters if the field is linked to a slot
  const TGTargetSlot *SlotClass;

public:
  TGFieldLayout(const CodeGenInstruction *const CGI, const std::string &Label,
                std::shared_ptr<TGFieldLayout> const &Parent, unsigned Size,
                FieldType Type)
      : CGI(CGI), Label(Label), Parent(Parent), DefRecord(nullptr), Size(Size),
        Type(Type), GlobalOffsets{-1, -1}, MIOpIdx(-1), SlotClass(nullptr) {
    assert(CGI && CGI->TheDef &&
           "ill-formed reference on the CodeGenInstruction");
  }

  /// Retrieve the size of the current layout
  unsigned getSize() const { return Size; }

  const std::string &getLabel() const { return Label; }

  bool isSlot() const { return SlotClass != nullptr; }

  bool isFixedBits() const { return Type == FieldType::FixedBits; }

  const std::shared_ptr<TGFieldLayout> &getParent() const { return Parent; }

  bool isGlobalPositionKnown() const {
    // We check here that LeftOffset and RightOffset have both >= 0 value as
    // the initialization values are (-1, -1)
    // NOTE: GlobalOffsets = (0, 0) is a valid field layout of 1-bit
    return GlobalOffsets.LeftOffset >= 0 && GlobalOffsets.RightOffset >= 0;
  }

  /// Returns true whether the field is a leaf of the tree (i.e. doesn't have
  /// children).
  bool isLeaf() const { return LayoutEntries.size() == 0; }

  const TGTargetSlot *getSlot() const { return SlotClass; }

  /// Set the current Slot reference of the field
  void setSlot(const TGTargetSlot &Slot,
               const bool IsMultipleSlotOptions = false) {
    SlotClass = &Slot;
    if (Size != SlotClass->getSlotSize() && !IsMultipleSlotOptions) {
      errs() << "Size doesn't match between the field " << Label
             << " of the Record " << DefRecord->getName() << " and the slot "
             << Slot.getInstanceName() << "\n";
      exit(EXIT_FAILURE);
    }
  }

  void setMCOperandIndex(int Idx) { MIOpIdx = Idx; }

  int getMCOperandIndex() const { return MIOpIdx; }

  /// Compute the Offset of a child field, relative to the current node
  /// position.
  /// NOTE: If the current field layout is: XXX|XXXXXX|XXXXXXX|XXXX
  ///                                           child0 child1
  /// Then, it will return bit offset 3 for child0 and 9 for child1
  unsigned
  computeRelativeChildOffset(const TGFieldLayout *const ChildField) const {
    unsigned InternalOffset = 0;
    assert(ChildField && "Undefined reference on the child node");
    for (auto &LE : LayoutEntries) {
      // If we find the child, then stop searching
      if (LE.get() == ChildField)
        // Post-condition: InternalOffset contains the offset of the child
        return InternalOffset;
      InternalOffset += LE->getSize();
    }
    llvm_unreachable("Parameter ChildField is not a child node"
                     " of the current node");
  }

  /// Dump a string summing up the knowledge of the current field (Debug)
  void dump(std::ostream &Stream) const {
    Stream << Label << " (size: " << getSize() << "-bit) at pos ["
           << GlobalOffsets.LeftOffset << ", " << GlobalOffsets.RightOffset
           << "] ";
    Stream << ((isSlot()) ? "IsSlot: " + SlotClass->getInstanceName() + " "
                          : "");

    if (!isLeaf()) {
      Stream << "-> ";
      for (auto &ref : LayoutEntries) {
        ref->dump(Stream);
        if (&ref != &LayoutEntries.back())
          Stream << ", ";
      }
      Stream << "<- ";
    }
  }

  /// Add a field as a sub-field in the current Layout.
  /// Returns a reference on the newly added entry.
  const std::shared_ptr<TGFieldLayout> &
  addSubField(const std::shared_ptr<TGFieldLayout> &Field) {
    LayoutEntries.push_back(Field);
    return LayoutEntries.back();
  }

private:
  /// This central method builds recursively the fields Tree (beginning by a
  /// call on the "Inst" Attribute, in the Ctor of TGInstrLayout). This is
  /// done by resolving a particular field definition (whose name is
  /// "LabelToFind") in the TableGen Hierarchy of the BaseRecord, started at
  /// level "HierarchyLevel".
  ///
  ///       class Instruction;
  ///       class TARGET##Inst <> : Instruction {
  ///         ...
  ///       }
  ///       class Class_0 <> : TARGET##Inst {    --- HierarchyLevel
  ///         bits<n> Inst = {myfield0, 0b0};
  ///       }
  ///       class Class_1 <> {
  ///         bits<m> myfield0 = {myfield1, 0b1011};
  ///       }
  ///       ...
  ///       def BaseRecord <> : Class_n <>;
  ///
  /// If we find it, then search recursively a definition of itself if needed.
  /// Otherwise, do nothing. In the example above, the method will search first
  /// for the definition of Inst, then for the definition of myfield0, then for
  /// myfield1...
  ///
  /// We need to perform a traversal of the TableGen hierarchy classes (of a
  /// given Record - instruction) by "hand" as the hierarchical representation
  /// of the instruction formats aren't registered in the Record. More
  /// specifically, in a TableGen Record, each field owns already its fixed-
  /// bits assignment (when there is one), i.e.
  ///
  ///       def Record_0 : Class_0 <> {
  ///         bits<32> instr32   = {0, 1, 1, ..., 0};
  ///         bits<2>  subfield0 = {0, 1};
  ///         bit      subfield1 = {1};
  ///         ...
  ///       }
  ///
  /// whereas in the TableGen classes, this resolution hasn't been performed
  /// and we still have the "dependencies" between a given field and its sub-
  /// components i.e.
  ///
  ///       class Class_0 <> {
  ///         bits<32> instr32 = { subfield0, subfield1, ... };
  ///         bits<2> subfield0;
  ///         bit     subfield1;
  ///         ...
  ///       }
  ///
  /// Thus, by iterating over each super-class, it's possible to "trace" the
  /// field derivation leading to a particular instruction format.
  ///
  /// NOTE: the hierarchy begins on the older parent (the one at the top). So
  /// when we speak to reach an upper level, in reality we're talking about
  /// descending the hierarchy, i.e. going from a parent node to a child node.
  void resolveFieldsDefInHierarchy(const std::string &LabelToFind,
                                   unsigned HierarchyLevel,
                                   const TGFieldLayoutPtr &BaseField);

  void resolveFieldsDefInBaseRecord(const std::string &LabelToFind,
                                    const TGFieldLayoutPtr &BaseField);
  // Bottom-up Recursive Resolution of GlobalOffsets:
  // The Recursive traversal begins on the leaves of the tree and ascend to the
  // upper level.
  void resolveFieldsGlobalOffsets();
};

/// This class introduces an iterator on the fields of an Instruction Layout.
/// It implements an iterative version of the depth-first search tree algorithm
/// (which visits each field of the tree in a "pre-order" way).
///
/// However, a filtering of the visited nodes could be done by using a
/// (functor) policy/predicate.
/// NOTE: The base predicate/policy returns always true. Thus in this
/// configuration, the iterator returns every field of the hierarchy.
///
/// Besides, 2 modes of Traversal are available:
/// - FullTraversal: every nodes of the tree will be visited and submitted
///   to an evaluation of the predicate, no matter the evaluation of the
///   current predicate returns us ;
/// - StopTraversal: if the predicate returns true on field A, then the
///   subtree, i.e. the subfields of A, will not be visited.
/// The default mode is FullTraversal.
/// NOTE: We can also implement a base abstract class defining the interface
/// of this iterator and then make it derives from it (could be nice if we need
/// to implement a BFS version of this iterator).
class TGFieldIterator
    : public std::iterator<std::forward_iterator_tag, TGFieldIterator> {
public:
  /// Traversal Modes
  enum class Mode { FullTraversal, StopTraversal };

private:
  using reverse_iterator =
      std::vector<std::shared_ptr<TGFieldLayout>>::reverse_iterator;

  /// Stack of pointers used by the depth-first search tree algorithm
  std::stack<TGFieldLayout *> InternalStack;

  /// Current value of the iterator
  TGFieldLayout *FieldCurrent;

  /// Traversal Mode of the iterator
  Mode TraversalMode;

  /// The functor policy can be used to filter the visited nodes by writing a
  /// custom predicate in the functor body
  std::function<bool(const TGFieldLayout &)> FunctorPolicy;

public:
  TGFieldIterator(
      TGFieldLayout *InitField, Mode Mode = Mode::FullTraversal,
      const std::function<bool(const TGFieldLayout &)>
          // BasePolicy: visits every node of the tree
          &BasePolicy = [](const TGFieldLayout &) { return true; });

  TGFieldIterator &operator++();
  TGFieldLayout *operator*() const;
  bool operator==(const TGFieldIterator &Other) const;
  bool operator!=(const TGFieldIterator &Other) const {
    return !(*this == Other);
  }

private:
  /// Performs an iteration of the depth-search tree algorithm.
  /// Returns whether we should continue or not.
  inline bool iterateOverFields();
};

} // end namespace llvm

#endif
