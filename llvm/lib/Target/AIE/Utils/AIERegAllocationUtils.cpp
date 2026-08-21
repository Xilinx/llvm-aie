//===-- AIERegAllocationUtils.cpp - AIE register allocation utils --------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements utilities shared by AIE register-allocation rewrites.
//
//===----------------------------------------------------------------------===//

#include "AIERegAllocationUtils.h"
#include "llvm/CodeGen/LiveInterval.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

using namespace llvm;

MCPhysReg AIERegAllocationUtils::findFreeNonOverlappingPhysReg(
    const LiveInterval &LI, const TargetRegisterClass &RC,
    ArrayRef<MCPhysReg> AllocationOrder, const BitVector &ReservedRegUnits,
    const TargetRegisterInfo &TRI, LiveRegMatrix &LRM) {
  for (MCPhysReg PhysReg : AllocationOrder) {
    if (!RC.contains(PhysReg))
      continue;

    if (llvm::any_of(TRI.regunits(PhysReg), [&](MCRegUnit Unit) {
          return !ReservedRegUnits.empty() && ReservedRegUnits.test(Unit);
        }))
      continue;

    if (LRM.checkInterference(LI, PhysReg) == LiveRegMatrix::IK_Free)
      return PhysReg;
  }

  return MCRegister::NoRegister;
}

void AIERegAllocationUtils::reserveRegUnits(MCPhysReg PhysReg,
                                            const TargetRegisterInfo &TRI,
                                            BitVector &ReservedRegUnits) {
  for (MCRegUnit Unit : TRI.regunits(PhysReg))
    ReservedRegUnits.set(Unit);
}
