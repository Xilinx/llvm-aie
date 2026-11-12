//===- AIETailDuplication.cpp - AIE Tail Duplication Pass --------*- C++
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// AIE-specific tail duplication pass. This pass substitutes the generic
/// EarlyTailDuplicate and TailDuplicate passes, giving the AIE backend full
/// control over the duplication threshold without modifying the common
/// infrastructure. Using the generic TII::getTailDuplicateSize hook would
/// require adding extra logic to the common code and can affect tests across
/// other targets. An internal pass avoids both problems.
//
//===----------------------------------------------------------------------===//

#include "AIETailDuplication.h"
#include "llvm/Analysis/ProfileSummaryInfo.h"
#include "llvm/CodeGen/LazyMachineBlockFrequencyInfo.h"
#include "llvm/CodeGen/MBFIWrapper.h"
#include "llvm/CodeGen/MachineBranchProbabilityInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/TailDuplicator.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/PassRegistry.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

#define DEBUG_TYPE "aie-tailduplication"

/// Maximum number of instructions to consider for tail duplication on AIE.
static cl::opt<unsigned> AIETailDupSize(
    "aie-tail-dup-size",
    cl::desc("Maximum instructions to consider tail duplicating on AIE"),
    cl::init(8), cl::Hidden);

namespace {

class AIETailDuplicate : public MachineFunctionPass {
  TailDuplicator Duplicator;
  std::unique_ptr<MBFIWrapper> MBFIW;
  bool PreRegAlloc;

public:
  AIETailDuplicate(char &PassID, bool PreRegAlloc)
      : MachineFunctionPass(PassID), PreRegAlloc(PreRegAlloc) {}

  bool runOnMachineFunction(MachineFunction &MF) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<MachineBranchProbabilityInfoWrapperPass>();
    AU.addRequired<LazyMachineBlockFrequencyInfoPass>();
    AU.addRequired<ProfileSummaryInfoWrapperPass>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }
};

class AIETailDuplicateLate : public AIETailDuplicate {
public:
  static char ID;
  AIETailDuplicateLate() : AIETailDuplicate(ID, /*PreRegAlloc=*/false) {
    initializeAIETailDuplicateLatePass(*PassRegistry::getPassRegistry());
  }
};

class AIETailDuplicateEarly : public AIETailDuplicate {
public:
  static char ID;
  AIETailDuplicateEarly() : AIETailDuplicate(ID, /*PreRegAlloc=*/true) {
    initializeAIETailDuplicateEarlyPass(*PassRegistry::getPassRegistry());
  }

  MachineFunctionProperties getClearedProperties() const override {
    return MachineFunctionProperties().setNoPHIs();
  }
};

} // end anonymous namespace

char AIETailDuplicateLate::ID;
char AIETailDuplicateEarly::ID;

char &llvm::AIETailDuplicateLateID = AIETailDuplicateLate::ID;
char &llvm::AIETailDuplicateEarlyID = AIETailDuplicateEarly::ID;

INITIALIZE_PASS(AIETailDuplicateLate, "aie-tailduplication",
                "AIE Tail Duplication", false, false)
INITIALIZE_PASS(AIETailDuplicateEarly, "aie-early-tailduplication",
                "AIE Early Tail Duplication", false, false)

bool AIETailDuplicate::runOnMachineFunction(MachineFunction &MF) {
  if (skipFunction(MF.getFunction()))
    return false;

  auto *MBPI =
      &getAnalysis<MachineBranchProbabilityInfoWrapperPass>().getMBPI();
  auto *PSI = &getAnalysis<ProfileSummaryInfoWrapperPass>().getPSI();
  auto *MBFI = (PSI && PSI->hasProfileSummary())
                   ? &getAnalysis<LazyMachineBlockFrequencyInfoPass>().getBFI()
                   : nullptr;
  if (MBFI)
    MBFIW = std::make_unique<MBFIWrapper>(*MBFI);

  Duplicator.initMF(MF, PreRegAlloc, MBPI, MBFI ? MBFIW.get() : nullptr, PSI,
                    /*LayoutMode=*/false, AIETailDupSize);

  bool MadeChange = false;
  while (Duplicator.tailDuplicateBlocks())
    MadeChange = true;

  return MadeChange;
}

FunctionPass *llvm::createAIETailDuplicateEarlyPass() {
  return new AIETailDuplicateEarly();
}

FunctionPass *llvm::createAIETailDuplicateLatePass() {
  return new AIETailDuplicateLate();
}
