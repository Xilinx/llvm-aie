//===-- AIESiblingLoopPreRAAligner.cpp - Pre-RA sibling loop alignment ----===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass aligns virtual register usage between sibling inner loops created
// by the Outer Loop Pipeliner (OLP), operating BEFORE register allocation.
//
// OLP creates a "steady" inner loop and a "peeled iteration" inner loop that
// execute the same code on mutually exclusive dynamic paths. After instruction
// selection the two loops have completely independent virtual registers, so the
// register allocator has no reason to assign them the same physical registers -
// leading to mismatches that hurt the post-pipeliner.
//
// This pass runs after PHI elimination and before register allocation. For each
// pair of corresponding virtual register definitions in the two loops it merges
// them into a single virtual register via MRI.replaceRegWith(). Since the two
// paths are mutually exclusive at runtime, the merged live ranges are disjoint
// and the allocator naturally assigns one physical register to both loops.
//
// Two categories of registers are aligned:
//   1. Inner-loop-local: defined and used exclusively within the inner loop.
//   2. Top-defined: defined in the region entry block and used only within
//      that block and the corresponding inner loop (e.g. pointer base regs).
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "Utils/AIELoopUtils.h"

#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/Debug.h"

#include <optional>
#include <vector>

using namespace llvm;

#define DEBUG_TYPE "aie-sibling-loop-pre-ra-align"

cl::opt<bool> EnableSiblingLoopPreRAAlign(
    "aie-enable-sibling-loop-pre-ra-align", cl::Hidden, cl::init(true),
    cl::desc("Merge sibling OLP loop registers before register allocation"));

namespace {

class AIESiblingLoopPreRAAligner : public MachineFunctionPass {
public:
  static char ID;
  AIESiblingLoopPreRAAligner() : MachineFunctionPass(ID) {}

