//===- ReservedRegsLICM.cpp - Machine Loop Invariant Code Motion Pass -----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
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
#include "llvm/ADT/SmallPtrSet.h"
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

static MCRegister getHoistablePhysRegDef(const MachineInstr &MI,
                                         const TargetRegisterInfo &TRI) {
  if (MI.getNumDefs() != 1)
    return MCRegister();

  // CR/SR reserved registers (explicit def) and aie2ps ZOL setup (implicit
  // def of $ls/$le).
  const MachineOperand &MO = *MI.all_defs().begin();
  if (!MO.isReg() || !MO.getReg().isPhysical())
    return MCRegister();
  MCRegister Reg = MO.getReg().asMCReg();
  if (TRI.isSimplifiableReservedReg(Reg))
    return Reg;
  return MCRegister();
}

/// The exiting block may branch to the loop exit and to another block still
/// inside the loop. Exit sinking moves a definition into the exit block, so it
/// runs only when leaving the loop. Return true if \p Reg is live-in at the
/// entry of any in-loop successor of \p ExitingBlock; sinking is then unsafe
/// because the other branch still needs \p Reg but would skip that definition.
static bool isRegLiveInAtInLoopSuccessor(
    const MachineLoop &L, const MachineBasicBlock &ExitingBlock, MCRegister Reg,
    const DenseMap<MachineBasicBlock *, BitVector> &LoopLiveIn) {
  for (MachineBasicBlock *Succ : ExitingBlock.successors()) {
    if (!L.contains(Succ))
      continue;
    if (LoopLiveIn.lookup(Succ).test(Reg))
      return true;
  }
  return false;
}

static bool areUsesLoopInvariant(const MachineLoop &L, const MachineInstr &MI,
                                 MCRegister ExcludeReg,
                                 const TargetRegisterInfo &TRI,
                                 const MachineRegisterInfo &MRI) {
  for (const MachineOperand &MO : MI.operands()) {
    if (!MO.isReg() || !MO.isUse() || !MO.readsReg())
      continue;
    Register Reg = MO.getReg();
    if (!Reg || Reg == ExcludeReg)
      continue;
    for (const MachineInstr &DefMI : MRI.def_instructions(Reg)) {
      if (L.contains(DefMI.getParent()))
        return false;
    }
  }
  return true;
}

