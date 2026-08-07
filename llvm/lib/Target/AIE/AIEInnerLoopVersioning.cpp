//===- AIEInnerLoopVersioning.cpp - Inner loop versioning for AIE ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Inner loop versioning for AIE.
//
// A loop carrying the llvm.loop.hint.aie-loop-versioning hint is split into two
// copies guarded by a runtime trip-count check. Large trip counts run a
// specialized copy the postpipeliner may pipeline aggressively, while small
// trip counts stay correct on the original loop. The block structure below is
// what the postpipeliner's guard finder (AIELoopUtils::getVersionGuardBlock)
// relies on: a guard block with two successors, each a dedicated fallthrough
// preheader of its copy. (The block names are for humans; the guard finder
// matches on structure, not names.)
//
//   [<hdr>.lver.guard]   if (trip < threshold) -> low else -> high
//          /     \
//   [<hdr>.ph]  [<hdr>.ph.lver.high]
//       |               |
//    [<hdr>]        [<hdr>.lver.high]   the high-trip-count copy is the clone;
//       \               /               it keeps the hint and gets pipelined.
//        \             /                the low-trip-count copy is the original
//         \           /                 loop, kept verbatim as the fallback.
//          [exit]
//
// The threshold is produced by a thin per-subtarget intrinsic that lowers to a
// non-CSE-able pseudo, giving the postpipeliner a stable handle to patch. After
// scheduling, the postpipeliner overwrites the placeholder with the required
// stage count, so the high-trip-count copy runs exactly when the trip count is
// large enough for its schedule.
//
// The placeholder is UINT32_MAX (-1): the guard compare is unsigned, so until
// the postpipeliner patches it, every trip count routes to the low-trip-count
// (verbatim, un-pipelined) copy. This fails safe -- if the guard is never
// patched (the high-trip-count copy is not pipelined, or updateVersionGuard
// bails on a reshaped region), the pipelined copy is simply never entered.
//
// Running before HardwareLoops lets both copies lower to ZOL uniformly.
//
// Preconditions, per candidate loop, in the order they are checked:
//  - it carries the versioning request hint (a positive integer value);
//  - it is innermost, so versioning it cannot clone another hinted loop;
//  - its trip count is computable by SCEV and fits i32;
//  - it reaches simplify + LCSSA form via on-demand canonicalization, with a
//    unique exiting block and a unique exit block;
//  - that trip count is expandable into the preheader.
// A loop failing one of the first three is left untouched, hint included. A
// loop failing a later one may have been canonicalized, but is never versioned
// and keeps its hint.
//
// Postconditions, per versioned loop:
//  - the guard shape drawn above, both copies in simplify + LCSSA form with
//    their own dedicated exit block;
//  - the request hint is gone from both copies, and only the high-trip-count
//    copy carries the versioned marker;
//  - the high-trip-count copy has no llvm.loop.itercount.range.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "Utils/AIEIRUtils.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/OptimizationRemarkEmitter.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/InitializePasses.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

using namespace llvm;

#define DEBUG_TYPE "aie-inner-loop-versioning"

namespace llvm {
cl::opt<bool> DisableInnerLoopVersioning(
    "aie-disable-inner-loop-versioning", cl::Hidden, cl::init(false),
    cl::desc("Do not run inner-loop versioning, overriding any loop pragma "
             "(the pass is left out of the pipeline entirely)"));
} // namespace llvm

static cl::opt<int> VersioningMinIterCount(
    "aie-inner-loop-versioning-min-itercount", cl::Hidden, cl::init(-1),
    cl::desc("Version every innermost loop whose known minimum iteration count "
             "is at most this value, ignoring the loop pragma (-1 disables). "
             "Loops without an iteration-count range are not affected"));

namespace {

class AIEInnerLoopVersioning : public FunctionPass {
public:
  static char ID;
  AIEInnerLoopVersioning() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    // We deliberately do not force LoopSimplify/LCSSA: that canonicalization
    // would perturb the IR seen by later passes (e.g. the outer-loop
    // pipeliner) even for functions with no versioning hint. Hinted candidates
    // are canonicalized one at a time instead, in
    // AIELoopVersioner::canonicalizeAndCheckStructure.
    AU.addRequired<LoopInfoWrapperPass>();
    AU.addRequired<DominatorTreeWrapperPass>();
    AU.addRequired<ScalarEvolutionWrapperPass>();
    AU.addRequired<OptimizationRemarkEmitterWrapperPass>();
    FunctionPass::getAnalysisUsage(AU);
  }

