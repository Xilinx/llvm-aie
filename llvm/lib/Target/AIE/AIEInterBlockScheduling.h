//===- AIEInterblockScheduling.h - Inter-block scheduling logic -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Class providing services for interblock scheduling.
// Supplies the function scope data and carries information from one block to
// another
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEINTERBLOCKSCHEDULING_H
#define LLVM_LIB_TARGET_AIE_AIEINTERBLOCKSCHEDULING_H

#include "AIEBaseSubtarget.h"
#include "AIEBundle.h"
#include "AIEDataDependenceHelper.h"
#include "AIEHazardRecognizer.h"
#include "AIEPostPipeliner.h"
#include "AIERegDefUseTracker.h"
#include "AIESchedulingTypes.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleHazardRecognizer.h"
#include <memory>

namespace llvm::AIE {

/// Tracks original instruction descriptions to enable dematerialization.
/// Records the original MCInstrDesc for each multi-slot pseudo instruction
/// before any materialization (e.g., multislot pseudo materialization) occurs.
/// Also tracks clones to their ultimate original instruction.
/// This allows restoring instructions to their original state if needed.
class MaterializationTracker {
  /// Maps MachineInstr pointers to their original MCInstrDesc.
  DenseMap<const MachineInstr *, const MCInstrDesc *> OriginalDescs;

  /// Maps cloned instructions to their ultimate original instruction.
  /// If Src is cloned to create Clone, we store Clone -> Src.
  /// If Src itself is a clone, we trace back to the ultimate original.
  DenseMap<const MachineInstr *, const MachineInstr *> CloneToOriginal;

public:
  /// Save the original descriptions for multi-slot pseudo instructions
  /// in the function. Only these can be materialized, so only they need
  /// tracking for potential dematerialization.
  void saveFunction(MachineFunction &MF, const AIEBaseInstrInfo &TII) {
    for (MachineBasicBlock &MBB : MF) {
      for (MachineInstr &MI : MBB) {
        if (TII.isMultiSlotPseudo(MI))
          OriginalDescs[&MI] = &MI.getDesc();
      }
    }
  }

  /// Clear all tracked descriptions (e.g., when leaving a function).
  void clear() {
    OriginalDescs.clear();
    CloneToOriginal.clear();
  }

  /// Get the ultimate original instruction for a clone, or MI itself if not a
  /// clone.
  const MachineInstr *getUltimateOriginal(const MachineInstr *MI) const {
    auto It = CloneToOriginal.find(MI);
    return It != CloneToOriginal.end() ? It->second : MI;
  }

  /// Clone an instruction and track the mapping to the ultimate original.
  /// If Src is itself a clone, traces back to the ultimate original.
  MachineInstr *cloneInstr(MachineFunction &MF, const MachineInstr *Src) {
    MachineInstr *Clone = MF.CloneMachineInstr(Src);

    // Find the ultimate original: if Src is a clone, use its original
    const MachineInstr *Ultimate = getUltimateOriginal(Src);

    CloneToOriginal[Clone] = Ultimate;
    return Clone;
  }

  /// Get the original description for an instruction, if tracked.
  /// For clones, traces back to the ultimate original's description.
  const MCInstrDesc *getOriginalDesc(const MachineInstr *MI) const {
    // If MI is a clone, trace to its ultimate original
    const MachineInstr *Original = getUltimateOriginal(MI);

    auto It = OriginalDescs.find(Original);
    return It != OriginalDescs.end() ? It->second : nullptr;
  }

  /// Returns true if the instruction was materialized (desc changed).
  /// For clones, compares against the ultimate original's saved description.
  bool wasMaterialized(const MachineInstr *MI) const {
    const MCInstrDesc *OrigDesc = getOriginalDesc(MI);
    return OrigDesc && OrigDesc != &MI->getDesc();
  }

  /// Restore an instruction to its original opcode (before materialization).
  /// Works for both original instructions and clones.
  /// Returns true if the instruction was dematerialized, false if no
  /// original description was tracked (i.e., it was never materialized).
  bool dematerialize(MachineInstr *MI) {
    const MCInstrDesc *OrigDesc = getOriginalDesc(MI);
    if (!OrigDesc || OrigDesc == &MI->getDesc())
      return false; // Not tracked or already at original opcode
    MI->setDesc(*OrigDesc);
    return true;
  }

