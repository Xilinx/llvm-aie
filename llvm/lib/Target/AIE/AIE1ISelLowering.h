//===-- AIE1ISelLowering.h - AIE DAG Lowering Interface ---------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that AIEv1 uses to lower LLVM code into a
// selection DAG. Even though some of the SelectionDAG code isn't AIEv1
// specific, it is moved here because future AIE versions exclusively support
// GlobalISel.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE1ISELLOWERING_H
#define LLVM_LIB_TARGET_AIE_AIE1ISELLOWERING_H

#include "AIE.h"
#include "AIEBaseISelLowering.h"
#include "llvm/CodeGen/SelectionDAG.h"

namespace llvm {
class AIEBaseSubtarget;
namespace AIEISD {
// These node types represent target-specific pseudo-instructions.
// See AIEInstrInfo.td
enum NodeType : unsigned {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  RET_FLAG,
  CALL,
  SELECT_CC,
  BuildPairF64,
  SplitF64,
  TAIL,
  GLOBALADDRESSWRAPPER,
  GPR_CAST,
  STACK_SAVE,
  STACK_RESTORE
};
} // namespace AIEISD

class AIE1TargetLowering : public AIEBaseTargetLowering {
public:
  explicit AIE1TargetLowering(const TargetMachine &TM,
                              const AIEBaseSubtarget &STI);

  // Provide custom lowering hooks for some operations.
  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  // This method returns the name of a target specific DAG node.
  const char *getTargetNodeName(unsigned Opcode) const override;

  std::pair<unsigned, const TargetRegisterClass *>
  getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                               StringRef Constraint, MVT VT) const override;

  EVT getSetCCResultType(const DataLayout &DL, LLVMContext &Context,
                         EVT VT) const override;

private:
  void analyzeCallOperands(const TargetLowering::CallLoweringInfo &CLI,
                           CCState &CCInfo) const;
  // Lower incoming arguments, copy physregs into vregs
  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool IsVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;
  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      LLVMContext &Context) const override;
  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;
  SDValue LowerCall(TargetLowering::CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;
  void ReplaceNodeResults(SDNode *N, SmallVectorImpl<SDValue> &Results,
                          SelectionDAG &DAG) const override;
  // Custom lowerings
  SDValue LowerConstantPool(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerTargetGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerBUILD_VECTOR_x8(SDValue Op, SelectionDAG &DAG, MVT VecTy,
                               int start) const;
  SDValue LowerBUILD_VECTOR_i1(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerBUILD_VECTOR(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerLOAD(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerSTORE(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerVASTART(SDValue Op, SelectionDAG &DAG) const;
  SDValue LowerVAARG(SDValue Op, SelectionDAG &DAG) const;
  EVT getShiftAmountTy(EVT LHSTy, const DataLayout &DL,
                       bool LegalTypes = true) const override;
  bool shouldConvertConstantLoadToIntImm(const APInt &Imm,
                                         Type *Ty) const override {
    return true;
  }
};
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE1ISELLOWERING_H
