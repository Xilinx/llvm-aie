//===-- AIESubtarget.cpp - AIE Subtarget Information ------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIE specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "AIE1Subtarget.h"
#include "AIE.h"
#include "AIE1FrameLowering.h"
#include "AIE1LegalizerInfo.h"
#include "AIE1RegisterBankInfo.h"
#include "AIE1TargetMachine.h"
#include "AIECallLowering.h"
#include "llvm/CodeGen/ScheduleDAG.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "aie-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "AIEGenSubtargetInfo.inc"

void AIESubtarget::anchor() {}

AIESubtarget &AIESubtarget::initializeSubtargetDependencies(const Triple &TT,
                                                            StringRef CPU,
                                                            StringRef FS,
                                                            StringRef ABIName) {
  if (CPUName.empty())
    CPUName = "aie";
  ParseSubtargetFeatures(CPUName, CPUName, FS);

  return *this;
}

AIESubtarget::AIESubtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                           StringRef FS, StringRef ABIName,
                           const TargetMachine &TM)
    : AIEGenSubtargetInfo(TT, CPU, TuneCPU, FS),
      FrameLowering(initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrInfo(*this), RegInfo(getHwMode()), TLInfo(TM, *this),
      InstrItins(getInstrItineraryForCPU(StringRef(CPU))) {
  LLVM_DEBUG(dbgs() << "CPU:" << CPU << "." << CPUName << "." << FS << "."
                    << ABIName << "\n");

  CallLoweringInfo.reset(new AIECallLowering(*getTargetLowering()));
  Legalizer.reset(new AIE1LegalizerInfo(*this));

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

const AIEBaseAddrSpaceInfo &AIESubtarget::getAddrSpaceInfo() const {
  return AddrSpaceInfo;
}

// Perform target-specific adjustments to the latency of a schedule
// dependency.
// If a pair of operands is associated with the schedule dependency, DefIdx
// and UseIdx are the indices of the operands in Def and Use, respectively.
// Otherwise, either may be -1.
//
// Reminder: this is called for ALL dependencies carried by physical registers,
// but only for DATA dependencies on virtual registers...
void AIESubtarget::adjustSchedDependency(
    SUnit *Def, int DefIdx, SUnit *Use, int UseIdx, SDep &Dep,
    const TargetSchedModel *SchedModel) const {

  // Ignore artificial and SDNode-based (SelectionDAG) dependencies.
  // Those are not needed for correctness.
  if (Dep.isArtificial() || !Def->isInstr() || !Use->isInstr()) {
    return;
  }

  // Note: Def is a misnomer, it is the source of the edge, but it isn't
  // necessarily a Def in case of WAW or WAR dependencies. Similar for Use.
  assert(DefIdx >= 0 && UseIdx >= 0);
  MachineInstr &SrcMI = *Def->getInstr();
  auto SrcOpIdx = unsigned(DefIdx);
  MachineInstr &DstMI = *Use->getInstr();
  auto DstOpIdx = unsigned(UseIdx);

  // We cannot use itineraries for implicit operands that were added
  // "dynamically", i.e. those that are not part of the static MCInstrDesc.
  auto NumStaticOps = [](const MCInstrDesc &D) -> unsigned {
    return D.NumOperands + D.NumImplicitDefs + D.NumImplicitUses;
  };
  if (SrcOpIdx >= NumStaticOps(SrcMI.getDesc()) ||
      DstOpIdx >= NumStaticOps(DstMI.getDesc())) {
    LLVM_DEBUG(llvm::dbgs()
               << "Warning!: No latency for dynamic op #" << SrcOpIdx << " in "
               << SrcMI << "Or op #" << DstOpIdx << " in " << DstMI << "\n");
    return;
  }

  if (std::optional<int> Lat = InstrInfo.getSignedOperandLatency(
          &InstrItins, SrcMI, DefIdx, DstMI, UseIdx, Dep.getKind())) {
    Dep.setSignedLatency(*Lat);
  }
}
