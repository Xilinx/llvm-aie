//===- aie2pintrin.h -------------------------------------------*- C++ -*-===//
//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PINTRIN_H
#define __AIE2PINTRIN_H

#define restrict __restrict

#define INTRINSIC(TYPE)                                                        \
  static __inline__ TYPE __attribute__((__always_inline__, __nodebug__))
// Parameter sign
#define __SIGN_UNSIGNED 0
#define __SIGN_SIGNED 1

// Parameter sign
#define __SIGN_UNSIGNED 0
#define __SIGN_SIGNED 1

#include "aie2p/aie2p_enums.h"
#include "aie2p/aie2p_version.h"
#include "aiebase_chess.h"
#include "aiebase_typedefs.h"
#ifdef __cplusplus
#include "aie2p/aie2p_defines.h"
#include "aiebase_resources.h"
// clang-format off
#include "aie2p/aie2p_addlog.h"
#include "aie2p/aie2p_locks.h"
#include "aie2p/aie2p_undef.h"
#include "aie2p/aie2p_upd_ext.h"
#include "aie2p/aie2p_ldst.h"
#include "aie2p/aie2p_scl2vec.h"
#include "aie2p/aie2p_set_mode.h"
#include "aie2p/aie2p_ups.h"
#include "aie2p/aie2p_scalar.h"
#include "aie2p/aie2p_srs.h"
#include "aie2p/aie2p_vmult.h"
#include "aie2p/aie2p_vadd.h"
#include "aie2p/aie2p_addr.h"
#include "aie2p/aie2p_streams.h"
#include "aie2p/aie2p_cnvf2f.h"
#include "aie2p/aie2p_nlf.h"
#include "aie2p/aie2p_nlf_vector.h"
#include "aie2p/aie2p_aie_api_compat.h"
// clang-format on
#endif /* __cplusplus */

// Locks
INTRINSIC(void)
event0() { return __builtin_aie2p_event(0); }
INTRINSIC(void)
event1() { return __builtin_aie2p_event(1); }
INTRINSIC(void)
event_warning() { return __builtin_aie2p_event(2); }
INTRINSIC(void)
event_error() { return __builtin_aie2p_event(3); }

#if defined(__cplusplus) && !defined(__AIECC__DISABLE_READ_WRITE_TM)
// Read/Write for Tile Memory Map
INTRINSIC(uint32) read_tm(uint32 regAddr, uint32 TMAddrSpaceStart = 0x80000) {
  return __builtin_aie2p_read_tm((int *)(TMAddrSpaceStart + regAddr));
}
INTRINSIC(void)
write_tm(uint32 regVal, uint32 regAddr, uint32 TMAddrSpaceStart = 0x80000) {
  return __builtin_aie2p_write_tm(regVal, (int *)(TMAddrSpaceStart + regAddr));
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

#endif /* __AIE2PINTRIN_H */
