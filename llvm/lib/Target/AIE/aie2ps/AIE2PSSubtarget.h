//===------AIE2PSSubtarget.h -Define Subtarget for the AIE2ps ----*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIE2ps specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2PS_AIE2PSSUBTARGET_H
#define LLVM_LIB_TARGET_AIE2PS_AIE2PSSUBTARGET_H
#include "AIE2PS.h"
#include "AIE2PSFrameLowering.h"
#include "AIE2PSISelLowering.h"
#include "AIE2PSInstrInfo.h"
#include "AIE2PSRegisterInfo.h"
#include "aie2/AIE2Subtarget.h"
#include "llvm/CodeGen/GlobalISel/CallLowering.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "AIE2PSGenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class AIE2PSSubtarget : public AIE2PSGenSubtargetInfo {
  virtual void anchor();
  std::string CPUName;
  // FIXME: Do we need a custom AIE2PSAddrSpaceInfo?
  AIE2AddrSpaceInfo AddrSpaceInfo;
  AIE2PSFrameLowering FrameLowering;
  AIE2PSInstrInfo InstrInfo;
  AIE2PSRegisterInfo RegInfo;
  AIE2PSTargetLowering TLInfo;
  InstrItineraryData InstrItins;
  SelectionDAGTargetInfo TSInfo;

  /// Initializes using the passed in CPU and feature strings so that we can
  /// use initializer lists for subtarget initialization.
  AIE2PSSubtarget &initializeSubtargetDependencies(const Triple &TT,
                                                   StringRef CPU, StringRef FS,
                                                   StringRef ABIName);

public:
  AIE2PSSubtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                  StringRef FS, StringRef ABIName, const TargetMachine &TM);
  bool enableMachineScheduler() const override { return true; }
  bool enablePostRAScheduler() const override { return true; }
  bool enablePostRAMachineScheduler() const override { return true; }
  bool useAA() const override { return true; }
  bool enableEarlyIfConversion() const override { return true; }
  bool enableWindowScheduler() const override { return true; }

  void
  adjustSchedDependency(SUnit *Def, int DefOpIdx, SUnit *Use, int UseOpIdx,
                        SDep &Dep,
                        const TargetSchedModel *SchedModel) const override {
    AIEBaseSubtarget::adjustSchedDependency(InstrItins, Def, DefOpIdx, Use,
                                            UseOpIdx, Dep);
  }
  bool enableSubRegLiveness() const override { return true; }

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  const AIE2PSFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const InstrItineraryData *getInstrItineraryData() const override {
    return &InstrItins;
  }
  const AIE2PSInstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const AIE2PSRegisterInfo *getRegisterInfo() const override {
    return &RegInfo;
  }
  const AIE2PSTargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
  const SelectionDAGTargetInfo *getSelectionDAGInfo() const override {
    return &TSInfo;
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

#endif
