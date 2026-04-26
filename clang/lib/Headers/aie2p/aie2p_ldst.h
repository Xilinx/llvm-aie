//===-------------------- AIEngine AIE2p intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef AIE2P_LDST_H
#define AIE2P_LDST_H

//* Automatically generated file, do not edit! *
//
INTRINSIC(v32int8) pack(v32int16 v, int sign) {
  return __builtin_aie2p_pack_I512_I8_I16(v, sign);
}

INTRINSIC(v32uint8) pack(v32uint16 v, int sign) {
  return __builtin_aie2p_pack_I512_I8_I16(v, sign);
}

INTRINSIC(v64int4) pack(v64int8 v, int sign) {
  return __builtin_aie2p_pack_I512_I4_I8(v, sign);
  ;
}

INTRINSIC(v64uint4) pack(v64uint8 v, int sign) {
  return __builtin_aie2p_pack_I512_I4_I8(v, sign);
}

INTRINSIC(v64int8) pack(v64int16 v, int sign) {
  return __builtin_aie2p_pack_I1024_I8_I16(v, sign);
}

INTRINSIC(v64uint8) pack(v64uint16 v, int sign) {
  return __builtin_aie2p_pack_I1024_I8_I16(v, sign);
}

INTRINSIC(v128int4) pack(v128int8 v, int sign) {
  return __builtin_aie2p_pack_I1024_I4_I8(v, sign);
}

INTRINSIC(v128uint4) pack(v128uint8 v, int sign) {
  return __builtin_aie2p_pack_I1024_I4_I8(v, sign);
}

INTRINSIC(v32int8) pack(v32int16 v) { return pack(v, __SIGN_SIGNED); }

INTRINSIC(v32uint8) pack(v32uint16 v) { return pack(v, __SIGN_UNSIGNED); }

INTRINSIC(v64int4) pack(v64int8 v) { return pack(v, __SIGN_SIGNED); }

INTRINSIC(v64uint4) pack(v64uint8 v) { return pack(v, __SIGN_UNSIGNED); }

INTRINSIC(v64int8) pack(v64int16 v) { return pack(v, __SIGN_SIGNED); }

INTRINSIC(v64uint8) pack(v64uint16 v) { return pack(v, __SIGN_UNSIGNED); }

INTRINSIC(v128int4) pack(v128int8 v) { return pack(v, __SIGN_SIGNED); }

INTRINSIC(v128uint4) pack(v128uint8 v) { return pack(v, __SIGN_UNSIGNED); }

INTRINSIC(v32int16) unpack(v32int8 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I512_I16_I8(v, sign);
}

INTRINSIC(v32uint16) unpack(v32uint8 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I512_I16_I8(v, sign);
}

INTRINSIC(v64int8) unpack(v64int4 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I512_I8_I4(v, sign);
}

INTRINSIC(v64uint8) unpack(v64uint4 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I512_I8_I4(v, sign);
}

INTRINSIC(v64int16) unpack(v64int8 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I1024_I16_I8(v, sign);
}

INTRINSIC(v64uint16) unpack(v64uint8 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I1024_I16_I8(v, sign);
}

INTRINSIC(v128int8) unpack(v128int4 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I1024_I8_I4(v, sign);
}

INTRINSIC(v128uint8) unpack(v128uint4 v, uint1_t sign) {
  return __builtin_aie2p_unpack_I1024_I8_I4(v, sign);
}

INTRINSIC(v32int16) unpack(v32int8 v) { return unpack(v, __SIGN_SIGNED); }

INTRINSIC(v32uint16) unpack(v32uint8 v) { return unpack(v, __SIGN_UNSIGNED); }

INTRINSIC(v64int8) unpack(v64int4 v) { return unpack(v, __SIGN_SIGNED); }

INTRINSIC(v64uint8) unpack(v64uint4 v) { return unpack(v, __SIGN_UNSIGNED); }

INTRINSIC(v64int16) unpack(v64int8 v) { return unpack(v, __SIGN_SIGNED); }

INTRINSIC(v64uint16) unpack(v64uint8 v) { return unpack(v, __SIGN_UNSIGNED); }