  /// Returns the number of tracked original instructions.
  size_t size() const { return OriginalDescs.size(); }

  /// Returns the number of tracked clones.
  size_t cloneCount() const { return CloneToOriginal.size(); }

  /// Returns true if no instructions are being tracked.
  bool empty() const {
    return OriginalDescs.empty() && CloneToOriginal.empty();
  }
};

/// Parameters that drive fixpoint convergence
class FixedpointState {
public:
  SchedulingStage Stage = SchedulingStage::Scheduling;
  // PostPipeliner mode - physical or virtual register mode
  PostPipelinerMode PipelinerMode = PostPipelinerMode::None;
  // Parameters of the loop-aware convergence
  int LatencyMargin = 0;
  SmallMapVector<MachineInstr *, int, 8> PerMILatencyMargin;
  SmallMapVector<MachineInstr *, int, 8> PerMIExtraDepth;
  int ResourceMargin = 0;
  // The II of the modulo schedule we are trying.
  int II = 0;
  // The number of II steps we've made from the minimum
  int IITries = 0;
  // Results from the convergence test
  int MaxLatencyExtent = 0;
  int MaxResourceExtent = 0;
  int NumIters = 0;
  // Lower bound on the issue cycle of the delay slot instruction of an
  // epilogue. Every reschedule moves it one cycle past the position the
  // instruction reached, so it grows strictly. Zero leaves the instruction
  // unconstrained, as in the first schedule.
  unsigned EarliestDelaySlotCycle = 0;
  // The shortest region seen so far and the bound that produced it, so that
  // the block can end on that schedule rather than on the last one tried. A
  // zero length means that no schedule has been recorded yet.
  unsigned BestDelaySlotLength = 0;
  unsigned BestDelaySlotCycle = 0;
  // Set while the delay slot instruction is too far from the end of the block,
  // which wastes some of its delay slots.
  bool DelaySlotRetry = false;
  // Set while the block is scheduled a last time to reproduce the best
  // schedule, so that no further reschedule is requested.
  bool DelaySlotFinalPass = false;
};

// For interblock scheduling we need the original code (SemanticOrder) to
// compute inter-block dependences and the scheduled code (Bundles) to check
// interblock contraints
// Region decomposition is inaccessible from the SchedStrategy, we only
// get enter and leave calls. We construct our Regions from the iterators
// passed to the enterRegion call.
class Region {
  // The instrutions in their original order
  std::vector<MachineInstr *> SemanticOrder;
  // The instruction that starts the next region, if any
  MachineInstr *ExitInstr = nullptr;

  MachineBasicBlock *BB = nullptr;

  /// Instructions that are already scheduled at the top, e.g. an swp epilogue.
  /// Those should not be re-ordered by the scheduler.
  ArrayRef<MachineBundle> TopFixedBundles;

  /// Instructions that are already scheduled at the bottom, e.g. an swp
  /// prologue. Those should not be re-ordered by the scheduler.
  ArrayRef<MachineBundle> BotFixedBundles;

public:
  Region(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
         MachineBasicBlock::iterator End);

  using free_iterator = std::vector<MachineInstr *>::const_iterator;
  using fixed_iterator = MachineBasicBlock::iterator;

  /// Iterate over the "free" instructions in semantic order.
  inline ArrayRef<MachineInstr *> getFreeInstructions() const {
    return SemanticOrder;
  }

  /// Iterate over the instructions that are fixed at the top. Typically those
  /// represent a SWP epilogue.
  inline iterator_range<fixed_iterator> top_fixed_instrs() const {
    fixed_iterator FixedEnd = std::next(BB->begin(), TopFixedBundles.size());
    return make_range(BB->begin(), FixedEnd);
  }
  ArrayRef<MachineBundle> getTopFixedBundles() const { return TopFixedBundles; }

