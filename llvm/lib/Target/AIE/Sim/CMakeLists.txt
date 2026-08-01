#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
add_llvm_component_library(LLVMAIESim
  AIECoreState.cpp
  AIEExecutor.cpp
  AIE2PSemantics.cpp

  LINK_COMPONENTS
  MCDisassembler
  AIEDesc
  AIEInfo
  MC
  Support
  TargetParser

  ADD_TO_COMPONENT
  AIE
  )
