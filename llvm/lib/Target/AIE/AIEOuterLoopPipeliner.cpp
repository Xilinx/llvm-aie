//===- AIEOuterLoopPipeliner.cpp - Outer Loop Pipeliner Pass --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Outer loop pipelining for AIE: overlaps the stage-0 chain of outer iteration
// i+1 with the inner loop + store chain of iteration i.
//
// Positions are named top / inner / bottom (the outer loop's header, inner
// loop, and latch). The top block splits into stage-0 and stage-1 instructions
// by role (the partitioning strategy lives in collectStages):
//   Stage 0 = the prefetched chain: cloned into the stage0.top preheader above
//             the steady loop and prefetched in the bottom block for the next
//             iteration.
//   Stage 1 = the rest of the top: kept in the steady top and re-cloned into
//             the last-iteration top. May be empty, in which case the whole top
//             is stage 0.
//
// Original CFG:
//
//   [preheader]
//       |
//   [top]  <------------------\  <- stage 0 + stage 1 (+ set.loop.iterations)
//       |                      |
//   [inner.*]                  |  <- inner loop
//       |                      |
//   [bottom]  ----------------/   <- stage 1 + latch
//       |  (exit branch)
//   [exit]
//
// Produced CFG:
//
//   [preheader]
//       |
//   [stage0.top]                             <- stage 0 (preheader copy)
//       |
//   [steady.stage1.top]  <---------------\   <- stage 1 (+ set.loop.iterations)
//       |                                 |
//   [steady.stage1.inner.*]              |   <- steady-state inner loop
//       |                                 |
//   [steady.stage1.bottom.and.stage0.top]-/  <- stage 1 + next-iter stage 0
//       |  (exit branch)
//   [lastiter.stage1.top]                    <- stage 1 (+ set.loop.iterations)
//       |
//   [lastiter.stage1.inner.*]                <- inner loop clone
//       |
//   [lastiter.stage1.bottom]                 <- stage 1
//       |
//   [exit]
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "Utils/AIEIRUtils.h"
#include "Utils/AIELoopOptionOverrides.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Analysis/DependenceAnalysis.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Metadata.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

#include <functional>
#include <memory>
#include <optional>
#include <vector>

using namespace llvm;

#define DEBUG_TYPE "aie-outer-loop-pipeliner"

static cl::opt<bool> EnableOuterLoopPipelining(
    "aie-enable-outer-loop-pipelining",
    cl::desc("Enable outer loop pipelining for long-latency loops"),
    cl::init(false), cl::Hidden);

static cl::opt<unsigned> OuterLoopPipeliningMinTripCount(
    "aie-outer-loop-pipelining-min-trip-count",
    cl::desc("Minimum trip count required for outer loop pipelining"),
    cl::init(2), cl::Hidden);

// External flag name keeps the legacy "split-prologue" spelling for test
// stability; the in-tree symbol and prose use the stage-0/stage-1 vocabulary.
static cl::opt<bool> SplitStages(
    "aie-outer-loop-pipelining-split-prologue",
    cl::desc("Split the top block into stage-0 and stage-1 using "
             "different strategies to reach more compact schedules"),
    cl::init(true), cl::Hidden);

static cl::opt<bool> SpeculativeLastIteration(
    "aie-outer-loop-pipelining-speculative",
    cl::desc("Use speculative loads on the last iteration instead of creating "
             "a last iteration region. Avoids code duplication and register "
             "pressure at the cost of unused loads on the final iteration."),
    cl::init(false), cl::Hidden);

static cl::opt<bool> LeanStage0Mode(
    "aie-outer-loop-pipelining-lean-stage0",
    cl::desc("Limit stage 0 to loads, address computations, and target "
             "intrinsics selected for lean stage 0"),
    cl::init(false), cl::Hidden);

static cl::opt<bool> EnableOuterLoopHardwareLoop(
    "aie-outer-loop-hw-loop",
    cl::desc("Convert downcounting outer loops to JNZD hardware loops after "
             "outer loop pipelining"),
    cl::init(true), cl::Hidden);

using SplitStrategy = std::function<bool(const Instruction *)>;

static bool produces2048BitVector(const Instruction *I) {
  if (!isa<CallInst>(I))
    return false;
  auto *VT = dyn_cast<FixedVectorType>(I->getType());
  return VT && VT->getPrimitiveSizeInBits() == 2048;
}

static SmallVector<SplitStrategy, 4> getSplitStrategies() {
  // TODO: add more split strategies.
  return {
      produces2048BitVector,
  };
}

// Returns true if any split strategy marks this instruction as the point where
// stage 1 begins (e.g. a wide-vector producer; see produces2048BitVector).
static bool isStage1SplitPoint(const Instruction *I) {
  for (const auto &Strategy : getSplitStrategies()) {
    if (Strategy(I))
      return true;
  }
  return false;
}

static bool isSafePointerIncrementIntrinsic(const AIEBaseInstrInfo &TII,
                                            Intrinsic::ID IID) {
  return IID == TII.getAddrIntrinsic2D() || IID == TII.getAddrIntrinsic3D();
}

// How reroutePhiIncomings treats OldPred's existing incoming entry.
enum class PhiEdge {
  Repoint,   // overwrite OldPred's entry to point at NewPred
  AppendNew, // keep OldPred's entry and add a parallel one for NewPred
};

// Reroute each PHI incoming in BB from OldPred to NewPred, value MapVal(old).
// AppendNew adds a parallel incoming (OldPred still live); Repoint overwrites.
static void reroutePhiIncomings(
    BasicBlock *BB, BasicBlock *OldPred, BasicBlock *NewPred, PhiEdge Edge,
    function_ref<Value *(Value *)> MapVal = [](Value *V) { return V; }) {
  for (PHINode &PHI : BB->phis()) {
    int Idx = PHI.getBasicBlockIndex(OldPred);
    if (Idx < 0)
      continue;
    Value *NewVal = MapVal(PHI.getIncomingValue(Idx));
    if (Edge == PhiEdge::AppendNew) {
      PHI.addIncoming(NewVal, NewPred);
    } else {
      PHI.setIncomingValue(Idx, NewVal);
      PHI.setIncomingBlock(Idx, NewPred);
    }
  }
}

namespace {

// Source value -> replacement (clone, or a header PHI's resolved incoming),
// consumed by RemapInstruction. Blocks and instructions share one table.
using RemapTable = ValueToValueMapTy;

// A single-entry/single-exit run of basic blocks in program order, queried and
// iterated through one interface that extends to multi-block regions.
class BlockRegion {
  SmallVector<BasicBlock *, 4> Blocks;

  // No edge enters a non-entry block and none leaves a non-exit block: the run
  // is single-entry/single-exit. Vacuously true for a single-block region.
  [[maybe_unused]] bool isSingleEntrySingleExit() const;

public:
  void assign(ArrayRef<BasicBlock *> BBs);

  size_t size() const { return Blocks.size(); }
  bool empty() const { return Blocks.empty(); }

  BasicBlock *entry() const {
    return Blocks.empty() ? nullptr : Blocks.front();
  }
  BasicBlock *back() const { return Blocks.empty() ? nullptr : Blocks.back(); }

  bool contains(const BasicBlock *BB) const {
    return llvm::is_contained(Blocks, BB);
  }
  bool contains(const Instruction *I) const { return contains(I->getParent()); }

  ArrayRef<BasicBlock *> blocks() const { return Blocks; }

  // Visit every instruction of the region in program order, block by block.
  // Works identically whether the region is one block or several.
  void forEachInstruction(function_ref<void(Instruction *)> Visit) const;
};

struct LatchConditionInfo {
  ICmpInst *Cmp = nullptr;
  Value *Limit = nullptr;
  unsigned LimitIdx = 0;
  int64_t Step = 0;
  BinaryOperator *Counter = nullptr;
  PHINode *IV = nullptr;

  // A unit-downcounting latch (step -1), convertible to
  // @llvm.loop.decrement.reg.
  bool isDowncounting() const { return Step == -1; }

  static std::optional<LatchConditionInfo>
  tryCreate(const Loop *L, ICmpInst *Cmp, const BasicBlock *Header);
};

class LoopStructure {
protected:
  BasicBlock *InnerExit = nullptr;

  SmallVector<BasicBlock *, 4> InnerLoopBlocks;

  MDNode *OuterLoopID = nullptr;

  // The top region in program order; its entry is the top block.
  BlockRegion TopRegion;

  // The bottom region in program order; its exit is the bottom block.
  BlockRegion BottomRegion;

  std::optional<LatchConditionInfo> OuterLoopCondition;

  // Returns true if I is in the top region.
  bool isInTop(const Instruction *I) const { return TopRegion.contains(I); }

public:
  LoopStructure() = default;
  virtual ~LoopStructure() = default;

  BasicBlock *getTop() const { return TopRegion.entry(); }
  BasicBlock *getBottom() const { return BottomRegion.back(); }

  // LoopInfo lists a loop's header first, and cloning preserves block order.
  BasicBlock *getInnerHeader() const { return InnerLoopBlocks.front(); }
  // The single inner-loop predecessor of the inner header (single-latch form).
  BasicBlock *getInnerLatch() const;
  BasicBlock *getInnerExit() const { return InnerExit; }
  ArrayRef<BasicBlock *> getInnerBlocks() const { return InnerLoopBlocks; }
  MDNode *getOuterLoopID() const { return OuterLoopID; }

  // The LS blocks in program order: top, inner-loop blocks, bottom.
  SmallVector<BasicBlock *, 8> blocksInProgramOrder() const;

  const BlockRegion &topRegion() const { return TopRegion; }
  const BlockRegion &bottomRegion() const { return BottomRegion; }

  const LatchConditionInfo &latchCondition() const {
    assert(OuterLoopCondition && "loop structure has no latch condition");
    return *OuterLoopCondition;
  }
  void clearLatchCondition() { OuterLoopCondition.reset(); }

  // Returns true if I is in the bottom region.
  bool isInBottom(const Instruction *I) const {
    return BottomRegion.contains(I);
  }

  // The outer loop preheader. The original derives it from LoopInfo; a clone
  // returns its stored preheader.
  virtual BasicBlock *getPreheader() const = 0;

  // Returns the bottom-block terminator as a BranchInst.
  BranchInst *getLatchBranch() const {
    return cast<BranchInst>(getBottom()->getTerminator());
  }