  /// Iterate over the instructions that are fixed at the bottom. Typically
  /// those represent a SWP prologue.
  inline iterator_range<fixed_iterator> bot_fixed_instrs() const {
    fixed_iterator FixedBegin = std::prev(BB->end(), BotFixedBundles.size());
    return make_range(FixedBegin, BB->end());
  }
  ArrayRef<MachineBundle> getBotFixedBundles() const { return BotFixedBundles; }

  /// Set the fixed bundles at the top of the region (e.g. a SWP epilogue).
  /// The instructions must already be physically present at the start of the
  /// block. Trims SemanticOrder to exclude the newly fixed instructions.
  /// \pre The region starts at BB->begin().
  void setTopFixedBundles(ArrayRef<MachineBundle> Bundles);

  /// Set the fixed bundles at the bottom of the region (e.g. a SWP prologue).
  /// The instructions must already be physically present at the end of the
  /// block. Trims SemanticOrder to exclude the newly fixed instructions.
  /// \pre The region ends at BB->end().
  void setBotFixedBundles(ArrayRef<MachineBundle> Bundles);

  MachineInstr *getExitInstr() const { return ExitInstr; }

  std::vector<MachineBundle> Bundles;
};

class BlockState {
  /// This vector is created during the first fixpoint iteration, triggered
  /// by the enterRegion callback
  std::vector<Region> Regions;
  /// Maintain the index of the region that is currently being updated.
  unsigned CurrentRegion = 0;

  /// Per-CFG-successor inter-block DDG edges, built during the DAG mutation
  /// phase by MaxLatencyFinder::buildInterBlockEdges(). Persists into the
  /// initialize() phase so that initializeBotScoreBoard() can also use them.
  std::vector<std::unique_ptr<InterBlockEdges>> PerSuccEdges;

  // This holds an instance of the PostPipeliner for candidate loops.
  std::unique_ptr<PostPipeliner> PostSWP;

  // This holds the information if this is an epilogue of a loop that was
  // optimized by the outer-loop pipeliner.
  bool IsEpilogueOfOuterPipelinedLoop = false;

  // This holds the information whether it is safe to drop memory dependencies,
  // for example, a load -> process -> store using the same pointer, where
  // we know that the store will never overwrite a memory position that
  // will be loaded in a further iteration.
  bool IsSafeToIgnoreMemDeps = false;

  // This holds an instance of the RegLiveRangeTracker for loops.
  std::unique_ptr<llvm::RegLiveRangeTracker> RegTracker;

public:
  BlockState(MachineBasicBlock *Block);
  MachineBasicBlock *TheBlock = nullptr;
  FixedpointState FixPoint;
  BlockType Kind = BlockType::Regular;
  LivePhysRegs LiveOuts;

  /// These are owned bundles of instructions that need to be inserted
  /// in the top and the bottom of the block respectively.
  /// PostPipelined loops use these to push out the epilogue and prologue
  /// in the preheader and exit block.
  std::vector<MachineBundle> TopInsert;
  std::vector<MachineBundle> BottomInsert;

  /// The top-level instructions (BUNDLEs or standalone) that TopInsert and
  /// BottomInsert were emitted as, in block order. top_fixed_instrs() and
  /// bot_fixed_instrs() are positional, so these are moved back to the top and
  /// the bottom of the block before a reschedule.
  SmallVector<MachineInstr *, 8> TopFixedInstrs;
  SmallVector<MachineInstr *, 8> BotFixedInstrs;

  /// For pipelined loop preheaders: a parallel array to the loop body's
  /// SemanticOrder. Each entry is the first-iteration clone from BottomInsert
  /// for the corresponding original loop instruction, or nullptr when that
  /// instruction has no copy in the prologue. Populated by PipelineExtractor
  /// during PipeliningDone.
  std::vector<MachineInstr *> BottomInsertSemanticOrder;

  void initInterBlock(const MachineSchedContext &Context,
                      const AIEHazardRecognizer &HR);

