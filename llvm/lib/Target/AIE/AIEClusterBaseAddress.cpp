//===--- AIEClusterBaseAddress.cpp - Base Address Clustering --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// AIE base address clustering to support post increment addressing.
//
// Cluster G_PTR_ADDs depending on the base address.
// Example:
//  Transform:
//    %1 = COPY $p1
//    %2 = G_CONSTANT i20 12
//    %3 = G_PTR_ADD %1, %2
//    G_LOAD %3
//    %5 = G_CONSTANT i20 16
//    %6 = G_PTR_ADD %1, %5
//    G_LOAD %6
//  Into:
//    %1 = COPY $p1
//    %2 = G_CONSTANT i20 12
//    %3 = G_PTR_ADD %1, %2
//    G_LOAD %3
//    %5 = G_CONSTANT i20 4
//    %6 = G_PTR_ADD %3, %5
//    G_LOAD %6
//
//  This will be later combined to
//    %1 = COPY $p1
//    %2 = G_CONSTANT i20 12
//    %3 = G_PTR_ADD %1, %2
//    %4 = G_CONSTANT i20 4
//    %_, %5 = G_AIE_POSTINC_LOAD %1, %4
//    G_LOAD %5
//
// TODO: As a preliminary implementation, we consider the ptr adds in only a
// single basic block. As such we try to avoid changing any ptr reg during
// clustering if we find that the base register of the ptr reg has uses later in
// the basic block. We need to implement a cross basic block approach where we
// are sure the clustering won't create any copies.
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/CSEMIRBuilder.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineDominators.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/InitializePasses.h"
#include <optional>
#include <set>

#define DEBUG_TYPE "aie-cluster-base-address"

using namespace llvm;

static const char AIE_CLUSTER_BASE_ADDRESS[] =
    "AIE Base Address Clustering Optimization";

static cl::opt<bool> EnableChainsForScalarLdSt(
    "aie-chain-addr-scl-ldst", cl::Hidden, cl::init(true),
    cl::desc("Enable ptradd chaining for scalar loads and stores."));

static cl::opt<bool> EnableChainsForVectorLdSt(
    "aie-chain-addr-vec-ldst", cl::Hidden, cl::init(true),
    cl::desc("Enable ptradd chaining for vector loads and stores."));

static cl::opt<bool> EnableChainsAcrossMultiBlocks(
    "aie-chain-addr-multi-block", cl::Hidden, cl::init(true),
    cl::desc("Enable ptradd chaining when Ptr is used across multiple MBBs."));

static cl::opt<bool>
    DetachCondLoadChain("aie-chain-addr-detach-cond-load-jump", cl::Hidden,
                        cl::init(true),
                        cl::desc("Disable ptradd chaining that feed "
                                 "loads that are used in conditional jumps."));

static cl::opt<bool>
    EnableStackChaining("aie-chain-stack", cl::Hidden, cl::init(true),
                        cl::desc("Enable pointer chaining for stack access."));

static cl::opt<bool> EnableCrossBlockChainReuse(
    "aie-chain-addr-cross-block-reuse", cl::Hidden, cl::init(true),
    cl::desc("Reuse the tail of a ptr-add chain produced in a strictly "
             "dominating MBB when starting a new chain on the same base."));

static cl::opt<unsigned> CrossBlockReuseMaxDomSteps(
    "aie-chain-addr-cross-block-reuse-max-steps", cl::Hidden, cl::init(8),
    cl::desc("Maximum number of dominator-tree steps to climb when searching "
             "for a reusable ptr-add chain tail. Bounds the liveness extension "
             "of the reused anchor."));

namespace {

LLT getLoadStoreType(const MachineInstr &MI) {
  return (*MI.memoperands_begin())->getMemoryType();
}

std::optional<int64_t> getConstOffset(const MachineInstr &MI,
                                      const MachineRegisterInfo &MRI) {
  assert(MI.getOpcode() == TargetOpcode::G_PTR_ADD);
  auto Off = getIConstantVRegValWithLookThrough(MI.getOperand(2).getReg(), MRI);
  if (!Off)
    return std::nullopt;
  return Off->Value.getSExtValue();
}

/// SSA def that equals BaseReg + Offset after the chain built in some MBB
/// has run to completion.
struct BlockEndOffset {
  Register EndReg;
  int64_t Offset;
};

/// Chain-tail defs published by a single MBB, keyed by base register.
using BaseToEndOffset = DenseMap<Register, BlockEndOffset>;

/// Per-(MBB, BaseReg) record of the chain-tail def published by each MBB.
class PtrChainEndTracker {
  const MachineDominatorTree &MDT;
  DenseMap<MachineBasicBlock *, BaseToEndOffset> Records;

public:
  PtrChainEndTracker(const MachineDominatorTree &MDT) : MDT(MDT) {}

