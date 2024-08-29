//===-- AIE.h - Top-level interface for AIE -----------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// AIEngine back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIE_H
#define LLVM_LIB_TARGET_AIE_AIE_H

#include "Utils/AIEBaseInfo.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class AIESubtarget;
class AIETargetMachine;
class AsmPrinter;
class FunctionPass;
class InstructionSelector;
class MachineFunctionPass;
class MCInst;
class MCOperand;
class MachineInstr;
class MachineOperand;
class PassRegistry;
class RegisterBankInfo;
class ImmutablePass;

void LowerAIEMachineInstrToMCInst(const MachineInstr *MI, MCInst &OutMI,
                                    const AsmPrinter &AP);
bool LowerAIEMachineOperandToMCOperand(const MachineOperand &MO,
                                         MCOperand &MCOp, const AsmPrinter &AP);

FunctionPass *createAIEISelDag(AIETargetMachine &TM);
FunctionPass *createAIEFinalizeBundle();
FunctionPass *createAIEMachineAlignment();
FunctionPass *createAIEMachineBlockPlacement();
// createAIEDelaySlotFillerPass - This pass fills delay slots
// with useful instructions or nop's
FunctionPass *createAIEDelaySlotFillerPass(const AIETargetMachine &TM);

InstructionSelector *createAIEInstructionSelector(const AIETargetMachine &,
                                                  AIESubtarget &,
                                                  RegisterBankInfo &);
FunctionPass *createAIEBaseHardwareLoopsPass();
FunctionPass *createAIEPseudoBranchExpansion();
FunctionPass *createAIESubRegConstrainer();
MachineFunctionPass *createAIEClusterBaseAddress();
MachineFunctionPass *createAIEAddressSpaceFlattening();
MachineFunctionPass *createAIEEliminateDuplicatePHI();
FunctionPass *createAIESuperRegRewriter();
FunctionPass *createAIEWawRegRewriter();
FunctionPass *createAIEPostSelectOptimize();
MachineFunctionPass *
createDeadMachineInstructionElim(bool KeepLifetimeInstructions);

void initializeAIEBaseHardwareLoopsPass(PassRegistry &);
void initializeAIEClusterBaseAddressPass(PassRegistry &);
void initializeAIEAddressSpaceFlatteningPass(PassRegistry &);
void initializeAIEEliminateDuplicatePHIPass(PassRegistry &);
extern char &AIEFormatSelectorID;
void initializeAIEFormatSelectorPass(PassRegistry &);
void initializeAIEFinalizeBundlePass(PassRegistry &);
void initializeAIEMachineAlignmentPass(PassRegistry &);
void initializeAIEMachineBlockPlacementPass(PassRegistry &);
extern char &AIEPacketizerID;
void initializeAIEPacketizerPass(PassRegistry &);
void initializeAIEPostSelectOptimizePass(PassRegistry &);
void initializeAIEPseudoBranchExpansionPass(PassRegistry &);
extern char &AIESubRegConstrainerID;
void initializeAIESubRegConstrainerPass(PassRegistry &);
extern char &AIESuperRegRewriterID;
void initializeAIESuperRegRewriterPass(PassRegistry &);
extern char &AIEWawRegRewriterID;
void initializeAIEWawRegRewriterPass(PassRegistry &);

ImmutablePass *createAIEBaseAAWrapperPass();
void initializeAIEBaseAAWrapperPassPass(PassRegistry &);
ImmutablePass *createAIEBaseExternalAAWrapperPass();
void initializeAIEBaseExternalAAWrapperPass(PassRegistry &);

extern char &AIESplitInstrBuilderID;
void initializeAIESplitInstrBuilderPass(PassRegistry &);
llvm::FunctionPass *createAIESplitInstrBuilder();

extern char &AIESplitInstrReplacerID;
void initializeAIESplitInstrReplacerPass(PassRegistry &);
llvm::FunctionPass *createAIESplitInstrReplacer();

extern char &ReservedRegsLICMID;
void initializeReservedRegsLICMPass(PassRegistry &);
llvm::FunctionPass *createReservedRegsLICMPass();
}

#endif
