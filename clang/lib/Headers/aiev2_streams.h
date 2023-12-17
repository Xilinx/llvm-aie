//===- aiev2_streams.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

INTRINSIC(v16acc32) get_scd_v16acc32(int en) {
  return __builtin_aiev2_scd_read_acc32(en);
}
INTRINSIC(v8acc64) get_scd_v8acc64(int en) {
  return (v8acc64)__builtin_aiev2_scd_read_acc32(en);
}
INTRINSIC(v16accfloat) get_scd_v16accfloat(int en) {
  return (v16accfloat)get_scd_v16acc32(en);
}
INTRINSIC(v128int4) get_scd_v128int4(int en) {
  return (v128int4)__builtin_aiev2_scd_read_vec(en);
}
INTRINSIC(v128uint4) get_scd_v128uint4(int en) {
  return (v128uint4)__builtin_aiev2_scd_read_vec(en);
}
INTRINSIC(v64int8) get_scd_v64int8(int en) {
  return (v64int8)__builtin_aiev2_scd_read_vec(en);
}
INTRINSIC(v64uint8) get_scd_v64uint8(int en) {
  return (v64uint8)__builtin_aiev2_scd_read_vec(en);
}
INTRINSIC(v32int16) get_scd_v32int16(int en) {
  return (v32int16)__builtin_aiev2_scd_read_vec(en);
}
INTRINSIC(v32uint16) get_scd_v32uint16(int en) {
  return (v32uint16)__builtin_aiev2_scd_read_vec(en);
}
INTRINSIC(v16int32) get_scd_v16int32(int en) {
  return (v16int32)__builtin_aiev2_scd_read_vec(en);
}
INTRINSIC(v16uint32) get_scd_v16uint32(int en) {
  return (v16uint32)__builtin_aiev2_scd_read_vec(en);
}
INTRINSIC(v32bfloat16) get_scd_v32bfloat16(int en) {
  return (v32bfloat16)__builtin_aiev2_scd_read_vec(en);
}
INTRINSIC(v16acc32) get_scd_v16acc32() { return get_scd_v16acc32(1); }
INTRINSIC(v8acc64) get_scd_v8acc64() { return get_scd_v8acc64(1); }
INTRINSIC(v16accfloat) get_scd_v16accfloat() { return get_scd_v16accfloat(1); }
INTRINSIC(v128int4) get_scd_v128int4() { return get_scd_v128int4(1); }
INTRINSIC(v128uint4) get_scd_v128uint4() { return get_scd_v128uint4(1); }
INTRINSIC(v64int8) get_scd_v64int8() { return get_scd_v64int8(1); }
INTRINSIC(v64uint8) get_scd_v64uint8() { return get_scd_v64uint8(1); }
INTRINSIC(v32int16) get_scd_v32int16() { return get_scd_v32int16(1); }
INTRINSIC(v32uint16) get_scd_v32uint16() { return get_scd_v32uint16(1); }
INTRINSIC(v16int32) get_scd_v16int32() { return get_scd_v16int32(1); }
INTRINSIC(v16uint32) get_scd_v16uint32() { return get_scd_v16uint32(1); }
INTRINSIC(v32bfloat16) get_scd_v32bfloat16() { return get_scd_v32bfloat16(1); }
INTRINSIC(v32acc32) get_scd_v32acc32(int en) {
  v32acc32 r;
  r = set_v32acc32(0, get_scd_v16acc32(en));
  r = insert(r, 1, get_scd_v16acc32(en));
  return r;
}
INTRINSIC(v16acc64) get_scd_v16acc64(int en) {
  return (v16acc64)get_scd_v32acc32(en);
}
INTRINSIC(v32accfloat) get_scd_v32accfloat(int en) {
  return (v32accfloat)get_scd_v32acc32(en);
}
INTRINSIC(v32int32) get_scd_v32int32(int en) {
  v32int32 r;
  r = set_v32int32(0, get_scd_v16int32(en));
  r = insert(r, 1, get_scd_v16int32(en));
  return r;
}
INTRINSIC(v256int4) get_scd_v256int4(int en) {
  return (v256int4)get_scd_v32int32(en);
}
INTRINSIC(v256uint4) get_scd_v256uint4(int en) {
  return (v256uint4)get_scd_v32int32(en);
}
INTRINSIC(v128int8) get_scd_v128int8(int en) {
  return (v128int8)get_scd_v32int32(en);
}
INTRINSIC(v128uint8) get_scd_v128uint8(int en) {
  return (v128uint8)get_scd_v32int32(en);
}
INTRINSIC(v64int16) get_scd_v64int16(int en) {
  return (v64int16)get_scd_v32int32(en);
}
INTRINSIC(v64uint16) get_scd_v64uint16(int en) {
  return (v64uint16)get_scd_v32int32(en);
}
INTRINSIC(v32uint32) get_scd_v32uint32(int en) {
  return (v32uint32)get_scd_v32int32(en);
}
INTRINSIC(v32acc32) get_scd_v32acc32() { return get_scd_v32acc32(1); }
INTRINSIC(v16acc64) get_scd_v16acc64() { return get_scd_v16acc64(1); }
INTRINSIC(v256int4) get_scd_v256int4() { return get_scd_v256int4(1); }
INTRINSIC(v256uint4) get_scd_v256uint4() { return get_scd_v256uint4(1); }
INTRINSIC(v128int8) get_scd_v128int8() { return get_scd_v128int8(1); }
INTRINSIC(v128uint8) get_scd_v128uint8() { return get_scd_v128uint8(1); }
INTRINSIC(v64int16) get_scd_v64int16() { return get_scd_v64int16(1); }
INTRINSIC(v64uint16) get_scd_v64uint16() { return get_scd_v64uint16(1); }
INTRINSIC(v32int32) get_scd_v32int32() { return get_scd_v32int32(1); }
INTRINSIC(v32uint32) get_scd_v32uint32() { return get_scd_v32uint32(1); }
INTRINSIC(v32acc32) get_scd_v32acc32_lo(int en) {
  return __builtin_aiev2_scd_expand_lo(en);
}
INTRINSIC(v32acc32) get_scd_v32acc32_hi(int en) {
  return __builtin_aiev2_scd_expand_hi(en);
}
INTRINSIC(v16acc64) get_scd_v16acc64_lo(int en) {
  return (v16acc64)__builtin_aiev2_scd_expand_lo(en);
}
INTRINSIC(v16acc64) get_scd_v16acc64_hi(int en) {
  return (v16acc64)__builtin_aiev2_scd_expand_hi(en);
}
INTRINSIC(v32acc32) get_scd_v32acc32_lo() { return get_scd_v32acc32_lo(1); }
INTRINSIC(v32acc32) get_scd_v32acc32_hi() { return get_scd_v32acc32_hi(1); }
INTRINSIC(v16acc64) get_scd_v16acc64_lo() { return get_scd_v16acc64_lo(1); }
INTRINSIC(v16acc64) get_scd_v16acc64_hi() { return get_scd_v16acc64_hi(1); }
INTRINSIC(void) put_mcd(v16acc32 a, int en) {
  __builtin_aiev2_mcd_write_acc32(a, en);
}
INTRINSIC(void) put_mcd(v8acc64 a, int en) {
  __builtin_aiev2_mcd_write_acc32(a, en);
}
INTRINSIC(void) put_mcd(v16accfloat a, int en) { put_mcd((v16acc32)a, en); }
INTRINSIC(void) put_mcd(v128int4 a, int en) {
  __builtin_aiev2_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v128uint4 a, int en) {
  __builtin_aiev2_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v64int8 a, int en) {
  __builtin_aiev2_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v64uint8 a, int en) {
  __builtin_aiev2_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v32int16 a, int en) {
  __builtin_aiev2_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v32uint16 a, int en) {
  __builtin_aiev2_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v16int32 a, int en) {
  __builtin_aiev2_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v16uint32 a, int en) {
  __builtin_aiev2_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v32bfloat16 a, int en) {
  __builtin_aiev2_mcd_write_vec(a, en);
}
INTRINSIC(void) put_mcd(v16acc32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v8acc64 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v16accfloat a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v128int4 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v128uint4 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v64int8 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v64uint8 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32int16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32uint16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v16int32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v16uint32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32bfloat16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32acc32 a, int en) {
  put_mcd(extract_v16acc32(a, 0), en);
  put_mcd(extract_v16acc32(a, 1), en);
}
INTRINSIC(void) put_mcd(v16acc64 a, int en) { put_mcd((v32acc32)a, en); }
INTRINSIC(void) put_mcd(v32int32 a, int en) {
  put_mcd(extract_v16int32(a, 0), en);
  put_mcd(extract_v16int32(a, 1), en);
}
INTRINSIC(void) put_mcd(v256int4 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v256uint4 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v128int8 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v128uint8 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v64int16 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v64uint16 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v32uint32 a, int en) { put_mcd((v32int32)a, en); }
INTRINSIC(void) put_mcd(v32acc32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v16acc64 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v256int4 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v256uint4 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v128int8 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v128uint8 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v64int16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v64uint16 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32int32 a) { put_mcd(a, 1); }
INTRINSIC(void) put_mcd(v32uint32 a) { put_mcd(a, 1); }

