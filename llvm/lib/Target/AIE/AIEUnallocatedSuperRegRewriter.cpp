//===-- AIEUnallocatedSuperRegRewriter.cpp - Constrain tied sub-registers -===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
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

struct RegRewriteInfo {
  // Registers that need bundle expansion (superset - includes rewritable)
  std::vector<std::pair<Register, SmallSet<int, 8>>> ExpandableRegs;

  // Registers that can be rewritten immediately (subset of ExpandableRegs)
  std::vector<std::pair<Register, SmallSet<int, 8>>> RewritableRegs;
};

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
/// Returns both expandable registers (those with copy bundles to expand) and
/// rewritable registers (subset that can be immediately rewritten).
/// Excludes unused registers and those already assigned to physical registers.
static RegRewriteInfo
getRewriteAndExpandCandidates(MachineRegisterInfo &MRI,
                              const AIEBaseRegisterInfo &TRI, VirtRegMap &VRM) {
  RegRewriteInfo Info;

  for (unsigned VRegIdx = 0, End = MRI.getNumVirtRegs(); VRegIdx != End;
       ++VRegIdx) {
    const Register Reg = Register::index2VirtReg(VRegIdx);

    // Ignore un-used or already allocated registers.
    if (MRI.reg_nodbg_empty(Reg) || VRM.hasPhys(Reg))
      continue;

    const SmallSet<int, 8> RewritableSubRegs =
        AIESuperRegUtils::getRewritableSubRegs(Reg, MRI, TRI);

    if (!RewritableSubRegs.empty()) {
      // Register can be rewritten immediately
      LLVM_DEBUG(dbgs() << "Candidate " << printReg(Reg, &TRI, 0, &MRI) << ":"
                        << printRegClassOrBank(Reg, MRI, &TRI) << '\n');
      Info.RewritableRegs.push_back({Reg, RewritableSubRegs});
      Info.ExpandableRegs.push_back({Reg, RewritableSubRegs});
    } else if (AIESuperRegUtils::isExpandableRegister(Reg, MRI, TRI)) {
      // Register is expandable but not immediately rewritable
      // It needs bundle expansion first
      auto &SubRegSplit = TRI.getSubRegSplit(MRI.getRegClass(Reg)->getID());
      SmallSet<int, 8> AllSubRegs(SubRegSplit.begin(), SubRegSplit.end());
      LLVM_DEBUG(dbgs() << "Expandable (not rewritable) "
                        << printReg(Reg, &TRI, 0, &MRI) << ":"
                        << printRegClassOrBank(Reg, MRI, &TRI) << '\n');
      Info.ExpandableRegs.push_back({Reg, AllSubRegs});
    }
  }

  LLVM_DEBUG(dbgs() << "Found " << Info.ExpandableRegs.size()
                    << " expandable register(s) (" << Info.RewritableRegs.size()
                    << " immediately rewritable)\n");

  return Info;
}

/// Split candidate registers into independent virtual registers for each
/// subregister. Each composite register is rewritten using its subregister
/// indices, with live intervals and debug information updated accordingly.
void rewriteCandidates(
    const std::vector<std::pair<Register, SmallSet<int, 8>>> &RewritableRegs,
    MachineRegisterInfo &MRI, const AIEBaseRegisterInfo &TRI, VirtRegMap &VRM,
    LiveRegMatrix &LRM, LiveIntervals &LIS, SlotIndexes &Indexes,
    LiveDebugVariables &DebugVars) {

  LLVM_DEBUG(dbgs() << "Rewriting " << RewritableRegs.size()
                    << " candidate register(s)\n");

  for (auto [VReg, SubRegs] : RewritableRegs) {
    LLVM_DEBUG(dbgs() << "  Rewriting " << printReg(VReg, &TRI, 0, &MRI)
                      << " into " << SubRegs.size() << " subregister(s)\n");
    std::optional<Register> NoPhysReg = {};
    AIESuperRegUtils::rewriteSuperReg(VReg, NoPhysReg, SubRegs, MRI, TRI, VRM,
                                      LRM, LIS, Indexes, DebugVars);
  }
}