  void recordOffsetAtBlockEnd(Register BaseReg, MachineBasicBlock *MBB,
                              Register EndReg, int64_t Offset) {
    assert(!Records[MBB].count(BaseReg) &&
           "duplicate base record for this MBB");
    Records[MBB][BaseReg] = {EndReg, Offset};
  }

  /// Walk MBB's idom chain upward and return the nearest dominating block's
  /// record for BaseReg, if any. The climb is bounded so the reused anchor's
  /// live range stays short: it stops after a fixed number of steps, never
  /// climbs past BaseReg's defining block (no dominator above it can hold a
  /// record for BaseReg), and never reuses an anchor across a call.
  std::optional<BlockEndOffset>
  findDominatingBlockEndOffset(Register BaseReg, MachineBasicBlock &MBB,
                               const MachineRegisterInfo &MRI) const {
    const MachineInstr *BaseDef = MRI.getVRegDef(BaseReg);
    const MachineBasicBlock *DefMBB = BaseDef ? BaseDef->getParent() : nullptr;

    auto *N = MDT.getNode(&MBB);
    unsigned StepsLeft = CrossBlockReuseMaxDomSteps;
    for (N = N ? N->getIDom() : nullptr; N && StepsLeft; N = N->getIDom()) {
      --StepsLeft;
      const MachineBasicBlock *DomBlock = N->getBlock();

      // Look up a tail this dominator published for BaseReg. A dominator with
      // no records at all, or none for BaseReg, just falls through and we keep
      // climbing toward a higher dominator that may hold one.
      const auto BlockIt = Records.find(DomBlock);
      if (BlockIt != Records.end()) {
        const auto It = BlockIt->second.find(BaseReg);
        if (It != BlockIt->second.end())
          return It->second;
      }

      // No dominator above BaseReg's defining block can hold a record for it.
      if (DomBlock == DefMBB)
        break;

      // Reusing an anchor from a higher dominator would stretch its live range
      // across this block's call, exposing it to callee clobbers and spills.
      const bool DomBlockHasCall = llvm::any_of(
          *DomBlock, [](const MachineInstr &MI) { return MI.isCall(); });
      if (DomBlockHasCall)
        break;
    }
    return std::nullopt;
  }
};

/// Try and re-order PTR_ADD instructions to maximise the size of constant
/// PTR_ADD chains.
bool bundleConstIncrements(ArrayRef<MachineInstr *> PtrAdds,
                           const MachineRegisterInfo &MRI,
                           MachineIRBuilder &MIB,
                           GISelObserverWrapper &Observer) {
  bool Changed = false;

  // Look for the following sequence:
  // %0 = G_PTR_ADD %100, 64
  // %1 = G_PTR_ADD %100, %101
  // %2 = G_PTR_ADD %1, 32
  // And swap the offsets if it is safe to get:
  // %0 = G_PTR_ADD %100, 64
  // %1 = G_PTR_ADD %100, 32
  // %2 = G_PTR_ADD %1, %101
  for (MachineInstr *PtrAdd : PtrAdds) {
    assert(PtrAdd->getOpcode() == TargetOpcode::G_PTR_ADD);
    Register OutputPtr = PtrAdd->getOperand(0).getReg();
    Register OffsetReg = PtrAdd->getOperand(2).getReg();
    if (getConstOffset(*PtrAdd, MRI) || !MRI.hasOneNonDBGUser(OutputPtr))
      continue;

    // We found a non-constant PTRADD with a single user, now check if that
    // user is a constant PTRADD. If so, "swap" their offsets so that the
    // constant PTRADD appears first.
    MachineInstr &OutputUser = *MRI.use_instructions(OutputPtr).begin();
    if (OutputUser.getOpcode() != TargetOpcode::G_PTR_ADD)
      continue;
    std::optional<int64_t> CstOffset = getConstOffset(OutputUser, MRI);
    if (!CstOffset)
      continue;

    // Everything fine, now swap the offsets
    LLVM_DEBUG(dbgs() << "Swapping offsets for:\n      " << *PtrAdd << "  and "
                      << OutputUser);
    Observer.changingInstr(*PtrAdd);
    MIB.setInstr(*PtrAdd);
    Register NewOffsetReg =
        MIB.buildConstant(LLT::scalar(20), *CstOffset).getReg(0);
    PtrAdd->getOperand(2).setReg(NewOffsetReg);
    Observer.changedInstr(*PtrAdd);
    Observer.changingInstr(OutputUser);
    OutputUser.getOperand(2).setReg(OffsetReg);
    OutputUser.clearFlag(MachineInstr::NoUWrap);
    Observer.changedInstr(OutputUser);
    Changed = true;
  }

  return Changed;
}

class AIEClusterBaseAddress : public MachineFunctionPass {
public:
  static char ID;
  AIEClusterBaseAddress() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override;

