//===- AIEInlineSpiller.cpp - Custom AIE Inline Spiller -------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file implements the AIEInlineSpiller class, which provides AIE-specific
// spilling strategies by wrapping the standard InlineSpiller.
//
//===----------------------------------------------------------------------===//

#include "AIEInlineSpiller.h"
#include "llvm/CodeGen/LiveRangeEdit.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/Spiller.h"
#include "llvm/CodeGen/VirtRegMap.h"

using namespace llvm;

AIEInlineSpiller::AIEInlineSpiller(const Spiller::RequiredAnalyses &Analyses,
                                   MachineFunction &MF, VirtRegMap &VRM,
                                   VirtRegAuxInfo &VRAI)
    : BaseSpiller(createInlineSpiller(Analyses, MF, VRM, VRAI)) {
  // AIE-specific initialization if needed
}

void AIEInlineSpiller::spill(LiveRangeEdit &LRE) {
  // AIE-specific pre-processing can go here
  BaseSpiller->spill(LRE);
  // AIE-specific post-processing can go here
}

ArrayRef<Register> AIEInlineSpiller::getSpilledRegs() {
  return BaseSpiller->getSpilledRegs();
}

ArrayRef<Register> AIEInlineSpiller::getReplacedRegs() {
  return BaseSpiller->getReplacedRegs();
}

void AIEInlineSpiller::postOptimization() { BaseSpiller->postOptimization(); }
