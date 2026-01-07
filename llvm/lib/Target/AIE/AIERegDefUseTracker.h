//===- AIERegDefUseTracker.h - Track Register Live Ranges ----------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains declarations for tracking and analyzing register live
// ranges in a MachineBasicBlock. The tracker performs the following:
// - Identifies register definitions and uses that form live ranges
// - Merges aliasing register accesses into unified live ranges
// - Filters out unsafe ranges (tied operands, live-in/out, implicit uses)
// - Computes appropriate register classes for each live range
// - Optionally replaces physical registers with virtual registers for testing
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEREGDEFUSETRACKER_H
#define LLVM_LIB_TARGET_AIE_AIEREGDEFUSETRACKER_H

#include "AIEBaseInstrInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/MC/MCRegister.h"

namespace llvm {

struct AIEBaseInstrInfo;
struct LaneBitmask;
class MachineBasicBlock;
class MachineFunction;
class MachineInstr;
class MachineOperand;
class MachineRegisterInfo;
class TargetRegisterInfo;
class TargetRegisterClass;

/// Represents a register operand with its sub-register index
class RegOperandInfo {
  MachineOperand *Operand;
  unsigned SubRegIdx;

public:
  RegOperandInfo(MachineOperand *Op, unsigned SubIdx = 0)
      : Operand(Op), SubRegIdx(SubIdx) {}

  MachineOperand *getOperand() const { return Operand; }
  unsigned getSubRegIdx() const { return SubRegIdx; }
};

/// Structure representing a live range for a register
/// A live range can have multiple definitions (e.g., when different
/// sub-registers are defined separately) and multiple uses
class RegLiveRange {
public:
  // Sentinel value for live-out registers not yet associated with a live range
  static constexpr int NoLiveRange = -1;

private:
  // All definitions that contribute to this live range
  SmallVector<RegOperandInfo, 4> Defs;

  // All uses of this live range
  SmallVector<RegOperandInfo, 4> Uses;

  // Base register for this live range (largest register that covers all
  // operands)
  MCRegister BaseReg = MCRegister::NoRegister;

  // Register class that satisfies all constraints for this live range.
  const TargetRegisterClass *RegisterClass = nullptr;

  // Explicit set of admissible physical registers for this live range.
  // This represents the semantic constraint: which registers can be used
  // based on instruction encoding. Initially populated from RegisterClass,
  // but can be further constrained by per-LR requirements (e.g., bypass).
  // Note: this is separate from availability - PostRegAlloc intersects this
  // with the global available registers set to get candidates.
  DenseSet<MCRegister> AdmissibleRegs;

  // Virtual register assigned to this live range (if virtualized)
  Register VReg;

  // Whether this live range is scarce (has exactly 1 available register)
  bool IsScarce = false;

  // Whether this live range is reserved (virtualizable but register reserved).
  // This is used for disjoint live ranges that share a physical register with
  // subsequent full defs. The range can be virtualized to allow pipelining,
  // but its physical register must remain reserved for the subsequent def.
  bool IsReserved = false;

  // Unique ID for this live range (for debugging/tracking)
  // Use -1 as sentinel for invalid/cleared ranges
  int ID = -1;

public:
  RegLiveRange() = default;

  /// Construct a live range with the given ID, base register, and reserved
  /// status. This is the primary constructor used when creating new ranges.
  RegLiveRange(int ID, MCRegister BaseReg, bool IsReserved = false)
      : BaseReg(BaseReg), IsReserved(IsReserved), ID(ID) {}

  void addDef(MachineOperand *DefOp, unsigned SubRegIdx);
  void addUse(MachineOperand *UseOp, unsigned SubRegIdx);

  /// Get the number of definitions
  size_t getNumDefs() const { return Defs.size(); }

  /// Get the number of uses
  size_t getNumUses() const { return Uses.size(); }

  /// Iterator access to definitions
  auto defs() const { return llvm::make_range(Defs.begin(), Defs.end()); }

  /// Iterator access to uses
  auto uses() const { return llvm::make_range(Uses.begin(), Uses.end()); }

  /// Iterator across all defs and uses.
  auto operands() const {
    return llvm::concat<const RegOperandInfo>(Uses, Defs);
  }

  /// Get the base register for this live range.
  MCRegister getBaseReg() const { return BaseReg; }

  /// Get the register class for this live range.
  const TargetRegisterClass *getRegisterClass() const { return RegisterClass; }

  /// Get the admissible physical registers for this live range.
  const DenseSet<MCRegister> &getAdmissibleRegs() const {
    return AdmissibleRegs;
  }

  /// Check if a register is admissible for this live range.
  bool isAdmissible(MCRegister Reg) const {
    return AdmissibleRegs.contains(Reg);
  }

