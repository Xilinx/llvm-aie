#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
include_directories(
  ${LLVM_MAIN_SRC_DIR}/lib/Target/AIE
  ${LLVM_BINARY_DIR}/lib/Target/AIE
  )

set(LLVM_LINK_COMPONENTS
  AIECodeGen
  AIEDesc
  AIEInfo
  CodeGen
  Core
  GlobalISel
  MC
  MIRParser
  SelectionDAG
  Support
  Target
)

add_llvm_target_unittest(AIETests
  BundleTest.cpp
  HazardRecognizerTest.cpp
  )
