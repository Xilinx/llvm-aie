//===--- AIEClusterBaseAddress.cpp - Base Address Clustering --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
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

#include "AIEBaseSubtarget.h"
#include <optional>
#include <set>
#include "llvm/CodeGen/GlobalISel/CSEInfo.h"
#include "llvm/CodeGen/GlobalISel/CSEMIRBuilder.h"
#include "llvm/CodeGen/GlobalISel/GenericMachineInstrs.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/InitializePasses.h"

#define DEBUG_TYPE "aie-cluster-base-address"

using namespace llvm;

static const char AIE_CLUSTER_BASE_ADDRESS[] =
    "AIE Base Address Clustering Optimization";

static cl::opt<bool> EnableAddressChaining("aie-address-chaining", cl::Hidden,
                                           cl::init(true),
                                           cl::desc("Enable ptradd chaining."));

static cl::opt<bool> EnableChainsForScalarLdSt(
    "aie-chain-addr-scl-ldst", cl::Hidden, cl::init(true),
    cl::desc("Enable ptradd chaining for scalar loads and stores."));

static cl::opt<bool> EnableChainsForVectorLdSt(
    "aie-chain-addr-vec-ldst", cl::Hidden, cl::init(true),
    cl::desc("Enable ptradd chaining for vector loads and stores."));

namespace {
/**
 * @brief Struct PtrAdd
 *
 * @param PtrAddMI The next ptr add to a load/store MI that has the potential to
 * be chained.
 * @param BaseReg The base ptr of the load/store that is found by traversing the
 * ptr adds backwards. This is also the new operand of the next ptr add.
 * @param NewOffset This is the new offset for the next ptr add to be chained,
 * calculated using the offset information of the previous ptr adds.
 */
struct PtrAdd {
  MachineInstr *PtrAddMI;
  Register BaseReg;
  int64_t NewOffset;
};

class AIEClusterBaseAddress : public MachineFunctionPass {
public:
  static char ID;
  AIEClusterBaseAddress() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    if (!EnableAddressChaining)
      return false;
    MachineRegisterInfo &MRI = MF.getRegInfo();
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
      Changed |= processBasicBlock(MBB, MRI, MIB, Observer);
    }
    return Changed;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<MachineModuleInfoWrapperPass>();
    AU.addRequired<GISelCSEAnalysisWrapperPass>();
    AU.addRequired<TargetPassConfig>();
    AU.setPreservesAll();
  }

  StringRef getPassName() const override { return AIE_CLUSTER_BASE_ADDRESS; }

