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

/// Describes a matched save/restore bracket for a reserved register:
///   save:    vreg = COPY $reserved_reg   (first use in latch, before set)
///   set:     $reserved_reg = <imm>       (loop-invariant, immediately after)
///   restore: $reserved_reg = COPY vreg   (last def in latch)
/// The transformation hoists save+set to the preheader and sinks restore to
/// the exit block, so the loop body always sees the loop-invariant value.
struct SaveRestoreBracket {
  MachineInstr *Save = nullptr;
  MachineInstr *Set = nullptr;
  MachineInstr *Restore = nullptr;
  MCPhysReg Reg = MCRegister::NoRegister;
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
    AU.addRequired<MachineLoopInfoWrapperPass>();
    AU.addPreserved<MachineLoopInfoWrapperPass>();
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

  /// Detect and transform a save/restore bracket in \p L's latch block.
  /// A bracket has the form:
  ///   vreg = COPY $reserved_reg   (save)
  ///   $reserved_reg = <imm>       (set, loop-invariant)
  ///   ...uses of $reserved_reg...
  ///   $reserved_reg = COPY vreg   (restore)
  /// Returns the matched bracket, or std::nullopt if not found.
  std::optional<SaveRestoreBracket> findSaveRestoreBracket(MachineLoop &L);

  /// Apply the save/restore bracket transformation: hoist save+set to the
  /// preheader and sink restore to the exit block.
  bool processSaveRestoreBracket(MachineLoop &L);

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
  assert(MRI->isSSA() && "ReservedRegsLICM can only deal with SSA vregs");

  LLVM_DEBUG(dbgs() << "******** Reserved register LICM: " << MF.getName()
                    << " ********\n");

  MachineLoopInfo &MLI = getAnalysis<MachineLoopInfoWrapperPass>().getLI();

  SmallVector<MachineLoop *, 4> Loops = MLI.getLoopsInPreorder();
  for (MachineLoop *L : reverse(Loops)) {
    runOnLoop(*L);
  }

  return Changed;
}

BitVector ReservedRegsLICM::collectLoopReservedLiveins(const MachineLoop &L) {
  LivePhysRegs LiveRegs(*TRI);

  // Conservatively assume reserved regs are all liveouts
  for (MCPhysReg PhysReg : MRI->getReservedRegs().set_bits()) {
    if (MRI->canSimplifyPhysReg(PhysReg)) {
      LiveRegs.addReg(PhysReg);
    }
  }

  // Traverse all blocks in the loop to remove defs. stepBackward handles
  // regmask operands (calls) correctly.
  for (MachineBasicBlock *MBB : L.getBlocks())
    for (const MachineInstr &MI : reverse(*MBB))
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

  BitVector ReservedLiveins = collectLoopReservedLiveins(L);
  processForExitSink(L, ReservedLiveins);
  processForPreheaderHoist(L, ReservedLiveins);
  Changed |= processSaveRestoreBracket(L);
}

// Forward declaration — defined after processForPreheaderHoist.
static void moveInstruction(const CandidateInfo &Cand,
                            MachineBasicBlock::iterator InsertBefore,
                            MachineBasicBlock &InsertMBB);

