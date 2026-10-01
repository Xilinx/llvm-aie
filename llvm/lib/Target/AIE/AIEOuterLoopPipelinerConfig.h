//===- AIEOuterLoopPipelinerConfig.h - OLP target policy --------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Target-specific policy for the outer-loop pipeliner. A target's pass config
// hands the pass an instance when the pass is created, so the queries the pass
// needs from a target live in one AIE-owned class. Targets that are happy with
// the defaults do not have to provide anything.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPIPELINERCONFIG_H
#define LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPIPELINERCONFIG_H

#include "AIEBaseInstrInfo.h"
#include "llvm/IR/Intrinsics.h"

namespace llvm {

class Instruction;

class AIEOLPTargetConfig {
  /// Source for the queries that are already described by the instruction
  /// info. Null only in unit tests, which do not exercise them.
  const AIEBaseInstrInfo *TII;

public:
  explicit AIEOLPTargetConfig(const AIEBaseInstrInfo *TII = nullptr)
      : TII(TII) {}
  virtual ~AIEOLPTargetConfig() = default;

  /// Returns true when \p I should be included in a target's lean stage-0
  /// prefetch chain.
  virtual bool isLeanStage0Intrinsic(const Instruction &I) const {
    return false;
  }

  /// Returns true when \p I is a target load intrinsic that seeds a lean
  /// stage-0 prefetch chain on its own, the way a plain load does.
  virtual bool isLeanStage0LoadIntrinsic(const Instruction &I) const {
    return false;
  }

  /// Returns true for the target's multidimensional address-increment
  /// intrinsics, the pointer updates that are safe to move along with the
  /// loads they feed.
  virtual bool isSafePointerIncrementIntrinsic(Intrinsic::ID ID) const {
    return TII &&
           (ID == TII->getAddrIntrinsic2D() || ID == TII->getAddrIntrinsic3D());
  }
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEOUTERLOOPPIPELINERCONFIG_H
