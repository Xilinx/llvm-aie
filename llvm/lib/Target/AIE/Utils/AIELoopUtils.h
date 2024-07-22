//===-- AIELoopUtils.h ----------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains helper functions to handle loops and its metadata.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_UTILS_AIELOOPUTILS_H
#define LLVM_LIB_TARGET_AIE_UTILS_AIELOOPUTILS_H

#include "llvm/Analysis/LoopInfo.h"

namespace llvm::AIELoopUtils {

std::optional<int64_t> getMinTripCount(const MDNode *);

} // namespace llvm::AIELoopUtils

#endif
