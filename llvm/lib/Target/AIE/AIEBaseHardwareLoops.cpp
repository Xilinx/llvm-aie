//===-- AIEBaseHardwareLoops.cpp - CodeGen Low-overhead Loops ---*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass looks for AIE hardware loop related pseudo instructions and lowers
// them to the respective loop control.
// We currently support one loop control structure:
//
// JNZD loops:
// There are two pseudo instructions involved: LoopDec and LoopJNZ
// The former does the loop counter decrement, the latter compares against zero
// and jumps to the beginning of the loop if zero isn't reached.
// After combining the two into the actual JNZD instruction, the compare happens
// before decrementing the value. Thus the incoming loop counter needs to be
// pre-adjusted to contain the backedge-taken count, not the loop trip count.
// This is handled during instruction selection when setting up the loop counter
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIEBaseSubtarget.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "Utils/AIELoopOptionOverrides.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineLoopUtils.h"
#include "llvm/CodeGen/MachineOptimizationRemarkEmitter.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/ReachingDefAnalysis.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "aie-hardware-loops"
#define AIE_HARDWARE_LOOPS_NAME "AIE Hardware Loops pass"

// Use PC-relative addressing for ZOL loop setup when the target provides the
// corresponding opcodes (default: on). This default is overridable per-loop
// via the llvm.loop.hint.aie-pcrel-zol metadata hint (see
// AIE::LoopOptionOverrides).
static cl::opt<bool> EnablePCRelativeZOL(
    "aie-pcrel-zol",
    cl::desc("Use PC-relative addressing for ZOL loop setup when the target "
             "provides it (default: on)."),
    cl::init(true));

namespace {

class LowOverheadLoop {
public: // unused
  MachineLoop &ML;
  MachineLoopInfo &MLI;
  const TargetRegisterInfo &TRI;
  MachineFunction *MF = nullptr;

private:
  const AIEBaseInstrInfo &TII;
  // Delayed erasure to avoid invalidating iterators
  SmallPtrSet<MachineInstr *, 4> Trash;

  void dumpLoopID() {
    std::string LoopID = getLoopID(&ML, "");

    MachineOptimizationRemarkEmitter ORE(*ML.getHeader()->getParent(), nullptr);
    ORE.emit([&]() {
      auto RE = MachineOptimizationRemarkAnalysis(
                    DEBUG_TYPE, "analysis",
                    ML.getHeader()->begin()->getDebugLoc(), ML.getHeader())
                << ore::NV("LoopID", LoopID);

      for (auto *MBB : ML.blocks()) {
        RE << ore::NV("BasicBlock", MBB->getName());
      }

      RE << ore::NV("Zero-Overhead-Loop", foundAllComponents());
      return RE;
    });
  }

  std::string getLoopID(MachineLoop *ML, std::string LoopID) {
    if (!ML)
      return LoopID;

    unsigned LoopCount = 0;
    std::string Suffix = "";
    if (LoopID != "") {
      Suffix = "_" + LoopID;
    }

    std::vector<MachineLoop *> MachineLoops;
    if (ML->getParentLoop()) {
      MachineLoops = ML->getParentLoop()->getSubLoops();
    } else {
      MachineLoops = {MLI.begin(), MLI.end()};
    }

    auto MLHeaderNumber = ML->getHeader()->getNumber();
    for (auto *L : MachineLoops) {
      if (L->getHeader()->getNumber() < MLHeaderNumber)
        LoopCount++;
    }
    return getLoopID(ML->getParentLoop(), std::to_string(LoopCount) + Suffix);
  }

public:
  MachineBasicBlock *Preheader = nullptr;
  MachineInstr *Start = nullptr;
  MachineInstr *Dec = nullptr;
  MachineInstr *End = nullptr;
  MachineInstr *LoopEnd = nullptr;

