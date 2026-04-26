//===- aiebase_typedefs.h ---------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
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

#define COMPLEX_TYPE(type)                                                     \
  typedef struct {                                                             \
    type real;                                                                 \
    type imag;                                                                 \
  } c##type

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
// type alignment in bytes
#if __AIEARCH__ == 21 || __AIEARCH__ == 22
#define __MIN_ALIGNMENT_16 16
#define __MIN_ALIGNMENT_32 32
#define __MIN_ALIGNMENT_64 64
#define __MIN_ALIGNMENT_128 64
#define __MIN_ALIGNMENT_256 64
#else
#define __MIN_ALIGNMENT_16 16
#define __MIN_ALIGNMENT_32 32
#define __MIN_ALIGNMENT_64 32
#define __MIN_ALIGNMENT_128 32
#define __MIN_ALIGNMENT_256 32
#endif

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
typedef signed _BitInt(5) int5_t;
typedef unsigned _BitInt(5) uint5_t;
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
typedef float v64float __attribute__((__vector_size__(256)))
__attribute__((aligned(__MIN_ALIGNMENT_256)));
typedef float v32float __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef float v16float __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef float v8float __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef float v4float __attribute__((__vector_size__(16)))
__attribute__((aligned(__MIN_ALIGNMENT_16)));

typedef int32_t v16int64 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef int32_t v32int32 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef int16_t v64int16 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef int8_t v128int8 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef uint32_t v16uint64 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef uint32_t v32uint32 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef uint16_t v64uint16 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef uint8_t v128uint8 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));

typedef int32_t v8int64 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef int32_t v16int32 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef int16_t v32int16 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef int8_t v64int8 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef char v64char __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef uint32_t v8uint64 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef uint32_t v16uint32 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef uint16_t v32uint16 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef uint8_t v64uint8 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));

typedef int32_t v4int64 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef int32_t v8int32 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef int16_t v16int16 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef int8_t v32int8 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef uint32_t v4uint64 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef uint32_t v8uint32 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef uint16_t v16uint16 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef uint8_t v32uint8 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));

typedef int32_t v4int32 __attribute__((__vector_size__(16)))
__attribute__((aligned(__MIN_ALIGNMENT_16)));
typedef int16_t v8int16 __attribute__((__vector_size__(16)))
__attribute__((aligned(__MIN_ALIGNMENT_16)));
typedef char v16char __attribute__((__vector_size__(16)));
typedef int8_t v16int8 __attribute__((__vector_size__(16)))
__attribute__((aligned(__MIN_ALIGNMENT_16)));
typedef uint32_t v4uint32 __attribute__((__vector_size__(16)))
__attribute__((aligned(__MIN_ALIGNMENT_16)));
typedef uint16_t v8uint16 __attribute__((__vector_size__(16)))
__attribute__((aligned(__MIN_ALIGNMENT_16)));
typedef uint8_t v16uint8 __attribute__((__vector_size__(16)))
__attribute__((aligned(__MIN_ALIGNMENT_16)));

#if __AIEARCH__ == 10
typedef __acc48 v8acc48 __attribute__((__vector_size__(48)));
typedef __acc48 v4acc80 __attribute__((__vector_size__(48)));
typedef __acc48 v16acc48 __attribute__((__vector_size__(96)));
typedef __acc48 v8acc80 __attribute__((__vector_size__(96)));
#endif //__AIEARCH__ == 10

/*64-bit type is handled as v2int32 */
typedef int32_t v2int32 __attribute__((__vector_size__(8)));