#define PUT_MS(TYPE)                                                           \
  INTRINSIC(void) put_ms(TYPE val, int tlast) {                                \
    __builtin_aiev2_put_ms((int)val, tlast);                                   \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE val) { __builtin_aiev2_put_ms((int)val, 0); }    \
  INTRINSIC(void) put_ms(TYPE val, int en, int tlast) { put_ms(val, tlast); }  \
  INTRINSIC(void) put_ms_nb(TYPE val, int tlast, bool &success) {              \
    int suc;                                                                   \
    __builtin_aiev2_put_ms_nb((int)val, tlast, suc);                           \
    success = suc;                                                             \
  }                                                                            \
  INTRINSIC(void) put_ms_nb(TYPE val, bool &success) {                         \
    int suc;                                                                   \
    __builtin_aiev2_put_ms_nb((int)val, 0, suc);                               \
    success = suc;                                                             \
  }
PUT_MS(int)
PUT_MS(unsigned int);
PUT_MS(float);
PUT_MS(v8int4);
PUT_MS(v8uint4);
PUT_MS(v4int8);
PUT_MS(v4uint8);
PUT_MS(v2int16);
PUT_MS(v2uint16);

// Note: no non-blocking variants of these.
#define PUT_MS_N(TYPE, TYPECAST, N)                                            \
  INTRINSIC(void) put_ms(TYPE a, int tlast) {                                  \
    v16int32 aa = set_v16int32(0, (TYPECAST)a);                                \
    for (int n = 0; n < N - 1; n++) {                                          \
      put_ms(ext_elem(aa, n), 0);                                              \
    }                                                                          \
    put_ms(ext_elem(aa, N - 1), tlast);                                        \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE a) {                                             \
    v16int32 aa = set_v16int32(0, (TYPECAST)a);                                \
    for (int n = 0; n < N - 1; n++) {                                          \
      put_ms(ext_elem(aa, n));                                                 \
    }                                                                          \
    put_ms(ext_elem(aa, N - 1));                                               \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE a, int en, int tlast) { put_ms(a, tlast); }