  LowOverheadLoop(MachineLoop &ML, MachineLoopInfo &MLI,
                  const TargetRegisterInfo &TRI, const AIEBaseInstrInfo &TII)
      : ML(ML), MLI(MLI), TRI(TRI), TII(TII) {
    MF = ML.getHeader()->getParent();
    if (auto *MBB = ML.getLoopPreheader())
      Preheader = MBB;
    else if (auto *MBB = MLI.findLoopPreheader(&ML, true, true))
      Preheader = MBB;
  }
  ~LowOverheadLoop() {
    // dump the loop ID here, otherwise hardware loop detection is wrong
    dumpLoopID();

    // Responsably empty the trash can
    for (auto *I : Trash) {
      LLVM_DEBUG(dbgs() << "AIE Loops: Erasing " << *I);
      I->eraseFromParent();
    }
  }

  bool foundAllComponents() const {
    return isJNZDLoop() || isZeroOverheadLoop();
  }

  void dump() const {
    if (Start)
      dbgs() << "AIE Loops: Found Loop Start: " << *Start;
    if (Dec)
      dbgs() << "AIE Loops: Found Loop Dec: " << *Dec;
    if (End)
      dbgs() << "AIE Loops: Found Loop End: " << *End;
    if (!foundAllComponents()) {
      dbgs() << "AIE Loops: Failed to find all loop components.\n";
      dbgs() << "AIE Loops: Not a low-overhead loop.\n";
    }
  }

  bool isJNZDLoop() const {
    return (Dec && End && TII.isHardwareLoopDec(Dec->getOpcode()) &&
            TII.isHardwareLoopJNZ(End->getOpcode()));
  }

  bool isZeroOverheadLoop() const {
    return (Start && LoopEnd && TII.isHardwareLoopStart(Start->getOpcode()) &&
            TII.isHardwareLoopEnd(LoopEnd->getOpcode()));
  }

  // Detect a degenerate case: ZOL needs to be able to find a last bundle.
  // If not, the loop is just decrementing the loopcounter and is useless.
  bool isEmptyZeroOverheadLoop() const {
    if (!isZeroOverheadLoop()) {
      return false;
    }
    bool LookingForBundle = false;
    for (auto &MI : reverse(*LoopEnd->getParent())) {
      if (LookingForBundle && !MI.isDebugInstr()) {
        return false;
      }
      if (&MI == LoopEnd) {
        LookingForBundle = true;
        continue;
      }
    }
    return true;
  }
  void remove(MachineInstr *Item) { Trash.insert(Item); }
};

class AIEBaseHardwareLoops : public MachineFunctionPass {
  MachineFunction *MF = nullptr;
  MachineLoopInfo *MLI = nullptr;
  ReachingDefAnalysis *RDA = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;
  MachineRegisterInfo *MRI = nullptr;
  const TargetRegisterInfo *TRI = nullptr;

public:
  static char ID;

  AIEBaseHardwareLoops() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<MachineLoopInfoWrapperPass>();
    AU.addRequired<ReachingDefAnalysis>();
    MachineFunctionPass::getAnalysisUsage(AU);
    AU.addRequired<MachineOptimizationRemarkEmitterPass>();
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

  MachineFunctionProperties getRequiredProperties() const override {
    return MachineFunctionProperties()
        .set(MachineFunctionProperties::Property::NoVRegs)
        .set(MachineFunctionProperties::Property::TracksLiveness);
  }

  StringRef getPassName() const override { return AIE_HARDWARE_LOOPS_NAME; }

private:
  bool processLoop(MachineLoop *ML);

  void expandLoopStart(LowOverheadLoop &LoLoop);
  void expandLoopEnd(LowOverheadLoop &LoLoop);
  /// Whether keeping the JNZD of \p LoLoop costs more than falling back to an
  /// explicit decrement and JNZ.
  bool isJNZDExpensive(const LowOverheadLoop &LoLoop) const;
  bool tryExpandLoopEndAsJNZ(LowOverheadLoop &LoLoop);

  void expand(LowOverheadLoop &LoLoop);
};
} // namespace

char AIEBaseHardwareLoops::ID = 0;

