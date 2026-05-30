//===- AIELoopUtils.cpp ---------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIELoopUtils.h"
#include "AIEBaseInstrInfo.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"
#include <optional>

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

bool hasIIPragma(const MachineBasicBlock &LoopBlock) {
  return getLoopMetadata(getLoopID(LoopBlock),
                         "llvm.loop.pipeline.initiationinterval")
      .has_value();
}

bool isOuterLoopPipelined(const MachineBasicBlock &LoopLatch) {
  // This metadata should be inserted by outerloop pipeliner.
  // We just check the availability. No other pass is expected
  // to insert this metadata.
  return getLoopMetadata(getLoopID(LoopLatch),
                         "llvm.loop.hint.aie_outerloop_pipeliner_success")
      .has_value();
}

std::optional<bool> getPipelinerDisabled(const MachineBasicBlock &LoopBlock) {
  if (getLoopMetadata(getLoopID(LoopBlock), "llvm.loop.pipeline.disable"))
    return true;
  return std::nullopt;
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

SmallVector<MachineBasicBlock *, 4>
getSingleBlockLoopMBBs(MachineFunction &MF) {
  // Delegate to the const overload so the LLVM_DEBUG output (and any future
  // logic) stays in one place. The returned MBB pointers refer to MBBs in
  // the non-const \p MF, so dropping const on each pointer is safe.
  SmallVector<MachineBasicBlock *, 4> LoopMBBs;
  for (const MachineBasicBlock *MBB :
       getSingleBlockLoopMBBs(static_cast<const MachineFunction &>(MF)))
    LoopMBBs.push_back(const_cast<MachineBasicBlock *>(MBB));
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

MachineBasicBlock *getLoopPredecessor(const MachineBasicBlock &EpilogueMBB) {
  MachineBasicBlock *LoopPred = nullptr;
  if (EpilogueMBB.pred_size() == 1) {
    // if we have only one, it must be the loop
    LoopPred = *EpilogueMBB.predecessors().begin();
  } else {
    // Otherwise, the loop is the fallthrough predecessor by construction
    for (auto *Pred : EpilogueMBB.predecessors()) {
      if (Pred->isLayoutSuccessor(&EpilogueMBB)) {
        LoopPred = Pred;
        break;
      }
    }
  }

  assert((!LoopPred || (isSingleMBBLoop(LoopPred))) &&
         "Layout predecessor is not a loop");
  return LoopPred;
}

std::optional<StringRef> getMetadataKey(const MDNode &MD) {
  if (MD.getNumOperands() == 0)
    return std::nullopt;
  if (const auto *S = dyn_cast<MDString>(MD.getOperand(0)))
    return S->getString();
  return std::nullopt;
}

std::optional<int64_t> getMetadataIntValue(const MDNode &MD) {
  if (MD.getNumOperands() < 2)
    return std::nullopt;
  if (const auto *CI = mdconst::dyn_extract<ConstantInt>(MD.getOperand(1)))
    return CI->getSExtValue();
  return std::nullopt;
}

std::optional<StringRef> getMetadataStringValue(const MDNode &MD) {
  if (MD.getNumOperands() < 2)
    return std::nullopt;
  if (const auto *S = dyn_cast<MDString>(MD.getOperand(1)))
    return S->getString();
  return std::nullopt;
}

SmallVector<const MDNode *, 4> getLoopMetadataEntries(const MDNode *LoopID) {
  SmallVector<const MDNode *, 4> Entries;
  if (!LoopID)
    return Entries;

  // First operand should refer to the loop id itself.
  assert(LoopID->getNumOperands() > 0 && "requires at least one operand");
  assert(dyn_cast_or_null<MDNode>(LoopID->getOperand(0)) == LoopID &&
         "invalid loop id");

  for (const MDOperand &MDO : llvm::drop_begin(LoopID->operands())) {
    if (const auto *MD = dyn_cast<MDNode>(MDO))
      Entries.push_back(MD);
  }
  return Entries;
}

std::optional<const MDNode *> getLoopMetadata(const MDNode *LoopID,
                                              const StringRef Name) {
  for (const auto *MD : getLoopMetadataEntries(LoopID)) {
    if (getMetadataKey(*MD) == Name)
      return MD;
  }
  return std::nullopt;
}

std::optional<bool> getLoopHintBool(const MDNode *LoopID, StringRef Key) {
  if (const auto Val = getLoopHintInt(LoopID, Key))
    return *Val != 0;
  return std::nullopt;
}

std::optional<bool> getLoopHintBool(const MachineBasicBlock &MBB,
                                    StringRef Key) {
  return getLoopHintBool(getLoopID(MBB), Key);
}

std::optional<int64_t> getLoopHintInt(const MDNode *LoopID, StringRef Key) {
  const auto MD = getLoopMetadata(LoopID, Key);
  if (!MD)
    return std::nullopt;
  return getMetadataIntValue(**MD);
}

std::optional<int64_t> getLoopHintInt(const MachineBasicBlock &MBB,
                                      StringRef Key) {
  return getLoopHintInt(getLoopID(MBB), Key);
}

std::optional<int64_t> getLoopVersioningHint(const MachineBasicBlock &MBB) {
  return getLoopHintInt(MBB, LoopVersioningHintKey);
}

std::optional<StringRef> getLoopHintString(const MDNode *LoopID,
                                           StringRef Key) {
  const auto MD = getLoopMetadata(LoopID, Key);
  if (!MD)
    return std::nullopt;
  return getMetadataStringValue(**MD);
}

std::optional<StringRef> getLoopHintString(const MachineBasicBlock &MBB,
                                           StringRef Key) {
  return getLoopHintString(getLoopID(MBB), Key);
}

// Returns true if the loop has an unroll(full) pragma.
bool hasUnrollFullPragma(const MDNode *LoopID) {
  return getLoopMetadata(LoopID, "llvm.loop.unroll.full").has_value();
}

// Returns true if the loop has an unroll(enable) pragma. This metadata is used
// for both "#pragma unroll" and "#pragma clang loop unroll(enable)" directives.
bool hasUnrollEnablePragma(const MDNode *LoopID) {
  return getLoopMetadata(LoopID, "llvm.loop.unroll.enable").has_value();
}

// Returns true if the loop has an unroll x pragma.
bool hasUnrollCountPragma(const MDNode *LoopID) {
  return getLoopMetadata(LoopID, "llvm.loop.unroll.count").has_value();
}

bool hasUnrollPragma(const Loop *L) {

  if (const MDNode *LoopID = L->getLoopID()) {
    return hasUnrollFullPragma(LoopID) || hasUnrollEnablePragma(LoopID) ||
           hasUnrollCountPragma(LoopID);
  }

  return false;
}

std::pair<MachineBasicBlock *, MachineBasicBlock *>
findPrologueEpilogue(const MachineBasicBlock &LoopBB) {
  assert(isSingleMBBLoop(&LoopBB) && "Expected a single-MBB loop");

  // Return nullptr for the ambiguous side if the loop has multiple non-loop
  // predecessors/successors. A valid do-while style outer loop may take its
  // backedge from a different block than the one that enters it, producing
  // two non-loop predecessors. Callers that require uniqueness (e.g. the
  // pipeliner's PipelineExtractor) must check for nullptr themselves.
  MachineBasicBlock *Prologue = nullptr;
  MachineBasicBlock *Epilogue = nullptr;
  bool MultiplePrologues = false;
  bool MultipleEpilogues = false;
  for (MachineBasicBlock *P : LoopBB.predecessors()) {
    if (P == &LoopBB)
      continue;
    if (Prologue) {
      MultiplePrologues = true;
      break;
    }
    Prologue = P;
  }
  for (MachineBasicBlock *S : LoopBB.successors()) {
    if (S == &LoopBB)
      continue;
    if (Epilogue) {
      MultipleEpilogues = true;
      break;
    }
    Epilogue = S;
  }
  if (MultiplePrologues)
    Prologue = nullptr;
  if (MultipleEpilogues)
    Epilogue = nullptr;
  return {Prologue, Epilogue};
}

std::optional<unsigned> getSWPStageCount(const MachineBasicBlock &LoopBB,
                                         const AIEBaseInstrInfo &TII) {
  // The lowered LoopStart in the preheader holds the trip-count adjustment
  // applied by SWP. For an NS-stage pipelined loop, Adj == -(NS - 1)
  // (negative because SWP peels NS-1 iterations into the prologue). Adj == 0
  // means no SWP touched this loop. A positive Adj should never appear; if
  // multiple non-loop preds exist they must all agree.
  std::optional<unsigned> Result;
  for (const MachineBasicBlock *Pred : LoopBB.predecessors()) {
    if (Pred == &LoopBB)
      continue;
    const MachineInstr *TripCountDef = TII.findZOLTripCountDef(*Pred);
    if (!TripCountDef)
      continue;
    const int Adj = TripCountDef->getOperand(2).getImm();
    if (Adj == 0)
      continue;
    assert(Adj < 0 && "SWP trip-count adjustment must be negative");
    const unsigned NS = static_cast<unsigned>(-Adj) + 1;
    assert((!Result || *Result == NS) &&
           "Disagreeing SWP stage counts across preheaders");
    Result = NS;
  }
  return Result;
}

} // namespace llvm::AIELoopUtils
