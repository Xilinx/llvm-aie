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

#define DEBUG_TYPE "aielooputils"

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

std::optional<int64_t> getMinTripCount(const MDNode *LoopID) {
  if (LoopID == nullptr)
    return std::nullopt;

  assert(LoopID->getNumOperands() > 0 && "requires at least one operand");
  assert(dyn_cast<MDNode>(LoopID->getOperand(0)) == LoopID &&
         "invalid loop metadata");

  int64_t MinTripCount = 0;
  for (unsigned I = 1, E = LoopID->getNumOperands(); I < E; ++I) {
    const MDNode *MD = dyn_cast<MDNode>(LoopID->getOperand(I));

    if (MD == nullptr)
      continue;

    if (const MDString *S = dyn_cast<MDString>(MD->getOperand(0))) {
      if (S->getString() == "llvm.loop.itercount.range") {
        assert((MD->getNumOperands() >= 2 && MD->getNumOperands() <= 3) &&
               "Iteration count hint should have one or two numeric operands.");
        MinTripCount =
            mdconst::extract<ConstantInt>(MD->getOperand(1))->getSExtValue();
        assert(MinTripCount >= 0 && "Range lwb should not be negative.");
        LLVM_DEBUG(dbgs() << "AIELoopUtils: MinTripCount from pragma =  "
                          << MinTripCount << "\n");
        return MinTripCount;
      }
    }
  }
  return std::nullopt;
}

std::optional<int64_t> getMinTripCount(const MachineBasicBlock &LoopBlock) {
  return getMinTripCount(getLoopID(LoopBlock));
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
