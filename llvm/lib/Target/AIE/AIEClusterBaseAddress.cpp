//===--- AIEClusterBaseAddress.cpp - Base Address Clustering --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
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
#include "AIEBaseInstrInfo.h"
#include "Utils/AIELoopUtils.h"
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/CSEMIRBuilder.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineLoopInfo.h"
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

cl::opt<int> AddressChainCostLimit(
    "aie-chain-cost-limit",
    cl::desc("Maximum allowed cost for pointer add chains"), cl::init(-1),
    cl::Hidden);

namespace {

/// Try and re-order PTR_ADD instructions to maximise the size of constant
/// PTR_ADD chains.
bool optimisePostIncrements(ArrayRef<MachineInstr *> PtrAdds,
                            const MachineRegisterInfo &MRI,
                            MachineIRBuilder &MIB,
                            GISelObserverWrapper &Observer) {
  bool Changed = false;

  // Look for the following sequence:
  // %0 = G_PTR_ADD %100, %101
  // %1 = G_PTR_ADD %100, 32
  // And swap the offsets if it is safe to get:
  // %0 = G_PTR_ADD %100, 32
  // %1 = G_PTR_ADD %100, %101
  for (MachineInstr *PtrAdd : PtrAdds) {
    assert(PtrAdd->getOpcode() == TargetOpcode::G_PTR_ADD);
    Register OutputPtr = PtrAdd->getOperand(0).getReg();
    Register OffsetReg = PtrAdd->getOperand(2).getReg();
    if (getIConstantVRegValWithLookThrough(OffsetReg, MRI) ||
        !MRI.hasOneNonDBGUser(OutputPtr))
      continue;

    // We found a non-constant PTRADD with a single user, now check if that
    // user is a constant PTRADD. If so, "swap" their offsets so that the
    // constant PTRADD appears first.
    MachineInstr &OutputUser = *MRI.use_instructions(OutputPtr).begin();
    if (OutputUser.getOpcode() != TargetOpcode::G_PTR_ADD)
      continue;
    Register SecondOffsetReg = OutputUser.getOperand(2).getReg();
    std::optional<ValueAndVReg> CstOffset =
        getIConstantVRegValWithLookThrough(SecondOffsetReg, MRI);
    if (!CstOffset)
      continue;

    // Everything fine, now swap the offsets
    LLVM_DEBUG(dbgs() << "Swapping offsets for:\n      " << *PtrAdd << "  and "
                      << OutputUser);
    Observer.changingInstr(*PtrAdd);
    MIB.setInstr(*PtrAdd);
    Register NewOffsetReg =
        MIB.buildConstant(LLT::scalar(20), CstOffset->Value.getSExtValue())
            .getReg(0);
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

  bool runOnMachineFunction(MachineFunction &MF) override {
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
    for (MachineBasicBlock &MBB : MF) {
      while (processBasicBlock(MBB, MIB, Observer))
        Changed = true;
    }
    return Changed;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<MachineModuleInfoWrapperPass>();
    AU.addRequired<GISelCSEAnalysisWrapperPass>();
    AU.addRequired<MachineLoopInfo>();
    AU.addPreserved<MachineLoopInfo>();
    AU.addRequired<TargetPassConfig>();
    AU.setPreservesAll();
  }

  StringRef getPassName() const override { return AIE_CLUSTER_BASE_ADDRESS; }

  using RegUseMap = std::map<Register, SmallVector<MachineInstr *, 8>>;

private:
  const MachineRegisterInfo *MRI = nullptr;

  bool processBasicBlock(MachineBasicBlock &MBB, MachineIRBuilder &MIB,
                         GISelObserverWrapper &Observer) {

    bool Changed = false;

    // Get all G_PTR_ADDs that use the same pointer.
    RegUseMap RegAndUses = collectPtrUses(MBB);

    // Optimise instruction order
    for (auto &RegAndUse : reverse(RegAndUses)) {
      ArrayRef<MachineInstr *> PtrAdds = RegAndUse.second;
      Changed |= optimisePostIncrements(PtrAdds, *MRI, MIB, Observer);
    }

    // Create chains, when profitable.
    for (auto RegAndUse : RegAndUses) {

      SmallVector<MachineInstr *, 8> &Instrs = RegAndUse.second;
      // Chaining acceptance criteria.
      if (shouldSkipChaining(RegAndUse.first, Instrs, MBB))
        continue;

      // Build chain, breaking it (or restarting it) when necessary
      Changed |= buildChain(Instrs, MBB, MIB, Observer);
    }
    return Changed;
  }

  // Get all candidates, i.e. groups of G_PTR_ADDs in the same
  // basic block that shares the same input pointer.
  RegUseMap collectPtrUses(MachineBasicBlock &MBB) {
    RegUseMap RegAndUses;
    for (MachineInstr &MI : MBB) {
      if (MI.getOpcode() == TargetOpcode::G_PTR_ADD)
        RegAndUses[MI.getOperand(1).getReg()].push_back(&MI);
    }
    return RegAndUses;
  }

  // Evaluate if we consider a group of G_PTR_ADDs as a candidate to
  // create a chain.
  bool shouldSkipChaining(Register PtrReg,
                          const SmallVector<MachineInstr *, 8> &Instrs,
                          MachineBasicBlock &MBB) {

    // No chain possibility at all.
    if (Instrs.size() <= 1)
      return true;

    // If the base reg is used in any of the successive MBBs, would introduce a
    // COPY and increase reg pressure. We only skip chaining in this case if it
    // is considered unprofitable.
    if (isRegUsedInSuccessiveMBBs(&MBB, PtrReg) &&
        !isChainingProfitable(PtrReg, Instrs, MBB))
      return true;

    return false;
  }

  // Decide heuristically if chaining will be profitable
  bool isChainingProfitable(Register PtrReg,
                            const SmallVector<MachineInstr *, 8> &Instrs,
                            MachineBasicBlock &MBB) {
    const TargetSubtargetInfo &ST = MBB.getParent()->getSubtarget();
    const AIEBaseInstrInfo *TII =
        static_cast<const AIEBaseInstrInfo *>(ST.getInstrInfo());
    using OffsetType = std::variant<int64_t, std::string>;
    assert(Instrs.size() > 1);

    bool InLoop = true;
    MachineLoopInfo &MLI = getAnalysis<MachineLoopInfo>();
    MachineLoop *ToLoop = MLI.getLoopFor(&MBB);
    if (!ToLoop)
      InLoop = false;

    unsigned ChainedCost = 0;
    unsigned ChainedCostLimit = Instrs.size() / 2; // Experimental threshold

    if (AddressChainCostLimit > -1) {
      ChainedCostLimit = AddressChainCostLimit;
    }

    if (isRegUsedInSuccessiveMBBs(&MBB, PtrReg)) {
      if (InLoop)
        return false;   // A copy in a loop is costly
      ChainedCost += 1; // Add cost of resulting copy
    }

    int64_t ImmediateRangeMax = 0;
    int64_t ImmediateRangeMin = 0;
    bool ImmediateRangeSet = false;
    int64_t AccumulatedOffset = 0;
    int64_t NewOffset;
    SmallVector<OffsetType, 8> Offsets;

    for (unsigned I = 0; I < Instrs.size() - 1; I++) {
      MachineInstr *MI = Instrs[I];
      MachineInstr *MINext = Instrs[I + 1];

      const Register PtrReg = MI->getOperand(0).getReg();
      for (const MachineInstr &UseMI : MRI->use_instructions(PtrReg)) {
        if (ImmediateRangeSet)
          continue; // Check first use only
        if (!UseMI.mayLoadOrStore())
          continue;
        const LLT MemType = getLoadStoreType(UseMI);
        // Immediate ranges for vectors are sufficient so we
        // assume chaining is always profitable.
        if (MemType.isVector()) {
          return true;
        } else {
          if (MemType.getSizeInBits() <= 32) {
            ImmediateRangeMax = TII->getLoadStorePostIncImmediateRange(MemType)
                                    .ImmediateRangeMax;
            ImmediateRangeMin = TII->getLoadStorePostIncImmediateRange(MemType)
                                    .ImmediateRangeMin;
            ImmediateRangeSet = true;
          } else {
            llvm_unreachable(
                "unreachable: Unsupported immediate range of scalar size ");
          }
        }
      }

      // If the immediate range is not set, the pointers aren't used by any
      // loads and stores, so we return.
      if (!ImmediateRangeSet) {
        assert(ImmediateRangeMin == 0 && ImmediateRangeMax == 0);
        return false;
      }

      auto OffsetMI =
          getIConstantVRegValWithLookThrough(MI->getOperand(2).getReg(), *MRI);
      auto OffsetMINext = getIConstantVRegValWithLookThrough(
          MINext->getOperand(2).getReg(), *MRI);

      if (shouldBreakChain(MI, MINext, OffsetMI, OffsetMINext)) {
        ChainedCost++;
        AccumulatedOffset = 0;
        Offsets.push_back("Break");
        continue;
      }

      const int64_t CurrOffset = OffsetMI->Value.getSExtValue();
      const int64_t NextOffset = OffsetMINext->Value.getSExtValue();

      assert(I == 0 || !Offsets.empty());
      AccumulatedOffset +=
          (I == 0 || (std::holds_alternative<std::string>(Offsets.back()) &&
                      std::get<std::string>(Offsets.back()) == "Break"))
              ? CurrOffset
              : NewOffset;
      Offsets.push_back(
          (I == 0 || (std::holds_alternative<std::string>(Offsets.back()) &&
                      std::get<std::string>(Offsets.back()) == "Break"))
              ? CurrOffset
              : OffsetType(NewOffset));

      NewOffset = NextOffset - AccumulatedOffset;

      if (NewOffset < ImmediateRangeMin || NewOffset > ImmediateRangeMax) {
        ChainedCost += 1; // Immediate materialization cost
      }
    }

    return ChainedCostLimit > ChainedCost;
  }

  // Build a chain (or set of chains) of G_PTR_ADDs. We consider as
  // chain a linear sequence of linked G_PTR_ADDs, tied to output and
  // input pointers.
  bool buildChain(SmallVector<MachineInstr *, 8> &Instrs,
                  MachineBasicBlock &MBB, MachineIRBuilder &MIB,
                  GISelObserverWrapper &Observer) {
    bool Changed = false;
    int64_t AccumulatedOffset = 0;
    for (unsigned I = 0; I < Instrs.size() - 1; I++) {
      MachineInstr *MI = Instrs[I];
      MachineInstr *MINext = Instrs[I + 1];
      auto OffsetMI =
          getIConstantVRegValWithLookThrough(MI->getOperand(2).getReg(), *MRI);
      auto OffsetMINext = getIConstantVRegValWithLookThrough(
          MINext->getOperand(2).getReg(), *MRI);

      // Evaluate if we should restart the chain from the base
      // pointer. This is necessary when we deal with unknown offsets
      // (not constants) and desirable when we share pointers between
      // loads and stores (avoiding dependencies).
      if (shouldBreakChain(MI, MINext, OffsetMI, OffsetMINext)) {
        AccumulatedOffset = 0;
        continue;
      }

      AccumulatedOffset += OffsetMI->Value.getSExtValue();
      const int64_t NewNextOffset =
          OffsetMINext->Value.getSExtValue() - AccumulatedOffset;
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

  // Evaluate if we should break the chain construction.
  // Criteria:
  //  * Unknown offsets.
  //  * Pointer shared between load(s) and store(s).
  bool shouldBreakChain(MachineInstr *MIA, MachineInstr *MIB,
                        std::optional<ValueAndVReg> OffsetA,
                        std::optional<ValueAndVReg> OffsetB) {

    // If one of the offsets is not constant, it is better to break the chain.
    if (!OffsetA || !OffsetB)
      return true;

    return hasMixedLoadStoreUse({MIA, MIB});
  }

  // Return true if the instructions are used by both loads and stores.
  bool hasMixedLoadStoreUse(SmallVector<MachineInstr *, 2> Instrs) {
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

  LLT getLoadStoreType(const MachineInstr &MI) {
    return (*MI.memoperands_begin())->getMemoryType();
  }

  // Get a set of all reachable MBBs from a given MBB.
  // Loops are handled using the ReachableMBBs set, once we encounter any
  // reachable MBB from a particular MBB, we store it in the set and continue if
  // we find it again. Lastly, we remove the current MBB from the set in case it
  // comes up in the successive basic blocks.
  std::set<MachineBasicBlock *> findReachableMBBs(MachineBasicBlock *MBB) {
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

  // Find if a register is used in reachable MBBs.
  bool isRegUsedInSuccessiveMBBs(MachineBasicBlock *MBB, Register Reg) {
    std::set<MachineBasicBlock *> ReachableMBBs = findReachableMBBs(MBB);
    for (MachineInstr &Use : MRI->use_nodbg_instructions(Reg)) {
      if (ReachableMBBs.count(Use.getParent()))
        return true;
    }
    return false;
  }
};
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
