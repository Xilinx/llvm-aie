#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Modifications (c) Copyright 2025 Advanced Micro Devices, Inc. or its
# affiliates

set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Analysis
  AsmParser
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  FileCheck
  IRPrinter
  MC
  MIRParser
  Passes
  ScalarOpts
  SelectionDAG
  Support
  Target
  TargetParser
  TransformUtils
  )

add_llvm_unittest(CodeGenTests
  AllocationOrderTest.cpp
  AMDGPUMetadataTest.cpp
  AsmPrinterDwarfTest.cpp
  CCStateTest.cpp
  DIEHashTest.cpp
  DIETest.cpp
  DroppedVariableStatsMIRTest.cpp
  DwarfStringPoolEntryRefTest.cpp
  GCMetadata.cpp
  InstrRefLDVTest.cpp
  LowLevelTypeTest.cpp
  LexicalScopesTest.cpp
  MachineBasicBlockTest.cpp
  MachineDomTreeUpdaterTest.cpp
  MachineInstrBundleIteratorTest.cpp
  MachineInstrTest.cpp
  MachineOperandTest.cpp
  RegAllocScoreTest.cpp
  PassManagerTest.cpp
  ResourceScoreboardTest.cpp
  ScalableVectorMVTsTest.cpp
  SchedBoundary.cpp
  ScheduleDAGMITest.cpp
  ScheduleDAGMITestUtils.cpp
  SelectionDAGAddressAnalysisTest.cpp
  SelectionDAGNodeConstructionTest.cpp
  SelectionDAGPatternMatchTest.cpp
  TypeTraitsTest.cpp
  TargetOptionsTest.cpp
  TestAsmPrinter.cpp
  MLRegAllocDevelopmentFeatures.cpp
  X86MCInstLowerTest.cpp
  )

add_subdirectory(GlobalISel)
add_subdirectory(CGPluginTest)

target_link_libraries(CodeGenTests PRIVATE LLVMTestingSupport)
