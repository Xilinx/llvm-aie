//===-- AIE2Subtarget.h - Define Subtarget for the AIEngine V2 ---*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIEngine V2 specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2_AIE2SUBTARGET_H
#define LLVM_LIB_TARGET_AIE2_AIE2SUBTARGET_H
#include "AIE2.h"
#include "AIE2AddrSpace.h"
#include "AIE2FrameLowering.h"
#include "AIE2ISelLowering.h"
#include "AIE2InstrInfo.h"
#include "AIEBaseSubtarget.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/CodeGen/GlobalISel/CallLowering.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"
#include "llvm/CodeGen/RegisterBankInfo.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

#define GET_SUBTARGETINFO_HEADER
#include "AIE2GenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class AIE2Subtarget : public AIE2GenSubtargetInfo {
  virtual void anchor();
  std::string CPUName;
  AIE2AddrSpaceInfo AddrSpaceInfo;
  AIE2FrameLowering FrameLowering;
  AIE2InstrInfo InstrInfo;
  AIE2RegisterInfo RegInfo;
  AIE2TargetLowering TLInfo;
  InstrItineraryData InstrItins;
  SelectionDAGTargetInfo TSInfo;

  /// Initializes using the passed in CPU and feature strings so that we can
  /// use initializer lists for subtarget initialization.
  AIE2Subtarget &initializeSubtargetDependencies(const Triple &TT,
                                                 StringRef CPU, StringRef FS,
                                                 StringRef ABIName);

public:
  AIE2Subtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                StringRef FS, StringRef ABIName, const TargetMachine &TM);

  bool enableMachineScheduler() const override { return true; }
  bool enablePostRAMachineScheduler() const override { return true; }
  bool useAA() const override { return true; }
  bool enableEarlyIfConversion() const override { return true; }

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  const AIE2FrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const InstrItineraryData *getInstrItineraryData() const override {
    return &InstrItins;
  }
  const AIE2InstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const AIE2RegisterInfo *getRegisterInfo() const override { return &RegInfo; }
  const AIE2TargetLowering *getTargetLowering() const override {
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

  bool enableSubRegLiveness() const override { return true; }

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

#endif