INITIALIZE_PASS(AIEBaseHardwareLoops, DEBUG_TYPE, AIE_HARDWARE_LOOPS_NAME,
                false, false)

namespace {
// Everything becomes much simpler if ZOLs exit to their layout successor
// We can achieve that by splitting a non-fallthrough exit jump off into
// a new fallthrough block.
// Note that we don't actually check the block layout; we are near the end of
// the codegen pipeline, and we assume that explicit jumps to a fallthrough
// block don't occur.
bool splitLoopEndJump(MachineBasicBlock &MBB, const AIEBaseInstrInfo *TII) {
  auto Terminator = MBB.getFirstInstrTerminator();
  if (Terminator == MBB.end() ||
      !TII->isHardwareLoopEnd((*Terminator).getOpcode())) {
    return false;
  }
  SmallVector<MachineOperand, 4> Cond;
  MachineBasicBlock *TBB = nullptr, *FBB = nullptr;
  const bool AllowModify = false;
  if (TII->analyzeBranch(MBB, TBB, FBB, Cond, AllowModify)) {
    // can't analyze
    return false;
  }
  if (!FBB) {
    // Is fallthrough already
    return false;
  }

  // So:
  // 1. Create a new block in fallthrough position
  // 2. remove the entire control flow
  // 3. add back the loopend to this block
  // 4. add back the jump to the new block
  // 5. fix successors
  auto *MF = MBB.getParent();
  MachineBasicBlock *NewBB = MF->CreateMachineBasicBlock(FBB->getBasicBlock());
  MF->insert(++MBB.getIterator(), NewBB);
  TII->removeBranch(MBB);
  DebugLoc DL;
  TII->insertBranch(MBB, TBB, nullptr, Cond, DL);
  Cond.clear();
  TII->insertBranch(*NewBB, FBB, nullptr, Cond, DL);
  for (auto *Edge : make_early_inc_range(MBB.successors())) {
    if (Edge == FBB) {
      MBB.removeSuccessor(FBB);
    }
  }

  NewBB->addSuccessor(FBB);
  MBB.addSuccessor(NewBB);
  return true;
}
} // namespace

bool AIEBaseHardwareLoops::runOnMachineFunction(MachineFunction &mf) {
  MF = &mf;
  LLVM_DEBUG(dbgs() << "AIE Hardware Loops on " << MF->getName()
                    << " ------------- \n");

  MLI = &getAnalysis<MachineLoopInfoWrapperPass>().getLI();
  RDA = &getAnalysis<ReachingDefAnalysis>();
  MF->getProperties().set(MachineFunctionProperties::Property::TracksLiveness);
  MRI = &MF->getRegInfo();
  TII = static_cast<const AIEBaseInstrInfo *>(mf.getSubtarget().getInstrInfo());
  TRI = mf.getSubtarget().getRegisterInfo();

  bool Changed = false;
  for (auto &MBB : *MF) {
    Changed |= splitLoopEndJump(MBB, TII);
  }

  for (auto *ML : *MLI) {
    if (ML->isOutermost())
      Changed |= processLoop(ML);
  }
  return Changed;
}