  // Concatenate Bundles to the current region
  void addBundles(const std::vector<MachineBundle> &Bundles) {
    auto &TheBundles = Regions.at(CurrentRegion).Bundles;
    TheBundles.insert(TheBundles.end(), Bundles.begin(), Bundles.end());
  }
  void addRegion(MachineBasicBlock *BB, MachineBasicBlock::iterator RegionBegin,
                 MachineBasicBlock::iterator RegionEnd) {
    // During the gathering pass addRegion is called once per region.
    CurrentRegion = Regions.size();
    Regions.emplace_back(BB, RegionBegin, RegionEnd);
  }
  auto &getCurrentRegion() const { return Regions.at(CurrentRegion); }
  auto &getCurrentRegion() { return Regions[CurrentRegion]; }
  auto &getPostSWP() const { return *PostSWP; }
  const Region &getTop() const { return Regions.back(); }
  Region &getTop() { return Regions.back(); }
  const Region &getBottom() const { return Regions.front(); }
  /// Return the self-loop back-edge DAG from PerSuccEdges, or null if absent.
  /// Populated by buildPerSuccEdges().
  InterBlockEdges *getLoopSelfEdge();
  const InterBlockEdges *getLoopSelfEdge() const;

  std::vector<std::unique_ptr<InterBlockEdges>> &getPerSuccEdges() {
    return PerSuccEdges;
  }
  const std::vector<std::unique_ptr<InterBlockEdges>> &getPerSuccEdges() const {
    return PerSuccEdges;
  }
  const std::vector<Region> &getRegions() const { return Regions; }
  const char *kindAsString() const {
    return Kind == BlockType::Loop       ? "Loop"
           : Kind == BlockType::Epilogue ? "Epilogue"
                                         : "Regular";
  }

  /// Maintains the current region. The need for this is given by the fact that
  /// we record the regions during the first fixpoint iteration, and then
  /// re-traverse them on following ones. So on the first iteration it is the
  /// index of the last region created, on following iterations it is the index
  /// of the region we are curently updating.
  void advanceRegion() { ++CurrentRegion; }
  void resetRegion() { CurrentRegion = 0; }

  /// This prepares for the next fixpoint iteration. The region structure stays
  /// intact, but the actual schedule is cleared.
  /// It rewinds to the first region.
  void clearSchedule();

  void setPipelined();

  /// Initialize for pipelining - virtualizes registers if in virtual mode
  void initPipelining();

  /// Restore after failed pipelining - restores physical registers if
  /// virtualized
  void restorePipelining();

  bool isScheduled() const {
    return FixPoint.Stage == SchedulingStage::SchedulingDone || isPipelined() ||
           pipeliningFailed();
  }
  bool isPipelined() const {
    return FixPoint.Stage == SchedulingStage::PipeliningDone;
  }
  bool pipeliningFailed() const {
    return FixPoint.Stage == SchedulingStage::PipeliningFailed;
  }

  /// return the safety margin that the epilogue of this loop should provide
  /// \pre Kind == Loop
  int getSafetyMargin() const;

  int getScheduleLength() const;

  bool isEpilogueOfOuterPipelinedLoop() const {
    return IsEpilogueOfOuterPipelinedLoop;
  }

  bool isSafeToIgnoreMemDeps() const { return IsSafeToIgnoreMemDeps; }

protected:
  void setBlockProperties();
};

class InterBlockScheduling {
  const MachineSchedContext *Context = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;

  // Captures the command line option from AIEMachineScheduler.cpp
  bool InterBlockScoreboard = true;

  // A hazard recognizer to interpret itineraries
  std::unique_ptr<AIEHazardRecognizer> HR;

  AIEAlternateDescriptors SelectedAltDescs;
  std::map<MachineBasicBlock *, BlockState> Blocks;
  std::vector<MachineBasicBlock *> MBBSequence;
  unsigned NextInOrder = 0;

  // True during the global first pass where regions are gathered for every
  // block before any block is actually scheduled.
  // Once all blocks have been visited this flag is cleared to begin
  // the scheduling pass.
  bool IsGatheringPhase = true;

  /// Return one instruction that needs to be moved higher to avoid a resource
  /// conflict, or nullptr if all resources converged.
  /// \param FindInBottomRegion Whether the conflicting instruction is searched
  ///        in the Bottom or Top region of \p BS.
  MachineInstr *resourcesConverged(BlockState &BS,
                                   bool FindInBottomRegion = true) const;

