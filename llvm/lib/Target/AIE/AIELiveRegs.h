//===- AIELiveRegs.h - Liveness Analysis logic -*- C++ -*------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Class providing services for Liveness Analysis.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIELIVEREGS_H
#define LLVM_LIB_TARGET_AIE_AIELIVEREGS_H

#include "llvm/CodeGen/LivePhysRegs.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include <map>
#include <queue>
#include <set>

namespace llvm::AIE {

class LiveRegs {
  // Mapping from Machine Basic Blocks to their livein registers.
  std::map<const MachineBasicBlock *, LivePhysRegs> LiveIns;

  // Queue to manage the order of MBBs to be processed.
  std::queue<const MachineBasicBlock *> WorkList;

  // Map to track whether an MBB is in the WorkList.
  std::map<const MachineBasicBlock *, bool> InWorkList;

  // Pointer to Target Register Information, used for initializing livein
  // registers.
  const TargetRegisterInfo *TRI;

  // Adds a Machine Basic Block to the work list if it is not already present.
  void addToWorkList(const MachineBasicBlock *MBB);

  // Pops a Machine Basic Block from the work list, removes it from the work
  // list set, and returns the popped MBB.
  const MachineBasicBlock *popFromWorkList();

  // Computes the live-in registers for a given Machine Basic Block.
  void computeBlockLiveIns(const MachineBasicBlock *MBB,
                           LivePhysRegs &CurrentLive);

  // Checks if the CurrentLive is the same as the liveins of the given MBB.
  static bool equal(LivePhysRegs &CurrentLive, LivePhysRegs &OldLive);

  // Updates OldLive registers with CurrentLive registers.
  void updateLiveRegs(LivePhysRegs &CurrentLive, LivePhysRegs &OldLive);

public:
  const std::map<const MachineBasicBlock *, LivePhysRegs> &getLiveIns() const;
  LiveRegs(const MachineFunction *MF);
};

} // end namespace llvm::AIE

#endif // LLVM_LIB_TARGET_AIE_AIELIVEREGS_H