bool AIEBaseHardwareLoops::processLoop(MachineLoop *ML) {

  bool Changed = false;

  // Process inner loops first.
  for (MachineLoop *L : *ML)
    Changed |= processLoop(L);

  MachineBasicBlock *LastMBB = ML->findLoopControlBlock();
  // More than one exit, don't generate ZOL.
  // NOTE: These early returns are assumed to match decisions upstream.
  // When we return here, there should not be anything to expand, so either
  // the LLVM IR should not have created hwloop intrinsics or code selection
  // should have lowered them.
  if (!LastMBB)
    return Changed;
  MachineBasicBlock::iterator LastI = LastMBB->getFirstTerminator();
  if (LastI == LastMBB->end())
    return Changed;

  LowOverheadLoop LoLoop(*ML, *MLI, *TRI, *TII);
  LLVM_DEBUG({
    dbgs() << "AIE Loops: Processing loop containing:\n";
    if (auto *Preheader = LoLoop.Preheader)
      dbgs() << " - Preheader: " << printMBBReference(*Preheader) << "\n";
    for (auto *MBB : ML->getBlocks())
      dbgs() << " - Block: " << printMBBReference(*MBB) << "\n";
  });

  // Look for LoopStart instruction in a given block. If there is only one
  // loop predecessor BB, look in that block.
  std::function<MachineInstr *(MachineBasicBlock *)> FindLoopStart =
      [&FindLoopStart, this](MachineBasicBlock *MBB) -> MachineInstr * {
    for (auto &MI : *MBB) {
      if (TII->isHardwareLoopStart(MI.getOpcode()))
        return &MI;
    }
    if (MBB->pred_size() == 1)
      return FindLoopStart(*MBB->pred_begin());
    return nullptr;
  };

  if (!LoLoop.Preheader)
    return Changed;

  LoLoop.Start = FindLoopStart(LoLoop.Preheader);
  // Find the low-overhead loop components
  for (auto *MBB : reverse(ML->getBlocks())) {
    for (auto &MI : *MBB) {

      if (MI.isDebugValue())
        continue;

      unsigned Opc = MI.getOpcode();
      if (TII->isHardwareLoopDec(Opc))
        LoLoop.Dec = &MI;
      else if (TII->isHardwareLoopJNZ(Opc))
        LoLoop.End = &MI;
      else if (TII->isHardwareLoopStart(Opc))
        LoLoop.Start = &MI;
      // HardwareLoopEnd is not lowered in this pass, the
      // information in LoLoop is captured for sanity check
      // to make sure that all the loop components are found
      // in the loop.
      else if (TII->isHardwareLoopEnd(Opc))
        LoLoop.LoopEnd = &MI;
      else if (MI.getDesc().isCall()) {
        LLVM_DEBUG(dbgs() << "AIE Loops: Found call.\n");
      }
    }
  }

  LLVM_DEBUG(LoLoop.dump());
  if (!LoLoop.foundAllComponents())
    return Changed;

  // Check that the only instruction using LoopDec is LoopEnd
  if (LoLoop.isJNZDLoop()) {
    SmallPtrSet<MachineInstr *, 2> Uses;
    RDA->getReachingLocalUses(LoLoop.Dec, LoLoop.Dec->getOperand(0).getReg(),
                              Uses);
    if (Uses.size() > 1 || !Uses.count(LoLoop.End)) {
      assert(0 && "AIE Loops: Wrong/multiple use of LoopDec result!.\n");
    }
  }

  expand(LoLoop);
  return true;
}

// Return true if the loop \p ML is small enough that the ZOL setup can use
// PC-relative addressing.
//
// The ZOL PC-relative offset is an unsigned 11-bit field, so the byte distance
// from the setup instruction to the loop-end label must fit in [0, 2047]. We
// must decide this here, early in codegen, before the loop has been packed into
// VLIW bundles, so we use a deliberately pessimistic bound: assume every
// instruction ends up in its own full bundle of the maximum bundle size
// (\p MaxBundleSizeBytes). In practice the real size is much smaller:
//   - the software pipeliner combines many instructions into far fewer bundles,
//     and
//   - the code aligner only pads enough to satisfy alignment, not full bundles
//     everywhere.
// If this worst-case bound fits, the real distance is guaranteed to fit.
//
// Note: we count the loop-body instructions only; we do NOT explicitly model
// the loop-setup instructions. We add the fixed LoopSetupDistance slack for the
// setup->loop gap, and the setup sequence itself is small and bounded, so it is
// subsumed by this conservative estimate.
static bool loopFitsZOLPCRel(const MachineLoop &ML, unsigned LoopSetupDistance,
                             unsigned MaxBundleSizeBytes) {
  // Unsigned 11-bit PC-relative field.
  static constexpr unsigned ZOLPcRelMaxBytes = (1u << 11) - 1; // 2047

  unsigned NumInstrs = 0;
  for (const MachineBasicBlock *MBB : ML.getBlocks())
    for (const MachineInstr &MI : *MBB)
      if (!MI.isMetaInstruction() && !MI.isDebugInstr())
        ++NumInstrs;

  const unsigned EstimatedBytes =
      (NumInstrs + LoopSetupDistance) * MaxBundleSizeBytes;
  return EstimatedBytes <= ZOLPcRelMaxBytes;
}

