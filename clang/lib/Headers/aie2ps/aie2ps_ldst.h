//===-------------------- AIEngine AIE2ps intrinsics -----------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef AIE2PS_LDST_H
#define AIE2PS_LDST_H

//* Automatically generated file, do not edit! *
//
typedef uint2_t crsat_t;

INTRINSIC(v32int8) pack(v32int16 v, int sign) {
  return __builtin_aie2ps_pack_I512_I8_I16(v, sign);
}
INTRINSIC(v32int8) pack_conf(v32int16 v, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32int8 val = pack(v, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32int8) pack(v32int16 v) { return pack(v, __SIGN_SIGNED); }
INTRINSIC(v32int8) pack_conf(v32int16 v, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32int8 val = pack(v, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32uint8) pack(v32uint16 v, int sign) {
  return __builtin_aie2ps_pack_I512_I8_I16(v, sign);
}
INTRINSIC(v32uint8) pack_conf(v32uint16 v, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32uint8 val = pack(v, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v32uint8) pack(v32uint16 v) { return pack(v, __SIGN_UNSIGNED); }
INTRINSIC(v32uint8) pack_conf(v32uint16 v, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v32uint8 val = pack(v, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64int4) pack(v64int8 v, int sign) {
  return __builtin_aie2ps_pack_I512_I4_I8(v, sign);
}
INTRINSIC(v64int4) pack_conf(v64int8 v, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64int4 val = pack(v, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64int4) pack(v64int8 v) { return pack(v, __SIGN_SIGNED); }
INTRINSIC(v64int4) pack_conf(v64int8 v, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64int4 val = pack(v, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64uint4) pack(v64uint8 v, int sign) {
  return __builtin_aie2ps_pack_I512_I4_I8(v, sign);
}
INTRINSIC(v64uint4) pack_conf(v64uint8 v, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64uint4 val = pack(v, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64uint4) pack(v64uint8 v) { return pack(v, __SIGN_UNSIGNED); }
INTRINSIC(v64uint4) pack_conf(v64uint8 v, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64uint4 val = pack(v, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64int8) pack(v64int16 v, int sign) {
  return __builtin_aie2ps_pack_I1024_I8_I16(v, sign);
}
INTRINSIC(v64int8) pack_conf(v64int16 v, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64int8 val = pack(v, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64int8) pack(v64int16 v) { return pack(v, __SIGN_SIGNED); }
INTRINSIC(v64int8) pack_conf(v64int16 v, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64int8 val = pack(v, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64uint8) pack(v64uint16 v, int sign) {
  return __builtin_aie2ps_pack_I1024_I8_I16(v, sign);
}
INTRINSIC(v64uint8) pack_conf(v64uint16 v, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64uint8 val = pack(v, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v64uint8) pack(v64uint16 v) { return pack(v, __SIGN_UNSIGNED); }
INTRINSIC(v64uint8) pack_conf(v64uint16 v, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v64uint8 val = pack(v, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v128int4) pack(v128int8 v, int sign) {
  return __builtin_aie2ps_pack_I1024_I4_I8(v, sign);
}
INTRINSIC(v128int4) pack_conf(v128int8 v, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v128int4 val = pack(v, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v128int4) pack(v128int8 v) { return pack(v, __SIGN_SIGNED); }
INTRINSIC(v128int4) pack_conf(v128int8 v, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v128int4 val = pack(v, __SIGN_SIGNED);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v128uint4) pack(v128uint8 v, int sign) {
  return __builtin_aie2ps_pack_I1024_I4_I8(v, sign);
}
INTRINSIC(v128uint4) pack_conf(v128uint8 v, int sign, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v128uint4 val = pack(v, sign);
  set_satmode(prev_sat);
  return val;
}
INTRINSIC(v128uint4) pack(v128uint8 v) { return pack(v, __SIGN_UNSIGNED); }
INTRINSIC(v128uint4) pack_conf(v128uint8 v, crsat_t sat) {
  crsat_t prev_sat = get_sat();
  set_satmode(sat);
  v128uint4 val = pack(v, __SIGN_UNSIGNED);
  set_satmode(prev_sat);
  return val;
}

INTRINSIC(v32int16) unpack(v32int8 v, uint1_t sign) {
  return __builtin_aie2ps_unpack_I512_I16_I8(v, sign);
}
INTRINSIC(v32int16) unpack(v32int8 v) { return unpack(v, __SIGN_SIGNED); }
INTRINSIC(v32uint16) unpack(v32uint8 v, uint1_t sign) {
  return __builtin_aie2ps_unpack_I512_I16_I8(v, sign);
}
INTRINSIC(v32uint16) unpack(v32uint8 v) { return unpack(v, __SIGN_UNSIGNED); }
INTRINSIC(v64int8) unpack(v64int4 v, uint1_t sign) {
  return __builtin_aie2ps_unpack_I512_I8_I4(v, sign);
}
INTRINSIC(v64int8) unpack(v64int4 v) { return unpack(v, __SIGN_SIGNED); }
INTRINSIC(v64uint8) unpack(v64uint4 v, uint1_t sign) {
  return __builtin_aie2ps_unpack_I512_I8_I4(v, sign);
}
INTRINSIC(v64uint8) unpack(v64uint4 v) { return unpack(v, __SIGN_UNSIGNED); }
INTRINSIC(v64int16) unpack(v64int8 v, uint1_t sign) {
  return __builtin_aie2ps_unpack_I1024_I16_I8(v, sign);
}
INTRINSIC(v64int16) unpack(v64int8 v) { return unpack(v, __SIGN_SIGNED); }
INTRINSIC(v64uint16) unpack(v64uint8 v, uint1_t sign) {
  return __builtin_aie2ps_unpack_I1024_I16_I8(v, sign);
}
INTRINSIC(v64uint16) unpack(v64uint8 v) { return unpack(v, __SIGN_UNSIGNED); }
INTRINSIC(v128int8) unpack(v128int4 v, uint1_t sign) {
  return __builtin_aie2ps_unpack_I1024_I8_I4(v, sign);
}
INTRINSIC(v128int8) unpack(v128int4 v) { return unpack(v, __SIGN_SIGNED); }
INTRINSIC(v128uint8) unpack(v128uint4 v, uint1_t sign) {
  return __builtin_aie2ps_unpack_I1024_I8_I4(v, sign);
}
INTRINSIC(v128uint8) unpack(v128uint4 v) { return unpack(v, __SIGN_UNSIGNED); }

#define _FIFO_FILL(T, P, loc, RESTRICT)                                        \
  INTRINSIC(void) fifo_ld_reset(P loc *RESTRICT &p, fifo_state_t &s) {         \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_ld_fill((void loc *RESTRICT &)p, fifo, s.pos);       \
  }                                                                            \
  INTRINSIC(void) fifo_ld_fill(P loc *RESTRICT &p, fifo_state_t &s) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_ld_fill((void loc *RESTRICT &)p, fifo, pos);         \
  }

#define _FIFO2(T, P, loc, RESTRICT)                                            \
  _FIFO_FILL(T, P, loc, RESTRICT)                                              \
  INTRINSIC(T) fifo_ld_pop(P loc *RESTRICT &p, fifo_state_t &s) {              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = (T)__builtin_aie2ps_fifo_ld_pop_512_unaligned(                       \
        (void loc *RESTRICT &)p, fifo, pos);                                   \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_1d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = (T)__builtin_aie2ps_fifo_ld_pop_1d_512_unaligned(                    \
        (void loc *RESTRICT &)p, fifo, pos, off);                              \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_2d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off, int size1, \
                      addr_t &count1, int inc1) {                              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = (T)__builtin_aie2ps_fifo_ld_pop_2d_512_unaligned(                    \
        (void loc *RESTRICT &)p, fifo, pos, off, size1, count1, inc1);         \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_3d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off, int size1, \
                      addr_t &count1, int inc1, int size2, addr_t &count2,     \
                      int inc2) {                                              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = (T)__builtin_aie2ps_fifo_ld_pop_3d_512_unaligned(                    \
        (void loc *RESTRICT &)p, fifo, pos, off, size1, count1, inc1, size2,   \
        count2, inc2);                                                         \
    return r;                                                                  \
  }

#define _FIFO2_FP8(T, P, loc, RESTRICT)                                        \
  _FIFO_FILL(T, P, loc, RESTRICT)                                              \
  INTRINSIC(T) fifo_ld_pop(P loc *RESTRICT &p, fifo_state_t &s) {              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = {__builtin_aie2ps_fifo_ld_pop_512_unaligned((void loc *RESTRICT &)p, \
                                                      fifo, pos)};             \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_1d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = {__builtin_aie2ps_fifo_ld_pop_1d_512_unaligned(                      \
        (void loc *RESTRICT &)p, fifo, pos, off)};                             \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_2d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off, int size1, \
                      addr_t &count1, int inc1) {                              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = {__builtin_aie2ps_fifo_ld_pop_2d_512_unaligned(                      \
        (void loc *RESTRICT &)p, fifo, pos, off, size1, count1, inc1)};        \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_3d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off, int size1, \
                      addr_t &count1, int inc1, int size2, addr_t &count2,     \
                      int inc2) {                                              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = {__builtin_aie2ps_fifo_ld_pop_3d_512_unaligned(                      \
        (void loc *RESTRICT &)p, fifo, pos, off, size1, count1, inc1, size2,   \
        count2, inc2)};                                                        \
    return r;                                                                  \
  }

#define _FIFO2_BFP(T, P, loc, RESTRICT, SIZE, ...)                             \
  INTRINSIC(void) fifo_ld_reset(P loc *RESTRICT &p, fifo_state_t &s) {         \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_ld_fill((void loc *RESTRICT &)p, fifo, s.pos);       \
  }                                                                            \
  INTRINSIC(void) fifo_ld_fill(P loc *RESTRICT &p, fifo_state_t &s) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_ld_fill((void loc *RESTRICT &)p, fifo, pos);         \
  }                                                                            \
  INTRINSIC(T) fifo_ld_pop(P loc *RESTRICT &p, fifo_state_t &s) {              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r;                                                                       \
    __builtin_aie2ps_fifo_ld_pop_BFP##SIZE((void loc *RESTRICT &)p, fifo, pos, \
                                           ##__VA_ARGS__);                     \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_1d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r;                                                                       \
    __builtin_aie2ps_fifo_ld_pop_1d_BFP##SIZE((void loc *RESTRICT &)p, fifo,   \
                                              pos, off, ##__VA_ARGS__);        \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_2d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off, int size1, \
                      addr_t &count1, int inc1) {                              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r;                                                                       \
    __builtin_aie2ps_fifo_ld_pop_2d_BFP##SIZE((void loc *RESTRICT &)p, fifo,   \
                                              pos, off, size1, count1, inc1,   \
                                              ##__VA_ARGS__);                  \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_3d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off, int size1, \
                      addr_t &count1, int inc1, int size2, addr_t &count2,     \
                      int inc2) {                                              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r;                                                                       \
    __builtin_aie2ps_fifo_ld_pop_3d_BFP##SIZE(                                 \
        (void loc *RESTRICT &)p, fifo, pos, off, size1, count1, inc1, size2,   \
        count2, inc2, ##__VA_ARGS__);                                          \
    return r;                                                                  \
  }

#define _FIFO_FILLX(T, P, loc, RESTRICT)                                       \
  INTRINSIC(void)                                                              \
  fifo_ld_fillx(P loc *RESTRICT &p, fifo_state_t &s, int step, int mask) {     \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    v16int32 &extra = s.extra;                                                 \
    int conf = (step << 6) | mask;                                             \
    __builtin_aie2ps_fifo_ld_fillx((void loc *RESTRICT &)p, fifo, pos, extra,  \
                                   conf);                                      \
  }                                                                            \
  INTRINSIC(void) fifo_ld_fillx(P loc *RESTRICT &p, fifo_state_t &s) {         \
    return fifo_ld_fillx(p, s, 31, 31);                                        \
  }

#define _FIFO_POPX(T, P, loc, RESTRICT)                                        \
  INTRINSIC(T)                                                                 \
  fifo_ld_popx(P loc *RESTRICT &p, fifo_state_t &s, int step, int mask) {      \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    int conf = (step << 6) | mask;                                             \
    v16int32 &extra = s.extra;                                                 \
    T r = (T)__builtin_aie2ps_fifo_ld_popx((void loc *RESTRICT &)p, fifo, pos, \
                                           extra, conf);                       \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T) fifo_ld_popx(P loc *RESTRICT &p, fifo_state_t &s) {             \
    return fifo_ld_popx(p, s, 31, 31);                                         \
  }

#define _FIFO_POPX_FP8(T, P, loc, RESTRICT)                                    \
  INTRINSIC(T)                                                                 \
  fifo_ld_popx(P loc *RESTRICT &p, fifo_state_t &s, int step, int mask) {      \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    int conf = (step << 6) | mask;                                             \
    v16int32 &extra = s.extra;                                                 \
    T r = {__builtin_aie2ps_fifo_ld_popx((void loc *RESTRICT &)p, fifo, pos,   \
                                         extra, conf)};                        \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T) fifo_ld_popx(P loc *RESTRICT &p, fifo_state_t &s) {             \
    return fifo_ld_popx(p, s, 31, 31);                                         \
  }

#define _FIFO_BASIC(T, P, RESTRICT)                                            \
  _FIFO2(T, P, , RESTRICT)                                                     \
  _FIFO2(T, P, __aie_dm_resource_a, RESTRICT)                                  \
  _FIFO2(T, P, __aie_dm_resource_b, RESTRICT)                                  \
  _FIFO2(T, P, __aie_dm_resource_c, RESTRICT)                                  \
  _FIFO2(T, P, __aie_dm_resource_d, RESTRICT)                                  \
  _FIFO2(T, P, __aie_dm_resource_ab, RESTRICT)                                 \
  _FIFO2(T, P, __aie_dm_resource_ac, RESTRICT)                                 \
  _FIFO2(T, P, __aie_dm_resource_ad, RESTRICT)                                 \
  _FIFO2(T, P, __aie_dm_resource_bc, RESTRICT)                                 \
  _FIFO2(T, P, __aie_dm_resource_bd, RESTRICT)                                 \
  _FIFO2(T, P, __aie_dm_resource_cd, RESTRICT)

#define _FIFO_BASIC_FP8(T, P, RESTRICT)                                        \
  _FIFO2_FP8(T, P, , RESTRICT)                                                 \
  _FIFO2_FP8(T, P, __aie_dm_resource_a, RESTRICT)                              \
  _FIFO2_FP8(T, P, __aie_dm_resource_b, RESTRICT)                              \
  _FIFO2_FP8(T, P, __aie_dm_resource_c, RESTRICT)                              \
  _FIFO2_FP8(T, P, __aie_dm_resource_d, RESTRICT)                              \
  _FIFO2_FP8(T, P, __aie_dm_resource_ab, RESTRICT)                             \
  _FIFO2_FP8(T, P, __aie_dm_resource_ac, RESTRICT)                             \
  _FIFO2_FP8(T, P, __aie_dm_resource_ad, RESTRICT)                             \
  _FIFO2_FP8(T, P, __aie_dm_resource_bc, RESTRICT)                             \
  _FIFO2_FP8(T, P, __aie_dm_resource_bd, RESTRICT)                             \
  _FIFO2_FP8(T, P, __aie_dm_resource_cd, RESTRICT)

#define _FIFO_BFP(T, P, RESTRICT, SIZE, ...)                                   \
  _FIFO2_BFP(T, P, , RESTRICT, SIZE, ##__VA_ARGS__)                            \
  _FIFO2_BFP(T, P, __aie_dm_resource_a, RESTRICT, SIZE, ##__VA_ARGS__)         \
  _FIFO2_BFP(T, P, __aie_dm_resource_b, RESTRICT, SIZE, ##__VA_ARGS__)         \
  _FIFO2_BFP(T, P, __aie_dm_resource_c, RESTRICT, SIZE, ##__VA_ARGS__)         \
  _FIFO2_BFP(T, P, __aie_dm_resource_d, RESTRICT, SIZE, ##__VA_ARGS__)         \
  _FIFO2_BFP(T, P, __aie_dm_resource_ab, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(T, P, __aie_dm_resource_ac, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(T, P, __aie_dm_resource_ad, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(T, P, __aie_dm_resource_bc, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(T, P, __aie_dm_resource_bd, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(T, P, __aie_dm_resource_cd, RESTRICT, SIZE, ##__VA_ARGS__)

#define _FIFO_BASIC_FILLX(T, P, RESTRICT, SIZE, ...)                           \
  _FIFO_BFP(T, P, RESTRICT, SIZE, ##__VA_ARGS__)                               \
  _FIFO_FILLX(T, P, , RESTRICT)                                                \
  _FIFO_FILLX(T, P, __aie_dm_resource_a, RESTRICT)                             \
  _FIFO_FILLX(T, P, __aie_dm_resource_b, RESTRICT)                             \
  _FIFO_FILLX(T, P, __aie_dm_resource_c, RESTRICT)                             \
  _FIFO_FILLX(T, P, __aie_dm_resource_d, RESTRICT)                             \
  _FIFO_FILLX(T, P, __aie_dm_resource_ab, RESTRICT)                            \
  _FIFO_FILLX(T, P, __aie_dm_resource_ac, RESTRICT)                            \
  _FIFO_FILLX(T, P, __aie_dm_resource_ad, RESTRICT)                            \
  _FIFO_FILLX(T, P, __aie_dm_resource_bc, RESTRICT)                            \
  _FIFO_FILLX(T, P, __aie_dm_resource_bd, RESTRICT)                            \
  _FIFO_FILLX(T, P, __aie_dm_resource_cd, RESTRICT)

#define _FIFO_BASIC_POPX(T, P, RESTRICT)                                       \
  _FIFO_BASIC(T, P, RESTRICT)                                                  \
  _FIFO_POPX(T, P, , RESTRICT)                                                 \
  _FIFO_POPX(T, P, __aie_dm_resource_a, RESTRICT)                              \
  _FIFO_POPX(T, P, __aie_dm_resource_b, RESTRICT)                              \
  _FIFO_POPX(T, P, __aie_dm_resource_c, RESTRICT)                              \
  _FIFO_POPX(T, P, __aie_dm_resource_d, RESTRICT)                              \
  _FIFO_POPX(T, P, __aie_dm_resource_ab, RESTRICT)                             \
  _FIFO_POPX(T, P, __aie_dm_resource_ac, RESTRICT)                             \
  _FIFO_POPX(T, P, __aie_dm_resource_ad, RESTRICT)                             \
  _FIFO_POPX(T, P, __aie_dm_resource_bc, RESTRICT)                             \
  _FIFO_POPX(T, P, __aie_dm_resource_bd, RESTRICT)                             \
  _FIFO_POPX(T, P, __aie_dm_resource_cd, RESTRICT)

#define _FIFO_BASIC_POPX_FP8(T, P, RESTRICT)                                   \
  _FIFO_BASIC_FP8(T, P, RESTRICT)                                              \
  _FIFO_POPX_FP8(T, P, , RESTRICT)                                             \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_a, RESTRICT)                          \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_b, RESTRICT)                          \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_c, RESTRICT)                          \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_d, RESTRICT)                          \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_ab, RESTRICT)                         \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_ac, RESTRICT)                         \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_ad, RESTRICT)                         \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_bc, RESTRICT)                         \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_bd, RESTRICT)                         \
  _FIFO_POPX_FP8(T, P, __aie_dm_resource_cd, RESTRICT)

_FIFO_BASIC_POPX(v128int4, v128int4, )
_FIFO_BASIC_POPX(v128uint4, v128uint4, )
_FIFO_BASIC_POPX(v64int8, v64int8, )
_FIFO_BASIC_POPX(v64uint8, v64uint8, )
_FIFO_BASIC_POPX(v32int16, v32int16, )
_FIFO_BASIC_POPX(v32uint16, v32uint16, )
_FIFO_BASIC_POPX(v16int32, v16int32, )
_FIFO_BASIC_POPX(v16uint32, v16uint32, )
_FIFO_BASIC_POPX_FP8(v64bfloat8, v64bfloat8, )
_FIFO_BASIC_POPX_FP8(v64float8, v64float8, )
// restrict
_FIFO_BASIC_POPX(v128int4, v128int4, restrict)
_FIFO_BASIC_POPX(v128uint4, v128uint4, restrict)
_FIFO_BASIC_POPX(v64int8, v64int8, restrict)
_FIFO_BASIC_POPX(v64uint8, v64uint8, restrict)
_FIFO_BASIC_POPX(v32int16, v32int16, restrict)
_FIFO_BASIC_POPX(v32uint16, v32uint16, restrict)
_FIFO_BASIC_POPX(v16int32, v16int32, restrict)
_FIFO_BASIC_POPX(v16uint32, v16uint32, restrict)
_FIFO_BASIC_POPX_FP8(v64bfloat8, v64bfloat8, restrict)
_FIFO_BASIC_POPX_FP8(v64float8, v64float8, restrict)

_FIFO_BASIC_POPX(v32float16, v32float16, )
_FIFO_BASIC_POPX(v32bfloat16, v32bfloat16, )
_FIFO_BASIC_POPX(v16float, v16float, )

_FIFO_BASIC_POPX(v32float16, v32float16, restrict)
_FIFO_BASIC_POPX(v32bfloat16, v32bfloat16, restrict)
_FIFO_BASIC_POPX(v16float, v16float, restrict)

#if 0
_FIFO_BASIC_POPX(v16cint16, v16cint16,)
_FIFO_BASIC_POPX( v8cint32,  v8cint32,)
_FIFO_BASIC_POPX(v16cint16, v16cint16, restrict)
_FIFO_BASIC_POPX( v8cint32,  v8cint32, restrict)

_FIFO_BASIC_POPX(    v8cfloat,    v8cfloat,)
_FIFO_BASIC_POPX(v16cbfloat16,v16cbfloat16,)
_FIFO_BASIC_POPX(    v8cfloat,    v8cfloat, restrict)
_FIFO_BASIC_POPX(v16cbfloat16,v16cbfloat16, restrict)

_FIFO_BASIC(v256uint4_sparse,      v256uint4_sparse_unaligned,)
_FIFO_BASIC(v128uint8_sparse,      v128uint8_sparse_unaligned,)
_FIFO_BASIC(v64uint16_sparse,      v64uint16_sparse_unaligned,)

_FIFO_BASIC(v256int4_sparse,       v256int4_sparse_unaligned,)
_FIFO_BASIC(v128int8_sparse,       v128int8_sparse_unaligned,)
_FIFO_BASIC(v64int16_sparse,       v64int16_sparse_unaligned,)

_FIFO_BASIC(v64bfloat16_sparse, v64bfloat16_sparse_unaligned,)
_FIFO_BASIC(v64float16_sparse,   v64float16_sparse_unaligned,)
_FIFO_BASIC(v128bfloat8_sparse, v128bfloat8_sparse_unaligned,)
_FIFO_BASIC(v128float8_sparse,   v128float8_sparse_unaligned,)

_FIFO_BASIC(v256uint4_sparse,      v256uint4_sparse_unaligned, restrict)
_FIFO_BASIC(v128uint8_sparse,      v128uint8_sparse_unaligned, restrict)
_FIFO_BASIC(v64uint16_sparse,      v64uint16_sparse_unaligned, restrict)

_FIFO_BASIC(v256int4_sparse,       v256int4_sparse_unaligned, restrict)
_FIFO_BASIC(v128int8_sparse,       v128int8_sparse_unaligned, restrict)
_FIFO_BASIC(v64int16_sparse,       v64int16_sparse_unaligned, restrict)

_FIFO_BASIC(v64bfloat16_sparse, v64bfloat16_sparse_unaligned, restrict)
_FIFO_BASIC(v64float16_sparse,   v64float16_sparse_unaligned, restrict)
_FIFO_BASIC(v128bfloat8_sparse, v128bfloat8_sparse_unaligned, restrict)
_FIFO_BASIC(v128float8_sparse,   v128float8_sparse_unaligned, restrict)
#endif

_FIFO_BASIC_FILLX(v64mx9, v64mx9_unaligned, , 640, (v16int32 &)r.mantissa,
                  (v2int32 &)r.tileShift, (v2int32 &)r.exponent)
_FIFO_BASIC_FILLX(v128mx6, v128mx6_unaligned, , 768, (v8int32 &)r.mantissaX0,
                  (v8int32 &)r.mantissaX1, (v2int32 &)r.signF0,
                  (v2int32 &)r.signF1, (int32 &)r.tileShiftG0,
                  (int32 &)r.tileShiftG1, (int32 &)r.exponentE0,
                  (int32 &)r.exponentE1)
_FIFO_BASIC_FILLX(v128mx4, v128mx4_unaligned, , 768, (v8int32 &)r.mantissaX0,
                  (v8int32 &)r.mantissaX1, (v2int32 &)r.signF0,
                  (v2int32 &)r.signF1, (int32 &)r.tileShiftG0,
                  (int32 &)r.tileShiftG1, (int32 &)r.exponentE0,
                  (int32 &)r.exponentE1)

_FIFO_BASIC_FILLX(v64mx9, v64mx9_unaligned, restrict, 640,
                  (v16int32 &)r.mantissa, (v2int32 &)r.tileShift,
                  (v2int32 &)r.exponent)
_FIFO_BASIC_FILLX(v128mx6, v128mx6_unaligned, restrict, 768,
                  (v8int32 &)r.mantissaX0, (v8int32 &)r.mantissaX1,
                  (v2int32 &)r.signF0, (v2int32 &)r.signF1,
                  (int32 &)r.tileShiftG0, (int32 &)r.tileShiftG1,
                  (int32 &)r.exponentE0, (int32 &)r.exponentE1)
_FIFO_BASIC_FILLX(v128mx4, v128mx4_unaligned, restrict, 768,
                  (v8int32 &)r.mantissaX0, (v8int32 &)r.mantissaX1,
                  (v2int32 &)r.signF0, (v2int32 &)r.signF1,
                  (int32 &)r.tileShiftG0, (int32 &)r.tileShiftG1,
                  (int32 &)r.exponentE0, (int32 &)r.exponentE1)

#undef _FIFO_BASIC_FILLX
#undef _FIFO_BASIC_POPX
#undef _FIFO_BASIC_POPX_FP8
#undef _FIFO_BASIC
#undef _FIFO_BASIC_FP8
#undef _FIFO_POPX
#undef _FIFO_POPX_FP8
#undef _FIFO2
#undef _FIFO2_FP8
#undef _FIFO_BFP
#undef _FIFO2_BFP
#undef _FIFO_FILL

#define _FIFO2(T, P1, P2, loc, RESTRICT)                                       \
  INTRINSIC(void) fifo_ld_reset(P1 loc *RESTRICT &p, fifo_state_t &fifo) {     \
    P2 loc *RESTRICT &q = (P2 loc * RESTRICT &)p;                              \
    fifo_ld_reset(q, fifo);                                                    \
  }                                                                            \
  INTRINSIC(void) fifo_ld_fill(P1 loc *RESTRICT &p, fifo_state_t &fifo) {      \
    P2 loc *RESTRICT &q = (P2 loc * RESTRICT &)p;                              \
    fifo_ld_fill(q, fifo);                                                     \
  }                                                                            \
  INTRINSIC(T) fifo_ld_pop(P1 loc *RESTRICT &p, fifo_state_t &fifo) {          \
    P2 loc *RESTRICT &q = (P2 loc * RESTRICT &)p;                              \
    T v;                                                                       \
    v = set_##T(0, fifo_ld_pop(q, fifo));                                      \
    v = update(v, 1, fifo_ld_pop(q, fifo));                                    \
    p = (P1 loc *)q;                                                           \
    return v;                                                                  \
  }

#define _FIFO(a, b, c, RESTRICT)                                               \
  _FIFO2(a, b, c, , RESTRICT)                                                  \
  _FIFO2(a, b, c, __aie_dm_resource_a, RESTRICT)                               \
  _FIFO2(a, b, c, __aie_dm_resource_b, RESTRICT)                               \
  _FIFO2(a, b, c, __aie_dm_resource_c, RESTRICT)                               \
  _FIFO2(a, b, c, __aie_dm_resource_d, RESTRICT)                               \
  _FIFO2(a, b, c, __aie_dm_resource_ab, RESTRICT)                              \
  _FIFO2(a, b, c, __aie_dm_resource_ac, RESTRICT)                              \
  _FIFO2(a, b, c, __aie_dm_resource_ad, RESTRICT)                              \
  _FIFO2(a, b, c, __aie_dm_resource_bc, RESTRICT)                              \
  _FIFO2(a, b, c, __aie_dm_resource_bd, RESTRICT)                              \
  _FIFO2(a, b, c, __aie_dm_resource_cd, RESTRICT)

#if 0
_FIFO(v512uint4_sparse,      v512uint4_sparse_unaligned,      v256uint4_sparse_unaligned,)
_FIFO(v256uint8_sparse,      v256uint8_sparse_unaligned,      v128uint8_sparse_unaligned,)
_FIFO(v128uint16_sparse,     v128uint16_sparse_unaligned,     v64uint16_sparse_unaligned,)

_FIFO(v512int4_sparse,       v512int4_sparse_unaligned,       v256int4_sparse_unaligned,)
_FIFO(v256int8_sparse,       v256int8_sparse_unaligned,       v128int8_sparse_unaligned,)
_FIFO(v128int16_sparse,      v128int16_sparse_unaligned,      v64int16_sparse_unaligned,)

_FIFO(v128bfloat16_sparse,   v128bfloat16_sparse_unaligned,   v64bfloat16_sparse_unaligned,)
_FIFO(v128float16_sparse,    v128float16_sparse_unaligned,    v64float16_sparse_unaligned,)

_FIFO(v256bfloat8_sparse,     v256bfloat8_sparse_unaligned,      v128bfloat8_sparse_unaligned,)
_FIFO(v256float8_sparse,       v256float8_sparse_unaligned,       v128float8_sparse_unaligned,)

_FIFO(v512uint4_sparse,      v512uint4_sparse_unaligned,      v256uint4_sparse_unaligned, restrict)
_FIFO(v256uint8_sparse,      v256uint8_sparse_unaligned,      v128uint8_sparse_unaligned, restrict)
_FIFO(v128uint16_sparse,     v128uint16_sparse_unaligned,     v64uint16_sparse_unaligned, restrict)

_FIFO(v512int4_sparse,       v512int4_sparse_unaligned,       v256int4_sparse_unaligned, restrict)
_FIFO(v256int8_sparse,       v256int8_sparse_unaligned,       v128int8_sparse_unaligned, restrict)
_FIFO(v128int16_sparse,      v128int16_sparse_unaligned,      v64int16_sparse_unaligned, restrict)

_FIFO(v128bfloat16_sparse,   v128bfloat16_sparse_unaligned,   v64bfloat16_sparse_unaligned, restrict)
_FIFO(v128float16_sparse,    v128float16_sparse_unaligned,    v64float16_sparse_unaligned, restrict)

_FIFO(v256bfloat8_sparse,     v256bfloat8_sparse_unaligned,      v128bfloat8_sparse_unaligned, restrict)
_FIFO(v256float8_sparse,       v256float8_sparse_unaligned,       v128float8_sparse_unaligned, restrict)
#endif
//_FIFO(v128bfp16,        v128bfp16ebs16_unaligned, v64bfp16ebs16_unaligned,)
//_FIFO(v128bfp16,        v128bfp16ebs16_unaligned, v64bfp16ebs16_unaligned,
// restrict)
_FIFO(v256mx6, v256mx6_unaligned, v128mx6_unaligned, )
_FIFO(v256mx4, v256mx4_unaligned, v128mx4_unaligned, )

_FIFO(v256mx6, v256mx6_unaligned, v128mx6_unaligned, restrict)
_FIFO(v256mx4, v256mx4_unaligned, v128mx4_unaligned, restrict)

#undef _FIFO
#undef _FIFO2

#define _FIFO_PUSH(T, P, loc, RESTRICT)                                        \
  INTRINSIC(void)                                                              \
  fifo_st_reset(P loc *RESTRICT p, T v, P loc *RESTRICT &p_o,                  \
                sparse_fifo_t &fifo_o, int &pos_o) {                           \
    pos_o = 0;                                                                 \
    sparse_fifo_t fifo;                                                        \
    __builtin_aie2ps_fifo_st_push_512((void loc *RESTRICT &)p, v, fifo,        \
                                      pos_o);                                  \
    p_o = p;                                                                   \
    fifo_o = fifo;                                                             \
  }                                                                            \
  INTRINSIC(void) fifo_st_reset(P loc *RESTRICT &p, T v, fifo_state_t &s) {    \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_push_512((void loc *RESTRICT &)p, v, fifo,        \
                                      s.pos);                                  \
  }                                                                            \
  INTRINSIC(void) fifo_st_push(P loc *RESTRICT &p, T v, fifo_state_t &s) {     \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_push_512((void loc *RESTRICT &)p, v, fifo, pos);  \
  }

#define _FIFO_PUSH_FP8(T, P, loc, RESTRICT)                                    \
  INTRINSIC(void)                                                              \
  fifo_st_reset(P loc *RESTRICT p, T v, P loc *RESTRICT &p_o,                  \
                sparse_fifo_t &fifo_o, int &pos_o) {                           \
    pos_o = 0;                                                                 \
    sparse_fifo_t fifo;                                                        \
    __builtin_aie2ps_fifo_st_push_512((void loc *RESTRICT &)p, v.data, fifo,   \
                                      pos_o);                                  \
    p_o = p;                                                                   \
    fifo_o = fifo;                                                             \
  }                                                                            \
  INTRINSIC(void) fifo_st_reset(P loc *RESTRICT &p, T v, fifo_state_t &s) {    \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_push_512((void loc *RESTRICT &)p, v.data, fifo,   \
                                      s.pos);                                  \
  }                                                                            \
  INTRINSIC(void) fifo_st_push(P loc *RESTRICT &p, T v, fifo_state_t &s) {     \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_push_512((void loc *RESTRICT &)p, v.data, fifo,   \
                                      pos);                                    \
  }

#define _FIFO_PUSH_BFP(T, P, loc, RESTRICT, SIZE, ...)                         \
  INTRINSIC(void)                                                              \
  fifo_st_reset(P loc *RESTRICT p, T v, P loc *RESTRICT &p_o,                  \
                sparse_fifo_t &fifo_o, int &pos_o) {                           \
    pos_o = 0;                                                                 \
    sparse_fifo_t fifo;                                                        \
    __builtin_aie2ps_fifo_st_push_BFP##SIZE((void loc *RESTRICT &)p, fifo,     \
                                            pos_o, ##__VA_ARGS__);             \
    p_o = p;                                                                   \
    fifo_o = fifo;                                                             \
  }                                                                            \
  INTRINSIC(void) fifo_st_reset(P loc *RESTRICT &p, T v, fifo_state_t &s) {    \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_push_BFP##SIZE((void loc *RESTRICT &)p, fifo,     \
                                            s.pos, ##__VA_ARGS__);             \
  }                                                                            \
  INTRINSIC(void) fifo_st_push(P loc *RESTRICT &p, T v, fifo_state_t &s) {     \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_push_BFP##SIZE((void loc *RESTRICT &)p, fifo,     \
                                            pos, ##__VA_ARGS__);               \
  }

#define _FIFO_FLUSH(T, P, loc, RESTRICT)                                       \
  INTRINSIC(void) fifo_st_flush(P loc *RESTRICT &p, fifo_state_t &s) {         \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush((void loc *RESTRICT &)p, fifo, pos);        \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_1d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off) {        \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_1d_byte((void loc *RESTRICT &)p, fifo, pos, \
                                           off);                               \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_2d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off,          \
                        int size1, addr_t &count1, int inc1) {                 \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_2d_byte((void loc *RESTRICT &)p, fifo, pos, \
                                           off, size1, count1, inc1);          \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_3d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off,          \
                        int size1, addr_t &count1, int inc1, int size2,        \
                        addr_t &count2, int inc2) {                            \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_3d_byte((void loc *RESTRICT &)p, fifo, pos, \
                                           off, size1, count1, inc1, size2,    \
                                           count2, inc2);                      \
  }                                                                            \
  INTRINSIC(void) fifo_st_flush_bare(P loc *RESTRICT &p, fifo_state_t &s) {    \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush((void loc *RESTRICT &)p, fifo, pos);        \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_bare_1d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off) {   \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_1d_byte((void loc *RESTRICT &)p, fifo, pos, \
                                           off);                               \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_bare_2d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off,     \
                             int size1, addr_t &count1, int inc1) {            \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_2d_byte((void loc *RESTRICT &)p, fifo, pos, \
                                           off, size1, count1, inc1);          \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_bare_3d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off,     \
                             int size1, addr_t &count1, int inc1, int size2,   \
                             addr_t &count2, int inc2) {                       \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_3d_byte((void loc *RESTRICT &)p, fifo, pos, \
                                           off, size1, count1, inc1, size2,    \
                                           count2, inc2);                      \
  }                                                                            \
  INTRINSIC(void) fifo_st_flush_conv(P loc *RESTRICT &p, fifo_state_t &s) {    \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_conv((void loc *RESTRICT &)p, fifo, pos);   \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_conv_1d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off) {   \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_conv_1d_byte((void loc *RESTRICT &)p, fifo, \
                                                pos, off);                     \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_conv_2d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off,     \
                             int size1, addr_t &count1, int inc1) {            \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_conv_2d_byte(                               \
        (void loc *RESTRICT &)p, fifo, pos, off, size1, count1, inc1);         \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_conv_3d_byte(P loc *RESTRICT &p, fifo_state_t &s, int off,     \
                             int size1, addr_t &count1, int inc1, int size2,   \
                             addr_t &count2, int inc2) {                       \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2ps_fifo_st_flush_conv_3d_byte((void loc *RESTRICT &)p, fifo, \
                                                pos, off, size1, count1, inc1, \
                                                size2, count2, inc2);          \
  }

#define _FIFO2(T, P, loc, RESTRICT)                                            \
  _FIFO_PUSH(T, P, loc, RESTRICT)                                              \
  _FIFO_FLUSH(T, P, loc, RESTRICT)

#define _FIFO2_FP8(T, P, loc, RESTRICT)                                        \
  _FIFO_PUSH_FP8(T, P, loc, RESTRICT)                                          \
  _FIFO_FLUSH(T, P, loc, RESTRICT)

#define _FIFO2_BFP(T, P, loc, RESTRICT, SIZE, ...)                             \
  _FIFO_PUSH_BFP(T, P, loc, RESTRICT, SIZE, ##__VA_ARGS__)                     \
  _FIFO_FLUSH(T, P, loc, RESTRICT)

#define _FIFO(a, b, RESTRICT)                                                  \
  _FIFO2(a, b, , RESTRICT)                                                     \
  _FIFO2(a, b, __aie_dm_resource_a, RESTRICT)                                  \
  _FIFO2(a, b, __aie_dm_resource_b, RESTRICT)                                  \
  _FIFO2(a, b, __aie_dm_resource_c, RESTRICT)                                  \
  _FIFO2(a, b, __aie_dm_resource_d, RESTRICT)                                  \
  _FIFO2(a, b, __aie_dm_resource_ab, RESTRICT)                                 \
  _FIFO2(a, b, __aie_dm_resource_ac, RESTRICT)                                 \
  _FIFO2(a, b, __aie_dm_resource_ad, RESTRICT)                                 \
  _FIFO2(a, b, __aie_dm_resource_bc, RESTRICT)                                 \
  _FIFO2(a, b, __aie_dm_resource_bd, RESTRICT)                                 \
  _FIFO2(a, b, __aie_dm_resource_cd, RESTRICT)

#define _FIFO_FP8(a, b, RESTRICT)                                              \
  _FIFO2_FP8(a, b, , RESTRICT)                                                 \
  _FIFO2_FP8(a, b, __aie_dm_resource_a, RESTRICT)                              \
  _FIFO2_FP8(a, b, __aie_dm_resource_b, RESTRICT)                              \
  _FIFO2_FP8(a, b, __aie_dm_resource_c, RESTRICT)                              \
  _FIFO2_FP8(a, b, __aie_dm_resource_d, RESTRICT)                              \
  _FIFO2_FP8(a, b, __aie_dm_resource_ab, RESTRICT)                             \
  _FIFO2_FP8(a, b, __aie_dm_resource_ac, RESTRICT)                             \
  _FIFO2_FP8(a, b, __aie_dm_resource_ad, RESTRICT)                             \
  _FIFO2_FP8(a, b, __aie_dm_resource_bc, RESTRICT)                             \
  _FIFO2_FP8(a, b, __aie_dm_resource_bd, RESTRICT)                             \
  _FIFO2_FP8(a, b, __aie_dm_resource_cd, RESTRICT)

#define _FIFO_BFP(a, b, RESTRICT, SIZE, ...)                                   \
  _FIFO2_BFP(a, b, , RESTRICT, SIZE, ##__VA_ARGS__)                            \
  _FIFO2_BFP(a, b, __aie_dm_resource_a, RESTRICT, SIZE, ##__VA_ARGS__)         \
  _FIFO2_BFP(a, b, __aie_dm_resource_b, RESTRICT, SIZE, ##__VA_ARGS__)         \
  _FIFO2_BFP(a, b, __aie_dm_resource_c, RESTRICT, SIZE, ##__VA_ARGS__)         \
  _FIFO2_BFP(a, b, __aie_dm_resource_d, RESTRICT, SIZE, ##__VA_ARGS__)         \
  _FIFO2_BFP(a, b, __aie_dm_resource_ab, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(a, b, __aie_dm_resource_ac, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(a, b, __aie_dm_resource_ad, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(a, b, __aie_dm_resource_bc, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(a, b, __aie_dm_resource_bd, RESTRICT, SIZE, ##__VA_ARGS__)        \
  _FIFO2_BFP(a, b, __aie_dm_resource_cd, RESTRICT, SIZE, ##__VA_ARGS__)

_FIFO(v128int4, v128int4, )
_FIFO(v128uint4, v128uint4, )
_FIFO(v64int8, v64int8, )
_FIFO(v64uint8, v64uint8, )
_FIFO(v32int16, v32int16, )
_FIFO(v32uint16, v32uint16, )
_FIFO(v16int32, v16int32, )
_FIFO(v16uint32, v16uint32, )
_FIFO_FP8(v64bfloat8, v64bfloat8, )
_FIFO_FP8(v64float8, v64float8, )

_FIFO(v128int4, v128int4, restrict)
_FIFO(v128uint4, v128uint4, restrict)
_FIFO(v64int8, v64int8, restrict)
_FIFO(v64uint8, v64uint8, restrict)
_FIFO(v32int16, v32int16, restrict)
_FIFO(v32uint16, v32uint16, restrict)
_FIFO(v16int32, v16int32, restrict)
_FIFO(v16uint32, v16uint32, restrict)
_FIFO_FP8(v64bfloat8, v64bfloat8, restrict)
_FIFO_FP8(v64float8, v64float8, restrict)
#if 0
_FIFO(v16cint16, v16cint16,)
_FIFO( v8cint32,  v8cint32,)

_FIFO(    v8cfloat,    v8cfloat,)
_FIFO(v16cbfloat16,v16cbfloat16,)

_FIFO(v16cint16, v16cint16, restrict)
_FIFO( v8cint32,  v8cint32, restrict)

_FIFO(    v8cfloat,    v8cfloat, restrict)
_FIFO(v16cbfloat16,v16cbfloat16, restrict)
#endif
_FIFO(v32float16, v32float16, )
_FIFO(v32bfloat16, v32bfloat16, )
_FIFO(v16float, v16float, )

_FIFO(v32float16, v32float16, restrict)
_FIFO(v32bfloat16, v32bfloat16, restrict)
_FIFO(v16float, v16float, restrict)

_FIFO_BFP(v64mx9, v64mx9_unaligned, , 640, v.mantissa, v.tileShift, v.exponent)
_FIFO_BFP(v128mx6, v128mx6_unaligned, , 768, v.mantissaX0, v.mantissaX1,
          v.signF0, v.signF1, v.tileShiftG0, v.tileShiftG1, v.exponentE0,
          v.exponentE1)
_FIFO_BFP(v64mx6, v64mx6_unaligned, , 384, v.mantissa, v.sign, v.tileShift,
          v.exponent)
_FIFO_BFP(v128mx4, v128mx4_unaligned, , 768, v.mantissaX0, v.mantissaX1,
          v.signF0, v.signF1, v.tileShiftG0, v.tileShiftG1, v.exponentE0,
          v.exponentE1)
_FIFO_BFP(v64mx4, v64mx4_unaligned, , 384, v.mantissa, v.sign, v.tileShift,
          v.exponent)

_FIFO_BFP(v64mx9, v64mx9_unaligned, restrict, 640, v.mantissa, v.tileShift,
          v.exponent)
_FIFO_BFP(v128mx6, v128mx6_unaligned, restrict, 768, v.mantissaX0, v.mantissaX1,
          v.signF0, v.signF1, v.tileShiftG0, v.tileShiftG1, v.exponentE0,
          v.exponentE1)
_FIFO_BFP(v64mx6, v64mx6_unaligned, restrict, 384, v.mantissa, v.sign,
          v.tileShift, v.exponent)
_FIFO_BFP(v128mx4, v128mx4_unaligned, restrict, 768, v.mantissaX0, v.mantissaX1,
          v.signF0, v.signF1, v.tileShiftG0, v.tileShiftG1, v.exponentE0,
          v.exponentE1)
_FIFO_BFP(v64mx4, v64mx4_unaligned, restrict, 384, v.mantissa, v.sign,
          v.tileShift, v.exponent)

#undef _FIFO_BFP
#undef _FIFO
#undef _FIFO2
#undef _FIFO2_BFP
#undef _FIFO_PUSH
#undef _FIFO_PUSH_BFP
#undef _FIFO_FLUSH

#endif // AIE2PS_LDST_H
