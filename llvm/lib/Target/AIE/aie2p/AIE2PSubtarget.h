//===------AIE2PSubtarget.h -Define Subtarget for the AIE2p -----*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIE2p specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE2P_AIE2PSUBTARGET_H
#define LLVM_LIB_TARGET_AIE2P_AIE2PSUBTARGET_H
#include "AIE2P.h"
#include "AIE2PFrameLowering.h"
#include "AIE2PISelLowering.h"
#include "AIE2PInstrInfo.h"
#include "AIE2PRegisterInfo.h"
#include "aie2/AIE2Subtarget.h"
#include "llvm/CodeGen/GlobalISel/CallLowering.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "AIE2PGenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class AIE2PSubtarget : public AIE2PGenSubtargetInfo {
  virtual void anchor();

  // Subtarget capability bits generated from the AIEFeatures.td
  // SubtargetFeatures and assigned by ParseSubtargetFeatures. Declared ahead of
  // the members whose initialization runs ParseSubtargetFeatures (FrameLowering
  // et al.) so the in-class default initializer cannot clobber a parsed value.
#define GET_SUBTARGETINFO_MACRO(ATTRIBUTE, DEFAULT, GETTER)                    \
  bool ATTRIBUTE = DEFAULT;
#include "AIE2PGenSubtargetInfo.inc"
#undef GET_SUBTARGETINFO_MACRO

  std::string CPUName;
  // FIXME: Do we need a custom AIE2PAddrSpaceInfo?
  AIE2AddrSpaceInfo AddrSpaceInfo;
  AIE2PFrameLowering FrameLowering;
  AIE2PInstrInfo InstrInfo;
  AIE2PRegisterInfo RegInfo;
  AIE2PTargetLowering TLInfo;
  InstrItineraryData InstrItins;
  SelectionDAGTargetInfo TSInfo;

  /// Initializes using the passed in CPU and feature strings so that we can
  /// use initializer lists for subtarget initialization.
  AIE2PSubtarget &initializeSubtargetDependencies(const Triple &TT,
                                                  StringRef CPU, StringRef FS,
                                                  StringRef ABIName);

public:
  AIE2PSubtarget(const Triple &TT, StringRef CPU, StringRef TuneCPU,
                 StringRef FS, StringRef ABIName, const TargetMachine &TM);
  bool enableMachineScheduler() const override { return true; }
  bool enablePostRAMachineScheduler() const override { return true; }
  bool useAA() const override { return true; }
  bool enableEarlyIfConversion() const override { return true; }

  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  // Capability queries generated from AIEFeatures.td (e.g. hasBFP16(),
  // hasAcc2048()). Prefer these over checking the CPU name or an arch macro.
#define GET_SUBTARGETINFO_MACRO(ATTRIBUTE, DEFAULT, GETTER)                    \
  bool GETTER() const { return ATTRIBUTE; }
#include "AIE2PGenSubtargetInfo.inc"
#undef GET_SUBTARGETINFO_MACRO

  const AIE2PFrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const InstrItineraryData *getInstrItineraryData() const override {
    return &InstrItins;
  }
  const AIE2PInstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const AIE2PRegisterInfo *getRegisterInfo() const override { return &RegInfo; }
  const AIE2PTargetLowering *getTargetLowering() const override {
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
