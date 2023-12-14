//===- llvm/CodeGen/ResourceCycles.h - Resource Tracking      ---*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This is an abstraction to track the resource use of one cycle
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_RESOURCECYCLE_H
#define LLVM_CODEGEN_RESOURCECYCLE_H

namespace llvm {

class MachineInstr;
class MCInstrDesc;

class ResourceCycle {
public:
  ResourceCycle() = default;
  virtual ~ResourceCycle() = default;

  // Reset the current state to make all resources available.
  virtual void clearResources() = 0;

  // Check if the resources occupied by a machine instruction are available
  // in the current state.
  virtual bool canReserveResources(MachineInstr &MI) = 0;

  // Reserve the resources occupied by a machine instruction and change the
  // current state to reflect that change.
  virtual void reserveResources(MachineInstr &MI) = 0;

  // Check if the resources occupied by a MCInstrDesc are available in
  // the current state.
  virtual bool canReserveResources(const MCInstrDesc *MID) = 0;

  // Reserve the resources occupied by a MCInstrDesc and change the current
  // state to reflect that change.
  virtual void reserveResources(const MCInstrDesc *MID) = 0;

  // Property that is required for DFAPacketizer. Used for dynamic checks.
  bool CanTrackResources = false;
};

} // namespace llvm

#endif //  LLVM_CODEGEN_RESOURCECYCLE_H