  /// Return one instruction that needs a higher latency cap, or nullptr if all
  /// latencies converged.
  MachineInstr *latencyConverged(BlockState &BS);

  /// Derive the Kind for \p BS from the current CFG:
  /// A Loop block has an edge to itself and an exit edge.
  /// An Epilogue block is the exit successor of a Loop.
  /// Any other block is Regular.
  /// Epilogue blocks rely on the Loop classifcation, so the
  /// classification runs in two sweeps, one to establish the loops,
  /// the second to find their epilogues.
  /// The Loop state is persistent, i.e. once a loop always a loop.
  /// Epilogues sometimes revert to Regular, when a dedicated loop exit
  /// block is created.
  void classifyBlock(BlockState &BS);

  /// define the scheduling order, loops first then the reset bottom-up over
  /// the control flow graph
  void defineSchedulingOrder(MachineFunction *MF);

  /// Perform the convergence checks and set convergence parameters
  /// for the next iteration.
  /// Returns the stage this block is now in.
  SchedulingStage updateFixPoint(BlockState &BS);

  SchedulingStage updateScheduling(BlockState &BS);
  SchedulingStage updatePipelining(BlockState &BS);

  /// Convergence of the delay slot position in an epilogue. Reschedules the
  /// block with the delay slot instruction held back one more cycle, until it
  /// is close enough to the end of the block to leave no unused delay slots.
  SchedulingStage updateDelaySlots(BlockState &BS);

  /// Emit scheduling remarks for all loop blocks (post/pre/unpipelined).
  void emitLoopRemarks();

  /// Calculate the number of cycles that are needed to respect
  /// latencies related to the loop whose the epilogue is associated
  int getCyclesToRespectTiming(const BlockState &EpilogueBS,
                               const BlockState &LoopBS) const;

  /// Calculate the number of cycles that are needed to avoid resource
  /// conflicts between loop and epilogue
  int getCyclesToAvoidResourceConflicts(int ExistingLatency,
                                        const BlockState &EpilogueBS,
                                        const BlockState &LoopBS) const;

  BlockState *CurrentBlockState = nullptr;

  /// Tracks original instruction descriptions to enable dematerialization.
  /// Populated in enterFunction, cleared in leaveFunction.
  MaterializationTracker MatTracker;

public:
  InterBlockScheduling(const MachineSchedContext *C, bool InterBlock);
  void enterFunction(MachineFunction *MF);
  void leaveFunction();

  /// Return the next block to be scheduled.
  MachineBasicBlock *nextBlock();

  // Set up state for the next iteration of scheduling
  void enterBlock(MachineBasicBlock *BB);

  /// Reap the results from this round of scheduling
  bool leaveBlock();

  /// Record regions and reset state for next iteration.
  void enterRegion(MachineBasicBlock *BB,
                   MachineBasicBlock::iterator RegionBegin,
                   MachineBasicBlock::iterator RegionEnd);

  /// Retrieve the inter-block state for BB
  const BlockState &getBlockState(MachineBasicBlock *BB) const;
  BlockState &getBlockState(MachineBasicBlock *BB);

  /// Collect the inter-block DDG edges that end in \p BB, i.e. the entries of
  /// each predecessor's PerSuccEdges that name BB as their successor. The
  /// graphs stay owned by the predecessors; computing the list on demand keeps
  /// it in step with the CFG and with any rebuild done by buildPerSuccEdges().
  std::vector<InterBlockEdges *> getPerPredEdges(MachineBasicBlock *BB) const;

  /// Return the largest positive latency that each instruction in the top
  /// region of \p BB inherits from an edge out of a predecessor block, keyed
  /// by instruction. Latencies of any dependence kind are considered; zero and
  /// negative ones do not increase depth and are dropped.
  ///
  /// A pred edge graph only spans the bottom region of the predecessor and the
  /// top region of BB, so the result is empty unless BB is currently
  /// scheduling its top region. The self edge of a single-block loop is
  /// excluded as well: its predecessor is BB itself, so there is no separate
  /// schedule that could absorb the latency.
  DenseMap<const MachineInstr *, int>
  getIncomingLatencies(MachineBasicBlock *BB) const;

