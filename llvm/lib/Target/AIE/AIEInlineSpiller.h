//===- AIEInlineSpiller.h - Custom AIE Inline Spiller -----------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Custom AIE inline spiller.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEINLINESPILLER_H
#define LLVM_LIB_TARGET_AIE_AIEINLINESPILLER_H

#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/CodeGen/Spiller.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include <memory>

namespace llvm {

class SpillInfo {
  // Original register that is being spilled.
  Register Reg;
  // Defining operands of the original register.
  SmallVector<MachineOperand, 8> DefOps;
  // Stack slots used to spill the original register.
  SmallVector<unsigned, 8> StackSlots;
  SmallVector<LiveInterval *, 8> StackInts;
  // New virtual registers created for spilling.
  SmallVector<Register, 8> SpillVRegs;
  // Point to the instruction after which to Insert a Spill Store.
  SmallVector<MachineInstr *, 8> SpillLocations;
  // Point to the Instruction before which to Insert a Spill Load.
  SmallVector<MachineInstr *, 8> ReloadLocations;

  void insertSpill(MachineBasicBlock::iterator MI, bool IsKill,
                   MachineRegisterInfo &MRI, const TargetInstrInfo &TII,
                   const TargetRegisterInfo &TRI, VirtRegMap &VRM,
                   LiveIntervals &LIS);
  void insertReload(MachineInstr *MI, MachineRegisterInfo &MRI,
                    const TargetInstrInfo &TII, const TargetRegisterInfo &TRI,
                    VirtRegMap &VRM, LiveIntervals &LIS);

  void
  updateWriteDefs(SmallVector<std::pair<MachineInstr *, unsigned>, 8> &Ops);

public:
  SpillInfo(Register Reg) : Reg(Reg) {}
  void update(const Register Reg, MachineRegisterInfo &MRI);
  void calcStack(MachineRegisterInfo &MRI, const TargetRegisterInfo &TRI,
                 VirtRegMap &VRM, LiveStacks &LSS);
  void insertSpills(MachineRegisterInfo &MRI, const TargetInstrInfo &TII,
                    const TargetRegisterInfo &TRI, VirtRegMap &VRM,
                    LiveIntervals &LIS);
  void insertReloads(MachineRegisterInfo &MRI, const TargetInstrInfo &TII,
                     const TargetRegisterInfo &TRI, VirtRegMap &VRM,
                     LiveIntervals &LIS);

  Register getReg() const { return Reg; }
  void dump() const;
};

class AIEInlineSpiller : public Spiller {

  MachineFunction &MF;
  LiveIntervals &LIS;
  LiveStacks &LSS;
  VirtRegMap &VRM;
  MachineRegisterInfo &MRI;
  const TargetInstrInfo &TII;
  const TargetRegisterInfo &TRI;

  LiveRangeEdit *Edit = nullptr;
  LiveInterval *StackInt = nullptr;
  int StackSlot;
  Register Original;

  SmallVector<SpillInfo, 8> SpillInfos;

  // All registers to spill to StackSlot, including the main register.
  SmallVector<Register, 8> RegsToSpill;

  // All registers that were replaced by the spiller through some other method,
  // e.g. rematerialization.
  SmallVector<Register, 8> RegsReplaced;

  // All COPY instructions to/from snippets.
  // They are ignored since both operands refer to the same stack slot.
  // For bundled copies, this will only include the first header copy.
  SmallPtrSet<MachineInstr *, 8> SnippetCopies;

  // Live range weight calculator.
  VirtRegAuxInfo &VRAI;

public:
  AIEInlineSpiller(const Spiller::RequiredAnalyses &Analyses,
                   MachineFunction &MF, VirtRegMap &VRM, VirtRegAuxInfo &VRAI);

  void spill(LiveRangeEdit &Edit) override;
  ArrayRef<Register> getSpilledRegs() override;
  ArrayRef<Register> getReplacedRegs() override;
  void postOptimization() override;

private:
  void collectRegsToSpill();
  bool isSibling(Register Reg);
  bool isSnippet(const LiveInterval &SnipLI);
  bool isRegToSpill(Register Reg);

  void spillAll();

  SpillInfo collectSpillInfo() const;
  void updateSpillInfo(Register Reg, SpillInfo &SI) const;
  void insertSpills(SpillInfo &SI);
  void insertReloads(SpillInfo &SI);

  bool hasLiveDef(SmallVector<std::pair<MachineInstr *, unsigned>, 8> &Ops);
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEINLINESPILLER_H
