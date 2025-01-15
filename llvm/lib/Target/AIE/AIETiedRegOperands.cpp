//===----- AIETiedRegOperands.cpp - Constrain tied sub-registers --------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIETiedRegOperands.h"
#include <cassert>

using namespace llvm;

const OperandSubRegMapping *
TiedRegOperands::findOperandInfo(unsigned OpIdx) const {
  assert(SrcOps.size() == 1 && "Expected only one SrcOp (NoSubRegister) ");

  if (SrcOps.front().OpIdx == OpIdx)
    return &SrcOps.front();
  for (const OperandSubRegMapping &DstOp : DstOps) {
    if (DstOp.OpIdx == OpIdx)
      return &DstOp;
  }
  return nullptr;
}
