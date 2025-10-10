//===-- AIEUnallocatedSuperRegRewriter.cpp - Constrain tied sub-registers -===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
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
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/CodeGen/VirtRegMap.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "aie-ra-prepare"

namespace {

using RegRewriteInfo = std::vector<std::pair<Register, SmallSet<int, 8>>>;

/// Split large unallocated compound registers into multiple new smaller vregs
/// Than can be allocated to scalar registers. This pass will handle registers
/// that were not allocated by Greedy so far. Currently, it is expected to
/// process registers used in copies created during Greedy's LR split.
/// Registers used in *2d or *3d instructions should be already allocated
/// at this point.
class AIEUnallocatedSuperRegRewriter : public MachineFunctionPass {

public:
  static char ID;
  AIEUnallocatedSuperRegRewriter() : MachineFunctionPass(ID) {}

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

/// Identify unallocated virtual registers that can be split into subregisters.
/// Returns a list of candidate registers with their rewritable subregister
/// indices, excluding unused registers and those already assigned to physical
/// registers.
static RegRewriteInfo getRewriteCandidates(MachineRegisterInfo &MRI,
                                           const AIEBaseRegisterInfo &TRI,
                                           VirtRegMap &VRM) {
  RegRewriteInfo RegistersToRewrite;
  for (unsigned VRegIdx = 0, End = MRI.getNumVirtRegs(); VRegIdx != End;
       ++VRegIdx) {
    const Register Reg = Register::index2VirtReg(VRegIdx);

    // Ignore un-used or already allocated registers.
    if (MRI.reg_nodbg_empty(Reg) || VRM.hasPhys(Reg))
      continue;

    const SmallSet<int, 8> RewritableSubRegs =
        AIESuperRegUtils::getRewritableSubRegs(Reg, MRI, TRI);

    if (RewritableSubRegs.empty())
      continue;

    LLVM_DEBUG(dbgs() << "Candidate " << printReg(Reg, &TRI, 0, &MRI) << ":"
                      << printRegClassOrBank(Reg, MRI, &TRI) << '\n');

    RegistersToRewrite.push_back({Reg, RewritableSubRegs});
  }

  LLVM_DEBUG(dbgs() << "Found " << RegistersToRewrite.size()
                    << " candidate register(s) for rewriting\n");

  return RegistersToRewrite;
}

/// Split candidate registers into independent virtual registers for each
/// subregister. Each composite register is rewritten using its subregister
/// indices, with live intervals and debug information updated accordingly.
void rewriteCandidates(RegRewriteInfo &RegistersToRewrite,
                       MachineRegisterInfo &MRI, const AIEBaseRegisterInfo &TRI,
                       VirtRegMap &VRM, LiveRegMatrix &LRM, LiveIntervals &LIS,
                       SlotIndexes &Indexes, LiveDebugVariables &DebugVars) {

  LLVM_DEBUG(dbgs() << "Rewriting " << RegistersToRewrite.size()
                    << " candidate register(s)\n");

  for (auto [VReg, SubRegs] : RegistersToRewrite) {
    LLVM_DEBUG(dbgs() << "  Rewriting " << printReg(VReg, &TRI, 0, &MRI)
                      << " into " << SubRegs.size() << " subregister(s)\n");
    std::optional<Register> NoPhysReg = {};
    AIESuperRegUtils::rewriteSuperReg(VReg, NoPhysReg, SubRegs, MRI, TRI, VRM,
                                      LRM, LIS, Indexes, DebugVars);
  }
}

bool AIEUnallocatedSuperRegRewriter::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(llvm::dbgs() << "*** Splitting unallocated super-registers: "
                          << MF.getName() << " ***\n");

  MachineRegisterInfo &MRI = MF.getRegInfo();
  VirtRegMap &VRM = getAnalysis<VirtRegMapWrapperLegacy>().getVRM();
  LiveRegMatrix &LRM = getAnalysis<LiveRegMatrixWrapperLegacy>().getLRM();
  LiveIntervals &LIS = getAnalysis<LiveIntervalsWrapperPass>().getLIS();
  SlotIndexes &Indexes = getAnalysis<SlotIndexesWrapperPass>().getSI();
  LiveDebugVariables &DebugVars =
      getAnalysis<LiveDebugVariablesWrapperLegacy>().getLDV();
  auto &TRI =
      *static_cast<const AIEBaseRegisterInfo *>(MRI.getTargetRegisterInfo());

  LLVM_DEBUG(dbgs() << "Identifying rewrite candidates...\n");
  RegRewriteInfo RegistersToRewrite = getRewriteCandidates(MRI, TRI, VRM);

  if (RegistersToRewrite.empty()) {
    LLVM_DEBUG(dbgs() << "No candidates found, skipping rewrite\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "Performing register rewrites...\n");
  rewriteCandidates(RegistersToRewrite, MRI, TRI, VRM, LRM, LIS, Indexes,
                    DebugVars);

  LLVM_DEBUG(dbgs() << "Successfully rewrote " << RegistersToRewrite.size()
                    << " register(s)\n");

  return !RegistersToRewrite.empty();
}

} // end anonymous namespace

char AIEUnallocatedSuperRegRewriter::ID = 0;
char &llvm::AIEUnallocatedSuperRegRewriterID =
    AIEUnallocatedSuperRegRewriter::ID;

INITIALIZE_PASS(AIEUnallocatedSuperRegRewriter,
                "aie-unallocated-superreg-rewrite",
                "AIE unallocated super-reg rewrite", false, false)

llvm::FunctionPass *llvm::createAIEUnallocatedSuperRegRewriter() {
  return new AIEUnallocatedSuperRegRewriter();
}
