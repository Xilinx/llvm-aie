//===----- AIESuperRegRewriter.cpp - Constrain tied sub-registers ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"

#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/LiveStacks.h"
#include "llvm/CodeGen/MachineBlockFrequencyInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "aie-ra"

namespace {

/// Split large compound registers into multiple new smaller vregs.
/// This helps RA produce better spill code when needed.
/// The pass will update the \p VirtRegMap so that the new vregs have fixed
/// assignments, guaranteeing that they still belong to the same super-reg.
class AIESuperRegRewriter : public MachineFunctionPass {

public:
  static char ID;
  AIESuperRegRewriter() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addPreserved<MachineBlockFrequencyInfo>();
    AU.addRequired<VirtRegMap>();
    AU.addPreserved<VirtRegMap>();
    AU.addRequired<SlotIndexes>();
    AU.addPreserved<SlotIndexes>();
    AU.addRequired<LiveStacks>();
    AU.addPreserved<LiveStacks>();
    AU.addRequired<LiveIntervals>();
    AU.addPreserved<LiveIntervals>();
    AU.addRequired<LiveRegMatrix>();
    AU.addPreserved<LiveRegMatrix>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  void rewriteSuperReg(Register Reg, Register AssignedPhysReg,
                       MachineRegisterInfo &MRI, const AIEBaseRegisterInfo &TRI,
                       VirtRegMap &VRM, LiveRegMatrix &LRM, LiveIntervals &LIS,
                       SlotIndexes &Indexes);
};

/// Returns the subreg indices that can be used to rewrite \p Reg into smaller
/// regs. Returns {} if the rewrite isn't possible.
static SmallSet<int, 8> getRewritableSubRegs(Register Reg,
                                             const MachineRegisterInfo &MRI,
                                             const AIEBaseRegisterInfo &TRI) {
  auto &SubRegSplit = TRI.getSubRegSplit(MRI.getRegClass(Reg)->getID());
  SmallSet<int, 8> UsedSubRegs;
  for (MachineOperand &RegOp : MRI.reg_operands(Reg)) {
    int SubReg = RegOp.getSubReg();
    if (!SubReg || !SubRegSplit.count(SubReg)) {
      return {};
    }
    UsedSubRegs.insert(SubReg);
  }
  return UsedSubRegs;
}

bool AIESuperRegRewriter::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(llvm::dbgs() << "Splitting super-registers: " << MF.getName()
                          << "\n");

  MachineRegisterInfo &MRI = MF.getRegInfo();
  auto &TRI =
      *static_cast<const AIEBaseRegisterInfo *>(MRI.getTargetRegisterInfo());
  VirtRegMap &VRM = getAnalysis<VirtRegMap>();
  LiveRegMatrix &LRM = getAnalysis<LiveRegMatrix>();
  LiveIntervals &LIS = getAnalysis<LiveIntervals>();
  SlotIndexes &Indexes = getAnalysis<SlotIndexes>();
  std::map<Register, MCRegister> AssignedPhysRegs;

  // Collect already-assigned VRegs that can be split into smaller ones.
  for (unsigned VRegIdx = 0, End = MRI.getNumVirtRegs(); VRegIdx != End;
       ++VRegIdx) {
    Register Reg = Register::index2VirtReg(VRegIdx);

    // Ignore un-used registers and un-allocated registers
    if (MRI.reg_nodbg_empty(Reg) || !VRM.hasPhys(Reg))
      continue;

    // Skip vregs that are spilled, they would anyway be disregarded by
    // getRewritableSubRegs due to the spill instructions using the whole reg
    // without any subreg indices.
    if (VRM.getStackSlot(Reg) != VirtRegMap::NO_STACK_SLOT) {
      LLVM_DEBUG(dbgs() << "Skipping spilled register "
                        << printReg(Reg, &TRI, 0, &MRI) << '\n');
      continue;
    }

    LLVM_DEBUG(dbgs() << "Analysing " << printReg(Reg, &TRI, 0, &MRI) << ":"
                      << printRegClassOrBank(Reg, MRI, &TRI) << '\n');
    if (!getRewritableSubRegs(Reg, MRI, TRI).empty()) {
      AssignedPhysRegs[Reg] = VRM.getPhys(Reg);
      LRM.unassign(LIS.getInterval(Reg));
    } else {
      LLVM_DEBUG(dbgs() << "Could not rewrite " << printReg(Reg, &TRI, 0, &MRI)
                        << '\n');
    }
  }