void AIEBaseHardwareLoops::expandLoopStart(LowOverheadLoop &LoLoop) {
  LLVM_DEBUG(dbgs() << "AIE Loops: Expanding LoopStart.\n");

  if (LoLoop.isJNZDLoop()) {
    LLVM_DEBUG(dbgs() << "AIE Loops: JNZD loop. Nothing to be done.\n");
    return;
  }

  // if it's not one of our known loop kinds, we shouldn't be here
  assert(LoLoop.isZeroOverheadLoop());

  MachineInstr *Start = LoLoop.Start;
  MachineBasicBlock *MBB = Start->getParent();
  LLVM_DEBUG(dbgs() << "AIE Loops: ZOL loop. Expanding LoopStart.\n");

  auto LoweringData = TII->getZOLSupport();
  assert(LoweringData);

  // Select absolute vs. PC-relative ZOL setup opcodes. PC-relative addressing
  // requires the target to provide the corresponding opcodes; when present,
  // the -aie-pcrel-zol default is overridable per-loop via
  // llvm.loop.hint.aie-pcrel-zol. The metadata hint lives on the loop block
  // itself (the ZOL body holding the backedge).
  const bool HasPCRelOpcodes =
      LoweringData->SetLoopStartPCRelOpcode.has_value() &&
      LoweringData->SetLoopEndPCRelOpcode.has_value();
  MachineBasicBlock *LoopMBB = LoLoop.LoopEnd->getParent();
  AIE::LoopOptionOverrides Overrides(*LoopMBB);
  // Only use PC-relative setup when the loop is small enough for the 11-bit
  // offset to reach the loop-end label. getMachineBlockAlignmentBytes() gives
  // the target's maximum bundle size in bytes, which is exactly the per-bundle
  // upper bound used by the worst-case "one instruction per full bundle"
  // estimate in loopFitsZOLPCRel.
  const bool UsePCRel =
      HasPCRelOpcodes && Overrides.get(EnablePCRelativeZOL) &&
      loopFitsZOLPCRel(LoLoop.ML, LoweringData->LoopSetupDistance,
                       TII->getMachineBlockAlignmentBytes());

  const unsigned SetLoopStartOpc = UsePCRel
                                       ? *LoweringData->SetLoopStartPCRelOpcode
                                       : LoweringData->SetLoopStartOpcode;
  const unsigned SetLoopEndOpc = UsePCRel ? *LoweringData->SetLoopEndPCRelOpcode
                                          : LoweringData->SetLoopEndOpcode;

  // LoopStart carries an immediate operand that is dedicated to the tripcount
  // update of the pipeliner. We translate to ADD_NC, which has a similar
  // operand.
  BuildMI(*MBB, Start, Start->getDebugLoc(),
          TII->get(LoweringData->SetLoopCountOpcode), LoweringData->LCRegister)
      .addReg(Start->getOperand(0).getReg())
      .addImm(Start->getOperand(1).getImm());
  auto LoopStart =
      BuildMI(*MBB, Start, Start->getDebugLoc(), TII->get(SetLoopStartOpc));
  if (TII->get(SetLoopStartOpc).getNumDefs() > 0)
    LoopStart.addDef(LoweringData->LSRegister);
  LoopStart.addMBB(LoLoop.LoopEnd->getOperand(1).getMBB());

  auto LoopEnd =
      BuildMI(*MBB, Start, Start->getDebugLoc(), TII->get(SetLoopEndOpc));
  if (TII->get(SetLoopEndOpc).getNumDefs() > 0)
    LoopEnd.addDef(LoweringData->LERegister);
  LoopEnd.addSym(LoLoop.LoopEnd->getOperand(0).getMCSymbol());

  LoLoop.remove(LoLoop.Start);
}