  /// Get the number of admissible registers.
  size_t getNumAdmissibleRegs() const { return AdmissibleRegs.size(); }

  /// Get the virtual register assigned to this live range
  Register getVReg() const { return VReg; }

  /// Set the virtual register for this live range
  void setVReg(Register R) { VReg = R; }

  /// Check if this live range is scarce (has exactly 1 available register)
  bool isScarce() const { return IsScarce; }

  /// Set whether this live range is scarce
  void setIsScarce(bool Scarce) { IsScarce = Scarce; }

  /// Check if this live range is reserved (virtualizable but register reserved)
  bool isReserved() const { return IsReserved; }

  /// Set whether this live range is reserved
  void setIsReserved(bool Reserved) { IsReserved = Reserved; }

  /// Get the unique ID for this live range.
  int getID() const { return ID; }

  /// Set the register class and populate AdmissibleRegs.
  /// AdmissibleRegs is initially populated from the register class membership.
  void setRegisterClass(const TargetRegisterClass *RC);

  /// Merge another live range into this one.
  /// Copies all defs and uses from Other into this range.
  /// Updates BaseReg to the smallest register that contains all operands from
  /// both ranges. This handles sibling registers (e.g., cml4 and cmh4) by
  /// finding their common super-register (dm4).
  /// Other is NOT cleared after the merge (caller must do that if needed).
  /// @param Other The live range to merge from.
  /// @param TRI Target register info for computing sub-register indices.
  /// Returns false if no common super-register can be found to contain all
  /// operand registers from both ranges, in which case neither range is
  /// modified.
  bool mergeFrom(const RegLiveRange &Other, const TargetRegisterInfo *TRI);

  /// Expand the base register to include an external register.
  /// This is used for registers that affect the live range's base (e.g.,
  /// live-out sentinels) but don't have corresponding operands.
  /// If ExtReg is larger than BaseReg, or if they are siblings requiring
  /// a common super-register, BaseReg is updated accordingly.
  /// Existing operands have their SubRegIdx values recomputed.
  /// @param ExtReg The external register to include.
  /// @param TRI Target register info for computing sub-register indices.
  /// Returns false if ExtReg neither contains nor is contained by BaseReg and
  /// no common super-register can be found, in which case the range is not
  /// modified.
  bool expandBaseToInclude(MCRegister ExtReg, const TargetRegisterInfo *TRI);

  /// Clear all state, making this an invalid/empty range.
  void clear();

  /// Check if this live range is valid.
  bool isValid() const { return ID >= 0; }

  /// Dump a brief summary of this live range for debugging.
  void dumpBrief(const TargetRegisterInfo *TRI) const;
};

/// Tracker for register live ranges in a MachineBasicBlock
class RegLiveRangeTracker {
  MachineFunction *MF;
  const TargetRegisterInfo *TRI;
  const AIEBaseInstrInfo *TII;

  // List of all live ranges found in the block
  SmallVector<RegLiveRange, 16> LiveRanges;

  // All physical register operands in the block
  SmallVector<MachineOperand *, 32> AllPhysRegOperands;

  // Instruction order mapping for determining earliest operand
  DenseMap<const MachineInstr *, unsigned> InstrOrder;

  // Track whether registers have been virtualized
  mutable bool RegistersVirtualized = false;

  // Cached available physical registers (computed during analyze)
  DenseSet<MCRegister> AvailablePhysRegs;

  // Cached most promising scarce range set (computed during analyze)
  std::vector<const RegLiveRange *> MostPromisingScarceRanges;

  // Counter for assigning unique IDs to live ranges
  int NextLiveRangeID = 0;

  /// Get the sub-register index if AccessReg is a sub-register of BaseReg
  /// Returns 0 if AccessReg is not a sub-register of BaseReg
  unsigned getSubRegIndex(MCRegister AccessReg, MCRegister BaseReg) const;

  /// Check if a register overlaps with any register in a set
  bool overlapsAnyInSet(MCRegister Reg,
                        const DenseSet<MCRegister> &RegSet) const;

  /// Check if all instructions in a live range have fixed itineraries.
  /// An instruction has a fixed itinerary if it has at most one schedule class
  /// variant, meaning the schedule class doesn't depend on operand register
  /// classes.
  bool hasAllInstructionsWithFixedItinerary(const RegLiveRange &LR) const;

  /// Compute the register class for a live range based on all its operands
  void computeRegisterClass(RegLiveRange &LR) const;

  /// First-stage safety filtering.
  bool hasTiedOperands(const RegLiveRange &LR) const;

