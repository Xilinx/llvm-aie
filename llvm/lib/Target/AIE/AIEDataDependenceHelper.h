//===- AIEDataDependenceHelper.h - Inter-block DAG --------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Class providing an inter-block dependence graph
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEDATADEPENDENCEHELPER_H
#define LLVM_LIB_TARGET_AIE_AIEDATADEPENDENCEHELPER_H

#include "AIEBaseAliasAnalysis.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleDAGInstrs.h"
#include "llvm/MC/LaneBitmask.h"
#include "llvm/MC/MCRegisterInfo.h"
#include <algorithm>
#include <map>
#include <optional>

namespace llvm {

class raw_ostream;

namespace AIE {

/// Class to derive actual helpers from. It is a placeholder for the future
/// stand-alone DDG class, it just implements the unrelated schedule() as
/// a dummy
/// It copies the Mutations mechanism from ScheduleDAGMI; this represents a
/// change of perspective on DAGMutations: They are target-dependent ways to
/// modify the dependence graph, not target-dependent ways to tweak the
/// scheduler.
/// Per-instruction snapshot of register operand events for use in pairwise
/// edge building. Produced by DataDependenceHelper::collectEvents() using
/// AIE itinerary data. The OperandCycle and ForwardingClass fields enable
/// direct latency computation without repeated schedule-class lookups.
class InstrEvents {
public:
  /// One physical register def or use event. The reg unit is the matching
  /// key; Reg and OpIdx are needed to construct the SDep. OperandCycle is
  /// the itinerary-derived cycle at which the operand is written or read,
  /// relative to instruction issue.
  /// ForwardingClass is the pipeline bypass class; 0 means no bypass.
  class PhysRegEvent {
    MCRegUnit Unit;
    Register Reg;
    unsigned OpIdx;
    int OperandCycle;
    unsigned ForwardingClass;

  public:
    PhysRegEvent(MCRegUnit Unit, Register Reg, unsigned OpIdx,
                 int OperandCycle = 0, unsigned ForwardingClass = 0)
        : Unit(Unit), Reg(Reg), OpIdx(OpIdx), OperandCycle(OperandCycle),
          ForwardingClass(ForwardingClass) {}

    MCRegUnit getUnit() const { return Unit; }
    Register getReg() const { return Reg; }
    unsigned getOpIdx() const { return OpIdx; }
    int getOperandCycle() const { return OperandCycle; }
    unsigned getForwardingClass() const { return ForwardingClass; }
  };

  /// One virtual register def or use event with its lane mask and operand
  /// index. OperandCycle and ForwardingClass have the same meaning as in
  /// PhysRegEvent.
  class VRegEntry {
    Register Reg;
    LaneBitmask LaneMask;
    unsigned OpIdx;
    int OperandCycle;
    unsigned ForwardingClass;

  public:
    VRegEntry(Register Reg, LaneBitmask LaneMask, unsigned OpIdx,
              int OperandCycle = 0, unsigned ForwardingClass = 0)
        : Reg(Reg), LaneMask(LaneMask), OpIdx(OpIdx),
          OperandCycle(OperandCycle), ForwardingClass(ForwardingClass) {}

    Register getReg() const { return Reg; }
    LaneBitmask getLaneMask() const { return LaneMask; }
    unsigned getOpIdx() const { return OpIdx; }
    int getOperandCycle() const { return OperandCycle; }
    unsigned getForwardingClass() const { return ForwardingClass; }
  };

  SUnit *SU = nullptr;
  // Physical register defs, one entry per reg unit covered by the operand.
  SmallVector<PhysRegEvent, 4> PhysDefs;
  // Physical register uses, one entry per reg unit covered by the operand.
  SmallVector<PhysRegEvent, 4> PhysUses;
  // Virtual register defs.
  SmallVector<VRegEntry, 4> VRegDefs;
  // Virtual register uses.
  SmallVector<VRegEntry, 4> VRegUses;
};

class DataDependenceHelper : public ScheduleDAGInstrs {
  /// Ordered list of DAG postprocessing steps.
  std::vector<std::unique_ptr<ScheduleDAGMutation>> Mutations;
  const MachineSchedContext &Context;
  // Set at the start of buildEdges() and consulted by the reg-dep helpers.
  bool TrackLaneMasks = false;

