//===--------- AIE2PSubtarget.cpp - AIE2p Subtarget Information-----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE2p specific subclass of
// TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "AIE2PSubtarget.h"
#include "AIE2PLegalizerInfo.h"
#include "AIE2PRegisterBankInfo.h"
#include "AIE2PRegisterInfo.h"
#include "AIE2PTargetMachine.h"
#include "AIECallLowering.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "aie2p-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "AIE2PGenSubtargetInfo.inc"

void AIE2PSubtarget::anchor() {}

AIE2PSubtarget &AIE2PSubtarget::initializeSubtargetDependencies(
    const Triple &TT, StringRef CPU, StringRef FS, StringRef ABIName) {
  if (CPUName.empty())
    CPUName = "aie2p";
  ParseSubtargetFeatures(CPUName, CPUName, FS);
  return *this;
}

AIE2PSubtarget::AIE2PSubtarget(const Triple &TT, StringRef CPU,
                               StringRef TuneCPU, StringRef FS,
                               StringRef ABIName, const TargetMachine &TM)
    : AIE2PGenSubtargetInfo(TT, CPU, TuneCPU, FS), AIEBaseSubtarget(TT),
      FrameLowering(initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrInfo(), RegInfo(getHwMode()),
      TLInfo(TM, initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrItins(getInstrItineraryForCPU(StringRef(CPU))) {
  LLVM_DEBUG(dbgs() << "CPU:" << CPU << "." << CPUName << "." << FS << "."
                    << ABIName << "\n");
  CallLoweringInfo.reset(new AIECallLowering(*getTargetLowering()));
  Legalizer.reset(new AIE2PLegalizerInfo(*this));
  auto *RBI = new AIE2PRegisterBankInfo(*getRegisterInfo());
  RegBankInfo.reset(RBI);
  InstSelector.reset(createAIE2PInstructionSelector(
      *static_cast<const AIE2PTargetMachine *>(&TM), *this, *RBI));
}

const CallLowering *AIE2PSubtarget::getCallLowering() const {
  return CallLoweringInfo.get();
}

const LegalizerInfo *AIE2PSubtarget::getLegalizerInfo() const {
  return Legalizer.get();
}

const RegisterBankInfo *AIE2PSubtarget::getRegBankInfo() const {
  return RegBankInfo.get();
}

InstructionSelector *AIE2PSubtarget::getInstructionSelector() const {
  return InstSelector.get();
}

const AIEBaseAddrSpaceInfo &AIE2PSubtarget::getAddrSpaceInfo() const {
  return AddrSpaceInfo;
}