#define PUT_MS_4(TYPE) PUT_MS_N(TYPE, v4int32, 4)
PUT_MS_4(v4int32);
PUT_MS_4(v4uint32);
PUT_MS_4(v4float);
PUT_MS_4(v8int16);
PUT_MS_4(v8uint16);
PUT_MS_4(v16int8);
PUT_MS_4(v16uint8);
PUT_MS_4(v32int4);
PUT_MS_4(v32uint4);

#define PUT_MS_8(TYPE) PUT_MS_N(TYPE, v8int32, 8)
PUT_MS_8(v8int32);
PUT_MS_8(v8uint32);
PUT_MS_8(v8float);
PUT_MS_8(v16int16);
PUT_MS_8(v16uint16);
PUT_MS_8(v32int8);
PUT_MS_8(v32uint8);
PUT_MS_8(v64int4);
PUT_MS_8(v64uint4);

#define PUT_MS_16(TYPE)                                                        \
  INTRINSIC(void) put_ms(TYPE a, int tlast) {                                  \
    v16int32 aa = a;                                                           \
    for (int n = 0; n < 16 - 1; n++) {                                         \
      put_ms(ext_elem(aa, n), 0);                                              \
    }                                                                          \
    put_ms(ext_elem(aa, 16 - 1), tlast);                                       \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE a) {                                             \
    v16int32 aa = a;                                                           \
    for (int n = 0; n < 16 - 1; n++) {                                         \
      put_ms(ext_elem(aa, n));                                                 \
    }                                                                          \
    put_ms(ext_elem(aa, 16 - 1));                                              \
  }                                                                            \
  INTRINSIC(void) put_ms(TYPE a, int en, int tlast) { put_ms(a, tlast); }

PUT_MS_16(v16int32);
PUT_MS_16(v16uint32);
PUT_MS_16(v16float);
PUT_MS_16(v32int16);
PUT_MS_16(v32uint16);
PUT_MS_16(v64int8);
PUT_MS_16(v64uint8);
PUT_MS_16(v128int4);
PUT_MS_16(v128uint4);

#define PUT_MS_32(TYPE)                                                        \
  INTRINSIC(void) put_ms(TYPE a, int en, int tlast) {                          \
    put_ms(extract_v16int32(a, 0), en, 0);                                     \
    put_ms(extract_v16int32(a, 1), en, tlast);                                 \
  }