private:
  bool processBasicBlock(MachineBasicBlock &MBB, MachineRegisterInfo &MRI,
                         MachineIRBuilder &MIB,
                         GISelObserverWrapper &Observer) {
    /* Pass 1 :
      Traverse MBB to try and connect G_LOAD or G_STORE to following PTR_ADDs.
      This also checks if any MI uses the base ptr and if it does, we simply
      remove the entire entry from the ChainedPtrAdds map. We avoid combining if
      the base register of the to-be-chained ptradd is used by another instr.
      This would otherwise generate a COPY and increase reg pressure.
      Use a map to store the Load/Store MI and the next ptr add with the base
      ptr of the Ld/St MI and the new updated offset.
    */
    std::map<MachineInstr *, PtrAdd> ChainedPtrAdds =
        findChainablePtrAdds(MBB, MRI);

    // Return false if ChainedPtrAdds is empty since we have no ptradds to
    // update.
    if (ChainedPtrAdds.empty())
      return false;

    /* Pass 2 :
      Simply update the chainable ptradds in the MBB using the information in
      the ChainedPtrAdds map.
    */
    updatePtrAddsInMBB(MBB, MRI, MIB, Observer, ChainedPtrAdds);

    return true;
  }

  std::map<MachineInstr *, PtrAdd>
  findChainablePtrAdds(MachineBasicBlock &MBB, MachineRegisterInfo &MRI) {
    std::map<MachineInstr *, PtrAdd> ChainedPtrAdds;
    for (MachineInstr &MI : MBB) {
      if (dyn_cast<GLoadStore>(&MI)) {
        processLoadOrStore(MI, MRI, ChainedPtrAdds);
        continue;
      }

      // Check if any base pointer in `ChainedPtrAdds` is used by MI. In this
      // case, we do not want to chain the addresses, because this would
      // introduce a COPY that increases the pressure on PTR registers.

      // If MI is a ptradd already chained with a previous load or store,
      // this is a safe use.
      if (any_of(ChainedPtrAdds,
                 [&](std::pair<MachineInstr *const, PtrAdd> &LdStAndPtrAdd) {
                   return LdStAndPtrAdd.second.PtrAddMI == &MI;
                 }))
        continue;

      // Otherwise, remove all chained load/stores that use one of our operands
      // as base pointer.
      std::set<MachineInstr *> ToBeErased;
      for (const MachineOperand &Op : MI.operands()) {
        if (Op.isReg() && Op.isUse())
          removeChainedInstrsWithBasePtr(ChainedPtrAdds, Op.getReg());
      }
    }
    return ChainedPtrAdds;
  }

  // Process all the encountered load and stores in the basic block.
  // We find the base register of the Ld/St MI and using that base register, we
  // find following ptr adds which have the same base register. If we find such
  // a ptr add, we create an entry in the ChainedPtrAdds map.
  void processLoadOrStore(MachineInstr &MI, MachineRegisterInfo &MRI,
                          std::map<MachineInstr *, PtrAdd> &ChainedPtrAdds) {
    Register LdOrStPtrReg = MI.getOperand(1).getReg();
    // Find the base defining reg of the given MI and the ptr offset.
    auto BaseRegAndOffset = findBaseReg(MRI, LdOrStPtrReg);
    if (!BaseRegAndOffset.has_value())
      return;
    if ((!EnableChainsForScalarLdSt && getLoadStoreSize(MI) <= 32) ||
        (!EnableChainsForVectorLdSt && getLoadStoreSize(MI) >= 256))
      return;
    Register BaseReg = BaseRegAndOffset->first;
    // If the base reg is used in any of the successive MBBs, then we don't want
    // to chain the corresponding ptr adds. Since that would introduce a COPY
    // and increase reg pressure.
    if (isRegUsedInSuccessiveMBBs(MI.getParent(), BaseReg, MRI))
      return;
    int64_t BasePtrOffset = BaseRegAndOffset->second;
    // Find the next G_PTR_ADD MachineInstr that comes after the given
    // MachineInstr and has the same base register.
    MachineInstr *NextPtrAddMI = findNextPtrAddForReg(MI, MRI, BaseReg);
    if (!NextPtrAddMI)
      return;
    auto [OffsetReg, ConstVal] = getPtrAddOffsetInfo(*NextPtrAddMI, MRI);
    if (!ConstVal)
      return;
    int64_t NextPtrOffset = ConstVal->Value.getSExtValue();
    PtrAdd PA;
    PA.PtrAddMI = NextPtrAddMI;
    PA.BaseReg = BaseReg;
    PA.NewOffset = NextPtrOffset - BasePtrOffset;
    ChainedPtrAdds[&MI] = PA;
  }

  unsigned getLoadStoreSize(const MachineInstr &MI) {
    return (*MI.memoperands_begin())->getSizeInBits();
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
  bool isRegUsedInSuccessiveMBBs(MachineBasicBlock *MBB, Register Reg,
                                 MachineRegisterInfo &MRI) {
    std::set<MachineBasicBlock *> ReachableMBBs = findReachableMBBs(MBB);
    for (MachineInstr &Use : MRI.use_nodbg_instructions(Reg)) {
      if (ReachableMBBs.count(Use.getParent()))
        return true;
    }
    return false;
  }

  void removeChainedInstrsWithBasePtr(
      std::map<MachineInstr *, PtrAdd> &ChainedPtrAdds, unsigned BaseReg) {
    std::set<MachineInstr *> ToBeErased;
    for (auto &[LdStMI, ChainedPtrAdd] : ChainedPtrAdds) {
      if (ChainedPtrAdd.BaseReg == BaseReg)
        ToBeErased.insert(LdStMI);
    }
    for (auto &LdStMI : ToBeErased) {
      ChainedPtrAdds.erase(LdStMI);
    }
  }

  void
  updatePtrAddsInMBB(MachineBasicBlock &MBB, MachineRegisterInfo &MRI,
                     MachineIRBuilder &MIB, GISelObserverWrapper &Observer,
                     const std::map<MachineInstr *, PtrAdd> &ChainedPtrAdds) {
    for (auto &MI : MBB) {
      auto Entry = ChainedPtrAdds.find(&MI);
      if (Entry == ChainedPtrAdds.end())
        continue;
      MachineInstr *LdOrStMI = Entry->first;
      Register LdOrStPtrReg = LdOrStMI->getOperand(1).getReg();
      const PtrAdd &ChainedPtrAdd = Entry->second;
      MachineInstr *PtrAddMI = ChainedPtrAdd.PtrAddMI;
      int64_t ChainedOffset = ChainedPtrAdd.NewOffset;

      MIB.setInsertPt(*PtrAddMI->getParent(), PtrAddMI->getIterator());
      // Change the ptr register operand of PtrAddMI to be the ptr reg operand
      // of the load/store
      Observer.changingInstr(*PtrAddMI);
      PtrAddMI->getOperand(1).setReg(LdOrStPtrReg);
      Observer.changedInstr(*PtrAddMI);
      // Build a new G_CONSTANT MachineInstr with NewOffset as its value
      // If there is a G_CONSTANT present which don't have any further uses
      // other than a given ptr add, then it would just be eliminated as dead
      // code.
      Register NewOffsetReg =
          MIB.buildConstant(LLT::scalar(20), ChainedOffset).getReg(0);
      // Change the offset register operand of PtrAddMI to be NewOffsetReg
      Observer.changingInstr(*PtrAddMI);
      PtrAddMI->getOperand(2).setReg(NewOffsetReg);
      Observer.changedInstr(*PtrAddMI);
    }
  }

  std::optional<std::pair<Register, int64_t>>
  findBaseReg(MachineRegisterInfo &MRI, const Register Reg) {
    Register BaseReg = Reg;
    int64_t Offset = 0;
    while (true) {
      // Get the defining instruction for the register
      MachineInstr *DefMI = MRI.getVRegDef(BaseReg);
      if (!DefMI || DefMI->getOpcode() != TargetOpcode::G_PTR_ADD)
        break;
      auto [OffsetReg, ConstVal] = getPtrAddOffsetInfo(*DefMI, MRI);
      // TODO: Handle ptr adds with indirect constant offsets as needed.
      if (!ConstVal)
        break;
      Offset += ConstVal->Value.getSExtValue();
      BaseReg = DefMI->getOperand(1).getReg();
    }
    return std::make_optional(std::make_pair(BaseReg, Offset));
  }

  // Find next ptr add having the same base register.
  MachineInstr *findNextPtrAddForReg(MachineInstr &Start,
                                     MachineRegisterInfo &MRI,
                                     const Register BaseReg) {
    MachineBasicBlock *MBB = Start.getParent();
    MachineBasicBlock::iterator It = std::next(Start.getIterator()),
                                End = MBB->end();
    auto FoundIt = std::find_if(It, End, [&](MachineInstr &MI) {
      if (MI.getOpcode() == TargetOpcode::G_PTR_ADD &&
          MI.getOperand(1).getReg() == BaseReg)
        return true;
      // We search for GLoadStore because we always want to stick to the
      // immediately preceding GLoadStore to chain the ptr add.
      if (dyn_cast<GLoadStore>(&MI)) {
        Register LdOrStPtrReg = MI.getOperand(1).getReg();
        auto BaseRegAndOffset = findBaseReg(MRI, LdOrStPtrReg);
        if (!BaseRegAndOffset.has_value())
          return true;
        Register BasePtr = BaseRegAndOffset->first;
        if (BasePtr == BaseReg)
          return true;
      }
      return false;
    });
    if (FoundIt != End && FoundIt->getOpcode() == TargetOpcode::G_PTR_ADD)
      return &*FoundIt;
    return nullptr;
  }

  std::pair<Register, std::optional<ValueAndVReg>>
  getPtrAddOffsetInfo(const MachineInstr &MI, MachineRegisterInfo &MRI) {
    assert(MI.getOpcode() == TargetOpcode::G_PTR_ADD &&
           "Expected a ptr add MI");
    Register OffsetReg = MI.getOperand(2).getReg();
    std::optional<ValueAndVReg> ConstVal =
        getIConstantVRegValWithLookThrough(OffsetReg, MRI);
    return {OffsetReg, ConstVal};
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