  StringRef getPassName() const override { return AIE_CLUSTER_BASE_ADDRESS; }

  using RegUseMap = std::map<Register, SmallVector<MachineInstr *, 8>>;

private:
  const MachineRegisterInfo *MRI = nullptr;

  bool processBasicBlock(MachineBasicBlock &MBB, MachineIRBuilder &MIB,
                         GISelObserverWrapper &Observer,
                         PtrChainEndTracker &Tracker);

  bool rerootChainsInBlock(MachineBasicBlock &MBB, MachineIRBuilder &MIB,
                           GISelObserverWrapper &Observer,
                           PtrChainEndTracker &Tracker);

  // Create chaining opportunities related to FRAME_INDEX.
  bool convertFIToPtrAdd(MachineBasicBlock &MBB, MachineIRBuilder &MIB,
                         GISelObserverWrapper &Observer);

  // Get all candidates, i.e. groups of G_PTR_ADDs in the same
  // basic block that shares the same input pointer.
  RegUseMap collectPtrUses(MachineBasicBlock &MBB);

  // Evaluate if we consider a group of G_PTR_ADDs as a candidate to
  // create a chain.
  bool shouldSkipChaining(Register PtrReg,
                          const SmallVector<MachineInstr *, 8> &Instrs,
                          MachineBasicBlock &MBB);

  // Build a chain (or set of chains) of G_PTR_ADDs. We consider as
  // chain a linear sequence of linked G_PTR_ADDs, tied to output and
  // input pointers.
  bool buildChain(SmallVector<MachineInstr *, 8> &Instrs,
                  MachineBasicBlock &MBB, MachineIRBuilder &MIB,
                  GISelObserverWrapper &Observer);

  // Re-root every G_PTR_ADD on BaseReg in MBB onto the tail of a dominating
  // chain. All-or-nothing: leaving any sibling on BaseReg keeps BaseReg live
  // through the rerouted chain and forces a reload around it.
  bool tryRerootBucket(Register BaseReg, ArrayRef<MachineInstr *> Heads,
                       MachineBasicBlock &MBB, MachineIRBuilder &MIB,
                       GISelObserverWrapper &Observer,
                       PtrChainEndTracker &Tracker);

  // Evaluate if we should break the chain construction.
  // Criteria:
  //  * Unknown offsets.
  //  * Pointer shared between load(s) and store(s).
  bool shouldBreakChain(MachineInstr *MIA, MachineInstr *MIB,
                        std::optional<int64_t> OffsetA,
                        std::optional<int64_t> OffsetB);

  // Return true if the instructions are used by both loads and stores.
  bool hasMixedLoadStoreUse(SmallVector<MachineInstr *, 2> Instrs);

  // Get a set of all reachable MBBs from a given MBB.
  // Loops are handled using the ReachableMBBs set, once we encounter any
  // reachable MBB from a particular MBB, we store it in the set and continue if
  // we find it again. Lastly, we remove the current MBB from the set in case it
  // comes up in the successive basic blocks.
  std::set<MachineBasicBlock *> findReachableMBBs(MachineBasicBlock *MBB);