/// Unbundle COPY/KILL instruction bundles for expandable registers.
/// Bundled instructions are separated into individual instructions with updated
/// slot indexes, and live intervals are repaired for affected registers.
static void expandCopyBundles(
    const std::vector<std::pair<Register, SmallSet<int, 8>>> &ExpandableRegs,
    MachineRegisterInfo &MRI, SlotIndexes &Indexes, LiveIntervals &LIS,
    VirtRegMap &VRM, LiveRegMatrix &LRM) {

  SmallSet<Register, 8> RegistersToRepair;
  for (auto [VReg, SubRegs] : ExpandableRegs) {

    for (MachineInstr &MI : MRI.reg_instructions(VReg)) {

      // Finding the last instruction in a COPY/KILL bundle (which has a
      // predecessor but no successor).
      if (!MI.isBundledWithPred() || MI.isBundledWithSucc())
        continue;

      SmallVector<MachineInstr *, 8> MIs({&MI});

      // Walking backwards through the bundle to collect all bundled
      // instructions.
      // Only do this when the complete bundle is made out of COPYs and KILLs.
      MachineBasicBlock &MBB = *MI.getParent();
      for (MachineBasicBlock::reverse_instr_iterator
               I = std::next(MI.getReverseIterator()),
               E = MBB.instr_rend();
           I != E && I->isBundledWithSucc(); ++I) {
        if (!I->isCopy() && !I->isKill())
          break;
        MIs.push_back(&*I);
      }

      // Unbundling them one by one from the end.
      MachineInstr *FirstMI = MIs.back();
      MachineInstr *BundleStart = FirstMI;
      for (MachineInstr *BundledMI : llvm::reverse(MIs)) {
        //  If instruction is in the middle of the bundle, move it before the
        //  bundle starts, otherwise, just unbundle it. When we get to the last
        //  instruction, the bundle will have been completely undone.
        if (BundledMI != BundleStart) {
          BundledMI->removeFromBundle();
          MBB.insert(BundleStart, BundledMI);
        } else if (BundledMI->isBundledWithSucc()) {
          BundledMI->unbundleFromSucc();
          BundleStart = &*std::next(BundledMI->getIterator());
        }

        if (BundledMI != FirstMI) {
          Indexes.insertMachineInstrInMaps(*BundledMI);
          RegistersToRepair.insert(BundledMI->getOperand(0).getReg());
          RegistersToRepair.insert(BundledMI->getOperand(1).getReg());
          BundledMI->getOperand(0).setIsInternalRead(false);
        }
      }
    }
  }

  AIESuperRegUtils::repairLiveIntervals(RegistersToRepair, VRM, LRM, LIS);
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

  LLVM_DEBUG(dbgs() << "Identifying rewrite and expand candidates...\n");
  RegRewriteInfo Info = getRewriteAndExpandCandidates(MRI, TRI, VRM);

  if (Info.ExpandableRegs.empty()) {
    LLVM_DEBUG(dbgs() << "No candidates found, skipping rewrite\n");
    return false;
  }

  // Snapshot Originals whose LI is about to go stale.
  SmallSet<Register, 8> TaintedOriginals;
  for (auto &P : Info.ExpandableRegs)
    TaintedOriginals.insert(VRM.getOriginal(P.first));
  for (auto &P : Info.RewritableRegs)
    TaintedOriginals.insert(VRM.getOriginal(P.first));

  LLVM_DEBUG(dbgs() << "Expanding copy bundles...\n");
  expandCopyBundles(Info.ExpandableRegs, MRI, Indexes, LIS, VRM, LRM);

  LLVM_DEBUG(dbgs() << "Performing register rewrites...\n");
  rewriteCandidates(Info.RewritableRegs, MRI, TRI, VRM, LRM, LIS, Indexes,
                    DebugVars);

  // Prevent SplitKit from rematerializing through stale ancestor LIs.
  AIESuperRegUtils::clearStaleSplitFromMappings(TaintedOriginals, MRI, VRM);

  LLVM_DEBUG(dbgs() << "Successfully rewrote " << Info.RewritableRegs.size()
                    << " register(s)\n");

  return !Info.RewritableRegs.empty();
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
