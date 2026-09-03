//===-- AIEFeatures.td - AIE subtarget capability set ------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// SubtargetFeatures describing per-generation AIE capability deltas. These let
// a compiler pass or a kernel query a capability (has(FeatureBFP16)) instead of
// hard-coding the generation by CPU name / arch macro. This is a capability
// SET, not a monotonic level: a later generation is not required to be a
// superset of an earlier one (e.g. bfp16 is aie2p-only).
//
//===----------------------------------------------------------------------===//

// Native block-floating-point (bfp16, ebs8/ebs16) multiply-accumulate support.
// aie2p only.
def FeatureBFP16
    : SubtargetFeature<
          "bfp16", "HasBFP16", "true",
          "Native block-floating-point (bfp16) multiply-accumulate support">;

// 2048-bit wide accumulator register file.
def FeatureAcc2048
    : SubtargetFeature<"acc2048", "HasAcc2048", "true",
                       "2048-bit wide accumulator register file">;
