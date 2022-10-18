//===-- AIEBaseInfo.h - Top level definitions for AIE MC ----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains small standalone enum definitions for the AIE target
// useful for the compiler back-end and the MC libraries.
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEBASEINFO_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEBASEINFO_H

#include "MCTargetDesc/AIEMCTargetDesc.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/TargetParser/SubtargetFeature.h"

namespace llvm {

// AIEII - This namespace holds all of the target specific flags that
// instruction info tracks. All definitions must match AIEInstrFormats.td.
namespace AIEII {
enum {
  MO_None = 0,
  MO_CALL = 1,
  MO_GLOBAL = 2,

  // Used to differentiate between target-specific "direct" flags and "bitmask"
  // flags. A machine operand can only have one "direct" flag, but can have
  // multiple "bitmask" flags.
  MO_DIRECT_FLAG_MASK = 3,
};
} // namespace AIEII

namespace AIEABI {

enum ABI {
  ABI_VITIS,
};

// Returns the target ABI, or else a StringError if the requested ABIName is
// not supported for the given TT and FeatureBits combination.
ABI computeTargetABI(const Triple &TT, FeatureBitset FeatureBits,
                     StringRef ABIName);

} // namespace AIEABI

namespace AIEFeatures {

// Validates if the given combination of features are valid for the target
// triple. Exits with report_fatal_error if not.
void validate(const Triple &TT, const FeatureBitset &FeatureBits);

} // namespace AIEFeatures

} // namespace llvm

#endif