  // The bottom successor that leaves the loop (the non-top edge).
  BasicBlock *getExitBlock() const;
};

// The original loop, built from LoopInfo. Owns the LoopInfo loops and the
// stage-0 / stage-1 instruction lists.
class OrigLoopStructure : public LoopStructure {
  Loop *OuterLoop = nullptr;
  Loop *InnerLoop = nullptr;

  // The stage-0 / stage-1 split of the top-block instructions, in program
  // order.
  std::vector<Instruction *> Stage0Insts;
  std::vector<Instruction *> Stage1Insts;

  explicit OrigLoopStructure(Loop *L) : OuterLoop(L) {}

  Loop *getOuterLoop() const { return OuterLoop; }

  // Populate the inner-loop fields and top/bottom regions from
  // OuterLoop; returns false unless the loop is a supported pipelining
  // candidate.
  bool analyzeLoopStructure();

  // Every outer-loop block belongs to the top, the inner loop, or the
  // single-block bottom; false on any other (unsupported) shape.
  bool allOuterBlocksAccountedFor() const;

  // Validate the linear single-block top (top block == inner preheader)
  // and populate the top region; false for any other shape.
  bool discoverTopRegion();

  /// \return true if latch exit condition are valid and the loop bound can be
  /// adjusted (Step != 0).
  bool canAdjustLoopBound();

  // True if every incoming block is inside the top region (cloned with
  // it), as opposed to a loop-carried PHI resolved to a concrete value when
  // cloning.
  bool isRegionInternalPhi(const PHINode *PHI) const;

  // True if I lives in the top and is a plain instruction or a
  // region-internal PHI; loop-carried PHIs are excluded.
  bool isPipelineableValue(const Instruction *I) const;

  // A pipelineable value also clonable into the prefetch/last-iteration sites;
  // excludes terminators and hardware-loop setup.
  bool isPipelineCandidate(const Instruction *I) const;

  // Returns true if the inner loop is a hardware (JNZD) loop, i.e. its latch is
  // controlled by an @llvm.loop.decrement intrinsic.
  bool isInnerLoopHardwareLoop() const;

  // Returns the top-block loads.
  SmallVector<LoadInst *, 16> collectTopLoads() const;

  // Returns the bottom-block stores.
  SmallVector<StoreInst *, 16> collectBottomStores() const;

  // Returns the pipelineable closure of Seeds: users if TraverseUsers,
  // otherwise operands.
  SmallPtrSet<Instruction *, 32> collectClosure(ArrayRef<Instruction *> Seeds,
                                                bool TraverseUsers) const;

  // The stage-1 set: the split points (IsSplitPoint, reachable from the
  // top-block loads) and their top-block descendants. Empty if none is found.
  SmallPtrSet<Instruction *, 32>
  collectStage1Cone(function_ref<bool(const Instruction *)> IsSplitPoint) const;

  // The fallback stage-0 collection (no split point): pipeline candidates
  // backward-reachable from inner-loop uses, in program order.
  void collectStage0FromInnerLoop();

public:
  // Build and validate the LS for L; nullptr if L is not a supported candidate.
  static std::unique_ptr<OrigLoopStructure> tryBuildFrom(Loop *L);

  Loop *getInnerLoop() const { return InnerLoop; }

  BasicBlock *getPreheader() const override {
    return OuterLoop->getLoopPreheader();
  }

  const std::vector<Instruction *> &stage0Insts() const { return Stage0Insts; }
  const std::vector<Instruction *> &stage1Insts() const { return Stage1Insts; }

  // True if rotating pays off: hardware inner loop, outer trip count (from SE)
  // meets MinTripCount, and the bottom has stores.
  bool isProfitableToRotate(ScalarEvolution &SE, unsigned MinTripCount) const;

  // Returns true if it is safe to reorder the top-block loads before the
  // bottom-block stores (rejects volatile/atomic memory ops).
  bool isSafeToReorderMemoryOps() const;

  // Split the top-block pipeline candidates into the stage-1 split-point cone
  // and the stage-0 remainder (the load/address chain).
  void collectStages(function_ref<bool(const Instruction *)> IsSplitPoint);

  // Stage 0 contains each top-block load, its backward address-computation
  // chain, and its sole direct target-selected intrinsic user with the user's
  // required operand chains. All other pipeline candidates form stage 1.
  void collectLeanStage0(const TargetTransformInfo &TTI);

  // Delete this (now unreachable) LS's blocks.
  void removeFromCFG() const;
};

// A steady-state or last-iteration clone. Owns its source->clone CloneMap and a
// stored outer preheader.
class CloneLoopStructure : public LoopStructure {
  BasicBlock *OuterPreheader = nullptr;

  // Each source value mapped to its clone in this LS.
  RemapTable CloneMap;

public:
  // Deep-clone Src into "<name>.<Suffix>" IR; internal references are remapped,
  // edges leaving the LS stay at Src's externals for the caller to rewire.
  CloneLoopStructure(const LoopStructure &Src, const Twine &Suffix);

  // Create the empty last-iteration blocks spliced before Src's exit and record
  // the Src->lastiter block mappings; the caller fills the bodies. Src is the
  // structural source (the original loop), so the last iteration is a clone of
  // the original body.
  explicit CloneLoopStructure(const LoopStructure &Src);

  // Clone of V in this LS, or V itself if it has none (invariants/args pass
  // through). Direction is source->clone, matching RemapInstruction.
  Value *cloneOf(Value *V) const {
    auto It = CloneMap.find(V);
    return It != CloneMap.end() ? static_cast<Value *>(It->second) : V;
  }
  // Clone of block BB in this LS. BB must have been cloned into this LS.
  BasicBlock *clonedBlock(BasicBlock *BB) const {
    return cast<BasicBlock>(cloneOf(BB));
  }
  RemapTable &cloneMap() { return CloneMap; }
  const RemapTable &cloneMap() const { return CloneMap; }

  // Repoint Src's recorded clone to NewClone. Used when a clone is superseded,
  // e.g. a stage-0 clone replaced by a pipelined header PHI, so the map keeps
  // naming the value that actually represents Src in this loop.
  void retargetClone(Value *Src, Value *NewClone) { CloneMap[Src] = NewClone; }

  // The value Src carries into the final (peeled) iteration: if Src's clone is
  // a header PHI of this loop, the value on its back edge (the prefetched /
  // next-iteration value); otherwise the clone itself.
  Value *lastIterInputFor(Value *Src) const;

  BasicBlock *getPreheader() const override { return OuterPreheader; }

  // Record a preheader the header PHIs already reference (its branch is rewired
  // by the caller). Use installPreheader to splice in a fresh block instead.
  void recordExistingPreheader(BasicBlock *BB);

  // Splice a fresh block in as this LS's preheader, repointing the header PHIs'
  // edge from the current preheader to NewPreheader.
  void installPreheader(BasicBlock *NewPreheader);

  // Retarget the cached latch condition's pointers from the source LS to this
  // clone, so adjustLoopBound / getDowncountingInfo operate on the clone.
  void remapBoundThroughCloneMap();

  // Adjust the outer loop trip count from N to N-1.
  void adjustLoopBound() const;

  // Repair loop metadata (trip count changed): decrement itercount.range, drop
  // the consumed enable hint, and append the success marker.
  void updateLoopMetadata() const;
};

class AIEOuterLoopPipeliner : public FunctionPass {
public:
  static char ID;
  AIEOuterLoopPipeliner() : FunctionPass(ID) {}
  bool runOnFunction(Function &F) override;
  void getAnalysisUsage(AnalysisUsage &AU) const override;
  StringRef getPassName() const override { return "AIE Outer Loop Pipeliner"; }

private:
  LoopInfo *LI = nullptr;
  DominatorTree *DT = nullptr;
  ScalarEvolution *SE = nullptr;
  const TargetTransformInfo *TTI = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;

  bool runOnLoop(Loop *L);
  // Run every pipelining precondition as a flat early-return guard and
  // transform L iff all pass. True if L was pipelined.
  bool tryPipelineLoop(Loop *L, const AIE::LoopOptionOverrides &Overrides);
  // Seed Map with each outer-header PHI's incoming value on the FromEdge edge,
  // collapsing loop-carried PHIs to the values seen entering via that edge.
  void seedHeaderPhiEdge(RemapTable &Map, const LoopStructure &LS,
                         BasicBlock *FromEdge) const;
  bool performTransformation(OrigLoopStructure &OrigLS,
                             const AIE::LoopOptionOverrides &Overrides);

  // Swap a freshly cloned steady-state LS in for the original by rewiring its
  // preheader, leaving the original unreachable and ready for deletion.
  void swapInClonedLS(const OrigLoopStructure &OrigLS,
                      CloneLoopStructure &SteadyLS) const;

  // Clone OrigLS's stage-0 chain into a new stage-0 top block before the steady
  // loop and adopt it as the steady preheader (entry pointer values).
  void cloneStage0IntoPreheader(const OrigLoopStructure &OrigLS,
                                CloneLoopStructure &SteadyLS,
                                RemapTable &PreheaderVMap);

  // Clone OrigLS's stage-0 chain into the bottom block using next-iteration
  // pointer values, so the loads prefetch for the next iteration.
  void cloneStage0IntoBottom(const OrigLoopStructure &OrigLS,
                             const CloneLoopStructure &SteadyLS,
                             RemapTable &BottomVMap);

  // For each steady stage-0 instruction, merge its preheader (entry-edge) and
  // bottom (back-edge) clones via a header PHI, erase the instruction, and
  // retarget SteadyLS's clone map so the slot now resolves to the merged PHI.
  void createPipelinedPHIs(const OrigLoopStructure &OrigLS,
                           CloneLoopStructure &SteadyLS,
                           const RemapTable &PreheaderVMap,
                           const RemapTable &BottomVMap);

  // Create the last-iteration region (top + inner-loop clone + stores-only
  // bottom) and redirect the steady latch exit into it.
  void peelLastIteration(const OrigLoopStructure &OrigLS,
                         const CloneLoopStructure &SteadyLS);

  // Clone I into Dest before InsertPt, record orig->clone in VMap, and return
  // the clone. A non-empty Suffix renames non-void clones to "<orig><Suffix>".
  static Instruction *cloneInstInto(Instruction &I, BasicBlock &Dest,
                                    BasicBlock::iterator InsertPt,
                                    RemapTable &VMap, const Twine &Suffix);
  // Clone Insts (in order) into DstBB before InsertPt, then remap operands of
  // all the clones through VMap. Returns the clones, parallel to Insts.
  static SmallVector<Instruction *, 16>
  cloneAndRemapInsts(ArrayRef<Instruction *> Insts, BasicBlock &DstBB,
                     BasicBlock::iterator InsertPt, RemapTable &VMap,
                     const Twine &Suffix);
  // Second-pass remap of freshly inserted clones, once all are in place.
  static void remapClones(ArrayRef<Instruction *> Clones, RemapTable &VMap);