  StringRef getPassName() const override { return "AIE Inner Loop Versioning"; }
};

/// Versions a single loop: clone it into a high-trip-count copy, route a
/// runtime trip-count guard to either copy, and fix up metadata. One instance
/// is constructed per candidate loop; call tryVersionLoop() to run the
/// transform.
class AIELoopVersioner {
public:
  /// What tryVersionLoop() did to the loop. Canonicalized and Versioned both
  /// mean the IR changed; only Unchanged lets the caller keep its analyses.
  enum class Result { Unchanged, Canonicalized, Versioned };

  AIELoopVersioner(Loop &L, DominatorTree &DT, LoopInfo &LI,
                   ScalarEvolution &SE, OptimizationRemarkEmitter &ORE)
      : L(L), DT(DT), LI(LI), SE(SE), ORE(ORE) {}

  /// Version the loop. Bailing out after canonicalization leaves the loop in
  /// simplify + LCSSA form.
  Result tryVersionLoop();

private:
  Loop &L;
  DominatorTree &DT;
  LoopInfo &LI;
  ScalarEvolution &SE;
  OptimizationRemarkEmitter &ORE;

  /// Set when canonicalizeAndCheckStructure() actually rewrote the loop, so a
  /// later bail can still report the IR as changed.
  bool Canonicalized = false;

  /// Whether the trip count can be computed and held by the runtime guard.
  /// Read-only: decided solely from the trip-count SCEV.
  bool hasVersionableTripCount() const;
  /// Bring the loop into simplify + LCSSA form and check its structure supports
  /// versioning. Returns the unique exit block on success, or nullptr if the
  /// loop is unsuitable. Mutates the loop either way.
  BasicBlock *canonicalizeAndCheckStructure();
  /// Materialize the loop trip count at InsertPt, or nullptr if it cannot be
  /// computed / expanded there.
  Value *expandTripCount(Instruction *InsertPt) const;
  /// Emit the guard condition into \p GuardBB, which it also renames: true
  /// when \p TripCount is below the placeholder threshold, i.e. when the
  /// low-trip-count copy must run.
  Value *emitGuardCondition(BasicBlock &GuardBB, Value *TripCount) const;
  /// Merge loop-defined values used after the loop across the two copies, by
  /// giving every exit PHI its incoming value from \p ClonedLoop.
  void addExitPHIs(Loop *ClonedLoop, BasicBlock *ExitBlock,
                   ValueToValueMapTy &VMap) const;
  /// Consume the request hint on \p LowLoop and \p HighLoop and mark
  /// \p HighLoop as the versioned one, so the postpipeliner can tell the
  /// copies apart.
  void updateLoopsMetadata(Loop &LowLoop, Loop &HighLoop);
  /// Remove the versioning request hint from \p Loop.
  void stripVersioningHint(Loop &Loop) const;
};

/// True when \p L carries the iteration-count versioning request hint.
bool isIterCountVersioningEnabled(const Loop *L) {
  std::optional<int64_t> Hint = AIELoopUtils::getLoopHintInt(
      L->getLoopID(), AIELoopUtils::LoopVersioningHintKey);
  return Hint && *Hint > 0;
}

/// True when the command line requests \p L to be versioned without a pragma,
/// i.e. its known minimum iteration count is at most the requested bound.
bool isIterCountVersioningRequestedByOption(const Loop *L) {
  if (VersioningMinIterCount < 0)
    return false;
  const std::optional<int64_t> MinIterCount = getMinTripCount(L->getLoopID());
  return MinIterCount && *MinIterCount <= VersioningMinIterCount;
}

/// True when \p L is a copy an earlier run of the pass already produced.
/// Versioning consumes the request hint, so this only matters for loops picked
/// up by VersioningMinIterCount, which ignores hints.
bool isAlreadyVersioned(const Loop *L) {
  return AIELoopUtils::getLoopHintInt(L->getLoopID(),
                                      AIELoopUtils::LoopVersionedHintKey)
      .has_value();
}

