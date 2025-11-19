//===- AIEInlineSpiller.h - Custom AIE Inline Spiller -----------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Custom AIE inline spiller.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEINLINESPILLER_H
#define LLVM_LIB_TARGET_AIE_AIEINLINESPILLER_H

#include "llvm/CodeGen/Spiller.h"
#include <memory>

namespace llvm {

class AIEInlineSpiller : public Spiller {
  // Composition: wrap the standard InlineSpiller
  std::unique_ptr<Spiller> BaseSpiller;

public:
  AIEInlineSpiller(const Spiller::RequiredAnalyses &Analyses,
                   MachineFunction &MF, VirtRegMap &VRM, VirtRegAuxInfo &VRAI);

  void spill(LiveRangeEdit &LRE) override;
  ArrayRef<Register> getSpilledRegs() override;
  ArrayRef<Register> getReplacedRegs() override;
  void postOptimization() override;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEINLINESPILLER_H