  // Translate each instruction of the original LS to its clone via VMap;
  // unmapped entries (e.g. loop-invariant operands) pass through unchanged.
  static SmallVector<Instruction *, 16>
  remapToClone(ArrayRef<Instruction *> Insts, const RemapTable &VMap);

  // Step helpers of peelLastIteration, in call order. The last iteration is
  // cloned structurally from OrigLS (orig->lastiter map); its live-in values
  // come from SteadyLS via seedLastIterInputs / lastIterInputFor.

  // Seed Map so each stage-0 slot and loop-carried header PHI of OrigLS
  // resolves to the value SteadyLS carries into the last iteration (its
  // prefetch / next-iteration value).
  void seedLastIterInputs(RemapTable &Map, const OrigLoopStructure &OrigLS,
                          const CloneLoopStructure &SteadyLS) const;

  // Clone the hardware-loop setup (set.loop.iterations) from Src's top into the
  // last-iteration top.
  void cloneHardwareLoopSetupInto(CloneLoopStructure &LastIterLS,
                                  const LoopStructure &Src) const;

  // Fill the last-iteration inner-loop block clones with remapped OrigLS
  // instruction bodies and wire the top into the inner header.
  void cloneInnerLoopIntoLastIter(const OrigLoopStructure &OrigLS,
                                  CloneLoopStructure &LastIterLS) const;

  // Fill the last-iteration bottom with the pristine original stores and
  // pointer updates only (no prefetch loads), cloned directly from OrigLS.
  void populateLastIterBottom(const OrigLoopStructure &OrigLS,
                              CloneLoopStructure &LastIterLS) const;

  // Splice the last-iteration into the CFG: last-iteration bottom -> original
  // exit, repoint the exit's bottom-predecessor PHIs to it, redirect the
  // steady bottom exit to the last-iteration top.
  void wireLastIterIntoCFG(const OrigLoopStructure &OrigLS,
                           const CloneLoopStructure &SteadyLS,
                           const CloneLoopStructure &LastIterLS) const;

  // The speculative path exits directly from the steady loop. Repoint exit
  // PHIs and remap exit live-outs away from the soon-to-be-deleted original.
  void wireSteadyIntoExit(const OrigLoopStructure &OrigLS,
                          const CloneLoopStructure &SteadyLS) const;

  // The bottom instructions forming PHI's next-iteration pointer-update
  // chain, or nullopt if it cannot be safely lifted.
  std::optional<SmallPtrSet<Instruction *, 16>>
  collectLiftableBottomChain(const OrigLoopStructure &OrigLS,
                             PHINode &PHI) const;

  // Lift the bottom pointer-update chains (add.2d/add.3d and descendants) to
  // the top-block end so top-block cloning naturally covers them. True if
  // moved.
  bool liftBottomPointerUpdatesToTop(const OrigLoopStructure &OrigLS);

  // Convert the steady loop to a JNZD hardware loop: start.loop.iterations in
  // the preheader, a counter PHI in the header, loop.decrement.reg in the
  // latch. Requires latchCondition().isDowncounting().
  void convertOuterLoopToHardwareLoop(CloneLoopStructure &SteadyLS);
};

} // end anonymous namespace

char AIEOuterLoopPipeliner::ID = 0;
char &llvm::AIEOuterLoopPipelinerID = AIEOuterLoopPipeliner::ID;

INITIALIZE_PASS_BEGIN(AIEOuterLoopPipeliner, DEBUG_TYPE,
                      "AIE Outer Loop Pipeliner", false, false)
INITIALIZE_PASS_DEPENDENCY(LoopInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(DominatorTreeWrapperPass)
INITIALIZE_PASS_DEPENDENCY(ScalarEvolutionWrapperPass)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_END(AIEOuterLoopPipeliner, DEBUG_TYPE,
                    "AIE Outer Loop Pipeliner", false, false)

llvm::FunctionPass *llvm::createAIEOuterLoopPipelinerPass() {
  return new AIEOuterLoopPipeliner();
}

void AIEOuterLoopPipeliner::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<LoopInfoWrapperPass>();
  AU.addRequired<DominatorTreeWrapperPass>();
  AU.addRequired<ScalarEvolutionWrapperPass>();
  AU.addRequired<TargetTransformInfoWrapperPass>();
  AU.addRequired<TargetPassConfig>();
  FunctionPass::getAnalysisUsage(AU);
}

bool AIEOuterLoopPipeliner::runOnFunction(Function &F) {
  if (skipFunction(F))
    return false;

  LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
  DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
  SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();
  TTI = &getAnalysis<TargetTransformInfoWrapperPass>().getTTI(F);
  const TargetMachine &TM =
      getAnalysis<TargetPassConfig>().getTM<TargetMachine>();
  TII = static_cast<const AIEBaseInstrInfo *>(
      TM.getSubtargetImpl(F)->getInstrInfo());
  LLVM_DEBUG(dbgs() << "AIEOuterLoopPipeliner: " << F.getName() << "\n");

  bool Changed = false;
  SmallVector<Loop *, 4> TopLevelLoops(LI->begin(), LI->end());
  for (Loop *L : TopLevelLoops)
    Changed |= runOnLoop(L);
  return Changed;
}

bool AIEOuterLoopPipeliner::runOnLoop(Loop *L) {
  LLVM_DEBUG(dbgs() << "  Considering: ";
             L->getHeader()->printAsOperand(dbgs(), false); dbgs() << "\n");
  // Build per-loop option overrides from !llvm.loop.hint.* metadata.
  AIE::LoopOptionOverrides Overrides(L->getLoopID());

  if (tryPipelineLoop(L, Overrides))
    return true;

  // Untransformed: recurse into subloops, e.g. a nested middle loop is the only
  // one annotated for pipelining.
  bool Changed = false;
  for (Loop *SubLoop : L->getSubLoops())
    Changed |= runOnLoop(SubLoop);
  return Changed;
}

bool AIEOuterLoopPipeliner::tryPipelineLoop(
    Loop *L, const AIE::LoopOptionOverrides &Overrides) {
  if (!Overrides.get(EnableOuterLoopPipelining)) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: not enabled (flag/metadata)\n");
    return false;
  }

  std::unique_ptr<OrigLoopStructure> LS = OrigLoopStructure::tryBuildFrom(L);
  if (!LS) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: unsupported loop structure\n");
    return false;
  }

  if (!LS->isProfitableToRotate(
          *SE, Overrides.get(OuterLoopPipeliningMinTripCount))) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: not profitable to rotate\n");
    return false;
  }

  if (!LS->isSafeToReorderMemoryOps()) {
    LLVM_DEBUG(dbgs() << "    Not pipelining: unsafe to reorder memory ops\n");
    return false;
  }

  LLVM_DEBUG(dbgs() << "  Applying outer loop pipelining on ";
             LS->getTop()->printAsOperand(dbgs(), false); dbgs() << "\n");
  return performTransformation(*LS, Overrides);
}

bool OrigLoopStructure::analyzeLoopStructure() {
  // Early validation before populating the inner-loop fields.
  if (OuterLoop->getSubLoops().size() != 1) {
    LLVM_DEBUG(dbgs() << "    Not exactly one subloop\n");
    return false;
  }
  if (!OuterLoop->getLoopLatch()) {
    LLVM_DEBUG(dbgs() << "    No single bottom block\n");
    return false;
  }

  // Validate inner loop components before deriving fields from them.
  InnerLoop = OuterLoop->getSubLoops()[0];
  if (!InnerLoop->getLoopPreheader() || !InnerLoop->getExitBlock() ||
      !InnerLoop->getLoopLatch()) {
    LLVM_DEBUG(dbgs() << "    Inner loop missing preheader/exit/latch\n");
    return false;
  }

  // Populate the top/bottom regions and the derived-field backing state.
  InnerExit = InnerLoop->getExitBlock();
  InnerLoopBlocks.assign(InnerLoop->block_begin(), InnerLoop->block_end());
  OuterLoopID = OuterLoop->getLoopID();
  TopRegion.assign({OuterLoop->getHeader()});
  BottomRegion.assign({OuterLoop->getLoopLatch()});

  // The bottom must be a single block: inner exit == bottom block.
  if (getInnerExit() != getBottom()) {
    LLVM_DEBUG(dbgs() << "    Inner exit != bottom block\n");
    return false;
  }

  if (!discoverTopRegion())
    return false;

  if (!allOuterBlocksAccountedFor())
    return false;

  LLVM_DEBUG(dbgs() << "    top region: " << topRegion().size()
                    << " block(s); stores in bottom block\n");

  if (!canAdjustLoopBound()) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound\n");
    return false;
  }
  return true;
}

bool OrigLoopStructure::discoverTopRegion() {
  // The derived getTop() assumes the linear single-block top
  // (top block == inner preheader); validate that against LoopInfo here.
  if (InnerLoop->getLoopPreheader() != getTop()) {
    LLVM_DEBUG(dbgs() << "    Inner preheader != top block\n");
    return false;
  }
  TopRegion.assign({getTop()});
  return true;
}

bool OrigLoopStructure::allOuterBlocksAccountedFor() const {
  for (BasicBlock *BB : OuterLoop->blocks()) {
    const bool Accounted = getInnerLoop()->contains(BB) || BB == getBottom() ||
                           topRegion().contains(BB);
    if (Accounted)
      continue;
    LLVM_DEBUG(dbgs() << "    Unexpected outer-loop block: " << BB->getName()
                      << "\n");
    return false;
  }
  return true;
}

bool OrigLoopStructure::isInnerLoopHardwareLoop() const {
  auto *BI = dyn_cast<BranchInst>(getInnerLatch()->getTerminator());
  if (!BI || !BI->isConditional())
    return false;
  auto *Cond = dyn_cast<Instruction>(BI->getCondition());
  return Cond && AIEIRUtils::isHardwareLoopDecrement(Cond);
}

