//===-- AIEBasePipelinerLoopInfo.h - AIE PipelinerLoopInfoInfo --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the PipelinerLoopInfo interface for AIE.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASEPIPELINERLOOPINFO_H
#define LLVM_LIB_TARGET_AIE_AIEBASEPIPELINERLOOPINFO_H
#include "AIEBaseInstrInfo.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include <memory>

namespace llvm {

class AIEBasePipelinerLoopInfo : public TargetInstrInfo::PipelinerLoopInfo {
protected:
  const AIEBaseInstrInfo &TII;
  MachineRegisterInfo &MRI;
  MachineInstr *EndLoop;
  MachineBasicBlock *LoopBlock;
  bool Negated = false;

  // MinTripCount is computed by the initial sanity check, either from
  // a constant loop counter initialization or from a user pragma
  // After scheduling it is compared against the stagecount of the found
  // schedule to see if we can take out the prologue stages without
  // guarding them.
  // The value 0 is certain to reject the schedule.
  int64_t MinTripCount = 0;

  /// Find the defining instruction for operand \p Idx of \p MI
  MachineInstr *getDefInstr(MachineInstr *MI, unsigned Idx);

  /// Look through instructions that maintain zero/non-zero
  MachineInstr *skipBooleanNoOps(MachineInstr *Condition);
  /// Check that \p Phi is a PHI node that reads \p Decr from the loop
  /// block
  /// \return The index of the phi value operand the enter the loop
  /// from the preheader, zero if checks fail. (0 is the result
  /// operand of the Phi, so is unambiguous)
  unsigned checkPhi(MachineInstr *Phi, MachineInstr *Decr);
  /// Check that the endloop can be handled. This requires a single
  /// conditional branch to the loop block. The fallthrough is the exit.
  /// \return Instruction defining the condition if successful, else nullptr
  /// If successful, it notes the polarity of the branch in \p Negated.
  MachineInstr *checkLoopControl(MachineInstr *EndLoop);

public:
  AIEBasePipelinerLoopInfo(MachineInstr *EndLoop, const AIEBaseInstrInfo &TII);

  void setMinTripCount(int64_t TC);

  std::optional<bool> createTripCountGreaterCondition(
      int TC, MachineBasicBlock &MBB,
      SmallVectorImpl<MachineOperand> &Cond) override;

  /// Modify the loop such that the trip count is
  /// OriginalTC + TripCountAdjust.
  /// Note that TripCountAdjust is always negative
  void adjustTripCount(int TripCountAdjust) override;

  /// Called when the loop's preheader has been modified to NewPreheader.
  void setPreheader(MachineBasicBlock *NewPreheader) override;

  /// Called when the loop is being removed. Any instructions in the preheader
  /// should be removed.
  ///
  /// Once this function is called, no other functions on this object are
  /// valid; the loop has been removed.
  void disposed() override;
};

std::unique_ptr<TargetInstrInfo::PipelinerLoopInfo>
createAIEBasePipelinerLoopInfo(MachineInstr *EndLoop,
                               const AIEBaseInstrInfo &TII);

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASEPIPELINERLOOPINFO_H
