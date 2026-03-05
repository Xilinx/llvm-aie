//===-- AIE2BaseAsmPrinter.h - AIEngine LLVM assembly writer --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to the AIEngine assembly language.
//
//===----------------------------------------------------------------------===//
//
#ifndef LLVM_LIB_TARGET_AIE_AIEBASEASMPRINTER_H
#define LLVM_LIB_TARGET_AIE_AIEBASEASMPRINTER_H

#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/ADT/SmallSet.h"

namespace llvm {

class TargetMachine;
class MachineInstr;
class MCStreamer;

class AIEBaseAsmPrinter : public AsmPrinter {
  const MCRegisterInfo *MCRI = nullptr;

public:
  explicit AIEBaseAsmPrinter(TargetMachine &TM,
                             std::unique_ptr<MCStreamer> Streamer);

  /// Called before any MBB is processed.
  void emitFunctionBodyStart() override;

  void emitInstruction(const MachineInstr *MI) override;

  bool isBlockOnlyReachableByFallthrough(
      const MachineBasicBlock *MBB) const override;

  void EmitToStreamer(MCStreamer &S, const MCInst &Inst);

  virtual bool lowerPseudoInstExpansion(const MachineInstr *MI, MCInst &Inst) = 0;

  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       const char *ExtraCode, raw_ostream &OS) override;
  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                             const char *ExtraCode, raw_ostream &OS) override;

  void emitBasicBlockStart(const MachineBasicBlock &MBB) override;

  void emitXXStructorList(const DataLayout &DL, const Constant *List,
                          bool IsCtor) override;

protected:
  // Dump Bundle Count to Optimization Remarks (for AIE2+ targets)
  virtual void emitBundleCount(const MachineBasicBlock &MBB);

private:
  // Count delay slots after an instruction with a delay slot
  int DelaySlotCounter = 0;

  /// The MBBs whose address is taken in the current function.
  SmallSet<const MachineBasicBlock *, 8> FunctionTakenMBBAddresses;
};
} // namespace llvm

#endif // #define LLVM_LIB_TARGET_AIE_AIEBASEASMPRINTER_H
