//===-- AIERegUnitUtils.h -------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Shared reg-unit and callee-saved-register bookkeeping used by passes that
// track physreg occupancy outside of LiveRegMatrix (e.g. AIEWARBreaker,
// AIEWawRegRewriter).
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIEREGUNITUTILS_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIEREGUNITUTILS_H

#include "llvm/ADT/BitVector.h"
#include "llvm/MC/MCRegister.h"

namespace llvm {
class MachineRegisterInfo;
class TargetRegisterInfo;

namespace AIERegUnitUtils {

/// Adds \p Phys's reg-units to \p Out (sized TRI.getNumRegUnits()); callers
/// resolve vregs via VRM first, unlike llvm::LiveRegUnits::accumulate.
void addRegUnits(const TargetRegisterInfo &TRI, MCRegister Phys,
                 BitVector &Out);

/// Return a BitVector sized TRI.getNumRegs(), with every callee-saved
/// register of MRI's calling convention set.
BitVector computeCalleeSavedRegSet(const TargetRegisterInfo &TRI,
                                   const MachineRegisterInfo &MRI);

} // namespace AIERegUnitUtils
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_UTILS_AIEREGUNITUTILS_H