  // Find if a register is used in reachable MBBs.
  bool isRegUsedInSuccessiveMBBs(MachineBasicBlock *MBB, Register Reg);

  /// Return a set of Load Instrs whose results are used in the path of
  /// the conditional branch of \p MBB .
  std::set<MachineInstr *>
  getLoadsFeedingCondBranch(MachineBasicBlock &MBB) const;

  /// \return whether PtrAdd uses a Load Instr in \p LoadsToAvoid .
  bool avoidPtrAdd(MachineInstr *PtrAdd,
                   const std::set<MachineInstr *> &LoadsToAvoid) const;
};

void AIEClusterBaseAddress::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.addRequired<MachineModuleInfoWrapperPass>();
  AU.addRequired<GISelCSEAnalysisWrapperPass>();
  AU.addRequired<TargetPassConfig>();
  AU.addRequired<MachineDominatorTreeWrapperPass>();
  AU.setPreservesAll();
}

bool AIEClusterBaseAddress::runOnMachineFunction(MachineFunction &MF) {
  MRI = &MF.getRegInfo();
  TargetPassConfig &TPC = getAnalysis<TargetPassConfig>();
  // Enable CSE.
  GISelCSEAnalysisWrapper &Wrapper =
      getAnalysis<GISelCSEAnalysisWrapperPass>().getCSEWrapper();
  auto *CSEInfo = &Wrapper.get(TPC.getCSEConfig());
  std::unique_ptr<MachineIRBuilder> Builder =
      CSEInfo ? std::make_unique<CSEMIRBuilder>()
              : std::make_unique<MachineIRBuilder>();
  Builder->setMF(MF);
  MachineIRBuilder &MIB = *Builder;
  // Set Observer
  GISelObserverWrapper Observer;
  if (CSEInfo) {
    Observer.addObserver(CSEInfo);
    MIB.setChangeObserver(Observer);
  }

  bool Changed = false;

  const MachineDominatorTree &MDT =
      getAnalysis<MachineDominatorTreeWrapperPass>().getDomTree();
  PtrChainEndTracker Tracker(MDT);

  // Phase 1: build chains and record each chain tail at end of its MBB.
  for (MachineBasicBlock &MBB : MF) {
    if (EnableStackChaining)
      Changed |= convertFIToPtrAdd(MBB, MIB, Observer);
    Changed |= processBasicBlock(MBB, MIB, Observer, Tracker);
  }

  // Phase 2: with all tails recorded, try to re-root each chain head on a
  // dominating chain's tail. Independent of iteration order.
  if (EnableCrossBlockChainReuse)
    for (MachineBasicBlock &MBB : MF)
      Changed |= rerootChainsInBlock(MBB, MIB, Observer, Tracker);

  return Changed;
}

bool AIEClusterBaseAddress::processBasicBlock(MachineBasicBlock &MBB,
                                              MachineIRBuilder &MIB,
                                              GISelObserverWrapper &Observer,
                                              PtrChainEndTracker &Tracker) {

  bool Changed = false;

  // Get all G_PTR_ADDs that use the same pointer.
  RegUseMap RegAndUses = collectPtrUses(MBB);

  // Optimise instruction order
  for (auto &RegAndUse : reverse(RegAndUses)) {
    // Chaining acceptance criteria.
    SmallVector<MachineInstr *, 8> &Instrs = RegAndUse.second;
    if (shouldSkipChaining(RegAndUse.first, Instrs, MBB))
      continue;

    ArrayRef<MachineInstr *> PtrAdds = RegAndUse.second;
    Changed |= bundleConstIncrements(PtrAdds, *MRI, MIB, Observer);
  }

  // Create chains, when profitable.
  for (auto &[BaseReg, Instrs] : RegAndUses) {
    // Chaining acceptance criteria.
    if (shouldSkipChaining(BaseReg, Instrs, MBB))
      continue;

    // Snapshot the tail's offset from BaseReg before buildChain rewrites
    // operand(2) into a per-hop delta.
    const std::optional<int64_t> TailOffsetFromBase =
        getConstOffset(*Instrs.back(), *MRI);

    // Build chain, breaking it (or restarting it) when necessary.
    Changed |= buildChain(Instrs, MBB, MIB, Observer);
    // Record the chain tail offset at the end of the block.
    if (TailOffsetFromBase)
      Tracker.recordOffsetAtBlockEnd(BaseReg, &MBB,
                                     Instrs.back()->getOperand(0).getReg(),
                                     *TailOffsetFromBase);
  }
  return Changed;
}