static bool isLiveInAnyAlias(const MachineBasicBlock &MBB, Register Reg,
                             const TargetRegisterInfo &TRI) {
  for (MCRegAliasIterator AI(Reg, &TRI, /*IncludeSelf=*/true); AI.isValid();
       ++AI)
    if (MBB.isLiveIn(*AI))
      return true;
  return false;
}

/// Collect in \p Defs the instructions of \p ML that define the JNZD target
/// pointer of \p End.
static void findTargetDefsInLoop(const MachineLoop &ML, const MachineInstr &End,
                                 const TargetRegisterInfo &TRI,
                                 SmallVectorImpl<MachineInstr *> &Defs) {
  const Register Ptr = End.getOperand(1).getReg();
  for (MachineBasicBlock *MBB : ML.blocks())
    for (MachineInstr &MI : *MBB)
      if (!MI.isDebugInstr() && MI.modifiesRegister(Ptr, &TRI))
        Defs.push_back(&MI);
}

/// Whether \p End is the only reader of the value \p Def gives the JNZD target
/// pointer. Collect in \p Reached the blocks of \p ML that value enters.
static bool
isTargetDefOnlyReadByEnd(MachineInstr &Def, const MachineInstr &End,
                         const MachineLoop &ML, const TargetRegisterInfo &TRI,
                         SmallPtrSetImpl<MachineBasicBlock *> &Reached) {
  const Register Ptr = End.getOperand(1).getReg();
  using Position = std::pair<MachineBasicBlock *, MachineBasicBlock::iterator>;
  SmallVector<Position, 8> Worklist = {
      {Def.getParent(), std::next(MachineBasicBlock::iterator(Def))}};
  while (!Worklist.empty()) {
    auto [MBB, I] = Worklist.pop_back_val();
    bool Redefined = false;
    for (; I != MBB->end() && !Redefined; ++I) {
      if (I->isDebugInstr())
        continue;
      if (&*I != &End && I->readsRegister(Ptr, &TRI))
        return false;
      Redefined = I->modifiesRegister(Ptr, &TRI);
    }
    if (Redefined)
      continue;
    for (MachineBasicBlock *Succ : MBB->successors()) {
      if (!ML.contains(Succ)) {
        if (isLiveInAnyAlias(*Succ, Ptr, TRI))
          return false;
      } else if (Reached.insert(Succ).second) {
        Worklist.push_back({Succ, Succ->begin()});
      }
    }
  }
  return true;
}

/// Whether \p Def, which defines the JNZD target pointer \p Ptr, can be
/// removed once nothing reads its value.
static bool isTargetDefRemovable(MachineInstr &Def, Register Ptr) {
  bool SawStore = false;
  if (!Def.isSafeToMove(SawStore) || !Def.getOperand(0).isReg() ||
      Def.getOperand(0).getReg() != Ptr)
    return false;
  for (const MachineOperand &MO : Def.operands())
    if (MO.isReg() && MO.isDef() && !MO.isDead() && MO.getReg() != Ptr)
      return false;
  return true;
}

/// Return the add-immediate defining \p Counter before \p Before in \p MBB, if
/// nothing reads \p Counter in between.
static MachineInstr *findCounterAdjust(MachineBasicBlock &MBB,
                                       MachineBasicBlock::iterator Before,
                                       Register Counter, unsigned AddOpcode,
                                       const TargetRegisterInfo &TRI) {
  for (auto I = Before; I != MBB.begin();) {
    MachineInstr &MI = *--I;
    if (MI.isDebugInstr())
      continue;
    if (MI.modifiesRegister(Counter, &TRI))
      return MI.getOpcode() == AddOpcode && MI.getOperand(2).isImm() &&
                     MI.getOperand(2).getImm() < 0
                 ? &MI
                 : nullptr;
    if (MI.readsRegister(Counter, &TRI))
      return nullptr;
  }
  return nullptr;
}

