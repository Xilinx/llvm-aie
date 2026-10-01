//===- AIE2PSOuterLoopPipelinerConfig.h - AIE2PS OLP policy -----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// AIE2PS answers to the outer-loop pipeliner's target queries.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSOUTERLOOPPIPELINERCONFIG_H
#define LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSOUTERLOOPPIPELINERCONFIG_H

#include "AIEOuterLoopPipelinerConfig.h"

namespace llvm {

class AIE2PSOLPTargetConfig : public AIEOLPTargetConfig {
public:
  using AIEOLPTargetConfig::AIEOLPTargetConfig;

  bool isLeanStage0LoadIntrinsic(const Instruction &I) const override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSOUTERLOOPPIPELINERCONFIG_H
