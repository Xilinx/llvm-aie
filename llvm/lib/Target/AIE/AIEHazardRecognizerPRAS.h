//===--- AIEHazardRecognizerPRAS.h - AIE Hazard Recognizer for PRAS -------===//
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
// list scheduler. I am looking forward to deleting this file. I am hoping
// that isolating everything here will make the removal painless, whether it's
// removing AIE1 or removing PRAS in favor of MISched.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEHAZARDRECOGNIZERPRAS_H
#define LLVM_LIB_TARGET_AIE_AIEHAZARDRECOGNIZERPRAS_H

#include "AIEHazardRecognizer.h"

namespace llvm {

/// Obsolescent interfaces for PRAS
/// In particular:
///  - This maintains a list of Bundles
///  - This defines some callbacks only used by PRAS
class AIEHazardRecognizerPRAS : public AIEHazardRecognizer {
public:
  AIEHazardRecognizerPRAS(const AIEBaseInstrInfo *TII,
                          const InstrItineraryData *II);

  ~AIEHazardRecognizerPRAS() override {}

  void Reset() override;
  bool emitNoopsIfNoInstructionsAvailable() override { return true; }
  void EmitInstruction(SUnit *, int DeltaCycles) override;
  void EmitNoop() override;
  unsigned PreEmitNoops(SUnit *) override;
  void AdvanceCycle() override;
  void RecedeCycle() override;
  void StartBlock(MachineBasicBlock *MBB) override;
  void EndBlock(MachineBasicBlock *MBB) override;

private:
  std::vector<AIE::MachineBundle> Bundles;
  AIE::MachineBundle CurrentBundle;
  AIEAlternateDescriptors PRASAlternateDescriptors;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEHAZARDRECOGNIZERPRAS_H
