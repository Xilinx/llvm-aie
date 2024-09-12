//===-- AIEEliminateDuplicatePHI.cpp - Identify and remove duplicate PHI --===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Identify and replace duplicate PHI nodes uses with the base PHI node.
// The pass specifically targets PHI nodes with pointer type operands.
//===----------------------------------------------------------------------===//

#include "AIEBaseSubtarget.h"
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

#define DEBUG_TYPE "aie-duplicate-PHI-elimination"

using namespace llvm;

static const char AIE_ELIMINATE_DUPLICATE_PHI[] =
    "AIE Eliminate Duplicate PHI Pass";

namespace {

/// Eliminates duplicate PHI uses by replacing all uses of DuplicateReg with
/// BaseReg.
///
/// This function iterates over all instructions that use DuplicateReg and
/// replaces any register operands that match DuplicateReg with BaseReg.
///
/// \param MRI The MachineRegisterInfo object for the current function.
/// \param Observer GISelObserverWrapper object to observe and track changes.
/// \param BaseReg The register to replace DuplicateReg with.
/// \param DuplicateReg The register to be replaced.
static void eliminateDuplicatePHIUses(MachineRegisterInfo &MRI,
                                      GISelObserverWrapper &Observer,
                                      Register BaseReg, Register DuplicateReg) {
  const auto UseInstIter = MRI.use_instructions(DuplicateReg);
  std::vector<MachineInstr *> UseInstr;
  // We cannot directly iterate on \var UseInstIter and modify the
  // instruction. Creating a std::vector allows us to iterate without
  // corrupting the iterator allowing us to modify the instructions.
  for (auto &Use : UseInstIter)
    UseInstr.push_back(&Use);
  // Iterate on all the uses and modify the register operand.
  for (auto &Use : UseInstr) {
    for (auto &MO : Use->uses()) {
      LLVM_DEBUG(dbgs() << "    Modifying Use: " << *Use);
      if (MO.isReg() && MO.getReg() == DuplicateReg) {
        Observer.changingInstr(*Use);
        MO.setReg(BaseReg);
        Observer.changedInstr(*Use);
      }
    }
  }
}

/// Returns true if the instruction is a PHI node with a pointer type operand.
/// \param MRI The MachineRegisterInfo object for the current function.
/// \param MI The instruction to check.
static bool isPointerTypePHI(MachineRegisterInfo &MRI, const MachineInstr &MI) {
  assert(MI.getOpcode() == TargetOpcode::G_PHI);
  const int DefaultAddrSpace = 0;
  const Register Reg = MI.getOperand(0).getReg();
  const LLT RegType = MRI.getType(Reg);
  return RegType == LLT::pointer(DefaultAddrSpace, RegType.getSizeInBits());
}

class AIEEliminateDuplicatePHI : public MachineFunctionPass {
public:
  static char ID;
  AIEEliminateDuplicatePHI() : MachineFunctionPass(ID) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    MachineRegisterInfo &MRI = MF.getRegInfo();
    TargetPassConfig &TPC = getAnalysis<TargetPassConfig>();
    // Enable CSE to set up GISelObserverWrapper object
    GISelCSEAnalysisWrapper &Wrapper =
        getAnalysis<GISelCSEAnalysisWrapperPass>().getCSEWrapper();
    auto *CSEInfo = &Wrapper.get(TPC.getCSEConfig());
    std::unique_ptr<MachineIRBuilder> Builder =
        CSEInfo ? std::make_unique<CSEMIRBuilder>()
                : std::make_unique<MachineIRBuilder>();
    Builder->setMF(MF);
    MachineIRBuilder &MIB = *Builder;
    // Set Observer to inform change of register type or change in op-code
    GISelObserverWrapper Observer;
    if (CSEInfo) {
      Observer.addObserver(CSEInfo);
      MIB.setChangeObserver(Observer);
    }

    bool Changed = false;
    for (MachineBasicBlock &MBB : MF) {
      Changed |= processMBB(MBB, MRI, MIB, Observer);
    }
    return Changed;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<MachineModuleInfoWrapperPass>();
    AU.addRequired<GISelCSEAnalysisWrapperPass>();
    AU.addRequired<TargetPassConfig>();
    AU.setPreservesAll();
  }

  StringRef getPassName() const override { return AIE_ELIMINATE_DUPLICATE_PHI; }

private:
  bool processMBB(MachineBasicBlock &MBB, MachineRegisterInfo &MRI,
                  MachineIRBuilder &MIB, GISelObserverWrapper &Observer) {
    bool Changed = false;
    auto FirstNonPHI = MBB.getFirstNonPHI();
    for (auto &MI : make_early_inc_range(MBB.phis())) {
      if (!isPointerTypePHI(MRI, MI))
        continue;
      for (auto &PHI : make_early_inc_range(
               make_range(++MachineBasicBlock::iterator(MI), FirstNonPHI))) {
        if (!isPointerTypePHI(MRI, PHI))
          continue;
        if (MI.isIdenticalTo(PHI, MachineInstr::IgnoreDefs)) {
          LLVM_DEBUG(dbgs() << "Identical PHI nodes found\n");
          LLVM_DEBUG(dbgs() << "  Base PHI: " << MI);
          LLVM_DEBUG(dbgs() << "  Duplicate PHI: " << PHI);
          eliminateDuplicatePHIUses(MRI, Observer, MI.getOperand(0).getReg(),
                                    PHI.getOperand(0).getReg());
          Changed = true;
        }
      }
    }
    return Changed;
  }
};
} // namespace

char AIEEliminateDuplicatePHI::ID = 0;
INITIALIZE_PASS_BEGIN(AIEEliminateDuplicatePHI, DEBUG_TYPE,
                      AIE_ELIMINATE_DUPLICATE_PHI, false, false)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_DEPENDENCY(MachineModuleInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_END(AIEEliminateDuplicatePHI, DEBUG_TYPE,
                    AIE_ELIMINATE_DUPLICATE_PHI, false, false)

namespace llvm {
MachineFunctionPass *createAIEEliminateDuplicatePHI() {
  return new AIEEliminateDuplicatePHI();
}
} // namespace llvm