INTRINSIC(v128int8) unpack(v128int4 v) { return unpack(v, __SIGN_SIGNED); }

INTRINSIC(v128uint8) unpack(v128uint4 v) { return unpack(v, __SIGN_UNSIGNED); }

#define FIFO_ST_PUSH_NORMAL(T, DM_BANK, RESTRICT)                              \
  INTRINSIC(void)                                                              \
  fifo_st_reset(T DM_BANK *RESTRICT &p, T v, fifo_state_t &s) {                \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_push_512_bfp16((void DM_BANK *RESTRICT &)p, v,     \
                                           fifo, s.pos);                       \
  }                                                                            \
  INTRINSIC(void) fifo_st_push(T DM_BANK *RESTRICT &p, T v, fifo_state_t &s) { \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_push_512_bfp16((void DM_BANK *RESTRICT &)p, v,     \
                                           fifo, pos);                         \
  }

#define FIFO_ST_PUSH_BFP16(T, SIZE, DM_BANK, RESTRICT)                         \
  INTRINSIC(void)                                                              \
  fifo_st_reset(T##_unaligned DM_BANK *RESTRICT &p, T v, fifo_state_t &s) {    \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    v64char mant = v.mantissa;                                                 \
    v8char exp = v.exponent;                                                   \
    __builtin_aie2p_fifo_st_push_##SIZE##_bfp16((void DM_BANK *RESTRICT &)p,   \
                                                mant, exp, fifo, s.pos);       \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_push(T##_unaligned DM_BANK *RESTRICT &p, T v, fifo_state_t &s) {     \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    v64char mant = v.mantissa;                                                 \
    v8char exp = v.exponent;                                                   \
    __builtin_aie2p_fifo_st_push_##SIZE##_bfp16((void DM_BANK *RESTRICT &)p,   \
                                                mant, exp, fifo, pos);         \
  }

#define FIFO_ST_FLUSH_BARE(T, VAR, DM_BANK, RESTRICT)                          \
  INTRINSIC(void)                                                              \
  fifo_st_flush##VAR(T DM_BANK *RESTRICT &p, fifo_state_t &s) {                \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_flush((void DM_BANK *RESTRICT &)p, fifo, pos);     \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush##VAR##_1d_byte(T DM_BANK *RESTRICT &p, fifo_state_t &s,        \
                               int off) {                                      \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_flush_1d_byte((void DM_BANK *RESTRICT &)p, fifo,   \
                                          pos, off);                           \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush##VAR##_2d_byte(T DM_BANK *RESTRICT &p, fifo_state_t &s,        \
                               int off, int size1, addr_t &count1, int inc1) { \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_flush_2d_byte((void DM_BANK *RESTRICT &)p, fifo,   \
                                          pos, off, size1, count1, inc1);      \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush##VAR##_3d_byte(T DM_BANK *RESTRICT &p, fifo_state_t &s,        \
                               int off, int size1, addr_t &count1, int inc1,   \
                               int size2, addr_t &count2, int inc2) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_flush_3d_byte((void DM_BANK *RESTRICT &)p, fifo,   \
                                          pos, off, size1, count1, inc1,       \
                                          size2, count2, inc2);                \
  }

