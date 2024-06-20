#
# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
add_llvm_component_library(LLVMAIEDesc
  AIE1AsmBackend.cpp
  AIE2AsmBackend.cpp
  AIEBaseAsmBackend.cpp
  AIEBaseMCFormats.cpp
  AIEBaseMCCodeEmitter.cpp
  AIEELFObjectWriter.cpp
  AIEMCAsmInfo.cpp
  AIEMCCodeEmitter.cpp
  AIEMCExpr.cpp
  AIEMCFixupKinds.cpp
  AIE1MCFixupKinds.cpp
  AIEMCFormats.cpp
  AIEMCInstrInfo.cpp
  AIEMCTargetDesc.cpp
  AIETargetAsmStreamer.cpp
  AIETargetELFStreamer.cpp
  AIE2MCFixupKinds.cpp
  AIE2MCFormats.cpp
  AIE2MCTargetDesc.cpp
  AIE2MCCodeEmitter.cpp
  AIEFormat.cpp

  LINK_COMPONENTS
  MC
  AIEInfo
  AIEAsmPrinter
  Support
  TargetParser

  ADD_TO_COMPONENT
  AIE

  )
