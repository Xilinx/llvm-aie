//===- AIEOLPMovRotation.cpp - Rotate MOVs out of OLP top blocks -*- C++
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// In Outer Loop Pipelined (OLP) code, the steady-state top and last-iteration
// top blocks should be sibling blocks with identical instructions so the
// post-machine scheduler's sibling loop optimizer can merge them. However,
// Register allocation often introduces register MOV instructions (e.g. pointer
// copies) in the top blocks that differ between steady and peeled, preventing
// sibling matching.
//
// This pass rotates such MOV instructions from the top blocks into their
// predecessor blocks:
//   - From SteadyTop -> OuterPreheader (and/or SteadyBottom for backedge)
//   - From PeeledIterTop -> SteadyBottom
//
// Two cases are handled:
//   Case 1: Matched MOV pair in both top blocks -> rotate both out
//   Case 2: MOV only in SteadyTop -> rotate to preheader, clone to bottom
//
// After rotation, live-ins are recalculated for all affected blocks.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "aie-olp-mov-rotation"

static cl::opt<bool> EnableOLPMovRotation(
    "aie-enable-olp-mov-rotation", cl::Hidden, cl::init(true),
    cl::desc("Rotate MOV instructions out of OLP top blocks to improve "
             "sibling loop optimization"));

namespace {

class AIEOLPMovRotation : public MachineFunctionPass {
public:
  static char ID;

  AIEOLPMovRotation() : MachineFunctionPass(ID) {
    initializeAIEOLPMovRotationPass(*PassRegistry::getPassRegistry());
  }