#define FIFO_ST_FLUSH_CONV(T, DM_BANK, RESTRICT)                               \
  INTRINSIC(void)                                                              \
  fifo_st_flush_conv(T DM_BANK *RESTRICT &p, fifo_state_t &s) {                \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_flush_conv((void DM_BANK *RESTRICT &)p, fifo,      \
                                       pos);                                   \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_conv_1d_byte(T DM_BANK *RESTRICT &p, fifo_state_t &s,          \
                             int off) {                                        \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_flush_conv_1d_byte((void DM_BANK *RESTRICT &)p,    \
                                               fifo, pos, off);                \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_conv_2d_byte(T DM_BANK *RESTRICT &p, fifo_state_t &s, int off, \
                             int size1, addr_t &count1, int inc1) {            \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_flush_conv_2d_byte(                                \
        (void DM_BANK *RESTRICT &)p, fifo, pos, off, size1, count1, inc1);     \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush_conv_3d_byte(T DM_BANK *RESTRICT &p, fifo_state_t &s, int off, \
                             int size1, addr_t &count1, int inc1, int size2,   \
                             addr_t &count2, int inc2) {                       \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_st_flush_conv_3d_byte((void DM_BANK *RESTRICT &)p,    \
                                               fifo, pos, off, size1, count1,  \
                                               inc1, size2, count2, inc2);     \
  }

#define FIFO_ST_NORMAL(T, DM_BANK, RESTRICT)                                   \
  FIFO_ST_PUSH_NORMAL(T, DM_BANK, RESTRICT)                                    \
  FIFO_ST_FLUSH_BARE(T, , DM_BANK, RESTRICT)                                   \
  FIFO_ST_FLUSH_BARE(T, _bare, DM_BANK, RESTRICT)                              \
  FIFO_ST_FLUSH_CONV(T, DM_BANK, RESTRICT)

#define FIFO_ST_BFP16(T, SIZE, DM_BANK, RESTRICT)                              \
  FIFO_ST_PUSH_BFP16(T, SIZE, DM_BANK, RESTRICT)                               \
  FIFO_ST_FLUSH_BARE(T##_unaligned, , DM_BANK, RESTRICT)                       \
  FIFO_ST_FLUSH_BARE(T##_unaligned, _bare, DM_BANK, RESTRICT)                  \
  FIFO_ST_FLUSH_CONV(T##_unaligned, DM_BANK, RESTRICT)

#define FIFO_ST_PUSH_BFP16_WIDE(T1, T2, DM_BANK, RESTRICT)                     \
  INTRINSIC(void)                                                              \
  fifo_st_reset(T1##_unaligned DM_BANK *RESTRICT &p, T1 v, fifo_state_t &s) {  \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    s.pos = 0;                                                                 \
    T2 tmp = extract_##T2(v, 0);                                               \
    fifo_st_push(q, tmp, s);                                                   \
    tmp = extract_##T2(v, 1);                                                  \
    fifo_st_push(q, tmp, s);                                                   \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_push(T1##_unaligned DM_BANK *RESTRICT &p, T1 v, fifo_state_t &s) {   \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    T2 tmp = extract_##T2(v, 0);                                               \
    fifo_st_push(q, tmp, s);                                                   \
    tmp = extract_##T2(v, 1);                                                  \
    fifo_st_push(q, tmp, s);                                                   \
  }

#define FIFO_ST_FLUSH_BFP16_WIDE(T1, T2, VAR, DM_BANK, RESTRICT)               \
  INTRINSIC(void)                                                              \
  fifo_st_flush##VAR(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {   \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    fifo_st_flush(q, s);                                                       \
    fifo_st_flush(q, s);                                                       \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush##VAR##_1d_byte(T1##_unaligned DM_BANK *RESTRICT &p,            \
                               fifo_state_t &s, int off) {                     \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    fifo_st_flush(q, s);                                                       \
    fifo_st_flush_1d_byte(q, s, off);                                          \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush##VAR##_2d_byte(T1##_unaligned DM_BANK *RESTRICT &p,            \
                               fifo_state_t &s, int off, int size1,            \
                               addr_t &count1, int inc1) {                     \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    fifo_st_flush(q, s);                                                       \
    fifo_st_flush_2d_byte(q, s, off, size1, count1, inc1);                     \
  }                                                                            \
  INTRINSIC(void)                                                              \
  fifo_st_flush##VAR##_3d_byte(T1##_unaligned DM_BANK *RESTRICT &p,            \
                               fifo_state_t &s, int off, int size1,            \
                               addr_t &count1, int inc1, int size2,            \
                               addr_t &count2, int inc2) {                     \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    fifo_st_flush(q, s);                                                       \
    fifo_st_flush_3d_byte(q, s, off, size1, count1, inc1, size2, count2,       \
                          inc2);                                               \
  }

#define FIFO_ST(DM_BANK, RESTRICT)                                             \
  FIFO_ST_BFP16(v64bfp16ebs8, 576, DM_BANK, RESTRICT)                          \
  FIFO_ST_BFP16(v64bfp16ebs16, 544, DM_BANK, RESTRICT)                         \
  FIFO_ST_NORMAL(v32bfloat16, DM_BANK, RESTRICT)                               \
  FIFO_ST_NORMAL(v16float, DM_BANK, RESTRICT)                                  \
  FIFO_ST_NORMAL(v128int4, DM_BANK, RESTRICT)                                  \
  FIFO_ST_NORMAL(v128uint4, DM_BANK, RESTRICT)                                 \
  FIFO_ST_NORMAL(v64int8, DM_BANK, RESTRICT)                                   \
  FIFO_ST_NORMAL(v64uint8, DM_BANK, RESTRICT)                                  \
  FIFO_ST_NORMAL(v32int16, DM_BANK, RESTRICT)                                  \
  FIFO_ST_NORMAL(v32uint16, DM_BANK, RESTRICT)                                 \
  FIFO_ST_NORMAL(v16int32, DM_BANK, RESTRICT)                                  \
  FIFO_ST_NORMAL(v16uint32, DM_BANK, RESTRICT)                                 \
  FIFO_ST_PUSH_BFP16_WIDE(v128bfp16ebs8, v64bfp16ebs8, DM_BANK, RESTRICT)      \
  FIFO_ST_PUSH_BFP16_WIDE(v128bfp16ebs16, v64bfp16ebs16, DM_BANK, RESTRICT)    \
  FIFO_ST_FLUSH_BFP16_WIDE(v128bfp16ebs8, v64bfp16ebs8, , DM_BANK, RESTRICT)   \
  FIFO_ST_FLUSH_BFP16_WIDE(v128bfp16ebs8, v64bfp16ebs8, _bare, DM_BANK,        \
                           RESTRICT)                                           \
  FIFO_ST_FLUSH_BFP16_WIDE(v128bfp16ebs8, v64bfp16ebs8, _conv, DM_BANK,        \
                           RESTRICT)                                           \
  FIFO_ST_FLUSH_BFP16_WIDE(v128bfp16ebs16, v64bfp16ebs16, , DM_BANK, RESTRICT) \
  FIFO_ST_FLUSH_BFP16_WIDE(v128bfp16ebs16, v64bfp16ebs16, _bare, DM_BANK,      \
                           RESTRICT)                                           \
  FIFO_ST_FLUSH_BFP16_WIDE(v128bfp16ebs16, v64bfp16ebs16, _conv, DM_BANK,      \
                           RESTRICT)

FIFO_ST(, )
FIFO_ST(__aie_dm_resource_a, )
FIFO_ST(__aie_dm_resource_b, )
FIFO_ST(__aie_dm_resource_c, )
FIFO_ST(__aie_dm_resource_d, )
FIFO_ST(__aie_dm_resource_ab, )
FIFO_ST(__aie_dm_resource_ac, )
FIFO_ST(__aie_dm_resource_ad, )
FIFO_ST(__aie_dm_resource_bc, )
FIFO_ST(__aie_dm_resource_bd, )
FIFO_ST(__aie_dm_resource_cd, )

FIFO_ST(, restrict)
FIFO_ST(__aie_dm_resource_a, restrict)
FIFO_ST(__aie_dm_resource_b, restrict)
FIFO_ST(__aie_dm_resource_c, restrict)
FIFO_ST(__aie_dm_resource_d, restrict)
FIFO_ST(__aie_dm_resource_ab, restrict)
FIFO_ST(__aie_dm_resource_ac, restrict)
FIFO_ST(__aie_dm_resource_ad, restrict)
FIFO_ST(__aie_dm_resource_bc, restrict)
FIFO_ST(__aie_dm_resource_bd, restrict)
FIFO_ST(__aie_dm_resource_cd, restrict)

#undef FIFO_ST_PUSH_NORMAL
#undef FIFO_ST_FLUSH_BARE
#undef FIFO_ST_FLUSH_CONV
#undef FIFO_ST_PUSH_BFP16
#undef FIFO_ST_NORMAL
#undef FIFO_ST_BFP16
#undef FIFO_ST_PUSH_BFP16_WIDE
#undef FIFO_ST_FLUSH_BFP16_WIDE
#undef FIFO_ST

#define FIFO_LD_NORMAL(T, DM_BANK, RESTRICT)                                   \
  INTRINSIC(void) fifo_ld_reset(T DM_BANK *RESTRICT &p, fifo_state_t &s) {     \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_ld_fill((void DM_BANK *RESTRICT &)p, fifo, s.pos);    \
  }                                                                            \
                                                                               \
  INTRINSIC(void) fifo_ld_fill(T DM_BANK *RESTRICT &p, fifo_state_t &s) {      \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_ld_fill((void DM_BANK *RESTRICT &)p, fifo, pos);      \
  }                                                                            \
                                                                               \
  INTRINSIC(T) fifo_ld_pop(T DM_BANK *RESTRICT &p, fifo_state_t &s) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = (T)__builtin_aie2p_fifo_ld_pop_512_unaligned(                        \
        (void DM_BANK *RESTRICT &)p, fifo, pos);                               \
    return r;                                                                  \
  }                                                                            \
                                                                               \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_1d_byte(T DM_BANK *RESTRICT &p, fifo_state_t &s, int off) {      \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = (T)__builtin_aie2p_fifo_ld_pop_1d_512_unaligned(                     \
        (void DM_BANK *RESTRICT &)p, fifo, pos, off);                          \
    return r;                                                                  \
  }                                                                            \
                                                                               \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_2d_byte(T DM_BANK *RESTRICT &p, fifo_state_t &s, int off,        \
                      int size1, addr_t &count1, int inc1) {                   \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = (T)__builtin_aie2p_fifo_ld_pop_2d_512_unaligned(                     \
        (void DM_BANK *RESTRICT &)p, fifo, pos, off, size1, count1, inc1);     \
    return r;                                                                  \
  }                                                                            \
                                                                               \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_3d_byte(T DM_BANK *RESTRICT &p, fifo_state_t &s, int off,        \
                      int size1, addr_t &count1, int inc1, int size2,          \
                      addr_t &count2, int inc2) {                              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r = (T)__builtin_aie2p_fifo_ld_pop_3d_512_unaligned(                     \
        (void DM_BANK *RESTRICT &)p, fifo, pos, off, size1, count1, inc1,      \
        size2, count2, inc2);                                                  \
    return r;                                                                  \
  }

#define FIFO_LD_BFP16(T, SIZE, DM_BANK, RESTRICT)                              \
  INTRINSIC(void)                                                              \
  fifo_ld_reset(T##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {         \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_ld_fill((void DM_BANK *RESTRICT &)p, fifo, s.pos);    \
  }                                                                            \
                                                                               \
  INTRINSIC(void)                                                              \
  fifo_ld_fill(T##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_ld_fill((void DM_BANK *RESTRICT &)p, fifo, pos);      \
  }                                                                            \
                                                                               \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop(T##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {           \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r;                                                                       \
    __builtin_aie2p_fifo_ld_pop_##SIZE##_bfp16(                                \
        (void DM_BANK *RESTRICT &)p, fifo, pos, (v64char &)r.mantissa,         \
        (v8char &)r.exponent);                                                 \
    return r;                                                                  \
  }                                                                            \
                                                                               \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_1d_byte(T##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s,     \
                      int off) {                                               \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r;                                                                       \
    __builtin_aie2p_fifo_ld_pop_1d_##SIZE##_bfp16(                             \
        (void DM_BANK *RESTRICT &)p, fifo, pos, off, (v64char &)r.mantissa,    \
        (v8char &)r.exponent);                                                 \
    return r;                                                                  \
  }                                                                            \
                                                                               \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_2d_byte(T##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s,     \
                      int off, int size1, addr_t &count1, int inc1) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r;                                                                       \
    __builtin_aie2p_fifo_ld_pop_2d_##SIZE##_bfp16(                             \
        (void DM_BANK *RESTRICT &)p, fifo, pos, off, size1, count1, inc1,      \
        (v64char &)r.mantissa, (v8char &)r.exponent);                          \
    return r;                                                                  \
  }                                                                            \
                                                                               \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop_3d_byte(T##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s,     \
                      int off, int size1, addr_t &count1, int inc1, int size2, \
                      addr_t &count2, int inc2) {                              \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r;                                                                       \
    __builtin_aie2p_fifo_ld_pop_3d_##SIZE##_bfp16(                             \
        (void DM_BANK *RESTRICT &)p, fifo, pos, off, size1, count1, inc1,      \
        size2, count2, inc2, (v64char &)r.mantissa, (v8char &)r.exponent);     \
    return r;                                                                  \
  }

