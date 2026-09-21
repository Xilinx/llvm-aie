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
// what the postpipeliner's guard finder (AIELoopUtils::getGuardBlock)
// relies on: a guard block with two successors, each a dedicated fallthrough
// preheader of its copy. (The block names are for humans; the guard finder
// matches on structure, not names.)
//
//   [<hdr>.lver.guard]   if (trip < threshold) -> low else -> high
//          /     \
//   [<hdr>.ph]  [<hdr>.ph.lver.high]
//       |               |
//    [<hdr>]        [<hdr>.lver.high]   the high-trip-count copy is the clone;
//       |               |               it keeps the hint and gets pipelined.
//   [lo.exit]       [hi.exit]           the low-trip-count copy is the original
//        \             /                loop, kept verbatim as the fallback.
//         \           /                 each copy keeps a dedicated exit block,
//          [exit]                       <exit>.lver.{low,high}.loopexit.
//
// The threshold is produced by a thin per-subtarget intrinsic that lowers to a
// non-CSE-able pseudo, giving the pipeliners a stable handle. The hint's value
// says who owns it:
//
//  - A hint of 1 defers: the guard is seeded with the UINT32_MAX (-1)
//    placeholder, and the postpipeliner overwrites it after scheduling with
//    the stage count it needs. The compare is unsigned, so until then every
//    trip count routes to the low-trip-count (verbatim, un-pipelined) copy.
//    This fails safe -- if the guard is never patched (the high-trip-count
//    copy is not pipelined, or updateVersionGuard bails on a reshaped
//    region), the pipelined copy is simply never entered.
//  - A hint of N >= 2 states the threshold, and it is authoritative: the
//    guard is seeded with N and nothing rewrites it. N is restated as the
//    high copy's llvm.loop.itercount.range minimum, which is what the guard
//    proves, so any later pass reads it as an ordinary declared minimum.
//    The versioned marker is deliberately left off that copy: it exists to
//    tell the postpipeliner a placeholder is waiting, and there is none.
//    Without it the postpipeliner treats the copy as any other loop of known
//    minimum trip count, which also keeps its stage count within N.
//
// A stated threshold above MaxStatedThreshold falls back to the placeholder,
// so an out-of-range hint degrades to the deferred case instead of truncating
// into a guard that admits too little.
//
// Running before HardwareLoops lets both copies lower to ZOL uniformly.
//
// Preconditions, per candidate loop, in the order they are checked:
//  - it carries the versioning request hint (a positive integer value);
//  - it is innermost, so versioning it cannot clone another hinted loop;
//  - its exit count is computable by SCEV and at most 32 bits wide, and the
//    resulting trip count is not the constant zero a 2^32 loop wraps to;
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
//  - the request hint is gone from both copies, so a later run of the pass
//    re-versions neither; the low-trip-count copy carries the fallback marker
//    and a deferred high-trip-count copy the versioned marker;
//  - the high-trip-count copy has no llvm.loop.itercount.range of its own,
//    beyond the minimum a stated threshold restates.
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
#include "llvm/Support/MathExtras.h"
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

  StringRef getPassName() const override { return "AIE Inner Loop Versioner"; }
};

/// Versions a single loop: clone it into a high-trip-count copy, route a
/// runtime trip-count guard to either copy, and fix up metadata. One instance
/// is constructed per candidate loop; call tryVersionLoop() to run the
/// transform.
class AIELoopVersioner {
public:
  AIELoopVersioner(Loop &L, DominatorTree &DT, LoopInfo &LI,
                   ScalarEvolution &SE, OptimizationRemarkEmitter &ORE)
      : L(L), DT(DT), LI(LI), SE(SE), ORE(ORE) {}

  /// Version the loop, returning whether the IR changed. Bailing out after
  /// canonicalization leaves the loop in simplify + LCSSA form, which counts
  /// as a change.
  bool tryVersionLoop();

private:
  Loop &L;
  DominatorTree &DT;
  LoopInfo &LI;
  ScalarEvolution &SE;
  OptimizationRemarkEmitter &ORE;

  /// Set when canonicalizeAndCheckStructure() actually rewrote the loop, so a
  /// later bail can still report the IR as changed.
  bool IsCanonicalized = false;

  static constexpr int32_t DeferredThreshold = -1;

  /// Largest threshold a hint may state. The guard's threshold pseudo
  /// materializes into a scalar-move immediate, narrowest simm10 across
  /// subtargets; a round number well inside that leaves the cap stable if a
  /// subtarget's field changes, and is far beyond any useful stage count.
  static constexpr int64_t MaxStatedThreshold = 100;

