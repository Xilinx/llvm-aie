//===--- AIEMaxNumResources.h - Collect max number of scheduler resources
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEMAXNUMRESOURCES_H
#define LLVM_LIB_TARGET_AIE_AIEMAXNUMRESOURCES_H

#include <algorithm>

#define GET_NUM_RESOURCES
#include "AIEGenSubtargetInfo.inc"

#define GET_NUM_RESOURCES
#include "AIE2GenSubtargetInfo.inc"

#define GET_NUM_RESOURCES
#include "AIE2PGenSubtargetInfo.inc"

constexpr const int TotalNumResources =
    std::max({AIE2PItineraries::NumResources, AIE2Itineraries::NumResources,
              AIEItineraries::NumResources});

#endif // LLVM_LIB_TARGET_AIE_AIEMAXNUMRESOURCES_H