#if (__AIEARCH__ == 20) || (__AIEARCH__ == 21) || (__AIEARCH__ == 22)
typedef int32_t addr_t;
/* bfloat16 type */
typedef __bf16 bfloat16;
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
typedef char v8char __attribute__((__vector_size__(8)));
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
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef acc32 v16acc32 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef acc32 v32acc32 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef acc64 v4acc64 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef acc64 v8acc64 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef acc64 v16acc64 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
/* accumulator float types in aiev2 */
typedef __accfloat accfloat;
typedef accfloat v8accfloat __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef accfloat v16accfloat __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef accfloat v32accfloat __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
/* vector bfloat16 types in aiev2 */
typedef __bf16 v2bfloat16 __attribute__((__vector_size__(4)));
typedef __bf16 v4bfloat16 __attribute__((__vector_size__(8)));
typedef __bf16 v8bfloat16 __attribute__((__vector_size__(16)));
typedef __bf16 v16bfloat16 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef __bf16 v32bfloat16 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef __bf16 v64bfloat16 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
/*Sparse vector types*/
typedef unsigned _BitInt(128) sparsity_t __attribute__((aligned(16)));
/* sparse vector types in aiev2*/
struct v256int4_sparse {
  v128int4 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v128int8_sparse {
  v64int8 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v64int16_sparse {
  v32int16 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v256uint4_sparse {
  v128uint4 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v128uint8_sparse {
  v64uint8 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v64uint16_sparse {
  v32uint16 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v64bfloat16_sparse {
  v32bfloat16 data;
  sparsity_t mask;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));

/*
Compressed vector types are defined as a struct of an integer.
The integer in the struct does not represent anything and is never used.
These types are used to overload the compressed load intrinsics and
represent pointers to the memory location where the compressed vector is
to be loaded from (e.g. v64int4_compress *& is used instead of
v64int4_compress).
*/
/*Compressed 256-bit integer vector types*/
struct v64int4_compress {
  int x;
};
struct v32int8_compress {
  int x;
};
struct v16int16_compress {
  int x;
};
struct v64uint4_compress {
  int x;
};
struct v32uint8_compress {
  int x;
};
struct v16uint16_compress {
  int x;
};
struct v8int32_compress {
  int x;
};
struct v8uint32_compress {
  int x;
};

/*Compressed 512-bit integer vector types*/
struct v128int4_compress {
  int x;
};
struct v64int8_compress {
  int x;
};
struct v32int16_compress {
  int x;
};
struct v128uint4_compress {
  int x;
};
struct v64uint8_compress {
  int x;
};
struct v32uint16_compress {
  int x;
};
struct v16int32_compress {
  int x;
};
struct v16uint32_compress {
  int x;
};

/*Compressed floating-point vector types*/
struct v16bfloat16_compress {
  int x;
};
struct v32bfloat16_compress {
  int x;
};
/*Compressed sparse vector types*/
struct v256int4_sparse_compress {
  int x;
};
struct v128int8_sparse_compress {
  int x;
};
struct v64int16_sparse_compress {
  int x;
};
struct v256uint4_sparse_compress {
  int x;
};
struct v128uint8_sparse_compress {
  int x;
};
struct v64uint16_sparse_compress {
  int x;
};
struct v64bfloat16_sparse_compress {
  int x;
};

#endif //(__AIEARCH__ == 20) || (__AIEARCH__ == 21) || (__AIEARCH__ == 22)

#if (__AIEARCH__ == 21) || (__AIEARCH__ == 22)
typedef float v2float __attribute__((__vector_size__(8)));
typedef acc32 v64acc32 __attribute__((__vector_size__(256)))
__attribute__((aligned(__MIN_ALIGNMENT_256)));
typedef acc64 v32acc64 __attribute__((__vector_size__(256)))
__attribute__((aligned(__MIN_ALIGNMENT_256)));
typedef accfloat v64accfloat __attribute__((__vector_size__(256)))
__attribute__((aligned(__MIN_ALIGNMENT_256)));
typedef int32_t v64int32 __attribute__((__vector_size__(256)))
__attribute__((aligned(__MIN_ALIGNMENT_256)));

/*
Unaligned vector types are defined as a struct with one unused int member.
This gives the struct a 4-byte size for correct word-sized offset
computations.
These types are used to overload the unaligned load intrinsics and
represent pointers to the memory location where the unaligned vector is
to be loaded from (e.g. v256bfp16ebs16_sparse_unaligned *& is used instead of
v256bfp16ebs16_sparse_unaligned).
*/
struct v512uint4_sparse_unaligned {
  int _d;
};
struct v256uint4_sparse_unaligned {
  int _d;
};
struct v256uint8_sparse_unaligned {
  int _d;
};
struct v128uint8_sparse_unaligned {
  int _d;
};
struct v128uint16_sparse_unaligned {
  int _d;
};
struct v64uint16_sparse_unaligned {
  int _d;
};
struct v512int4_sparse_unaligned {
  int _d;
};
struct v256int4_sparse_unaligned {
  int _d;
};
struct v256int8_sparse_unaligned {
  int _d;
};
struct v128int8_sparse_unaligned {
  int _d;
};
struct v128int16_sparse_unaligned {
  int _d;
};
struct v64int16_sparse_unaligned {
  int _d;
};
struct v256bfp16ebs16_sparse_unaligned {
  int _d;
};
struct v256bfp16ebs8_sparse_unaligned {
  int _d;
};
struct v128bfp16ebs16_sparse_unaligned {
  int _d;
};
struct v128bfp16ebs8_sparse_unaligned {
  int _d;
};
struct v128bfp16ebs16_unaligned {
  int _d;
};
struct v128bfp16ebs8_unaligned {
  int _d;
};
struct v64bfp16ebs16_unaligned {
  int _d;
};
struct v64bfp16ebs8_unaligned {
  int _d;
};
struct v64mx9_unaligned {
  int _d;
};
struct v128mx6_unaligned {
  int _d;
};
struct v64mx6_unaligned {
  int _d;
};
struct v128mx4_unaligned {
  int _d;
};
struct v64mx4_unaligned {
  int _d;
};
struct v256mx6_unaligned {
  int _d;
};
struct v256mx4_unaligned {
  int _d;
};

typedef v32int32 sparse_fifo_t;
struct fifo_state_t {
  sparse_fifo_t fifo;
  int pos;
  v16int32 extra;
} __attribute__((return_in_regs));
#endif // (__AIEARCH__ == 21) || (__AIEARCH__ == 22)

#if (__AIEARCH__ == 21)
/* Block floating-point(bpf16) */
// v64bfp16ebs16: The data is four blocks in bfp16ebs16 format and is written to
// an EX register. The mantissas of the four blocks are written to the X part of
// the register. The exponents are written to the E part of the register. As the
// size of the E register (64 bits) is twice of the size of the exponents (32
// bits), the exponent for each block is stored twice in the E register:
// exp0::exp0::exp1::exp1::exp2::exp2::exp3::exp3
struct v64bfp16ebs16 {
  v64int8 mantissa;
  v8int8 exponent;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));
struct v64bfp16ebs8 {
  v64int8 mantissa;
  v8int8 exponent;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));
struct v128bfp16ebs16 {
  v64int8 mantissaX0;
  v64int8 mantissaX1;
  v8int8 exponentE0;
  v8int8 exponentE1;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));
struct v128bfp16ebs8 {
  v64int8 mantissaX0;
  v64int8 mantissaX1;
  v8int8 exponentE0;
  v8int8 exponentE1;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

// AIE2P-larger (1280-bit) sparse vector types — Followup H, G-T3.6-003.
//
// These are the AIE-API-style "wide sparse" types: each holds a pair of the
// AIEv2-sized (640-bit) sparse vectors. They were previously declared as
// empty-stub structs in aie2p_aie_api_compat.h:53-66 (now removed), which
// blocked any non-trivial implementation of the forward-decls in that file.
//
// Defining them as composite structs (mirroring the v128bfp16ebs16 pattern
// at lines 563-576 above) makes:
//   - extract_v128int8_sparse(v256int8_sparse, int)  -> field access
//   - concat(v128int8_sparse, v128int8_sparse)        -> brace-init
//   - set_v256int8_sparse / insert(v256int8_sparse, ...) -> field write
// all implementable as pure header functions in aie2p_upd_ext.h.
//
// Bodies live at aie2p_upd_ext.h (tail), in the
// "AIE2P-larger sparse <-> smaller sparse conversion" block.
struct v512uint4_sparse {
  v256uint4_sparse lo;
  v256uint4_sparse hi;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v256uint8_sparse {
  v128uint8_sparse lo;
  v128uint8_sparse hi;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v128uint16_sparse {
  v64uint16_sparse lo;
  v64uint16_sparse hi;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v512int4_sparse {
  v256int4_sparse lo;
  v256int4_sparse hi;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v256int8_sparse {
  v128int8_sparse lo;
  v128int8_sparse hi;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));
struct v128int16_sparse {
  v64int16_sparse lo;
  v64int16_sparse hi;
} __attribute__((packed)) __attribute__((aligned(16)))
__attribute__((return_in_regs)) __attribute__((is_sparse));

#endif // __AIEARCH__ == 21

#if __AIEARCH__ == 22

/*  fp16 type */
typedef _Float16 fp16;

typedef fp16 float16;

/* fp8 type */
typedef struct float8 {
  unsigned _BitInt(8) data;
} fp8;

/* bf8 type */
typedef struct bfloat8 {
  unsigned _BitInt(8) data;
} bf8;

/* Vector data types using 8-bit floating point (fp8) */
struct v2float8 {
  v2int8 data;
};

struct v4float8 {
  int32_t data;
};

struct v8float8 {
  v8int8 data;
};

struct v16float8 {
  v16int8 data;
};

struct v32float8 {
  v32int8 data;
};

struct v64float8 {
  v64int8 data;
};

struct v128float8 {
  v128int8 data;
};

struct v256float8 {
  v64acc32 data;
};

/* Vector data types using 8-bit brain floating point (bfp8) */
struct v2bfloat8 {
  v2int8 data;
};

struct v4bfloat8 {
  int32_t data;
};

struct v8bfloat8 {
  v8int8 data;
};

struct v16bfloat8 {
  v16int8 data;
};

struct v32bfloat8 {
  v32int8 data;
};

struct v64bfloat8 {
  v64int8 data;
};

struct v128bfloat8 {
  v128int8 data;
};

struct v256bfloat8 {
  v64acc32 data;
};

/* Vector data types using 16-bit floating point (fp16) */
typedef float16 v2float16 __attribute__((__vector_size__(4)));
typedef float16 v4float16 __attribute__((__vector_size__(8)));
typedef float16 v8float16 __attribute__((__vector_size__(16)));
typedef float16 v16float16 __attribute__((__vector_size__(32)))
__attribute__((aligned(__MIN_ALIGNMENT_32)));
typedef float16 v32float16 __attribute__((__vector_size__(64)))
__attribute__((aligned(__MIN_ALIGNMENT_64)));
typedef float16 v64float16 __attribute__((__vector_size__(128)))
__attribute__((aligned(__MIN_ALIGNMENT_128)));
typedef float16 v128float16 __attribute__((__vector_size__(256)))
__attribute__((aligned(__MIN_ALIGNMENT_256)));

/*
Block floating point(BFP)

In addition to new bfp11 and bfp13 types, there are other basic difference
for block floating point types in aie2ps. No more ebs8(exponent block
sharing), only ebs16 is supported In addition to the mantissas and exponents,
every two mantissas share a sub-tile shift bit, which will be used to
decrease the shared exponent by one for the pair of mantissas. New G register
is introduced.
*/

/*
BFP16(aka mx9)
v64mx9:
4 blocks of 16 8-bit mantissas, 1 8-bitshared exponent along with 64-bit
sub-tile shift bits.

* 4 X 16  X 8 --> X
* 4 X  1  X 8 --> E(64-bit, replicate 32-bits)
* 64(with zero padding) --> G
*/

// 320 bit
struct v32mx9 {
  v8int32 mantissa;
  int32_t tileShift;
  int32_t exponent;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

// 640 bit
struct v64mx9 {
  v16int32 mantissa;
  v2int32 tileShift;
  v2int32 exponent;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

// 1280 bit
struct v128mx9 {
  v16int32 mantissaX0;
  v16int32 mantissaX1;
  v2int32 tileShiftG0;
  v2int32 tileShiftG1;
  v2int32 exponentE0;
  v2int32 exponentE1;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

// 2560 bit
struct v256mx9 {
  struct v128mx9 l, h;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

/*
BFP13(aka mx6)
v128mx6:
8 blocks of 16 4-bit mantissas(LSB), 16 1-bit mantissa(sign bit, MSB), 1
8-bit shared exponent along with 64-bit sub-tile shift bits.

* 8 X 16 X 4 --> X
* 8 X 16 X 1 --> F
* 8 X  1 X 8 --> E
* 64-bit     --> G
*/

// 384 bit
struct v64mx6 {
  v8int32 mantissa;
  v2int32 sign;
  int32 tileShift;
  int32 exponent;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

// 768 bit
struct v128mx6 {
  v8int32 mantissaX0;
  v8int32 mantissaX1;
  v2int32 signF0;
  v2int32 signF1;
  int32 tileShiftG0;
  int32 tileShiftG1;
  int32 exponentE0;
  int32 exponentE1;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

// 1536 bit
struct v256mx6 {
  v8int32 mantissaX0;
  v8int32 mantissaX1;
  v8int32 mantissaX2;
  v8int32 mantissaX3;
  v2int32 signF0;
  v2int32 signF1;
  v2int32 signF2;
  v2int32 signF3;
  int32 tileShiftG0;
  int32 tileShiftG1;
  int32 tileShiftG2;
  int32 tileShiftG3;
  int32 exponentE0;
  int32 exponentE1;
  int32 exponentE2;
  int32 exponentE3;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

/*
BFP11(aka mx4)
bfp11 is emulated using bfp13.  bfp11 has 3 mantissa bits, so zeroing the 2
msb bits of bfp13 to make it bfp11.
*/

// 384 bit
struct v64mx4 {
  v8int32 mantissa;
  v2int32 sign;
  int32 tileShift;
  int32 exponent;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

// 768 bit
struct v128mx4 {
  v8int32 mantissaX0;
  v8int32 mantissaX1;
  v2int32 signF0;
  v2int32 signF1;
  int32 tileShiftG0;
  int32 tileShiftG1;
  int32 exponentE0;
  int32 exponentE1;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

// 1536 bit
struct v256mx4 {
  v8int32 mantissaX0;
  v8int32 mantissaX1;
  v8int32 mantissaX2;
  v8int32 mantissaX3;
  v2int32 signF0;
  v2int32 signF1;
  v2int32 signF2;
  v2int32 signF3;
  int32 tileShiftG0;
  int32 tileShiftG1;
  int32 tileShiftG2;
  int32 tileShiftG3;
  int32 exponentE0;
  int32 exponentE1;
  int32 exponentE2;
  int32 exponentE3;
} __attribute__((packed)) __attribute__((return_in_regs))
__attribute__((aligned(8)));

typedef struct v32mx9 v32bfp16p;
typedef struct v64mx9 v64bfp16p;
typedef struct v128mx9 v128bfp16p;
typedef struct v256mx9 v256bfp16p;
typedef struct v64mx6 v64bfp13p;
typedef struct v128mx6 v128bfp13p;
typedef struct v256mx6 v256bfp13p;
typedef struct v64mx4 v64bfp11p;
typedef struct v128mx4 v128bfp11p;
typedef struct v256mx4 v256bfp11p;

#endif //__AIEARCH__ == 22

#endif /*  __AIEBASE_TYPEDEFS_H */
