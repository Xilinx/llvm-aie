//===-- AIESubtarget.cpp - AIE Subtarget Information ------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "AIESubtarget.h"
#include "AIE.h"
#include "AIE1RegisterBankInfo.h"
#include "AIECallLowering.h"
#include "AIEFrameLowering.h"
#include "AIELegalizerInfo.h"
#include "AIETargetMachine.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "aie-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "AIEGenSubtargetInfo.inc"

void AIESubtarget::anchor() {}

AIESubtarget &AIESubtarget::initializeSubtargetDependencies(
    const Triple &TT, StringRef CPU, StringRef FS, StringRef ABIName) {
  if (CPUName.empty())
      CPUName = "aie";
  ParseSubtargetFeatures(CPUName, CPUName, FS);

  return *this;
}

AIESubtarget::AIESubtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                           StringRef FS, StringRef ABIName,
                           const TargetMachine &TM)
    : AIEGenSubtargetInfo(TT, CPU, TuneCPU, FS), AIEBaseSubtarget(TT),
      FrameLowering(initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrInfo(), RegInfo(getHwMode()), TLInfo(TM, *this),
      InstrItins(getInstrItineraryForCPU(StringRef(CPU))) { // CPUName)) {
  LLVM_DEBUG(dbgs() << "CPU:" << CPU << "." << CPUName << "." << FS << "."
                    << ABIName << "\n");

  CallLoweringInfo.reset(new AIECallLowering(*getTargetLowering()));
  Legalizer.reset(new AIELegalizerInfo(*this));

  auto *RBI = new AIE1RegisterBankInfo(*getRegisterInfo());
  RegBankInfo.reset(RBI);
  InstSelector.reset(createAIEInstructionSelector(
      *static_cast<const AIETargetMachine *>(&TM), *this, *RBI));
}

const CallLowering *AIESubtarget::getCallLowering() const {
  return CallLoweringInfo.get();
}

const LegalizerInfo *AIESubtarget::getLegalizerInfo() const {
  return Legalizer.get();
}

const RegisterBankInfo *AIESubtarget::getRegBankInfo() const {
  return RegBankInfo.get();
}

InstructionSelector *AIESubtarget::getInstructionSelector() const {
  return InstSelector.get();
}