  StringRef getPassName() const override {
    return "AIE sibling loop pre-RA register alignment";
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  /// Returns true if VReg is defined in DefMBB and all uses are within
  /// {DefMBB, ExtraMBB} (ExtraMBB may be null).
  bool isLocalTo(Register VReg, const MachineBasicBlock *DefMBB,
                 const MachineBasicBlock *ExtraMBB,
                 const MachineRegisterInfo &MRI) const;

  /// Merges PeeledReg into SteadyReg if their register classes are compatible.
  /// Returns true if the merge was performed.
  bool mergeIfCompatible(Register SteadyReg, Register PeeledReg,
                         MachineRegisterInfo &MRI) const;

  /// Align register pairs whose definitions are inside the inner loop body.
  unsigned alignInnerRegs(const AIELoopUtils::OuterLoopStructure &OLS,
                          MachineRegisterInfo &MRI) const;

  /// Align register pairs whose definitions are in the Top block.
  unsigned alignTopRegs(const AIELoopUtils::OuterLoopStructure &OLS,
                        MachineRegisterInfo &MRI) const;

  /// Core helper: iterate matching instruction pairs in two MBBs (lockstep)
  /// and merge corresponding def registers that pass the locality check.
  /// Assumes both blocks have structurally identical instruction sequences.
  unsigned alignMBBPair(const MachineBasicBlock *SteadyMBB,
                        const MachineBasicBlock *PeeledMBB,
                        const MachineBasicBlock *SteadyExtraMBB,
                        const MachineBasicBlock *PeeledExtraMBB,
                        MachineRegisterInfo &MRI) const;

  /// Like alignMBBPair but uses LCS (Longest Common Subsequence) matching
  /// on instruction opcodes. Handles blocks of different sizes correctly,
  /// skipping unmatched instructions in either block. Used for the Top blocks
  /// which may have different instruction counts across the two loop copies.
  unsigned alignMBBPairLCS(const MachineBasicBlock *SteadyMBB,
                           const MachineBasicBlock *PeeledMBB,
                           const MachineBasicBlock *SteadyExtraMBB,
                           const MachineBasicBlock *PeeledExtraMBB,
                           MachineRegisterInfo &MRI) const;
};

char AIESiblingLoopPreRAAligner::ID = 0;

bool AIESiblingLoopPreRAAligner::isLocalTo(
    Register VReg, const MachineBasicBlock *DefMBB,
    const MachineBasicBlock *ExtraMBB, const MachineRegisterInfo &MRI) const {
  if (!VReg.isVirtual())
    return false;

  // After PHI elimination a vreg may have multiple defs; require all in DefMBB.
  for (const MachineOperand &DefMO : MRI.def_operands(VReg))
    if (DefMO.getParent()->getParent() != DefMBB)
      return false;

  // All uses must be within {DefMBB, ExtraMBB}.
  for (const MachineOperand &UseMO : MRI.use_nodbg_operands(VReg)) {
    const MachineBasicBlock *const UseMBB = UseMO.getParent()->getParent();
    if (UseMBB != DefMBB && UseMBB != ExtraMBB)
      return false;
  }

  return true;
}

bool AIESiblingLoopPreRAAligner::mergeIfCompatible(
    Register SteadyReg, Register PeeledReg, MachineRegisterInfo &MRI) const {
  if (!SteadyReg.isVirtual() || !PeeledReg.isVirtual())
    return false;
  if (SteadyReg == PeeledReg)
    return false;

  const TargetRegisterClass *const RC =
      MRI.constrainRegClass(SteadyReg, MRI.getRegClass(PeeledReg));
  if (!RC) {
    LLVM_DEBUG(dbgs() << "  Skipping " << printReg(SteadyReg) << " / "
                      << printReg(PeeledReg)
                      << ": incompatible register classes\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "  Merging " << printReg(PeeledReg) << " -> "
                    << printReg(SteadyReg) << "\n");
  MRI.replaceRegWith(PeeledReg, SteadyReg);
  return true;
}

unsigned AIESiblingLoopPreRAAligner::alignMBBPair(
    const MachineBasicBlock *SteadyMBB, const MachineBasicBlock *PeeledMBB,
    const MachineBasicBlock *SteadyExtraMBB,
    const MachineBasicBlock *PeeledExtraMBB, MachineRegisterInfo &MRI) const {
  unsigned Count = 0;

  auto SI = SteadyMBB->begin();
  auto PI = PeeledMBB->begin();
  for (; SI != SteadyMBB->end() && PI != PeeledMBB->end(); ++SI, ++PI) {
    if (SI->getOpcode() != PI->getOpcode())
      continue;

    const unsigned NumDefs = SI->getNumDefs();
    for (unsigned I = 0; I < NumDefs; ++I) {
      const MachineOperand &SOp = SI->getOperand(I);
      const MachineOperand &POp = PI->getOperand(I);
      if (!SOp.isReg() || !POp.isReg())
        continue;

      const Register SteadyReg = SOp.getReg();
      const Register PeeledReg = POp.getReg();

      if (!isLocalTo(SteadyReg, SteadyMBB, SteadyExtraMBB, MRI))
        continue;
      if (!isLocalTo(PeeledReg, PeeledMBB, PeeledExtraMBB, MRI))
        continue;

      if (mergeIfCompatible(SteadyReg, PeeledReg, MRI))
        ++Count;
    }
  }

  return Count;
}

unsigned AIESiblingLoopPreRAAligner::alignMBBPairLCS(
    const MachineBasicBlock *SteadyMBB, const MachineBasicBlock *PeeledMBB,
    const MachineBasicBlock *SteadyExtraMBB,
    const MachineBasicBlock *PeeledExtraMBB, MachineRegisterInfo &MRI) const {

  // Collect instructions into random-access arrays.
  SmallVector<const MachineInstr *> SteadyInstrs, PeeledInstrs;
  for (const MachineInstr &MI : *SteadyMBB)
    SteadyInstrs.push_back(&MI);
  for (const MachineInstr &MI : *PeeledMBB)
    PeeledInstrs.push_back(&MI);

  const int N = SteadyInstrs.size();
  const int M = PeeledInstrs.size();

  if (N == 0 || M == 0)
    return 0;

  // dp[i][j] = LCS length of SteadyInstrs[0..i-1] and PeeledInstrs[0..j-1],
  // using same opcode as the match criterion.
  std::vector<std::vector<int>> DP(N + 1, std::vector<int>(M + 1, 0));
  for (int I = 1; I <= N; ++I)
    for (int J = 1; J <= M; ++J)
      if (SteadyInstrs[I - 1]->getOpcode() == PeeledInstrs[J - 1]->getOpcode())
        DP[I][J] = DP[I - 1][J - 1] + 1;
      else
        DP[I][J] = std::max(DP[I - 1][J], DP[I][J - 1]);

  // Backtrack to recover matched pairs (in reverse order, which does not
  // affect the merge since each pair involves independent registers).
  SmallVector<std::pair<const MachineInstr *, const MachineInstr *>> Matches;
  int I = N, J = M;
  while (I > 0 && J > 0) {
    if (SteadyInstrs[I - 1]->getOpcode() == PeeledInstrs[J - 1]->getOpcode() &&
        DP[I][J] == DP[I - 1][J - 1] + 1) {
      Matches.push_back({SteadyInstrs[I - 1], PeeledInstrs[J - 1]});
      --I;
      --J;
    } else if (DP[I - 1][J] >= DP[I][J - 1]) {
      --I;
    } else {
      --J;
    }
  }

  LLVM_DEBUG({
    dbgs() << "  [LCS] Matched " << Matches.size() << " instruction pairs"
           << " (Steady=" << N << ", Peeled=" << M << ")\n";
  });

  // For each matched pair, merge compatible def registers.
  unsigned Count = 0;
  for (auto &[SI, PI] : Matches) {
    const unsigned NumDefs = SI->getNumDefs();
    for (unsigned K = 0; K < NumDefs; ++K) {
      const MachineOperand &SOp = SI->getOperand(K);
      const MachineOperand &POp = PI->getOperand(K);
      if (!SOp.isReg() || !POp.isReg())
        continue;

      const Register SteadyReg = SOp.getReg();
      const Register PeeledReg = POp.getReg();

      if (!isLocalTo(SteadyReg, SteadyMBB, SteadyExtraMBB, MRI))
        continue;
      if (!isLocalTo(PeeledReg, PeeledMBB, PeeledExtraMBB, MRI))
        continue;

      if (mergeIfCompatible(SteadyReg, PeeledReg, MRI))
        ++Count;
    }
  }

  return Count;
}

/// Searches \p TopBlock for a COPY of the form "%dst:DstRC = COPY %Src".
/// Returns the destination register if found (with a unique def, as an
/// out-of-SSA safety guard), or std::nullopt otherwise.
static std::optional<Register>
findEquivalentCopyIn(const MachineBasicBlock &TopBlock, Register Src,
                     const TargetRegisterClass *DstRC,
                     MachineRegisterInfo &MRI) {
  for (const MachineInstr &MI : TopBlock) {
    if (!MI.isCopy())
      continue;
    const Register TopDst = MI.getOperand(0).getReg();
    const Register TopSrc = MI.getOperand(1).getReg();
    if (TopSrc != Src || !TopDst.isVirtual())
      continue;
    if (MRI.getRegClass(TopDst) != DstRC)
      continue;
    // Skip if phi-elim introduced multiple defs for this vreg.
    if (!MRI.getUniqueVRegDef(TopDst))
      continue;
    return TopDst;
  }
  return std::nullopt;
}

/// Returns true if \p MI is a pure constant materializer: it defines exactly
/// one virtual register, has no register USE operands (all non-def operands
/// are immediates), and has no observable side effects. Such instructions
/// compute the same value whenever their immediate operands are equal.
static bool isPureConstantDef(const MachineInstr &MI) {
  if (MI.getNumDefs() != 1)
    return false;
  if (!MI.getOperand(0).isReg() || !MI.getOperand(0).getReg().isVirtual())
    return false;
  if (MI.mayLoad() || MI.mayStore() || MI.hasUnmodeledSideEffects() ||
      MI.isCall())
    return false;
  for (unsigned I = MI.getNumDefs(), E = MI.getNumOperands(); I < E; ++I)
    if (MI.getOperand(I).isReg())
      return false; // Has register uses — not a pure constant.
  return true;
}

/// Searches \p TopBlock for a pure constant materializer instruction with the
/// same opcode and identical non-def immediate operands as \p Template.
/// Returns the def register if found (with a unique def), or std::nullopt.
static std::optional<Register>
findEquivalentConstantIn(const MachineBasicBlock &TopBlock,
                         const MachineInstr &Template,
                         MachineRegisterInfo &MRI) {
  for (const MachineInstr &MI : TopBlock) {
    if (MI.getOpcode() != Template.getOpcode())
      continue;
    if (MI.getNumOperands() != Template.getNumOperands())
      continue;
    const Register TopDef = MI.getOperand(0).getReg();
    if (!TopDef.isVirtual() || !MRI.getUniqueVRegDef(TopDef))
      continue;
    // All non-def operands must be identical (same immediate values).
    bool Match = true;
    for (unsigned I = MI.getNumDefs(), E = MI.getNumOperands(); I < E; ++I) {
      if (!MI.getOperand(I).isIdenticalTo(Template.getOperand(I))) {
        Match = false;
        break;
      }
    }
    if (Match)
      return TopDef;
  }
  return std::nullopt;
}

/// For each instruction in \p MBB that is either:
///   - an invariant COPY "%dst = COPY %src" (source defined outside MBB), or
///   - a pure constant materializer (e.g. "%dst = MOV_imm 63") that is
///     already computed by an equivalent instruction in \p TopBlock,
/// replace uses of \p dst within \p MBB with the TopBlock equivalent and
/// erase the redundant instruction.
static void substituteEquivalentDefs(MachineBasicBlock *MBB,
                                     const MachineBasicBlock *TopBlock,
                                     MachineRegisterInfo &MRI) {
  for (auto I = MBB->begin(), E = MBB->end(); I != E;) {
    MachineInstr &MI = *I++;
    std::optional<Register> Replacement;
    Register Dst;

    if (MI.isCopy()) {
      Dst = MI.getOperand(0).getReg();
      const Register Src = MI.getOperand(1).getReg();
      if (!Dst.isVirtual() || !Src.isVirtual())
        continue;
      // Only handle invariant COPYs: source defined outside MBB.
      const MachineInstr *const SrcDef = MRI.getUniqueVRegDef(Src);
      if (!SrcDef || SrcDef->getParent() == MBB)
        continue;
      const TargetRegisterClass *const DstRC = MRI.getRegClass(Dst);
      Replacement = findEquivalentCopyIn(*TopBlock, Src, DstRC, MRI);
      if (!Replacement)
        continue;
      LLVM_DEBUG(dbgs() << "  [SubstCopy] Replacing " << printReg(Dst)
                        << " with " << printReg(*Replacement) << " in "
                        << MBB->getName() << "\n");
    } else if (isPureConstantDef(MI)) {
      Dst = MI.getOperand(0).getReg();
      if (!MRI.getUniqueVRegDef(Dst))
        continue;
      Replacement = findEquivalentConstantIn(*TopBlock, MI, MRI);
      if (!Replacement)
        continue;
      // Guard against incompatible register classes.
      if (!MRI.constrainRegClass(*Replacement, MRI.getRegClass(Dst)))
        continue;
      LLVM_DEBUG(dbgs() << "  [SubstConst] Replacing " << printReg(Dst)
                        << " with " << printReg(*Replacement) << " in "
                        << MBB->getName() << "\n");
    } else {
      continue;
    }

    // Replace ALL uses — the TopBlock dominates MBB and all blocks that
    // could use Dst, so the replacement register is always available.
    for (MachineOperand &UseMO :
         llvm::make_early_inc_range(MRI.use_operands(Dst)))
      UseMO.setReg(*Replacement);

    MI.eraseFromParent();
  }
}

unsigned AIESiblingLoopPreRAAligner::alignInnerRegs(
    const AIELoopUtils::OuterLoopStructure &OLS,
    MachineRegisterInfo &MRI) const {
  // Normalise both inner loops by substituting invariant COPYs with already-
  // computed equivalents from a dominating block (outer preheader first, then
  // SteadyTop). This ensures hasMatchingInnerLoops() can succeed and that both
  // loops reference the same vreg for loop-invariant values.
  for (MachineBasicBlock *Dom : {OLS.OuterPreheader, OLS.SteadyTop}) {
    substituteEquivalentDefs(OLS.SteadyInner, Dom, MRI);
    substituteEquivalentDefs(OLS.PeeledIterInner, Dom, MRI);
  }

  if (!OLS.hasMatchingInnerLoops()) {
    LLVM_DEBUG(dbgs() << "  [Inner] Skipping alignment: inner loops do not "
                         "structurally match\n");
    return 0;
  }

  LLVM_DEBUG(dbgs() << "  [Inner] Aligning " << OLS.SteadyInner->getName()
                    << " <-> " << OLS.PeeledIterInner->getName() << "\n");

  return alignMBBPair(OLS.SteadyInner, OLS.PeeledIterInner,
                      /*SteadyExtraMBB=*/nullptr,
                      /*PeeledExtraMBB=*/nullptr, MRI);
}

unsigned AIESiblingLoopPreRAAligner::alignTopRegs(
    const AIELoopUtils::OuterLoopStructure &OLS,
    MachineRegisterInfo &MRI) const {
  LLVM_DEBUG(dbgs() << "  [Top] Aligning " << OLS.SteadyTop->getName()
                    << " <-> " << OLS.PeeledIterTop->getName() << "\n");

  // Normalise the peeled Top block by substituting constants already
  // available from the outer preheader. LICM handles the steady side
  // automatically; the peeled copy may still have redundant materializers.
  substituteEquivalentDefs(OLS.PeeledIterTop, OLS.OuterPreheader, MRI);

  // Use LCS (Longest Common Subsequence) matching rather than a simple
  // lockstep walk because the two Top blocks may have different instruction
  // counts. OLP copies only the essential inner-loop body into the peeled
  // iteration, while the steady Top block may carry additional instructions
  // for pipeline management (extra stage loads, post-amble addresses, etc.)
  // that have no counterpart in the peeled Top. A lockstep walk would
  // desynchronise at the first size mismatch and miss valid register pairs.
  // LCS finds the largest set of opcode-matching instruction pairs in order,
  // skipping unmatched instructions on either side, and then merges the
  // corresponding def registers so the allocator assigns them one physical
  // register across both execution paths.
  return alignMBBPairLCS(OLS.SteadyTop, OLS.PeeledIterTop,
                         /*SteadyExtraMBB=*/OLS.SteadyInner,
                         /*PeeledExtraMBB=*/OLS.PeeledIterInner, MRI);
}

bool AIESiblingLoopPreRAAligner::runOnMachineFunction(MachineFunction &MF) {
  if (!EnableSiblingLoopPreRAAlign)
    return false;

  LLVM_DEBUG(dbgs() << "*** Sibling Loop Pre-RA Register Alignment: "
                    << MF.getName() << " ***\n");

  MachineRegisterInfo &MRI = MF.getRegInfo();
  bool Modified = false;

  for (MachineBasicBlock &MBB : MF) {
    if (!AIELoopUtils::isOuterLoopPipelined(MBB))
      continue;

    const auto OLS = AIELoopUtils::OuterLoopStructure::tryBuildFrom(MBB);
    if (!OLS) {
      LLVM_DEBUG(dbgs() << "Failed to build OLP structure from "
                        << MBB.getName() << "\n");
      continue;
    }
    if (!OLS->hasPeeledIterRegion()) {
      LLVM_DEBUG(dbgs() << "Skipping speculative OLP (no peeled region)\n");
      continue;
    }

    LLVM_DEBUG(dbgs() << "Processing OLP structure rooted at " << MBB.getName()
                      << "\n");

    const unsigned InnerCount = alignInnerRegs(*OLS, MRI);
    const unsigned TopCount = alignTopRegs(*OLS, MRI);
    const unsigned Total = InnerCount + TopCount;

    LLVM_DEBUG(dbgs() << "  Merged " << InnerCount << " inner-local + "
                      << TopCount << " top-defined = " << Total
                      << " register pairs\n");

    Modified |= (Total > 0);
  }

  return Modified;
}

} // end anonymous namespace

char &llvm::AIESiblingLoopPreRAAlignerID = AIESiblingLoopPreRAAligner::ID;

INITIALIZE_PASS(AIESiblingLoopPreRAAligner, DEBUG_TYPE,
                "AIE sibling loop pre-RA register alignment", false, false)

FunctionPass *llvm::createAIESiblingLoopPreRAAligner() {
  return new AIESiblingLoopPreRAAligner();
}
