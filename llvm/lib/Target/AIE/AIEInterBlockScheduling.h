//===- AIEInterblockScheduling.h - Inter-block scheduling logic -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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
#include "AIEHazardRecognizer.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleHazardRecognizer.h"
#include <memory>

namespace llvm::AIE {

/// Class to derive actual helpers from. It is a placeholder for the future
/// stand-alone DDG class, it just implements the unrelated schedule() as
/// a dummy
/// It copies the Mutations mechanism from ScheduleDAGMI; this represents a
/// change of perspective on DAGMutations: They are target-dependent ways to
/// modify the dependence graph, not target-dependent ways to tweak the
/// scheduler.
class DataDependenceHelper : public ScheduleDAGInstrs {
  /// Ordered list of DAG postprocessing steps.
  std::vector<std::unique_ptr<ScheduleDAGMutation>> Mutations;
  const MachineSchedContext &Context;
  void schedule() override{};

public:
  DataDependenceHelper(const MachineSchedContext &Context)
      : ScheduleDAGInstrs(*Context.MF, Context.MLI), Context(Context) {
    auto &Subtarget = Context.MF->getSubtarget();
    auto TT = Subtarget.getTargetTriple();
    for (auto &M : AIEBaseSubtarget::getInterBlockMutationsImpl(TT)) {
      Mutations.emplace_back(std::move(M));
    }
  }
  void buildEdges() {
    ScheduleDAGInstrs::buildEdges(Context.AA);
    for (auto &M : Mutations) {
      M->apply(this);
    }
  }
};

/// This class generates all edges between nodes in two flow-adjacent regions
/// The nodes are added in forward flow order, marking the boundary at the
/// appropriate point.
class InterBlockEdges {
  DataDependenceHelper DDG;
  // the boundary between Pred and Succ nodes
  std::optional<unsigned> Boundary;

  /// We can add the same instruction on both sides of the boundary.
  /// We maintain explicit maps to retrieve the corresponding SUnit
  using IndexMap = std::map<MachineInstr *, unsigned>;
  IndexMap PredMap;
  IndexMap SuccMap;

public:
  InterBlockEdges(const MachineSchedContext &Context) : DDG(Context) {}

  /// Add a Node to the DAG.
  void addNode(MachineInstr *);

  /// Mark the boundary between the predecessor block and the successor block.
  /// In normal operation, there should just be one call to this method.
  /// Nodes added before are part of the predecesor, nodes added after are
  /// part of the successor
  void markBoundary();

  /// Create all the edges by interpreting read and write events of the nodes
  // in reverse order.
  void buildEdges() { DDG.buildEdges(); }

  /// To iterate forward across the SUnits of the underlying DDG.
  auto begin() const { return DDG.SUnits.begin(); }
  auto end() const { return DDG.SUnits.end(); }

  /// The following two methods are used to find the cross-boundary edges,
  /// by starting from a pre-boundary node and select its successor edges that
  /// connect to a post-boundary node.
  /// ---
  /// Retrieve the SUnit that represents MI's instance before the
  /// boundary, null if not found.
  const SUnit *getPreBoundaryNode(MachineInstr *MI) const;
  /// Check whether SU represents an instruction after the boundary
  bool isPostBoundaryNode(SUnit *SU) const;
};

// BlockType determines scheduling priority, direction and safety margin
// handling.
enum class BlockType { Regular, Loop, Epilogue };

/// Parameters that drive fixpoint convergence
class FixedpointState {
public:
  bool IsScheduled = false;
  int LatencyMargin = 0;
  int ResourceMargin = 0;
  // Results from the convergence test
  int MaxLatencyExtent = 0;
  int MaxResourceExtent = 0;
  int NumIters = 0;
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

public:
  Region(MachineBasicBlock *BB, MachineBasicBlock::iterator Begin,
         MachineBasicBlock::iterator End) {
    for (auto It = Begin; It != End; ++It) {
      SemanticOrder.push_back(&*It);
    }
    if (End != BB->end()) {
      ExitInstr = &*End;
    }
  }
  std::vector<MachineInstr *>::const_iterator begin() const {
    return SemanticOrder.begin();
  }
  std::vector<MachineInstr *>::const_iterator end() const {
    return SemanticOrder.end();
  }
  MachineInstr *getExitInstr() const { return ExitInstr; }

  std::vector<MachineBundle> Bundles;
};

class BlockState {
  /// This vector is created during the first fixpoint iteration, triggered
  /// by the enterRegion callback
  std::vector<Region> Regions;
  /// Maintain the index of the region that is currently being updated.
  unsigned CurrentRegion = 0;