  // When non-zero, the DAG contains two virtual copies of the loop body with
  // PipeliningNInstr instructions each. mayAlias() uses this to detect
  // loop-carried dependences via aliasAcrossVirtualUnrolls().
  unsigned PipeliningNInstr = 0;
  // When true, all memory accesses in the current region are proved
  // non-aliasing and chain dependencies can be suppressed entirely.
  bool SafeToIgnoreMemDeps = false;
  // When true, singly defined virtual registers get no output or anti
  // dependences.
  bool AbandonSingleDefDeps = false;
  // When true, a physical register def in a fixed node does not erase the
  // earlier defs and uses it would otherwise stand in for by transitivity.
  // This produces redundant edges, so it is off by default.
  bool RetainFixedPhysRegDeps = false;

  void schedule() override {};

  // Copies of ScheduleDAGInstrs register-dependency builders, now instance
  // methods so their call sites are visible in this class for future
  // latency centralization.
  void addPhysRegDataDeps(SUnit *SU, unsigned OperIdx);
  void addPhysRegDeps(SUnit *SU, unsigned OperIdx);
  void addVRegDefDeps(SUnit *SU, unsigned OperIdx);
  void addVRegUseDeps(SUnit *SU, unsigned OperIdx);
  bool handleRegEvents(SUnit *SU);

  // Set the latency of Dep before AIE-specific adjustment.
  void setBaseLatency(SUnit *Src, int SrcOpIdx, SUnit *Dst, int DstOpIdx,
                      SDep &Dep) const;
  // Compute the latency of a data dependence from AIE itinerary data.
  // UseMI may be null for an unknown user.
  int computeDataLatency(const MachineInstr &DefMI, unsigned DefOpIdx,
                         const MachineInstr *UseMI, unsigned UseOpIdx) const;

  /// Collect per-operand events for SU into an InstrEvents record using
  /// AIE itinerary data. The itinerary class is resolved once per
  /// instruction using the AIE register-dependent schedule class.
  /// This is left unused for now; future work will thread InstrEvents
  /// through the edge builders to centralise latency computation.
  InstrEvents collectEvents(SUnit *SU);

protected:
  bool mayAlias(SUnit *SUa, SUnit *SUb, bool TBAA) override;

  /// True when the cycle of \p SU is set externally, e.g. by the pipeliner.
  virtual bool isFixedNode(const SUnit &SU) const { return false; }

public:
  // Compute the latency of Dep according to the configuration of this helper
  // and add it to Dst. This merges the computation of the base latency and
  // the AIE-specific adjustment, without going through TargetSchedModel or
  // the virtual TargetSubtargetInfo::adjustSchedDependency dispatch.
  // Returns true if the dependence was added. Callers that track event state
  // can use a false return to learn that the edge was found unnecessary.
  bool addDep(SUnit *Src, int SrcOpIdx, SUnit *Dst, int DstOpIdx, SDep Dep);

  // Set the number of instructions per virtual-unroll copy. Must be called
  // before buildEdges() when the DAG is built in pipelining mode (two copies
  // of the loop body). Zero (default) disables the virtual-unroll alias check.
  void setPipeliningNInstr(unsigned N) { PipeliningNInstr = N; }
  void setSafeToIgnoreMemDeps(bool V) { SafeToIgnoreMemDeps = V; }
  void setAbandonSingleDefDeps(bool V) { AbandonSingleDefDeps = V; }
  void setRetainFixedPhysRegDeps(bool V) { RetainFixedPhysRegDeps = V; }