SmallVector<LoadInst *, 16> OrigLoopStructure::collectTopLoads() const {
  // Loads live in the single top block.
  SmallVector<LoadInst *, 16> Loads;
  for (Instruction &I : *getTop())
    if (auto *L = dyn_cast<LoadInst>(&I))
      Loads.push_back(L);
  return Loads;
}

SmallVector<StoreInst *, 16> OrigLoopStructure::collectBottomStores() const {
  // Stores live in the single bottom block.
  SmallVector<StoreInst *, 16> Stores;
  for (Instruction &I : *getBottom())
    if (auto *S = dyn_cast<StoreInst>(&I))
      Stores.push_back(S);
  return Stores;
}

void AIEOuterLoopPipeliner::seedHeaderPhiEdge(RemapTable &Map,
                                              const LoopStructure &LS,
                                              BasicBlock *FromEdge) const {
  for (PHINode &PHI : LS.getTop()->phis()) {
    int Idx = PHI.getBasicBlockIndex(FromEdge);
    if (Idx >= 0)
      Map[&PHI] = PHI.getIncomingValue(Idx);
  }
}

bool OrigLoopStructure::isProfitableToRotate(ScalarEvolution &SE,
                                             unsigned MinTripCount) const {
  if (!isInnerLoopHardwareLoop()) {
    LLVM_DEBUG(dbgs() << "    Inner loop is not a hardware loop\n");
    return false;
  }
  std::optional<int64_t> MinTC = llvm::getMinTripCount(getOuterLoop(), &SE);
  const bool TripCountTooLow = !MinTC || *MinTC < (int64_t)MinTripCount;
  if (TripCountTooLow) {
    LLVM_DEBUG(dbgs() << "    Trip count too low\n");
    return false;
  }

  return true;
}

// A volatile or atomic memory op can never be reordered.
template <typename MemInstT>
static bool anyVolatileOrAtomic(ArrayRef<MemInstT *> MemOps) {
  return llvm::any_of(MemOps, [](const MemInstT *M) {
    return M->isVolatile() || M->isAtomic();
  });
}

bool OrigLoopStructure::isSafeToReorderMemoryOps() const {
  // TODO(outer-loop-pipeliner): add alias/dependence analysis for load/store
  // reordering; for now only reject never-reorderable volatile/atomic ops.
  if (anyVolatileOrAtomic<LoadInst>(collectTopLoads())) {
    LLVM_DEBUG(dbgs() << "    Unsafe: volatile/atomic top-block load\n");
    return false;
  }
  if (anyVolatileOrAtomic<StoreInst>(collectBottomStores())) {
    LLVM_DEBUG(dbgs() << "    Unsafe: volatile/atomic bottom-block store\n");
    return false;
  }
  return true;
}

SmallPtrSet<Instruction *, 32>
OrigLoopStructure::collectClosure(ArrayRef<Instruction *> Seeds,
                                  bool TraverseUsers) const {
  SmallPtrSet<Instruction *, 32> Closure;
  SmallVector<Instruction *, 32> Worklist;
  auto Enqueue = [&](Instruction *I) {
    if (!I)
      return;
    if (!isPipelineableValue(I))
      return;
    if (Closure.insert(I).second)
      Worklist.push_back(I);
  };
  for (Instruction *I : Seeds)
    Enqueue(I);

  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    if (TraverseUsers) {
      for (User *U : I->users())
        Enqueue(dyn_cast<Instruction>(U));
      continue;
    }

    for (Value *Op : I->operands())
      Enqueue(dyn_cast<Instruction>(Op));
  }
  return Closure;
}

SmallPtrSet<Instruction *, 32> OrigLoopStructure::collectStage1Cone(
    function_ref<bool(const Instruction *)> IsSplitPoint) const {
  // Forward closure from the top-block loads: the candidates a stage-1 split
  // point (e.g. a wide-vector producer; see isStage1SplitPoint) is found among.
  SmallVector<Instruction *, 16> TopLoads;
  topRegion().forEachInstruction([&](Instruction *I) {
    if (isa<LoadInst>(I))
      TopLoads.push_back(I);
  });
  const SmallPtrSet<Instruction *, 32> ReachableFromLoad =
      collectClosure(TopLoads, /*TraverseUsers=*/true);

  // Seed the cone with those split points, then take their forward closure (all
  // descendants within the top).
  SmallVector<Instruction *, 16> SplitPoints;
  for (Instruction *I : ReachableFromLoad)
    if (IsSplitPoint(I))
      SplitPoints.push_back(I);
  return collectClosure(SplitPoints, /*TraverseUsers=*/true);
}

void OrigLoopStructure::collectStages(
    function_ref<bool(const Instruction *)> IsSplitPoint) {
  // Stage 1 is the split-point cone. With no split point every candidate
  // backward-reachable from the inner loop is stage 0.
  const SmallPtrSet<Instruction *, 32> Stage1Set =
      collectStage1Cone(IsSplitPoint);
  if (Stage1Set.empty()) {
    collectStage0FromInnerLoop();
    return;
  }

  // Stage 0 is every other pipeline candidate (the load/address chain).
  topRegion().forEachInstruction([&](Instruction *I) {
    if (!isPipelineCandidate(I))
      return;
    (Stage1Set.count(I) ? Stage1Insts : Stage0Insts).push_back(I);
  });

  LLVM_DEBUG(dbgs() << "    Stages: " << stage0Insts().size() << " stage-0, "
                    << stage1Insts().size() << " stage-1 instructions\n");
}

void OrigLoopStructure::collectStage0FromInnerLoop() {
  // The fallback only follows SSA def-use edges from the inner loop. Do not
  // pipeline when the top contains an effectful candidate: its consumer may be
  // connected through target state rather than SSA, so moving its operands
  // without moving the operation would be incorrect.
  // TODO: We might wanna fix this fallback collection later to not only follow
  // SSA edges, if it turns out beneficial in practice. For now, we bail.
  bool HasTopSideEffects = false;
  topRegion().forEachInstruction([&](Instruction *I) {
    HasTopSideEffects |= isPipelineCandidate(I) && I->mayHaveSideEffects();
  });
  if (HasTopSideEffects)
    return;

  SmallVector<Instruction *, 16> Seeds;
  auto Seed = [&](Value *V) {
    if (auto *I = dyn_cast<Instruction>(V))
      Seeds.push_back(I);
  };
  // Seed from values used inside the inner loop.
  for (BasicBlock *BB : getInnerLoop()->blocks())
    for (Instruction &I : *BB)
      for (Value *Op : I.operands())
        Seed(Op);
  // Seed from initial values of inner header PHIs (from the preheader).
  for (PHINode &PHI : getInnerHeader()->phis())
    for (unsigned I = 0; I < PHI.getNumIncomingValues(); ++I)
      if (PHI.getIncomingBlock(I) == getTop())
        Seed(PHI.getIncomingValue(I));

  const SmallPtrSet<Instruction *, 32> Visited =
      collectClosure(Seeds, /*TraverseUsers=*/false);

  // Emit the reached candidates in region program order.
  topRegion().forEachInstruction([&](Instruction *I) {
    if (isPipelineCandidate(I) && Visited.count(I))
      Stage0Insts.push_back(I);
  });
}

void OrigLoopStructure::collectLeanStage0(const TargetTransformInfo &TTI) {
  SmallVector<Instruction *, 16> TopLoads;
  SmallVector<Instruction *, 16> TopSingleUserLoads;
  topRegion().forEachInstruction([&](Instruction *I) {
    if (!isa<LoadInst>(I))
      return;
    TopLoads.push_back(I);
    if (I->hasOneUser())
      TopSingleUserLoads.push_back(I);
  });

  SmallPtrSet<Instruction *, 32> Stage0Set =
      collectClosure(TopLoads, /*TraverseUsers=*/false);
  for (Instruction *Load : TopSingleUserLoads) {
    auto *User = cast<Instruction>(*Load->user_begin());
    if (!TTI.isLeanStage0Intrinsic(*User))
      continue;
    const SmallPtrSet<Instruction *, 32> IntrinsicClosure =
        collectClosure({User}, /*TraverseUsers=*/false);
    Stage0Set.insert(IntrinsicClosure.begin(), IntrinsicClosure.end());
  }

  topRegion().forEachInstruction([&](Instruction *I) {
    if (!isPipelineCandidate(I))
      return;
    (Stage0Set.count(I) ? Stage0Insts : Stage1Insts).push_back(I);
  });

  LLVM_DEBUG(dbgs() << "    Stages: " << stage0Insts().size() << " stage-0, "
                    << stage1Insts().size() << " stage-1 instructions\n");
}

SmallVector<Instruction *, 16>
AIEOuterLoopPipeliner::remapToClone(ArrayRef<Instruction *> Insts,
                                    const RemapTable &VMap) {
  SmallVector<Instruction *, 16> Out;
  for (Instruction *I : Insts) {
    auto It = VMap.find(I);
    Out.push_back(It != VMap.end()
                      ? cast<Instruction>(static_cast<Value *>(It->second))
                      : I);
  }
  return Out;
}

void AIEOuterLoopPipeliner::cloneStage0IntoPreheader(
    const OrigLoopStructure &OrigLS, CloneLoopStructure &SteadyLS,
    RemapTable &PreheaderVMap) {
  Function *F = SteadyLS.getTop()->getParent();
  BasicBlock *Preheader = SteadyLS.getPreheader();

  // Seed PreheaderVMap with the entry (preheader) values of the top block PHIs
  // so the entry clone's loads use the entry pointers.
  seedHeaderPhiEdge(PreheaderVMap, SteadyLS, Preheader);

  BasicBlock *Stage0Top =
      BasicBlock::Create(F->getContext(), "stage0.top", F, SteadyLS.getTop());
  PreheaderVMap[SteadyLS.getTop()] = Stage0Top;
  cloneAndRemapInsts(remapToClone(OrigLS.stage0Insts(), SteadyLS.cloneMap()),
                     *Stage0Top, Stage0Top->end(), PreheaderVMap, ".top");

  BranchInst::Create(SteadyLS.getTop(), Stage0Top);
  Preheader->getTerminator()->replaceSuccessorWith(SteadyLS.getTop(),
                                                   Stage0Top);

  // The stage-0 top block is now the preheader for the steady LS.
  SteadyLS.installPreheader(Stage0Top);
  LLVM_DEBUG(dbgs() << "    Created stage-0 top block: " << Stage0Top->getName()
                    << "\n");
}

