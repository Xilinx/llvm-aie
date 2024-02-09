//===- aiebase_typedefs.h ---------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEBASE_TYPEDEFS_H
#define __AIEBASE_TYPEDEFS_H

#ifndef __COMPLEXINT_H__
#define __COMPLEXINT_H__

#include <stdbool.h>
#include <stdint.h>

struct cint16 {
#ifdef __cplusplus
  cint16(short re, short im = 0) : real(re), imag(im) {}
  cint16() = default;
#endif
  short real;
  short imag;
};
#ifdef __cplusplus
extern "C" {
#endif
#if (!defined(__AIENGINE__) && !defined(__PTHREAD_API__)) ||                   \
    defined(__PL_KERNEL__)
struct cint16 {
  short real;
  short imag;
};
#endif

#if defined(__AIENGINE__) && !defined(__PTHREAD_API__)
#define COMPLEX_TYPE(type)                                                     \
  typedef struct {                                                             \
    type chess_storage(% (sizeof(type) * 2)) real;                             \
    type imag;                                                                 \
  } c##type
#else
#define COMPLEX_TYPE(type)                                                     \
  typedef struct {                                                             \
    type real;                                                                 \
    type imag;                                                                 \
  } c##type
#endif

COMPLEX_TYPE(int8_t);
COMPLEX_TYPE(int32_t);
COMPLEX_TYPE(int64_t);
COMPLEX_TYPE(uint8_t);
COMPLEX_TYPE(uint16_t);
COMPLEX_TYPE(uint32_t);
COMPLEX_TYPE(uint64_t);

typedef struct {
  float real;
  float imag;
} cfloat_t;

typedef cfloat_t cfloat;

#undef COMPLEX_TYPE

#define COMPLEX_TYPEDEF(type) typedef c##type##_t c##type;

COMPLEX_TYPEDEF(int8)
COMPLEX_TYPEDEF(int32)
COMPLEX_TYPEDEF(int64)
COMPLEX_TYPEDEF(uint8)
COMPLEX_TYPEDEF(uint16)
COMPLEX_TYPEDEF(uint32)
COMPLEX_TYPEDEF(uint64)

typedef struct cint16 cint16_t;
#undef COMPLEX_TYPEDEF

// cascade types - currently defined using int64_t
typedef struct {
  int64_t real;
  int64_t imag;
} cacc48_t;
typedef cacc48_t cacc48;
typedef int64_t acc48_t;
typedef acc48_t acc48;

#ifdef __cplusplus
};
#endif
inline int as_int(cuint16_t v) { return *(int *)&v; }
#endif // __COMPLEXINT_H__

#include <stdint.h>

// Scalar types
typedef bool uint1_t;
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;
// BitInt Types
typedef signed _BitInt(2) int2_t;
typedef unsigned _BitInt(2) uint2_t;
typedef signed _BitInt(4) int4_t;
typedef unsigned _BitInt(4) uint4_t;
typedef signed _BitInt(6) int6_t;
typedef unsigned _BitInt(6) uint6_t;
typedef signed _BitInt(8) bint8_t;
typedef unsigned _BitInt(8) buint8_t;
// Complex BitInt Types
typedef signed _BitInt(32) v2cint16 __attribute__((__vector_size__(8)));
typedef unsigned _BitInt(32) v2cuint16 __attribute__((__vector_size__(8)));
typedef signed _BitInt(32) v4cint16 __attribute__((__vector_size__(16)));
typedef unsigned _BitInt(32) v4cuint16 __attribute__((__vector_size__(16)));
typedef signed _BitInt(32) v8cint16 __attribute__((__vector_size__(32)));
typedef unsigned _BitInt(32) v8cuint16 __attribute__((__vector_size__(32)));
typedef signed _BitInt(32) v16cint16 __attribute__((__vector_size__(64)));
typedef unsigned _BitInt(32) v16cuint16 __attribute__((__vector_size__(64)));
typedef signed _BitInt(32) v32cint16 __attribute__((__vector_size__(128)));
typedef unsigned _BitInt(32) v32cuint16 __attribute__((__vector_size__(128)));
typedef signed _BitInt(64) v2cint32 __attribute__((__vector_size__(16)));
typedef unsigned _BitInt(64) v2cuint32 __attribute__((__vector_size__(16)));
typedef signed _BitInt(64) v4cint32 __attribute__((__vector_size__(32)));
typedef unsigned _BitInt(64) v4cuint32 __attribute__((__vector_size__(32)));
typedef signed _BitInt(64) v8cint32 __attribute__((__vector_size__(64)));
typedef unsigned _BitInt(64) v8cuint32 __attribute__((__vector_size__(64)));
typedef signed _BitInt(64) v16cint32 __attribute__((__vector_size__(128)));
typedef unsigned _BitInt(64) v16cuint32 __attribute__((__vector_size__(128)));

