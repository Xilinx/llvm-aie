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

  PARTIAL_SOURCES_INTENDED

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

# The loadable engine an embedder without LLVM headers dlopens. Kept out of
# LLVMAIESim so that linking the executor statically does not also drag in the
# C ABI's process-wide target registration.
add_llvm_library(aie-iss SHARED
  AIEISSCAbi.cpp

  PARTIAL_SOURCES_INTENDED

  LINK_COMPONENTS
  AIEDesc
  AIEDisassembler
  AIEInfo
  AIESim
  MC
  MCDisassembler
  Support
  TargetParser
  )
