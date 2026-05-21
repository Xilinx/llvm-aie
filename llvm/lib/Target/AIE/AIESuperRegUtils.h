//===-- AIESuperRegUtils.h ------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains helper functions to work with 2D/3D composite registers.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_AIESUPERREGUTILS_H
#define LLVM_LIB_TARGET_AIE_AIESUPERREGUTILS_H

#include "llvm/ADT/SmallSet.h"
#include <optional>

namespace llvm {
class Register;
class MachineFunction;
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

/// Checks if a register is expandable (has a subregister split).
/// This is a lightweight check that only verifies if the register has
/// potential to be split into subregisters, without analyzing uses.
/// Returns true if the register is virtual and has a non-trivial SubRegSplit.
bool isExpandableRegister(Register Reg, const MachineRegisterInfo &MRI,
                          const AIEBaseRegisterInfo &TRI);

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

void rewriteSuperReg(Register Reg, std::optional<Register> AssignedPhysReg,
                     SmallSet<int, 8> &SubRegs, MachineRegisterInfo &MRI,
                     const AIEBaseRegisterInfo &TRI, VirtRegMap &VRM,
                     LiveRegMatrix &LRM, LiveIntervals &LIS,
                     SlotIndexes &Indexes, LiveDebugVariables &DebugVars);

bool isRegUsedBy2DOr3DInstruction(const MachineRegisterInfo &MRI,
                                  const Register &R);

void repairLiveIntervals(SmallSet<Register, 8> &RegistersToRepair,
                         VirtRegMap &VRM, LiveRegMatrix &LRM,
                         LiveIntervals &LIS);

/// Sever VRM split-from chain for descendants of \p TaintedOriginals so that
/// SplitKit::defFromParent consults the descendant's own (repaired) LI, not
/// the stale ancestor LI which may still hold VNs at slots whose MIs were
/// rewritten/unbundled by an AIE register-rewriter pass. Each affected
/// descendant is restored via VRM.clearSplitFromReg() to the canonical
/// "no split parent" state of a freshly created vreg.
///
///   before:                    after:
///   %0 (stale LI)              %0 (stale LI, ignored)
///    | split-from               x  (chain cut)
///   %35 ----.                  %35 (no split parent)
///    | split-from
///   %141..%144 (future split   greedy splits will use %35's LI
///   would consult %0's LI)     instead of %0's
///
/// Before severing, the pre-severance Original is recorded in
/// AIEMachineFunctionInfo's spill-group side map so that InlineSpiller (via
/// TargetSubtargetInfo::getSpillGroupOriginal) can still merge sibling spills
/// of these descendants onto a shared stack slot.
void clearStaleSplitFromMappings(const SmallSet<Register, 8> &TaintedOriginals,
                                 MachineFunction &MF, MachineRegisterInfo &MRI,
                                 VirtRegMap &VRM);

} // namespace llvm::AIESuperRegUtils

#endif
