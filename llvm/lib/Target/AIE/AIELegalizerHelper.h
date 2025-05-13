//===- AIELegalizerHelper.h -------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements AIE specific legalization functions
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIELEGALIZERHELPER_H
#define LLVM_LIB_TARGET_AIE_AIELEGALIZERHELPER_H

#include "llvm/CodeGen/GlobalISel/LegalizerHelper.h"
#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"
#include "llvm/IR/InstrTypes.h"

namespace llvm {
struct AIEBaseInstrInfo;
class AIEBaseSubtarget;
class LegalizerHelper;
class MachineInstr;
class GICmp;

class AIELegalizerHelper {
  const AIEBaseSubtarget &ST;
  const LLT S1 = LLT::scalar(1);
  const LLT S8 = LLT::scalar(8);
  const LLT S16 = LLT::scalar(16);
  const LLT S32 = LLT::scalar(32);
  const LLT S64 = LLT::scalar(64);
  const LLT S20 = LLT::scalar(20);
  const LLT P0_20 = LLT::pointer(0, 20);
  const LLT V2S16 = LLT::fixed_vector(2, 16);
  const LLT V2S32 = LLT::fixed_vector(2, 32);
  const LLT V8ACC64 = LLT::fixed_vector(8, 64);
  const LLT V16S8 = LLT::fixed_vector(16, 8);
  const LLT V16S16 = LLT::fixed_vector(16, 16);
  const LLT V16BF16 = LLT::fixed_vector(16, 16);
  const LLT V16S32 = LLT::fixed_vector(16, 32);
  const LLT V16FP32 = LLT::fixed_vector(16, 32);
  const LLT V32S1 = LLT::fixed_vector(32, 1);
  const LLT V32S8 = LLT::fixed_vector(32, 8);
  const LLT V32S16 = LLT::fixed_vector(32, 16);
  const LLT V32BF16 = LLT::fixed_vector(32, 16);
  const LLT V32FP32 = LLT::fixed_vector(32, 32);
  const LLT V32ACC32 = LLT::fixed_vector(32, 32);
  const LLT V64S8 = LLT::fixed_vector(64, 8);
  const LLT V64FP32 = LLT::fixed_vector(64, 32);
  const LLT V8S64 = LLT::fixed_vector(8, 64);

public:
  AIELegalizerHelper(const AIEBaseSubtarget &ST);
  const AIEBaseInstrInfo *getInstrInfo();

  bool legalizeG_VASTART(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_BUILD_VECTOR(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_MERGE_VALUES(LegalizerHelper &Helper, MachineInstr &MI) const;
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
  bool legalizeG_FCMP_FP32_FP64(LegalizerHelper &Helper, MachineInstr &MI,
                                const CmpInst::Predicate FPredicate,
                                LostDebugLocObserver &LocObserver,
                                int ArgSize) const;
  bool legalizeG_FPTRUNC(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_FPEXT(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_FABS(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_FNEG(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_FADD_G_FSUB(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_FMUL(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_SELECT(LegalizerHelper &Helper, MachineInstr &MI,
                        const unsigned MaxBitSize = 512) const;
  bool legalizeG_BITCAST(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeLoopDecrement(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool legalizeG_CONCAT_VECTORS(LegalizerHelper &Helper,
                                MachineInstr &MI) const;
  bool legalizeBinOp(LegalizerHelper &Helper, MachineInstr &MI) const;

  // Helper functions for legalization
  bool pack32BitVector(LegalizerHelper &Helper, MachineInstr &MI,
                       Register SourceReg) const;
  bool unpackVector(LegalizerHelper &Helper, MachineInstr &MI,
                    Register SourceReg) const;
  bool legalizeG_UNMERGE_VALUES_128bit(LegalizerHelper &Helper,
                                       MachineInstr &MI) const;
  bool legalizeG_CONCAT_VECTORS_128bit(LegalizerHelper &Helper,
                                       MachineInstr &MI) const;
  bool legalizeG_AIE_EXTRACT_VECTOR_ELT(LegalizerHelper &Helper,
                                        MachineInstr &MI,
                                        const unsigned LegalVectorSize) const;
  bool legalizeG_TRUNC(LegalizerHelper &Helper, MachineInstr &MI) const;
  bool isUnaligned20BitStore(const LegalityQuery &Query) const;
  bool legalize_G_STORE(LegalizerHelper &Helper, GStore &StoreI) const;
  bool legalizeG_ICMP(LegalizerHelper &Helper, GICmp &ICmp) const;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIELEGALIZERHELPER_H