  // Compute the instruction latency of MI from the AIE itineraries, without
  // going through TargetSchedModel.
  int computeLatency(const MachineInstr *MI) const;

  // Build all dependency edges for Target. Uses *this for AIE context (AA from
  // Context, scheduling model, and alias analysis flags).
  // The no-arg version calls buildEdges(*this).
  void buildEdges(ScheduleDAGInstrs &Target, bool TrackLaneMasks = false);
  void buildEdges();

  DataDependenceHelper(const MachineSchedContext &Context, bool AddMutators,
                       bool ExactLatencies);

  // Compute the maximum depth of all nodes. The depth is the earliest cycle
  // in which an instruction can run when considering all latencies leading
  // up to it.
  int maxDepth() const;

  // Dump a graphviz representation of the graph to OS.
  // IncludeBoundaries controls whether edges to the artificial boundary node
  // are printed.
  void dumpDot(raw_ostream &OS, bool IncludeBoundaries) const;
};

/// This class generates all edges between nodes in two flow-adjacent regions.
/// The nodes are added in forward flow order, marking the boundary at the
/// appropriate point. Since the same MachineInstruction may be present in the
/// predecessor and the successor in case of a self-edge, we keep separate maps
/// of pre- and post- boundary nodes. They disambiguate the pre- and
/// post-boundary SUnits, identified by their NodeNum.
///
/// When SafeToIgnoreMemDeps is set, memory-alias edges that cross the
/// pre/post boundary are suppressed via a mayAlias() override.
///
/// The class also stores signed cycle distances from the CFG boundary
/// (keyed by SUnit NodeNum):
///
///   PreDepths — distance of each pre-boundary node, negated: the last
///               predecessor cycle is -1.
///   PostDepths — distance of each post-boundary node: successor cycle 0 is
///               the first cycle after the boundary.
///
/// Both sides share one cycle axis through the boundary. PreDepth grows
/// upward through the predecessor and is stored negative, so the boundary
/// stays at 0. PostDepth grows downward through the successor. depth + latency
/// and latency - depth then work on either side.
///
///   ^
///   | PreDepth   -3  -2  -1
/// ----- Boundary
///   | PostDepth   0   1   2
///   v
///
/// Each side is further cut into TopFixed, Free and BotFixed by its FreeRange:
/// the NodeNums of its first free node (Begin) and of its first BotFixed node
/// (End). Any range may be empty. An unset marker leaves the free range open
/// at that end, so a side without markers is all free.
///
///   | TopFixed   epilogue clones of a one-region predecessor, no depth
/// ----- PreFree.Begin
///   | Free       predecessor bottom-region instructions
/// ----- PreFree.End
///   | BotFixed   prologue clones or the loop tail, PreDepth -3 -2 -1
/// ----- Boundary
///   | TopFixed   epilogue clones, PostDepth 0 1 2
/// ----- PostFree.Begin
///   | Free       successor top-region instructions
/// ----- PostFree.End
///   | BotFixed   prologue clones of a one-region successor, no depth
///
/// Each side is a BoundaryDistances object: a vector of those signed
/// distances, indexed by NodeNum relative to the start of that side (0 for
/// pre-boundary, Boundary for post-boundary), plus a region maximum. Call
/// allocateDepths() once all nodes are added so the vectors are sized to each
/// side. An absent entry means that node has no stored distance. Recording an
/// instruction that is not in the matching pre/post index is ignored for the
/// vector but still updates the region maximum. The no-instruction overload
/// exists for empty bundles (NOPs). Negative distances are allowed.
///
/// In practice, SUnits and their dependences are invariant after first
/// construction. PostDepth is variant, and lazily re-evaluated by algorithms
/// working on the predecessor block. The distances may change each time the
/// successor block is scheduled.
class InterBlockEdges : public DataDependenceHelper {
  /// MachineInstr to SUnit NodeNum. The same instruction may appear on both
  /// sides of the boundary, so predecessor and successor keep separate maps.
  using IndexMap = std::map<MachineInstr *, unsigned>;

public:
  /// Per-node signed cycle distances from the CFG boundary for one side,
  /// indexed by NodeNum - Base, plus the maximum distance recorded for that
  /// region. \p Nodes maps a MachineInstr to its SUnit NodeNum.
  class BoundaryDistances {
    const IndexMap &Nodes;
    /// First SUnit NodeNum of this side (0 pre-boundary, Boundary post).
    unsigned Base = 0;
    SmallVector<std::optional<int>, 8> Distances;
    int RegionMax = 0;

