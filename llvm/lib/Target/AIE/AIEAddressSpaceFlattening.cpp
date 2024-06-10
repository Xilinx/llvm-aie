//===----- AIEAddressSpaceFlattening.cpp - Retype pointer to P0 ----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Retype virtual register of pointer type to P0 type, to seemlessly connect
// with AIE backend passes. Since we retype the pointer to P0, any
// G_ADDRSPACE_CAST will have same i/p & o/p operand type which is not a valid
// instruction. Thus we to replace them with COPY.
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

#define DEBUG_TYPE "aie-addrspace-flattening"

using namespace llvm;

static const char AIE_ADDRSPACE_FLATTENING[] =
    "AIE Address Space Flattening Pass";

namespace {

class AIEAddressSpaceFlattening : public MachineFunctionPass {
public:
  static char ID;
  const int DefaultAddrSpace = 0;
  AIEAddressSpaceFlattening() : MachineFunctionPass(ID) {}

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
      for (MachineInstr &MI : MBB) {
        Changed = processMI(MI, MRI, MIB, Observer);
      }
    }
    return Changed;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<MachineModuleInfoWrapperPass>();
    AU.addRequired<GISelCSEAnalysisWrapperPass>();
    AU.addRequired<TargetPassConfig>();
    AU.setPreservesAll();
  }

  StringRef getPassName() const override { return AIE_ADDRSPACE_FLATTENING; }

private:
  bool processMI(MachineInstr &MI, MachineRegisterInfo &MRI,
                 MachineIRBuilder &MIB, GISelObserverWrapper &Observer) {
    bool Changed = false;
    // Retype all pointers to pointers with default address space
    for (llvm::MachineOperand &MO : MI.operands()) {
      if (!MO.isReg())
        continue;
      const Register Reg = MO.getReg();
      const LLT RegType = MRI.getType(Reg);
      if (RegType.isPointer()) {
        Observer.changingInstr(MI);
        MRI.setType(Reg,
                    LLT::pointer(DefaultAddrSpace, RegType.getSizeInBits()));
        Observer.changedInstr(MI);
        Changed = true;
      }
    }

    // Since we retype the pointer to P0, G_ADDRSPACE_CAST with same i/p & o/p
    // operand type is not a valid instruction. Thus we need to replace it with
    // COPY.
    if (MI.getOpcode() == TargetOpcode::G_ADDRSPACE_CAST) {
      Observer.changingInstr(MI);
      MI.setDesc(MIB.getTII().get(TargetOpcode::COPY));
      Observer.changedInstr(MI);
      Changed = true;
    }
    return Changed;
  }
};
} // namespace

char AIEAddressSpaceFlattening::ID = 0;
INITIALIZE_PASS_BEGIN(AIEAddressSpaceFlattening, DEBUG_TYPE,
                      AIE_ADDRSPACE_FLATTENING, false, false)
INITIALIZE_PASS_DEPENDENCY(GISelCSEAnalysisWrapperPass)
INITIALIZE_PASS_DEPENDENCY(MachineModuleInfoWrapperPass)
INITIALIZE_PASS_DEPENDENCY(TargetPassConfig)
INITIALIZE_PASS_END(AIEAddressSpaceFlattening, DEBUG_TYPE,
                    AIE_ADDRSPACE_FLATTENING, false, false)

namespace llvm {
MachineFunctionPass *createAIEAddressSpaceFlattening() {
  return new AIEAddressSpaceFlattening();
}
} // namespace llvm