Instruction *AIEOuterLoopPipeliner::cloneInstInto(Instruction &I,
                                                  BasicBlock &Dest,
                                                  BasicBlock::iterator InsertPt,
                                                  RemapTable &VMap,
                                                  const Twine &Suffix) {
  Instruction *Clone = I.clone();
  SmallString<32> SuffixStorage;
  StringRef SuffixStr = Suffix.toStringRef(SuffixStorage);
  if (!SuffixStr.empty() && !Clone->getType()->isVoidTy())
    Clone->setName(I.getName() + SuffixStr);
  Clone->insertBefore(Dest, InsertPt);
  VMap[&I] = Clone;
  return Clone;
}

void AIEOuterLoopPipeliner::remapClones(ArrayRef<Instruction *> Clones,
                                        RemapTable &VMap) {
  for (Instruction *CloneI : Clones)
    RemapInstruction(CloneI, VMap,
                     RF_NoModuleLevelChanges | RF_IgnoreMissingLocals);
}

SmallVector<Instruction *, 16> AIEOuterLoopPipeliner::cloneAndRemapInsts(
    ArrayRef<Instruction *> Insts, BasicBlock &DstBB,
    BasicBlock::iterator InsertPt, RemapTable &VMap, const Twine &Suffix) {
  SmallVector<Instruction *, 16> Clones;
  for (Instruction *I : Insts)
    Clones.push_back(cloneInstInto(*I, DstBB, InsertPt, VMap, Suffix));
  remapClones(Clones, VMap);
  return Clones;
}

CloneLoopStructure::CloneLoopStructure(const LoopStructure &Src,
                                       const Twine &Suffix) {
  Function *F = Src.getTop()->getParent();

  // Blocks in program order; CloneBasicBlock seeds CloneMap (src->clone).
  SmallVector<BasicBlock *, 8> OrigBlocks = Src.blocksInProgramOrder();

  SmallString<32> SuffixStorage;
  StringRef SuffixStr = Suffix.toStringRef(SuffixStorage);
  SmallVector<BasicBlock *, 8> CloneBlocks;
  for (BasicBlock *BB : OrigBlocks) {
    BasicBlock *CB = CloneBasicBlock(BB, CloneMap, "." + SuffixStr, F);
    // Move the clone before its original so it inherits the program-order slot
    // once the original LS is deleted.
    CB->moveBefore(BB);
    CloneMap[BB] = CB;
    CloneBlocks.push_back(CB);
  }

  // Remap within-LS references; edges leaving the LS stay pointed at the
  // originals for the caller to rewire (they are absent from CloneMap).
  remapInstructionsInBlocks(CloneBlocks, CloneMap);

  InnerExit = clonedBlock(Src.getInnerExit());
  for (BasicBlock *BB : Src.getInnerBlocks())
    InnerLoopBlocks.push_back(clonedBlock(BB));
  OuterLoopID = Src.getOuterLoopID();

  // Mirror the regions onto the clones so membership queries work on the clone.
  SmallVector<BasicBlock *, 4> CloneTopBlocks;
  for (BasicBlock *BB : Src.topRegion().blocks())
    CloneTopBlocks.push_back(clonedBlock(BB));
  TopRegion.assign(CloneTopBlocks);
  BottomRegion.assign({clonedBlock(Src.getBottom())});

  // Copy the cached latch condition; its pointers still reference the source LS
  // until remapBoundThroughCloneMap retargets them to this clone.
  OuterLoopCondition = Src.latchCondition();

  // Label clones by <copy>.stage1.<position>; the bottom block also hosts the
  // next iteration's stage-0 prefetch, hence the compound name.
  getTop()->setName(SuffixStr + ".stage1.top");
  getBottom()->setName(SuffixStr + ".stage1.bottom.and.stage0.top");
  for (auto [Orig, Clone] : zip(Src.getInnerBlocks(), getInnerBlocks()))
    Clone->setName(SuffixStr + ".stage1.inner." + Orig->getName());
}

CloneLoopStructure::CloneLoopStructure(const LoopStructure &Src) {
  OuterLoopID = Src.getOuterLoopID();
  Function *F = Src.getTop()->getParent();
  LLVMContext &Ctx = F->getContext();

  // The last-iteration LS is spliced just before the source loop's exit
  // successor; all its blocks are created before that block.
  BasicBlock *Exit = Src.getExitBlock();

  // The last-iteration top is also its inner preheader; inner-loop PHIs
  // incoming from the source top now come from it.
  BasicBlock *LastIterTop =
      BasicBlock::Create(Ctx, "lastiter.stage1.top", F, Exit);
  CloneMap[Src.getTop()] = LastIterTop;

  // One empty clone per inner-loop block, named lastiter.stage1.inner.<orig>,
  // recorded in CloneMap so the body cloners can resolve each block to its
  // clone.
  for (BasicBlock *BB : Src.getInnerBlocks()) {
    BasicBlock *Clone = BasicBlock::Create(
        Ctx, "lastiter.stage1.inner." + BB->getName(), F, Exit);
    CloneMap[BB] = Clone;
    InnerLoopBlocks.push_back(Clone);
  }

  // The last-iteration bottom is also the inner exit, so the cloned inner loop
  // and stores resolve their exits to it. It carries no stage-0 prefetch.
  BasicBlock *LastIterBottom =
      BasicBlock::Create(Ctx, "lastiter.stage1.bottom", F, Exit);
  CloneMap[Src.getBottom()] = LastIterBottom;
  if (Src.getInnerExit() == Src.getBottom())
    CloneMap[Src.getInnerExit()] = LastIterBottom;

  InnerExit = LastIterBottom;
  TopRegion.assign({LastIterTop});
  BottomRegion.assign({LastIterBottom});
}

void AIEOuterLoopPipeliner::swapInClonedLS(const OrigLoopStructure &OrigLS,
                                           CloneLoopStructure &SteadyLS) const {
  BasicBlock *Preheader = OrigLS.getPreheader();

  // Redirecting leaves the original LS unreachable, so capture the preheader on
  // the clone now — LoopInfo can no longer recover it afterwards.
  Preheader->getTerminator()->replaceSuccessorWith(OrigLS.getTop(),
                                                   SteadyLS.getTop());
  SteadyLS.recordExistingPreheader(Preheader);

  // The steady latch remains connected to the exit until the last iteration
  // takes ownership of the exit-PHI incoming edge.
}

Value *CloneLoopStructure::lastIterInputFor(Value *Src) const {
  Value *Clone = cloneOf(Src);
  // A header PHI of this loop carries the next-iteration value on its back
  // edge; that is what the peeled final iteration consumes.
  if (auto *PHI = dyn_cast<PHINode>(Clone); PHI && PHI->getParent() == getTop())
    return PHI->getIncomingValueForBlock(getBottom());
  return Clone;
}

void CloneLoopStructure::remapBoundThroughCloneMap() {
  auto Map = [&](Value *V) -> Value * { return V ? cloneOf(V) : V; };
  LatchConditionInfo &B = *OuterLoopCondition;
  B.Cmp = cast<ICmpInst>(Map(B.Cmp));
  B.Limit = Map(B.Limit);
  B.Counter = cast_or_null<BinaryOperator>(Map(B.Counter));
  B.IV = cast_or_null<PHINode>(Map(B.IV));
}

void OrigLoopStructure::removeFromCFG() const {
  SmallVector<BasicBlock *, 8> Dead = blocksInProgramOrder();

  // Remove external PHI incoming edges before deleting unreachable blocks.
  SmallPtrSet<BasicBlock *, 8> DeadSet(Dead.begin(), Dead.end());
  for (BasicBlock *BB : Dead) {
    for (BasicBlock *Succ : successors(BB)) {
      if (DeadSet.contains(Succ))
        continue;
      for (PHINode &PN : make_early_inc_range(Succ->phis()))
        while (PN.getBasicBlockIndex(BB) >= 0)
          PN.removeIncomingValue(BB, /*DeletePHIIfEmpty=*/false);
    }
    BB->getTerminator()->eraseFromParent();
    new UnreachableInst(BB->getContext(), BB);
  }

  DeleteDeadBlocks(Dead);
}

void AIEOuterLoopPipeliner::cloneStage0IntoBottom(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS,
    RemapTable &BottomVMap) {
  // Seed BottomVMap with the next-iteration (latch incoming) values of the
  // outer header PHIs so the cloned loads prefetch the next iteration's
  // pointers.
  seedHeaderPhiEdge(BottomVMap, SteadyLS, SteadyLS.getBottom());

  SmallVector<Instruction *, 16> Stage0Insts =
      remapToClone(OrigLS.stage0Insts(), SteadyLS.cloneMap());
  Instruction *LatchTerm = SteadyLS.getBottom()->getTerminator();
  cloneAndRemapInsts(Stage0Insts, *SteadyLS.getBottom(),
                     LatchTerm->getIterator(), BottomVMap, ".bottom");
  LLVM_DEBUG(dbgs() << "    Cloned stage-0 into bottom block\n");
}

void AIEOuterLoopPipeliner::createPipelinedPHIs(const OrigLoopStructure &OrigLS,
                                                CloneLoopStructure &SteadyLS,
                                                const RemapTable &PreheaderVMap,
                                                const RemapTable &BottomVMap) {
  BasicBlock *Preheader = SteadyLS.getPreheader();
  Instruction *InsertPt = &*SteadyLS.getTop()->getFirstInsertionPt();

  // Walk OrigLS stage-0 and its steady clones together: the merged PHI replaces
  // the steady clone, and SteadyLS's map is retargeted Orig -> PHI so the slot
  // resolves to the merged PHI afterwards (e.g. for lastIterInputFor).
  SmallVector<Instruction *, 16> SteadyClones;
  for (Instruction *OrigI : OrigLS.stage0Insts()) {
    Instruction *I = cast<Instruction>(SteadyLS.cloneOf(OrigI));
    SteadyClones.push_back(I);
    // Void-typed instructions produce no value to merge; each path runs its
    // own clone.
    if (I->getType()->isVoidTy())
      continue;
    auto PreheaderIt = PreheaderVMap.find(I);
    auto BottomIt = BottomVMap.find(I);
    // Both cloners add every stage-0 entry, so every non-void inst is in both.
    assert(
        PreheaderIt != PreheaderVMap.end() && BottomIt != BottomVMap.end() &&
        "Stage-0 instruction must be in both the preheader and bottom VMaps");
    PHINode *PHI = PHINode::Create(I->getType(), 2, I->getName() + ".phi");
    PHI->insertBefore(InsertPt->getIterator());
    PHI->addIncoming(PreheaderIt->second, Preheader);
    PHI->addIncoming(BottomIt->second, SteadyLS.getBottom());

    // The inner-loop and intra-top uses now read the merge PHI; the preheader /
    // bottom clones keep using their own mapped values.
    I->replaceAllUsesWith(PHI);
    SteadyLS.retargetClone(OrigI, PHI);
  }

  // Erase the now-dead steady stage-0 clones (reverse order).
  for (Instruction *I : reverse(SteadyClones))
    if (I->use_empty())
      I->eraseFromParent();
}

