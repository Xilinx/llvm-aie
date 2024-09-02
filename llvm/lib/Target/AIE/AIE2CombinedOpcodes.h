//===- AIE2CombinedOpcodes.h -------------------------------------*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This is the AIE2 combined opcodes class declaration.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2_COMBINEDOPCODES_H
#define LLVM_LIB_TARGET_AIE2_COMBINEDOPCODES_H

#include "AIEBaseCombinedOpcodes.h"

namespace llvm {

using LoadStoreOpcodes = AIEBaseCombinedOpcodes::LoadStoreOpcodes;

class AIE2CombinedOpcodes : public AIEBaseCombinedOpcodes {

  std::optional<LoadStoreOpcodes>
  getCombinedOpcodeCONV(const MachineInstr &MemOp, const MachineInstr &CombOp,
                        std::optional<APInt> Immediate) const override;

  std::optional<LoadStoreOpcodes> getCombinedOpcodeCONVLoad(
      const MachineInstr &MemOp, const MachineInstr &CombOp,
      const std::optional<APInt> Immediate) const override;

  std::optional<LoadStoreOpcodes>
  getCombinedOpcodePACK(const MachineInstr &MemOp, const MachineInstr &CombOp,
                        std::optional<APInt> Immediate, bool IsSigned,
                        bool Is32Lanes) const override;

  std::optional<LoadStoreOpcodes> getCombinedOpcodeUNPACKLoad(
      const MachineInstr &MemOp, const MachineInstr &CombOp,
      std::optional<APInt> Immediate, MachineRegisterInfo &MRI) const override;

  std::optional<LoadStoreOpcodes>
  getCombinedOpcodeSRSUPS(const MachineInstr &MemOp, const MachineInstr &CombOp,
                          std::optional<APInt> Immediate,
                          bool IsSigned) const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE2_COMBINEDOPCODES_H