bool AIEClusterBaseAddress::rerootChainsInBlock(MachineBasicBlock &MBB,
                                                MachineIRBuilder &MIB,
                                                GISelObserverWrapper &Observer,
                                                PtrChainEndTracker &Tracker) {
  // Group G_PTR_ADDs in MBB by base register. Reroot each bucket only if
  // every head in it can be rerouted onto the same dominating anchor -
  // otherwise rerooting a subset leaves BaseReg live in MBB and forces a
  // reload of the live-in around the chain.
  RegUseMap RegAndUses = collectPtrUses(MBB);
  bool Changed = false;
  for (auto &RegAndUse : RegAndUses)
    Changed |= tryRerootBucket(RegAndUse.first, RegAndUse.second, MBB, MIB,
                               Observer, Tracker);
  return Changed;
}

/// Recursively search bottom up for Load instrs in the use chain of \p MI .
/// Stop the search when Exiting \p MBB . Return all found Load MachineInstr in
/// \p LoadsFeedingInstrs .
void findLoadsFeedingInstr(MachineInstr &MI, MachineBasicBlock *MBB,
                           std::set<MachineInstr *> &LoadsFeedingInstrs,
                           const MachineRegisterInfo &MRI) {
  for (MachineOperand &MO : MI.uses()) {
    if (!MO.isReg())
      continue;

    Register UseReg = MO.getReg();
    if (!UseReg.isVirtual())
      continue;

    auto *UseMI = MRI.getUniqueVRegDef(UseReg);
    if (!UseMI)
      continue;

    if (UseMI->getParent() != MBB || UseMI->isPHI())
      continue;

    if (UseMI->mayLoad()) {
      LoadsFeedingInstrs.emplace(UseMI);
      LLVM_DEBUG(dbgs() << "Found Feeding Load " << *UseMI);
    }

    findLoadsFeedingInstr(*UseMI, MBB, LoadsFeedingInstrs, MRI);
  }
}

std::set<MachineInstr *>
AIEClusterBaseAddress::getLoadsFeedingCondBranch(MachineBasicBlock &MBB) const {
  assert(MRI);

  if (!DetachCondLoadChain)
    return {};

  std::set<MachineInstr *> LoadsFeedingCondBranch;
  for (auto &MI : make_range(MBB.getFirstTerminator(), MBB.end())) {
    if (MI.isConditionalBranch()) {
      findLoadsFeedingInstr(MI, &MBB, LoadsFeedingCondBranch, *MRI);
      break;
    }
  }

  return LoadsFeedingCondBranch;
}

bool AIEClusterBaseAddress::avoidPtrAdd(
    MachineInstr *PtrAdd, const std::set<MachineInstr *> &LoadsToAvoid) const {
  assert(PtrAdd->getOpcode() == TargetOpcode::G_PTR_ADD);

  // Is G_PTR_ADD feeding a Load instruction?
  const Register DefReg = PtrAdd->getOperand(0).getReg();
  if (MRI->use_nodbg_empty(DefReg))
    return false;

  auto UseBegin = MRI->use_instr_nodbg_begin(DefReg);
  MachineInstr *LoadMI = &*UseBegin;
  if (!LoadMI->mayLoad())
    return false;

  const bool LoadFeedCondBranch = LoadsToAvoid.count(LoadMI);
  LLVM_DEBUG(if (LoadFeedCondBranch) dbgs()
                 << "Found Load feeding Cond Branch attached to " << *PtrAdd;);

  return LoadFeedCondBranch;
}

