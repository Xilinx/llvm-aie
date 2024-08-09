//===- AIELegalizerHelper.h -------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements AIE specific legalization functions
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIELEGALIZERHELPER_H
#define LLVM_LIB_TARGET_AIE_AIELEGALIZERHELPER_H

#include "llvm/CodeGen/GlobalISel/LegalizerHelper.h"
#include "llvm/IR/InstrTypes.h"

namespace llvm {
struct AIEBaseInstrInfo;
class AIEBaseSubtarget;
class LegalizerHelper;
class MachineInstr;

class AIELegalizerHelper {
  const AIEBaseSubtarget &ST;

public:
  AIELegalizerHelper(const AIEBaseSubtarget &ST);
  const AIEBaseInstrInfo *getInstrInfo();

  bool legalizeG_VASTART(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_BUILD_VECTOR(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_UNMERGE_VALUES(LegalizerHelper &Helper,
                                MachineInstr &MI) const;
  bool legalizeG_SEXT_INREG(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_VAARG(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeMemCalls(LegalizerHelper &Helper, MachineInstr &MI,
                        LostDebugLocObserver &LocObserver) const;
  bool legalizeG_BRJT(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_FCONSTANT(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_JUMP_TABLE(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_DYN_STACKALLOC(LegalizerHelper &Helper,
                                MachineInstr &MI) const;
  bool legalizeG_EXTRACT_VECTOR_ELT(LegalizerHelper &Helper,
                                    MachineInstr &MI) const;
  bool legalizeG_INSERT_VECTOR_ELT(LegalizerHelper &Helper,
                                   MachineInstr &MI) const;
  bool legalizeG_FCMP(LegalizerHelper &Helper, MachineInstr &MI,
                      LostDebugLocObserver &LocObserver) const;
  bool legalizeG_FCMP_FP32(LegalizerHelper &Helper, MachineInstr &MI,
                           const CmpInst::Predicate FPredicate,
                           LostDebugLocObserver &LocObserver) const;
  bool legalizeG_FPTRUNC(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_FPEXT(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_FABS(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_FADDSUB(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeLoopDecrement(LegalizerHelper &Helper, MachineInstr &MI) const;

  // Helper functions for legalization
  bool pack32BitVector(LegalizerHelper &Helper, MachineInstr &MI,
                       Register SourceReg) const;
  bool unpack32BitVector(LegalizerHelper &Helper, MachineInstr &MI,
                         Register SourceReg) const;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIELEGALIZERHELPER_H
