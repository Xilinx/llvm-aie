//===-- AIEBaseSubtarget.h - Define Subtarget for AIEx ----------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIEx specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASESUBTARGET_H
#define LLVM_LIB_TARGET_AIE_AIEBASESUBTARGET_H

#include "AIEBaseInstrInfo.h"
#include "Utils/AIEBaseInfo.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/CodeGen/MachineValueType.h"
#include "llvm/CodeGen/ScheduleDAGMutation.h"
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

class AIEBaseSubtarget {
private:
  Triple TargetTriple;
  AIEABI::ABI TargetABI = AIEABI::ABI_VITIS;

public:
  AIEBaseSubtarget(const Triple &TT) : TargetTriple(TT) {}

  virtual const TargetRegisterInfo *getRegisterInfo() const = 0;
  virtual const TargetFrameLowering *getFrameLowering() const = 0;
  virtual const AIEBaseInstrInfo *getInstrInfo() const = 0;
  AIEABI::ABI getTargetABI() const { return TargetABI; }
  bool isAIE1() const { return (TargetTriple.isAIE1()); }
  bool isAIE2() const { return (TargetTriple.isAIE2()); }
  virtual ~AIEBaseSubtarget() = default;

  // Perform target-specific adjustments to the latency of a schedule
  // dependency.
  // If a pair of operands is associated with the schedule dependency, DefOpIdx
  // and UseOpIdx are the indices of the operands in Def and Use, respectively.
  // Otherwise, either may be -1.
  // This is the shared code of a virtual method in TargetSubtargetInfo. We
  // can't directly override here, since we don't inherit it. Instead we
  // separately override a wrapper calling this implementation in AIE1 and AIE2
  void adjustSchedDependency(const InstrItineraryData &Itineraries, SUnit *Def,
                             int DefOpIdx, SUnit *Use, int UseOpIdx,
                             SDep &Dep) const;

  /// Required DAG mutations during Post-RA scheduling.
  static std::vector<std::unique_ptr<ScheduleDAGMutation>>
  getPostRAMutationsImpl(const Triple &TT);

  /// Required DAG mutations during Pre-RA scheduling.
  static std::vector<std::unique_ptr<ScheduleDAGMutation>>
  getPreRAMutationsImpl(const Triple &TT);
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASESUBTARGET_H
