//===-- AIEFormats.h  Format utilities for AIE -----------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Internal bundle representation with format checking
// The model of a bundle is that each instruction has an associated slot
// and that particular combinations of slots (formats) are valid.
// A slot can be occupied exactly once.
// Every unoccupied slot can always be filled with a nop instruction.
// A bundle holds the machine instructions it contains in a number of
// partially redundant representations:
// - The instructions in the original issue order
// - A map indexed by slot number
// - A bitset representing the occupied slots.
// A bundle is valid if a format can be found that can encode the slots.
// For any valid bundle, a valid format can be queried. This is a data struct
// containing the opcode and the syntactical order of the slots.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_BUNDLE_H
#define LLVM_LIB_TARGET_AIE_BUNDLE_H

#include "AIEBaseSubtarget.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm-c/DebugInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/TargetOpcodes.h"

#include <unordered_map>

namespace llvm {
class MachineInstr;
namespace AIE {

/// \a I needs to provide getOpcode().
template <class I> class Bundle {
private:
  /// Maps a "multi-slot" instruction to its assigned slot
  std::unordered_map<const I *, MCSlotKind> SelectedSlot;

  /// Finds an unoccupied slot that can accommodate the instruction.
  /// This interprets multislot pseudos by trying all alternatives
  std::optional<MCSlotKind> findFreeSlot(unsigned int InstOpCode) const {
    std::vector<MCSlotKind> SupportedSlots =
        FormatInterface->getSlotAlternatives(InstOpCode);
    // Iterate on possible slots. Check if the slot free and forms a valid
    // format
    // TODO : Future scope today we return true for the first valid format,
    // which in turn gives priority based on list mentioned while defining
    // Multi-Slot Pseudo. This could lead to unoptimized bundles. We should look
    // ahead and select the slot accordingly. For Ex. PADD followed by VLADA, in
    // such case PADD should be mapped to slot B.
    //
    // One way would be to dynamically keep track of the slots each
    // sub-instruction can be materialized into.
    // Bundle { PADD(A/B/S) }
    //   --> canAdd(VST) = true
    //   --> add(VST) becomes:
    // Bundle { PADD(A/B), VST }
    //   --> canAdd(LDB) = true
    //   --> add(LDB) becomes:
    // Bundle { PADD(A), VST, LDB }
    for (const auto &Kind : SupportedSlots) {
      SlotBits NewSlots = FormatInterface->getSlotInfo(Kind)->getSlotSet();
      if (OccupiedSlots & NewSlots) {
        continue;
      }
      // Now search the table for a matching format
      if (FormatInterface->getPacketFormats().getFormat(
              SlotBits(OccupiedSlots | NewSlots)))
        return Kind;
    }
    return {};
  }

public:
  /// Construct a bundle
  /// \param FormatInterface The architecture's interface to the formats.
  Bundle(const AIEBaseMCFormats *FormatInterface)
      : FormatInterface(FormatInterface) {}

  void assignInstructionToSlot(const I *Instr, MCSlotKind Slot) {
    assert((!SelectedSlot.count(Instr) || SelectedSlot[Instr] == Slot) &&
           "Slot for Multi-Slot instr is already selected");
    SelectedSlot[Instr] = Slot;
  }

  template <class II> bool isPostRA(II *) const { return true; }

  bool isPostRA(MachineInstr *Instr) const {
    const MachineFunction *MF = Instr->getParent()->getParent();
    return MF->getProperties().hasProperty(
        MachineFunctionProperties::Property::NoVRegs);
  }

  /// Returns whether adding Instr to the current bundle leaves it valid.
  /// \param Instr instruction to add.
  bool canAdd(I *Instr) const { return canAdd(Instr->getOpcode()); }

  /// This is an overload taking an OpCode. This is the actual implementation
  /// of the wrapper above which takes a MachineInstr and traverses from the MI
  /// to Opcode
  bool canAdd(unsigned int InstOpCode) const {
    // (Temporary) shortcut for the hazard recognizer:
    // We know that each instruction can be coded as a standalone instruction,
    // even if we have no explicit format listed for it.
    // This allows to generate sequential code without the need for a
    // complete format list. (If we can't find a format we will keep
    // inserting nops infinitely, or at least until memory runs out)
    // A non-empty bundle can escape this situation by advancing the cycle,
    // rendering it empty.
    if (empty()) {
      return true;
    }

    if (isMetaInstruction(InstOpCode)) {
      return true;
    }

    // if we have a standalone bundle, we can't add anything.
    if (isStandalone())
      return false;

    auto Found = findFreeSlot(InstOpCode);
    return Found.has_value();
  }

  /// Add an instruction to the bundle
  /// \param Instr Instruction to add
  /// \pre canAdd(Instr);
  void add(I *Instr) {
    if (isMetaInstruction(Instr->getOpcode())) {
      MetaInstrs.push_back(Instr);
      return;
    }
    // Check if the pre-condition is ensured
    assert(!isStandalone() &&
           "Tried to add an instruction in a standalone Bundle");

    std::vector<MCSlotKind> SupportedSlots =
        FormatInterface->getSlotAlternatives(Instr->getOpcode());
    Instrs.push_back(Instr);

    if (SupportedSlots.empty()) {
      // The bundle must contain only the newly added unknown instruction,
      // otherwise it contradicts the pre-condition.
      assert(Instrs.size() == 1 &&
             "Tried to add an unknown slot instruction in a valid Bundle");
      return;
    }

    MCSlotKind FinalSlot;
    if (SupportedSlots.size() > 1) {
      assert(SelectedSlot.count(Instr) &&
             "Expected a slot for Multi-Slot Instr");
      FinalSlot = SelectedSlot[Instr];
    } else {
      FinalSlot = SupportedSlots.front();
    }

    SlotBits NewSlots = FormatInterface->getSlotInfo(FinalSlot)->getSlotSet();
    assert(!(OccupiedSlots & NewSlots) && "Selected slot already occupied");
    SlotMap[FinalSlot] = Instr;
    OccupiedSlots |= NewSlots;

    SelectedSlot.clear();
  }

  /// Expand multi-slot pseudo instruction in the Bundle.
  /// This is expected to be called after the final scheduling.
  void materializePseudos() {
    for (auto &[Slot, MI] : SlotMap) {
      if (const auto Opcode = FormatInterface->getMaterializableOpcodeForSlot(
              MI->getOpcode(), Slot)) {
        MachineFunction *MF = MI->getParent()->getParent();
        const auto *TII = static_cast<const AIEBaseInstrInfo *>(
            MF->getSubtarget().getInstrInfo());
        MI->setDesc(TII->get(Opcode.value()));
      }
    }
  }

  /// return the minimum size valid format for this bundle, if any
  const VLIWFormat *getFormatOrNull() const {
    assert(!isStandalone());
    return FormatInterface->getPacketFormats().getFormat(OccupiedSlots);
  }

  /// Empty the bundle
  void clear() {
    OccupiedSlots = 0;
    Instrs.clear();
    MetaInstrs.clear();
    SlotMap.clear();
    SelectedSlot.clear();
  }

  /// Check if empty
  bool empty() const { return Instrs.empty(); }

  /// return the number of occupied slots;
  unsigned size() const { return Instrs.size(); }

  /// Return instruction occupied by slot or nullptr
  I *at(MCSlotKind Slot) const {
    auto Found = SlotMap.find(Slot);
    if (Found == SlotMap.end()) {
      return nullptr;
    }
    return Found->second;
  }

  /// Find the slot a given Instruction was assigned to.
  /// Returns nullopt if \p Instr isn't in the Bundle.
  std::optional<MCSlotKind> findSlotForInstr(I *Instr) const {
    auto It = find_if(SlotMap,
                      [Instr](const std::pair<MCSlotKind, I *> &SlotAndInstr) {
                        return Instr == SlotAndInstr.second;
                      });
    return It != SlotMap.end() ? std::optional<MCSlotKind>(It->first)
                               : std::nullopt;
  }

  /// Read only access to the contained instructions
  const std::unordered_map<MCSlotKind, I *, MCSlotKind::Hasher> &
  getSlotMap() const {
    return SlotMap;
  }
  /// Read only access to the contained instructions
  const std::vector<I *> &getInstrs() const { return Instrs; }

  /// Read only access to the contained meta instructions
  const std::vector<I *> &getMetaInstrs() const { return MetaInstrs; }

  /// Return whether the Bundle must stay standalone.
  /// It is the case when (non-AIE or AIE) instructions owning an unknown slot
  /// are encountered. This is also the case for TargetOpcode specific
  /// instruction like KILL.
  /// NOTE: if this flag is set, then OccupiedSlots and SlotMap have no
  /// meaning.
  bool isStandalone() const {
    return Instrs.size() == 1 &&
           ((FormatInterface->getSlotAlternatives(Instrs.front()->getOpcode()))
                .empty());
  }

  bool isMetaInstruction(unsigned Opcode) const {
    switch (Opcode) {
    case TargetOpcode::IMPLICIT_DEF:
    case TargetOpcode::KILL:
      return true;
    }
    return false;
  }

  // The subtarget, holding bundle-related data like formats
  const AIEBaseMCFormats *FormatInterface;

  // Occupied slots. The population count in this bitset should always
  // match the size of Instrs;
  SlotBits OccupiedSlots = 0;

  // Contained instructions, in insertion order
  std::vector<I *> Instrs;

  // Contained instructions, mapped by slot
  std::unordered_map<MCSlotKind, I *, MCSlotKind::Hasher> SlotMap;

  // Contained meta instructions (These will end up after the bundle)
  std::vector<I *> MetaInstrs;
};

using MCBundle = Bundle<MCInst>;
using MachineBundle = Bundle<MachineInstr>;

} // namespace AIE
} // namespace llvm
#endif // LLVM_LIB_TARGET_AIE_BUNDLE_H