bool AIEClusterBaseAddress::convertFIToPtrAdd(MachineBasicBlock &MBB,
                                              MachineIRBuilder &MIB,
                                              GISelObserverWrapper &Observer) {

  const MachineFrameInfo &MFI = MBB.getParent()->getFrameInfo();
  std::vector<MachineInstr *> FIs;

  for (MachineInstr &FIInstr : MBB) {
    // Only consider G_FRAME_INDEX
    if (FIInstr.getOpcode() != TargetOpcode::G_FRAME_INDEX)
      continue;

    const int FrameIdx = FIInstr.getOperand(1).getIndex();
    if (!MFI.isFixedObjectIndex(FrameIdx))
      continue;

    FIs.push_back(&FIInstr);
  }

  bool Changed = false;
  if (FIs.size() < 2)
    return Changed;

  const MachineInstr *FirstMI = FIs[0];
  const int64_t FirstOffset =
      MFI.getObjectOffset(FirstMI->getOperand(1).getIndex());
  const Register FirstPtr = FirstMI->getOperand(0).getReg();

  for (MachineInstr *FI : make_range(next(FIs.begin()), FIs.end())) {
    const int64_t Offset =
        MFI.getObjectOffset(FI->getOperand(1).getIndex()) - FirstOffset;
    MIB.setInstrAndDebugLoc(*FI);
    const Register NewOffsetReg =
        MIB.buildConstant(LLT::scalar(20), Offset).getReg(0);

    Observer.createdInstr(*MIB.buildInstr(TargetOpcode::G_PTR_ADD,
                                          {FI->getOperand(0).getReg()},
                                          {FirstPtr, NewOffsetReg}));
    Observer.erasingInstr(*FI);
    FI->eraseFromParent();
    Changed = true;
  }

  return Changed;
}

AIEClusterBaseAddress::RegUseMap
AIEClusterBaseAddress::collectPtrUses(MachineBasicBlock &MBB) {
  // Initialize Load Instrs to avoid
  const std::set<MachineInstr *> LoadsToAvoid = getLoadsFeedingCondBranch(MBB);

  RegUseMap RegAndUses;
  for (MachineInstr &PtrAdd : MBB) {
    // Only consider G_PTR_ADDs
    if (PtrAdd.getOpcode() != TargetOpcode::G_PTR_ADD)
      continue;

    // If G_PTR_ADDs is used in a Load in LoadsToAvoid, ignore PtrAdd in
    // chain collection. An example could be a Load Instr that feeds a
    // conditional jump and increases the critical path because the Load
    // Instr is delayed because of chaining.
    if (!LoadsToAvoid.empty() && avoidPtrAdd(&PtrAdd, LoadsToAvoid))
      continue;

    RegAndUses[PtrAdd.getOperand(1).getReg()].push_back(&PtrAdd);
  }
  return RegAndUses;
}

bool AIEClusterBaseAddress::shouldSkipChaining(
    Register PtrReg, const SmallVector<MachineInstr *, 8> &Instrs,
    MachineBasicBlock &MBB) {

  // No chain possibility at all.
  if (Instrs.size() <= 1 || (!EnableChainsAcrossMultiBlocks &&
                             isRegUsedInSuccessiveMBBs(&MBB, PtrReg)))
    return true;

  return false;
}

bool AIEClusterBaseAddress::buildChain(SmallVector<MachineInstr *, 8> &Instrs,
                                       MachineBasicBlock &MBB,
                                       MachineIRBuilder &MIB,
                                       GISelObserverWrapper &Observer) {
  bool Changed = false;
  int64_t AccumulatedOffset = 0;
  for (unsigned I = 0; I < Instrs.size() - 1; I++) {
    MachineInstr *MI = Instrs[I];
    MachineInstr *MINext = Instrs[I + 1];
    std::optional<int64_t> OffsetMI = getConstOffset(*MI, *MRI);
    std::optional<int64_t> OffsetMINext = getConstOffset(*MINext, *MRI);

    // Evaluate if we should restart the chain from the base
    // pointer. This is necessary when we deal with unknown offsets
    // (not constants) and desirable when we share pointers between
    // loads and stores (avoiding dependencies).
    if (shouldBreakChain(MI, MINext, OffsetMI, OffsetMINext)) {
      AccumulatedOffset = 0;
      continue;
    }

    AccumulatedOffset += *OffsetMI;
    const int64_t NewNextOffset = *OffsetMINext - AccumulatedOffset;
    MIB.setInsertPt(MBB, MINext->getIterator());

    Register NewOffsetReg =
        MIB.buildConstant(LLT::scalar(20), NewNextOffset).getReg(0);

    Observer.changingInstr(*MINext);
    MINext->getOperand(1).setReg(MI->getOperand(0).getReg());
    MINext->getOperand(2).setReg(NewOffsetReg);
    Observer.changedInstr(*MINext);
    Changed = true;
  }
  return Changed;
}

