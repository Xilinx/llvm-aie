//===- ReservedRegsLICM.cpp - Machine Loop Invariant Code Motion Pass -----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass performs loop invariant code motion on machine instructions that
// define reserved physical registers. It can move those to the preheader or
// the exit block. This is based on the LivePhysRegs analysis and tracking of
// regs that are altered within loops.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "reserved-reg-licm"

namespace {

/// Track registers that have been defined/changed
class RegDefMap {
  const TargetRegisterInfo &TRI;
  DenseMap<MCRegister, MachineInstr *> UniqueDefs;
  BitVector PhysRegChanged;

public:
  RegDefMap(const TargetRegisterInfo &TRI)
      : TRI(TRI), PhysRegChanged(TRI.getNumRegs()) {}

  /// Track every register that was changed by \p MI
  void addChangedRegs(MachineInstr &MI);

  /// Whether \p Reg or any of its aliases has been changed.
  bool hasChanged(MCRegister Reg) const;

  /// Whether \p Reg has been defined a single time.
  MachineInstr *getUniqueDef(MCRegister Reg) const;
};

void RegDefMap::addChangedRegs(MachineInstr &MI) {
  for (const MachineOperand &MO : MI.operands()) {
    if (MO.isRegMask()) {
      PhysRegChanged.setBitsNotInMask(MO.getRegMask());
      UniqueDefs.clear();
    } else if (MO.isReg() && MO.isDef() && MO.getReg().isPhysical()) {
      MCRegister Reg = MO.getReg();
      if (!PhysRegChanged.test(Reg)) {
        assert(!UniqueDefs.contains(Reg));
        UniqueDefs[Reg] = &MI;
      } else {
        UniqueDefs.erase(Reg);
      }
      PhysRegChanged.set(Reg);
    }
  }
}

bool RegDefMap::hasChanged(MCRegister Reg) const {
  assert(range_size(TRI.regunits(Reg)) == 1 && "Phys reg has aliases.");
  return PhysRegChanged.test(Reg);
}

MachineInstr *RegDefMap::getUniqueDef(MCRegister Reg) const {
  assert(range_size(TRI.regunits(Reg)) == 1 && "Phys reg has aliases.");
  if (auto It = UniqueDefs.find(Reg); It != UniqueDefs.end())
    return It->second;
  return nullptr;
}

/// Information about a register and its defining instruction that is
/// a candidate for hoisting/sinking.
struct CandidateInfo {
  CandidateInfo(MCPhysReg DefinedReg) : DefinedReg(DefinedReg) {}
  MCPhysReg DefinedReg = MCRegister::NoRegister;
  MachineInstr *HoistCandidate = nullptr;
};

/// Used to collect candidates for hoisting/sinking
class Candidates {
  DenseMap<MCPhysReg, CandidateInfo> Candidates;
  using CandidateIt = DenseMap<MCPhysReg, CandidateInfo>::iterator;

public:
  CandidateInfo *getInfo(const MachineInstr &MI);
  CandidateIt begin() { return Candidates.begin(); }
  CandidateIt end() { return Candidates.end(); }
};

static MCRegister getSinglePhysRegDef(const MachineInstr &MI) {
  if (MI.getNumDefs() == 1 && MI.getNumExplicitDefs() == 1) {
    const MachineOperand &MO = MI.getOperand(0);
    if (MO.getReg().isPhysical())
      return MO.getReg().asMCReg();
  }
  return MCRegister();
}

CandidateInfo *Candidates::getInfo(const MachineInstr &MI) {
  const TargetRegisterInfo &TRI = *MI.getMF()->getSubtarget().getRegisterInfo();
  if (MCRegister Reg = getSinglePhysRegDef(MI)) {
    auto It = Candidates.find(Reg);
    if (It != Candidates.end()) {
      return &It->second;
    }
    if (TRI.isSimplifiableReservedReg(Reg)) {
      // Only consider "simplifiable" reserved regs in this pass.
      return &Candidates.try_emplace(Reg, Reg).first->second;
    }
  }
  return nullptr;
}

class ReservedRegsLICM : public MachineFunctionPass {
  const TargetRegisterInfo *TRI;
  MachineRegisterInfo *MRI;

  /// Whether the MachineFunction got changed
  bool Changed = false;

public:
  static char ID;
  ReservedRegsLICM() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<MachineLoopInfo>();
    AU.addPreserved<MachineLoopInfo>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

private:
  /// Collect all simplifiable reserved registers that all livein for \p L.
  BitVector collectLoopReservedLiveins(const MachineLoop &L);

  /// Go through \p L to look for instructions to hoist into the preheader or
  /// sink into the exit block.
  void runOnLoop(MachineLoop &L);

  /// Find instructions to sink into the exit block
  void processForExitSink(MachineLoop &L, const BitVector &ReservedLiveins);

  /// Sink \p Cand to \p L's exit block if it is safe to do so.
  bool trySinkToExitBlock(const CandidateInfo &Cand, MachineLoop &L);

  /// Find instructions to hoist to the preheader
  void processForPreheaderHoist(MachineLoop &L,
                                const BitVector &ReservedLiveins);