#define FIFO_FILLX(T, DM_BANK, RESTRICT)                                       \
  INTRINSIC(void)                                                              \
  fifo_ld_fillx(T DM_BANK *RESTRICT &p, fifo_state_t &s, int step, int mask) { \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    v16int32 &extra = s.extra;                                                 \
    int conf = (step << 6) | mask;                                             \
    __builtin_aie2p_fifo_ld_fillx((void DM_BANK *RESTRICT &)p, fifo, pos,      \
                                  extra, conf);                                \
  }                                                                            \
                                                                               \
  INTRINSIC(void) fifo_ld_fillx(T DM_BANK *RESTRICT &p, fifo_state_t &s) {     \
    return fifo_ld_fillx(p, s, 31, 31);                                        \
  }

#define FIFO_POPX(T, DM_BANK, RESTRICT)                                        \
  INTRINSIC(T)                                                                 \
  fifo_ld_popx(T DM_BANK *RESTRICT &p, fifo_state_t &s, int step, int mask) {  \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    v16int32 &extra = s.extra;                                                 \
    int conf = (step << 6) | mask;                                             \
    T r = (T)__builtin_aie2p_fifo_ld_popx((void DM_BANK *RESTRICT &)p, fifo,   \
                                          pos, extra, conf);                   \
    return r;                                                                  \
  }                                                                            \
  INTRINSIC(T) fifo_ld_popx(T DM_BANK *RESTRICT &p, fifo_state_t &s) {         \
    return fifo_ld_popx(p, s, 31, 31);                                         \
  }

