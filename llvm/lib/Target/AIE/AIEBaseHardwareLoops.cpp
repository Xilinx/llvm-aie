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

#include "AIEBaseInstrInfo.h"
#include "AIEBaseSubtarget.h"
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

struct LowOverheadLoop {

  MachineLoop &ML;
  MachineBasicBlock *Preheader = nullptr;
  MachineLoopInfo &MLI;
  const TargetRegisterInfo &TRI;
  const AIEBaseInstrInfo &TII;
  MachineFunction *MF = nullptr;
  MachineInstr *Start = nullptr;
  MachineInstr *Dec = nullptr;
  MachineInstr *End = nullptr;
  SmallPtrSet<MachineInstr *, 4> ToRemove;

  LowOverheadLoop(MachineLoop &ML, MachineLoopInfo &MLI,
                  const TargetRegisterInfo &TRI, const AIEBaseInstrInfo &TII)
      : ML(ML), MLI(MLI), TRI(TRI), TII(TII) {
    MF = ML.getHeader()->getParent();
    if (auto *MBB = ML.getLoopPreheader())
      Preheader = MBB;
    else if (auto *MBB = MLI.findLoopPreheader(&ML, true, true))
      Preheader = MBB;
  }

  bool FoundAllComponents() const {
    return isJNZDLoop() || isZeroOverheadLoop();
  }

  void dump() const {
    if (Start)
      dbgs() << "AIE Loops: Found Loop Start: " << *Start;
    if (Dec)
      dbgs() << "AIE Loops: Found Loop Dec: " << *Dec;
    if (End)
      dbgs() << "AIE Loops: Found Loop End: " << *End;
    if (!FoundAllComponents()) {
      dbgs() << "AIE Loops: Failed to find all loop components.\n";
      dbgs() << "AIE Loops: Not a low-overhead loop.\n";
    }
  }

  bool isJNZDLoop() const {
    return (Dec && End && TII.isHardwareLoopDec(Dec->getOpcode()) &&
            TII.isHardwareLoopJNZ(End->getOpcode()));
  }

  // TODO: implement when introducing ZOL lowering
  bool isZeroOverheadLoop() const { return false; }
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
  bool ProcessLoop(MachineLoop *ML);

  void ExpandLoopStart(LowOverheadLoop &LoLoop);
  void ExpandLoopEnd(LowOverheadLoop &LoLoop);

  void Expand(LowOverheadLoop &LoLoop);
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
      Changed |= ProcessLoop(ML);
  }
  return Changed;
}

bool AIEBaseHardwareLoops::ProcessLoop(MachineLoop *ML) {

  bool Changed = false;

  // Process inner loops first.
  for (MachineLoop *L : *ML)
    Changed |= ProcessLoop(L);

  LowOverheadLoop LoLoop(*ML, *MLI, *TRI, *TII);

  LLVM_DEBUG({
    dbgs() << "AIE Loops: Processing loop containing:\n";
    if (auto *Preheader = LoLoop.Preheader)
      dbgs() << " - Preheader: " << printMBBReference(*Preheader) << "\n";
    for (auto *MBB : ML->getBlocks())
      dbgs() << " - Block: " << printMBBReference(*MBB) << "\n";
  });

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
      else if (MI.getDesc().isCall()) {
        LLVM_DEBUG(dbgs() << "AIE Loops: Found call.\n");
      }
    }
  }

  LLVM_DEBUG(LoLoop.dump());
  if (!LoLoop.FoundAllComponents())
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

  Expand(LoLoop);
  return true;
}

void AIEBaseHardwareLoops::ExpandLoopStart(LowOverheadLoop &LoLoop) {
  LLVM_DEBUG(dbgs() << "AIE Loops: Expanding LoopStart.\n");

  if (LoLoop.isJNZDLoop()) {
    LLVM_DEBUG(dbgs() << "AIE Loops: JNZD loop. Nothing to be done.\n");
    return;
  }

  // Future: Handle true hardware loop setup code
  llvm_unreachable("Implement true hardware loop support");
}

void AIEBaseHardwareLoops::ExpandLoopEnd(LowOverheadLoop &LoLoop) {
  LLVM_DEBUG(dbgs() << "AIE Loops: Expanding LoopEnd.\n");

  MachineInstr *Dec = LoLoop.Dec;
  MachineInstr *End = LoLoop.End;

  assert(Dec->getOperand(0).getReg() == End->getOperand(0).getReg() &&
         "LoopDec not feeding into LoopEnd!?");
  MachineBasicBlock *MBB = End->getParent();
  BuildMI(*MBB, End, End->getDebugLoc(), TII->get(TII->getPseudoJNZDOpcode()))
      .addDef(Dec->getOperand(0).getReg())
      .addReg(Dec->getOperand(1).getReg())
      .addReg(End->getOperand(1).getReg());
  LoLoop.ToRemove.insert(End);
  LoLoop.ToRemove.insert(LoLoop.Dec);
}

void AIEBaseHardwareLoops::Expand(LowOverheadLoop &LoLoop) {

  ExpandLoopStart(LoLoop);
  ExpandLoopEnd(LoLoop);

  for (auto *I : LoLoop.ToRemove) {
    LLVM_DEBUG(dbgs() << "AIE Loops: Erasing " << *I);
    I->eraseFromParent();
  }
}

FunctionPass *llvm::createAIEBaseHardwareLoopsPass() {
  return new AIEBaseHardwareLoops();
}