PUT_MS_32(v32int32);
PUT_MS_32(v32uint32);
PUT_MS_32(v32float);
PUT_MS_32(v64int16);
PUT_MS_32(v64uint16);
PUT_MS_32(v128int8);
PUT_MS_32(v128uint8);
PUT_MS_32(v256int4);
PUT_MS_32(v256uint4);

#define GET_SS(TYPE, SUFFIX)                                                   \
  INTRINSIC(TYPE) get_ss##SUFFIX() {                                           \
    int status;                                                                \
    return (TYPE)__builtin_aiev2_get_ss(status);                               \
  }                                                                            \
  INTRINSIC(TYPE) get_ss##SUFFIX(bool &tlast) {                                \
    int status;                                                                \
    TYPE val = (TYPE)__builtin_aiev2_get_ss(status);                           \
    tlast = status & 1;                                                        \
    return val;                                                                \
  }                                                                            \
  INTRINSIC(TYPE) get_ss_nb##SUFFIX(bool &success) {                           \
    int status;                                                                \
    TYPE val = (TYPE)__builtin_aiev2_get_ss_nb(status);                        \
    success = status & 2;                                                      \
    return val;                                                                \
  }                                                                            \
  INTRINSIC(TYPE) get_ss_nb##SUFFIX(bool &success, bool &tlast) {              \
    int status;                                                                \
    TYPE val = (TYPE)__builtin_aiev2_get_ss_nb(status);                        \
    tlast = status & 1;                                                        \
    success = status & 2;                                                      \
    return val;                                                                \
  }
GET_SS(int, );
GET_SS(int, _int);
GET_SS(unsigned int, _uint);
GET_SS(float, _float);
GET_SS(v8int4, _v8int4);
GET_SS(v8uint4, _v8uint4);
GET_SS(v4int8, _v4int8);
GET_SS(v4uint8, _v4uint8);
GET_SS(v2int16, _v2int16);
GET_SS(v2uint16, _v2uint16);

#define GET_SS_N(TYPE, N)                                                      \
  INTRINSIC(TYPE) get_ss_##TYPE() {                                            \
    v16int32 r = undef_v16int32();                                             \
    for (int n = 0; n < N - 1; n++) {                                          \
      r = upd_elem(r, n, get_ss_int());                                        \
    }                                                                          \
    r = upd_elem(r, N - 1, get_ss_int());                                      \
    return extract_##TYPE(r, 0);                                               \
  }

#define GET_SS_4(TYPE) GET_SS_N(TYPE, 4)
GET_SS_4(v4int32);
GET_SS_4(v4uint32);
GET_SS_4(v8int16);
GET_SS_4(v8uint16);
GET_SS_4(v16int8);
GET_SS_4(v16uint8);
GET_SS_4(v32int4);
GET_SS_4(v32uint4);

#define GET_SS_8(TYPE) GET_SS_N(TYPE, 8)
GET_SS_8(v8int32);
GET_SS_8(v8uint32);
GET_SS_8(v16int16);
GET_SS_8(v16uint16);
GET_SS_8(v32int8);
GET_SS_8(v32uint8);
GET_SS_8(v64int4);
GET_SS_8(v64uint4);

#define GET_SS_16(TYPE)                                                        \
  INTRINSIC(TYPE) get_ss_##TYPE() {                                            \
    v16int32 r = undef_v16int32();                                             \
    for (int n = 0; n < 16 - 1; n++) {                                         \
      r = upd_elem(r, n, get_ss_int());                                        \
    }                                                                          \
    r = upd_elem(r, 16 - 1, get_ss_int());                                     \
    return r;                                                                  \
  }
GET_SS_16(v16int32);
GET_SS_16(v16uint32);
GET_SS_16(v32int16);
GET_SS_16(v32uint16);
GET_SS_16(v64int8);
GET_SS_16(v64uint8);
GET_SS_16(v128int4);
GET_SS_16(v128uint4);

#define GET_SS_32(TYPE)                                                        \
  INTRINSIC(TYPE) get_ss_##TYPE() {                                            \
    v32int32 r;                                                                \
    r = set_v32int32(0, get_ss_v16int32());                                    \
    r = insert(r, 1, get_ss_v16int32());                                       \
    return r;                                                                  \
  }
GET_SS_32(v32int32);
GET_SS_32(v32uint32);
GET_SS_32(v64int16);
GET_SS_32(v64uint16);
GET_SS_32(v128int8);
GET_SS_32(v128uint8);
GET_SS_32(v256int4);
GET_SS_32(v256uint4);
