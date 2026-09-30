#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
if(NOT "AIE" IN_LIST LLVM_TARGETS_TO_BUILD)
  return()
endif()

set(LLVM_LINK_COMPONENTS
  AllTargetsDescs
  AllTargetsDisassemblers
  AllTargetsInfos
  AllTargetsSims
  MC
  MCDisassembler
  Object
  Support
  TargetParser
  )

add_llvm_tool(llvm-aie-run
  llvm-aie-run.cpp
  )
