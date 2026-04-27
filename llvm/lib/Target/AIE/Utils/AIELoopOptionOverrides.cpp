//===- AIELoopOptionOverrides.cpp -----------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIELoopOptionOverrides.h"
#include "AIELoopUtils.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "aie-loop-option-overrides"

static llvm::cl::opt<bool> IgnoreLoopHints(
    "aie-ignore-loop-hints", llvm::cl::Hidden, llvm::cl::init(false),
    llvm::cl::desc(
        "Ignore per-loop option overrides from llvm.loop.hint metadata"));

namespace llvm::AIE {

LoopOptionOverrides::LoopOptionOverrides(const MDNode *LoopID) {
  if (!LoopID || IgnoreLoopHints)
    return;

  for (const auto *MD : AIELoopUtils::getLoopMetadataEntries(LoopID)) {
    const auto Key = AIELoopUtils::getMetadataKey(*MD);
    if (!Key || !Key->starts_with(Prefix))
      continue;

    // Strip the prefix to get the cl::opt ArgStr.
    const StringRef OptName = Key->drop_front(Prefix.size());

    if (const auto Val = AIELoopUtils::getMetadataIntValue(*MD)) {
      IntVals[OptName] = *Val;
      LLVM_DEBUG(dbgs() << "Loop override: " << OptName << " = " << *Val
                        << " (int)\n");
    } else if (const auto Val = AIELoopUtils::getMetadataStringValue(*MD)) {
      StrVals[OptName] = Val->str();
      LLVM_DEBUG(dbgs() << "Loop override: " << OptName << " = " << *Val
                        << " (string)\n");
    }
  }
}

LoopOptionOverrides::LoopOptionOverrides(const MachineBasicBlock &MBB)
    : LoopOptionOverrides(AIELoopUtils::getLoopID(MBB)) {}

} // namespace llvm::AIE