  /// Return the maximum interblock latency we need to account for
  /// the given successor. This represents the latency margin we assume for
  /// an unscheduled successor.
  std::optional<int> getLatencyCap(MachineInstr &MI) const;

  /// Return the maximum number of cycles to block for the given successor.
  /// This represents the resource usage we assume for an unscheduled successor.
  std::optional<int> getBlockedResourceCap(MachineBasicBlock *BB) const;

  /// Return the number of nops that must be inserted before the epilogue
  /// of the loop represented by this block.
  int getSafetyMargin(MachineBasicBlock *Loop,
                      MachineBasicBlock *Epilogue) const;

  /// Insert the instructions from Bundles into BB before the
  /// iterator Before and applies MIR bundling.
  ///
  /// \param Move Whether the instructions are assumed to be in the block
  ///        already, and need to be moved, not inserted
  /// \param EmitNops Whether to emit a NOP instead of an empty BUNDLE.
  void emitBundles(const std::vector<MachineBundle> &TimedRegion,
                   MachineBasicBlock *BB, MachineBasicBlock::iterator Before,
                   bool Move, bool EmitNops) const;

  /// Emit extra code induced by interblock scheduling:
  /// Safety margins, SWP prologues, SWP epilogues
  void emitTopSafetyMargin(const BlockState &BS);
  void emitInterBlockTop(BlockState &BS);
  void emitInterBlockBottom(const BlockState &BS) const;

  bool tryPipeline(ScheduleDAGMI &DAG, MachineBasicBlock *BB);

  void buildPerSuccEdges(MachineBasicBlock *BB);

  /// Update PerSuccEdges of BB for the successor \p For
  void updatePerSuccEdges(MachineBasicBlock *BB, MachineBasicBlock *For);

  void buildGraph(InterBlockEdges &);

  /// Clear and repopulate the PostDepths of every per-successor inter-block
  /// edge for BB. For scheduled successors, records the actual scheduled cycle
  /// of each instruction; for unscheduled ones, computes a static lower bound
  /// from the inter-block DDG topology. Called from initializeBotScoreBoard
  /// and MaxLatencyFinder::recordPerSuccDepths so both are self-contained.
  void recordPostDepths(MachineBasicBlock *BB);

  AIEAlternateDescriptors &getSelectedAltDescs() { return SelectedAltDescs; }

  const MachineSchedContext *getContext() const { return Context; }
  bool isGatheringPhase() const { return IsGatheringPhase; }

  /// Get the materialization tracker for clone tracking.
  MaterializationTracker &getMatTracker() { return MatTracker; }

  /// Take note of where the delay slot instruction of epilogue \p BB ended up,
  /// and decide whether the block should be scheduled again to place it better.
  /// \p BranchCycle is the cycle it was issued in, \p UnfixedLength and
  /// \p FixedLength the region lengths before and after fixupDelaySlotPosition.
  void recordDelaySlotPosition(MachineBasicBlock *BB, unsigned BranchCycle,
                               unsigned UnfixedLength, unsigned FixedLength,
                               unsigned NumDelaySlots);

  /// The cycle before which the delay slot instruction of \p BB may not be
  /// issued. Zero leaves it unconstrained, as in the first schedule.
  unsigned getEarliestDelaySlotCycle(MachineBasicBlock *BB) const;

  // Returns the scheduled bundles of the pipelined loop body preceding
  // \p Epilogue. Returns nullopt if \p Epilogue is not the epilogue of a
  // pipelined loop.
  std::optional<ArrayRef<MachineBundle>>
  getSWPLoopBundlesForEpilogue(MachineBasicBlock *Epilogue);

  /// If \p LoopMBB is not the only Predecessor of \p CurrentMBB, create a
  /// dedicated Exit MBB by splitting the edge between LoopMBB and CurrentBB
  /// by a new block.
  /// We return the now dediecated exit block of the loopexit block.
  MachineBasicBlock *makeDedicatedLoopExit(MachineBasicBlock *LoopMBB,
                                           MachineBasicBlock *CurrentMBB);
};

} // end namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIEINTERBLOCKSCHEDULING_H
