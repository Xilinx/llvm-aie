//===-- AIEBaseHardwareLoops.cpp - CodeGen Low-overhead Loops ---*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
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

#include "AIE2InstrInfo.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseSubtarget.h"
#include "MCTargetDesc/AIE2MCTargetDesc.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
#include "llvm/CodeGen/MachineLoopUtils.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/ReachingDefAnalysis.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "aie-hardware-loops"
#define AIE_HARDWARE_LOOPS_NAME "AIE Hardware Loops pass"

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
    AU.addRequired<MachineLoopInfo>();
    AU.addRequired<ReachingDefAnalysis>();
    MachineFunctionPass::getAnalysisUsage(AU);
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

  void expand(LowOverheadLoop &LoLoop);
};
} // namespace

char AIEBaseHardwareLoops::ID = 0;

INITIALIZE_PASS(AIEBaseHardwareLoops, DEBUG_TYPE, AIE_HARDWARE_LOOPS_NAME,
                false, false)

bool AIEBaseHardwareLoops::runOnMachineFunction(MachineFunction &mf) {
  MF = &mf;
  LLVM_DEBUG(dbgs() << "AIE Hardware Loops on " << MF->getName()
                    << " ------------- \n");

  MLI = &getAnalysis<MachineLoopInfo>();
  RDA = &getAnalysis<ReachingDefAnalysis>();
  MF->getProperties().set(MachineFunctionProperties::Property::TracksLiveness);
  MRI = &MF->getRegInfo();
  TII = static_cast<const AIEBaseInstrInfo *>(mf.getSubtarget().getInstrInfo());
  TRI = mf.getSubtarget().getRegisterInfo();

  bool Changed = false;
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
  BuildMI(*MBB, Start, Start->getDebugLoc(), TII->get(AIE2::MOV_mv_scl),
          AIE2::LC)
      .addReg(Start->getOperand(0).getReg());

  BuildMI(*MBB, Start, Start->getDebugLoc(), TII->get(AIE2::MOVXM_lng_cg),
          AIE2::LS)
      .addMBB(LoLoop.LoopEnd->getOperand(1).getMBB());
  MachineInstrBuilder MIB;
  MIB = BuildMI(*MBB, Start, Start->getDebugLoc(), TII->get(AIE2::MOVXM_lng_cg),
                AIE2::LE)
            .addSym(LoLoop.LoopEnd->getOperand(0).getMCSymbol());
  MachineInstr *MI = MIB.getInstr();
  MI->getOperand(1).ChangeToMCSymbol(
      LoLoop.LoopEnd->getOperand(0).getMCSymbol());

  LoLoop.remove(LoLoop.Start);
}

void AIEBaseHardwareLoops::expandLoopEnd(LowOverheadLoop &LoLoop) {
  LLVM_DEBUG(dbgs() << "AIE Loops: Expanding LoopEnd.\n");

  if (!LoLoop.isJNZDLoop()) {
    return;
  }
  MachineInstr *Dec = LoLoop.Dec;
  MachineInstr *End = LoLoop.End;
  assert(Dec->getOperand(0).getReg() == End->getOperand(0).getReg() &&
         "LoopDec not feeding into LoopEnd!?");
  MachineBasicBlock *MBB = End->getParent();
  BuildMI(*MBB, End, End->getDebugLoc(), TII->get(TII->getPseudoJNZDOpcode()))
      .addDef(Dec->getOperand(0).getReg())
      .addReg(Dec->getOperand(1).getReg())
      .addReg(End->getOperand(1).getReg());
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
