#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
add_llvm_component_group(AIE)

set(LLVM_TARGET_DEFINITIONS aie1/AIE1.td)

tablegen(LLVM AIEGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM AIEGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM AIEGenCallingConv.inc -gen-callingconv)
#tablegen(LLVM AIEGenCompressInstEmitter.inc -gen-compress-inst-emitter)
tablegen(LLVM AIEGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM AIEGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM AIEGenGlobalISel.inc -gen-global-isel -gisel-subtarget-class AIEBaseSubtarget)
tablegen(LLVM AIEGenFormats.inc -gen-instr-format)
tablegen(LLVM AIEGenInstrInfo.inc -gen-instr-info -base-instrinfo-class AIEBaseInstrInfo)
tablegen(LLVM AIEGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM AIEGenMCPseudoLowering.inc -gen-pseudo-lowering)
tablegen(LLVM AIEGenRegisterBank.inc -gen-register-bank -base-register-bank-class AIEBaseRegisterBankInfo)
tablegen(LLVM AIEGenRegisterInfo.inc -gen-register-info -base-registerinfo-class AIEBaseRegisterInfo)
tablegen(LLVM AIEGenSubtargetInfo.inc -gen-subtarget -base-subtargetinfo-class AIEBaseSubtarget)
#tablegen(LLVM AIEGenSystemOperands.inc -gen-searchable-tables)
#aie2
set(LLVM_TARGET_DEFINITIONS aie2/AIE2.td)
list(APPEND LLVM_TABLEGEN_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/aie2")
tablegen(LLVM AIE2GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM AIE2GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM AIE2GenPreLegalizerGICombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PreLegalizerCombiner")
tablegen(LLVM AIE2GenPostLegalizerGIGenericCombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PostLegalizerGenericCombiner")
tablegen(LLVM AIE2GenPostLegalizerGICustomCombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PostLegalizerCustomCombiner")
tablegen(LLVM AIEGenPreISelGICombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIEPreISelCombiner")
tablegen(LLVM AIE2GenCallingConv.inc -gen-callingconv)
tablegen(LLVM AIE2GenCopyMaterialization.inc -gen-aie-copy-materialization)
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
tablegen(LLVM AIE2GenSubtargetInfo.inc -gen-subtarget -base-subtargetinfo-class AIEBaseSubtarget)
tablegen(LLVM AIE2GenGlobalISel.inc -gen-global-isel -gisel-subtarget-class AIEBaseSubtarget --warn-on-skipped-patterns)
list(REMOVE_ITEM LLVM_TABLEGEN_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/aie2")

#aie2p
set(LLVM_TARGET_DEFINITIONS aie2p/AIE2P.td)
list(APPEND LLVM_TABLEGEN_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/aie2p")
tablegen(LLVM AIE2PGenInstrInfo.inc -gen-instr-info -base-instrinfo-class AIEBaseInstrInfo)
tablegen(LLVM AIE2PGenRegisterInfo.inc -gen-register-info -base-registerinfo-class AIEBaseRegisterInfo)
tablegen(LLVM AIE2PGenCallingConv.inc -gen-callingconv)
tablegen(LLVM AIE2PGenCopyMaterialization.inc -gen-aie-copy-materialization)
tablegen(LLVM AIE2PGenSubtargetInfo.inc -gen-subtarget -base-subtargetinfo-class AIEBaseSubtarget)
tablegen(LLVM AIE2PGenMemoryCycles.inc -gen-aie-memory-cycles)
tablegen(LLVM AIE2PGenPreSchedLowering.inc -gen-aie-presched-lowering)
tablegen(LLVM AIE2PGenSplitInstrTables.inc -gen-aie-split-instr-tables)
tablegen(LLVM AIE2PGenRegisterBank.inc -gen-register-bank -base-register-bank-class AIEBaseRegisterBankInfo)
tablegen(LLVM AIE2PGenGlobalISel.inc -gen-global-isel -gisel-subtarget-class AIEBaseSubtarget --warn-on-skipped-patterns)
tablegen(LLVM AIE2PGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM AIE2PGenFormats.inc -gen-instr-format)
tablegen(LLVM AIE2PGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM AIE2PGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM AIE2PGenPreLegalizerGICombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PPreLegalizerCombiner")
tablegen(LLVM AIE2PGenPostLegalizerGIGenericCombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PPostLegalizerGenericCombiner")
tablegen(LLVM AIE2PGenPostLegalizerGICustomCombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PPostLegalizerCustomCombiner")
tablegen(LLVM AIE2PGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM AIE2PGenVarInstructionItin.inc -gen-aie-alternate-itinerary-emitter)
list(REMOVE_ITEM LLVM_TABLEGEN_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/aie2p")

# aie2ps
set(LLVM_TARGET_DEFINITIONS aie2ps/AIE2PS.td)
# Add aie2ps/ to the tablegen include search path (see aie2p comment above).
list(APPEND LLVM_TABLEGEN_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/aie2ps")
tablegen(LLVM AIE2PSGenInstrInfo.inc -gen-instr-info -base-instrinfo-class AIEBaseInstrInfo)
tablegen(LLVM AIE2PSGenRegisterInfo.inc -gen-register-info -base-registerinfo-class AIEBaseRegisterInfo)
tablegen(LLVM AIE2PSGenRegisterBank.inc -gen-register-bank -base-register-bank-class AIEBaseRegisterBankInfo)
tablegen(LLVM AIE2PSGenCallingConv.inc -gen-callingconv)
tablegen(LLVM AIE2PSGenCopyMaterialization.inc -gen-aie-copy-materialization)
tablegen(LLVM AIE2PSGenGlobalISel.inc -gen-global-isel -gisel-subtarget-class AIEBaseSubtarget --warn-on-skipped-patterns)
tablegen(LLVM AIE2PSGenSubtargetInfo.inc -gen-subtarget -base-subtargetinfo-class AIEBaseSubtarget)
tablegen(LLVM AIE2PSGenMemoryCycles.inc -gen-aie-memory-cycles)
tablegen(LLVM AIE2PSGenPreSchedLowering.inc -gen-aie-presched-lowering)
tablegen(LLVM AIE2PSGenSplitInstrTables.inc -gen-aie-split-instr-tables)
tablegen(LLVM AIE2PSGenFormats.inc -gen-instr-format)
tablegen(LLVM AIE2PSGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM AIE2PSGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM AIE2PSGenVarInstructionItin.inc -gen-aie-alternate-itinerary-emitter)
tablegen(LLVM AIE2PSGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM AIE2PSGenPreLegalizerGICombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PSPreLegalizerCombiner")
tablegen(LLVM AIE2PSGenPostLegalizerGIGenericCombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PSPostLegalizerGenericCombiner")
tablegen(LLVM AIE2PSGenPostLegalizerGICustomCombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PSPostLegalizerCustomCombiner")
tablegen(LLVM AIE2PSGenPostLegalizerGIFinalCombiner.inc -gen-global-isel-combiner -gisel-subtarget-class AIEBaseSubtarget
              -combiners="AIE2PSPostLegalizerFinalCombiner")
tablegen(LLVM AIE2PSGenAsmMatcher.inc -gen-asm-matcher)
list(REMOVE_ITEM LLVM_TABLEGEN_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/aie2ps")

add_public_tablegen_target(AIECommonTableGen)

add_llvm_target(AIECodeGen
   AIEAddressSpaceFlattening.cpp
   AIEBaseAliasAnalysis.cpp
   AIEBaseAsmPrinter.cpp
   AIEBaseFrameLowering.cpp
   AIEBaseHardwareLoops.cpp
   AIEBaseISelLowering.cpp
   AIEBaseInstrInfo.cpp
   AIEInnerLoopVersioning.cpp
   AIEBaseRegisterInfo.cpp
   AIEBasePipelinerLoopInfo.cpp
   AIEBaseInstructionSelector.cpp
   AIEBaseTargetTransformInfo.cpp
   AIEClusterBaseAddress.cpp
   AIECombinerHelper.cpp
   AIEBaseRegisterBankInfo.cpp
   AIEBaseSubtarget.cpp
   AIEBaseTargetMachine.cpp
   AIECallLowering.cpp
   AIEDataDependenceHelper.cpp
   AIEDumpArtifacts.cpp
   AIEEliminateDuplicatePHI.cpp
   AIEFinalizeBundle.cpp
   AIEGlobalCombiner.cpp
   AIEGlobalCombinerPtrMods.cpp
   AIEHazardRecognizer.cpp
   AIEPreISelCombiner.cpp
   AIEInterBlockScheduling.cpp
   AIEISelDAGToDAG.cpp
   AIELegalizerHelper.cpp
   AIELiveRegs.cpp
   AIELoopClass.cpp
   AIEMachineAlignment.cpp
   AIEMachineFunctionInfo.cpp
   AIEMachineScheduler.cpp
   AIEMaxLatencyFinder.cpp
   AIEMCInstLower.cpp
   AIEMIRFormatter.cpp
   AIEMultiSlotInstrMaterializer.cpp
   AIEPostPipeliner.cpp
   AIEPostSelectOptimize.cpp
   AIEPseudoBranchExpansion.cpp
   AIEPtrModOptimizer.cpp
   AIERegClassConstrainer.cpp
   AIERegMemEventTracker.cpp
   AIESlotCounts.cpp
   AIESpillSlotOptimization.cpp
   AIESlotStatistics.cpp
   AIESlotUtils.cpp
   AIESplitInstructionRewriter.cpp
   AIESubRegConstrainer.cpp
   AIESWPSolver.cpp
   AIESuperRegRewriter.cpp
   AIESuperRegUtils.cpp
   AIETargetObjectFile.cpp
   aie2/AIE2AsmPrinter.cpp
   aie2/AIE2FrameLowering.cpp
   aie2/AIE2InstrInfo.cpp
   aie2/AIE2InstructionSelector.cpp
   aie2/AIE2ISelLowering.cpp
   aie2/AIE2LegalizerInfo.cpp
   aie2/AIE2PostLegalizerCustomCombiner.cpp
   aie2/AIE2PostLegalizerGenericCombiner.cpp
   aie2/AIE2PreLegalizerCombiner.cpp
   AIEPostLegalizerCustomCombiner.cpp
   AIEPostLegalizerGenericCombiner.cpp
   AIEPostLegalizerFinalCombiner.cpp
   AIEPreLegalizerCombiner.cpp
   aie2/AIE2RegisterBankInfo.cpp
   aie2/AIE2RegisterInfo.cpp
   aie2/AIE2Subtarget.cpp
   aie2/AIE2TargetMachine.cpp
   aie2/AIE2TargetTransformInfo.cpp
   AIETiedRegOperands.cpp
   AIEUnallocatedSuperRegRewriter.cpp
   ReservedRegsLICM.cpp
   AIEOuterLoopPipeliner.cpp
   AIEOutlineMemoryGEP.cpp
   AIEWawRegRewriter.cpp
   # aie1
   aie1/AIE1AsmPrinter.cpp
   aie1/AIE1AsmPrinter.cpp
   aie1/AIE1DelaySlotFiller.cpp
   aie1/AIE1FrameLowering.cpp
   aie1/AIEHazardRecognizerPRAS.cpp
   aie1/AIE1InstrInfo.cpp
   aie1/AIE1InstructionSelector.cpp
   aie1/AIE1ISelLowering.cpp
   aie1/AIE1LegalizerInfo.cpp
   aie1/AIE1MachineBlockPlacement.cpp
   aie1/AIE1RegisterBankInfo.cpp
   aie1/AIE1RegisterInfo.cpp
   aie1/AIE1Subtarget.cpp
   aie1/AIE1TargetMachine.cpp
   # aie2p
   aie2p/AIE2PSubtarget.cpp
   aie2p/AIE2PTargetMachine.cpp
   aie2p/AIE2PTargetTransformInfo.cpp
   aie2p/AIE2PRegisterInfo.cpp
   aie2p/AIE2PLegalizerInfo.cpp
   aie2p/AIE2PInstrInfo.cpp
   aie2p/AIE2PFrameLowering.cpp
   aie2p/AIE2PRegisterBankInfo.cpp
   aie2p/AIE2PISelLowering.cpp
   aie2p/AIE2PInstructionSelector.cpp
   aie2p/AIE2PPostLegalizerCustomCombiner.cpp
   aie2p/AIE2PPostLegalizerGenericCombiner.cpp
   aie2p/AIE2PPreLegalizerCombiner.cpp
   # aie2ps
   aie2ps/AIE2PSSubtarget.cpp
   aie2ps/AIE2PSTargetMachine.cpp
   aie2ps/AIE2PSTargetTransformInfo.cpp
   aie2ps/AIE2PSRegisterInfo.cpp
   aie2ps/AIE2PSLegalizerInfo.cpp
   aie2ps/AIE2PSInstrInfo.cpp
   aie2ps/AIE2PSFrameLowering.cpp
   aie2ps/AIE2PSRegisterBankInfo.cpp
   aie2ps/AIE2PSISelLowering.cpp
   aie2ps/AIE2PSInstructionSelector.cpp
   aie2ps/AIE2PSPostLegalizerCustomCombiner.cpp
   aie2ps/AIE2PSPostLegalizerGenericCombiner.cpp
   aie2ps/AIE2PSPostLegalizerFinalCombiner.cpp
   aie2ps/AIE2PSPreLegalizerCombiner.cpp
   aie2ps/AIE2PSConvertFP16Operation.cpp

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
   Vectorize

   ADD_TO_COMPONENT
   AIE
   )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
add_subdirectory(Utils)
