//===----- AIEWawRegRewriter.cpp - Rewrite Regs to remove Defs ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This pass rewrites loop assignments to break WAW dependencies; epilogue WAR
// repairs are performed by AIEEpilogueRegRewriter.
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEBaseInstrInfo.h"
#include "AIEBaseRegisterInfo.h"
#include "AIEDataDependenceHelper.h"
#include "AIELoopClass.h"
#include "AIESlotStatistics.h"
#include "Utils/AIELoopOptionOverrides.h"
#include "Utils/AIELoopUtils.h"
#include "Utils/AIERegAllocationUtils.h"

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/CodeGen/LiveDebugVariables.h"
#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/LiveRegMatrix.h"
#include "llvm/CodeGen/LiveStacks.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/CodeGen/VirtRegMap.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/CodeGen/MachineBasicBlock.h>
#include <map>
#include <queue>

using namespace llvm;

#define DEBUG_TYPE "aie-waw-reg-rewrite"

// This might be compatible with a future extension of the DEBUG rigging
#define DEBUG_DETAIL(x) DEBUG_WITH_TYPE("aie-waw-reg-rewrite:2", x)

enum class RewriteMode {
  Basic,
  Automatic,
  LatencyAware,
  SWPAware,
  SWPAwareAutoBias,
};

static cl::opt<bool> AggressiveReAlloc(
    "aie-aggressive-realloc", cl::Hidden, cl::init(false),
    cl::desc("Aggressively de-allocate live-through registers to favor "
             "loop-local registers"));

static cl::opt<bool> GPRRealloc("aie-gpr-realloc", cl::Hidden, cl::init(false),
                                cl::desc("Re-allocate GPRs as well"));

static cl::opt<bool> PreAlloc(
    "aie-realloc-loopaware", cl::Hidden, cl::init(false),
    cl::desc("Prime the LRU queue to make the allocations more loop-aware"));

static cl::opt<unsigned>
    MinRegisterLatency("aie-waw-reg-rewrite-min-lat", cl::Hidden,
                       cl::desc("Minimum operand latency that should be "
                                "considered for WAW rewriting"),
                       cl::init(3));

static cl::opt<RewriteMode> RegRewriteMode(
    "aie-reg-rewrite-mode", cl::Hidden, cl::init(RewriteMode::Automatic),
    cl::desc("Set the rewriting mode"),
    cl::values(clEnumValN(RewriteMode::Basic, "basic", "Basic"),
               clEnumValN(RewriteMode::Automatic, "auto",
                          "Automatic selection based on loop class"),
               clEnumValN(RewriteMode::LatencyAware, "latencyaware",
                          "Latency aware"),
               clEnumValN(RewriteMode::SWPAware, "swpaware",
                          "SWP aware with default bias"),
               clEnumValN(RewriteMode::SWPAwareAutoBias, "swpaware-auto",
                          "SWP aware with automatic bias")));

static cl::opt<int> MinIIBias("aie-realloc-ii-bias", cl::Hidden, cl::init(0),
                              cl::desc("Set default MinII bias for swpaware"));

namespace {

// Defines the next register to use in reallocation.
using RoundRobin = SmallVector<MCPhysReg, 32>;

// Record the candidates and their original allocation.
using OriginalAllocation = std::vector<std::pair<Register, MCPhysReg>>;

RewriteMode selectMode(RewriteMode Mode, int LoopClass) {
  if (Mode != RewriteMode::Automatic) {
    return Mode;
  }
  switch (LoopClass) {
  case 14:
  case 29:
  case 47:
    return RewriteMode::SWPAwareAutoBias;
  default:
    return RewriteMode::LatencyAware;
  }
}

std::optional<int> getMinIIBias(RewriteMode Mode, int LoopClass) {
  switch (Mode) {
  case RewriteMode::SWPAwareAutoBias:
    break;
  case RewriteMode::SWPAware:
    return MinIIBias;
  default:
    return {};
  }

  switch (LoopClass) {
  case 14:
    return -1;
  case 18:
  case 29:
    return 1;
  default:
    return MinIIBias;
  }
}

///
/// Rewrites physical register assignments in critical loop regions to break
/// WAW dependencies; epilogue WAR repairs belong to the separate epilogue pass.
class AIEWawRegRewriter : public MachineFunctionPass {

public:
  static char ID;
  AIEWawRegRewriter() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addRequired<VirtRegMapWrapperLegacy>();
    AU.addPreserved<VirtRegMapWrapperLegacy>();
    // no Machine Instructions are added, therefore the SlotIndexes remain
    // constant and preserved
    AU.addRequired<SlotIndexesWrapperPass>();
    AU.addPreserved<SlotIndexesWrapperPass>();
    // no new Virtual Registers are generated, therefore the LiveDebugVariables
    // do not have to be updated
    AU.addRequired<LiveDebugVariablesWrapperLegacy>();
    AU.addPreserved<LiveDebugVariablesWrapperLegacy>();
    AU.addRequired<LiveStacksWrapperLegacy>();
    AU.addPreserved<LiveStacksWrapperLegacy>();
    AU.addRequired<LiveIntervalsWrapperPass>();
    AU.addPreserved<LiveIntervalsWrapperPass>();
    AU.addRequired<LiveRegMatrixWrapperLegacy>();
    AU.addRequired<AAResultsWrapperPass>();
    AU.addPreserved<LiveRegMatrixWrapperLegacy>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  MachineFunction *MF = nullptr;
  const MachineRegisterInfo *MRI = nullptr;
  const AIEBaseRegisterInfo *TRI = nullptr;
  VirtRegMap *VRM = nullptr;
  LiveRegMatrix *LRM = nullptr;
  LiveIntervals *LIS = nullptr;
  const AIEBaseInstrInfo *TII = nullptr;
  /// Per-loop option overrides from !llvm.loop metadata.
  AIE::LoopOptionOverrides Overrides;

