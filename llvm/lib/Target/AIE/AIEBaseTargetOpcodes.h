//===-- AIEBaseTargetOpcodes.h - Shared opcode namespace -----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// Exposes the canonical AIE:: namespace enum values produced by
// AIEBaseTarget.td.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASETARGETOPCODES_H
#define LLVM_LIB_TARGET_AIE_AIEBASETARGETOPCODES_H

#define GET_INSTRINFO_ENUM
#include "AIEBaseTargetGenInstrInfo.inc"

#endif // LLVM_LIB_TARGET_AIE_AIEBASETARGETOPCODES_H
