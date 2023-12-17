//===- aiev2intrin.h --------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2INTRIN_H
#define __AIEV2INTRIN_H

#define restrict __restrict

#define INTRINSIC(TYPE)                                                        \
  static __inline__ TYPE __attribute__((__always_inline__, __nodebug__))

// Parameter sign
#define __SIGN_UNSIGNED 0
#define __SIGN_SIGNED 1

#include "aiebase_chess.h"
#include "aiebase_typedefs.h"
#include "aiev2_defines.h"
#include "aiev2_undef.h"

#ifdef __cplusplus
// clang-format off
#include "aiev2_addlog.h"
#include "aiev2_ldst.h"
#include "aiev2_locks.h"
#include "aiev2_pckt_header.h"
#include "aiev2_scl2vec.h"
#include "aiev2_set_mode.h"
#include "aiev2_upd_ext.h"
#include "aiev2_srs.h"
#include "aiev2_ups.h"
#include "aiev2_scalar.h"
#include "aiev2_streams.h"
#include "aiev2_vadd.h"
#include "aiev2_vmult.h"
#include "aiev2_addr.h"
#include "aiev2_core.h"
// clang-format on
// float_to_bfloat16
constexpr inline bfloat16::bfloat16(float a0) {
  uint32_t I32 = __builtin_bit_cast(unsigned int, a0);

  // This is a temporary implementation to make 'normal' constants
  // work. Denormalized input and values rounding up near the extrema of
  // the range might give strange effects

  const bool Rnd = (I32 & 0x8000) != 0;
  const bool Sticky = (I32 & 0x7FFF) != 0;
  const bool Lsb = (I32 & 0x10000) != 0;
  // The tie case (Rnd & !Sticky) should make the result even
  if (Rnd && (Sticky || Lsb)) {
    // FIXME: carry into exp kind of works out, carry into sign does not.
    I32 += 0x10000;
  }

  const uint16_t IBF = I32 >> 16;
  m0 = __builtin_bit_cast(__bf16, IBF);
}
constexpr inline bfloat16::bfloat16(double a0) {
  bfloat16 t = (float)a0;
  m0 = t.m0;
}
constexpr inline bfloat16::bfloat16(int a0) {
  bfloat16 t = (float)a0;
  m0 = t.m0;
}
constexpr inline bfloat16::bfloat16(unsigned int a0) {
  bfloat16 t = (float)a0;
  m0 = t.m0;
}
#endif /* __cplusplus */

// Locks
INTRINSIC(void)
event0() { return __builtin_aiev2_event(0); }
INTRINSIC(void)
event1() { return __builtin_aiev2_event(1); }
INTRINSIC(void)
event_warning() { return __builtin_aiev2_event(2); }
INTRINSIC(void)
event_error() { return __builtin_aiev2_event(3); }

#if defined(__cplusplus) && !defined(__AIECC__DISABLE_READ_WRITE_TM)
// Read/Write for Tile Memory Map
INTRINSIC(uint32) read_tm(uint32 regAddr, uint32 TMAddrSpaceStart = 0x80000) {
  return __builtin_aiev2_read_tm((int *)(TMAddrSpaceStart + regAddr));
}
INTRINSIC(void)
write_tm(uint32 regVal, uint32 regAddr, uint32 TMAddrSpaceStart = 0x80000) {
  return __builtin_aiev2_write_tm(regVal, (int *)(TMAddrSpaceStart + regAddr));
}
#endif /* __cplusplus && !(__AIECC__DISABLE_READ_WRITE_TM) */

#endif /* __AIEV2INTRIN_H */
