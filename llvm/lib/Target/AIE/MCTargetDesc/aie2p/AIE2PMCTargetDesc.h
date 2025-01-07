//===-- AIE2PMCTargetDesc.h - AIEngine V2+ Target Descriptions -*- C++ -*-===//
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
/// Provides AIEngine V2+ specific target descriptions.
//
//===----------------------------------------------------------------------===//
//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PMCTARGETDESC_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PMCTARGETDESC_H

#include <cstdint>

namespace llvm {
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCRegisterInfo;
class Triple;

MCCodeEmitter *createAIE2PMCCodeEmitter(const MCInstrInfo &MCII,
                                        MCContext &Ctx);
MCInstrInfo *createAIE2PMCInstrInfo();
MCRegisterInfo *createAIE2PMCRegisterInfo(const Triple &TT);

} // namespace llvm

#define GET_REGINFO_ENUM
#include "AIE2PGenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_OPERAND_ENUM
#define GET_INSTRINFO_SCHED_ENUM
#include "AIE2PGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "AIE2PGenSubtargetInfo.inc"

#endif
