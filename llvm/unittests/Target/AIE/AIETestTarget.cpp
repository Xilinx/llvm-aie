//===- AIETestTarget.cpp --------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIETestTarget.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetLowering.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

namespace {

// Include helper functions to define a testing target.
#include "MFCommon.inc"

MCSchedModel AIETestSchedModel = {
    1000, // IssueWidth
    1000, // MicroOpBufferSize
    MCSchedModel::DefaultLoopMicroOpBufferSize,
    MCSchedModel::DefaultLoadLatency,
    MCSchedModel::DefaultHighLatency,
    MCSchedModel::DefaultMispredictPenalty,
    true,    // PostRAScheduler
    true,    // CompleteModel
    false,   // EnableIntervals
    0,       // Processor ID
    nullptr, // No resources
    nullptr, // No sched classes
    0,       // No resources
    0,       // No sched classes
    nullptr, // No Itinerary
    nullptr  // No extra processor descriptor
};

SubtargetSubTypeKV ProcModels[] = {SubtargetSubTypeKV{
    "aie-test", FeatureBitArray({}), FeatureBitArray({}), &AIETestSchedModel}};

} // namespace

class AIETestSubTarget : public TestSubTarget<TargetInstrInfo> {
public:
  AIETestSubTarget(TargetMachine &TM)
      : TestSubTarget<TargetInstrInfo>(TM, "aie-test", "aie-test", ProcModels) {
  }
};

LLVMTargetMachine *llvm::AIE::createAIETestTargetMachine() {
  static TestTargetMachine<AIETestSubTarget> AIETM;
  return &AIETM;
}
