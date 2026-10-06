//===-- AIERegAllocationUtils.h - AIE register allocation utils -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares utilities shared by AIE register-allocation rewrites.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIEREGALLOCATIONUTILS_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIEREGALLOCATIONUTILS_H

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/MC/MCRegister.h"

namespace llvm {

class LiveInterval;
class LiveRegMatrix;
class TargetRegisterClass;
class TargetRegisterInfo;

namespace AIERegAllocationUtils {

/// Searches \p AllocationOrder for an \p RC register free for \p LI without a
/// \p ReservedRegUnits unit, or returns NoRegister when none is available.
MCPhysReg findFreeNonOverlappingPhysReg(const LiveInterval &LI,
                                        const TargetRegisterClass &RC,
                                        ArrayRef<MCPhysReg> AllocationOrder,
                                        const BitVector &ReservedRegUnits,
                                        const TargetRegisterInfo &TRI,
                                        LiveRegMatrix &LRM);

/// Adds every register unit of \p PhysReg to \p ReservedRegUnits.
void reserveRegUnits(MCPhysReg PhysReg, const TargetRegisterInfo &TRI,
                     BitVector &ReservedRegUnits);

} // namespace AIERegAllocationUtils
} // namespace llvm

#endif