  /// Check if a live range's base register is fully defined in the block.
  /// Uses lane mask intersection with the block's live-in set to determine
  /// if the register is truly defined within the block or comes from outside.
  /// This can discriminate between a truly undefined register (not in live-in,
  /// safe to virtualize) and a register defined outside the loop (in live-in,
  /// should be rejected to preserve loop-carried values).
  bool
  isFullyDefined(const RegLiveRange &LR,
                 const DenseMap<MCRegister, LaneBitmask> &LocalLiveLaneMasks,
                 const MachineBasicBlock &MBB) const;

  /// Second-stage full coverage pruning
  void pruneByFullCoverage();

  /// Merge aliasing live ranges when a definition is encountered.
  void mergeAliasingLiveRanges(
      unsigned DefLRIdx, MCRegister DefReg,
      DenseMap<MCRegister, std::pair<int, LaneBitmask>> &LiveRegs,
      DenseMap<MachineOperand *, unsigned> &OperandToLiveRange);

  /// Helper to find the most promising scarce range set.
  /// Called by analyze() to populate MostPromisingScarceRanges.
  std::vector<const RegLiveRange *> findMostPromisingScarceRanges(
      const DenseSet<MCRegister> &AvailablePhysRegs) const;

  /// Collect base registers from RESERVED live ranges.
  DenseSet<MCRegister> collectReservedBaseRegs() const;

  /// Populate AvailablePhysRegs from non-reserved live ranges.
  /// Adds base registers and sub-registers that don't overlap with reserved.
  void computeAvailableFromLiveRanges(const DenseSet<MCRegister> &ReservedRegs);

  /// Extend AvailablePhysRegs with super-registers whose sub-regs are all
  /// available.
  void deriveSuperRegsFromSubRegs();

  /// Add caller-saved registers that are completely unused in the block.
  /// Uses AllPhysRegOperands member for used registers, and iterates
  /// MBB.liveins() and MBB.liveouts() directly (with lane mask support).
  /// @param MBB The machine basic block (for live-in/out iteration).
  /// @param ImplicitRegs Registers used implicitly.
  /// @param ReservedRegs Reserved base registers.
  void addUnusedCallerSavedRegs(MachineBasicBlock &MBB,
                                const DenseSet<MCRegister> &ImplicitRegs,
                                const DenseSet<MCRegister> &ReservedRegs);

  /// Mark live ranges as scarce if they have exactly 1 available register.
  void markScarceRanges();

  //===--------------------------------------------------------------------===//
  // Analyze helper methods (decomposition of analyze())
  //===--------------------------------------------------------------------===//

  /// State passed through the liveness scan.
  /// Groups the mutable state that is threaded through the backward scan.
  struct LivenessScanState {
    /// Map from register to its current live range index (signed) and lane
    /// mask. Use NoLiveRange as sentinel for live-out registers not yet
    /// associated with a range.
    DenseMap<MCRegister, std::pair<int, LaneBitmask>> LiveRegs;

    /// Map from operand to live range index.
    DenseMap<MachineOperand *, unsigned> OperandToLiveRange;

    /// Set of registers used implicitly (invalidates explicit ranges).
    DenseSet<MCRegister> ImplicitRegs;
  };

  /// Build instruction order map and collect physical register operands.
  /// Also populates ImplicitRegs.
  void buildInstructionOrderAndCollectOperands(
      ArrayRef<MachineInstr *> SemanticOrder, LivenessScanState &State);

  /// Initialize LiveRegs from live-out registers.
  void initLiveRegsFromLiveOuts(const MachineBasicBlock &MBB,
                                LivenessScanState &State);

  /// Get or create a live range for a register operand.
  /// Returns the live range index.
  unsigned getOrCreateLiveRangeForOperand(MCRegister Reg, MachineOperand *MO,
                                          LivenessScanState &State);

  /// Process def operands for a single instruction (reverse pass).
  void processDefsInInstruction(MachineInstr &MI, LivenessScanState &State);

  /// Process use operands for a single instruction (reverse pass).
  void processUsesInInstruction(MachineInstr &MI, LivenessScanState &State);

  /// Perform the liveness scan over all instructions.
  void performLivenessScan(ArrayRef<MachineInstr *> SemanticOrder,
                           LivenessScanState &State);

  /// Apply first-stage safety filtering to live ranges.
  /// Returns the lane masks collected during analysis for isFullyDefined.
  void applySafetyFiltering(
      const MachineBasicBlock &MBB, const LivenessScanState &State,
      const DenseMap<MCRegister, LaneBitmask> &LocalLiveLaneMasks);

  /// Compute register classes and apply register class filtering.
  void computeRegisterClassesAndFilter();

  //===--------------------------------------------------------------------===//
  // Schedule class-preserving register class refinement
  //===--------------------------------------------------------------------===//

  /// Information about an instruction with variable itineraries.
  /// Used during register class refinement to preserve schedule class
  /// determinism.
  struct VarItinInstrInfo {
    /// The original schedule class computed using physical registers.
    unsigned OrigSchedClass;