CandidateInfo *Candidates::getInfo(const MachineInstr &MI) {
  const TargetRegisterInfo &TRI = *MI.getMF()->getSubtarget().getRegisterInfo();
  if (MCRegister Reg = getHoistablePhysRegDef(MI, TRI)) {
    auto It = Candidates.find(Reg);
    if (It != Candidates.end())
      return &It->second;
    return &Candidates.try_emplace(Reg, Reg).first->second;
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
    AU.addRequired<MachineLoopInfoWrapperPass>();
    AU.addPreserved<MachineLoopInfoWrapperPass>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

private:
  /// Per-block reserved register live-in sets for \p L.
  DenseMap<MachineBasicBlock *, BitVector>
  collectLoopReservedLiveins(const MachineLoop &L);

  /// Go through \p L to look for instructions to hoist into the preheader or
  /// sink into the exit block.
  void runOnLoop(MachineLoop &L);

  /// Find instructions to sink into the exit block
  void processForExitSink(
      MachineLoop &L,
      const DenseMap<MachineBasicBlock *, BitVector> &LoopLiveIn);

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
INITIALIZE_PASS_DEPENDENCY(MachineLoopInfoWrapperPass)
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

  LLVM_DEBUG(dbgs() << "******** Reserved register LICM: " << MF.getName()
                    << " ********\n");

  MachineLoopInfo &MLI = getAnalysis<MachineLoopInfoWrapperPass>().getLI();

  SmallVector<MachineLoop *, 4> Loops = MLI.getLoopsInPreorder();
  for (MachineLoop *L : reverse(Loops)) {
    runOnLoop(*L);
  }

  return Changed;
}

DenseMap<MachineBasicBlock *, BitVector>
ReservedRegsLICM::collectLoopReservedLiveins(const MachineLoop &L) {
  // Fixed-point backward dataflow for per-block reserved-reg live-in. A single
  // backward walk of the header is wrong when defs and uses sit on sibling
  // branches.

  // Conservative liveout assumption: all candidate reserved regs are live at
  // the exit of the loop (they may be used by code after the loop).
  BitVector InitLive(TRI->getNumRegs());
  for (MCPhysReg PhysReg : MRI->getReservedRegs().set_bits()) {
    if (MRI->canSimplifyPhysReg(PhysReg)) {
      InitLive.set(PhysReg);
    }
  }

  // live_in[MBB] = set of reserved registers live at the entry of MBB.
  // Initialised to empty; the worklist grows these sets monotonically.
  DenseMap<MachineBasicBlock *, BitVector> LiveIn;
  for (MachineBasicBlock *MBB : L.getBlocks())
    LiveIn[MBB] = BitVector(TRI->getNumRegs());

  // Seed the worklist with every block in the loop.
  SmallPtrSet<MachineBasicBlock *, 8> InWorklist;
  SmallVector<MachineBasicBlock *, 8> Worklist;
  for (MachineBasicBlock *MBB : L.getBlocks()) {
    Worklist.push_back(MBB);
    InWorklist.insert(MBB);
  }

  while (!Worklist.empty()) {
    MachineBasicBlock *MBB = Worklist.pop_back_val();
    InWorklist.erase(MBB);

    // live-out(MBB) = union of live-in of successors.
    // For successors outside the loop use InitLive conservatively.
    LivePhysRegs LiveRegs(*TRI);
    for (MachineBasicBlock *Succ : MBB->successors()) {
      const BitVector &SuccLive = L.contains(Succ) ? LiveIn[Succ] : InitLive;
      for (unsigned Reg : SuccLive.set_bits())
        LiveRegs.addReg(Reg);
    }

    // Walk backward through MBB to compute live-in. stepBackward handles
    // regmask operands (calls) correctly.
    for (const MachineInstr &MI : reverse(*MBB))
      LiveRegs.stepBackward(MI);

    // Convert to a BitVector, keeping only reserved registers.
    BitVector NewLiveIn(TRI->getNumRegs());
    for (MCRegister Reg : LiveRegs)
      if (MRI->isReserved(Reg))
        NewLiveIn.set(Reg);

    // If the live-in set grew, re-process all in-loop predecessors.
    if (NewLiveIn != LiveIn[MBB]) {
      LiveIn[MBB] = NewLiveIn;
      for (MachineBasicBlock *Pred : MBB->predecessors()) {
        if (L.contains(Pred) && !InWorklist.count(Pred)) {
          Worklist.push_back(Pred);
          InWorklist.insert(Pred);
        }
      }
    }
  }

  return LiveIn;
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

  if (!L.getLoopLatch()) {
    LLVM_DEBUG(dbgs() << "  Loop has no single latch.\n");
    return;
  }
  const MachineBasicBlock *LoopBlock = L.getExitingBlock();

  if (!ExitBlock->getSinglePredecessor() &&
      !LoopBlock->canSplitCriticalEdge(ExitBlock)) {
    LLVM_DEBUG(dbgs() << "  Loop has no dedicated exit.\n");
    return;
  }

  DenseMap<MachineBasicBlock *, BitVector> LoopLiveIn =
      collectLoopReservedLiveins(L);
  const BitVector &ReservedLiveins = LoopLiveIn[L.getHeader()];
  processForExitSink(L, LoopLiveIn);
  processForPreheaderHoist(L, ReservedLiveins);
}

void ReservedRegsLICM::processForExitSink(
    MachineLoop &L,
    const DenseMap<MachineBasicBlock *, BitVector> &LoopLiveIn) {
  // Sink candidates must come from the exiting block (the block that has the
  // edge to the exit block), not the latch. When the latch and the exiting
  // block differ, instructions in the latch are not on the exit path: sinking
  // them to the exit block would insert a def that was never executed on that
  // path, corrupting the register's exit value.
  MachineBasicBlock *ExitingBlock = L.getExitingBlock();
  assert(ExitingBlock);

  // Last def in the exiting block may sink only if the reg is not used below
  // the def, not live at block entry, and not live-in on a continue edge
  // (isRegLiveInAtInLoopSuccessor).
  LivePhysRegs ExitingBlockEntryLive(*TRI);
  for (const MachineInstr &MI : reverse(*ExitingBlock))
    ExitingBlockEntryLive.stepBackward(MI);

  RegDefMap PhysRegChanged(*TRI);
  LivePhysRegs LiveRegs(*TRI);
  Candidates SinkCandidates;

  for (MachineInstr &MI : reverse(*ExitingBlock)) {
    CandidateInfo *CandInfo = SinkCandidates.getInfo(MI);

    if (CandInfo && !PhysRegChanged.hasChanged(CandInfo->DefinedReg) &&
        !LiveRegs.contains(CandInfo->DefinedReg) &&
        !ExitingBlockEntryLive.contains(CandInfo->DefinedReg) &&
        !isRegLiveInAtInLoopSuccessor(L, *ExitingBlock, CandInfo->DefinedReg,
                                      LoopLiveIn)) {
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

  // Walk all blocks in the loop to find unique defs and LICM candidates.
  // RegDefMap::addChangedRegs handles regmask (calls clear UniqueDefs).
  assert(L.getHeader());
  RegDefMap PhysRegChanged(*TRI);
  for (MachineBasicBlock *MBB : L.getBlocks()) {
    for (MachineInstr &MI : *MBB) {
      PhysRegChanged.addChangedRegs(MI);
      HoistCandidates.getInfo(MI);
    }
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
  if (MI.mayLoadOrStore() || !MI.isSafeToMove(DontMoveAcrossStore)) {
    LLVM_DEBUG(dbgs() << "LICM: Instruction not safe to move: " << MI);
    return false;
  }

  // Verify source operands are loop invariant. Reserved-reg setup MOVs often
  // use physical registers post-RA, which generic isLoopInvariant rejects.
  if (!areUsesLoopInvariant(L, MI, Cand.DefinedReg, *TRI, *MRI)) {
    LLVM_DEBUG(dbgs() << "LICM: Instruction not loop invariant: " << MI);
    return false;
  }

  return true;
}
