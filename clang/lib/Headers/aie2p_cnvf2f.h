//===-------------------- AIEngine AIE2p intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2P_CNVF2F_H__
#define __AIE2P_CNVF2F_H__

#define DIAGNOSE_CNVF2F_SFT                                                    \
  __attribute__((diagnose_if(sft < -32 || sft > 31,                            \
                             "sft argument must be in the range [-32:31]",     \
                             "error")))

INTRINSIC(float) fix2float(int n, int sft) DIAGNOSE_CNVF2F_SFT {
  return __builtin_aie2p_fx2flt(n, sft);
}

INTRINSIC(float) fix2float(int n) { return fix2float(n, 0); }

INTRINSIC(int) float2fix(float n, int sft) DIAGNOSE_CNVF2F_SFT {
  return __builtin_aie2p_flt2fx(n, sft);
}

INTRINSIC(int) float2fix(float n) { return float2fix(n, 0); }

#endif // __AIE2P_CNVF2F_H__