/// Report that \p Reason kept \p L un-versioned, so a user who set the pragma
/// learns it had no effect. Shared by the candidate filter and the versioner
/// so every rejection reads the same and carries the same Reason field.
void remarkNotVersioned(OptimizationRemarkEmitter &ORE, const Loop &L,
                        StringRef RemarkName, StringRef Reason) {
  ORE.emit([&] {
    return OptimizationRemarkMissed(DEBUG_TYPE, RemarkName, L.getStartLoc(),
                                    L.getHeader())
           << "loop not versioned because " << ore::NV("Reason", Reason);
  });
}

} // namespace

char AIEInnerLoopVersioning::ID = 0;
char &llvm::AIEInnerLoopVersioningID = AIEInnerLoopVersioning::ID;

INITIALIZE_PASS_BEGIN(AIEInnerLoopVersioning, DEBUG_TYPE,
                      "AIE Inner Loop Versioning", false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_DEPENDENCY(ScalarEvolutionWrapperPass)
INITIALIZE_PASS_DEPENDENCY(OptimizationRemarkEmitterWrapperPass)
INITIALIZE_PASS_END(AIEInnerLoopVersioning, DEBUG_TYPE,
                    "AIE Inner Loop Versioning", false, false)

llvm::FunctionPass *llvm::createAIEInnerLoopVersioningPass() {
  return new AIEInnerLoopVersioning();
}

bool AIEInnerLoopVersioning::runOnFunction(Function &F) {
  if (skipFunction(F))
    return false;

  Triple TT(F.getParent()->getTargetTriple());
  Intrinsic::ID ThresholdIID = AIEIRUtils::getLoopVersionThresholdIntrinsic(TT);
  if (ThresholdIID == Intrinsic::not_intrinsic)
    return false;

  auto &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  auto &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  auto &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
  auto &ORE = getAnalysis<OptimizationRemarkEmitterWrapperPass>().getORE();

  LLVM_DEBUG(dbgs() << "AIEInnerLoopVersioning: " << F.getName() << "\n");

  // Collect enabled loops before mutating, so cloning does not disturb
  // iteration.
  SmallVector<Loop *, 4> Candidates;
  for (Loop *L : LI.getLoopsInPreorder()) {
    if (!isIterCountVersioningEnabled(L) &&
        !isIterCountVersioningRequestedByOption(L))
      continue;

    if (isAlreadyVersioned(L)) {
      LLVM_DEBUG(dbgs() << "  Skipping already versioned loop ";
                 L->getHeader()->printAsOperand(dbgs()); dbgs() << "\n");
      continue;
    }

    if (!L->isInnermost()) {
      LLVM_DEBUG(dbgs() << "  Skipping non-innermost loop ";
                 L->getHeader()->printAsOperand(dbgs()); dbgs() << "\n");
      remarkNotVersioned(ORE, *L, "NotInnermost", "it is not innermost");
      continue;
    }
    Candidates.push_back(L);
  }

  bool Changed = false;
  for (Loop *L : Candidates)
    Changed |= AIELoopVersioner(*L, DT, LI, SE, ORE).tryVersionLoop() !=
               AIELoopVersioner::Result::Unchanged;
  return Changed;
}

Value *AIELoopVersioner::expandTripCount(Instruction *InsertPt) const {
  // hasVersionableTripCount() already guaranteed the trip count is computable
  // and fits i32.
  const SCEV *BEC = SE.getBackedgeTakenCount(&L);
  const SCEV *TC = SE.getTripCountFromExitCount(BEC);
  SCEVExpander Exp(SE, InsertPt->getDataLayout(), "lver.tc");
  if (!Exp.isSafeToExpandAt(TC, InsertPt))
    return nullptr;
  return Exp.expandCodeFor(TC, TC->getType(), InsertPt);
}

bool AIELoopVersioner::hasVersionableTripCount() const {
  // The runtime guard compares the trip count in i32, and the pipelined
  // high-trip-count copy can only become a zero-overhead loop if its trip count
  // fits in a 32-bit register (see AIETTICommon::isHardwareLoopProfitable).
  const SCEV *BEC = SE.getBackedgeTakenCount(&L);
  if (isa<SCEVCouldNotCompute>(BEC))
    return false;

  const SCEV *TripCount = SE.getTripCountFromExitCount(BEC);
  return SE.getUnsignedRangeMax(TripCount).getActiveBits() <= 32;
}

BasicBlock *AIELoopVersioner::canonicalizeAndCheckStructure() {
  // We need simplify form for a unique preheader and a single merge point for
  // the two copies' results. Rather than force global LoopSimplify/LCSSA (which
  // would perturb non-hinted loops seen by later passes), canonicalize just
  // this hinted candidate on demand.
  Canonicalized = simplifyLoop(&L, &DT, &LI, &SE, /*AC=*/nullptr,
                               /*MSSAU=*/nullptr, /*PreserveLCSSA=*/false);
  Canonicalized |= formLCSSARecursively(L, DT, &LI, &SE);
  if (!L.isLoopSimplifyForm()) {
    LLVM_DEBUG(dbgs() << "  Not in simplify form\n");
    return nullptr;
  }
  assert(L.isLCSSAForm(DT) && "loop must be in LCSSA form for exit PHIs");

  BasicBlock *ExitBlock = L.getUniqueExitBlock();
  if (!ExitBlock || !L.getExitingBlock()) {
    LLVM_DEBUG(dbgs() << "  No unique exit / exiting block\n");
    return nullptr;
  }
  return ExitBlock;
}

AIELoopVersioner::Result AIELoopVersioner::tryVersionLoop() {
  // Read-only gate first, so an unsuitable loop is rejected without any IR
  // mutation.
  if (!hasVersionableTripCount()) {
    LLVM_DEBUG(dbgs() << "  No versionable trip count for ";
               L.getHeader()->printAsOperand(dbgs()); dbgs() << "\n");
    remarkNotVersioned(ORE, L, "NoVersionableTripCount",
                       "its trip count is unknown or does not fit 32 bits");
    return Result::Unchanged;
  }

  BasicBlock *ExitBlock = canonicalizeAndCheckStructure();
  if (!ExitBlock) {
    remarkNotVersioned(ORE, L, "UnsupportedStructure",
                       "it has no unique exit or resists canonicalization");
    return Canonicalized ? Result::Canonicalized : Result::Unchanged;
  }

  BasicBlock *GuardBB = L.getLoopPreheader();
  BasicBlock *Header = L.getHeader();

  Value *TripCount = expandTripCount(GuardBB->getTerminator());
  if (!TripCount) {
    LLVM_DEBUG(dbgs() << "  Trip count not expandable\n");
    remarkNotVersioned(ORE, L, "TripCountNotExpandable",
                       "its trip count cannot be computed before the loop");
    return Canonicalized ? Result::Canonicalized : Result::Unchanged;
  }

  LLVM_DEBUG(dbgs() << "  Versioning loop "; Header->printAsOperand(dbgs());
             dbgs() << "\n");

  Value *TakeLow = emitGuardCondition(*GuardBB, TripCount);

  // Split off an empty preheader for the original (low-trip-count) copy, then
  // clone the loop into the high-trip-count copy dominated by the guard block.
  BasicBlock *LowPH = SplitBlock(GuardBB, GuardBB->getTerminator(), &DT, &LI,
                                 nullptr, Header->getName() + ".ph");
  ValueToValueMapTy VMap;
  SmallVector<BasicBlock *, 8> HighBlocks;
  Loop *HighLoop = cloneLoopWithPreheader(LowPH, GuardBB, &L, VMap,
                                          ".lver.high", &LI, &DT, HighBlocks);
  remapInstructionsInBlocks(HighBlocks, VMap);

  // Replace the guard's fall-through with the trip-count branch: below the
  // threshold run the original (low) copy, otherwise the pipelined high copy.
  Instruction *OrigTerm = GuardBB->getTerminator();
  IRBuilder<> Builder(OrigTerm);
  Builder.CreateCondBr(TakeLow, LowPH, HighLoop->getLoopPreheader());
  OrigTerm->eraseFromParent();

  DT.changeImmediateDominator(ExitBlock, GuardBB);
  addExitPHIs(HighLoop, ExitBlock, VMap);
  // Cloning gave ExitBlock a predecessor from each copy, so it is no longer a
  // dedicated exit of either loop. Restore that for later loop passes.
  formDedicatedExitBlocks(HighLoop, &DT, &LI, nullptr, /*PreserveLCSSA=*/true);
  formDedicatedExitBlocks(&L, &DT, &LI, nullptr, /*PreserveLCSSA=*/true);

  updateLoopsMetadata(L, *HighLoop);
  ORE.emit([&] {
    return OptimizationRemark(DEBUG_TYPE, "Versioned", L.getStartLoc(),
                              L.getHeader())
           << "loop versioned: a runtime trip-count guard selects a copy the "
              "post-pipeliner may pipeline";
  });
  return Result::Versioned;
}

Value *AIELoopVersioner::emitGuardCondition(BasicBlock &GuardBB,
                                            Value *TripCount) const {
  // The threshold is a placeholder from the thin intrinsic, patched later by
  // the postpipeliner (see the file header for the fail-safe rationale). Seed
  // it with -1 (UINT32_MAX): besides failing safe, -1 fits the narrow
  // scalar-move immediate the pseudo materializes into, unlike a literal
  // INT32_MAX, so the fallback move is emitted intact.
  IRBuilder<> Builder(GuardBB.getTerminator());
  const Triple TT(L.getHeader()->getModule()->getTargetTriple());
  const Intrinsic::ID ThresholdIID =
      AIEIRUtils::getLoopVersionThresholdIntrinsic(TT);
  Value *Threshold =
      Builder.CreateIntrinsic(ThresholdIID, {}, {Builder.getInt32(-1)},
                              /*FMFSource=*/nullptr, "lver.threshold");
  // Exact in both directions: hasVersionableTripCount() bounded the trip count
  // to 32 bits.
  Type *I32Ty = Builder.getInt32Ty();
  if (TripCount->getType() != I32Ty)
    TripCount = Builder.CreateZExtOrTrunc(TripCount, I32Ty, "lver.tc.i32");
  GuardBB.setName(L.getHeader()->getName() + ".lver.guard");
  return Builder.CreateICmpULT(TripCount, Threshold, "lver.low");
}

void AIELoopVersioner::addExitPHIs(Loop *ClonedLoop, BasicBlock *ExitBlock,
                                   ValueToValueMapTy &VMap) const {
  // The loop is in LCSSA form with a single exiting block, so every value it
  // defines and something outside uses already has a one-edge PHI here. Giving
  // each of them the cloned copy's value is the whole merge.
  for (PHINode &PN : ExitBlock->phis()) {
    assert(PN.getNumIncomingValues() == 1 &&
           PN.getIncomingBlock(0) == L.getExitingBlock() &&
           "expected a single-edge LCSSA PHI");
    Value *OrigValue = PN.getIncomingValue(0);
    Value *ClonedValue = VMap.lookup(OrigValue);
    PN.addIncoming(ClonedValue ? ClonedValue : OrigValue,
                   ClonedLoop->getExitingBlock());
  }
}

void AIELoopVersioner::updateLoopsMetadata(Loop &LowLoop, Loop &HighLoop) {
  // Consume the request hint on both copies so a second run of the pass does
  // not re-version either. Mark the high-trip-count copy as versioned (carrying
  // the request's value) so the postpipeliner recognizes it as the pipelined
  // copy; the low copy stays the verbatim un-pipelined fallback.
  // A loop picked up by VersioningMinIterCount carries no request hint, so the
  // marker gets the value the hint would have had when simply enabled.
  const int64_t HintValue =
      AIELoopUtils::getLoopHintInt(LowLoop.getLoopID(),
                                   AIELoopUtils::LoopVersioningHintKey)
          .value_or(1);
  stripVersioningHint(LowLoop);
  stripVersioningHint(HighLoop);
  addStringMetadataToLoop(&HighLoop, AIELoopUtils::LoopVersionedHintKey.data(),
                          HintValue);

  // The runtime guard already guarantees the high copy's trip count, making its
  // llvm.loop.itercount.range redundant. Drop it: a small minimum (e.g. 1)
  // would otherwise make the hardware-loop profitability gate reject the ZOL
  // the postpipeliner needs.
  AIEIRUtils::dropLoopMetadata(HighLoop,
                               StringRef("llvm.loop.itercount.range"));
}

void AIELoopVersioner::stripVersioningHint(Loop &Loop) const {
  AIEIRUtils::dropLoopMetadata(Loop,
                               StringRef(AIELoopUtils::LoopVersioningHintKey));
}
