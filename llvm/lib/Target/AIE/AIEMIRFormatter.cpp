//===- AIEMIRFormatter.cpp ---------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// Implementation of AIE overrides of MIRFormatter.
//
//===----------------------------------------------------------------------===//

#include "AIEMIRFormatter.h"
#include "AIEMachineFunctionInfo.h"

using namespace llvm;

bool AIEMIRFormatter::parseCustomPseudoSourceValue(
    StringRef Src, MachineFunction &MF, PerFunctionMIParsingState &PFS,
    const PseudoSourceValue *&PSV, ErrorCallbackType ErrorCallback) const {
  const auto *MFI = MF.getInfo<AIEMachineFunctionInfo>();
  if (Src == "TileMemory") {
    PSV = MFI->getTileMemory();
    return false;
  }
  llvm_unreachable("unknown MIR custom pseudo source value");
}
