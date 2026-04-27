//===- aie2psintrin.h -------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PSINTRIN_H
#define __AIE2PSINTRIN_H

#define restrict __restrict

#define INTRINSIC(TYPE)                                                        \
  static __inline__ TYPE __attribute__((__always_inline__, __nodebug__))
// Parameter sign
#define __SIGN_UNSIGNED 0
#define __SIGN_SIGNED 1

#define MMODE_8x8_1 0
#define MMODE_16x16_1 1
#define MMODE_16x16_2 2
#define MMODE_BF20xBF20_1 3
#define MMODE_32x16_2 4
#define MMODE_BFP13xBFP13_1 5
#define MMODE_BFP16xBFP16_1 6
#define MMODE_BF9xBF9_1 7

#define CMODE_SEL_DONT_CARE 0

#define BFEXT_MODE_DONT_CARE 0
#define BFEXT_MODE_BF20 0
#define BFEXT_MODE_BF9 1

#define CMODE_SEL_FP16 0
#define CMODE_SEL_BF16 1

#define CMODE_SEL_FP8 0
#define CMODE_SEL_BF8 1

#define CMODE_BYPASS 0    /*000*/
#define CMODE_BF20_BF16 1 /*001*/
#define CMODE_BF20_FP16 5 /*101*/
#define CMODE_BF9_FP8 3   /*011*/
#define CMODE_BF9_BF8 7   /*111*/

#include "aiebase_chess.h"
#include "aiebase_typedefs.h"
#ifdef __cplusplus
// clang-format off
#include "aiebase_resources.h"
#include "aie2ps/aie2ps_defines.h"
#include "aie2ps/aie2ps_enums.h"
#include "aie2ps/aie2ps_undef.h"
#include "aie2ps/aie2ps_upd_ext.h"
#include "aie2ps/aie2ps_scl2vec.h"
#include "aie2ps/aie2ps_addlog.h"
#include "aie2ps/aie2ps_locks.h"
#include "aie2ps/aie2ps_undef.h"
#include "aie2ps/aie2ps_upd_ext.h"
#include "aie2ps/aie2ps_scalar.h"
#include "aie2ps/aie2ps_cnvf2f.h"
#include "aie2ps/aie2ps_nlf.h"
#include "aie2ps/aie2ps_set_mode.h"
#include "aie2ps/aie2ps_ups.h"
#include "aie2ps/aie2ps_srs.h"
#include "aie2ps/aie2ps_vmult.h"
#include "aie2ps/aie2ps_vadd.h"
#include "aie2ps/aie2ps_streams.h"
#include "aie2ps/aie2ps_addr.h"
#include "aie2ps/aie2ps_nlf_vector.h"
#include "aie2ps/aie2ps_ldst.h"
#include "aie2ps/aie2ps_aie_api_compat.h"
// clang-format on
#endif /* __cplusplus */

// Locks
INTRINSIC(void)
event0() { return __builtin_aie2ps_event(0); }
INTRINSIC(void)
event1() { return __builtin_aie2ps_event(1); }
INTRINSIC(void)
event_warning() { return __builtin_aie2ps_event(2); }
INTRINSIC(void)
event_error() { return __builtin_aie2ps_event(3); }

// Refer tile_control_aie2gen.h in Vitis TA for TM_address_space_start_addr.
// Read/Write for Tile Memory Map
#if defined(__cplusplus) && !defined(__AIECC__DISABLE_READ_WRITE_TM)
INTRINSIC(uint32) read_tm(uint32 regAddr, uint32 TMAddrSpaceStart = 0x80000) {
  return __builtin_aie2ps_read_tm((int *)(TMAddrSpaceStart + regAddr));
}
INTRINSIC(void)
write_tm(uint32 regVal, uint32 regAddr, uint32 TMAddrSpaceStart = 0x80000) {
  return __builtin_aie2ps_write_tm(regVal, (int *)(TMAddrSpaceStart + regAddr));
}
#endif /* __cplusplus && !(__AIECC__DISABLE_READ_WRITE_TM) */

#if defined(__cplusplus)
extern "C" {
#endif
// FIXME: this belongs to libc's stdio.h
int printf(const char *__restrict, ...);
#if defined(__cplusplus)
}
#endif

// _Float16 math function overloads: resolve ambiguity by promoting to float.
// Without these, calls like std::sqrt(_Float16) are ambiguous between the
// float and long double overloads in libc++.
#if defined(__cplusplus)
namespace std {
inline float sqrt(_Float16 __x) { return __builtin_aie2ps_sqrtf((float)__x); }
} // namespace std
#endif // __cplusplus

#endif /* __AIE2PSINTRIN_H */