    /// The RC requirements for each operand from the original physreg match.
    ArrayRef<OperandRCRequirement> OrigOperandRCs;

    /// Map from operand index to live range index.
    DenseMap<unsigned, unsigned> OperandToLRIdx;
  };

  /// Map from instruction to its variable itinerary info.
  using VarItinInstrMap = DenseMap<MachineInstr *, VarItinInstrInfo>;

  /// Stage 2 of register class refinement: collect instructions with variable
  /// itineraries and record their original schedule classes.
  void collectVariableItinInstructions(VarItinInstrMap &VarItinInstrs) const;

  /// Stage 3 of register class refinement: check if schedule classes changed
  /// and narrow LR register classes to restore the original schedule class.
  /// When narrowing is impossible (no common subclass), the LR's register
  /// class is set to null, marking it for removal from virtualization
  /// candidates. Null-RC LRs are treated as their original physical registers
  /// in subsequent iterations.
  /// Returns true if any register class was narrowed or nulled.
  bool checkAndRefineSchedClasses(const VarItinInstrMap &VarItinInstrs);

  /// Orchestrate the three-stage register class refinement algorithm.
  /// Called at the end of computeRegisterClassesAndFilter().
  void refineRegisterClassesForSchedClass();

  /// Finalize available registers and scarcity after all filtering.
  void finalizeAvailabilityAndScarcity(MachineBasicBlock &MBB,
                                       const LivenessScanState &State);

  /// Clear all state and bring the tracker back to its default constructed
  /// state.
  void clear();

public:
  RegLiveRangeTracker(MachineBasicBlock &MBB);

  /// Process a MachineBasicBlock to find all register live ranges
  /// @param MBB The machine basic block to analyze
  /// @param SemanticOrder The semantic instruction order (required - must be
  ///                      non-empty)
  void analyze(MachineBasicBlock &MBB, ArrayRef<MachineInstr *> SemanticOrder);

  /// Get all live ranges
  ArrayRef<RegLiveRange> getLiveRanges() const { return LiveRanges; }

  /// Dump the live range information for debugging.
  /// @param Header Optional header string to print before the dump.
  /// @param ShowAvailableRegs If false, suppress the available register set
  ///        section. Pass false when dumping before
  ///        finalizeAvailabilityAndScarcity() has run.
  void dump(const char *Header = nullptr, bool ShowAvailableRegs = true) const;

  /// Overlap policy for virtualization with respect to RESERVED ranges.
  enum class OverlapPolicy {
    /// Do not virtualize any range that overlaps a RESERVED base register.
    /// This is the safe default that prevents regressions.
    DisallowOverlapWithReservedBase,
    /// Allow virtualizing ranges that overlap RESERVED bases.
    /// This enables the RESERVED semantics for disjoint ranges sharing a base.
    AllowOverlapWithReservedBase
  };

  /// Replace filtered physical registers with virtual registers.
  /// This modifies the MachineBasicBlock and updates LiveRanges with VReg info.
  /// RESERVED ranges themselves are never virtualized.
  /// Other ranges may be filtered based on the policy.
  /// This is a non-destructive operation that supports partial virtualization.
  void virtualizeFilteredPhysRegs(
      OverlapPolicy Policy = OverlapPolicy::DisallowOverlapWithReservedBase);

  /// Get the set of physical registers that would be available for reallocation
  /// Returns the cached value computed during analyze()
  const DenseSet<MCRegister> &getAvailablePhysRegs() const {
    return AvailablePhysRegs;
  }

  /// Rewrite virtual registers to physical registers using the provided
  /// mapping.
  /// @param VRegToPhysMap Mapping from virtual registers to physical registers
  void rewriteToPhysRegs(const DenseMap<Register, MCRegister> &VRegToPhysMap);

  /// Restore original physical registers from virtual registers
  /// Uses the LiveRanges to map VRegs back to their original PhysRegs
  /// This is a convenience method that builds the mapping and calls
  /// rewriteToPhysRegs
  void restoreOriginalPhysRegs();

  /// Check if registers are currently virtualized
  bool areRegistersVirtualized() const;

  /// Filter live ranges based on available physical registers.
  /// Removes live ranges that have only one available physical register
  /// for their register class, as these should stay physical to avoid
  /// pipeliner invalidation.
  /// Uses the cached AvailablePhysRegs computed during analyze().
  void filterByRegisterAvailability();

  /// Get the most promising scarce range set for packing.
  /// Returns the cached value computed during analyze().
  /// An empty vector signals that no such set could be found.
  const std::vector<const RegLiveRange *> &
  getMostPromisingScarceRanges() const {
    return MostPromisingScarceRanges;
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEREGDEFUSETRACKER_H
