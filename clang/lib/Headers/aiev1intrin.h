//===- aiev1intrin.h --------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV1INTRIN_H
#define __AIEV1INTRIN_H

#define restrict __restrict

#define INTRINSIC(TYPE)                                                        \
  static __inline__ TYPE __attribute__((__always_inline__, __nodebug__))

#include "aiebase_chess.h"
#include "aiebase_typedefs.h"
#include "aiev1_undef.h"

#ifdef __cplusplus
// In order to compile compiler-rt, we need a pure-C environment

#include "aiev1_ldst.h"
#include "aiev1_upd_ext.h"
#include "aiev1_vcmp.h"
#include "aiev1_vfloat.h"
#include "aiev1_vmac.h"
#include "aiev1_vmult.h"

#endif /* __cplusplus */

// Locks
INTRINSIC(void)
event0() { return __builtin_aie_event(0); }
INTRINSIC(void)
event1() { return __builtin_aie_event(1); }
INTRINSIC(void)
event_done() {
  // NOTE: There may be some scheduling hazards here
  return __builtin_aie_event(2);
}
INTRINSIC(void)
event_error() { return __builtin_aie_event(3); }
INTRINSIC(void)
done() { return event_done(); }

// Locks
INTRINSIC(void)
acquire(int __a, int __b) { return __builtin_aie_acquire(__a, __b); }
INTRINSIC(void)
release(int __a, int __b) { return __builtin_aie_release(__a, __b); }

// Non-linear functions
/*
@param a    Fixed-point input value
@param sft1 Decimal point of input value
@param sft2 Decimal point of output value
@return     Fixed-point result
*/
INTRINSIC(float)
_sqrtf(float a) { return __builtin_aie_sqrt_flt_flt(a); }
INTRINSIC(int32_t)
sqrt(int32_t a, int32_t sft1, int32_t sft2) {
  return __builtin_aie_sqrt_fix_fix(sft1, a, sft2);
}
INTRINSIC(float)
rsqrtf(float a) { return __builtin_aie_invsqrt_flt_flt(a); }
INTRINSIC(float)
invsqrt(float a) { return __builtin_aie_invsqrt_flt_flt(a); }
INTRINSIC(float)
inv(float a) { return __builtin_aie_inv_flt_flt(a); }

#ifdef __cplusplus
// In order to compile compiler-rt, we need a pure-C environment

INTRINSIC(int32_t)
invsqrt(int32_t a, int32_t sft1, int32_t sft2) {
  return __builtin_aie_invsqrt_fix_fix(sft1, a, sft2);
}
INTRINSIC(int32_t)
inv(int32_t a, int32_t sft1, int32_t sft2) {
  return __builtin_aie_inv_fix_fix(sft1, a, sft2);
}

template <typename T>
T *cyclic_add(T *a, int offs, T *restrict start, int len) {
  return a + offs;
}

//    typedef enum {ss0, ms0, sst0, ssc0, wsst0, msc0,
//              ss1, ms1, sst1, ssc1, wsst1, msc1} __STREAM_NAME;
// typedef __STREAM_NAME ss0_idx_t;
// typedef __STREAM_NAME ms0_idx_t;
// typedef __STREAM_NAME sst0_idx_t;
// typedef __STREAM_NAME ssc0_idx_t;
// typedef __STREAM_NAME wsst0_idx_t;
// typedef __STREAM_NAME msc0_idx_t;
// typedef __STREAM_NAME ss1_idx_t;
// typedef __STREAM_NAME ms1_idx_t;
// typedef __STREAM_NAME sst1_idx_t;
// typedef __STREAM_NAME ssc1_idx_t;
// typedef __STREAM_NAME wsst1_idx_t;
// typedef __STREAM_NAME msc1_idx_t;
//! @}
//! @name Read 32-bit streams
//! @{

/** \brief Read a 32-bit value from input stream port SS0 or SS1
    Intrinsics suffixed with _nb perform a non-blocking read. All others are
   blocking.
*/
INTRINSIC(int32_t) get_ss(int stream) { return __builtin_aie_get_ss(stream); }
INTRINSIC(float) getf_ss(int stream) { return __builtin_aie_getf_ss(stream); }
INTRINSIC(int) get_ss0_tlast() { return __builtin_aie_bitget_mc0(1); }
INTRINSIC(int) get_ss1_tlast() { return __builtin_aie_bitget_mc0(2); }
INTRINSIC(int) get_wss0_tlast() { return __builtin_aie_bitget_mc0(3); }
INTRINSIC(int) get_wss1_tlast() { return __builtin_aie_bitget_mc0(4); }
static v8int16 t;
//! @}
//! @name Read 128-bit streams
//! @{

//! \brief Reads a 128-bit vector from input stream port WSS0 or WSS1
INTRINSIC(v8int16) get_wss(int stream) { return t; }
INTRINSIC(void) dummy(v8int16 x) { t = x; }
INTRINSIC(void) put_ms(int stream, int32_t value) {
  __builtin_aie_put_ms(stream, value);
}
INTRINSIC(void) put_ms(int stream, uint32_t value) {
  __builtin_aie_put_ms(stream, value);
}
INTRINSIC(void) put_ms(int stream, float value) {
  __builtin_aie_putf_ms(stream, value);
}
template <typename T> void put_ms(int stream, T value, bool tlast) {}
INTRINSIC(void) put_wms(int stream, v8int16 value) {}
INTRINSIC(void) put_wms(int stream, v8int16 value, bool tlast) {}
INTRINSIC(v8acc48) get_scd() {
  v8acc48 t;
  return t;
}
// v4cacc48 get_scd() {
//     return 0;
// }
INTRINSIC(void) put_mcd(v8acc48 value) {}

#define AS_VECTOR(ty, ln)                                                      \
  template <typename T> v##ln##ty as_v##ln##ty(T t) {                          \
    v##ln##ty x = t;                                                           \
    return x;                                                                  \
  }

AS_VECTOR(int8, 16);
AS_VECTOR(uint8, 16);
AS_VECTOR(int16, 8);
AS_VECTOR(uint16, 8);
AS_VECTOR(int32, 4);
// AS_VECTOR(cint16, 4);
// AS_VECTOR(cint32, 2);
AS_VECTOR(float, 4);

#undef AS_VECTOR

// ------------------------------------------------------------
//! \addtogroup intr_streams
//! @{
//! @name Packet headers
//! These intrinsics are designed to facilitate the headers creation. They do
//! not write directly to stream and should always be combined with a "put_ms"
//! call.
//! @{

/*! \brief Generate a packet header

  @param dstID unsigned 5 bit (LSB). Identifies the destination of the packet.
  Should be unique amongst all streams it shares ports with. Can be 32
  destinations or more with nested headers.
  @param pcktType unsigned 3 bit (LSB). Can be used to distinguish packets with
  the same source and destination. Control packets set this to 7 if there is an
  error. This *must* be a compile time constant.

  This intrinsic takes a destination ID and a packet type and returns a packet
  header.

  @note Parameter 'pcktType' *must* be a compile time constant.
  @note A packet with nested headers has a number of headers at the beginning
  (instead of the usual single header). For every level of header nesting, the
  number of possible destinations is multiplied by 32, thus for a two-nested
  header approach, the number of destinations is already 1024.
 */
INTRINSIC(uint32_t)
packet_header(unsigned int dstID, unsigned int pcktType) {
  return __builtin_aie_packet_header(dstID, pcktType);
}

/*! \brief Generate a control packet header

  @param Addr unsigned Local Byte Address (two LSB not used). Accesses are 128
  bit aligned.
  @param n_words unsigned 2 bits. 1 to 4 words of 32 bit data to follow. This
  *must* be a compile time constant.
  @param op_type unsigned Operation modes - 2 bit (LSB). 3 modes. 0 - Write
  without return, 1 - Read with return, 2 - Write with return. The response to a
  read will be a packet switched stream header and 1 to 4 32 bit word data. The
  response to a write will be a packet switched stream header.  This *must* be a
  compile time constant.
  @param rspID unsigned Response ID: 5 bit (LSB). Identifies the return
  destination for operation modes with a return. (The return packet's header's
  'dstID' is set to this).

  This intrinsic takes an address, number of words, operation type and response
  ID and returns a control packet header.

  @note Parameters 'n_words' and 'op_type' *must* be compile time constants.
 */
INTRINSIC(uint32_t)
ctrl_packet_header(unsigned int addr, unsigned int numWords, unsigned int op,
                   unsigned int returnID) {
  return __builtin_aie_ctrl_packet_header(addr, numWords, op, returnID);
}

/* Return src[idx] */
INTRINSIC(uint32_t) bitget(uint32_t src, uint32_t idx) {
  return __builtin_aie_bitget(src, idx);
}

/* return dst[idx] = src[0] */
INTRINSIC(uint32_t) bitset(uint32_t dst, uint32_t src, uint32_t idx) {
  return __builtin_aie_bitset(dst, src, idx);
}

// srs
INTRINSIC(v16int8) bsrs(v16acc48 bm, int shft) {
  return __builtin_aie_bsrs_v16i8(bm, shft);
}
INTRINSIC(v16uint8) ubsrs(v16acc48 bm, int shft) {
  return __builtin_aie_ubsrs_v16i8(bm, shft);
}

#endif /* __cplusplus */

#endif /* __AIEV1INTRIN_H */