  /// Set Architecture specific Options
  void setArchSpecificPassOptions();

  bool renameMBBPhysRegs(const MachineBasicBlock *MBB);

  /// Compute the LRU list of physical registers available for reallocation,
  /// derived from the register classes of the candidates.
  RoundRobin computeLRURegisters(const OriginalAllocation &Candidates);

  /// Sort the candidates to mimic interleaving the pipeline stages
  void sortSWPAware(OriginalAllocation &Candidates, MachineBasicBlock &MBB,
                    const llvm::AIE::SlotStatistics &Statistics, int Bias,
                    const IndexedMap<const MachineInstr *, VirtReg2IndexFunctor>
                        &LastVRegDef);

  /// Pre-allocate all virtual registers in Candidates. The sole purpose of
  /// this is to prime the LRURegisters, so that the end of the loop is
  /// considered to be near to the start. No actual allocations are made.
  /// The ultimate allocation is expected to be different from this initial
  /// run -- that's the whole purpose.
  void preAllocate(OriginalAllocation &Candidates, RoundRobin &LRURegisters);

  /// Reallocate all virtual registers in Candidates.
  /// Return true if successful for all classes.
  bool reAllocate(OriginalAllocation &Candidates, RoundRobin &LRURegisters);

  /// If reallocation fails, revert all reallocations to their original
  /// assignments.
  void revertAllocation(ArrayRef<std::pair<Register, MCPhysReg>> Entries);

  /// Get all the defined physical registers that the MachineBasicBlock already
  /// uses. These physical registers should not be used for replacement
  /// candidates, since this would introduce new WAW dependencies, which this
  /// pass tries to remove.
  BitVector getDefinedPhysRegs(const MachineBasicBlock *MBB) const;

  /// Returns true if the physical register \p Reg was replaced
  bool replaceReg(const Register Reg, RoundRobin &Registers,
                  BitVector &UsedUnits);

  void unassignReg(Register Reg);
  void assignReg(Register Reg, MCPhysReg PhysReg);

  /// Find a free register of the same register class type
  MCPhysReg getReplacementPhysReg(const Register Reg, RoundRobin &Registers,
                                  BitVector &UsedUnits) const;

  /// Whether \p Reg should be considered a candidate for re-assignment.
  bool isWorthRenaming(const Register &Reg,
                       const BitVector &VRegWithCopies) const;

  /// return the Physical register of the Register, look it up in VirtRegMap if
  /// the Reg is virtual
  MCPhysReg getAssignedPhysReg(const Register Reg) const;

  bool isIdentityCopy(const MachineInstr &MI) const;

  /// return a BitVector to identify if a VirtualRegister has been defined by at
  /// least one copy.
  /// The Virtual Registers are accessed by the VirtRegIndex
  BitVector getVRegWithCopies(const MachineBasicBlock &MBB) const;

  /// MachineInstructions can write into sub-registers (%0:sub_256_lo and
  /// %0:sub_256_hi). In the VirtualRegisterMap, they block the same
  /// register (i.e. $x0). This can cause issues, when detecting Write
  /// After Write (WAW) dependencies. To mitigate this, only use the last
  /// definition of a virtual register to count the definitions, so that
  /// writing into subregisters does not already trigger the register
  /// replacement. In this case $x0 would have already been replaced, even
  /// though there is no real WAW dependency.
  ///                                                                          \
  /// undef %0.sub_256_lo:mxa, _, _= VLDA_2D_dmw_lda_w _,_                     \
  /// % 0.sub_256_hi:vec512 = VLDA_dmw_lda_w_ag_idx_imm _,_                    \
  /// %7:mxm, %8:el = VMAX_LT_D8 %0, $x6                                       \
  ///                                                                          \
  /// returns a mapping between a virtual register and its last defining machine
  /// instruction.
  IndexedMap<const MachineInstr *, VirtReg2IndexFunctor>
  getLastVRegDef(const MachineBasicBlock &MBB) const;

