//===- AIEMachineAlignment.h ------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This Pass aligns MachineBasicBlocks to adhere to AIE alignment
// specifications.
//
// The MBBs are aligned sequentially.
// A MBB is padded with NOPs to reach alignment of the next MBB. Specific MBBs
// do not have to be aligned, so the predecessor MBB does not have to be padded.
// In the case that a MBB needs alignment and is not aligned, the previous
// MBBs are revisited and padded to achieve alignment.

#ifndef LLVM_LIB_TARGET_AIE_AIEMACHINEALIGNMENT_H
#define LLVM_LIB_TARGET_AIE_AIEMACHINEALIGNMENT_H

#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"

namespace llvm {

struct AIEBaseInstrInfo;

using Region = llvm::iterator_range<MachineBasicBlock::iterator>;

/// Helper Class to Group Regions together from multiple different MBBs.
class MultiBlockRegion {
  std::vector<Region> Regions;
  const AIEBaseInstrInfo &TII;

public:
  MultiBlockRegion(const Region Region, const AIEBaseInstrInfo &TII);

  void append(const Region &Region);

  const AIEBaseInstrInfo &getTII() const;

  unsigned getRegionSize() const;

  std::vector<Region> &getRegions() { return Regions; }
};

class AIEMachineAlignment : public llvm::MachineFunctionPass {

  std::vector<MultiBlockRegion> getAllRegions(MachineFunction &MF) const;

public:
  static char ID;
  AIEMachineAlignment() : MachineFunctionPass(ID) {}
  llvm::StringRef getPassName() const override {
    return "AIE Machine Alignment";
  }
  bool runOnMachineFunction(llvm::MachineFunction &MF) override;
};

} // end namespace llvm

#endif
