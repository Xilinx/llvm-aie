//===-- AIE2Subtarget.cpp - AIEngine V2 Subtarget Information-------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIEngine V2 specific subclass of
// TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "AIE2Subtarget.h"
#include "AIE2.h"
#include "AIE2FrameLowering.h"
#include "AIE2LegalizerInfo.h"
#include "AIE2RegisterBankInfo.h"
#include "AIE2TargetMachine.h"
#include "AIECallLowering.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "aie2-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "AIE2GenSubtargetInfo.inc"

void AIE2Subtarget::anchor() {}

AIE2Subtarget &AIE2Subtarget::initializeSubtargetDependencies(
    const Triple &TT, StringRef CPU, StringRef FS, StringRef ABIName) {
  if (CPUName.empty())
    CPUName = "aie2";
  ParseSubtargetFeatures(CPUName, CPUName, FS);
  return *this;
}

AIE2Subtarget::AIE2Subtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                             StringRef FS, StringRef ABIName,
                             const TargetMachine &TM)
    : AIE2GenSubtargetInfo(TT, CPU, TuneCPU, FS), AIEBaseSubtarget(TT),
      FrameLowering(initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrInfo(), RegInfo(getHwMode()),
      TLInfo(TM, initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrItins(getInstrItineraryForCPU(StringRef(CPU))) {
  LLVM_DEBUG(dbgs() << "CPU:" << CPU << "." << CPUName << "." << FS << "."
                    << ABIName << "\n");

  CallLoweringInfo.reset(new AIECallLowering(*getTargetLowering()));
  Legalizer.reset(new AIE2LegalizerInfo(*this));
  auto *RBI = new AIE2RegisterBankInfo(*getRegisterInfo());
  RegBankInfo.reset(RBI);
  InstSelector.reset(createAIE2InstructionSelector(
      *static_cast<const AIE2TargetMachine *>(&TM), *this, *RBI));
}

const CallLowering *AIE2Subtarget::getCallLowering() const {
  return CallLoweringInfo.get();
}

const LegalizerInfo *AIE2Subtarget::getLegalizerInfo() const {
  return Legalizer.get();
}

const RegisterBankInfo *AIE2Subtarget::getRegBankInfo() const {
  return RegBankInfo.get();
}

InstructionSelector *AIE2Subtarget::getInstructionSelector() const {
  return InstSelector.get();
}

MemoryBankBits AIE2Subtarget::getDefaultMemoryBank() const {
  using namespace AIE2;
  std::bitset<32> MemoryBanks;
  MemoryBanks.set(static_cast<unsigned>(AIEBanks::A))
      .set(static_cast<unsigned>(AIEBanks::B))
      .set(static_cast<unsigned>(AIEBanks::C))
      .set(static_cast<unsigned>(AIEBanks::D));
  return MemoryBanks.to_ulong();
}

MemoryBankBits
AIE2Subtarget::getMemoryBanksFromAddressSpace(unsigned AddrSpace) const {
  using namespace AIE2;
  std::bitset<32> MemoryBanks;

  switch (static_cast<AddressSpaces>(AddrSpace)) {
  case AddressSpaces::a:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::A));
    break;
  case AddressSpaces::b:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::B));
    break;
  case AddressSpaces::c:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::C));
    break;
  case AddressSpaces::d:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::D));
    break;
  case AddressSpaces::ab:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::A))
        .set(static_cast<unsigned>(AIEBanks::B));
    break;
  case AddressSpaces::ac:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::A))
        .set(static_cast<unsigned>(AIEBanks::C));
    break;
  case AddressSpaces::ad:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::A))
        .set(static_cast<unsigned>(AIEBanks::D));
    break;
  case AddressSpaces::bc:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::B))
        .set(static_cast<unsigned>(AIEBanks::C));
    break;
  case AddressSpaces::bd:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::B))
        .set(static_cast<unsigned>(AIEBanks::D));
    break;
  case AddressSpaces::cd:
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::C))
        .set(static_cast<unsigned>(AIEBanks::D));
    break;
  default:
    return getDefaultMemoryBank();
    break;
  }

  return MemoryBanks.to_ulong();
}