  // Re-write all the collected VRegs
  for (auto &[VReg, PhysReg] : AssignedPhysRegs) {
    rewriteSuperReg(VReg, PhysReg, MRI, TRI, VRM, LRM, LIS, Indexes);
  }

  LLVM_DEBUG(VRM.dump());
  return !AssignedPhysRegs.empty();
}

void AIESuperRegRewriter::rewriteSuperReg(
    Register Reg, Register AssignedPhysReg, MachineRegisterInfo &MRI,
    const AIEBaseRegisterInfo &TRI, VirtRegMap &VRM, LiveRegMatrix &LRM,
    LiveIntervals &LIS, SlotIndexes &Indexes) {
  LLVM_DEBUG(dbgs() << "Rewriting " << printReg(Reg, &TRI, 0, &MRI) << '\n');
  auto *TII = static_cast<const AIEBaseInstrInfo *>(
      VRM.getMachineFunction().getSubtarget().getInstrInfo());

  // Collect all the subreg indices to rewrite as independent vregs.
  SmallMapVector<int, Register, 8> SubRegToVReg;
  const TargetRegisterClass *SuperRC = MRI.getRegClass(Reg);
  SmallSet<int, 8> SubRegs = getRewritableSubRegs(Reg, MRI, TRI);
  assert(!SubRegs.empty());
  for (int SubReg : SubRegs) {
    const TargetRegisterClass *SubRC = TRI.getSubRegisterClass(SuperRC, SubReg);
    SubRegToVReg[SubReg] = MRI.createVirtualRegister(SubRC);
  }

  LLVM_DEBUG(dbgs() << "  Splitting range " << LIS.getInterval(Reg) << "\n");
  for (MachineOperand &RegOp : make_early_inc_range(MRI.reg_operands(Reg))) {
    LLVM_DEBUG(dbgs() << "  Changing " << *RegOp.getParent());
    int SubReg = RegOp.getSubReg();
    assert(SubReg);
    RegOp.setReg(SubRegToVReg[SubReg]);
    RegOp.setSubReg(0);

    // There might have been a write-undef due to only writing one sub-lane.
    // Now that each sub-lane has its own VReg, the qualifier is invalid.
    if (RegOp.isDef())
      RegOp.setIsUndef(false);

    // Make sure the right reg class is applied, some MIs might use compound
    // classes with both 20 and 32 bits registers.
    const TargetRegisterClass *OpRC = TII->getRegClass(
        RegOp.getParent()->getDesc(), RegOp.getParent()->getOperandNo(&RegOp),
        &TRI, VRM.getMachineFunction());
    MRI.constrainRegClass(SubRegToVReg[SubReg], OpRC);

    LLVM_DEBUG(dbgs() << "        to " << *RegOp.getParent());
  }

  VRM.grow();
  LIS.removeInterval(Reg);

  for (auto &[SubRegIdx, VReg] : SubRegToVReg) {
    MCRegister SubPhysReg = TRI.getSubReg(AssignedPhysReg, SubRegIdx);
    LiveInterval &SubRegLI = LIS.getInterval(VReg);
    LLVM_DEBUG(dbgs() << "  Assigning Range: " << SubRegLI << '\n');

    // By giving an independent VReg to each lane, we might have created
    // multiple separate components. Give a VReg to each separate component.
    SmallVector<LiveInterval *, 4> LIComponents;
    LIS.splitSeparateComponents(SubRegLI, LIComponents);
    LIComponents.push_back(&SubRegLI);
    VRM.grow();

    for (LiveInterval *LI : LIComponents) {
      LRM.assign(*LI, SubPhysReg);
      VRM.setRequiredPhys(LI->reg(), SubPhysReg);
      LLVM_DEBUG(dbgs() << "  Assigned " << printReg(LI->reg()) << "\n");
    }
  }
}

} // end anonymous namespace

char AIESuperRegRewriter::ID = 0;
char &llvm::AIESuperRegRewriterID = AIESuperRegRewriter::ID;

INITIALIZE_PASS(AIESuperRegRewriter, "aie-superreg-rewrite",
                "AIE super-reg rewrite", false, false)

llvm::FunctionPass *llvm::createAIESuperRegRewriter() {
  return new AIESuperRegRewriter();
}
