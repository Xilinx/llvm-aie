//===- AIEBaseInfo.cpp ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
#include "AIEBaseInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/Support/raw_ostream.h"

namespace llvm {

namespace AIEABI {
ABI computeTargetABI(const Triple &TT, FeatureBitset FeatureBits,
                     StringRef ABIName) {
  return ABI_VITIS;
} // namespace AIEABI

namespace AIEFeatures {

void validate(const Triple &TT, const FeatureBitset &FeatureBits) {
}

} // namespace AIEFeatures

} // namespace AIEABI
} // namespace llvm