// Vector types
typedef float v32float __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
typedef float v16float __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef float v8float __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef float v4float __attribute__((__vector_size__(16)))
__attribute__((aligned(16)));

typedef int32_t v16int64 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
typedef int32_t v32int32 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
typedef int16_t v64int16 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
typedef int8_t v128int8 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
typedef uint32_t v16uint64 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
typedef uint32_t v32uint32 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
typedef uint16_t v64uint16 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
typedef uint8_t v128uint8 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));

typedef int32_t v8int64 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef int32_t v16int32 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef int16_t v32int16 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef int8_t v64int8 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef uint32_t v8uint64 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef uint32_t v16uint32 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef uint16_t v32uint16 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef uint8_t v64uint8 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));

typedef int32_t v4int64 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef int32_t v8int32 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef int16_t v16int16 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef int8_t v32int8 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef uint32_t v4uint64 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef uint32_t v8uint32 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef uint16_t v16uint16 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef uint8_t v32uint8 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));

typedef int32_t v4int32 __attribute__((__vector_size__(16)))
__attribute__((aligned(16)));
typedef int16_t v8int16 __attribute__((__vector_size__(16)))
__attribute__((aligned(16)));
typedef int8_t v16int8 __attribute__((__vector_size__(16)))
__attribute__((aligned(16)));
typedef uint32_t v4uint32 __attribute__((__vector_size__(16)))
__attribute__((aligned(16)));
typedef uint16_t v8uint16 __attribute__((__vector_size__(16)))
__attribute__((aligned(16)));
typedef uint8_t v16uint8 __attribute__((__vector_size__(16)))
__attribute__((aligned(16)));

#if __AIEARCH__ == 10
typedef __acc48 v8acc48 __attribute__((__vector_size__(48)));
typedef __acc48 v4acc80 __attribute__((__vector_size__(48)));
typedef __acc48 v16acc48 __attribute__((__vector_size__(96)));
typedef __acc48 v8acc80 __attribute__((__vector_size__(96)));
#endif //__AIEARCH__ == 10

/*64-bit type is handled as v2int32 */
typedef int32_t v2int32 __attribute__((__vector_size__(8)));

#if __AIEARCH__ == 20
typedef int32_t addr_t;
#ifdef __cplusplus
/* bfloat16 type */
class bfloat16 {
  __bf16 m0;

public:
  bfloat16() = default;
  constexpr inline bfloat16(float a0);

  inline operator float() const {
    const uint16_t I16 = __builtin_bit_cast(const uint16_t, m0);
    uint32_t I32 = int32_t(I16) << 16;
    return __builtin_bit_cast(float, I32);
  }
  inline operator __bf16() const { return m0; }
  inline explicit operator int() const { return __builtin_bfloat16_to_int(m0); }
  inline explicit operator char() const {
    return (char)(int)*this;
  }
  inline explicit operator signed char() const {
    return (signed char)(int)*this;
  }
  inline explicit operator signed short() const {
    return (signed short)(int)*this;
  }
  inline explicit operator unsigned() const { return (unsigned)(int)*this; }
  inline explicit operator unsigned char() const {
    return (unsigned char)(unsigned)*this;
  }
  inline explicit operator unsigned short() const {
    return (unsigned short)(unsigned)*this;
  }
};
/* arithmetic operation with bfloat16 operands */
#define BFLOAT16_OPS(type)                                                     \
  /* Implicit cast from __bf16 to bfloat16 */                                  \
  inline bfloat16 operator/(bfloat16 a, type b) {                              \
    return (float(a) / float(b));                                              \
  }                                                                            \
  inline bfloat16 operator*(bfloat16 a, type b) {                              \
    return (float(a) * float(b));                                              \
  }                                                                            \
  inline bfloat16 operator+(bfloat16 a, type b) {                              \
    return (float(a) + float(b));                                              \
  }                                                                            \
  inline bfloat16 operator-(bfloat16 a, type b) {                              \
    return (float(a) - float(b));                                              \
  }

BFLOAT16_OPS(bfloat16)
BFLOAT16_OPS(float)
BFLOAT16_OPS(int)
#undef BFLOAT16_OPS

