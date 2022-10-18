//===-- AIEBasePipelinerLoopInfo.cpp - AIE Pipeliner interface --*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the AIE implementation of the PipelinerLoopInfo class.
//
//===----------------------------------------------------------------------===//

#include "AIEBasePipelinerLoopInfo.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachinePipeliner.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>

#define DEBUG_TYPE "pipeliner"
namespace llvm {
cl::opt<int> LoopMinTripCount(
    "aie-loop-min-tripcount",
    cl::desc("Minimum number of loop iterations (warning: applies to all loop"
             " pipelining candidates)"),
    cl::init(-1), cl::Hidden);

AIEBasePipelinerLoopInfo::AIEBasePipelinerLoopInfo(MachineInstr *EndLoop,
                                                   const AIEBaseInstrInfo &TII)
    : TII(TII), MRI(EndLoop->getMF()->getRegInfo()), EndLoop(EndLoop),
      LoopBlock(EndLoop->getParent()) {}

void AIEBasePipelinerLoopInfo::setMinTripCount(int64_t TC) {
  LLVM_DEBUG(dbgs() << "TripCount = " << TC << "\n");
  MinTripCount = TC;
}

std::optional<bool> AIEBasePipelinerLoopInfo::createTripCountGreaterCondition(
    int TC, MachineBasicBlock &MBB, SmallVectorImpl<MachineOperand> &Cond) {
  LLVM_DEBUG(dbgs() << "TripCount > " << TC << "?\n");
  // We only accept schedules that have a stage count that can be accommodated
  // by a statically known tripcount. Hence we don't have to guard the epilogs
  assert(MinTripCount > TC);
  return true;
}

/// Modify the loop such that the trip count is
/// OriginalTC + TripCountAdjust.
void AIEBasePipelinerLoopInfo::adjustTripCount(int TripCountAdjust) {
  LLVM_DEBUG(dbgs() << "TripCountAdjust =  " << TripCountAdjust << "\n");
}

/// Called when the loop's preheader has been modified to NewPreheader.
/// This may be used to move the loop setup here, either because loopstart is
/// implicitly taken as the next instruction, or to minimise life range of
/// loop state registers.
void AIEBasePipelinerLoopInfo::setPreheader(MachineBasicBlock *NewPreheader) {}

/// Called when the loop is being removed. This may happen when the loop kernel
/// is only reached by the last iteration.
/// This might remove loop setup
/// Once this function is called, no other functions on this object are
/// valid; the loop has been removed.
void AIEBasePipelinerLoopInfo::disposed() {}

MachineInstr *AIEBasePipelinerLoopInfo::getDefInstr(MachineInstr *MI,
                                                    unsigned Idx) {
  MachineOperand &MO = MI->getOperand(Idx);
  if (!MO.isReg()) {
    return nullptr;
  }
  Register Reg = MO.getReg();
  if (!Reg.isVirtual()) {
    // unexpected
    return nullptr;
  }
  return MRI.getVRegDef(Reg);
}

MachineInstr *
AIEBasePipelinerLoopInfo::skipBooleanNoOps(MachineInstr *Condition) {
  // This function summarizes condition negations on the way
  if (!Condition) {
    return nullptr;
  }
  if (TII.isBooleanNoOp(Condition->getOpcode())) {
    return skipBooleanNoOps(getDefInstr(Condition, 1));
  }
  if (TII.isBooleanNot(Condition->getOpcode())) {
    Negated = !Negated;
    return skipBooleanNoOps(getDefInstr(Condition, 1));
  }
  return Condition;
}

unsigned AIEBasePipelinerLoopInfo::checkPhi(MachineInstr *Phi,
                                            MachineInstr *Decr) {
  // Check that we have a phi that reads the decrement in the loop, and has one
  // incoming value from outside of the loop. We return the operand index of
  // the instruction that defines that value.
  if (!Phi || Phi->getOpcode() != TargetOpcode::PHI) {
    return 0;
  }
  unsigned NumOps = Phi->getNumOperands();
  if (NumOps != 5) {
    // result + 2 * incoming + 2 * loop
    return 0;
  }
  unsigned Incoming = 0;
  for (unsigned Op = 1; Op < NumOps; Op += 2) {
    MachineBasicBlock *Block = Phi->getOperand(Op + 1).getMBB();
    MachineInstr *Value = getDefInstr(Phi, Op);
    if (Block == LoopBlock) {
      if (Value != Decr) {
        return 0;
      }
    } else {
      Incoming = Op;
    }
  }
  return Incoming;
}

MachineInstr *
AIEBasePipelinerLoopInfo::checkLoopControl(MachineInstr *EndLoop) {
  // EndLoop is the first terminator of the loop block.
  // We allow a second terminator, but not a third. All terminators together
  // should define the loop branch and the exit branch, where a fallthrough
  // is obviously an exit branch
  // We leave information so that we can check the right polarity of
  // the conditional branch.
  //
  MachineInstr *Jump = nullptr;
  MachineBasicBlock::iterator Terminator(EndLoop);
  Terminator++;
  if (Terminator != LoopBlock->end()) {
    Jump = &(*Terminator);
    if (++Terminator != LoopBlock->end()) {
      // more than two terminators is strange
      return nullptr;
    }
  }

  // The first one should be a conditional branch
  if (TII.isJNZ(EndLoop->getOpcode())) {
    Negated = false;
  } else if (TII.isJZ(EndLoop->getOpcode())) {
    // This counts for one negation
    Negated = true;
  } else {
    return nullptr;
  }

  // Should define the backedge of a single block loop
  if (EndLoop->getOperand(1).getMBB() == LoopBlock) {
    return getDefInstr(EndLoop, 0);
  }
  // If the jump holds the backedge, we want one more negation
  if (Jump && Jump->getOperand(0).getMBB() == LoopBlock) {
    // TODO: check if it actually is a jump. Although I guess
    // analyzeBranch would have given up.
    Negated = !Negated;
    return getDefInstr(EndLoop, 0);
  }

  return nullptr;
}

namespace {

// Matches a regular downcounting loop with an explicit decrement of a GPR
// and conditional loop jump on counter != 0.
// Code will look like this (with small variations, like reversal of
// conditions and branches, steps different from 1)
// InitBB:
//   %1 = MOV cst
// LoopBB:
//   %2 = PHI %1, InitBB, %3, LoopBB
//   %3 = ADD %2, -1
//   JNZ %3, LoopBB
//   J ExitBB
// ExitBB:
//
// The pipeliner will schedule the block in a number of stages of equal length
// (the Initiation Interval) and will copy stage 0 in all prologue blocks,
// stage 1 in all but the first, etc.
// Currently, we will only accept the schedule if we know that the tripcount
// is sufficient. We don't insert guards, so all prologue blocks will be
// merged into one.
// When we accept the schedule, we only need to make sure that the value of
// the loop counter is correct when we enter the loop.
// This correction is dependent on the stage in which the condition is
// scheduled, since that determines how many the times the update was copied
// to the prologue.
//
// S0 -----------------
// cycle 0 DECR
// cycle 1
// S0 S1 --------------
// cycle 0 2 COND DECR
// cycle 1 3
// S0 S1 S2 -----------
// cycle 0 2 4 COND DECR
// cycle 1 3 5
// Loop ---------------------------
// cycle 0 2 4 6 COND DECR
// cycle 1 3 5 7 LOOPBRANCH
// Epilogue -----------------------

// In the example above, the loop should iterate 3 times less.
// The condition was scheduled in cycle 2, stage 1, and gets
// copied into two prologue stages. Hence the tripcount update
// is -1, which corresponds directly to the stage in which the
// condition was scheduled.
class DownCountLoop : public AIEBasePipelinerLoopInfo {
  // The instruction initializing the counter
  MachineInstr *Init;
  // The phi instruction for the counter, and the operand where
  // the incoming value is.
  MachineInstr *Phi;
  unsigned InitIdx;
  // The instruction decrementing the counter
  MachineInstr *Decr;
  // The condition going into the loop branch
  MachineInstr *Condition;
  // The decrement value. This is the negated immediate operand
  // of the decrement instruction
  int64_t Step;
  // The stage in which the loop condition is scheduled
  // This is necessary for the static trip count update
  std::optional<int> ConditionStage;

public:
  enum class Assessment {
    Accept,
    InvalidLoopControl,
    NoExitCondition,
    ExitAtNonZero,
    NotACountedLoop,
    NotDownCounting,
    NotRegular,
    UnsuitableInitVal,
    InitStepMismatch
  };
  DownCountLoop(MachineInstr *EndLoop, const AIEBaseInstrInfo &TII)
      : AIEBasePipelinerLoopInfo(EndLoop, TII) {}

