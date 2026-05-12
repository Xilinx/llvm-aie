//===----- AIESuperRegRewriter.cpp - Constrain tied sub-registers ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "AIESuperRegUtils.h"

#include "llvm/ADT/MapVector.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
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
#include "llvm/CodeGen/VirtRegMap.h"
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
    AU.addPreserved<MachineBlockFrequencyInfoWrapperPass>();
    AU.addRequired<VirtRegMapWrapperLegacy>();
    AU.addPreserved<VirtRegMapWrapperLegacy>();
    AU.addRequired<SlotIndexesWrapperPass>();
    AU.addPreserved<SlotIndexesWrapperPass>();
    AU.addRequired<LiveDebugVariablesWrapperLegacy>();
    AU.addPreserved<LiveDebugVariablesWrapperLegacy>();
    AU.addRequired<LiveStacksWrapperLegacy>();
    AU.addPreserved<LiveStacksWrapperLegacy>();
    AU.addRequired<LiveIntervalsWrapperPass>();
    AU.addPreserved<LiveIntervalsWrapperPass>();
    AU.addRequired<LiveRegMatrixWrapperLegacy>();
    AU.addPreserved<LiveRegMatrixWrapperLegacy>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;
};

bool AIESuperRegRewriter::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(llvm::dbgs() << "*** Splitting super-registers: " << MF.getName()
                          << " ***\n");

  MachineRegisterInfo &MRI = MF.getRegInfo();
  auto &TRI =
      *static_cast<const AIEBaseRegisterInfo *>(MRI.getTargetRegisterInfo());
  VirtRegMap &VRM = getAnalysis<VirtRegMapWrapperLegacy>().getVRM();
  LiveRegMatrix &LRM = getAnalysis<LiveRegMatrixWrapperLegacy>().getLRM();
  LiveIntervals &LIS = getAnalysis<LiveIntervalsWrapperPass>().getLIS();
  SlotIndexes &Indexes = getAnalysis<SlotIndexesWrapperPass>().getSI();
  LiveDebugVariables &DebugVars = getAnalysis<LiveDebugVariablesWrapperLegacy>().getLDV();
  std::map<Register, std::pair<MCRegister, SmallSet<int, 8>>> AssignedPhysRegs;

  // Collect already-assigned VRegs that can be split into smaller ones.
  LLVM_DEBUG(VRM.dump());
  LLVM_DEBUG(LIS.dump());
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
    SmallSet<int, 8> RewritableSubRegs =
        AIESuperRegUtils::getRewritableSubRegs(Reg, MRI, TRI);
    if (!RewritableSubRegs.empty()) {
      AssignedPhysRegs[Reg] =
          std::make_pair(VRM.getPhys(Reg), RewritableSubRegs);
      LRM.unassign(LIS.getInterval(Reg));
    } else {
      LLVM_DEBUG(dbgs() << "Could not rewrite " << printReg(Reg, &TRI, 0, &MRI)
                        << '\n');
    }
  }

  // Snapshot Originals whose LI is about to go stale.
  SmallSet<Register, 8> TaintedOriginals;
  for (auto &[VReg, _] : AssignedPhysRegs)
    TaintedOriginals.insert(VRM.getOriginal(VReg));

  // Re-write all the collected VRegs
  for (auto &[VReg, PhysRegAndSubRegs] : AssignedPhysRegs) {
    const Register PhysReg = PhysRegAndSubRegs.first;
    SmallSet<int, 8> &SubRegs = PhysRegAndSubRegs.second;
    AIESuperRegUtils::rewriteSuperReg(VReg, PhysReg, SubRegs, MRI, TRI, VRM,
                                      LRM, LIS, Indexes, DebugVars);
  }

  // Prevent SplitKit from rematerializing through stale ancestor LIs.
  AIESuperRegUtils::clearStaleSplitFromMappings(TaintedOriginals, MF, MRI, VRM);

  LLVM_DEBUG(VRM.dump());
  return !AssignedPhysRegs.empty();
}

} // end anonymous namespace

char AIESuperRegRewriter::ID = 0;
char &llvm::AIESuperRegRewriterID = AIESuperRegRewriter::ID;

INITIALIZE_PASS(AIESuperRegRewriter, "aie-superreg-rewrite",
                "AIE super-reg rewrite", false, false)

llvm::FunctionPass *llvm::createAIESuperRegRewriter() {
  return new AIESuperRegRewriter();
}
