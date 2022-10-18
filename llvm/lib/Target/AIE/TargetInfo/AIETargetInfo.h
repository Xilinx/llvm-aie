//===-- TargetInfo/AIETargetInfo.h - TargetInfo for AIE ---*- C++ -*-===//
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
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_TARGETINFO_AIETARGETINFO_H
#define LLVM_LIB_TARGET_AIE_TARGETINFO_AIETARGETINFO_H

namespace llvm {

class Target;

Target &getTheAIETarget();

Target &getTheAIE2Target();

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_TARGETINFO_AIETARGETINFO_H
