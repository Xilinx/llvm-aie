//===- AIEBaseCombinedOpcodes.h ----------------------------------*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This is the AIE combined opcodes base class.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_COMBINEDOPCODES_H
#define LLVM_LIB_TARGET_AIE_COMBINEDOPCODES_H

#include "llvm/ADT/APInt.h"
#include "llvm/CodeGen/MachineInstr.h"
#include <optional>

namespace llvm {

class AIEBaseCombinedOpcodes {
public:
  /// Hold the information needed to select instructions that deal with multiple
  /// addressing modes and might need to be split
  struct LoadStoreOpcodes {
    // The main Opcode with the correct addressing mode
    unsigned ISelOpcode;
    // Indicates whether the provided immediate offset fits in the immedate
    // range of the selected instruction
    bool FitsImmediateRange;
    // Opcode with an immediate indexed addressing mode to split larger loads
    // and stores into smaller ones E.g. Instruction selection for $x0, $p0 =
    // G_AIE_POSTINC_LOAD $p0, #128: $wh0 = VLDA_dmw_lda_w_ag_idx_imm $p0, #32
    // $wl0, $p0 = VLDA_dmw_lda_w_ag_pstm_nrm_imm $p0, #128
    std::optional<unsigned> OffsetOpcode;
  };

  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodeCONV(const MachineInstr &MemOp, const MachineInstr &CombOp,
                        std::optional<APInt> Immediate) const {
    llvm_unreachable("Target didn't implement getCombinedOpcodeCONV");
  }

  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodeCONVLoad(const MachineInstr &MemOp,
                            const MachineInstr &CombOp,
                            const std::optional<APInt> Immediate) const {
    llvm_unreachable("Target didn't implement getCombinedOpcodeCONVLoad");
  }

  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodePACK(const MachineInstr &MemOp, const MachineInstr &CombOp,
                        std::optional<APInt> Immediate, bool IsSigned,
                        bool Is32Lanes) const {
    llvm_unreachable("Target didn't implement getCombinedOpcodePACK");
  }

  virtual std::optional<LoadStoreOpcodes> getCombinedOpcodeUNPACKLoad(
      const MachineInstr &MemOp, const MachineInstr &CombOp,
      std::optional<APInt> Immediate, MachineRegisterInfo &MRI) const {
    llvm_unreachable("Target didn't implement getCombinedOpcodeUNPACKLoad");
  }

  virtual std::optional<LoadStoreOpcodes>
  getCombinedOpcodeSRSUPS(const MachineInstr &MemOp, const MachineInstr &CombOp,
                          std::optional<APInt> Immediate, bool IsSigned) const {
    llvm_unreachable("Target didn't implement getCombinedOpcodeSRSUPS");
  }

  bool canCombineCONV(MachineInstr &MemOp, MachineInstr &CombOp) const {
    const std::optional<APInt> NoImmediate = {};
    return getCombinedOpcodeCONV(MemOp, CombOp, NoImmediate).has_value();
  }

  bool canCombineCONVLoad(MachineInstr &MemOp, MachineInstr &CombOp) const {
    const std::optional<APInt> NoImmediate = {};
    return getCombinedOpcodeCONVLoad(MemOp, CombOp, NoImmediate).has_value();
  }

  bool canCombineUNPACKLoad(MachineInstr &MemOp, MachineInstr &CombOp,
                            MachineRegisterInfo &MRI) const {
    const std::optional<APInt> NoImmediate = {};
    return getCombinedOpcodeUNPACKLoad(MemOp, CombOp, NoImmediate, MRI)
        .has_value();
  }
  bool canCombineSRSUPS(MachineInstr &MemOp, MachineInstr &CombOp) const {

    const std::optional<APInt> NoImmediate = {};
    const bool IsSigned = true;

    return getCombinedOpcodeSRSUPS(MemOp, CombOp, NoImmediate, IsSigned)
        .has_value();
  }

  bool canCombinePACK(MachineInstr &MemOp, MachineInstr &CombOp) const {

    std::optional<APInt> NoImmediate = {};
    bool IsSigned = true;
    bool Is32Lanes = true;

    return getCombinedOpcodePACK(MemOp, CombOp, NoImmediate, IsSigned,
                                 Is32Lanes)
        .has_value();
  }

  virtual ~AIEBaseCombinedOpcodes() = default;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_COMBINEDOPCODES_H