void ReservedRegsLICM::processForExitSink(MachineLoop &L,
                                          const BitVector &ReservedLiveins) {
  // Pre-compute what's live at the entry of the latch block by walking it
  // fully backward. For multi-block loops with conditional paths, a candidate
  // register may be defined in some branches but not others, making it live at
  // the latch entry on those paths. If the register is live at the latch entry
  // (i.e., used before its first def in the latch), sinking its last def to
  // the exit block would expose the wrong value on those paths.
  assert(L.getLoopLatch());
  LivePhysRegs LatchEntryLive(*TRI);
  for (const MachineInstr &MI : reverse(*L.getLoopLatch()))
    LatchEntryLive.stepBackward(MI);

  RegDefMap PhysRegChanged(*TRI);
  LivePhysRegs LiveRegs(*TRI);
  Candidates SinkCandidates;

  // Walk the latch block, track defs for each register, and
  // collect potential LICM candidates.
  for (MachineInstr &MI : reverse(*L.getLoopLatch())) {
    CandidateInfo *CandInfo = SinkCandidates.getInfo(MI);

    // First time we meet a reserved reg definition while iterating upwards.
    // If that def is not a loop livein, it isn't used in this block after the
    // def, and it isn't live at the latch entry (which would indicate it is
    // used before its def in the latch, possibly carrying a value from a
    // conditional path that skips the def), then one can move the instruction
    // to the exit BB of the loop.
    if (CandInfo && !PhysRegChanged.hasChanged(CandInfo->DefinedReg) &&
        !ReservedLiveins.test(CandInfo->DefinedReg) &&
        !LiveRegs.contains(CandInfo->DefinedReg) &&
        !LatchEntryLive.contains(CandInfo->DefinedReg)) {
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

std::optional<SaveRestoreBracket>
ReservedRegsLICM::findSaveRestoreBracket(MachineLoop &L) {
  MachineBasicBlock *Latch = L.getLoopLatch();
  assert(Latch);

  for (MachineInstr &SaveMI : *Latch) {
    // Look for: vreg = COPY $reserved_reg
    if (!SaveMI.isCopy())
      continue;
    Register SaveDst = SaveMI.getOperand(0).getReg();
    Register SaveSrc = SaveMI.getOperand(1).getReg();
    if (!SaveDst.isVirtual() || !SaveSrc.isPhysical())
      continue;
    MCPhysReg PhysReg = SaveSrc.asMCReg();
    if (!TRI->isSimplifiableReservedReg(PhysReg))
      continue;

    // The saved vreg must have exactly one non-debug use (the restore).
    if (!MRI->hasOneNonDBGUse(SaveDst))
      continue;

    // Helper: does MI use PhysReg?
    auto UsesPhysReg = [&](const MachineInstr &MI) {
      return any_of(MI.operands(), [&](const MachineOperand &MO) {
        return MO.isReg() && MO.isUse() && MO.getReg() == PhysReg;
      });
    };

    // No uses of PhysReg before the save. Instructions before the save
    // rely on the original value of PhysReg; hoisting the set would make
    // them see the loop-invariant value instead.
    auto BeforeSave =
        make_range(Latch->begin(), MachineBasicBlock::iterator(SaveMI));
    if (any_of(BeforeSave, UsesPhysReg))
      continue;

    // Find the set: the next def of PhysReg after the save.
    // No use of PhysReg is allowed between the save and the set.
    MachineInstr *SetMI = nullptr;
    bool PhysRegUsedBeforeSet = false;
    for (MachineInstr *Next = SaveMI.getNextNode(); Next;
         Next = Next->getNextNode()) {
      if (getSinglePhysRegDef(*Next) == PhysReg) {
        SetMI = Next;
        break;
      }
      if (UsesPhysReg(*Next)) {
        PhysRegUsedBeforeSet = true;
        break;
      }
    }
    if (!SetMI || PhysRegUsedBeforeSet)
      continue;

    // The set must be loop-invariant (e.g. MOVX imm).
    CandidateInfo SetCand(PhysReg);
    SetCand.HoistCandidate = SetMI;
    if (!isLoopInvariantInst(SetCand, L))
      continue;

    // Find the restore: the last def of PhysReg in the latch.
    // It must be: $reserved_reg = COPY SaveDst.
    MachineInstr *RestoreMI = nullptr;
    for (MachineInstr &MI2 : reverse(*Latch)) {
      if (getSinglePhysRegDef(MI2) == PhysReg) {
        if (MI2.isCopy() && MI2.getOperand(1).getReg() == SaveDst)
          RestoreMI = &MI2;
        break;
      }
    }
    if (!RestoreMI)
      continue;

    // No uses of PhysReg after the restore. Once the restore is sinked to
    // the exit block, instructions after the restore position in the loop
    // body would see the loop-invariant value (from the set) instead of
    // the restored value.
    auto AfterRestore = make_range(
        std::next(MachineBasicBlock::iterator(RestoreMI)), Latch->end());
    if (any_of(AfterRestore, UsesPhysReg))
      continue;

    // PhysReg must not be used in any non-latch block of the loop.
    // If it were, those uses might see the wrong value after we hoist
    // the set to the preheader.
    if (any_of(L.getBlocks(), [&](MachineBasicBlock *MBB) {
          return MBB != Latch && any_of(*MBB, UsesPhysReg);
        }))
      continue;

    return SaveRestoreBracket{&SaveMI, SetMI, RestoreMI, PhysReg};
  }
  return std::nullopt;
}

bool ReservedRegsLICM::processSaveRestoreBracket(MachineLoop &L) {
  std::optional<SaveRestoreBracket> BracketOpt = findSaveRestoreBracket(L);
  if (!BracketOpt)
    return false;

  const SaveRestoreBracket &Bracket = *BracketOpt;
  MachineBasicBlock *Preheader = L.getLoopPreheader();
  MachineBasicBlock *ExitMBB = L.getExitBlock();
  assert(Preheader && ExitMBB);

  // Ensure the exit block is a dedicated exit (single predecessor).
  // runOnLoop already verified that the critical edge can be split if needed.
  if (!ExitMBB->getSinglePredecessor()) {
    MachineBasicBlock *ExitingBlock = L.getExitingBlock();
    ExitMBB = ExitingBlock->SplitCriticalEdge(ExitMBB, *this);
    assert(ExitMBB);
    LLVM_DEBUG(dbgs() << "Created dedicated exit: "
                      << printMBBReference(*ExitMBB) << "\n");
  }

  LLVM_DEBUG(dbgs() << "Save/restore bracket for " << TRI->getName(Bracket.Reg)
                    << ":\n"
                    << "  Save:    " << *Bracket.Save << "  Set:     "
                    << *Bracket.Set << "  Restore: " << *Bracket.Restore);

  // Move save + set to the preheader (save first so it captures the
  // pre-loop value before the set overwrites it).
  auto InsertPt = Preheader->getFirstTerminator();
  CandidateInfo SaveCand(Bracket.Reg);
  SaveCand.HoistCandidate = Bracket.Save;
  moveInstruction(SaveCand, InsertPt, *Preheader);

  CandidateInfo SetCand(Bracket.Reg);
  SetCand.HoistCandidate = Bracket.Set;
  moveInstruction(SetCand, InsertPt, *Preheader);

  // Sink restore to the exit block.
  CandidateInfo RestoreCand(Bracket.Reg);
  RestoreCand.HoistCandidate = Bracket.Restore;
  moveInstruction(RestoreCand, ExitMBB->getFirstNonPHI(), *ExitMBB);

  return true;
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
static void moveInstruction(const CandidateInfo &Cand,
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
      !MI.isSafeToMove(DontMoveAcrossStore)) {
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
