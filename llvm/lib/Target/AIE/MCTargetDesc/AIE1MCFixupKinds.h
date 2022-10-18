//===-- AIE1MCFixupKinds.h - AIE Specific Fixup Entries --------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE1MCFIXUPKINDS_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE1MCFIXUPKINDS_H

#include "AIEMCFixupKinds.h"

namespace llvm {
// AIE1 Fixups
namespace AIE {
#define GET_FIXUPS_DECL
#include "FixupInfo/AIE1FixupInfo.inc"
} // end namespace AIE
#define GET_FIXUPKINDS_DECL
#include "FixupInfo/AIE1FixupInfo.inc"
} // namespace llvm

#endif