void AIEOuterLoopPipeliner::peelLastIteration(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS) {
  // Build the empty last-iteration LS as a structural clone of OrigLS; it owns
  // its orig->lastiter clone map.
  CloneLoopStructure LastIterLS(OrigLS);
  RemapTable &LastIterMap = LastIterLS.cloneMap();

  // Seed the live-ins: stage-0 slots and loop-carried PHIs enter the last
  // iteration as the values SteadyLS prefetched / carried on its back edge.
  seedLastIterInputs(LastIterMap, OrigLS, SteadyLS);

  // Fill the top (hardware-loop setup + stage-1) before the inner loop, so its
  // PHIs referencing stage-1 results resolve.
  cloneHardwareLoopSetupInto(LastIterLS, OrigLS);
  cloneAndRemapInsts(OrigLS.stage1Insts(), *LastIterLS.getTop(),
                     LastIterLS.getTop()->end(), LastIterMap, ".lastiter");
  cloneInnerLoopIntoLastIter(OrigLS, LastIterLS);
  populateLastIterBottom(OrigLS, LastIterLS);
  wireLastIterIntoCFG(OrigLS, SteadyLS, LastIterLS);

  LLVM_DEBUG(dbgs() << "    Created last-iteration: "
                    << LastIterLS.getTop()->getName() << " -> "
                    << LastIterLS.getBottom()->getName() << "\n");
}

void AIEOuterLoopPipeliner::seedLastIterInputs(
    RemapTable &Map, const OrigLoopStructure &OrigLS,
    const CloneLoopStructure &SteadyLS) const {
  // Each stage-0 slot enters as its steady prefetch value.
  for (Instruction *I : OrigLS.stage0Insts())
    if (!I->getType()->isVoidTy())
      Map[I] = SteadyLS.lastIterInputFor(I);

  // Each loop-carried header PHI enters as its steady next-iteration value.
  // The outer IV is dead in the single last iteration, and for a JNZD hardware
  // loop its steady clone has been erased (eraseOldCounterCycle), so always
  // skip it.
  const PHINode *IV = OrigLS.latchCondition().IV;
  for (PHINode &PHI : OrigLS.getTop()->phis())
    if (&PHI != IV)
      Map[&PHI] = SteadyLS.lastIterInputFor(&PHI);
}

void AIEOuterLoopPipeliner::cloneHardwareLoopSetupInto(
    CloneLoopStructure &LastIterLS, const LoopStructure &Src) const {
  SmallVector<Instruction *, 4> SetupInsts;
  for (Instruction &I : *Src.getTop()) {
    if (I.isTerminator())
      break;
    if (AIEIRUtils::isHardwareLoopSetup(&I))
      SetupInsts.push_back(&I);
  }
  BasicBlock *Dest = LastIterLS.getTop();
  cloneAndRemapInsts(SetupInsts, *Dest, Dest->end(), LastIterLS.cloneMap(),
                     /*Suffix=*/"");
}

void AIEOuterLoopPipeliner::cloneInnerLoopIntoLastIter(
    const OrigLoopStructure &OrigLS, CloneLoopStructure &LastIterLS) const {
  RemapTable &LastIterMap = LastIterLS.cloneMap();
  // Clone every inner-loop body into its skeleton clone first, so all
  // cross-block references exist before any remap runs.
  SmallVector<Instruction *, 32> Clones;
  for (BasicBlock *Orig : OrigLS.getInnerBlocks()) {
    auto *Clone = cast<BasicBlock>(LastIterMap[Orig]);
    for (Instruction &Inst : *Orig)
      Clones.push_back(
          cloneInstInto(Inst, *Clone, Clone->end(), LastIterMap, ".lastiter"));
  }
  remapClones(Clones, LastIterMap);

  BranchInst::Create(LastIterLS.getInnerHeader(), LastIterLS.getTop());
}

void AIEOuterLoopPipeliner::populateLastIterBottom(
    const OrigLoopStructure &OrigLS, CloneLoopStructure &LastIterLS) const {
  // Clone the pristine original latch. Accumulated values are read after the
  // loop and must be cloned.
  const LatchConditionInfo &Bound = OrigLS.latchCondition();

  // The outer back-edge control (counter add + exit icmp) is dead in the
  // non-looping last iteration; correct behavior is to always elide it. (For a
  // JNZD hardware loop the steady counter/limit are also already erased, so it
  // MUST be elided.)

  SmallVector<Instruction *, 16> OrigBottomInsts;
  for (Instruction &I : *OrigLS.getBottom()) {
    if (I.isTerminator())
      break;
    // No prefetch in the last-iteration.
    if (isa<LoadInst>(&I))
      continue;
    if (&I == Bound.Counter || &I == Bound.Cmp)
      continue;
    OrigBottomInsts.push_back(&I);
  }
  BasicBlock *LastIterBottom = LastIterLS.getBottom();
  cloneAndRemapInsts(OrigBottomInsts, *LastIterBottom, LastIterBottom->end(),
                     LastIterLS.cloneMap(), ".lastiter");
}

void AIEOuterLoopPipeliner::wireLastIterIntoCFG(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS,
    const CloneLoopStructure &LastIterLS) const {
  BasicBlock *OrigExit = OrigLS.getExitBlock();
  BasicBlock *LastIterBottom = LastIterLS.getBottom();
  BranchInst::Create(OrigExit, LastIterBottom);

  reroutePhiIncomings(OrigExit, OrigLS.getBottom(), LastIterBottom,
                      PhiEdge::Repoint,
                      [&](Value *V) { return LastIterLS.cloneOf(V); });

  // Non-phi exit live-outs: when the latch dominates the exit there is no LCSSA
  // phi, so LCSSA rematerializes the live-out as a plain instruction in the
  // exit whose operands reference loop-internal values (e.g. the outer-header
  // PHI and an inner-loop def). removeFromCFG deletes those originals, which
  // would leave the exit reading poison. Remap each such operand to a live
  // clone so the exit reads a defined value.
  for (Instruction &I : *OrigExit) {
    if (isa<PHINode>(&I))
      continue;
    for (Use &Op : I.operands()) {
      Value *V = Op.get();
      if (!isa<Instruction>(V) && !isa<Argument>(V))
        continue;
      if (Value *Mapped = LastIterLS.cloneOf(V); Mapped != V)
        Op.set(Mapped);
    }
  }

  SteadyLS.getLatchBranch()->replaceSuccessorWith(OrigExit,
                                                  LastIterLS.getTop());
}

void AIEOuterLoopPipeliner::wireSteadyIntoExit(
    const OrigLoopStructure &OrigLS, const CloneLoopStructure &SteadyLS) const {
  BasicBlock *OrigExit = OrigLS.getExitBlock();
  reroutePhiIncomings(OrigExit, OrigLS.getBottom(), SteadyLS.getBottom(),
                      PhiEdge::Repoint,
                      [&](Value *V) { return SteadyLS.cloneOf(V); });

  // See wireLastIterIntoCFG: exit instructions may directly reference values
  // defined in the original loop and must instead use their steady clones
  // before the original is deleted.
  for (Instruction &I : *OrigExit) {
    if (isa<PHINode>(&I))
      continue;
    for (Use &Op : I.operands()) {
      Value *V = Op.get();
      if (!isa<Instruction>(V) && !isa<Argument>(V))
        continue;
      if (Value *Mapped = SteadyLS.cloneOf(V); Mapped != V)
        Op.set(Mapped);
    }
  }
}

bool BlockRegion::isSingleEntrySingleExit() const {
  for (BasicBlock *BB : Blocks) {
    if (BB != Blocks.front())
      for (BasicBlock *Pred : predecessors(BB))
        if (!contains(Pred))
          return false;
    if (BB != Blocks.back())
      for (BasicBlock *Succ : successors(BB))
        if (!contains(Succ))
          return false;
  }
  return true;
}

void BlockRegion::assign(ArrayRef<BasicBlock *> BBs) {
  Blocks.assign(BBs.begin(), BBs.end());
  assert(isSingleEntrySingleExit() && "BlockRegion must be single-entry "
                                      "single-exit");
}

void BlockRegion::forEachInstruction(
    function_ref<void(Instruction *)> Visit) const {
  for (BasicBlock *BB : Blocks)
    for (Instruction &I : *BB)
      Visit(&I);
}

std::unique_ptr<OrigLoopStructure> OrigLoopStructure::tryBuildFrom(Loop *L) {
  // Private constructor: a validated LS is only reachable through here.
  auto LS = std::unique_ptr<OrigLoopStructure>(new OrigLoopStructure(L));
  if (!LS->analyzeLoopStructure())
    return nullptr;
  return LS;
}

bool OrigLoopStructure::isRegionInternalPhi(const PHINode *PHI) const {
  for (const BasicBlock *IB : PHI->blocks())
    if (!TopRegion.contains(IB))
      return false;
  return true;
}

bool OrigLoopStructure::isPipelineableValue(const Instruction *I) const {
  if (!isInTop(I))
    return false;
  if (const auto *PHI = dyn_cast<PHINode>(I))
    return isRegionInternalPhi(PHI);
  return true;
}

bool OrigLoopStructure::isPipelineCandidate(const Instruction *I) const {
  if (I->isTerminator() || AIEIRUtils::isHardwareLoopSetup(I))
    return false;
  return isPipelineableValue(I);
}

BasicBlock *LoopStructure::getInnerLatch() const {
  for (BasicBlock *Pred : predecessors(getInnerHeader()))
    if (is_contained(InnerLoopBlocks, Pred))
      return Pred;
  return nullptr;
}

SmallVector<BasicBlock *, 8> LoopStructure::blocksInProgramOrder() const {
  SmallVector<BasicBlock *, 8> Blocks;
  Blocks.push_back(getTop());
  Blocks.append(getInnerBlocks().begin(), getInnerBlocks().end());
  // OuterLatch == InnerExit (validated in analyzeLoopStructure), so it is not
  // in InnerBlocks; append it once.
  Blocks.push_back(getBottom());
  return Blocks;
}

