//===-- AIE2MCFixupKinds.h - AIE2 Specific Fixup Entries --------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2MCFIXUPKINDS_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2MCFIXUPKINDS_H

#include "AIEMCFixupKinds.h"

namespace llvm {
// AIE2 Fixups
namespace AIE2 {
#define GET_FIXUPS_DECL
#include "FixupInfo/AIE2FixupInfo.inc"
} // end namespace AIE2
#define GET_FIXUPKINDS_DECL
#include "FixupInfo/AIE2FixupInfo.inc"
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2MCFIXUPKINDS_H
