//===- aie2p-capability-select.cc -------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// A kernel selects the multiply-accumulate path by capability, not by arch.
// There is no __AIEARCH__ / __AIE_ARCH__ guard here: the choice is driven by
// the __AIE_HAS_BFP16__ capability macro, which is toggled by a SubtargetFeature.
// REQUIRES: aie-registered-target

// Default aie2p: bfp16 is available, so the native path is selected.
// RUN: %clang_cc1 -triple aie2p -emit-llvm -o - %s | \
// RUN:     FileCheck %s --check-prefix=NATIVE

// aie2p with the capability turned off: the emulated fallback is selected.
// RUN: %clang_cc1 -triple aie2p -target-feature -bfp16 -emit-llvm -o - %s | \
// RUN:     FileCheck %s --check-prefix=EMULATED

int mac_path() {
#if defined(__AIE_HAS_BFP16__)
  return 16; // native block-floating-point path
#else
  return 32; // emulated fallback path
#endif
}

// NATIVE: ret i32 16
// EMULATED: ret i32 32
