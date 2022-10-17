#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_component_library(LLVMAArch64Disassembler
  AArch64Disassembler.cpp
  AArch64ExternalSymbolizer.cpp

  LINK_COMPONENTS
  AArch64CodeGen
  AArch64Desc
  AArch64Info
  AArch64Utils
  MC
  MCDisassembler
  Support

  ADD_TO_COMPONENT
  AArch64
  )

add_dependencies(LLVMAArch64Disassembler AArch64CommonTableGen)
