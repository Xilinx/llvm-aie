//===-- AIE2BaseAsmPrinter.h - AIEngine LLVM assembly writer --------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
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
public:
  explicit AIEBaseAsmPrinter(TargetMachine &TM,
                             std::unique_ptr<MCStreamer> Streamer)
      : AsmPrinter(TM, std::move(Streamer)) {}

  /// Called before any MBB is processed.
  void emitFunctionBodyStart() override;

  void emitInstruction(const MachineInstr *MI) override;

  bool isBlockOnlyReachableByFallthrough(
      const MachineBasicBlock *MBB) const override;

  void EmitToStreamer(MCStreamer &S, const MCInst &Inst);

  virtual bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                           const MachineInstr *MI) = 0;
  void emitXXStructorList(const DataLayout &DL, const Constant *List,
                          bool IsCtor) override;

private:
  // Count delay slots after an instruction with a delay slot
  int DelaySlotCounter = 0;

  /// The MBBs whose address is taken in the current function.
  SmallSet<const MachineBasicBlock *, 8> FunctionTakenMBBAddresses;
};
} // namespace llvm

#endif // #define LLVM_LIB_TARGET_AIE_AIEBASEASMPRINTER_H
