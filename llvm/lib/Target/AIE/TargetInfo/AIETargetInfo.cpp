//===-- AIETargetInfo.cpp - AIE Target Implementation -----------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/AIETargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

namespace llvm {
Target &getTheAIETarget() {
  static Target TheAIETarget;
  return TheAIETarget;
}

Target &getTheAIE2Target() {
  static Target TheAIE2Target;
  return TheAIE2Target;
}

Target &getTheAIE2PTarget() {
  static Target TheAIE2PTarget;
  return TheAIE2PTarget;
}

} // namespace llvm

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIETargetInfo() {
  RegisterTarget<Triple::aie> X(getTheAIETarget(), "aie", "Xilinx AI Engine",
                                "AIE");
  RegisterTarget<Triple::aie2> Y(getTheAIE2Target(), "aie2",
                                 "Xilinx AIML Engine", "AIE");
  RegisterTarget<Triple::aie2p> A(getTheAIE2PTarget(), "aie2p",
                                  "AMD AIE2p Engine", "AIE");
}
