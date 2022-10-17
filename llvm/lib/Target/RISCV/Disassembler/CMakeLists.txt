#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
add_llvm_component_library(LLVMRISCVDisassembler
  RISCVDisassembler.cpp

  LINK_COMPONENTS
  MC
  MCDisassembler
  RISCVCodeGen
  RISCVDesc
  RISCVInfo
  Support

  ADD_TO_COMPONENT
  RISCV
  )