  public:
    explicit BoundaryDistances(const IndexMap &Nodes) : Nodes(Nodes) {}

    /// Size \p Distances for NodeNums in [BaseNodeNum, BaseNodeNum + NumNodes).
    void allocate(unsigned BaseNodeNum, unsigned NumNodes);

    /// Drop recorded distances but keep the allocation from allocate().
    void clear();

    /// Drop allocation as well as recorded distances.
    void reset();

    /// Record distance \p Dist for \p MI. An instruction missing from Nodes is
    /// ignored for the vector, but the region maximum is still updated: the
    /// caller has found code at this cycle (a bundle interleaved with Fixed
    /// instructions, or a BottomFixed instruction outside the first
    /// iteration). Requires a prior allocate() that covers \p MI's NodeNum.
    void record(MachineInstr *MI, int Dist);

    /// Record a cycle that is not represented by a MachineInstr, for instance
    /// an empty bundle.
    void record(int Dist) { RegionMax = std::max(RegionMax, Dist); }

    /// Recorded distance of \p SU, or \p Default if none has been recorded (for
    /// example, the instruction is beyond the conflict horizon).
    int getDistanceOr(const SUnit *SU, int Default) const {
      return getDistance(SU).value_or(Default);
    }

    /// Recorded distance of \p SU, or nullopt if none has been recorded or
    /// \p SU is outside this side (e.g. ExitSU).
    std::optional<int> getDistance(const SUnit *SU) const;

    int getRegionMax() const { return RegionMax; }
  };

private:
  // The predecessor block feeding instructions before the boundary.
  MachineBasicBlock *Pred = nullptr;
  // The successor block feeding instructions after the boundary.
  MachineBasicBlock *Succ = nullptr;
  // The boundary between Pred and Succ nodes. Boundary holds the index of
  // the first post-boundary node. This is equal to its NodeNum.
  std::optional<unsigned> Boundary;
  /// Free range of one side, [Begin, End). Nodes of that side outside it are
  /// fixed. An unset end leaves the range open there.
  struct FreeRange {
    std::optional<unsigned> Begin;
    std::optional<unsigned> End;

    bool isFixed(unsigned NodeNum) const {
      return (Begin && NodeNum < *Begin) || (End && NodeNum >= *End);
    }
    void clear() {
      Begin.reset();
      End.reset();
    }
  };

  FreeRange PreFree;
  FreeRange PostFree;

  FreeRange &currentFreeRange() { return Boundary ? PostFree : PreFree; }
  const FreeRange &freeRangeOf(const SUnit &SU) const {
    return isPredecessorNode(&SU) ? PreFree : PostFree;
  }
  // When true, memory edges crossing the boundary are suppressed.
  bool SafeToIgnoreMemDeps = false;

  IndexMap PredMap;
  IndexMap SuccMap;

  /// Signed distances of pre-boundary SUnits (last cycle is -1).
  BoundaryDistances PreDepths{PredMap};
  /// Signed distances of post-boundary SUnits (first cycle is 0).
  BoundaryDistances PostDepths{SuccMap};

