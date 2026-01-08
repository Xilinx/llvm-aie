//===- MIRVirtRegMap.h - MIR VirtRegMap Info -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the MIRVirtRegMapInfo class, which holds VirtRegMap
// assignments loaded from MIR files, and the MIRVirtRegMapWrapperLegacy pass
// that provides access to this information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_MIRVIRTREGMAP_H
#define LLVM_CODEGEN_MIRVIRTREGMAP_H

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/Register.h"

namespace llvm {

class MachineFunction;

/// Holds VirtRegMap assignments loaded from MIR files.
/// Stored in MachineFunction, populated during MIR parsing.
class MIRVirtRegMapInfo {
  DenseMap<Register, MCRegister> Virt2PhysMap;
  DenseMap<Register, int> Virt2StackSlotMap;
  DenseMap<Register, Register> Virt2SplitMap;
  DenseMap<Register, MCRegister> Virt2RequiredPhysMap;

public:
  void setPhysReg(Register VReg, MCRegister PhysReg) {
    Virt2PhysMap[VReg] = PhysReg;
  }

  void setStackSlot(Register VReg, int StackSlot) {
    Virt2StackSlotMap[VReg] = StackSlot;
  }

  void setSplitFromReg(Register VReg, Register SrcReg) {
    Virt2SplitMap[VReg] = SrcReg;
  }

  void setRequiredPhys(Register VReg, MCRegister PhysReg) {
    Virt2RequiredPhysMap[VReg] = PhysReg;
  }

  MCRegister getPhysReg(Register VReg) const {
    auto It = Virt2PhysMap.find(VReg);
    return It != Virt2PhysMap.end() ? It->second : MCRegister();
  }

  int getStackSlot(Register VReg) const {
    auto It = Virt2StackSlotMap.find(VReg);
    return It != Virt2StackSlotMap.end() ? It->second : -1;
  }

  Register getSplitFromReg(Register VReg) const {
    auto It = Virt2SplitMap.find(VReg);
    return It != Virt2SplitMap.end() ? It->second : Register();
  }

  MCRegister getRequiredPhys(Register VReg) const {
    auto It = Virt2RequiredPhysMap.find(VReg);
    return It != Virt2RequiredPhysMap.end() ? It->second : MCRegister();
  }

  bool hasAssignments() const {
    return !Virt2PhysMap.empty() || !Virt2StackSlotMap.empty() ||
           !Virt2SplitMap.empty() || !Virt2RequiredPhysMap.empty();
  }

  void clear() {
    Virt2PhysMap.clear();
    Virt2StackSlotMap.clear();
    Virt2SplitMap.clear();
    Virt2RequiredPhysMap.clear();
  }

  // Iterators for reconstruction
  auto phys_regs() const {
    return llvm::make_range(Virt2PhysMap.begin(), Virt2PhysMap.end());
  }
  auto stack_slots() const {
    return llvm::make_range(Virt2StackSlotMap.begin(), Virt2StackSlotMap.end());
  }
  auto split_regs() const {
    return llvm::make_range(Virt2SplitMap.begin(), Virt2SplitMap.end());
  }
  auto required_phys_regs() const {
    return llvm::make_range(Virt2RequiredPhysMap.begin(),
                            Virt2RequiredPhysMap.end());
  }
};

/// MachineFunctionPass that provides access to MIR-loaded register assignments.
/// Queries MachineFunction for the underlying data.
class MIRVirtRegMapWrapperLegacy : public MachineFunctionPass {
  MIRVirtRegMapInfo *Info = nullptr; // Pointer to MachineFunction's data

public:
  static char ID;

  MIRVirtRegMapWrapperLegacy();

  bool runOnMachineFunction(MachineFunction &MF) override;

  /// Get MIR-loaded assignments (may return nullptr if no MIR assignments
  /// exist)
  MIRVirtRegMapInfo *getInfo() const { return Info; }

  void getAnalysisUsage(AnalysisUsage &AU) const override;
};

} // end namespace llvm

#endif // LLVM_CODEGEN_MIRVIRTREGMAP_H