bool AIEBaseHardwareLoops::isJNZDExpensive(
    const LowOverheadLoop &LoLoop) const {
  MachineInstr *End = LoLoop.End;
  if (End->getOperand(1).isReg()) {
    // A target pointer defined in the loop is rebuilt every iteration, whatever
    // the definition is (rematerialization, reload, copy, ...). JNZD has to
    // wait for it, so it cannot be scheduled early enough to fill its delay
    // slots.
    SmallVector<MachineInstr *, 2> TargetDefs;
    findTargetDefsInLoop(LoLoop.ML, *End, *TRI, TargetDefs);
    if (!TargetDefs.empty()) {
      LLVM_DEBUG(dbgs() << "AIE Loops: JNZD target defined in the loop by "
                        << *TargetDefs.front());
      return true;
    }
  }
  return false;
}

bool AIEBaseHardwareLoops::tryExpandLoopEndAsJNZ(LowOverheadLoop &LoLoop) {
  auto LoweringData = TII->getJNZDSupport();
  MachineInstr *Dec = LoLoop.Dec;
  MachineInstr *End = LoLoop.End;
  MachineBasicBlock *Latch = End->getParent();
  MachineBasicBlock *Header = LoLoop.ML.getHeader();
  MachineBasicBlock *Preheader = LoLoop.ML.getLoopPreheader();
  const Register Counter = Dec->getOperand(0).getReg();
  if (!Preheader || Dec->getParent() != Latch ||
      Dec->getOperand(1).getReg() != Counter || !Header->isLiveIn(Counter) ||
      !Latch->isSuccessor(Header))
    return false;

  for (MachineBasicBlock *MBB : LoLoop.ML.blocks())
    for (MachineInstr &MI : *MBB)
      if (&MI != Dec && MI.modifiesRegister(Counter, TRI))
        return false;

  const unsigned AddOpcode = LoweringData->DecTripCountOpcode;
  auto PreheaderInsertPt = Preheader->getFirstTerminator();
  for (MCPhysReg Reg : TII->get(AddOpcode).implicit_defs())
    if (Preheader->computeRegisterLiveness(TRI, Reg, PreheaderInsertPt) !=
            MachineBasicBlock::LQR_Dead ||
        Latch->computeRegisterLiveness(TRI, Reg, Dec) !=
            MachineBasicBlock::LQR_Dead)
      return false;

  LLVM_DEBUG(dbgs() << "AIE Loops: Falling back to JNZ.\n");

  if (End->getOperand(1).isReg()) {
    // Without End, a value only End reads is dead: drop the pointer from the
    // live-ins of the blocks that value enters, and remove its definition when
    // possible.
    const Register Ptr = End->getOperand(1).getReg();
    SmallVector<MachineInstr *, 2> TargetDefs;
    findTargetDefsInLoop(LoLoop.ML, *End, *TRI, TargetDefs);
    SmallPtrSet<MachineBasicBlock *, 8> DeadIn;
    for (MachineInstr *Def : TargetDefs) {
      SmallPtrSet<MachineBasicBlock *, 8> Reached;
      if (!isTargetDefOnlyReadByEnd(*Def, *End, LoLoop.ML, *TRI, Reached))
        continue;
      DeadIn.insert(Reached.begin(), Reached.end());
      if (isTargetDefRemovable(*Def, Ptr))
        LoLoop.remove(Def);
    }
    for (MachineBasicBlock *MBB : DeadIn)
      for (MCRegAliasIterator AI(Ptr, TRI, /*IncludeSelf=*/true); AI.isValid();
           ++AI)
        if (MBB->isLiveIn(*AI))
          MBB->removeLiveIn(*AI);
  }
  // JNZD tests the counter before decrementing it, so the counter enters the
  // loop holding the trip count minus one. JNZ tests after the decrement.
  const DebugLoc &DL = End->getDebugLoc();
  if (MachineInstr *Init = findCounterAdjust(*Preheader, PreheaderInsertPt,
                                             Counter, AddOpcode, *TRI)) {
    const int64_t Imm = Init->getOperand(2).getImm() + 1;
    const MachineOperand &Src = Init->getOperand(1);
    if (Imm != 0) {
      Init->getOperand(2).setImm(Imm);
    } else {
      // A plain move can be scheduled in more slots than the add.
      if (Src.getReg() != Counter)
        TII->copyPhysReg(*Preheader, Init, Init->getDebugLoc(), Counter,
                         Src.getReg(), Src.isKill());
      LoLoop.remove(Init);
    }
  } else
    BuildMI(*Preheader, PreheaderInsertPt, DL, TII->get(AddOpcode), Counter)
        .addReg(Counter)
        .addImm(1);
  BuildMI(*Latch, Dec, DL, TII->get(AddOpcode), Counter)
      .addReg(Counter)
      .addImm(-1);
  BuildMI(*Latch, End, DL, TII->get(LoweringData->JNZOpcode))
      .addReg(Counter)
      .addMBB(Header);
  LoLoop.remove(End);
  LoLoop.remove(Dec);
  return true;
}