// True iff some non-head instruction in MBB still reads BaseReg. Rerooting
// walks BaseReg's physreg in place, destroying the live-in; a surviving
// non-head use would force the allocator to spill BaseReg around the chain.
// Out-of-MBB uses are fine — they see BaseReg's original SSA def.
static bool baseStillNeededInBlock(Register BaseReg, MachineBasicBlock &MBB,
                                   ArrayRef<MachineInstr *> Heads,
                                   const MachineRegisterInfo &MRI) {
  SmallPtrSet<const MachineInstr *, 4> HeadSet(Heads.begin(), Heads.end());
  return llvm::any_of(
      MRI.use_nodbg_instructions(BaseReg), [&](const MachineInstr &UseMI) {
        return UseMI.getParent() == &MBB && !HeadSet.contains(&UseMI);
      });
}

// True iff EndReg already has any use in MBB. A new chain rooted on it would
// walk EndReg in place between the existing user and the new heads, clobbering
// it. We forbid every in-block use rather than just memory users: the hazard
// is the in-place walk, which corrupts the value for any consumer.
static bool anchorStillUsedInBlock(Register EndReg, MachineBasicBlock &MBB,
                                   const MachineRegisterInfo &MRI) {
  for (const MachineInstr &UseMI : MRI.use_nodbg_instructions(EndReg))
    if (UseMI.getParent() == &MBB)
      return true;
  return false;
}

struct RerootRewrite {
  MachineInstr *Head;
  LLT OffTy;
  int64_t NewOffset;
};

// Validate every head and pre-compute its rewrite. Returns nullopt if any
// head has a non-constant or non-scalar offset, or if any new offset
// (HeadOffset - AnchorOffset) does not fit the head's offset width.
static std::optional<SmallVector<RerootRewrite, 4>>
computeRerootRewrites(ArrayRef<MachineInstr *> Heads, int64_t AnchorOffset,
                      const MachineRegisterInfo &MRI) {
  SmallVector<RerootRewrite, 4> Rewrites;
  Rewrites.reserve(Heads.size());
  for (MachineInstr *Head : Heads) {
    const std::optional<int64_t> HeadOffset = getConstOffset(*Head, MRI);
    const LLT OffTy = MRI.getType(Head->getOperand(2).getReg());
    if (!HeadOffset || !OffTy.isScalar())
      return std::nullopt;
    const int64_t NewOffset = *HeadOffset - AnchorOffset;
    if (!isIntN(OffTy.getSizeInBits(), NewOffset))
      return std::nullopt;
    Rewrites.push_back({Head, OffTy, NewOffset});
  }
  return Rewrites;
}

// Rewrite each head in place: re-root onto AnchorEndReg with the
// precomputed offset. Pure mutation; assumes the rewrites have already
// been validated by computeRerootRewrites.
static void applyRerootRewrites(ArrayRef<RerootRewrite> Rewrites,
                                Register AnchorEndReg, MachineBasicBlock &MBB,
                                MachineIRBuilder &MIB,
                                GISelObserverWrapper &Observer) {
  for (const RerootRewrite &R : Rewrites) {
    MIB.setInsertPt(MBB, R.Head->getIterator());
    Register NewOff = MIB.buildConstant(R.OffTy, R.NewOffset).getReg(0);
    Observer.changingInstr(*R.Head);
    R.Head->getOperand(1).setReg(AnchorEndReg);
    R.Head->getOperand(2).setReg(NewOff);
    Observer.changedInstr(*R.Head);
  }
}