  bool mayAlias(SUnit *SUa, SUnit *SUb, bool TBAA) override;

public:
  /// Pruning by transitivity assumes that a def issues and lands no earlier
  /// than the earlier-listed defs it stands in for. Fixed nodes have their
  /// cycles set by the pipeliner, which need not follow that order, so the
  /// physical register edges of fixed nodes are retained.
  InterBlockEdges(const MachineSchedContext &Context,
                  bool SafeToIgnoreMemDeps = false,
                  MachineBasicBlock *Pred = nullptr,
                  MachineBasicBlock *Succ = nullptr)
      : DataDependenceHelper(Context, true, true), Pred(Pred), Succ(Succ),
        SafeToIgnoreMemDeps(SafeToIgnoreMemDeps) {
    setRetainFixedPhysRegDeps(true);
  }

  MachineBasicBlock *getPred() const { return Pred; }
  MachineBasicBlock *getSucc() const { return Succ; }

  /// Add a MI as a Node to the DAG.
  void addNode(MachineInstr *MI);

  /// Mark the boundary between the predecessor block and the successor block.
  /// Nodes added before are part of the predecessor, nodes added after are
  /// part of the successor.
  void markBoundary();

  /// Pre-size PreDepths and PostDepths to the pre- and post-boundary NodeNum
  /// ranges. Call after every node has been added and before recording depths.
  void allocateDepths();

  /// Start and end the free range of the current side: the predecessor
  /// before markBoundary(), the successor after it. Nodes added before
  /// markFreeBegin() are TopFixed, nodes added after markFreeEnd() BotFixed.
  void markFreeBegin();
  void markFreeEnd();

  /// To iterate forward across the SUnits of the underlying DDG.
  auto begin() { return SUnits.begin(); }
  auto begin() const { return SUnits.begin(); }
  auto end() { return SUnits.end(); }
  auto end() const { return SUnits.end(); }

  /// The following two methods are used to find the cross-boundary edges,
  /// by starting from a pre-boundary node and selecting its successor edges
  /// that connect to a post-boundary node.
  /// ---
  /// Retrieve the SUnit that represents MI's instance before the
  /// boundary, null if not found.
  const SUnit *getPreBoundaryNode(MachineInstr *MI) const;

  /// Retrieve the SUnit that represents MI's instance after the
  /// boundary, null if not found.
  const SUnit *getPostBoundaryNode(MachineInstr *MI) const;

  /// Check whether SU represents an instruction before the boundary.
  bool isPredecessorNode(const SUnit *SU) const;

  /// True when SU is a BotFixed pre-boundary node: its NodeNum is at or after
  /// PreFree.End and still before the CFG boundary. These carry PreDepth.
  bool isFixedPreBoundaryNode(const SUnit *SU) const;

  /// True when SU lies outside the free range of its side.
  bool isFixedNode(const SUnit &SU) const override;

  /// Check whether SU represents an instruction after the boundary.
  bool isPostBoundaryNode(const SUnit *SU) const;

  /// Pre-boundary height interface.
  /// Return the longest dependence chain that leads from each pre-boundary
  /// node to the end of the predecessor block, keyed by NodeNum. This is the
  /// mirror of PostDepths: a producer with height H has H cycles of its
  /// latency already covered by the predecessor before control reaches the
  /// successor.
  ///
  /// Chains are cut at the boundary, so a node whose only consumers are in
  /// the successor has no entry, which is equivalent to a height of zero.
  /// The values are static lower bounds derived from the DDG alone; they do
  /// not depend on the predecessor being scheduled.
  std::map<unsigned, int> computePreHeights() const;

  BoundaryDistances &getPreDepths() { return PreDepths; }
  const BoundaryDistances &getPreDepths() const { return PreDepths; }
  BoundaryDistances &getPostDepths() { return PostDepths; }
  const BoundaryDistances &getPostDepths() const { return PostDepths; }

  // Clear DAG and recorded depths.
  void clear();
};

} // namespace AIE
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEDATADEPENDENCEHELPER_H
