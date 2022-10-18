//===- AIEMCTargetDesc.h - AIE Target Descriptions --------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file provides AIE specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCTARGETDESC_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCTARGETDESC_H

#include "llvm/Config/config.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"
#include <memory>

namespace llvm {

class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class Target;

Target &getTheAIETarget();
Target &getTheAIE2Target();

MCCodeEmitter *createAIEMCCodeEmitter(const MCInstrInfo &MCII,
                                        MCContext &Ctx);

std::unique_ptr<MCObjectTargetWriter>
createAIEELFObjectWriter(uint8_t OSABI, Triple TargetTriple);

} // end namespace llvm

// Defines symbolic names for AIE registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "AIEGenRegisterInfo.inc"

// Defines symbolic names for the AIE instructions.
#define GET_INSTRINFO_ENUM
#include "AIEGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "AIEGenSubtargetInfo.inc"

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCTARGETDESC_H
