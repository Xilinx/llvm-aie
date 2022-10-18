//===- AIECallLowering.h - Call lowering ------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// This file describes how to lower LLVM calls to machine code calls for
/// GlobalISel.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIECALLLOWERING_H
#define LLVM_LIB_TARGET_AIE_AIECALLLOWERING_H

#include "llvm/CodeGen/FunctionLoweringInfo.h"
#include "llvm/CodeGen/GlobalISel/CallLowering.h"
#include "llvm/IR/CallingConv.h"

namespace llvm {

class AIETargetLowering;
class AIEBaseSubtarget;
class MachineInstrBuilder;

class AIECallLowering : public CallLowering {
public:
  explicit AIECallLowering(const TargetLowering &TLI);

  bool lowerReturn(MachineIRBuilder &MIRBuilder, const Value *Val,
                   ArrayRef<Register> VRegs,
                   FunctionLoweringInfo &FLI) const override;

  bool mustPreLowerReturn() const override { return true; }
  bool preLowerReturn(const Value *RetVal, ArrayRef<Register> VRegs,
                      FunctionLoweringInfo &FLI) const override;

  bool lowerFormalArguments(MachineIRBuilder &MIRBuilder, const Function &F,
                            ArrayRef<ArrayRef<Register>> VRegs,
                            FunctionLoweringInfo &FLI) const override;

  bool lowerCall(MachineIRBuilder &MIRBuilder,
                 CallLoweringInfo &Info) const override;

  /// A container class that helps access and maintain context between
  /// determining assignments and applying them, giving more flexibility than
  /// the standard CallLowering::determineAndHandleAssignments()
  class ArgAssignments {
  public:
    ArgAssignments(MachineFunction &MF, ArrayRef<ArgInfo> ArgInfos);
    SmallVector<CallLowering::ArgInfo, 16> &getArgInfos() { return ArgInfos; }
    SmallVector<CCValAssign, 16> &getArgLocs() { return ArgLocs; }
    CCState &getCCInfo() { return CCInfo; }
    SmallVector<Register, 4> getAssignedRegisters() const;
    void reserveRegisters(ArrayRef<Register> Regs);
    unsigned getAssignedStackSize() const;
    void reserveStackSize(unsigned Size);

  private:
    // State variables to be maintained
    SmallVector<CallLowering::ArgInfo, 16> ArgInfos;
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo;
  };

  /// Wrapper around CallLowering::determineAssignments to make use of
  /// the ArgAssignments class.
  bool determineAndLegalizeAssignments(ArgAssignments &AA,
                                       ValueAssigner &Assigner) const;

  /// Wrapper around CallLowering::handleAssignments to make use of
  /// the ArgAssignments class.
  bool handleAssignments(ArgAssignments &AA, ValueHandler &Handler,
                         MachineIRBuilder &MIRBuilder) const;

private:
  /// Split the return type into first class types and create an ArgInfo
  /// for each of them.
  SmallVector<CallLowering::ArgInfo, 4>
  createSplitRetInfos(const Value &RetVal, ArrayRef<Register> VRegs,
                      const Function &F) const;

  bool lowerReturnVal(MachineIRBuilder &MIRBuilder, const Value *Val,
                      ArrayRef<Register> VRegs,
                      FunctionLoweringInfo::SavedRetCCState &RetAssignments,
                      MachineInstrBuilder &Ret) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIECALLLOWERING_H
