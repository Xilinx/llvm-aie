//===-- AIERegUnitUtils.cpp -----------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIERegUnitUtils.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

namespace llvm::AIERegUnitUtils {

void addRegUnits(const TargetRegisterInfo &TRI, MCRegister Phys,
                 BitVector &Out) {
  for (MCRegUnit RU : TRI.regunits(Phys))
    Out.set(RU);
}

BitVector computeCalleeSavedRegSet(const TargetRegisterInfo &TRI,
                                   const MachineRegisterInfo &MRI) {
  BitVector CSRRegs(TRI.getNumRegs());
  for (const MCPhysReg *CSR = MRI.getCalleeSavedRegs(); CSR && *CSR; ++CSR)
    CSRRegs.set(*CSR);
  return CSRRegs;
}

} // namespace llvm::AIERegUnitUtils