  /// The threshold the postpipeliner patches
  int32_t Threshold = DeferredThreshold;

  /// A trip count the runtime guard can hold, or the reason it cannot.
  struct GuardTripCount {
    /// The trip count in the i32 the guard compares in, null when unusable.
    const SCEV *TC;
    /// Remark name and user-facing reason, set only when TC is null.
    StringRef RemarkName;
    StringRef RejectionReason;
  };

  /// The trip count the guard compares, or why the guard cannot hold it.
  /// Mutates nothing, so a rejection costs no IR churn.
  GuardTripCount getGuardTripCount() const;
  /// The value to seed the guard's threshold with, per the hint. Emits a
  /// remark when a stated threshold has to be dropped.
  int32_t computeGuardThreshold();
  /// Bring the loop into simplify + LCSSA form and check its structure supports
  /// versioning. Returns the unique exit block on success, or nullptr if the
  /// loop is unsuitable. Mutates the loop either way.
  BasicBlock *canonicalizeAndCheckStructure();
  /// Materialize \p TC at \p InsertPt, or nullptr if it cannot be expanded
  /// there.
  Value *expandTripCount(const SCEV *TC, Instruction *InsertPt) const;
  /// Emit the guard condition into \p GuardBB, which it also renames: true
  /// when \p TripCount is below the threshold, i.e. when the low-trip-count
  /// copy must run.
  Value *emitGuardCondition(BasicBlock &GuardBB, Value *TripCount) const;
  /// Merge loop-defined values used after the loop across the two copies, by
  /// giving every exit PHI its incoming value from \p ClonedLoop.
  void addExitPHIs(Loop *ClonedLoop, BasicBlock *ExitBlock,
                   ValueToValueMapTy &VMap) const;
  /// Name the dedicated exit block of each copy after the copy it belongs to,
  /// deriving both from \p ExitBlock, the block they merge into.
  void nameDedicatedExits(const BasicBlock &ExitBlock, Loop &HighLoop) const;
  /// Consume the request hint on \p LowLoop and \p HighLoop and mark
  /// \p HighLoop as the versioned one, so the postpipeliner can tell the
  /// copies apart.
  void updateLoopsMetadata(Loop &LowLoop, Loop &HighLoop);
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

/// True when \p L is either copy an earlier run of the pass produced.
/// Versioning consumes the request hint, so this only matters for loops picked
/// up by VersioningMinIterCount, which ignores hints: the fallback copy keeps
/// the iteration-count range that made it a candidate in the first place.
bool isAlreadyVersioned(const Loop *L) {
  const MDNode *LoopID = L->getLoopID();
  return AIELoopUtils::getLoopHintInt(LoopID,
                                      AIELoopUtils::LoopVersionedHintKey)
             .has_value() ||
         AIELoopUtils::getLoopHintInt(LoopID,
                                      AIELoopUtils::LoopVersionFallbackHintKey)
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
    Changed |= AIELoopVersioner(*L, DT, LI, SE, ORE).tryVersionLoop();
  return Changed;
}

Value *AIELoopVersioner::expandTripCount(const SCEV *TC,
                                         Instruction *InsertPt) const {
  SCEVExpander Exp(SE, InsertPt->getDataLayout(), "lver.tc");
  if (!Exp.isSafeToExpandAt(TC, InsertPt))
    return nullptr;
  return Exp.expandCodeFor(TC, TC->getType(), InsertPt);
}

AIELoopVersioner::GuardTripCount AIELoopVersioner::getGuardTripCount() const {
  const SCEV *BEC = SE.getBackedgeTakenCount(&L);
  if (isa<SCEVCouldNotCompute>(BEC))
    return {nullptr, "UnknownTripCount", "its trip count is not computable"};

  // The runtime guard compares the trip count in i32, and the pipelined
  // high-trip-count copy can only become a zero-overhead loop if its trip count
  // fits in a 32-bit register (see AIETTICommon::isHardwareLoopProfitable).
  // Gate on the exit count's type, not its range: truncating an exit count that
  // can exceed UINT32_MAX aliases a huge trip count onto a large i32 that
  // passes the guard, running the high copy with a wrong ZOL count.
  // FIXME: Wide exit counts of narrow range could be versioned again, provided
  // a range check replaces this type check; see narrow_range_i64_trip_count in
  // inner-loop-versioning.ll.
  if (SE.getTypeSizeInBits(BEC->getType()) > 32)
    return {nullptr, "WideTripCount", "its trip count does not fit 32 bits"};

  // Evaluate the +1 in i32. SCEV's default evaluation type is always one bit
  // wider than the exit count, and when the exit count's range covers all-ones
  // (as the rotated `i < n` count of n - 1 does) the i33 result really needs
  // that top bit, so the guard could not hold it. Wrapping in i32 instead costs
  // only the single trip count of 2^32, which the guard routes to the fallback.
  const SCEV *TC = SE.getTripCountFromExitCount(
      BEC, Type::getInt32Ty(L.getHeader()->getContext()), &L);

  // An always-zero trip count is one that wrapped: 2^32 iterations. It sits
  // below every threshold, so the high copy would be unreachable.
  if (SE.getUnsignedRangeMax(TC).isZero())
    return {nullptr, "WrappedTripCount",
            "its trip count of 2^32 wraps to zero in the 32-bit guard"};

  return {TC, {}, {}};
}

int32_t AIELoopVersioner::computeGuardThreshold() {
  // A VersioningMinIterCount loop has no hint, which states no threshold.
  const int64_t Hint = AIELoopUtils::getLoopHintInt(
                           L.getLoopID(), AIELoopUtils::LoopVersioningHintKey)
                           .value_or(1);
  if (Hint < 2)
    return DeferredThreshold;

  if (Hint > MaxStatedThreshold) {
    ORE.emit([&] {
      return OptimizationRemarkMissed(DEBUG_TYPE, "ThresholdTooLarge",
                                      L.getStartLoc(), L.getHeader())
             << "loop versioned without its stated threshold of "
             << ore::NV("Threshold", Hint) << ", which exceeds the maximum of "
             << ore::NV("Maximum", MaxStatedThreshold)
             << "; the threshold is left to be filled in later";
    });
    return DeferredThreshold;
  }
  return static_cast<int32_t>(Hint);
}

BasicBlock *AIELoopVersioner::canonicalizeAndCheckStructure() {
  // We need simplify form for a unique preheader and a single merge point for
  // the two copies' results. Rather than force global LoopSimplify/LCSSA (which
  // would perturb non-hinted loops seen by later passes), canonicalize just
  // this hinted candidate on demand.
  IsCanonicalized = simplifyLoop(&L, &DT, &LI, &SE, /*AC=*/nullptr,
                                 /*MSSAU=*/nullptr, /*PreserveLCSSA=*/false);
  IsCanonicalized |= formLCSSARecursively(L, DT, &LI, &SE);
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

bool AIELoopVersioner::tryVersionLoop() {
  // Read-only gate first, so an unsuitable loop is rejected without any IR
  // mutation.
  const GuardTripCount Guard = getGuardTripCount();
  if (!Guard.TC) {
    LLVM_DEBUG(dbgs() << "  No versionable trip count for ";
               L.getHeader()->printAsOperand(dbgs());
               dbgs() << ": " << Guard.RejectionReason << "\n");
    remarkNotVersioned(ORE, L, Guard.RemarkName, Guard.RejectionReason);
    return false;
  }

  BasicBlock *ExitBlock = canonicalizeAndCheckStructure();
  if (!ExitBlock) {
    remarkNotVersioned(ORE, L, "UnsupportedStructure",
                       "it has no unique exit or resists canonicalization");
    return IsCanonicalized;
  }

  BasicBlock *GuardBB = L.getLoopPreheader();
  BasicBlock *HeaderBB = L.getHeader();

  Value *TripCount = expandTripCount(Guard.TC, GuardBB->getTerminator());
  if (!TripCount) {
    LLVM_DEBUG(dbgs() << "  Trip count not expandable\n");
    remarkNotVersioned(ORE, L, "TripCountNotExpandable",
                       "its trip count cannot be computed before the loop");
    return IsCanonicalized;
  }

  LLVM_DEBUG(dbgs() << "  Versioning loop "; HeaderBB->printAsOperand(dbgs());
             dbgs() << "\n");

  // Read the hint once, before updateLoopsMetadata() consumes it.
  Threshold = computeGuardThreshold();
  Value *TakeLow = emitGuardCondition(*GuardBB, TripCount);

  // Split off an empty preheader for the original (low-trip-count) copy, then
  // clone the loop into the high-trip-count copy dominated by the guard block.
  BasicBlock *LowPH = SplitBlock(GuardBB, GuardBB->getTerminator(), &DT, &LI,
                                 nullptr, HeaderBB->getName() + ".ph");
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
  nameDedicatedExits(*ExitBlock, *HighLoop);

  updateLoopsMetadata(L, *HighLoop);
  ORE.emit([&] {
    return OptimizationRemark(DEBUG_TYPE, "Versioned", L.getStartLoc(),
                              L.getHeader())
           << "loop versioned: a runtime trip-count guard selects a copy the "
              "post-pipeliner may pipeline";
  });
  return true;
}

Value *AIELoopVersioner::emitGuardCondition(BasicBlock &GuardBB,
                                            Value *TripCount) const {
  assert(TripCount->getType() == Type::getInt32Ty(GuardBB.getContext()) &&
         "trip count must be expanded in the type the guard compares in");

  // The threshold comes from the thin intrinsic, either stated by the hint or
  // left as the placeholder the postpipeliner patches (see the file header).
  // The deferred value of -1 (UINT32_MAX) besides failing safe fits the narrow
  // scalar-move immediate the pseudo materializes into, unlike a literal
  // INT32_MAX, so the fallback move is emitted intact.
  IRBuilder<> Builder(GuardBB.getTerminator());
  const Triple TT(L.getHeader()->getModule()->getTargetTriple());
  const Intrinsic::ID ThresholdIID =
      AIEIRUtils::getLoopVersionThresholdIntrinsic(TT);
  Value *ThresholdVal =
      Builder.CreateIntrinsic(ThresholdIID, {}, {Builder.getInt32(Threshold)},
                              /*FMFSource=*/nullptr, "lver.threshold");
  GuardBB.setName(L.getHeader()->getName() + ".lver.guard");
  return Builder.CreateICmpULT(TripCount, ThresholdVal, "lver.low");
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

void AIELoopVersioner::nameDedicatedExits(const BasicBlock &ExitBlock,
                                          Loop &HighLoop) const {
  // formDedicatedExitBlocks derives both new names from ExitBlock, which for a
  // loop canonicalization already renamed reads <exit>.loopexit.loopexit. Drop
  // that suffix so each name says which copy exits there.
  StringRef Base = ExitBlock.getName();
  Base.consume_back(".loopexit");
  if (BasicBlock *HighExit = HighLoop.getUniqueExitBlock())
    HighExit->setName(Base + ".lver.high.loopexit");
  if (BasicBlock *LowExit = L.getUniqueExitBlock())
    LowExit->setName(Base + ".lver.low.loopexit");
}

void AIELoopVersioner::updateLoopsMetadata(Loop &LowLoop, Loop &HighLoop) {
  // Consume the request hint on both copies and mark each one, so a second run
  // of the pass re-versions neither. The low copy gets the fallback marker,
  // which no other pass reads.
  // A loop picked up by VersioningMinIterCount carries no request hint, so the
  // marker gets the value the hint would have had when simply enabled.
  const int64_t HintValue =
      AIELoopUtils::getLoopHintInt(LowLoop.getLoopID(),
                                   AIELoopUtils::LoopVersioningHintKey)
          .value_or(1);
  const StringRef RequestHint = AIELoopUtils::LoopVersioningHintKey;
  AIEIRUtils::dropLoopMetadata(LowLoop, RequestHint);
  AIEIRUtils::dropLoopMetadata(HighLoop, RequestHint);
  addStringMetadataToLoop(&LowLoop,
                          AIELoopUtils::LoopVersionFallbackHintKey.data(), 1);

  AIEIRUtils::dropLoopMetadata(HighLoop,
                               StringRef("llvm.loop.itercount.range"));

  if (Threshold < 2) {
    // A deferred threshold leaves a placeholder in the guard. The versioned
    // marker is what tells the postpipeliner to go find it, and it also lifts
    // the minimum trip-count gate, which only the guard makes safe. No
    // iteration-count range goes with it: a minimum of 1 would make the
    // hardware-loop profitability gate reject the ZOL the postpipeliner needs.
    addStringMetadataToLoop(
        &HighLoop, AIELoopUtils::LoopVersionedHintKey.data(), HintValue);
    return;
  }

  // A stated threshold is authoritative, so the guard needs no patching and
  // the marker that requests it is left off. Restating the threshold as the
  // minimum is all a later pass needs: it reads as an ordinary declared
  // minimum, and the guard is what proves it.
  addStringMetadataToLoop(&HighLoop, "llvm.loop.itercount.range", Threshold);
}