inline bfloat16 operator-(bfloat16 a) { return bfloat16(0) - a; }
inline bfloat16 operator+(bfloat16 a) { return bfloat16(0) + a; }

/* compare operation with bfloat16 operands */
inline bool operator>(bfloat16 a, bfloat16 b) { return ((float)b < (float)a); }
inline bool operator<=(bfloat16 a, bfloat16 b) { return !((float)a > (float)b); }
inline bool operator!=(bfloat16 a, bfloat16 b) { return !((float)a == (float)b); }
inline bool operator==(bfloat16 a, bfloat16 b) { return (float)a == (float)b; }
#endif
/* 8-bit types */
typedef buint8_t v2uint4;
typedef bint8_t v2int4;
/* 16-bit types */
typedef uint8_t v2uint8 __attribute__((__vector_size__(2)));
typedef int8_t v2int8 __attribute__((__vector_size__(2)));
typedef buint8_t v4uint4 __attribute__((__vector_size__(2)));
typedef bint8_t v4int4 __attribute__((__vector_size__(2)));
/* 32-bit types */
typedef uint16_t v2uint16 __attribute__((__vector_size__(4)));
typedef int16_t v2int16 __attribute__((__vector_size__(4)));
typedef uint8_t v4uint8 __attribute__((__vector_size__(4)));
typedef int8_t v4int8 __attribute__((__vector_size__(4)));
typedef buint8_t v8uint4 __attribute__((__vector_size__(4)));
typedef bint8_t v8int4 __attribute__((__vector_size__(4)));
/* 64-bit types */
typedef uint32_t mask64 __attribute__((__vector_size__(8)));
typedef uint32_t v2uint32 __attribute__((__vector_size__(8)));
typedef int16_t v4int16 __attribute__((__vector_size__(8)));
typedef uint16_t v4uint16 __attribute__((__vector_size__(8)));
typedef uint8_t v8uint8 __attribute__((__vector_size__(8)));
typedef int8_t v8int8 __attribute__((__vector_size__(8)));
typedef buint8_t v16uint4 __attribute__((__vector_size__(8)));
typedef bint8_t v16int4 __attribute__((__vector_size__(8)));
/* vector types */
typedef buint8_t v32uint4 __attribute__((__vector_size__(16)));
typedef bint8_t v32int4 __attribute__((__vector_size__(16)));
typedef buint8_t v64uint4 __attribute__((__vector_size__(32)));
typedef bint8_t v64int4 __attribute__((__vector_size__(32)));
typedef buint8_t v128uint4 __attribute__((__vector_size__(64)));
typedef bint8_t v128int4 __attribute__((__vector_size__(64)));
typedef buint8_t v256uint4 __attribute__((__vector_size__(128)));
typedef bint8_t v256int4 __attribute__((__vector_size__(128)));
/* accumulator types in aiev2 */
typedef __acc32 acc32;
typedef __acc64 acc64;
typedef acc32 v8acc32 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef acc32 v16acc32 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef acc32 v32acc32 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
typedef acc64 v4acc64 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef acc64 v8acc64 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef acc64 v16acc64 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
/* accumulator float types in aiev2 */
typedef __accfloat accfloat;
typedef accfloat v8accfloat __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef accfloat v16accfloat __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef accfloat v32accfloat __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
/* vector bfloat16 types in aiev2 */
typedef __bf16 v2bfloat16 __attribute__((__vector_size__(4)));
typedef __bf16 v4bfloat16 __attribute__((__vector_size__(8)));
typedef __bf16 v8bfloat16 __attribute__((__vector_size__(16)));
typedef __bf16 v16bfloat16 __attribute__((__vector_size__(32)))
__attribute__((aligned(32)));
typedef __bf16 v32bfloat16 __attribute__((__vector_size__(64)))
__attribute__((aligned(32)));
typedef __bf16 v64bfloat16 __attribute__((__vector_size__(128)))
__attribute__((aligned(32)));
// Sparse types
typedef unsigned _BitInt(128) sparsity_t __attribute__((aligned(16)));
/* sparse vector types in aiev2*/
struct v256int4_sparse {
  v128int4 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs));
struct v128int8_sparse {
  v64int8 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs));
struct v64int16_sparse {
  v32int16 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs));
struct v256uint4_sparse {
  v128uint4 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs));
struct v128uint8_sparse {
  v64uint8 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs));
struct v64uint16_sparse {
  v32uint16 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs));
struct v64bfloat16_sparse {
  v32bfloat16 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs));

#endif //__AIEARCH__ == 20

#endif /*  __AIEBASE_TYPEDEFS_H */
