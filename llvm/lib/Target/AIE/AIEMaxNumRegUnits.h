//===--- AIEMaxNumRegUnits.h - Collect max number of register units -------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEMAXNUMREGUNITS_H
#define LLVM_LIB_TARGET_AIE_AIEMAXNUMREGUNITS_H

#include <algorithm>

#define GET_NUM_REGUNITS
#include "AIEGenRegisterInfo.inc"

#define GET_NUM_REGUNITS
#include "AIE2GenRegisterInfo.inc"

#define GET_NUM_REGUNITS
#include "AIE2PGenRegisterInfo.inc"

#define GET_NUM_REGUNITS
#include "AIE2PSGenRegisterInfo.inc"

#include "StaticBitSet.h"

constexpr const int TotalNumRegUnits =
    std::max({AIE2PSRegInfo::NumRegUnits, AIE2PRegInfo::NumRegUnits,
              AIE2RegInfo::NumRegUnits, AIERegInfo::NumRegUnits});

/// PartSet: a bit set sized to hold one bit per register unit.
/// Used as the occupancy representation in Liveness, replacing LaneBitmask.
using PartSet = StaticBitSet<TotalNumRegUnits>;

#endif // LLVM_LIB_TARGET_AIE_AIEMAXNUMREGUNITS_H
