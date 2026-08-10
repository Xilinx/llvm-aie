//===-- AIEBaseSubtarget.h - Define Subtarget for AIEx ----------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIEx specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASESUBTARGET_H
#define LLVM_LIB_TARGET_AIE_AIEBASESUBTARGET_H

#include "AIEBaseAddrSpaceInfo.h"
#include "AIEBaseInstrInfo.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/CodeGen/MachineScheduler.h"
#include "llvm/CodeGen/ScheduleDAGMutation.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/CodeGenTypes/MachineValueType.h"
#include "llvm/MC/MCInstrItineraries.h"

namespace llvm {

class TargetRegisterInfo;
class TargetFrameLowering;
class TargetInstrInfo;
struct AIEBaseInstrInfo;
class InstrItineraryData;
class ScheduleDAGMutation;
class SUnit;
class SDep;

class AIEBaseSubtarget : public TargetSubtargetInfo {
  AIEABI::ABI TargetABI = AIEABI::ABI_VITIS;

public:
  using TargetSubtargetInfo::TargetSubtargetInfo;

  static const AIEBaseSubtarget &get(const MachineFunction &MF);
  const AIEBaseInstrInfo *getInstrInfo() const override = 0;
  virtual const AIEBaseAddrSpaceInfo &getAddrSpaceInfo() const = 0;
  AIEABI::ABI getTargetABI() const { return TargetABI; }
  bool isAIE1() const { return getTargetTriple().isAIE1(); }
  bool isAIE2() const { return getTargetTriple().isAIE2(); }
  bool isAIE2P() const { return getTargetTriple().isAIE2P(); }
  bool isAIE2PS() const { return getTargetTriple().isAIE2PS(); }

  void getSMSMutations(std::vector<std::unique_ptr<ScheduleDAGMutation>>
                           &Mutations) const override {
    Mutations = AIEBaseSubtarget::getSMSMutationsImpl(getTargetTriple());
  }
  void getPostRAMutations(std::vector<std::unique_ptr<ScheduleDAGMutation>>
                              &Mutations) const override {
    Mutations =
        AIEBaseSubtarget::getPostRAMutationsImpl(getTargetTriple(), nullptr);
  }

  void overrideSchedPolicy(MachineSchedPolicy &Policy,
                           const SchedRegion &Region) const override;

  using TargetSubtargetInfo::adjustSchedDependency;
  void adjustSchedDependency(const InstrItineraryData &Itineraries, SUnit *Def,
                             int DefOpIdx, SUnit *Use, int UseOpIdx,
                             SDep &Dep) const;

  /// Required DAG mutations during Post-RA scheduling.
  static std::vector<std::unique_ptr<ScheduleDAGMutation>>
  getPostRAMutationsImpl(const Triple &TT, AAResults *AA);

  /// Required DAG mutations for InterBlock dependence analysis
  static std::vector<std::unique_ptr<ScheduleDAGMutation>>
  getDDGMutationsImpl(const Triple &TT, bool ExactLatencies,
                      AAResults *AA = nullptr);

  /// Required DAG mutations during Pre-RA scheduling.
  static std::vector<std::unique_ptr<ScheduleDAGMutation>>
  getPreRAMutationsImpl(const Triple &TT);

  /// Required DAG mutations during software pipelining.
  static std::vector<std::unique_ptr<ScheduleDAGMutation>>
  getSMSMutationsImpl(const Triple &TT);

  /// Whether to enable the pre-RA MachinePipeliner. This can be disabled to let
  /// the post-RA pipeliner handle the scheduling.
  bool enableMachinePipeliner() const override;

  bool enableWindowScheduler() const override;

  unsigned getCriticalPathLimit() const override;
  unsigned classifyGlobalReference(const GlobalValue *GV,
                                   const TargetMachine &TM) const;

  // All AIE targets need post scheduling for correct instruction timing
  bool forcePostRAScheduling() const override { return true; }

  /// See TargetSubtargetInfo::getSpillGroupOriginal. Forwards to the side map
  /// in AIEMachineFunctionInfo populated by AIE register-rewriter passes when
  /// they sever the VRM split-from chain for correctness.
  std::optional<Register>
  getSpillGroupOriginal(const MachineFunction &MF,
                        Register VirtReg) const override;
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASESUBTARGET_H
