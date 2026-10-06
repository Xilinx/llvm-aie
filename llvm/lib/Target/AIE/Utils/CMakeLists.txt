#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
add_llvm_component_library(LLVMAIEUtils
  AIEBaseInfo.cpp
  AIEIRUtils.cpp
  AIELoopUtils.cpp
  AIELoopOptionOverrides.cpp
  AIEMachineBasicBlockUtils.cpp
  AIERegAllocationUtils.cpp

  LINK_COMPONENTS
  Analysis
  CodeGen
  Core
  Support
  TransformUtils

  ADD_TO_COMPONENT
  AIE
  )
