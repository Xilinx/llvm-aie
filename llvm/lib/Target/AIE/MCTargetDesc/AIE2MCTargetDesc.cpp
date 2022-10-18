//===-- AIE2MCTargetDesc.cpp - AIEngine V2 Target Descriptions ------------===//
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
/// \brief This file provides AIEngine V2 specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "AIE2MCTargetDesc.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;
#define GET_INSTRINFO_MC_DESC
#include "AIE2GenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "AIE2GenRegisterInfo.inc"

MCInstrInfo *llvm::createAIE2MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitAIE2MCInstrInfo(X);
  return X;
}

MCRegisterInfo *llvm::createAIE2MCRegisterInfo(const Triple &TT) {
  auto *X = new MCRegisterInfo();
  InitAIE2MCRegisterInfo(X, AIE2::lr);
  return X;
}
