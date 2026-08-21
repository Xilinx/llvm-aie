//===-- AIE.h - Top-level interface for AIE -----------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
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
#include "llvm/Support/CommandLine.h"
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
FunctionPass *createAIE1MachineBlockPlacement();
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
MachineFunctionPass *createAIEPtrModOptimizer();
MachineFunctionPass *createAIEAddressSpaceFlattening();
MachineFunctionPass *createAIEEliminateDuplicatePHI();
FunctionPass *createAIEOutlineMemoryGEP();
FunctionPass *createAIESuperRegRewriter();
FunctionPass *createAIEWawRegRewriter();
FunctionPass *createAIEEpilogueRegRewriter();
FunctionPass *createAIEUnallocatedSuperRegRewriter();
FunctionPass *createAIESpillSlotOptimization();
FunctionPass *createAIEPostSelectOptimize();
FunctionPass *createAIEPreISelCombiner();
void initializeAIEPreISelCombinerPass(PassRegistry &);
MachineFunctionPass *
createDeadMachineInstructionElim(bool KeepLifetimeInstructions);

void initializeAIEBaseHardwareLoopsPass(PassRegistry &);
void initializeAIEClusterBaseAddressPass(PassRegistry &);
void initializeAIEPtrModOptimizerPass(PassRegistry &);
void initializeAIEAddressSpaceFlatteningPass(PassRegistry &);
void initializeAIEEliminateDuplicatePHIPass(PassRegistry &);
extern char &AIEFormatSelectorID;
void initializeAIEFormatSelectorPass(PassRegistry &);
void initializeAIEFinalizeBundlePass(PassRegistry &);
void initializeAIEMachineAlignmentPass(PassRegistry &);
void initializeAIE1MachineBlockPlacementPass(PassRegistry &);
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
extern char &AIEEpilogueRegRewriterID;
void initializeAIEEpilogueRegRewriterPass(PassRegistry &);
extern char &AIEUnallocatedSuperRegRewriterID;
void initializeAIEUnallocatedSuperRegRewriterPass(PassRegistry &);
extern char &AIESpillSlotOptimizationID;
void initializeAIESpillSlotOptimizationPass(PassRegistry &);
extern char &AIEOutlineMemoryGEPID;
void initializeAIEOutlineMemoryGEPPass(PassRegistry &);

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

extern char &AIERegClassConstrainerID;
void initializeAIERegClassConstrainerPass(PassRegistry &);
llvm::FunctionPass *createAIERegClassConstrainer();

extern char &ReservedRegsLICMID;
void initializeReservedRegsLICMPass(PassRegistry &);
llvm::FunctionPass *createReservedRegsLICMPass();

// Outer Loop Pipeliner (IR-level, aie2p and aie2ps targets)
extern char &AIEOuterLoopPipelinerID;
void initializeAIEOuterLoopPipelinerPass(PassRegistry &);
llvm::FunctionPass *createAIEOuterLoopPipelinerPass();

// Inner Loop Versioning (IR-level). Emits a runtime trip-count guard around a
// pipelined copy of a single-block inner loop whose minimum trip count is too
// small for the software pipeliner. See AIEInnerLoopVersioning.cpp.
extern char &AIEInnerLoopVersioningID;
extern llvm::cl::opt<bool> DisableInnerLoopVersioning;
void initializeAIEInnerLoopVersioningPass(PassRegistry &);
llvm::FunctionPass *createAIEInnerLoopVersioningPass();

// Outer Loop Pointer Optimizer (IR-level, runs before Outer Loop Pipeliner)
extern char &AIEOuterLoopPointerOptimizerID;
void initializeAIEOuterLoopPointerOptimizerPass(PassRegistry &);
llvm::FunctionPass *createAIEOuterLoopPointerOptimizerPass();
} // namespace llvm

#endif
