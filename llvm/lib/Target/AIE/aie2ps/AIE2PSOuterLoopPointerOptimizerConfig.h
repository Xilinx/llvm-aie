//===- AIE2PSOuterLoopPointerOptimizerConfig.h - AIE2PS OLPO -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// AIE2PS answers to the outer-loop pointer optimizer's target queries.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSOUTERLOOPPOINTEROPTIMIZERCONFIG_H
#define LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSOUTERLOOPPOINTEROPTIMIZERCONFIG_H

#include "AIEOuterLoopPointerOptimizerConfig.h"

namespace llvm {

class AIE2PSOLPOTargetConfig : public AIEOLPOTargetConfig {
public:
  bool isLegalPointerAddImmediate(int64_t Offset) const override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2PS_AIE2PSOUTERLOOPPOINTEROPTIMIZERCONFIG_H
