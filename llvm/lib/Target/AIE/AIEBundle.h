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
#include "llvm/CodeGen/TargetOpcodes.h"

#include <unordered_map>

namespace llvm {
class MachineInstr;
namespace AIE {

/// \a I needs to provide getOpcode().
template <class I> class Bundle {
public:
  /// Construct a bundle
  /// \param FormatInterface The architecture's interface to the formats.
  Bundle(const AIEBaseMCFormats *FormatInterface)
      : FormatInterface(FormatInterface) {}

  Bundle(const std::initializer_list<I *> &Instrs,
         const AIEBaseMCFormats *FormatInterface)
      : FormatInterface(FormatInterface) {
    bool ComputeSlots = (FormatInterface != nullptr);
    for (I *Instr : Instrs) {
      add(Instr, Instr->getOpcode(), ComputeSlots);
    }
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

    if (isNoHazardMetaInstruction(InstOpCode)) {
      return true;
    }

    if (InstOpCode == TargetOpcode::BUNDLE) {
      return !BundleRoot;
    }

    // if we have a standalone bundle, we can't add anything.
    if (isStandalone())
      return false;

    // Instructions with no format/slot cannot be added in a non-empty Bundle.
    if (!FormatInterface->isSupportedInstruction(InstOpCode))
      return false;

    // Veryfy there is a format that can accomodate the new slots
    MCSlotKind Slot = FormatInterface->getSlotKind(InstOpCode);
    assert(Slot != MCSlotKind());
    SlotBits NewSlots = FormatInterface->getSlotInfo(Slot)->getSlotSet();
    return (OccupiedSlots & NewSlots) == 0 &&
           FormatInterface->getPacketFormats().getFormat(OccupiedSlots |
                                                         NewSlots);
  }

  /// Add an instruction to the bundle
  /// \param Instr Instruction to add
  /// \pre canAdd(Instr);
  void add(I *Instr, unsigned OpCode, bool ComputeSlots = true) {
    if (isNoHazardMetaInstruction(Instr->getOpcode())) {
      MetaInstrs.push_back(Instr);
      return;
    }

    // Keep track of BUNDLE instructions. They need to be cleaned up when
    // de-bundling before re-bundling. See applyBundles()
    if (Instr->getOpcode() == TargetOpcode::BUNDLE) {
      assert(!BundleRoot &&
             "AIE::Bundle already has a root BUNDLE instruction");
      BundleRoot = Instr;
      return;
    }

    // Check if the pre-condition is ensured
    assert((!ComputeSlots || !isStandalone()) &&
           "Tried to add an instruction in a standalone Bundle");

    Instrs.push_back(Instr);

    if (!ComputeSlots)
      return;

    MCSlotKind FinalSlot = FormatInterface->getSlotKind(OpCode);
    if (FinalSlot == MCSlotKind()) {
      assert(Instrs.size() == 1 &&
             "Tried to add an unknown slot instruction in a valid Bundle");
      return;
    }

    SlotBits NewSlots = FormatInterface->getSlotInfo(FinalSlot)->getSlotSet();
    assert(!(OccupiedSlots & NewSlots) && "Selected slot already occupied");
    SlotMap[FinalSlot] = Instr;
    OccupiedSlots |= NewSlots;
  }

  void add(I *Instr) { add(Instr, Instr->getOpcode()); }

  /// return the minimum size valid format for this bundle, if any
  const VLIWFormat *getFormatOrNull(unsigned Size = 0) const {
    assert(!isStandalone());
    if (Size)
      return FormatInterface->getPacketFormats().getFormatBySize(OccupiedSlots,
                                                                 Size);
    return FormatInterface->getPacketFormats().getFormat(OccupiedSlots);
  }

  /// Empty the bundle
  void clear() {
    OccupiedSlots = 0;
    Instrs.clear();
    MetaInstrs.clear();
    SlotMap.clear();
    BundleRoot = nullptr;
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
    return Instrs.size() == 1 && !FormatInterface->isSupportedInstruction(
                                     Instrs.front()->getOpcode());
  }

  /// Whether the opcode represents a meta instruction that can be ignored for
  /// format hazards.
  static bool isNoHazardMetaInstruction(unsigned Opcode) {
    switch (Opcode) {
    case TargetOpcode::IMPLICIT_DEF:
    case TargetOpcode::KILL:
      return true;
    }
    return false;
  }

  /// Erase the BUNDLE root instruction from its parent MBB.
  /// This does not remove the instructions within the BUNDLE, only the root.
  void eraseRootFromBlock() {
    if (BundleRoot) {
      BundleRoot->eraseFromBundle();
      BundleRoot = nullptr;
    }
  }

  bool isNOPBundle() const {
    const VLIWFormat *Format = getFormatOrNull();
    assert(Format);
    for (MCSlotKind Slot : Format->getSlots()) {
      const MCSlotInfo *SlotInfo = FormatInterface->getSlotInfo(Slot);
      assert(SlotInfo);

      llvm::MachineInstr *Instr = at(Slot);
      if (Instr->getOpcode() != SlotInfo->getNOPOpcode())
        return false;
    }
    return true;
  }
  void clearBundle() const {
    const VLIWFormat *Format = getFormatOrNull();
    assert(Format);
    for (MCSlotKind Slot : Format->getSlots()) {
      llvm::MachineInstr *Instr = at(Slot);
      Instr->removeFromBundle();
    }
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

private:
  /// A root BUNDLE instruction if it exists.
  I *BundleRoot = nullptr;
};

template <class I> bool operator==(const Bundle<I> &B1, const Bundle<I> &B2) {
  return std::tie(B1.Instrs, B1.SlotMap, B1.MetaInstrs) ==
         std::tie(B2.Instrs, B2.SlotMap, B2.MetaInstrs);
}

using MCBundle = Bundle<MCInst>;
using MachineBundle = Bundle<MachineInstr>;

} // namespace AIE
} // namespace llvm
#endif // LLVM_LIB_TARGET_AIE_BUNDLE_H