  bool shouldIgnoreForPipelining(const MachineInstr *MI) const override {
    return false;

    // Tagging the loop decrement here would force it to be moved to stage0.
    // In that case, it is easy to create prologue guards. It's a pity that it
    // causes the schedule to be rejected if it doesn't succeeed to push it
    // to stage 0, since we can still do useful things if we have a static
    // trip count
    // return MI == Decr;
  }

  std::optional<bool> createTripCountGreaterCondition(
      int TC, MachineBasicBlock &MBB,
      SmallVectorImpl<MachineOperand> &Cond) override;

  bool shouldUseSchedule(SwingSchedulerDAG &SSD, SMSchedule &SMS) override;

  // This adjusts the incoming tripcount.
  void startExpand() override;

  /// Check whether EndLoop can be dealt with by the software pipeliner.
  /// Returns zero for success or a failure reason
  Assessment accept(MachineInstr *EndLoop);
};

DownCountLoop::Assessment DownCountLoop::accept(MachineInstr *EndLoop) {
  // We search for a cycle that looks like a loop decrement and test.
  // We leave all our findings in class members, which can only be trusted
  // if we return Accept.

  Condition = checkLoopControl(EndLoop);
  if (!Condition) {
    LLVM_DEBUG(dbgs() << "Invalid loop control\n");
    return Assessment::InvalidLoopControl;
  }
  Decr = skipBooleanNoOps(Condition);
  if (!Decr) {
    LLVM_DEBUG(dbgs() << "Could not find exit condition\n");
    return Assessment::NoExitCondition;
  }
  if (Negated) {
    // The base pattern is a JNZ loop branch. If we have a
    // negation left, we're out of business
    LLVM_DEBUG(dbgs() << "Exiting when counter non-zero\n");
    return Assessment::ExitAtNonZero;
  }
  if (!TII.isConstStep(*Decr, Step)) {
    LLVM_DEBUG(dbgs() << "Not a counted loop\n");
    return Assessment::NotACountedLoop;
  }
  Step = -Decr->getOperand(2).getImm();
  if (Step <= 0) {
    LLVM_DEBUG(dbgs() << "Not downcounting\n");
    return Assessment::NotDownCounting;
  }
  Phi = getDefInstr(Decr, 1);
  InitIdx = checkPhi(Phi, Decr);
  if (!InitIdx) {
    LLVM_DEBUG(dbgs() << "Not a simple induction value\n");
    return Assessment::NotRegular;
  }
  Init = getDefInstr(Phi, InitIdx);
  if (!Init) {
    LLVM_DEBUG(dbgs() << "Unsuitable counter initialization\n");
    return Assessment::UnsuitableInitVal;
  }

  /// It might just work at this point, and we don't immediately reject,
  /// so that we get a proposed schedule. If we don't set tripcount,
  /// we will reject in shouldUseSchedule()
  if (!TII.isIConst(Init->getOpcode())) {
    if (Step != 1) {
      LLVM_DEBUG(dbgs() << "Dynamic Init, but step value = " << Step << ". "
                        << "Can't compute trip count\n");
    }
  } else {
    int64_t InitVal = Init->getOperand(1).getImm();
    if (InitVal <= 0 || InitVal % Step) {
      LLVM_DEBUG(dbgs() << "Init and step value mismatch\n");
      // We will definitely not reach zero soon or not at all.
      // Note: Would it hurt to pipeline a possibly infinite loop?
      return Assessment::InitStepMismatch;
    }
    setMinTripCount(InitVal / Step);
  }
  LLVM_DEBUG(dbgs() << "Loop accepted\n");
  return Assessment::Accept;
}

bool DownCountLoop::shouldUseSchedule(SwingSchedulerDAG &SSD, SMSchedule &SMS) {
  const int PrologCount = SMS.getMaxStageCount();
  const int StageCount = PrologCount + 1;
  LLVM_DEBUG(dbgs() << "StageCount = " << StageCount << "\n");
  if (StageCount <= 1) {
    LLVM_DEBUG(dbgs() << "PLI: Rejected schedule (StageCount <= 1)\n");
    return false;
  }

  // We need the stage in which the condition is scheduled
  // to update the trip count
  auto ConditionSU = llvm::find_if(
      SSD.SUnits, [&](SUnit &SU) { return SU.getInstr() == Condition; });

  assert(ConditionSU != SSD.SUnits.end());
  ConditionStage = SMS.stageScheduled(&(*ConditionSU));

  // The easy case: a static tripcount
  if (PrologCount < MinTripCount) {
    LLVM_DEBUG(dbgs() << "PLI: Accepting schedule for StageCount=" << StageCount
                      << " (static TC)\n");
    return true;
  }

  if (*ConditionStage != 0) {
    LLVM_DEBUG(
        dbgs() << "PLI: Rejected schedule "
                  "(dynamic TC, Loop condition not scheduled in stage 0)\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "PLI: Accepting schedule for StageCount=" << StageCount
                    << " (dynamic TC)\n");
  return true;
}

std::optional<bool> DownCountLoop::createTripCountGreaterCondition(
    int TC, MachineBasicBlock &MBB, SmallVectorImpl<MachineOperand> &Cond) {

  LLVM_DEBUG(dbgs() << "Check TC > " << TC << "\n");
  // The static case. We know we don't have to guard.
  if (MinTripCount > TC) {
    return true;
  }

  if (LoopMinTripCount > 0 && LoopMinTripCount >= TC)
    return true;

  // We effectively use the same condition as EndLoop, reusing its register
  // operand, which should be properly remapped to the prologue version by
  // the Modulo Extractor.
  // We have two possibilities for the loop control here:
  //
  // JNZ Loop
  // J/Fallthrough Exit
  //
  // and
  //
  // JZ Exit
  // J Loop
  //
  // EndLoop always refers to the first, conditional, branch, and we can
  // find out which of the two cases we have by looking at the branch target.
  // The guards will jump to the epilogue block corresponding to the prologue
  // block it is guarding. As such it reflects the loop exit condition, we
  // create that exit condition by inverting the condition for the first case
  // and reuse it for the second case.
  // The resulting guard will be JZ Epilogue for the first case and
  // JNZ Epilogue for the second case. To be complete, the JNZ Loop can
  // also  be coded as EQZ; JZ Loop, but that is implicit in the branch opcode.

  Cond.clear();
  unsigned LoopBranchOpcode = EndLoop->getOpcode();
  unsigned GuardOpcode = EndLoop->getOperand(1).getMBB() == LoopBlock
                             ? TII.getOppositeBranchOpcode(LoopBranchOpcode)
                             : LoopBranchOpcode;

  Cond.push_back(MachineOperand::CreateImm(GuardOpcode));
  Cond.push_back(EndLoop->getOperand(0));
  return {};
}

void DownCountLoop::startExpand() {
  // We have recorded the stage in which the loop condition
  // is computed. That matches the number of updates that
  // will not get extracted into the prologue, which is the adjustment
  // we need.
  assert(ConditionStage);
  const int Adjust = *ConditionStage;
  if (!Adjust) {
    return;
  }

  // We have
  // Preheader:
  //   %1 = Tripcount
  // Loop:
  //   %2 = PHI(Preheader:%1 , Loop: %nn)
  //
  // We rewrite that to
  // Preheader:
  //   %1 = Tripcount
  //   %99 = ADD %1, Adjust
  // Loop:
  //   %2 = PHI(Preheader:%99 , Loop: %nn)

  // Note that the opcode of the decrement instruction represents
  // an immediate add, which we use to perform the update
  unsigned AddImmOpc = Decr->getOpcode();
  Register Reg = Init->getOperand(0).getReg();
  Register NewReg = MRI.createVirtualRegister(MRI.getRegClass(Reg));
  MachineBasicBlock::iterator Insert(Init);
  Insert++;
  BuildMI(*Init->getParent(), Insert, Init->getDebugLoc(), TII.get(AddImmOpc),
          NewReg)
      .addReg(Reg)
      .addImm(-Adjust * Step);
  Phi->getOperand(InitIdx).setReg(NewReg);
}
} // namespace

std::unique_ptr<TargetInstrInfo::PipelinerLoopInfo>
createAIEBasePipelinerLoopInfo(MachineInstr *EndLoop,
                               const AIEBaseInstrInfo &TII) {
  LLVM_DEBUG(dbgs() << "PLI: ----START LOOP----\n");
  DownCountLoop DCL(EndLoop, TII);
  auto Outcome = DCL.accept(EndLoop);
  if (Outcome == DownCountLoop::Assessment::Accept) {
    LLVM_DEBUG(dbgs() << "PLI: Loop accepted by DownCountLoop\n");
    return std::make_unique<DownCountLoop>(DCL);
  }
  LLVM_DEBUG(dbgs() << "PLI: Loop rejected: " << (int)Outcome << "\n");
  return nullptr;
}

} // namespace llvm