  /// Hoist \p Cand to \p L's preheader if it is safe to do so.
  bool tryHoistToPreHeader(const CandidateInfo &Cand, MachineLoop &L);

  /// Verify if \p Cand is loop invariant and can be safely hoisted.
  /// \pre Cand->DefinedReg has a unique live value within the loop. This is
  /// verified by processForExitSink() or processForPreheaderHoist().
  bool isLoopInvariantInst(const CandidateInfo &Cand, const MachineLoop &L);
};

} // end anonymous namespace

char ReservedRegsLICM::ID;

char &llvm::ReservedRegsLICMID = ReservedRegsLICM::ID;

INITIALIZE_PASS_BEGIN(ReservedRegsLICM, DEBUG_TYPE,
                      "Machine LICM for reserved regs", false, false)
INITIALIZE_PASS_DEPENDENCY(MachineLoopInfo)
INITIALIZE_PASS_END(ReservedRegsLICM, DEBUG_TYPE,
                    "Machine LICM for reserved regs", false, false)

llvm::FunctionPass *llvm::createReservedRegsLICMPass() {
  return new ReservedRegsLICM();
}

bool ReservedRegsLICM::runOnMachineFunction(MachineFunction &MF) {
  if (skipFunction(MF.getFunction()))
    return false;

  const TargetSubtargetInfo &ST = MF.getSubtarget();
  TRI = ST.getRegisterInfo();
  MRI = &MF.getRegInfo();
  assert(MRI->isSSA() && "ReservedRegsLICM can only deal with SSA vregs");

  LLVM_DEBUG(dbgs() << "******** Reserved register LICM: " << MF.getName()
                    << " ********\n");

  MachineLoopInfo &MLI = getAnalysis<MachineLoopInfo>();

  SmallVector<MachineLoop *, 4> Loops = MLI.getBase().getLoopsInPreorder();
  for (MachineLoop *L : reverse(Loops)) {
    runOnLoop(*L);
  }

  return Changed;
}

BitVector ReservedRegsLICM::collectLoopReservedLiveins(const MachineLoop &L) {
  const MachineBasicBlock &Header = *L.getHeader();
  LivePhysRegs LiveRegs(*TRI);

  // Conservativaly assume reserved reserved regs are all liveouts
  for (MCPhysReg PhysReg : MRI->getReservedRegs().set_bits()) {
    if (MRI->canSimplifyPhysReg(PhysReg)) {
      LiveRegs.addReg(PhysReg);
    }
  }

  // Traverse instructions to remove defs
  for (const MachineInstr &MI : reverse(Header))
    LiveRegs.stepBackward(MI);

  BitVector ReservedLiveins(TRI->getNumRegs());
  for (MCRegister Reg : LiveRegs) {
    if (MRI->isReserved(Reg)) {
      ReservedLiveins.set(Reg);
    }
  }
  return ReservedLiveins;
}

/// Walk the specified region of the CFG and hoist loop invariants out to the
/// preheader.
void ReservedRegsLICM::runOnLoop(MachineLoop &L) {
  const MachineBasicBlock *Preheader = L.getLoopPreheader();
  const MachineBasicBlock *ExitBlock = L.getExitBlock();
  if (!Preheader || !ExitBlock) {
    LLVM_DEBUG(dbgs() << "  Loop has no preheader or single exit block.\n");
    return;
  }

  // TODO: Handle simple multi-BB loops.
  if (L.getNumBlocks() != 1) {
    LLVM_DEBUG(dbgs() << "  Loop has multiple blocks.\n");
    return;
  }
  const MachineBasicBlock *LoopBlock = L.getExitingBlock();

  if (!ExitBlock->getSinglePredecessor() &&
      !LoopBlock->canSplitCriticalEdge(ExitBlock)) {
    LLVM_DEBUG(dbgs() << "  Loop has no dedicated exit.\n");
    return;
  }

  BitVector ReservedLiveins = collectLoopReservedLiveins(L);
  processForExitSink(L, ReservedLiveins);
  processForPreheaderHoist(L, ReservedLiveins);
}

void ReservedRegsLICM::processForExitSink(MachineLoop &L,
                                          const BitVector &ReservedLiveins) {
  RegDefMap PhysRegChanged(*TRI);
  LivePhysRegs LiveRegs(*TRI);
  Candidates SinkCandidates;

  // Walk the entire region, track defs for each register, and
  // collect potential LICM candidates.
  assert(L.getNumBlocks() == 1 && L.getLoopLatch());
  for (MachineInstr &MI : reverse(*L.getLoopLatch())) {
    CandidateInfo *CandInfo = SinkCandidates.getInfo(MI);

    // First time we meet a reserved reg definition while iterating upwards.
    // If that def is not a loop livein and it isn't used in this block either,
    // then one can move the instruction to the exit BB of the loop.
    if (CandInfo && !PhysRegChanged.hasChanged(CandInfo->DefinedReg) &&
        !ReservedLiveins.test(CandInfo->DefinedReg) &&
        !LiveRegs.contains(CandInfo->DefinedReg)) {
      assert(!CandInfo->HoistCandidate);
      CandInfo->HoistCandidate = &MI;
    }

    // Track physregs that were changed/used by MI, and what's now live.
    PhysRegChanged.addChangedRegs(MI);
    LiveRegs.stepBackward(MI);
  }

  for (auto &[Reg, CandInfo] : SinkCandidates) {
    Changed |= trySinkToExitBlock(CandInfo, L);
  }
}

