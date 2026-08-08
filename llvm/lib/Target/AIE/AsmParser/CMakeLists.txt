#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
add_llvm_component_library(LLVMAIEAsmParser
  AIEBaseAsmParser.cpp
  AIE1AsmParser.cpp
  AIE2AsmParser.cpp
  AIE2PAsmParser.cpp
  AIE2PSAsmParser.cpp
  AIEBaseOperand.cpp

  LINK_COMPONENTS
  MC
  MCParser
  AIEDesc
  AIEInfo
  AIEUtils
  Support
  TargetParser

  ADD_TO_COMPONENT
  AIE
  )
