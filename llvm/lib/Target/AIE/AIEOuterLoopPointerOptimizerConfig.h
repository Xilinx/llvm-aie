//===- AIEOuterLoopPointerOptimizerConfig.h - OLPO policy -*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Target-specific policy for the outer-loop pointer optimizer. A target's
// pass config hands the pass an instance when the pass is created, so the
// queries the pass needs from a target live in one AIE-owned class. The
// default accepts only a zero offset, which reuses the pointer and encodes
// no immediate.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPOINTEROPTIMIZERCONFIG_H
#define LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPOINTEROPTIMIZERCONFIG_H

#include <cstdint>

namespace llvm {

class AIEOLPOTargetConfig {
public:
  virtual ~AIEOLPOTargetConfig() = default;

  /// True when \p Offset bytes is a legal pointer-add immediate. Zero reuses
  /// the pointer. A value this returns false for needs a modifier register.
  virtual bool isLegalPointerAddImmediate(int64_t Offset) const {
    return Offset == 0;
  }
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPOINTEROPTIMIZERCONFIG_H
