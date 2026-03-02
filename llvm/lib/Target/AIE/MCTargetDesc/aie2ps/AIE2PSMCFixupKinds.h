//===-- AIE2PSMCFixupKinds.h - AIE2ps Specific Fixup Entries ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PSMCFIXUPKINDS_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PSMCFIXUPKINDS_H

#include "AIEMCFixupKinds.h"

namespace llvm {
// AIE2PS Fixups
namespace AIE2PS {
#define GET_FIXUPS_DECL
#include "FixupInfo/AIE2PSFixupInfo.inc"
} // end namespace AIE2PS
#define GET_FIXUPKINDS_DECL
#include "FixupInfo/AIE2PSFixupInfo.inc"
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PSMCFIXUPKINDS_H
