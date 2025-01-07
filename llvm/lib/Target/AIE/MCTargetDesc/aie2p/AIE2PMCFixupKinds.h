//===-- AIE2PMCFixupKinds.h - AIE2P Specific Fixup Entries -----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PMCFIXUPKINDS_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PMCFIXUPKINDS_H

#include "AIEMCFixupKinds.h"

namespace llvm {
// AIE2P Fixups
namespace AIE2P {
#define GET_FIXUPS_DECL
#include "FixupInfo/AIE2PFixupInfo.inc"
} // end namespace AIE2P
#define GET_FIXUPKINDS_DECL
#include "FixupInfo/AIE2PFixupInfo.inc"
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIE2PMCFIXUPKINDS_H
