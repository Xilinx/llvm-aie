//=- AIEMachineFunctionInfo.cpp - AIE machine function info -----*- C++ -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares AIE-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#include "AIEMachineFunctionInfo.h"

using namespace llvm;

yaml::AIEMachineFunctionInfo::AIEMachineFunctionInfo(
    const llvm::AIEMachineFunctionInfo &MFI)
    : VarArgsFrameIndex(MFI.getVarArgsFrameIndex()) {}

void yaml::AIEMachineFunctionInfo::mappingImpl(yaml::IO &YamlIO) {
  MappingTraits<AIEMachineFunctionInfo>::mapping(YamlIO, *this);
}

void AIEMachineFunctionInfo::initializeBaseYamlFields(
    const yaml::AIEMachineFunctionInfo &YamlMFI) {
  if (YamlMFI.VarArgsFrameIndex.has_value())
    setVarArgsFrameIndex(*YamlMFI.VarArgsFrameIndex);
}

namespace llvm {
AIEMachineFunctionInfo::AIEMachineFunctionInfo(const Function &F,
                                               const TargetSubtargetInfo *STI,
                                               const TargetMachine &TM)
    : TileMemory(TM) {}
} // namespace llvm