  // This holds pre-computed dependences between two blocks.
  // Currently only used for loops, where both blocks are the same.
  std::unique_ptr<InterBlockEdges> BoundaryEdges;

public:
  BlockState(MachineBasicBlock *Block);
  MachineBasicBlock *TheBlock = nullptr;
  FixedpointState FixPoint;
  BlockType Kind = BlockType::Regular;
  void initInterBlock(const MachineSchedContext &Context);

  // Concatenate Bundles to the current region
  void addBundles(const std::vector<MachineBundle> &Bundles) {
    auto &TheBundles = Regions.at(CurrentRegion).Bundles;
    TheBundles.insert(TheBundles.end(), Bundles.begin(), Bundles.end());
  }
  void addRegion(MachineBasicBlock *BB, MachineBasicBlock::iterator RegionBegin,
                 MachineBasicBlock::iterator RegionEnd) {
    CurrentRegion = Regions.size();
    Regions.emplace_back(BB, RegionBegin, RegionEnd);
  }
  auto getCurrentRegion() const { return Regions.at(CurrentRegion); }
  const Region &getTop() const { return Regions.back(); }
  Region &getTop() { return Regions.back(); }
  const Region &getBottom() const { return Regions.front(); }
  const InterBlockEdges &getBoundaryEdges() const {
    assert(Kind == BlockType::Loop);
    return *BoundaryEdges;
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

  /// Inform the fixpoint iteration that we are done with this block.
  void setScheduled();
  bool isScheduled() const { return FixPoint.IsScheduled; }

  /// return the safety margin that the epilogue of this loop should provide
  /// \pre Kind == Loop
  int getSafetyMargin() const;

protected:
  void classify();
};

// Represents how accurate our successor information is
enum class ScoreboardTrust {
  // The bundles represent the true start of the blocks
  Absolute,
  // The bundles are accurate, but may shift at most one cycle
  // due to alignment of a successor block
  AccountForAlign,
  // We don't have bundles for all successors
  Conservative
};

class InterBlockScheduling {
  const MachineSchedContext *Context;

  // Captures the command line option from AIEMachineScheduler.cpp
  bool InterBlockScoreboard = true;

  // A hazard recognizer to interpret itineraries
  std::unique_ptr<AIEHazardRecognizer> HR;

  std::map<MachineBasicBlock *, BlockState> Blocks;
  std::vector<MachineBasicBlock *> MBBSequence;
  unsigned NextInOrder = 0;

  /// The two components of the convergence test
  bool resourcesConverged(BlockState &BS) const;
  bool latencyConverged(BlockState &BS) const;

  /// After finding the loops, determine the epilogue blocks
  void markEpilogueBlocks();

  /// define the scheduling order, loops first then the reset bottom-up over
  /// the control flow graph
  void defineSchedulingOrder(MachineFunction *MF);

  /// Perform the convergence checks and set convergence parameters
  /// for the next iteration.
  /// returns true if converged
  bool updateFixPoint(BlockState &BS);

  /// Calculate the number of cycles that are needed to respect
  /// latencies related to the loop whose the epilogue is associated
  int getCyclesToRespectTiming(const BlockState &EpilogueBS,
                               const BlockState &LoopBS) const;

  /// Calculate the number of cycles that are needed to avoid resource
  /// conflicts between loop and epilogue
  int getCyclesToAvoidResourceConflicts(int ExistingLatency,
                                        const BlockState &EpilogueBS,
                                        const BlockState &LoopBS) const;

  BlockState *CurrentBlock = nullptr;

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

  /// Check whether all successors of BB have been scheduled.
  bool successorsAreScheduled(MachineBasicBlock *BB) const;

  /// Retrieve the inter-block state for BB
  const BlockState &getBlockState(MachineBasicBlock *BB) const;
  BlockState &getBlockState(MachineBasicBlock *BB);

  /// Return the maximum interblock latency we need to account for
  /// for the given successor. This represents the latency margin we assume for
  /// an unscheduled successor.
  std::optional<int> getLatencyCap(MachineBasicBlock *BB) const;

  /// Return the maximum number of cycles to block for the given successor.
  /// This represents the resource usage we assume for an unscheduled successor.
  std::optional<int> getBlockedResourceCap(MachineBasicBlock *BB) const;

  /// Return the number of nops that must be inserted before this block.
  /// This is typical for epilogue blocks, which supply the safety margin of
  /// their loops.
  int getNumEntryNops(const BlockState &BS) const;
};

} // end namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIEINTERBLOCKSCHEDULING_H
