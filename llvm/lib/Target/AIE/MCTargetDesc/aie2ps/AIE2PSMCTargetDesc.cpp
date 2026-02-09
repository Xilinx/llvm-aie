//===-------- AIE2PSMCTargetDesc.cpp - AIE2ps Target Descriptions ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// \brief This file provides AIE2ps specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "AIE2PSMCTargetDesc.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;
#define GET_INSTRINFO_MC_DESC
#include "AIE2PSGenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "AIE2PSGenRegisterInfo.inc"

MCInstrInfo *llvm::createAIE2PSMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitAIE2PSMCInstrInfo(X);
  return X;
}

MCRegisterInfo *llvm::createAIE2PSMCRegisterInfo(const Triple &TT) {
  auto *X = new MCRegisterInfo();
  InitAIE2PSMCRegisterInfo(X, AIE2PS::lr);
  return X;
}