BasicBlock *LoopStructure::getExitBlock() const {
  BranchInst *LatchBr = getLatchBranch();
  assert(LatchBr->isConditional() && "Bottom block must be conditional");
  for (BasicBlock *Succ : LatchBr->successors())
    if (Succ != getTop())
      return Succ;
  llvm_unreachable("Bottom block must have an exit successor");
}

void CloneLoopStructure::recordExistingPreheader(BasicBlock *BB) {
  assert(llvm::all_of(
             getTop()->phis(),
             [&](PHINode &PHI) { return PHI.getBasicBlockIndex(BB) >= 0; }) &&
         "header PHIs must already reference the recorded preheader");
  OuterPreheader = BB;
}

void CloneLoopStructure::installPreheader(BasicBlock *NewPreheader) {
  BasicBlock *OldPreheader = getPreheader();
  reroutePhiIncomings(getTop(), OldPreheader, NewPreheader, PhiEdge::Repoint);
  OuterPreheader = NewPreheader;
}

// The `add Counter, C` (with constant C) that defines Counter, or nullptr.
static BinaryOperator *getConstantStepAdd(Value *Counter) {
  auto *Add = dyn_cast<BinaryOperator>(Counter);
  const bool IsConstantStepAdd = Add && Add->getOpcode() == Instruction::Add &&
                                 isa<ConstantInt>(Add->getOperand(1));
  return IsConstantStepAdd ? Add : nullptr;
}

// The induction step the predicate implies for a unit-stride loop, or nullopt
// for EQ/NE (ambiguous without a visible constant step).
static std::optional<int64_t> stepFromPredicate(ICmpInst::Predicate Pred) {
  if (Pred == ICmpInst::ICMP_SLT || Pred == ICmpInst::ICMP_ULT)
    return 1;
  if (Pred == ICmpInst::ICMP_SGT || Pred == ICmpInst::ICMP_UGT)
    return -1;
  return std::nullopt;
}

// The counting PHI in Header that feeds Add, or nullptr.
static PHINode *findCountingPhi(BinaryOperator *Add, const BasicBlock *Header) {
  for (Value *Op : Add->operands()) {
    auto *PHI = dyn_cast<PHINode>(Op);
    if (PHI && PHI->getParent() == Header)
      return PHI;
  }
  return nullptr;
}

bool OrigLoopStructure::canAdjustLoopBound() {
  auto *BI = dyn_cast<BranchInst>(getBottom()->getTerminator());
  if (!BI || !BI->isConditional())
    return false;
  auto *Cmp = dyn_cast<ICmpInst>(BI->getCondition());
  if (!Cmp)
    return false;

  OuterLoopCondition =
      LatchConditionInfo::tryCreate(getOuterLoop(), Cmp, getTop());
  return OuterLoopCondition.has_value();
}

std::optional<LatchConditionInfo>
LatchConditionInfo::tryCreate(const Loop *L, ICmpInst *Cmp,
                              const BasicBlock *Header) {
  // Split Cmp's operands into its loop-invariant limit (with operand index)
  // and the remaining counter operand.
  Value *Limit = nullptr;
  unsigned LimitIdx = 0;
  Value *Counter = nullptr;
  for (unsigned I = 0; I < 2; ++I) {
    Value *Op = Cmp->getOperand(I);
    if (L->isLoopInvariant(Op)) {
      Limit = Op;
      LimitIdx = I;
      Counter = Cmp->getOperand(1 - I);
      break;
    }
  }
  if (!Limit)
    return std::nullopt;

  // The step comes from the counting add's constant; a zero/absent constant
  // falls back to the predicate's unit stride.
  BinaryOperator *CounterAdd = getConstantStepAdd(Counter);
  int64_t Step =
      CounterAdd ? cast<ConstantInt>(CounterAdd->getOperand(1))->getSExtValue()
                 : 0;
  if (Step == 0) {
    std::optional<int64_t> PredStep = stepFromPredicate(Cmp->getPredicate());
    if (!PredStep) {
      LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: EQ/NE with no "
                           "visible constant step\n");
      return std::nullopt;
    }
    Step = *PredStep;
  }

  // A plain-PHI counter has no add to identify IV from, so its step is
  // unverifiable and JNZD conversion is impossible; reject it.
  if (!CounterAdd) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: counter is not an add "
                         "instruction\n");
    return std::nullopt;
  }

  PHINode *IV = findCountingPhi(CounterAdd, Header);
  if (!IV) {
    LLVM_DEBUG(dbgs() << "    Cannot adjust loop bound: counting PHI not "
                         "found in top block\n");
    return std::nullopt;
  }

  return LatchConditionInfo{Cmp, Limit, LimitIdx, Step, CounterAdd, IV};
}

// NewLimit = Limit - Step covers increment (Step > 0) and decrement
// (Step < 0) loops of any constant step magnitude.
void CloneLoopStructure::adjustLoopBound() const {
  const LatchConditionInfo &B = latchCondition();
  IRBuilder<> Builder(getPreheader()->getTerminator());
  Value *NewLimit = Builder.CreateSub(
      B.Limit, ConstantInt::getSigned(B.Limit->getType(), B.Step),
      "outer.trip.adj");
  B.Cmp->setOperand(B.LimitIdx, NewLimit);
  LLVM_DEBUG(dbgs() << "    Adjusted loop bound: N -> N-1 (step=" << B.Step
                    << ")\n");
}

// Copy Source's hint entries dropping the consumed enable hint, append the
// pipeliner success marker, and self-reference operand 0 as a loop ID requires.
static MDNode *rebuildPipelinedLoopID(LLVMContext &Ctx, MDNode *Source) {
  const std::string EnableHintKey =
      (AIE::LoopOptionOverrides::Prefix + EnableOuterLoopPipelining.ArgStr)
          .str();

  SmallVector<Metadata *, 8> MDs;
  for (unsigned I = 1, E = Source->getNumOperands(); I < E; ++I) {
    MDNode *Entry = cast<MDNode>(Source->getOperand(I));
    auto Key = AIELoopUtils::getMetadataKey(*Entry);
    // Drop the consumed enable hint.
    if (Key && *Key == EnableHintKey)
      continue;
    MDs.push_back(Entry);
  }

  // Append the success marker: !{!"<OuterLoopPipelinedKey>", i64 1}
  MDNode *SuccessEntry = MDNode::get(
      Ctx,
      {MDString::get(Ctx, AIELoopUtils::OuterLoopPipelinedKey),
       ConstantAsMetadata::get(ConstantInt::get(Type::getInt64Ty(Ctx), 1))});
  MDs.push_back(SuccessEntry);

  // Loop IDs require operand 0 to refer to the node itself; reserve that slot
  // before uniquing so replaceOperandWith does not clobber the first hint
  // entry.
  MDs.insert(MDs.begin(), nullptr);
  MDNode *FinalLoopID = MDNode::getDistinct(Ctx, MDs);
  FinalLoopID->replaceOperandWith(0, FinalLoopID);
  return FinalLoopID;
}

void CloneLoopStructure::updateLoopMetadata() const {
  MDNode *LoopID = getOuterLoopID();
  if (!LoopID)
    return;
  LLVMContext &Ctx = getTop()->getContext();

  // Decrement itercount.range (N -> N-1) to reflect the peeled iteration.
  MDNode *AdjustedID = updateIterCounts(
      Ctx, LoopID, /*FixMin=*/[](int64_t V) { return V - 1; },
      /*FixMax=*/[](int64_t V) { return V - 1; });
  MDNode *Source = AdjustedID ? AdjustedID : LoopID;

  // Drop the consumed enable hint and append the success marker.
  MDNode *FinalLoopID = rebuildPipelinedLoopID(Ctx, Source);

  // Write onto the latch terminator (what Loop::setLoopID does internally) so
  // this works on a clone with no LoopInfo Loop.
  getBottom()->getTerminator()->setMetadata(LLVMContext::MD_loop, FinalLoopID);
}

// An intrinsic other than a safe 2D/3D pointer increment, whose unknown side
// effects forbid moving it out of the bottom block.
static bool isUnsafeIntrinsicToLift(const AIEBaseInstrInfo &TII,
                                    const Instruction *I) {
  const auto *II = dyn_cast<IntrinsicInst>(I);
  return II && !isSafePointerIncrementIntrinsic(TII, II->getIntrinsicID());
}

// True if a chain instruction is used by a bottom instruction outside the
// chain (a store, the exit icmp, ...), which pins the chain to the bottom.
static bool hasExternalBottomUser(const LoopStructure &OrigLS,
                                  const SmallPtrSetImpl<Instruction *> &Chain) {
  for (Instruction *I : Chain)
    for (User *U : I->users()) {
      auto *UI = dyn_cast<Instruction>(U);
      if (UI && OrigLS.isInBottom(UI) && !Chain.contains(UI))
        return true;
    }
  return false;
}

std::optional<SmallPtrSet<Instruction *, 16>>
AIEOuterLoopPipeliner::collectLiftableBottomChain(
    const OrigLoopStructure &OrigLS, PHINode &PHI) const {
  Value *LatchVal = PHI.getIncomingValueForBlock(OrigLS.getBottom());
  auto *LatchInst = dyn_cast<Instruction>(LatchVal);
  const bool BottomDefinedBackEdge = LatchInst && OrigLS.isInBottom(LatchInst);
  if (!BottomDefinedBackEdge)
    return std::nullopt;

  // Collect the bottom operands feeding the back-edge value; bail if the
  // chain reaches an inner-loop value or an unsafe intrinsic.
  SmallPtrSet<Instruction *, 16> Chain;
  SmallVector<Instruction *, 16> Worklist;
  Chain.insert(LatchInst);
  Worklist.push_back(LatchInst);
  while (!Worklist.empty()) {
    Instruction *I = Worklist.pop_back_val();
    if (isUnsafeIntrinsicToLift(*TII, I))
      return std::nullopt;

    for (Value *Op : I->operands()) {
      auto *OpI = dyn_cast<Instruction>(Op);
      if (!OpI)
        continue;
      // A value computed in the inner loop cannot be lifted above it.
      const bool DependsOnInnerLoopValue =
          OrigLS.getInnerLoop()->contains(OpI->getParent());
      if (DependsOnInnerLoopValue)
        return std::nullopt;
      const bool ExtendsChain = OrigLS.isInBottom(OpI);
      if (!ExtendsChain)
        continue;
      const bool NewlyReached = Chain.insert(OpI).second;
      if (NewlyReached)
        Worklist.push_back(OpI);
    }
  }

  if (hasExternalBottomUser(OrigLS, Chain))
    return std::nullopt;

  LLVM_DEBUG(dbgs() << "    PHI " << PHI.getName() << ": lifting chain of "
                    << Chain.size() << " instructions\n");
  return Chain;
}

