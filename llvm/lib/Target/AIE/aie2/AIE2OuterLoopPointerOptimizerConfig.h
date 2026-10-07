//===- AIE2OuterLoopPointerOptimizerConfig.h - AIE2 OLPO policy -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// AIE2 answers to the outer-loop pointer optimizer's target queries.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE2_AIE2OUTERLOOPPOINTEROPTIMIZERCONFIG_H
#define LLVM_LIB_TARGET_AIE_AIE2_AIE2OUTERLOOPPOINTEROPTIMIZERCONFIG_H

#include "AIEOuterLoopPointerOptimizerConfig.h"

namespace llvm {

class AIE2OLPOTargetConfig : public AIEOLPOTargetConfig {
public:
  bool isLegalPointerAddImmediate(int64_t Offset) const override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIE2_AIE2OUTERLOOPPOINTEROPTIMIZERCONFIG_H
