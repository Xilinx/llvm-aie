//===--------- AIE2PSSubtarget.cpp - AIE2ps Subtarget Information ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE2ps specific subclass of
// TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "AIE2PSSubtarget.h"
#include "AIE2PSLegalizerInfo.h"
#include "AIE2PSRegisterBankInfo.h"
#include "AIE2PSRegisterInfo.h"
#include "AIE2PSTargetMachine.h"
#include "AIECallLowering.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "aie2ps-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "AIE2PSGenSubtargetInfo.inc"

void AIE2PSSubtarget::anchor() {}

AIE2PSSubtarget &AIE2PSSubtarget::initializeSubtargetDependencies(
    const Triple &TT, StringRef CPU, StringRef FS, StringRef ABIName) {
  if (CPUName.empty())
    CPUName = "aie2ps";
  ParseSubtargetFeatures(CPUName, CPUName, FS);
  return *this;
}

AIE2PSSubtarget::AIE2PSSubtarget(const Triple &TT, StringRef CPU,
                                 StringRef TuneCPU, StringRef FS,
                                 StringRef ABIName, const TargetMachine &TM)
    : AIE2PSGenSubtargetInfo(TT, CPU, TuneCPU, FS),
      FrameLowering(initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrInfo(), RegInfo(getHwMode()),
      TLInfo(TM, initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrItins(getInstrItineraryForCPU(StringRef(CPU))) {
  LLVM_DEBUG(dbgs() << "CPU:" << CPU << "." << CPUName << "." << FS << "."
                    << ABIName << "\n");
  CallLoweringInfo.reset(new AIECallLowering(*getTargetLowering()));
  Legalizer.reset(new AIE2PSLegalizerInfo(*this));
  auto *RBI = new AIE2PSRegisterBankInfo(*getRegisterInfo());
  RegBankInfo.reset(RBI);
  InstSelector.reset(createAIE2PSInstructionSelector(
      *static_cast<const AIE2PSTargetMachine *>(&TM), *this, *RBI));
}

const CallLowering *AIE2PSSubtarget::getCallLowering() const {
  return CallLoweringInfo.get();
}

const LegalizerInfo *AIE2PSSubtarget::getLegalizerInfo() const {
  return Legalizer.get();
}

const RegisterBankInfo *AIE2PSSubtarget::getRegBankInfo() const {
  return RegBankInfo.get();
}

InstructionSelector *AIE2PSSubtarget::getInstructionSelector() const {
  return InstSelector.get();
}

const AIEBaseAddrSpaceInfo &AIE2PSSubtarget::getAddrSpaceInfo() const {
  return AddrSpaceInfo;
}
