//===-- AIEDelaySlotFiller.cpp - AIE delay slot filler ----------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Simple pass to fills delay slots with useful instructions.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIE2TargetMachine.h"
#include "AIETargetMachine.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

#define DEBUG_TYPE "delay-slot-filler"

STATISTIC(FilledSlots, "Number of delay slots filled");
STATISTIC(FilledSlotsWithNops, "Number of delay slots filled with Nops");

static cl::opt<bool>
    NopDelaySlotFiller("aie-nop-delay-filler", cl::init(false),
                       cl::desc("Fill AIE delay slots with NOPs."), cl::Hidden);

namespace {
struct Filler : public MachineFunctionPass {
  // Target machine description which we query for reg. names, data
  // layout, etc.
  const AIEBaseInstrInfo *TII;
  const TargetRegisterInfo *TRI;
  MachineBasicBlock::instr_iterator LastFiller;

  static char ID;
  explicit Filler() : MachineFunctionPass(ID) {}

  StringRef getPassName() const override { return "AIE Delay Slot Filler"; }

  bool runOnMachineBasicBlock(MachineBasicBlock &MBB);

  bool runOnMachineFunction(MachineFunction &MF) override {
    const AIESubtarget &Subtarget = MF.getSubtarget<AIESubtarget>();
    TII = Subtarget.getInstrInfo();
    TRI = Subtarget.getRegisterInfo();

    bool Changed = false;
    for (MachineFunction::iterator FI = MF.begin(), FE = MF.end(); FI != FE;
         ++FI)
      Changed |= runOnMachineBasicBlock(*FI);
    return Changed;
  }

  MachineFunctionProperties getRequiredProperties() const override {
    return MachineFunctionProperties().set(
        MachineFunctionProperties::Property::NoVRegs);
  }
};
char Filler::ID = 0;
} // end of anonymous namespace

// createAIEDelaySlotFillerPass - Returns a pass that fills in delay
// slots in AIE MachineFunctions
FunctionPass *
llvm::createAIEDelaySlotFillerPass(const AIETargetMachine & /*tm*/) {
  return new Filler();
}

// runOnMachineBasicBlock - Fill in delay slots for the given basic block.
// There are five delay slots per delayed instruction.
bool Filler::runOnMachineBasicBlock(MachineBasicBlock &MBB) {
  bool Changed = false;
  LastFiller = MBB.instr_end();

  // TODO: replace with I->getDesc()->numDelaySlots() ??
  const int NumDelaySlots = 5;

  for (MachineBasicBlock::instr_iterator I = MBB.instr_begin();
       I != MBB.instr_end(); ++I) {

    if (I->getDesc().hasDelaySlot()) {
      MachineBasicBlock::instr_iterator InstrWithSlot = I;
      MachineBasicBlock::instr_iterator InstrAfterSlot = std::next(I);

      auto &NopDesc = TII->get(TII->getNopOpcode());
      for (int DS = 0; DS < NumDelaySlots; DS++) {
        BuildMI(MBB, std::next(I), DebugLoc(), NopDesc);
      }
      FilledSlots += NumDelaySlots;
      FilledSlotsWithNops += NumDelaySlots;

      Changed = true;
      // Record the filler instruction that filled the delay slot.
      // The instruction after it will be visited in the next iteration.
      LastFiller = ++I;

      // Bundle the delay slot filler to InstrWithSlot so that the machine
      // verifier doesn't expect this instruction to be a terminator.
      MIBundleBuilder(MBB, InstrWithSlot, InstrAfterSlot);

      // Mark successors to emit the label
      // This is mainly for reading convenience of the assembly
      for (MachineBasicBlock::succ_iterator Succ = MBB.succ_begin(),
                                            SuccEnd = MBB.succ_end();
           Succ != SuccEnd; ++Succ) {
        MachineBasicBlock *Temp = *Succ;
        Temp->setLabelMustBeEmitted();
      }
    }
  }
  return Changed;
}