void AIEBaseHardwareLoops::expandLoopEnd(LowOverheadLoop &LoLoop) {
  LLVM_DEBUG(dbgs() << "AIE Loops: Expanding LoopEnd.\n");

  if (!LoLoop.isJNZDLoop()) {
    return;
  }
  auto LoweringData = TII->getJNZDSupport();
  assert(LoweringData);
  MachineInstr *Dec = LoLoop.Dec;
  MachineInstr *End = LoLoop.End;
  assert(Dec->getOperand(0).getReg() == End->getOperand(0).getReg() &&
         "LoopDec not feeding into LoopEnd!?");
  MachineBasicBlock *MBB = End->getParent();
  // An MBB target means the branch carries the loop header itself rather than
  // a pointer register holding its address.
  const MachineOperand &Target = End->getOperand(1);
  const bool HasMBBTarget = Target.isMBB();
  if (isJNZDExpensive(LoLoop) && tryExpandLoopEndAsJNZ(LoLoop))
    return;
  unsigned JNZDOpcode = LoweringData->LoopJNZDOpcode;
  if (HasMBBTarget) {
    assert(LoweringData->LoopJNZDPCRelOpcode &&
           "PC-relative LoopJNZ needs a matching JNZD opcode");
    JNZDOpcode = *LoweringData->LoopJNZDPCRelOpcode;
  }
  auto JNZD = BuildMI(*MBB, End, End->getDebugLoc(), TII->get(JNZDOpcode))
                  .addDef(Dec->getOperand(0).getReg())
                  .addReg(Dec->getOperand(1).getReg());
  if (HasMBBTarget)
    JNZD.addMBB(Target.getMBB());
  else
    JNZD.addReg(Target.getReg());
  LoLoop.remove(End);
  LoLoop.remove(LoLoop.Dec);
}

void AIEBaseHardwareLoops::expand(LowOverheadLoop &LoLoop) {

  if (LoLoop.isEmptyZeroOverheadLoop()) {
    // Strip empty ZOL instructions from the code.
    // The loop block will lose itself as successor.
    LoLoop.remove(LoLoop.Start);
    LoLoop.remove(LoLoop.LoopEnd);
    MachineBasicBlock *LoopBlock = LoLoop.LoopEnd->getParent();
    const bool NormalizeProbabilities = true;
    LoopBlock->removeSuccessor(LoopBlock, NormalizeProbabilities);
    return;
  }
  expandLoopStart(LoLoop);
  expandLoopEnd(LoLoop);
}

FunctionPass *llvm::createAIEBaseHardwareLoopsPass() {
  return new AIEBaseHardwareLoops();
}
