//===-- AIEHazardRecognizerPRAS.cpp - AIE Hazard Recognizer for PRAS ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the hazard recognizer for scheduling on AIE. It is an
// extension of the base AIEHazardRecognizer to support the ancient post-RA
// list scheduler. I am looking forward to deleting this file.
//
//===----------------------------------------------------------------------===//

#include "AIEHazardRecognizerPRAS.h"

using namespace llvm;

#define DEBUG_TYPE "post-RA-sched"

// NOTE: This whole file is obsolesent

AIEHazardRecognizerPRAS::AIEHazardRecognizerPRAS(const AIEBaseInstrInfo *TII,
                                                 const InstrItineraryData *II)
    : AIEHazardRecognizer(TII, II, PRASAlternateDescriptors,
                          /*IsPreRA=*/false),
      CurrentBundle(TII->getFormatInterface()) {}

void AIEHazardRecognizerPRAS::StartBlock(MachineBasicBlock *MBB) {}

void AIEHazardRecognizerPRAS::EndBlock(MachineBasicBlock *MBB) {
  // This runs after reordering the instruction list in MBB.
  // Now we can set the bundling attributes from the bundles we collected
  // without confusing the reordering.

  // Flushes unfinished bundles at the end of scheduling a basic block.
  // FIXME: Would be nicer if this was guaranteed by the scheduler itself.
  if (!CurrentBundle.empty()) {
    Bundles.emplace_back(CurrentBundle);
    CurrentBundle.clear();
  }

  applyBundles(Bundles, MBB);

  // Clean up BB-local stuff.
  Bundles.clear();
}

void AIEHazardRecognizerPRAS::Reset() {
  AIEHazardRecognizer::Reset();

  // Assuming this is the start of a new region. We should
  // flush the previous
  if (!CurrentBundle.empty()) {
    Bundles.emplace_back(CurrentBundle);
    CurrentBundle.clear();
  }
}

// This is called in two circumstances:
// 1) No instruction has its operands available and is ready
// 2) An instruction *is* ready to execute, but cannot execute
// due to an architecture hazard/resource contention.
void AIEHazardRecognizerPRAS::AdvanceCycle() {
  AIEHazardRecognizer::AdvanceCycle();
  Bundles.emplace_back(CurrentBundle);
  CurrentBundle.clear();
}

// Similar to AdvanceCycle, but for bottom-up scheduling.
void AIEHazardRecognizerPRAS::RecedeCycle() {
  AIEHazardRecognizer::RecedeCycle();
  Bundles.emplace_back(CurrentBundle);
  CurrentBundle.clear();
}

unsigned AIEHazardRecognizerPRAS::PreEmitNoops(SUnit *SU) { return 0; }

void AIEHazardRecognizerPRAS::EmitInstruction(SUnit *SU, int DeltaCycles) {
  assert(DeltaCycles == 0);
  AIEHazardRecognizer::EmitInstruction(SU, DeltaCycles);
  CurrentBundle.add(SU->getInstr());
}

void AIEHazardRecognizerPRAS::EmitNoop() {
  LLVM_DEBUG(dbgs() << "Emit Noop\n");
  // Scheduler will add a nop to the instruction stream when
  // emitting the schedule. It should only do so if the
  // current cycle is empty. We could just advance the scoreboard state,
  // but for consistency we also force an empty bundle.
  AdvanceCycle();
}