bool AIEClusterBaseAddress::tryRerootBucket(Register BaseReg,
                                            ArrayRef<MachineInstr *> Heads,
                                            MachineBasicBlock &MBB,
                                            MachineIRBuilder &MIB,
                                            GISelObserverWrapper &Observer,
                                            PtrChainEndTracker &Tracker) {
  if (Heads.empty())
    return false;

  const std::optional<BlockEndOffset> Anchor =
      Tracker.findDominatingBlockEndOffset(BaseReg, MBB, *MRI);
  if (!Anchor)
    return false;

  if (baseStillNeededInBlock(BaseReg, MBB, Heads, *MRI))
    return false;

  // Distinct from the BaseReg check above: that one guards the chain's input;
  // this one guards the dominating chain's tail (a different vreg).
  if (anchorStillUsedInBlock(Anchor->EndReg, MBB, *MRI))
    return false;

  const std::optional<SmallVector<RerootRewrite, 4>> Rewrites =
      computeRerootRewrites(Heads, Anchor->Offset, *MRI);
  if (!Rewrites)
    return false;

  applyRerootRewrites(*Rewrites, Anchor->EndReg, MBB, MIB, Observer);
  return true;
}

bool AIEClusterBaseAddress::shouldBreakChain(MachineInstr *MIA,
                                             MachineInstr *MIB,
                                             std::optional<int64_t> OffsetA,
                                             std::optional<int64_t> OffsetB) {

  // If one of the offsets is not constant, it is better to break the chain.
  if (!OffsetA || !OffsetB)
    return true;

  return hasMixedLoadStoreUse({MIA, MIB});
}

bool AIEClusterBaseAddress::hasMixedLoadStoreUse(
    SmallVector<MachineInstr *, 2> Instrs) {
  unsigned LoadCount = 0;
  unsigned StoreCount = 0;
  for (MachineInstr *MI : Instrs) {
    const Register PtrReg = MI->getOperand(0).getReg();
    for (const MachineInstr &UseMI : MRI->use_instructions(PtrReg)) {
      if (!UseMI.mayLoadOrStore())
        continue;
      if (UseMI.mayLoad())
        LoadCount++;
      else
        StoreCount++;
      const LLT MemType = getLoadStoreType(UseMI);
      // If desired, we also can break the chain between pairs of
      // pointers that are used to load/store vectors and/or scalars.
      if ((!EnableChainsForScalarLdSt && MemType.isScalar()) ||
          (!EnableChainsForVectorLdSt && MemType.isVector()))
        return true;
    }
  }
  return (LoadCount > 0 && StoreCount > 0);
}

std::set<MachineBasicBlock *>
AIEClusterBaseAddress::findReachableMBBs(MachineBasicBlock *MBB) {
  std::set<MachineBasicBlock *> ReachableMBBs;
  SmallVector<MachineBasicBlock *, 8> Worklist;
  Worklist.append(MBB->succ_begin(), MBB->succ_end());
  while (!Worklist.empty()) {
    MachineBasicBlock *CurrMBB = Worklist.pop_back_val();
    if (!ReachableMBBs.insert(CurrMBB).second)
      continue;
    Worklist.append(CurrMBB->succ_begin(), CurrMBB->succ_end());
  }
  // Remove the starting MBB from the ReachableMBBs set since we don't want to
  // be too pessimistic as to not consider uses in the current basic block.
  ReachableMBBs.erase(MBB);
  return ReachableMBBs;
}

bool AIEClusterBaseAddress::isRegUsedInSuccessiveMBBs(MachineBasicBlock *MBB,
                                                      Register Reg) {
  std::set<MachineBasicBlock *> ReachableMBBs = findReachableMBBs(MBB);
  for (MachineInstr &Use : MRI->use_nodbg_instructions(Reg)) {
    if (ReachableMBBs.count(Use.getParent()))
      return true;
  }
  return false;
}
} // namespace

char AIEClusterBaseAddress::ID = 0;
INITIALIZE_PASS_BEGIN(AIEClusterBaseAddress, DEBUG_TYPE,
                      AIE_CLUSTER_BASE_ADDRESS, false, false)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_DEPENDENCY(MachineModuleInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_END(AIEClusterBaseAddress, DEBUG_TYPE, AIE_CLUSTER_BASE_ADDRESS,
                    false, false)

namespace llvm {
MachineFunctionPass *createAIEClusterBaseAddress() {
  return new AIEClusterBaseAddress();
}
} // namespace llvm