  /// For a given MBB, get the physical registers that are mapped to register
  /// operands with high output latency. This also includes alias and
  /// sub-registers.
  std::set<MCRegister>
  getHighOutputLatencyRegs(const MachineBasicBlock *MBB) const;
};

MCPhysReg AIEWawRegRewriter::getAssignedPhysReg(const Register Reg) const {
  assert(Reg.isPhysical() || Reg.isVirtual());

  if (Reg.isVirtual())
    return VRM->getPhys(Reg);

  return Reg;
}

void AIEWawRegRewriter::setArchSpecificPassOptions() {
  // TODO: use loop classes to enable WAW strategies.
  // This optimization is specifically only tested for AIE2P.
  if (!MF->getTarget().getTargetTriple().isAIE2P())
    return;

  // NumOccurrences increments if it is set by a command line argument
  const bool UseDefaultPreAlloc = PreAlloc.getNumOccurrences() == 0;
  if (UseDefaultPreAlloc)
    PreAlloc = true;
}

bool AIEWawRegRewriter::runOnMachineFunction(MachineFunction &MF) {

  SmallVector<MachineBasicBlock *, 4> LoopMBBs =
      AIELoopUtils::getSingleBlockLoopMBBs(MF);

  if (LoopMBBs.empty())
    return false;

  this->MF = &MF;
  MRI = &MF.getRegInfo();
  TRI = static_cast<const AIEBaseRegisterInfo *>(MRI->getTargetRegisterInfo());
  VRM = &getAnalysis<VirtRegMapWrapperLegacy>().getVRM();
  LRM = &getAnalysis<LiveRegMatrixWrapperLegacy>().getLRM();
  LIS = &getAnalysis<LiveIntervalsWrapperPass>().getLIS();
  TII = static_cast<const AIEBaseInstrInfo *>(MF.getSubtarget().getInstrInfo());
  bool Modified = false;
  setArchSpecificPassOptions();

  LLVM_DEBUG(dbgs() << "*** WAW Loop Register Rewriting: " << MF.getName());
  LLVM_DEBUG(dbgs() << " ***\n");

  for (const MachineBasicBlock *MBB : LoopMBBs)
    Modified |= renameMBBPhysRegs(MBB);

  return Modified;
}

BitVector
AIEWawRegRewriter::getVRegWithCopies(const MachineBasicBlock &MBB) const {
  // FIXME: The current heuristic would still rename a register used in a copy
  // if there are several inner loops in the kernel. A solution could be to keep
  // a running list of VRegWithCopies across various MBBs.

  SmallVector<unsigned, 16> VRegs;
  unsigned MaxVReg = 0;
  for (const MachineInstr &MI : MBB) {
    for (const MachineOperand Def : MI.defs()) {
      Register Reg = Def.getReg();

      if (!Reg.isVirtual())
        continue;

      unsigned VRegIndex = Reg.virtRegIndex();
      // Same-bank copies can be folded, so don't rename them
      if (const auto DestSource = TII->isCopyInstr(MI)) {
        const Register SrcReg = DestSource->Source->getReg();
        const bool IsSameBankVRegCopy =
            SrcReg.isVirtual() &&
            MRI->getRegClass(Reg) == MRI->getRegClass(SrcReg);
        if (IsSameBankVRegCopy)
          VRegs.push_back(VRegIndex);
      }

      if (VRegIndex > MaxVReg)
        MaxVReg = VRegIndex;
    }
  }

  // copy to BitVector so that lookups become very cheap
  BitVector VRegWithCopies(MaxVReg + 1);
  for (const unsigned RegIndex : VRegs)
    VRegWithCopies[RegIndex] = true;

  return VRegWithCopies;
}

void AIEWawRegRewriter::preAllocate(OriginalAllocation &Candidates,
                                    RoundRobin &LRURegisters) {
  // This is tracking any used register unit across the entire loop.
  BitVector UsedUnits(TRI->getNumRegUnits());
  for (auto &[VReg, Org] : Candidates) {
    assert(VReg.isVirtual());
    (void)getReplacementPhysReg(VReg, LRURegisters, UsedUnits);
  }
}

bool AIEWawRegRewriter::reAllocate(OriginalAllocation &Candidates,
                                   RoundRobin &Registers) {
  bool Success = true;
  // This is tracking any used register unit across the entire loop.
  BitVector UsedUnits(TRI->getNumRegUnits());
  for (auto &[VReg, Org] : Candidates) {
    if (!replaceReg(VReg, Registers, UsedUnits)) {
      LLVM_DEBUG(dbgs() << "Renaming " << printReg(VReg, TRI, 0, MRI)
                        << " failed\n");
      Success = false;
    }
  }
  return Success;
}

void AIEWawRegRewriter::revertAllocation(
    ArrayRef<std::pair<Register, MCPhysReg>> Entries) {
  LLVM_DEBUG(dbgs() << "=== revertAllocation: Processing " << Entries.size()
                    << " entries ===\n");

  // When reallocation fails, some candidates may not have been assigned a
  // physical register. We must revert those to their original assignments.
  // However, reverting to originals may conflict with NEW assignments of
  // successfully reallocated candidates. This creates a cascading effect
  // where those successful candidates must also revert to make room. We use
  // a fixed-point iteration to find all candidates that need to revert.

  const unsigned NumRegUnits = TRI->getNumRegUnits();

  // BlockedUnits tracks RegUnits that will be occupied by reverted originals.
  // Any NEW assignment overlapping these units must also revert.
  BitVector BlockedUnits(NumRegUnits);

  // ToRevert tracks which virtual registers need to revert to their originals.
  BitVector ToRevert(MRI->getNumVirtRegs());

  // Helper: Get RegUnits for a physical register as a BitVector.
  auto GetRegUnits = [&](MCPhysReg Reg) {
    BitVector Units(NumRegUnits);
    for (MCRegUnit Unit : TRI->regunits(Reg))
      Units.set(Unit);
    return Units;
  };

  // Phase 1: Seed BlockedUnits with originals from unassigned candidates.
  // Entries without a physical assignment must revert to their originals.
  // Remaining holds candidates with valid new assignments to check for
  // conflicts.
  LLVM_DEBUG(dbgs() << "Phase 1: Identifying failed/unassigned candidates\n");
  std::queue<std::pair<Register, MCPhysReg>> Remaining;
  for (const auto &[VReg, Org] : Entries) {
    const bool HasPhys = VRM->hasPhys(VReg);

    if (!HasPhys) {
      ToRevert.set(VReg.virtRegIndex());
      BlockedUnits |= GetRegUnits(Org);
      LLVM_DEBUG(dbgs() << "  " << printReg(VReg, TRI) << " -> must revert to "
                        << printReg(Org, TRI) << " (no phys)\n");
    } else {
      Remaining.emplace(VReg, Org);
      LLVM_DEBUG(dbgs() << "  " << printReg(VReg, TRI) << " -> remaining (new="
                        << printReg(VRM->getPhys(VReg), TRI)
                        << ", orig=" << printReg(Org, TRI) << ")\n");
    }
  }
  LLVM_DEBUG(dbgs() << "Phase 1 complete: " << ToRevert.count()
                    << " to revert, " << Remaining.size() << " remaining\n");

  // Phase 2: Fixed-point iteration to find cascading conflicts.
  // A successful candidate must revert if its NEW assignment overlaps with
  // BlockedUnits. When it reverts, its original is added to BlockedUnits,
  // potentially causing more candidates to revert.
  // Convergence is guaranteed: Remaining shrinks each iteration and
  // BlockedUnits only grows. Loop exits when no new conflicts are found.
  LLVM_DEBUG(
      dbgs() << "Phase 2: Fixed-point iteration for cascading conflicts\n");

  unsigned Iteration = 0;
  unsigned SkipCounter = 0;
  // If we add VReg to ToRevert and then we reach the same entry again without
  // adding anything to ToRevert, we converged.
  while (!Remaining.empty() && SkipCounter <= Remaining.size()) {
    auto Entry = Remaining.front();
    Remaining.pop();
    const auto &[VReg, Org] = Entry;
    const BitVector NewUnits = GetRegUnits(VRM->getPhys(VReg));
    if (NewUnits.anyCommon(BlockedUnits)) {
      // NEW assignment conflicts with a reverted original - must revert.
      ToRevert.set(VReg.virtRegIndex());
      BlockedUnits |= GetRegUnits(Org);
      LLVM_DEBUG(dbgs() << "  Iteration " << Iteration << ": "
                        << printReg(VReg, TRI) << " conflicts (new="
                        << printReg(VRM->getPhys(VReg), TRI)
                        << "), reverting to " << printReg(Org, TRI) << "\n");
      SkipCounter = 0;
    } else {
      Remaining.push(Entry);
      SkipCounter++;
    }
    Iteration = (SkipCounter == Remaining.size()) ? Iteration + 1 : Iteration;
    (void)Iteration;
  }

  LLVM_DEBUG(dbgs() << "Phase 2 complete after " << Iteration << " iterations: "
                    << ToRevert.count() << " total to revert, "
                    << Remaining.size() << " unchanged\n");

  // Phase 3: Apply reversions.
  // First unassign all to avoid conflicts, then reassign to originals.
  LLVM_DEBUG(dbgs() << "Phase 3: Applying reversions\n");
  for (const auto &[VReg, Org] : Entries) {
    if (ToRevert.test(VReg.virtRegIndex()) && VRM->hasPhys(VReg))
      unassignReg(VReg);
  }

  for (const auto &[VReg, Org] : Entries) {
    if (ToRevert.test(VReg.virtRegIndex())) {
      LLVM_DEBUG(dbgs() << "  Reverting " << printReg(VReg, TRI) << " to "
                        << printReg(Org, TRI) << "\n");
      assignReg(VReg, Org);
    }
  }
  LLVM_DEBUG(dbgs() << "=== revertAllocation complete ===\n");
}

RoundRobin
AIEWawRegRewriter::computeLRURegisters(const OriginalAllocation &Candidates) {
  RoundRobin LRURegisters;

  // ExcludedPhysregs makes sure that each register is only added once to the
  // LRU queue. Additionally, it excludes the callee saved registers
  BitVector ExcludedPhysRegs{TRI->getNumRegs()};

  for (const MCPhysReg *CSR = MRI->getCalleeSavedRegs(); CSR && *CSR; ++CSR)
    ExcludedPhysRegs[*CSR] = true;

  // For each reg class, allocate the candidates in round-robin fashion.
  // If we fail, we fall back to the original allocation
  for (const auto &[VReg, Org] : Candidates) {
    const auto *RC = MRI->getRegClass(VReg);
    LLVM_DEBUG(dbgs() << "Allowed registers in RC=" << TRI->getRegClassName(RC)
                      << ":");
    for (MCPhysReg PhysReg : RC->getRegisters()) {
      if (!ExcludedPhysRegs[PhysReg]) {
        LLVM_DEBUG(dbgs() << " " << printReg(PhysReg, TRI));
        LRURegisters.push_back(PhysReg);
      }
      ExcludedPhysRegs[PhysReg] = true;
    }
    LLVM_DEBUG(dbgs() << "\n");
  }
  return LRURegisters;
}

void AIEWawRegRewriter::sortSWPAware(
    OriginalAllocation &Candidates, MachineBasicBlock &MBB,
    const llvm::AIE::SlotStatistics &Statistics, int Bias,
    const IndexedMap<const MachineInstr *, VirtReg2IndexFunctor> &LastVRegDef) {
  // We estimate the length of the schedule based on latencies and the
  // minimum II based on slots. We then estimate the modulo cycle of each
  // instruction based on its depth and apply LRU in the order of the modulo
  // cycle.
  // Note that both the depth and the II are underestimations since we don't
  // account for them interfering. Hence the modulo cycle estimate won't be
  // too far off.
  const int MinII = std::max(Statistics.getMinII() + Bias, 1);

  MachineSchedContext Context;
  Context.MF = MF;
  Context.AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
  AIE::DataDependenceHelper DDG(Context, true, false);
  for (auto &MI : MBB) {
    if (!MI.isTerminator())
      DDG.initSUnit(MI);
  }
  DDG.buildEdges();
  DEBUG_DETAIL(DDG.dumpDot(dbgs(), false));

  // Compute and record the modulo cycle of each instruction.
  std::map<const MachineInstr *, int> ModuloCycle;
  for (auto &SU : DDG.SUnits) {
    int D = SU.getDepth();
    ModuloCycle.emplace(SU.getInstr(), D % MinII);
    LLVM_DEBUG(dbgs() << format("%4d D=%4d: ", SU.NodeNum, D)
                      << *SU.getInstr());
  }

  LLVM_DEBUG(dbgs() << format("MinII = %d\n", MinII));

  // Now sort the candidates to simulate the parallelism
  using Element = std::pair<Register, MCPhysReg>;
  auto ModuloCycleLess = [&ModuloCycle, &LastVRegDef](const Element &A,
                                                      const Element &B) {
    const MachineInstr *IA = LastVRegDef[A.first];
    const MachineInstr *IB = LastVRegDef[B.first];
    return ModuloCycle[IA] < ModuloCycle[IB];
  };
  llvm::sort(Candidates, ModuloCycleLess);
}

bool AIEWawRegRewriter::renameMBBPhysRegs(const MachineBasicBlock *MBB) {
  LLVM_DEBUG(dbgs() << "WAW Reg Renaming BasicBlock "; MBB->dump();
             dbgs() << "\n");

  // Build per-loop option overrides from !llvm.loop metadata.
  Overrides = AIE::LoopOptionOverrides(*MBB);

  // Collect all the virtual registers that have at least a copy instruction
  // that defines them. Subregisters may contain constants that may be shared
  // across different virtual registers. Renaming would reintroduce unnecessary
  // copies, if physical registers are shared. Also do not rename copies, since
  // they could be removed in a later pass.
  BitVector VRegWithCopies = getVRegWithCopies(*MBB);

  IndexedMap<const MachineInstr *, VirtReg2IndexFunctor> LastVRegDef =
      getLastVRegDef(*MBB);

  auto &NonConstMBB = *(const_cast<MachineBasicBlock *>(MBB));
  AIE::SlotStatistics Statistics = AIE::computeSlotStatistics(NonConstMBB, TII);
  const int LoopClass = llvm::AIE::classifyLoop(Statistics);
  LLVM_DEBUG(dbgs() << "Stats="; Statistics.dumpShort(); dbgs() << "\n");
  LLVM_DEBUG(dbgs() << "LoopClass=" << LoopClass << "\n");

  RewriteMode Mode = selectMode(Overrides.get(RegRewriteMode), LoopClass);

  std::set<MCRegister> HighLatencyRegs;
  if (Mode == RewriteMode::LatencyAware) {
    HighLatencyRegs = getHighOutputLatencyRegs(MBB);
  }

  OriginalAllocation Candidates;

  for (const MachineInstr &MI : *MBB) {
    // Identity copies will be removed in a later pass, therefore, these are not
    // real defines of a physical register
    if (isIdentityCopy(MI))
      continue;

    for (const MachineOperand &MO : MI.defs()) {
      Register Reg = MO.getReg();
      if (!Reg.isVirtual())
        continue;
      if (VRM->hasRequiredPhys(Reg))
        continue;
      if (MO.isTied())
        continue;
      // several definitions of the same virtual register are not relevant
      // because even if the virtual register is renamed, by construction
      // all the definitions would be renamed as well and achieve nothing wrt
      // WAW dependency resolution
      if (LastVRegDef[Reg] != &MI)
        continue;
      // See llvm bug #48911.
      // Skip reassign if a register has originated from InlineSpiller.
      // FIXME: Remove the workaround when bug #48911 is fixed.
      if (VRM->getPreSplitReg(Reg)) {
        continue;
      }
      if (isWorthRenaming(Reg, VRegWithCopies)) {
        assert(VRM->hasPhys(Reg));
        MCRegister AssignedPhysReg = VRM->getPhys(Reg);

        // High latency registers should be renamed first, therefore insert them
        // at the front.
        const auto InsertPoint = HighLatencyRegs.count(AssignedPhysReg)
                                     ? Candidates.begin()
                                     : Candidates.end();
        Candidates.emplace(InsertPoint, Reg, AssignedPhysReg);

        LLVM_DEBUG(dbgs() << "Candidate " << printReg(Reg, TRI, 0, MRI) << ":"
                          << TRI->getRegClassName(MRI->getRegClass(Reg)) << " ("
                          << TRI->getName(AssignedPhysReg) << ")\n");
      }
    }
  }

  // Free physregs of all candidates
  for (auto &[VReg, Org] : Candidates) {
    if (VRM->hasPhys(VReg))
      unassignReg(VReg);
  }
  LLVM_DEBUG(dbgs() << "Renaming " << Candidates.size() << " candidates\n");

  // If requested, unassign MBB's liveins as well to get even more freedom
  SmallVector<std::pair<Register, MCPhysReg>> FreedLiveIns;
  if (Overrides.get(AggressiveReAlloc)) {
    for (unsigned I = 0, E = MRI->getNumVirtRegs(); I != E; ++I) {
      Register Reg = Register::index2VirtReg(I);
      if (!LIS->hasInterval(Reg))
        continue;
      LiveInterval &LI = LIS->getInterval(Reg);
      if (LIS->isLiveInToMBB(LI, MBB) && VRM->hasPhys(Reg)) {
        FreedLiveIns.emplace_back(Reg, VRM->getPhys(Reg));
        unassignReg(Reg);
      }
    }
  }

  if (auto Bias = getMinIIBias(Mode, LoopClass)) {
    sortSWPAware(Candidates, NonConstMBB, Statistics, *Bias, LastVRegDef);
  }

  // Least-Recently-Used list of physical registers for assignments to VRegs.
  // Physical registers that have recently been used are moved to the back.
  RoundRobin LRURegisters = computeLRURegisters(Candidates);

  // Prime the LRURegisters, so that the allocation is loop-aware.
  if (Overrides.get(PreAlloc)) {
    preAllocate(Candidates, LRURegisters);
  }

  if (!reAllocate(Candidates, LRURegisters)) {
    llvm::append_range(Candidates, FreedLiveIns);
    revertAllocation(Candidates);
    return false;
  }

  return true;
}

bool AIEWawRegRewriter::isWorthRenaming(const Register &Reg,
                                        const BitVector &VRegWithCopies) const {
  assert(Reg.isVirtual());

  // The register might have been de-allocated when processing another loop.
  if (!VRM->hasPhys(Reg))
    return false;

  // Only consider vec/acc registers as candidates, and optionally GPRs.
  bool IsCandidateClass =
      TRI->isVecOrAccRegClass(*(MRI->getRegClass(Reg))) ||
      (Overrides.get(GPRRealloc) &&
       TRI->getGPRRegClass(*MF)->hasSubClassEq(MRI->getRegClass(Reg)));
  if (!IsCandidateClass)
    return false;

  return !VRegWithCopies[Reg.virtRegIndex()];
}

void AIEWawRegRewriter::unassignReg(Register VReg) {
  const LiveInterval &LI = LIS->getInterval(VReg);
  LRM->unassign(LI);
}

void AIEWawRegRewriter::assignReg(Register VReg, MCPhysReg PhysReg) {
  const LiveInterval &LI = LIS->getInterval(VReg);
  if (VRM->hasPhys(VReg)) {
    LRM->unassign(LI);
  }
  LRM->assign(LI, PhysReg);
}

bool AIEWawRegRewriter::replaceReg(const Register VReg,
                                   RoundRobin &LRURegisters,
                                   BitVector &UsedUnits) {
  assert(VReg.isVirtual());
  MCPhysReg ReplacementPhysReg =
      getReplacementPhysReg(VReg, LRURegisters, UsedUnits);

  if (ReplacementPhysReg == MCRegister::NoRegister)
    return false;

  LLVM_DEBUG(dbgs() << "     replace: " << printReg(VReg, TRI) << " with "
                    << TRI->getName(ReplacementPhysReg) << '\n');
  assert(Register::isPhysicalRegister(ReplacementPhysReg));

  assignReg(VReg, ReplacementPhysReg);
  return true;
}

/// Returns a vreg of the same class that is exclusively used (and killed)
/// at the point \p VReg gets defined.
std::optional<Register>
getKilledRegAtSingleDefPoint(Register VReg, const MachineRegisterInfo &MRI) {
  MachineOperand *MO = MRI.getOneDef(VReg);
  if (!MO)
    return std::nullopt;

  MachineInstr &DefMI = *MO->getParent();
  auto OnlyUsedByInstr = [&MRI](Register Reg, const MachineInstr &MI) {
    return all_of(MRI.use_instructions(Reg),
                  [&MI](const MachineInstr &UseMI) { return &UseMI == &MI; });
  };

  for (MachineOperand &UseMO : DefMI.explicit_uses()) {
    if (UseMO.isReg() && UseMO.getReg().isVirtual() &&
        MRI.getRegClass(VReg) == MRI.getRegClass(UseMO.getReg()) &&
        OnlyUsedByInstr(UseMO.getReg(), DefMI)) {
      return UseMO.getReg();
    }
  }
  return std::nullopt;
}

void moveRegAndAliasesBack(MCPhysReg PhysReg, RoundRobin &LRURegisters,
                           const TargetRegisterInfo *TRI) {
  for (MCRegAliasIterator AI(MCRegister(PhysReg), TRI, true); AI.isValid();
       ++AI) {
    // TODO: Use hints to speed up the search of aliases?
    auto AliasIt = llvm::find(LRURegisters, *AI);
    if (AliasIt != LRURegisters.end()) {
      LRURegisters.erase(AliasIt);
      LRURegisters.emplace_back(*AI);
    }
  }
}

MCPhysReg AIEWawRegRewriter::getReplacementPhysReg(const Register VReg,
                                                   RoundRobin &LRURegisters,
                                                   BitVector &UsedUnits) const {
  assert(VReg.isVirtual() && "Reg has to be a virtual register");

  /// Whether \p PhysReg was ever used for re-assigning a vreg
  auto WasUsedForReassignment = [TRI = this->TRI,
                                 &UsedUnits](MCPhysReg PhysReg) {
    return any_of(TRI->regunits(PhysReg),
                  [&UsedUnits](MCRegUnit RU) { return UsedUnits.test(RU); });
  };

  LLVM_DEBUG(dbgs() << "     Try to re-assign" << printReg(VReg, TRI) << "\n");
  const TargetRegisterClass *RC = MRI->getRegClass(VReg);
  const LiveInterval &LI = LIS->getInterval(VReg);

  MCPhysReg PhysReg = AIERegAllocationUtils::findFreeNonOverlappingPhysReg(
      LI, *RC, LRURegisters, BitVector(), *TRI, *LRM);
  if (!PhysReg)
    return MCRegister::NoRegister;

  assert(llvm::is_contained(LRURegisters, PhysReg));

  // Prefer a previously used killed register when it remains free.
  if (std::optional<Register> KilledReg =
          getKilledRegAtSingleDefPoint(VReg, *MRI);
      KilledReg && WasUsedForReassignment(PhysReg)) {
    MCRegister KilledPhysReg = getAssignedPhysReg(*KilledReg);
    if (KilledPhysReg &&
        LRM->checkInterference(LI, KilledPhysReg) == LiveRegMatrix::IK_Free) {

      LLVM_DEBUG(dbgs() << "     re-use killed physreg for assigning: "
                        << printReg(VReg, TRI) << " to "
                        << TRI->getName(KilledPhysReg) << '\n');
      PhysReg = KilledPhysReg;
      assert(llvm::is_contained(LRURegisters, KilledPhysReg));
    }
  }

  moveRegAndAliasesBack(PhysReg, LRURegisters, TRI);
  AIERegAllocationUtils::reserveRegUnits(PhysReg, *TRI, UsedUnits);
  return PhysReg;
}

bool AIEWawRegRewriter::isIdentityCopy(const MachineInstr &MI) const {
  std::optional<DestSourcePair> DestSource = TII->isCopyInstr(MI);
  if (!DestSource.has_value())
    return false;

  return getAssignedPhysReg(DestSource->Source->getReg()) ==
         getAssignedPhysReg(DestSource->Destination->getReg());
}

IndexedMap<const MachineInstr *, VirtReg2IndexFunctor>
AIEWawRegRewriter::getLastVRegDef(const MachineBasicBlock &MBB) const {
  IndexedMap<const MachineInstr *, VirtReg2IndexFunctor> LastVRegDef;

  // Initialize the IndexedMap size
  LastVRegDef.grow(Register::index2VirtReg(MRI->getNumVirtRegs()));

  for (const MachineInstr &MI : llvm::reverse(MBB)) {
    for (const MachineOperand &Def : MI.defs()) {

      if (!Def.isReg() || !Def.getReg().isVirtual())
        continue;

      Register Reg = Def.getReg();
      LastVRegDef[Reg] = &MI;
    }
  }
  return LastVRegDef;
}

std::set<MCRegister> AIEWawRegRewriter::getHighOutputLatencyRegs(
    const MachineBasicBlock *MBB) const {
  auto *ItinData = MF->getSubtarget().getInstrItineraryData();
  std::set<MCRegister> HighLatRegisters;
  for (const MachineInstr &MI : *MBB) {
    const unsigned SchedClass = MI.getDesc().getSchedClass();
    for (unsigned I = 0, E = MI.getNumOperands(); I != E; ++I) {
      const MachineOperand &MO = MI.getOperand(I);
      if (!MO.isReg() || !MO.isDef())
        continue;

      const Register Reg = MO.getReg();
      if (!Reg.isVirtual())
        continue;

      auto IsHighLatInstrOperand = [&]() {
        auto OperandCycle = ItinData->getOperandCycle(SchedClass, I);
        if (OperandCycle)
          return OperandCycle.value() >= Overrides.get(MinRegisterLatency);
        // If we have an instruction without OperandCycles, it is most probably
        // a pseudo instruction (no itinerary). In this case, if it is a _split
        // load, consider it as high latency.
        return MI.mayLoad();
      };

      if (!IsHighLatInstrOperand())
        continue;

      // AggressiveReAlloc de-allocates the live-through registers of every
      // loop it processes and leaves them for the greedy run that follows this
      // pass. A later loop can therefore define a register that currently
      // holds no physreg, which then classifies as neither high nor low
      // latency. It cannot become a candidate here either, see isWorthRenaming.
      if (!VRM->hasPhys(Reg))
        continue;

      const MCRegister AssignedPhysReg = VRM->getPhys(Reg);
      for (MCRegAliasIterator AI(AssignedPhysReg, TRI, true); AI.isValid();
           ++AI)
        HighLatRegisters.insert(*AI);
    }
  }

  // This metric gives us an idea about the "demand" for high latency registers
  // in a specific loop. If the demand is low, clear HighLatRegisters to skip
  // latency-aware heuristic. Basically, we evaluate high latency register count
  // to instruction count ratio in percent.
  // TODO: this should be replaced by more stable metrics related to SWP.
  const int HighLatencyRegisterInstrRatio =
      ((HighLatRegisters.size() * 100) / MBB->size());
  if (HighLatencyRegisterInstrRatio < 250 /*calibrated value*/)
    HighLatRegisters.clear();

  return HighLatRegisters;
}

} // end anonymous namespace

char AIEWawRegRewriter::ID = 0;
char &llvm::AIEWawRegRewriterID = AIEWawRegRewriter::ID;

INITIALIZE_PASS(AIEWawRegRewriter, DEBUG_TYPE, "AIE waw-reg rewrite", false,
                false)

llvm::FunctionPass *llvm::createAIEWawRegRewriter() {
  return new AIEWawRegRewriter();
}
