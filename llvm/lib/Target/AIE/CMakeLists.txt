#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
add_llvm_component_group(AIE)

set(LLVM_TARGET_DEFINITIONS AIE.td)

tablegen(LLVM AIEGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM AIEGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM AIEGenCallingConv.inc -gen-callingconv)
#tablegen(LLVM AIEGenCompressInstEmitter.inc -gen-compress-inst-emitter)
tablegen(LLVM AIEGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM AIEGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM AIEGenGlobalISel.inc -gen-global-isel)
tablegen(LLVM AIEGenFormats.inc -gen-instr-format)
tablegen(LLVM AIEGenInstrInfo.inc -gen-instr-info -base-instrinfo-class AIEBaseInstrInfo)
tablegen(LLVM AIEGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM AIEGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM AIEGenRegisterBank.inc -gen-register-bank -base-register-bank-class AIEBaseRegisterBankInfo)
tablegen(LLVM AIEGenRegisterInfo.inc -gen-register-info -base-registerinfo-class AIEBaseRegisterInfo)
tablegen(LLVM AIEGenSubtargetInfo.inc -gen-subtarget)
#tablegen(LLVM AIEGenSystemOperands.inc -gen-searchable-tables)
set(LLVM_TARGET_DEFINITIONS AIE2.td)
tablegen(LLVM AIE2GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM AIE2GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM AIE2GenPreLegalizerGICombiner.inc -gen-global-isel-combiner
              -combiners="AIE2PreLegalizerCombiner")
tablegen(LLVM AIE2GenPostLegalizerGIGenericCombiner.inc -gen-global-isel-combiner
              -combiners="AIE2PostLegalizerGenericCombiner")
tablegen(LLVM AIE2GenPostLegalizerGICustomCombiner.inc -gen-global-isel-combiner
              -combiners="AIE2PostLegalizerCustomCombiner")
tablegen(LLVM AIE2GenCallingConv.inc -gen-callingconv)
tablegen(LLVM AIE2GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM AIE2GenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM AIE2GenFormats.inc -gen-instr-format)
tablegen(LLVM AIE2GenInstrInfo.inc -gen-instr-info -base-instrinfo-class AIEBaseInstrInfo)
tablegen(LLVM AIE2GenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM AIE2GenMemoryCycles.inc -gen-aie-memory-cycles)
tablegen(LLVM AIE2GenPreSchedLowering.inc -gen-aie-presched-lowering)
tablegen(LLVM AIE2GenSplitInstrTables.inc -gen-aie-split-instr-tables)
tablegen(LLVM AIE2GenVarInstructionItin.inc -gen-aie-alternate-itinerary-emitter)
tablegen(LLVM AIE2GenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM AIE2GenRegisterBank.inc -gen-register-bank -base-register-bank-class AIEBaseRegisterBankInfo)
tablegen(LLVM AIE2GenRegisterInfo.inc -gen-register-info -base-registerinfo-class AIEBaseRegisterInfo)
tablegen(LLVM AIE2GenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM AIE2GenGlobalISel.inc -gen-global-isel --warn-on-skipped-patterns)

add_public_tablegen_target(AIECommonTableGen)

add_llvm_target(AIECodeGen
   AIE1ISelLowering.cpp
   AIE1RegisterBankInfo.cpp
   AIE1AsmPrinter.cpp
   AIEAddressSpaceFlattening.cpp
   AIEBaseAliasAnalysis.cpp
   AIEBaseAsmPrinter.cpp
   AIEBaseFrameLowering.cpp
   AIEBaseHardwareLoops.cpp
   AIEBaseISelLowering.cpp
   AIEBaseInstrInfo.cpp
   AIEBasePipelinerLoopInfo.cpp
   AIEClusterBaseAddress.cpp
   AIECombinerHelper.cpp
   AIEBaseRegisterBankInfo.cpp
   AIEBaseSubtarget.cpp
   AIEBaseTargetMachine.cpp
   AIECallLowering.cpp
   AIEDelaySlotFiller.cpp
   AIEDumpArtifacts.cpp
   AIEEliminateDuplicatePHI.cpp
   AIEFinalizeBundle.cpp
   AIEFrameLowering.cpp
   AIEHazardRecognizer.cpp
   AIEHazardRecognizerPRAS.cpp
   AIEInstrInfo.cpp
   AIEInstructionSelector.cpp
   AIEInterBlockScheduling.cpp
   AIEISelDAGToDAG.cpp
   AIELegalizerHelper.cpp
   AIE1LegalizerInfo.cpp
   AIELiveRegs.cpp
   AIEMachineAlignment.cpp
   AIEMachineBlockPlacement.cpp
   AIEMachineFunctionInfo.cpp
   AIEMachineScheduler.cpp
   AIEMaxLatencyFinder.cpp
   AIEMCInstLower.cpp
   AIEMIRFormatter.cpp
   AIEPostPipeliner.cpp
   AIEPostSelectOptimize.cpp
   AIEPseudoBranchExpansion.cpp
   AIERegClassConstrainer.cpp
   AIERegisterInfo.cpp
   AIESlotCounts.cpp
   AIESplitInstructionRewriter.cpp
   AIESubRegConstrainer.cpp
   AIESubtarget.cpp
   AIESuperRegRewriter.cpp
   AIETargetMachine.cpp
   AIETargetObjectFile.cpp
   AIE2AsmPrinter.cpp
   AIE2FrameLowering.cpp
   AIE2InstrInfo.cpp
   AIE2InstructionSelector.cpp
   AIE2ISelDAGToDAG.cpp
   AIE2ISelLowering.cpp
   AIE2LegalizerInfo.cpp
   AIE2PostLegalizerCustomCombiner.cpp
   AIE2PostLegalizerGenericCombiner.cpp
   AIE2PreLegalizerCombiner.cpp
   AIE2RegisterBankInfo.cpp
   AIE2RegisterInfo.cpp
   AIE2Subtarget.cpp
   AIE2TargetMachine.cpp
   AIE2TargetTransformInfo.cpp
   AIETiedRegOperands.cpp
   ReservedRegsLICM.cpp
   AIEWawRegRewriter.cpp

   LINK_COMPONENTS
   Analysis
   AsmPrinter
   Core
   CodeGen
   CodeGenTypes
   IPO
   MC
   AIEAsmPrinter
   AIEDesc
   AIEInfo
   AIEUtils
   Scalar
   SelectionDAG
   Support
   Target
   TargetParser
   TransformUtils
   GlobalISel

   ADD_TO_COMPONENT
   AIE
   )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
add_subdirectory(Utils)
