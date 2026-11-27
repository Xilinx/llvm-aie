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

#include "llvm/MC/MCRegister.h"
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

/// RegUnit: a register unit, as an index into a PartSet. MCRegUnit is opaque so
/// that TableGen can emit tables for unknown targets. PartSet is specific to
/// the union of the AIE targets, so here the index can be exposed. Building a
/// RegUnit from a raw index is explicit, an MCRegUnit converts implicitly, and
/// it converts back to unsigned to index a PartSet.
class RegUnit {
  unsigned Idx;

public:
  RegUnit() : Idx(0) {}
  explicit RegUnit(unsigned Idx) : Idx(Idx) {}
  RegUnit(llvm::MCRegUnit Unit) : Idx(static_cast<unsigned>(Unit)) {}
  operator unsigned() const { return Idx; }
};

/// PartSet: a bit set sized to hold one bit per register unit.
/// Used as the occupancy representation in Liveness, replacing LaneBitmask.
using PartSet = StaticBitSet<TotalNumRegUnits, RegUnit>;

#endif // LLVM_LIB_TARGET_AIE_AIEMAXNUMREGUNITS_H