// Sparse FIFO load for the 640-bit-class types (data=v64* + sparsity_t mask).
// Wires through the new __builtin_aie2p_fifo_ld_pop_640_unaligned_sparse
// builtin, which lowers to silicon ops VLDA_POP_640_normal_pop /
// VLDB_POP_640_normal_pop (selected via VLD_POP_640_normal_pop_pseudo).
//
// T must be a sparse vector struct (v128int8_sparse, v128uint8_sparse,
// v64int16_sparse, v64uint16_sparse, v256int4_sparse, v256uint4_sparse)
// with the standard {DATA data; sparsity_t mask;} layout from
// aiebase_typedefs.h. T_DATA is the 512-bit dense data field type.
#define FIFO_LD_SPARSE(T, T_DATA, DM_BANK, RESTRICT)                           \
  INTRINSIC(void)                                                              \
  fifo_ld_reset(T##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {         \
    s.pos = 0;                                                                 \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_ld_fill((void DM_BANK *RESTRICT &)p, fifo, s.pos);    \
  }                                                                            \
                                                                               \
  INTRINSIC(void)                                                              \
  fifo_ld_fill(T##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {          \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    __builtin_aie2p_fifo_ld_fill((void DM_BANK *RESTRICT &)p, fifo, pos);      \
  }                                                                            \
                                                                               \
  INTRINSIC(T)                                                                 \
  fifo_ld_pop(T##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {           \
    int &pos = s.pos;                                                          \
    sparse_fifo_t &fifo = s.fifo;                                              \
    T r;                                                                       \
    __builtin_aie2p_fifo_ld_pop_640_unaligned_sparse(                          \
        (void DM_BANK *RESTRICT &)p, fifo, pos, (v64char &)r.data,             \
        (v16char &)r.mask);                                                    \
    return r;                                                                  \
  }