void ReservedRegsLICM::processForPreheaderHoist(
    MachineLoop &L, const BitVector &ReservedLiveins) {
  Candidates HoistCandidates;

  // Walk the entire loop to find unique defs and LICM candidates
  assert(L.getNumBlocks() == 1 && L.getHeader());
  RegDefMap PhysRegChanged(*TRI);
  for (MachineInstr &MI : *L.getHeader()) {
    PhysRegChanged.addChangedRegs(MI);
    HoistCandidates.getInfo(MI);
  }

  for (auto &[Reg, CandInfo] : HoistCandidates) {
    // Reg is defined a single time in the loop and is not loop-livein.
    // Try to hoist it to the pre-header
    if (PhysRegChanged.getUniqueDef(Reg) && !ReservedLiveins.test(Reg)) {
      CandInfo.HoistCandidate = PhysRegChanged.getUniqueDef(Reg);
      Changed |= tryHoistToPreHeader(CandInfo, L);
    }
  }
}

/// When an instruction is found to only use loop invariant operands that is
/// safe to hoist/sink, this function is called to actually move the MI out of
/// the loop.
void moveInstruction(const CandidateInfo &Cand,
                     MachineBasicBlock::iterator InsertBefore,
                     MachineBasicBlock &InsertMBB) {
  MachineInstr &MI = *Cand.HoistCandidate;
  LLVM_DEBUG(dbgs() << "Moving to " << printMBBReference(InsertMBB) << " from "
                    << printMBBReference(*MI.getParent()) << ": " << MI);

  // Splice the instruction to the preheader.
  // Note we do not need to extend the liveins of the loops BBs with the
  // newly-hoisted def, because reserved regs are always considered live.
  InsertMBB.splice(InsertBefore, MI.getParent(), MI);

  // Since we are moving the instruction out of its basic block, we do not
  // retain its debug location. Doing so would degrade the debugging
  // experience and adversely affect the accuracy of profiling information.
  assert(!MI.isDebugInstr() && "Should not hoist/sink debug inst");
  MI.setDebugLoc(DebugLoc());
}

bool ReservedRegsLICM::trySinkToExitBlock(const CandidateInfo &Cand,
                                          MachineLoop &L) {
  if (isLoopInvariantInst(Cand, L)) {
    MachineBasicBlock *InsertMBB = L.getExitBlock();
    assert(InsertMBB);
    if (!InsertMBB->getSinglePredecessor()) {
      MachineBasicBlock *ExitingBlock = L.getExitingBlock();
      // Note SplitCriticalEdge will also update MachineLoopInfo.
      InsertMBB = ExitingBlock->SplitCriticalEdge(InsertMBB, *this);
      assert(InsertMBB);
      LLVM_DEBUG(dbgs() << "Created dedicated exit: "
                        << printMBBReference(*InsertMBB) << "\n");
    }
    moveInstruction(Cand, InsertMBB->getFirstNonPHI(), *InsertMBB);
    return true;
  }
  return false;
}

bool ReservedRegsLICM::tryHoistToPreHeader(const CandidateInfo &Cand,
                                           MachineLoop &L) {
  if (isLoopInvariantInst(Cand, L)) {
    MachineBasicBlock *InsertMBB = L.getLoopPreheader();
    assert(InsertMBB);
    moveInstruction(Cand, InsertMBB->getFirstTerminator(), *InsertMBB);
    return true;
  }
  return false;
}

/// Returns true if the instruction is loop invariant.
bool ReservedRegsLICM::isLoopInvariantInst(const CandidateInfo &Cand,
                                           const MachineLoop &L) {
  if (!Cand.HoistCandidate) {
    return false;
  }
  MachineInstr &MI = *Cand.HoistCandidate;

  // Check if it's safe to move the instruction.
  bool DontMoveAcrossStore = true;
  if (MI.mayLoadOrStore() ||
      !MI.isSafeToMove(/*AA=*/nullptr, DontMoveAcrossStore)) {
    LLVM_DEBUG(dbgs() << "LICM: Instruction not safe to move: " << MI);
    return false;
  }

  // Then verify all operands are loop invariant
  auto IsInvariantOperand = [&](const MachineOperand &MO) -> bool {
    if (MO.isImm())
      return true;
    if (MO.isReg()) {
      Register Reg = MO.getReg();
      if (MO.isDef())
        return Reg == Cand.DefinedReg;
      return Reg.isVirtual() && !L.contains(MRI->getUniqueVRegDef(Reg));
    }
    return false;
  };
  if (!all_of(MI.operands(), IsInvariantOperand)) {
    LLVM_DEBUG(dbgs() << "LICM: Operands not loop invariant: " << MI);
    return false;
  }

  return true;
}
