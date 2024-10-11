//===-- AIERegClassConstrainer.cpp - Constrain Reg classes before RA ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/LiveIntervals.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <map>

#include "AIE.h"

using namespace llvm;

#define DEBUG_TYPE "aie-ra"

namespace {

/// Constrain partially allocatable classes to fully allocatable ones.
/// This makes sure that greedy works with classes for which it is easy to
/// generate spill code.
class AIERegClassConstrainer : public MachineFunctionPass {

public:
  static char ID;
  AIERegClassConstrainer() : MachineFunctionPass(ID) {}

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
    AU.addPreserved<SlotIndexes>();
    AU.addPreserved<LiveIntervals>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &Fn) override;

private:
  /// Find the largest subclass of \p RC that is suitable for register
  /// allocation. That is, it does not contain reserved registers, as those
  /// are harder to spill/reload.
  const TargetRegisterClass &getSuitableRCForRA(const TargetRegisterClass &RC,
                                                const MachineRegisterInfo &MRI);

  /// Make sure the cached RC mapping isn't invalidated by \p MF
  void updateSuitableRCCacheForFunction(const MachineFunction &MF);

  /// Cache for \p getSuitableRCForRA
  std::map<unsigned, const TargetRegisterClass *> SuitableRCForRA;

  /// Reserved registers for which \p SuitableRCForRA was computed.
  BitVector KnownReservedRegs;
};

bool AIERegClassConstrainer::runOnMachineFunction(MachineFunction &MF) {
  LLVM_DEBUG(llvm::dbgs() << "Constraining RCs for RA: " << MF.getName()
                          << "\n");
  MachineRegisterInfo &MRI = MF.getRegInfo();
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();

  // Check if the existing cache was invalidated: reserved registers can
  // change for each function.
  updateSuitableRCCacheForFunction(MF);

  for (unsigned I = 0, E = MRI.getNumVirtRegs(); I != E; ++I) {
    Register Reg = Register::index2VirtReg(I);
    if (MRI.reg_nodbg_empty(Reg))
      continue;

    const TargetRegisterClass &SrcRC = *MRI.getRegClass(Reg);
    const TargetRegisterClass &SuitableRC = getSuitableRCForRA(SrcRC, MRI);
    if (SrcRC.getID() == SuitableRC.getID())
      continue;

    LLVM_DEBUG(llvm::dbgs() << "  Constraining " << printReg(Reg, &TRI)
                            << " from " << TRI.getRegClassName(&SrcRC) << " to "
                            << TRI.getRegClassName(&SuitableRC) << "\n");
    MRI.setRegClass(Reg, &SuitableRC);
  }

  // We are only changing register classes, so it's probably okay to return
  // false. But let's be conservative, the return value of runOnMachineFunction
  // isn't very well documented.
  return true;
}

const TargetRegisterClass &
getSuitableRCForRAImpl(const TargetRegisterClass &RC,
                       const MachineRegisterInfo &MRI) {
  const TargetRegisterInfo &TRI = *MRI.getTargetRegisterInfo();

  auto ContainsReservedReg = [&MRI](const TargetRegisterClass &RC) {
    return any_of(RC, [&MRI](MCRegister Reg) { return MRI.isReserved(Reg); });
  };

  if (!ContainsReservedReg(RC))
    return RC;

  // Iterate over all sub-classes to find the largest one not containing
  // reserved registers.
  const TargetRegisterClass *SuitableRC = nullptr;
  for (BitMaskClassIterator It(RC.getSubClassMask(), TRI); It.isValid(); ++It) {
    const TargetRegisterClass *SubRC = TRI.getRegClass(It.getID());
    if (ContainsReservedReg(*SubRC))
      continue;
    if (!SuitableRC || SuitableRC->getNumRegs() < SubRC->getNumRegs())
      SuitableRC = SubRC;
  }

  assert(SuitableRC && "No valid sub-class without reserved register");
  return *SuitableRC;
}

const TargetRegisterClass &
AIERegClassConstrainer::getSuitableRCForRA(const TargetRegisterClass &RC,
                                           const MachineRegisterInfo &MRI) {

  if (auto RCIt = SuitableRCForRA.find(RC.getID());
      RCIt != SuitableRCForRA.end()) {
    return *RCIt->second;
  }

  const TargetRegisterClass &SuitableRC = getSuitableRCForRAImpl(RC, MRI);
  SuitableRCForRA.emplace(RC.getID(), &SuitableRC);
  return SuitableRC;
}

void AIERegClassConstrainer::updateSuitableRCCacheForFunction(
    const MachineFunction &MF) {
  const MachineRegisterInfo &MRI = MF.getRegInfo();
  if (MRI.getReservedRegs() == KnownReservedRegs)
    return;

  // Reserved registers have changed, clear the cached RC mapping.
  KnownReservedRegs = MRI.getReservedRegs();
  SuitableRCForRA.clear();
}

} // end anonymous namespace

char AIERegClassConstrainer::ID = 0;
char &llvm::AIERegClassConstrainerID = AIERegClassConstrainer::ID;

INITIALIZE_PASS(AIERegClassConstrainer, "aie-regclass-constrainer",
                "Constrain partially-allocatable classes", false, false)

llvm::FunctionPass *llvm::createAIERegClassConstrainer() {
  return new AIERegClassConstrainer();
}
