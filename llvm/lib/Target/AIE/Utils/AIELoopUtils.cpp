//===- AIELoopUtils.cpp ---------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIELoopUtils.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils/LoopUtils.h"

#define DEBUG_TYPE "aielooputils"

namespace llvm {
cl::opt<int> LoopMinTripCount(
    "aie-loop-min-tripcount",
    cl::desc("Minimum number of loop iterations (warning: applies to all loop"
             " pipelining candidates)"),
    cl::init(-1), cl::Hidden);
} // namespace llvm

namespace llvm::AIELoopUtils {
const MDNode *getLoopID(const MachineBasicBlock &LoopBlock) {
  const BasicBlock *BBLK = LoopBlock.getBasicBlock();
  if (!BBLK)
    return nullptr;

  const Instruction *TI = BBLK->getTerminator();
  if (!TI)
    return nullptr;

  const MDNode *LoopID = TI->getMetadata(LLVMContext::MD_loop);
  return LoopID;
}

std::optional<int64_t> getMinTripCount(const MachineBasicBlock &LoopBlock) {
  std::optional<int64_t> MinTripCount = getMinTripCount(getLoopID(LoopBlock));
  if (LoopMinTripCount > MinTripCount.value_or(0)) {
    MinTripCount = LoopMinTripCount;
  }
  return MinTripCount;
}

std::optional<bool> getPipelinerDisabled(const MachineBasicBlock &LoopBlock) {
  auto *LoopID = getLoopID(LoopBlock);
  if (!LoopID) {
    return {};
  }
  for (const MDOperand &MDO : llvm::drop_begin(LoopID->operands())) {
    MDNode *MD = dyn_cast<MDNode>(MDO);
    if (MD == nullptr) {
      continue;
    }

    MDString *S = dyn_cast<MDString>(MD->getOperand(0));
    if (S == nullptr) {
      continue;
    }

    if (S->getString() == "llvm.loop.pipeline.disable") {
      return true;
    }
  }
  return {};
}

MachineBasicBlock *
getDedicatedFallThroughPreheader(const MachineBasicBlock &LoopBlock) {
  MachineBasicBlock *Candidate = nullptr;
  for (auto *P : LoopBlock.predecessors()) {
    if (P == &LoopBlock) {
      continue;
    }
    if (Candidate) {
      // This would be the second preheader
      return nullptr;
    }
    Candidate = P;
  }

  // Dedicated and fallthrough
  if (Candidate->succ_size() != 1 ||
      Candidate->getFirstTerminator() != Candidate->end() ||
      !Candidate->isLayoutSuccessor(&LoopBlock)) {
    return nullptr;
  }

  return Candidate;
}

SmallVector<const MachineBasicBlock *, 4>
getSingleBlockLoopMBBs(const MachineFunction &MF) {
  SmallVector<const MachineBasicBlock *, 4> LoopMBBs;
  for (const MachineBasicBlock &MBB : MF) {

    if (isSingleMBBLoop(&MBB)) {
      LoopMBBs.push_back(&MBB);
      LLVM_DEBUG(dbgs() << "Found Single Block Loop: " << MBB.getFullName()
                        << "\n");
    }
  }
  return LoopMBBs;
}

bool isSingleMBBLoop(const MachineBasicBlock *MBB) {
  int NumLoopEdges = 0;
  int NumExitEdges = 0;
  for (auto *S : MBB->successors())
    if (S == MBB)
      NumLoopEdges++;
    else
      NumExitEdges++;
  return NumLoopEdges == 1 && NumExitEdges == 1;
}

} // namespace llvm::AIELoopUtils