bool AIEOuterLoopPipeliner::liftBottomPointerUpdatesToTop(
    const OrigLoopStructure &OrigLS) {
  // Union the liftable chains of every PHI (each lifts independently).
  SmallPtrSet<Instruction *, 32> AllLiftable;
  for (PHINode &PHI : OrigLS.getTop()->phis())
    if (auto Chain = collectLiftableBottomChain(OrigLS, PHI))
      AllLiftable.insert(Chain->begin(), Chain->end());

  if (AllLiftable.empty())
    return false;

  // Move the liftable instructions to the top-block end in program order.
  Instruction *InsertPt = OrigLS.getTop()->getTerminator();
  SmallVector<Instruction *, 32> ToLift;
  for (Instruction &I : *OrigLS.getBottom())
    if (AllLiftable.count(&I))
      ToLift.push_back(&I);
  for (Instruction *I : ToLift)
    I->moveBefore(InsertPt->getIterator());

  LLVM_DEBUG(dbgs() << "    Lifted " << ToLift.size()
                    << " instructions from bottom block to top block\n");
  return true;
}

bool AIEOuterLoopPipeliner::performTransformation(
    OrigLoopStructure &OrigLS, const AIE::LoopOptionOverrides &Overrides) {
  liftBottomPointerUpdatesToTop(OrigLS);

  const bool SplitMode = Overrides.get(SplitStages);
  const auto IsSplitPoint = [SplitMode](const Instruction *I) {
    return SplitMode && isStage1SplitPoint(I);
  };

  const bool LeanStage0 = Overrides.get(LeanStage0Mode);
  if (LeanStage0)
    OrigLS.collectLeanStage0(*TTI);
  else
    OrigLS.collectStages(IsSplitPoint);
  if (OrigLS.stage0Insts().empty()) {
    LLVM_DEBUG(dbgs() << "    Could not extract Stage 0\n");
    return false;
  }

  // Lean stage-0 defaults to speculation, regardless of side effects in that
  // stage. Explicit speculative overrides take precedence; non-lean
  // speculation still requires a side-effect-free stage-0 chain.
  const bool Stage0IsSideEffectFree =
      llvm::none_of(OrigLS.stage0Insts(), [](const Instruction *I) {
        return I->mayHaveSideEffects();
      });
  const bool SpeculationEnabled =
      Overrides.hasOverride(SpeculativeLastIteration)
          ? Overrides.get(SpeculativeLastIteration)
          : LeanStage0;
  const bool UseSpeculativeLastIteration =
      SpeculationEnabled && (LeanStage0 || Stage0IsSideEffectFree);

  // Clone the LS into a steady-state copy and swap it into the original's CFG
  // slot; transform steps below run on the clone, leaving OrigLS pristine.
  CloneLoopStructure SteadyLS(OrigLS, "steady");
  swapInClonedLS(OrigLS, SteadyLS);
  SteadyLS.remapBoundThroughCloneMap();

  // Clone the stage-0 chain above the steady header (entry pointer values);
  // this also adopts that block as the steady preheader.
  RemapTable PreheaderVMap;
  cloneStage0IntoPreheader(OrigLS, SteadyLS, PreheaderVMap);

  // Prefetch the stage-0 chain in the bottom block (next-iteration pointer
  // values).
  RemapTable BottomVMap;
  cloneStage0IntoBottom(OrigLS, SteadyLS, BottomVMap);

  // Merge the preheader and bottom copies into the steady header via PHIs.
  createPipelinedPHIs(OrigLS, SteadyLS, PreheaderVMap, BottomVMap);

  const bool OuterIsHardwareLoop =
      EnableOuterLoopHardwareLoop && SteadyLS.latchCondition().isDowncounting();

  if (!UseSpeculativeLastIteration) {
    // Adjust the outer loop trip count from N to N-1. Must happen before the
    // peel so the hardware-loop conversion can find the right icmp to replace.
    SteadyLS.adjustLoopBound();

    // Convert to a JNZD hardware loop. MUST run before peelLastIteration so the
    // counting add/icmp are erased from the latch before the peel clones it.
    if (OuterIsHardwareLoop)
      convertOuterLoopToHardwareLoop(SteadyLS);

    // Create the last-iteration region from the steady loop.
    peelLastIteration(OrigLS, SteadyLS);

    // Adjust itercount metadata to reflect the reduced trip count.
    SteadyLS.updateLoopMetadata();
  } else {
    // Exit live-outs must keep the original IV cycle alive when it is observed
    // outside the loop, before JNZD removes the latch condition.
    wireSteadyIntoExit(OrigLS, SteadyLS);
    const bool OuterIsHardwareLoop = EnableOuterLoopHardwareLoop &&
                                     SteadyLS.latchCondition().isDowncounting();
    if (OuterIsHardwareLoop)
      convertOuterLoopToHardwareLoop(SteadyLS);
    LLVM_DEBUG(dbgs() << "    Speculative last iteration: no last-iteration "
                         "region\n");
  }

  OrigLS.removeFromCFG();

  return true;
}

// The i32 JNZD trip count is the distance from the preheader IV value to the
// live latch limit. The non-speculative path updates that limit to peel one
// iteration, producing N-1; speculative mode leaves the original limit,
// producing N.
static Value *computeJNZDTripCount(const CloneLoopStructure &SteadyLS,
                                   Type *I32Ty) {
  const LatchConditionInfo &Info = SteadyLS.latchCondition();
  BasicBlock *Preheader = SteadyLS.getPreheader();
  IRBuilder<> PreBuilder(Preheader->getTerminator());
  Value *InitN = Info.IV->getIncomingValueForBlock(Preheader);
  Value *Limit = Info.Cmp->getOperand(Info.LimitIdx);
  Value *TripCount = PreBuilder.CreateSub(InitN, Limit, "outer.jnzd.tc");
  if (TripCount->getType() != I32Ty)
    TripCount =
        PreBuilder.CreateZExtOrTrunc(TripCount, I32Ty, "outer.jnzd.tc.i32");
  return TripCount;
}

// Create the outer-header counter PHI seeded from start_loop_iterations in the
// preheader; its back-edge incoming is filled by rewriteLatchToDecrement.
static PHINode *createOuterCounterPHI(const CloneLoopStructure &SteadyLS,
                                      Value *TripCount, Type *I32Ty) {
  BasicBlock *Preheader = SteadyLS.getPreheader();
  IRBuilder<> PreBuilder(Preheader->getTerminator());
  Value *CtrInit = PreBuilder.CreateIntrinsic(
      Intrinsic::start_loop_iterations, {I32Ty}, {TripCount},
      /*FMFSource=*/nullptr, "outer.ctr.init");
  Instruction *InsertPt = &*SteadyLS.getTop()->getFirstInsertionPt();
  PHINode *CtrPHI =
      PHINode::Create(I32Ty, 2, "outer.ctr", InsertPt->getIterator());
  CtrPHI->addIncoming(CtrInit, Preheader);
  return CtrPHI;
}

// Replace the latch condition with loop.decrement.reg(CtrPHI) != 0, keep the
// loop-continue edge as the true successor, close CtrPHI, and DCE the old icmp.
static void rewriteLatchToDecrement(const CloneLoopStructure &SteadyLS,
                                    PHINode *CtrPHI, Type *I32Ty) {
  BranchInst *LatchBr = SteadyLS.getLatchBranch();
  IRBuilder<> LatchBuilder(LatchBr);
  Value *CtrNext =
      LatchBuilder.CreateIntrinsic(Intrinsic::loop_decrement_reg, {I32Ty},
                                   {CtrPHI, ConstantInt::get(I32Ty, 1)},
                                   /*FMFSource=*/nullptr, "outer.ctr.next");
  Value *NewCond = LatchBuilder.CreateICmpNE(
      CtrNext, ConstantInt::get(I32Ty, 0), "outer.loop.cond");

  Value *OldCond = LatchBr->getCondition();
  LatchBr->setCondition(NewCond);
  const bool TopIsTakenSuccessor =
      LatchBr->getSuccessor(0) == SteadyLS.getTop();
  if (!TopIsTakenSuccessor)
    LatchBr->swapSuccessors();
  CtrPHI->addIncoming(CtrNext, SteadyLS.getBottom());
  RecursivelyDeleteTriviallyDeadInstructions(OldCond);
}

// Break the dead counter/IV use cycle that trivial-DCE cannot: poison the IV's
// latch slot when it feeds only the counter, then DCE the add.
static void eraseOldCounterCycle(const CloneLoopStructure &SteadyLS,
                                 const LatchConditionInfo &Info) {
  PHINode *IV = Info.IV;
  if (IV->hasOneUse()) {
    int LatchIdx = IV->getBasicBlockIndex(SteadyLS.getBottom());
    const bool HasLatchIncoming = LatchIdx >= 0;
    if (HasLatchIncoming)
      IV->setIncomingValue(LatchIdx, PoisonValue::get(IV->getType()));
  }
  RecursivelyDeleteTriviallyDeadInstructions(Info.Counter);
}

void AIEOuterLoopPipeliner::convertOuterLoopToHardwareLoop(
    CloneLoopStructure &SteadyLS) {
  const LatchConditionInfo &Info = SteadyLS.latchCondition();
  assert(Info.isDowncounting() &&
         "JNZD conversion needs a fully validated downcounting latch");
  Type *I32Ty = Type::getInt32Ty(SteadyLS.getTop()->getContext());

  Value *TripCount = computeJNZDTripCount(SteadyLS, I32Ty);
  PHINode *CtrPHI = createOuterCounterPHI(SteadyLS, TripCount, I32Ty);
  rewriteLatchToDecrement(SteadyLS, CtrPHI, I32Ty);
  eraseOldCounterCycle(SteadyLS, Info);
  SteadyLS.clearLatchCondition();

  LLVM_DEBUG(dbgs() << "    Converted outer loop to JNZD hardware loop\n");
}