// Wide-pair sparse FIFO load: pop a smaller sparse vector twice and stitch
// into the wider sparse vector via Followup H's set_##T1 + insert helpers.
// T1 is the wide sparse type (e.g. v256int8_sparse, must have lo/hi fields
// of T2 type per aiebase_typedefs.h composite-struct definitions).
// T2 is the narrow sparse type (e.g. v128int8_sparse).
#define FIFO_LD_SPARSE_WIDE(T1, T2, DM_BANK, RESTRICT)                         \
  INTRINSIC(void)                                                              \
  fifo_ld_reset(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {        \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    fifo_ld_reset(q, s);                                                       \
  }                                                                            \
                                                                               \
  INTRINSIC(void)                                                              \
  fifo_ld_fill(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {         \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    fifo_ld_fill(q, s);                                                        \
  }                                                                            \
                                                                               \
  INTRINSIC(T1)                                                                \
  fifo_ld_pop(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {          \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    T1 v;                                                                      \
    v = set_##T1(0, fifo_ld_pop(q, s));                                        \
    v = insert(v, 1, fifo_ld_pop(q, s));                                       \
    return v;                                                                  \
  }

#define FIFO_LD_BFP16_WIDE(T1, T2, DM_BANK, RESTRICT)                          \
  INTRINSIC(void)                                                              \
  fifo_ld_reset(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {        \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    fifo_ld_reset(q, s);                                                       \
  }                                                                            \
                                                                               \
  INTRINSIC(void)                                                              \
  fifo_ld_fill(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {         \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    fifo_ld_fill(q, s);                                                        \
  }                                                                            \
  INTRINSIC(T1)                                                                \
  fifo_ld_pop(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s) {          \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    T1 v;                                                                      \
    v = set_##T1(0, fifo_ld_pop(q, s));                                        \
    v = insert(v, 1, fifo_ld_pop(q, s));                                       \
    return v;                                                                  \
  }                                                                            \
  INTRINSIC(T1)                                                                \
  fifo_ld_pop_1d_byte(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s,    \
                      int off) {                                               \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    T1 v;                                                                      \
    v = set_##T1(0, fifo_ld_pop(q, s));                                        \
    v = insert(v, 1, fifo_ld_pop_1d_byte(q, s, off));                          \
    return v;                                                                  \
  }                                                                            \
  INTRINSIC(T1)                                                                \
  fifo_ld_pop_2d_byte(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s,    \
                      int off, int size1, addr_t &count1, int inc1) {          \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    T1 v;                                                                      \
    v = set_##T1(0, fifo_ld_pop(q, s));                                        \
    v = insert(v, 1, fifo_ld_pop_2d_byte(q, s, off, size1, count1, inc1));     \
    return v;                                                                  \
  }                                                                            \
  INTRINSIC(T1)                                                                \
  fifo_ld_pop_3d_byte(T1##_unaligned DM_BANK *RESTRICT &p, fifo_state_t &s,    \
                      int off, int size1, addr_t &count1, int inc1, int size2, \
                      addr_t &count2, int inc2) {                              \
    T2##_unaligned DM_BANK *RESTRICT &q =                                      \
        (T2##_unaligned DM_BANK * RESTRICT &)p;                                \
    T1 v;                                                                      \
    v = set_##T1(0, fifo_ld_pop(q, s));                                        \
    v = insert(v, 1,                                                           \
               fifo_ld_pop_3d_byte(q, s, off, size1, count1, inc1, size2,      \
                                   count2, inc2));                             \
    return v;                                                                  \
  }

#define FIFO_LD(DM_BANK, RESTRICT)                                             \
  FIFO_LD_NORMAL(v32bfloat16, DM_BANK, RESTRICT)                               \
  FIFO_LD_NORMAL(v16float, DM_BANK, RESTRICT)                                  \
  FIFO_LD_NORMAL(v128int4, DM_BANK, RESTRICT)                                  \
  FIFO_LD_NORMAL(v128uint4, DM_BANK, RESTRICT)                                 \
  FIFO_LD_NORMAL(v64int8, DM_BANK, RESTRICT)                                   \
  FIFO_LD_NORMAL(v64uint8, DM_BANK, RESTRICT)                                  \
  FIFO_LD_NORMAL(v32int16, DM_BANK, RESTRICT)                                  \
  FIFO_LD_NORMAL(v32uint16, DM_BANK, RESTRICT)                                 \
  FIFO_LD_NORMAL(v16int32, DM_BANK, RESTRICT)                                  \
  FIFO_LD_NORMAL(v16uint32, DM_BANK, RESTRICT)                                 \
  FIFO_LD_BFP16(v64bfp16ebs8, 576, DM_BANK, RESTRICT)                          \
  FIFO_LD_BFP16(v64bfp16ebs16, 544, DM_BANK, RESTRICT)                         \
  FIFO_POPX(v32bfloat16, DM_BANK, RESTRICT)                                    \
  FIFO_POPX(v16float, DM_BANK, RESTRICT)                                       \
  FIFO_POPX(v128int4, DM_BANK, RESTRICT)                                       \
  FIFO_POPX(v128uint4, DM_BANK, RESTRICT)                                      \
  FIFO_POPX(v64int8, DM_BANK, RESTRICT)                                        \
  FIFO_POPX(v64uint8, DM_BANK, RESTRICT)                                       \
  FIFO_POPX(v32int16, DM_BANK, RESTRICT)                                       \
  FIFO_POPX(v32uint16, DM_BANK, RESTRICT)                                      \
  FIFO_POPX(v16int32, DM_BANK, RESTRICT)                                       \
  FIFO_POPX(v16uint32, DM_BANK, RESTRICT)                                      \
  FIFO_FILLX(v64bfp16ebs8_unaligned, DM_BANK, RESTRICT)                        \
  FIFO_FILLX(v64bfp16ebs16_unaligned, DM_BANK, RESTRICT)                       \
  FIFO_LD_BFP16_WIDE(v128bfp16ebs8, v64bfp16ebs8, DM_BANK, RESTRICT)           \
  FIFO_LD_BFP16_WIDE(v128bfp16ebs16, v64bfp16ebs16, DM_BANK, RESTRICT)         \
  FIFO_LD_SPARSE(v128int8_sparse, v64int8, DM_BANK, RESTRICT)                  \
  FIFO_LD_SPARSE_WIDE(v256int8_sparse, v128int8_sparse, DM_BANK, RESTRICT)

FIFO_LD(, )
FIFO_LD(__aie_dm_resource_a, )
FIFO_LD(__aie_dm_resource_b, )
FIFO_LD(__aie_dm_resource_c, )
FIFO_LD(__aie_dm_resource_d, )
FIFO_LD(__aie_dm_resource_ab, )
FIFO_LD(__aie_dm_resource_ac, )
FIFO_LD(__aie_dm_resource_ad, )
FIFO_LD(__aie_dm_resource_bc, )
FIFO_LD(__aie_dm_resource_bd, )
FIFO_LD(__aie_dm_resource_cd, )

FIFO_LD(, restrict)
FIFO_LD(__aie_dm_resource_a, restrict)
FIFO_LD(__aie_dm_resource_b, restrict)
FIFO_LD(__aie_dm_resource_c, restrict)
FIFO_LD(__aie_dm_resource_d, restrict)
FIFO_LD(__aie_dm_resource_ab, restrict)
FIFO_LD(__aie_dm_resource_ac, restrict)
FIFO_LD(__aie_dm_resource_ad, restrict)
FIFO_LD(__aie_dm_resource_bc, restrict)
FIFO_LD(__aie_dm_resource_bd, restrict)
FIFO_LD(__aie_dm_resource_cd, restrict)

#undef FIFO_LD_NORMAL
#undef FIFO_LD_BFP16
#undef FIFO_POPX
#undef FIFO_FILLX
#undef FIFO_LD_BFP16_WIDE
#undef FIFO_LD_SPARSE
#undef FIFO_LD_SPARSE_WIDE
#undef FIFO_LD

#endif // AIE2P_LDST_H
