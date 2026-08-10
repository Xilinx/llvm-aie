//===-- AIE1Subtarget.h - Define Subtarget for the AIE -------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIE specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE1SUBTARGET_H
#define LLVM_LIB_TARGET_AIE_AIE1SUBTARGET_H

#include "AIEBaseSubtarget.h"
#include "aie1/AIE1FrameLowering.h"
#include "aie1/AIE1ISelLowering.h"
#include "aie1/AIE1InstrInfo.h"
#include "llvm/CodeGen/GlobalISel/CallLowering.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"
#include "llvm/CodeGen/RegisterBankInfo.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

#define GET_SUBTARGETINFO_HEADER
#include "AIEGenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class AIESubtarget final : public AIEGenSubtargetInfo {
  virtual void anchor();
  std::string CPUName;
  AIEBaseAddrSpaceInfo AddrSpaceInfo;
  AIEFrameLowering FrameLowering;
  AIEInstrInfo InstrInfo;
  AIERegisterInfo RegInfo;
  AIE1TargetLowering TLInfo;
  InstrItineraryData InstrItins;
  SelectionDAGTargetInfo TSInfo;

  /// Initializes using the passed in CPU and feature strings so that we can
  /// use initializer lists for subtarget initialization.
  AIESubtarget &initializeSubtargetDependencies(const Triple &TT, StringRef CPU,
                                                StringRef FS,
                                                StringRef ABIName);

public:
  AIESubtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU, StringRef FS,
               StringRef ABIName, const TargetMachine &TM);

  bool enablePostRAScheduler() const override { return true; }

  CodeGenOptLevel getOptLevelToEnablePostRAScheduler() const override {
    return CodeGenOptLevel::None;
  }

  bool enableMachineScheduler() const override { return false; }

  void overrideSchedPolicy(MachineSchedPolicy &Policy,
                           const SchedRegion &Region) const override {}
  unsigned getCriticalPathLimit() const override {
    return getSchedModel().MispredictPenalty / 2;
  }
  bool enableWindowScheduler() const override { return true; }

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  const AIEFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const InstrItineraryData *getInstrItineraryData() const override {
    return &InstrItins;
  }
  const AIEInstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const AIERegisterInfo *getRegisterInfo() const override { return &RegInfo; }
  const AIE1TargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
  }

  void
  adjustSchedDependency(SUnit *Def, int DefOpIdx, SUnit *Use, int UseOpIdx,
                        SDep &Dep,
                        const TargetSchedModel *SchedModel) const override {
    AIEBaseSubtarget::adjustSchedDependency(InstrItins, Def, DefOpIdx, Use,
                                            UseOpIdx, Dep);
  }

protected:
  std::unique_ptr<CallLowering> CallLoweringInfo;
  std::unique_ptr<LegalizerInfo> Legalizer;
  std::unique_ptr<RegisterBankInfo> RegBankInfo;
  std::unique_ptr<InstructionSelector> InstSelector;

public:
  const CallLowering *getCallLowering() const override;
  const LegalizerInfo *getLegalizerInfo() const override;
  const RegisterBankInfo *getRegBankInfo() const override;
  InstructionSelector *getInstructionSelector() const override;
  const AIEBaseAddrSpaceInfo &getAddrSpaceInfo() const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE1SUBTARGET_H
