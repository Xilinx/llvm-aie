//===-- AIESuperRegUtils.h ------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains helper functions to work with 2D/3D composite registers.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_AIESUPERREGUTILS_H
#define LLVM_LIB_TARGET_AIE_AIESUPERREGUTILS_H

#include "llvm/ADT/SmallSet.h"

namespace llvm {
class Register;
class MachineRegisterInfo;
struct AIEBaseRegisterInfo;
class MachineInstr;
class LiveIntervals;
class TargetInstrInfo;
class TargetRegisterInfo;
struct LaneBitmask;
class SlotIndex;
class SlotIndexes;
class VirtRegMap;
class LiveRegMatrix;
class LiveDebugVariables;
} // namespace llvm

namespace llvm::AIESuperRegUtils {

/// Determines if a composite register can be safely decomposed into its
/// subregisters by analyzing all uses. A register is rewritable if all uses
/// either access specific subregisters or are full copies where both operands
/// are also rewritable. Returns the set of subregister indices that can be
/// used for rewriting, or an empty set if decomposition is not possible.
/// Physical registers and registers without subregister splits cannot be
/// rewritten.
///
/// Returns the subreg indices that can be used to rewrite \p Reg into smaller
/// regs. Returns {} if the rewrite isn't possible.
SmallSet<int, 8> getRewritableSubRegs(Register Reg,
                                      const MachineRegisterInfo &MRI,
                                      const AIEBaseRegisterInfo &TRI,
                                      std::set<Register> &VisitedVRegs);

SmallSet<int, 8> getRewritableSubRegs(Register Reg,
                                      const MachineRegisterInfo &MRI,
                                      const AIEBaseRegisterInfo &TRI);

/// Rewrite a full copy into multiple copies using the subregs in \p CopySubRegs
void rewriteFullCopy(MachineInstr &MI, const std::set<int> &CopySubRegs,
                     LiveIntervals &LIS, const TargetInstrInfo &TII,
                     const TargetRegisterInfo &TRI, VirtRegMap &VRM,
                     LiveRegMatrix &LRM);

/// Return a mask of all the lanes that are live at \p Index
LaneBitmask getLiveLanesAt(SlotIndex Index, Register Reg,
                           const LiveIntervals &LIS);

void rewriteSuperReg(Register Reg, Register AssignedPhysReg,
                     SmallSet<int, 8> &SubRegs, MachineRegisterInfo &MRI,
                     const AIEBaseRegisterInfo &TRI, VirtRegMap &VRM,
                     LiveRegMatrix &LRM, LiveIntervals &LIS,
                     SlotIndexes &Indexes, LiveDebugVariables &DebugVars);

bool isRegUsedBy2DOr3DInstruction(const MachineRegisterInfo &MRI,
                                  const Register &R);

void repairLiveIntervals(SmallSet<Register, 8> &RegistersToRepair,
                         VirtRegMap &VRM, LiveRegMatrix &LRM,
                         LiveIntervals &LIS);

} // namespace llvm::AIESuperRegUtils

#endif
