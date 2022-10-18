//===----- AIEFormatSelector.cpp - VLIW Format Determination --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// After packetization, each wide VLIW instruction is represented as a BUNDLE
/// machine instruction, followed by the instructions that execute together.
/// At this point, we can select a format encoding. This is represented by
/// replacing the BUNDLE opcode with the approporiate container instruction.
/// Note that this need only be done for instructions with multiple slots.
/// Instructions encoded with only a single slot (basically, 16-bit and 32-bit
/// AIE instructions) can stand on their own. Note that if this transformation
/// does not happen, then the BUNDLE will be emitted as individual instructions,
/// and will not be executed concurrently.
//
//===----------------------------------------------------------------------===//

#include "AIEFormatSelector.h"
#include "AIE.h"
#include "AIEBundle.h"
#include "AIEInstrInfo.h"
#include "AIESubtarget.h"
#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "llvm/ADT/SmallBitVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "packets"

namespace {

class AIEFormatSelector : public MachineFunctionPass {

public:
  static char ID;
  AIEFormatSelector() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  StringRef getPassName() const override { return "AIE Format Selector"; }

  bool runOnMachineFunction(MachineFunction &Fn) override;
};

bool AIEFormatSelector::runOnMachineFunction(MachineFunction &Fn) {
  const AIESubtarget &ST = Fn.getSubtarget<AIESubtarget>();
  const AIEInstrInfo *TII = ST.getInstrInfo();

  LLVM_DEBUG(llvm::dbgs() << "Format Selecting " << Fn.getName() << "\n");

  // Loop over all basic blocks, searching for BUNDLE instructions. When
  // found, look through the contained instructions and pick the right
  // container instruction format.
  for (auto &MBB : Fn) {
    MachineBasicBlock::iterator End = MBB.end();
    MachineBasicBlock::iterator MI = MBB.begin();
    while (MI != End) {
      if (MI->isBundle()) {
        LLVM_DEBUG(llvm::dbgs() << "\nSelecting VLIW format for bundle:\n");
        LLVM_DEBUG(llvm::dbgs() << *MI);

        // Iterate over the instructions in the bundle.
        MachineBasicBlock::instr_iterator I = MI->getIterator();
        MachineBasicBlock::instr_iterator E = MI->getParent()->instr_end();
        I++;

        AIE::MachineBundle B(TII->getFormatInterface());
        // Register everything in the bundle.
        while ((I != E) && I->isBundledWithPred()) {
          LLVM_DEBUG(llvm::dbgs() << "Add to bundle: " << *I);
          B.add(&(*I));
          I++;
        }

        // Retrieve the format
        // If this fails, the scheduler has violated the bundle's format
        // constraint
        const VLIWFormat *Format = B.getFormatOrNull();
        assert(Format);

        // We're going to reinsert the bundle's instructions
        // in the order that the format expects. Pick up the
        // insertion point here
        applyFormatOrdering(B, *Format, &*MI, std::next(MI),
                            /*InsertMissingNops=*/true);
        MI->setDesc(TII->get(Format->Opcode));

        LLVM_DEBUG(llvm::dbgs() << "Slots: "; for (auto &KV
                                                   : B.getSlotMap()) {
          const MCSlotInfo *SlotInfo = TII->getSlotInfo(KV.first);
          // if KV == Default/Unknwon => SlotInfo = nullptr
          if (SlotInfo == nullptr)
            llvm::dbgs() << "unknown"
                         << " ";
          else
            llvm::dbgs() << SlotInfo->getName() << " ";
        } llvm::dbgs() << "\n";
                   llvm::dbgs() << "Selected format " << Format->Name << "\n";);
      }
      ++MI;
    }
  }

  return true;

}

} // end anonymous namespace

INITIALIZE_PASS_BEGIN(AIEFormatSelector, DEBUG_TYPE,
                     "AIE Format Selector", false, false)
INITIALIZE_PASS_END(AIEFormatSelector, DEBUG_TYPE,
                    "AIE Format Selector", false, false)

char AIEFormatSelector::ID = 0;

llvm::FunctionPass *llvm::createAIEFormatSelector() {
  return new AIEFormatSelector();
}

void llvm::applyFormatOrdering(AIE::MachineBundle &Bundle,
                               const VLIWFormat &Format,
                               MachineInstr *BundleRoot,
                               MachineBasicBlock::iterator InsertPoint,
                               bool InsertMissingNops) {
  if (Bundle.empty())
    return;

  MachineBasicBlock &MBB = *Bundle.getInstrs()[0]->getParent();
  auto *TII = static_cast<const AIEBaseInstrInfo *>(
      MBB.getParent()->getSubtarget().getInstrInfo());

  // Run over the slots of the format and either insert the occupying
  // instruction or a nop. Reapply bundling.
  for (MCSlotKind Slot : Format.getSlots()) {
    const MCSlotInfo *SlotInfo = TII->getSlotInfo(Slot);
    assert(SlotInfo);

    llvm::MachineInstr *Instr = Bundle.at(Slot);
    if (Instr) {
      Instr->removeFromBundle();
      MBB.insert(InsertPoint, Instr);
    } else if (InsertMissingNops) {
      DebugLoc DL;
      Instr = BuildMI(MBB, InsertPoint, DL, TII->get(SlotInfo->getNOPOpcode()));
    }
    if (!BundleRoot)
      BundleRoot = Instr;
    else if (Instr)
      Instr->bundleWithPred();
  }
}