  StringRef getPassName() const override {
    return "AIE OLP MOV rotation for sibling optimization";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;
};

char AIEOLPMovRotation::ID = 0;

} // end anonymous namespace

// ===--------------------------------------------------------------------=== //
// Helper predicates
// ===--------------------------------------------------------------------=== //

/// Check if \p Reg (or any overlapping register) is live-in to \p MBB.
static bool isRegLiveIn(MCRegister Reg, const MachineBasicBlock &MBB,
                        const TargetRegisterInfo &TRI) {
  for (const auto &LI : MBB.liveins())
    if (TRI.regsOverlap(LI.PhysReg, Reg))
      return true;
  return false;
}

/// Check if \p Reg (or any overlapping register) is referenced (used or
/// defined) before \p TargetMI in the same block.  Debug instructions are
/// skipped because they do not affect execution semantics.
static bool isRegReferencedBeforeMI(const MachineInstr &TargetMI,
                                    MCRegister Reg,
                                    const TargetRegisterInfo &TRI) {
  const MachineBasicBlock *MBB = TargetMI.getParent();
  for (const MachineInstr &MI : *MBB) {
    if (&MI == &TargetMI)
      return false;
    if (MI.isDebugInstr())
      continue;
    for (const MachineOperand &MO : MI.operands()) {
      if (!MO.isReg() || !MO.getReg().isPhysical())
        continue;
      if (TRI.regsOverlap(MO.getReg(), Reg))
        return true;
    }
  }
  return false;
}

/// Check if \p SrcReg is live at the end of \p PredMBB using LivePhysRegs.
static bool isSrcAvailableInPred(MCRegister SrcReg,
                                 const MachineBasicBlock &PredMBB,
                                 const TargetRegisterInfo &TRI) {
  LivePhysRegs LiveOut(TRI);
  LiveOut.addLiveOuts(PredMBB);
  return LiveOut.contains(SrcReg);
}

/// Check if \p MI is part of the block "tail" that must stay at the end:
/// branches, terminators, barriers, and scheduling barrier pseudos.
static bool isBlockTailInstr(const MachineInstr &MI,
                             const AIEBaseInstrInfo &TII) {
  return MI.isBranch() || MI.isTerminator() || MI.isBarrier() ||
         TII.isSchedBarrier(MI) || TII.isDelayedSchedBarrier(MI);
}

/// Check if \p Reg (or any overlapping register) is referenced (used or
/// defined) by any tail instruction (branches, terminators, barriers) in
/// \p MBB.  Inserting a MOV before those instructions is unsafe when:
///  - A tail instr uses \p Reg: the MOV would clobber the value it reads.
///  - A tail instr defines \p Reg: the tail's def would overwrite the MOV's
///    result, so the successor would see the tail's value instead of the
///    MOV's.
static bool isRegReferencedByTailInstrs(MCRegister Reg,
                                        const MachineBasicBlock &MBB,
                                        const AIEBaseInstrInfo &TII,
                                        const TargetRegisterInfo &TRI) {
  for (const MachineInstr &MI : MBB) {
    if (!isBlockTailInstr(MI, TII))
      continue;
    for (const MachineOperand &MO : MI.operands()) {
      if (!MO.isReg() || !MO.getReg().isPhysical())
        continue;
      if (TRI.regsOverlap(MO.getReg(), Reg))
        return true;
    }
  }
  return false;
}

/// Return true when it is safe to rotate \p MovMI out of its top block.
/// Checks that:
///  - The source register is live-out from both \p Preheader and
///    \p SteadyBottom (the two blocks where copies will be inserted).
///  - Neither the destination nor the source register is referenced before
///    \p MovMI in its block (a prior reference to SrcReg would mean the
///    MOV captures a value produced inside the block, so moving it to a
///    predecessor would capture a stale value instead).
///  - Neither the destination nor the source register is referenced by tail
///    instructions in either \p Preheader or \p SteadyBottom.  A tail use
///    of DstReg would be clobbered by the MOV; a tail def of DstReg would
///    overwrite the MOV's result; a tail def of SrcReg means the MOV would
///    capture a stale value that the tail is about to redefine.
static bool canRotateMov(const MachineInstr &MovMI,
                         const MachineBasicBlock &Preheader,
                         const MachineBasicBlock &SteadyBottom,
                         const AIEBaseInstrInfo &TII,
                         const TargetRegisterInfo &TRI) {
  const MCRegister DstReg = MovMI.getOperand(0).getReg().asMCReg();
  const MCRegister SrcReg = MovMI.getOperand(1).getReg().asMCReg();

  if (!isSrcAvailableInPred(SrcReg, Preheader, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (src not available in preheader): " << MovMI);
    return false;
  }
  if (!isSrcAvailableInPred(SrcReg, SteadyBottom, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (src not available in bottom): " << MovMI);
    return false;
  }
  if (isRegReferencedBeforeMI(MovMI, DstReg, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (dst referenced before MOV): " << MovMI);
    return false;
  }
  if (isRegReferencedBeforeMI(MovMI, SrcReg, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (src referenced before MOV): " << MovMI);
    return false;
  }
  if (isRegReferencedByTailInstrs(DstReg, Preheader, TII, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (dst referenced by tail instrs in preheader): "
                      << MovMI);
    return false;
  }
  if (isRegReferencedByTailInstrs(DstReg, SteadyBottom, TII, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (dst referenced by tail instrs in bottom): "
                      << MovMI);
    return false;
  }
  if (isRegReferencedByTailInstrs(SrcReg, Preheader, TII, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (src referenced by tail instrs in preheader): "
                      << MovMI);
    return false;
  }
  if (isRegReferencedByTailInstrs(SrcReg, SteadyBottom, TII, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (src referenced by tail instrs in bottom): "
                      << MovMI);
    return false;
  }
  return true;
}

// ===--------------------------------------------------------------------=== //
// MOV collection and matching
// ===--------------------------------------------------------------------=== //

/// Return true if \p MI is a plain register-to-register move with no
/// implicit operands.  MOVs with implicit defs (e.g. status flags) or
/// implicit uses are not safe to rotate because those side-effects may
/// depend on or affect block-local state.
static bool isPlainMoveReg(const MachineInstr &MI) {
  if (!MI.isMoveReg())
    return false;
  for (const MachineOperand &MO : MI.operands())
    if (MO.isReg() && MO.isImplicit())
      return false;
  return true;
}

/// Collect all plain MOV (isMoveReg, no implicit operands) instructions
/// from a block.
static SmallVector<MachineInstr *, 4> collectMovInstrs(MachineBasicBlock &MBB) {
  SmallVector<MachineInstr *, 4> Movs;
  for (MachineInstr &MI : MBB)
    if (isPlainMoveReg(MI))
      Movs.push_back(&MI);
  return Movs;
}

/// Try to find a MOV in \p OtherMovs with the same opcode and identical
/// operands as \p MI.
static MachineInstr *findMatchingMov(const MachineInstr &MI,
                                     ArrayRef<MachineInstr *> OtherMovs) {
  for (MachineInstr *Other : OtherMovs) {
    if (!Other)
      continue;
    if (Other->getOpcode() != MI.getOpcode())
      continue;
    if (Other->getNumOperands() != MI.getNumOperands())
      continue;
    bool Match = true;
    for (unsigned I = 0, E = MI.getNumOperands(); I != E; ++I) {
      const MachineOperand &A = MI.getOperand(I);
      const MachineOperand &B = Other->getOperand(I);
      if (!A.isIdenticalTo(B)) {
        Match = false;
        break;
      }
    }
    if (Match)
      return Other;
  }
  return nullptr;
}

// ===--------------------------------------------------------------------=== //
// Insertion and live-in helpers
// ===--------------------------------------------------------------------=== //

/// Insert a copy of \p MI at the end of \p DestMBB, before any branch,
/// terminator, barrier, and scheduling barrier pseudos.
/// Clears kill flags on the source operand of the clone.
static void insertMovBeforeTerminator(MachineBasicBlock &DestMBB,
                                      const MachineInstr &MI,
                                      const AIEBaseInstrInfo &TII) {
  // Walk backwards from end, skipping all block-tail instructions.
  // If the block contains only tail instructions (or is empty), insert at
  // the beginning so the MOV comes before any terminators/barriers.
  auto InsertPt = DestMBB.begin();
  for (auto It = DestMBB.rbegin(), End = DestMBB.rend(); It != End; ++It) {
    if (isBlockTailInstr(*It, TII))
      continue;
    InsertPt = std::next(It->getIterator());
    break;
  }

  MachineInstr *NewMI = DestMBB.getParent()->CloneMachineInstr(&MI);
  DestMBB.insert(InsertPt, NewMI);

  if (NewMI->getNumOperands() > 1 && NewMI->getOperand(1).isReg())
    NewMI->getOperand(1).setIsKill(false);

  LLVM_DEBUG(dbgs() << "  Inserted into BB#" << DestMBB.getNumber() << ": "
                    << *NewMI);
}

/// Recalculate live-ins for a set of blocks.
static void recomputeAllLiveIns(ArrayRef<MachineBasicBlock *> Blocks) {
  for (MachineBasicBlock *MBB : Blocks)
    recomputeLiveIns(*MBB);
}

/// Undef any DBG_VALUE instructions in \p MBB that reference \p Reg.
/// Called before erasing a MOV so that debug info referencing the MOV's
/// result becomes explicitly undefined rather than dangling.
static void undefDebugValuesForReg(MachineBasicBlock &MBB, MCRegister Reg,
                                   const TargetRegisterInfo &TRI) {
  for (MachineInstr &MI : MBB) {
    if (!MI.isDebugValue())
      continue;
    for (MachineOperand &MO : MI.debug_operands()) {
      if (!MO.isReg() || !MO.getReg().isPhysical())
        continue;
      if (TRI.regsOverlap(MO.getReg(), Reg)) {
        LLVM_DEBUG(dbgs() << "  Undef debug value: " << MI);
        MI.setDebugValueUndef();
        break;
      }
    }
  }
}

// ===--------------------------------------------------------------------=== //
// Case 1 & Case 2 rotation logic
// ===--------------------------------------------------------------------=== //

/// Case 1: Matched MOV pair -- rotate both out of their top blocks.
/// \p SteadyMI goes to \p Preheader; \p PeeledMI goes to \p SteadyBottom.
/// On success the consumed \p PeeledMI is also erased from \p PeeledMovs
/// so that later calls to findMatchingMov never dereference a freed pointer.
/// Returns true if rotation was performed.
static bool rotateMatchedPair(MachineInstr &SteadyMI, MachineInstr &PeeledMI,
                              MachineBasicBlock &Preheader,
                              MachineBasicBlock &SteadyBottom,
                              SmallVectorImpl<MachineInstr *> &PeeledMovs,
                              const AIEBaseInstrInfo &TII,
                              const TargetRegisterInfo &TRI) {
  const MCRegister DstReg = PeeledMI.getOperand(0).getReg().asMCReg();
  const MCRegister SrcReg = PeeledMI.getOperand(1).getReg().asMCReg();

  if (isRegReferencedBeforeMI(PeeledMI, DstReg, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (dst referenced before MOV in PeeledTop): "
                      << PeeledMI);
    return false;
  }
  if (isRegReferencedBeforeMI(PeeledMI, SrcReg, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip (src referenced before MOV in PeeledTop): "
                      << PeeledMI);
    return false;
  }

  LLVM_DEBUG(dbgs() << "  Case 1 (matched pair): " << SteadyMI);

  const MCRegister SteadyDst = SteadyMI.getOperand(0).getReg().asMCReg();
  undefDebugValuesForReg(*SteadyMI.getParent(), SteadyDst, TRI);
  insertMovBeforeTerminator(Preheader, SteadyMI, TII);
  SteadyMI.eraseFromParent();

  undefDebugValuesForReg(*PeeledMI.getParent(), DstReg, TRI);
  insertMovBeforeTerminator(SteadyBottom, PeeledMI, TII);
  llvm::erase(PeeledMovs, &PeeledMI);
  PeeledMI.eraseFromParent();

  return true;
}

/// Case 2: MOV only in SteadyTop -- rotate to Preheader, clone to
/// SteadyBottom. Returns true if rotation was performed.
static bool rotateSteadyOnly(MachineInstr &SteadyMI,
                             MachineBasicBlock &Preheader,
                             MachineBasicBlock &SteadyBottom,
                             const MachineBasicBlock &PeeledTop,
                             const AIEBaseInstrInfo &TII,
                             const TargetRegisterInfo &TRI) {
  const MCRegister DstReg = SteadyMI.getOperand(0).getReg().asMCReg();

  // Safety: dst must NOT be live-in to PeeledTop, since we are inserting
  // a def of DstReg at the end of SteadyBottom (PeeledTop's predecessor).
  if (isRegLiveIn(DstReg, PeeledTop, TRI)) {
    LLVM_DEBUG(dbgs() << "  Skip Case 2 (dst live-in to PeeledTop): "
                      << SteadyMI);
    return false;
  }

  LLVM_DEBUG(dbgs() << "  Case 2 (steady-only): " << SteadyMI);

  undefDebugValuesForReg(*SteadyMI.getParent(), DstReg, TRI);
  insertMovBeforeTerminator(Preheader, SteadyMI, TII);
  insertMovBeforeTerminator(SteadyBottom, SteadyMI, TII);
  SteadyMI.eraseFromParent();

  return true;
}

// ===--------------------------------------------------------------------=== //
// Pass entry point
// ===--------------------------------------------------------------------=== //

bool AIEOLPMovRotation::runOnMachineFunction(MachineFunction &MF) {
  if (!EnableOLPMovRotation)
    return false;

  const auto *TII =
      static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());
  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();

  bool Changed = false;

  for (MachineBasicBlock &MBB : MF) {
    if (!AIELoopUtils::isOuterLoopPipelined(MBB))
      continue;

    auto OLS = AIELoopUtils::OuterLoopStructure::tryBuildFrom(MBB);
    if (!OLS)
      continue;

    // isSafeForSiblingMerge() validates the full OLP preconditions:
    // peeled iteration exists, OuterPreheader has exactly one successor,
    // SteadyTop predecessors are {P, SB}, PeeledIterTop's predecessor is
    // SB, and all blocks are laid out consecutively.  If the structure
    // doesn't satisfy these, the sibling optimizer cannot run, so there
    // is no point in rotating MOVs.
    if (!OLS->isSafeForSiblingMerge()) {
      LLVM_DEBUG(dbgs() << "AIEOLPMovRotation: Skipping BB#" << MBB.getNumber()
                        << " -- OLP structure not safe for sibling merge\n");
      continue;
    }

    MachineBasicBlock *const SteadyTop = OLS->SteadyTop;
    MachineBasicBlock *const PeeledTop = OLS->PeeledIterTop;
    MachineBasicBlock *const Preheader = OLS->OuterPreheader;
    MachineBasicBlock *const SteadyBottom = OLS->SteadyBottom;

    LLVM_DEBUG(dbgs() << "AIEOLPMovRotation: Processing OLP structure:\n"
                      << "  Preheader: BB#" << Preheader->getNumber() << "\n"
                      << "  SteadyTop: BB#" << SteadyTop->getNumber() << "\n"
                      << "  SteadyBottom: BB#" << SteadyBottom->getNumber()
                      << "\n"
                      << "  PeeledTop: BB#" << PeeledTop->getNumber() << "\n");

    SmallVector<MachineInstr *, 4> SteadyMovs = collectMovInstrs(*SteadyTop);
    if (SteadyMovs.empty())
      continue;

    // Only SteadyTop MOVs are processed: either they have a matching MOV in
    // PeeledTop (Case 1) or they are unique to SteadyTop (Case 2). MOVs that
    // exist only in PeeledTop are not handled for now.
    SmallVector<MachineInstr *, 4> PeeledMovs = collectMovInstrs(*PeeledTop);
    bool LocalChanged = false;

    for (MachineInstr *SteadyMI : SteadyMovs) {
      if (!canRotateMov(*SteadyMI, *Preheader, *SteadyBottom, *TII, *TRI))
        continue;

      if (MachineInstr *PeeledMI = findMatchingMov(*SteadyMI, PeeledMovs))
        LocalChanged |=
            rotateMatchedPair(*SteadyMI, *PeeledMI, *Preheader, *SteadyBottom,
                              PeeledMovs, *TII, *TRI);
      else
        LocalChanged |= rotateSteadyOnly(*SteadyMI, *Preheader, *SteadyBottom,
                                         *PeeledTop, *TII, *TRI);
    }

    if (LocalChanged) {
      recomputeAllLiveIns({SteadyTop, PeeledTop, Preheader, SteadyBottom});
      Changed = true;
    }
  }

  return Changed;
}

INITIALIZE_PASS(AIEOLPMovRotation, DEBUG_TYPE,
                "AIE OLP MOV rotation for sibling optimization", false, false)

MachineFunctionPass *llvm::createAIEOLPMovRotation() {
  return new AIEOLPMovRotation();
}
