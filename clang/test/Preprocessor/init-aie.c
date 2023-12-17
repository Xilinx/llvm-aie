//===- init-aie.c -----------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Check predefinitions for AI Engine
// REQUIRES: aie-registered-target

// RUN: %clang_cc1 -E -dM -triple=aie2 < /dev/null | \
// RUN:     FileCheck -match-full-lines -check-prefix AIE %s
//
// AIE: #define _ILP32 1
// AIE: #define __AIECC__ 1
// AIE: #define __AIENGINE__ 1
// AIE: #define __ATOMIC_ACQUIRE 2
// AIE: #define __ATOMIC_ACQ_REL 4
// AIE: #define __ATOMIC_CONSUME 1
// AIE: #define __ATOMIC_RELAXED 0
// AIE: #define __ATOMIC_RELEASE 3
// AIE: #define __ATOMIC_SEQ_CST 5
// AIE: #define __BIGGEST_ALIGNMENT__ 4
// AIE: #define __BITINT_MAXWIDTH__ 128
// AIE: #define __BOOL_WIDTH__ 8
// AIE: #define __BYTE_ORDER__ __ORDER_LITTLE_ENDIAN__
// AIE: #define __CHAR16_TYPE__ unsigned short
// AIE: #define __CHAR32_TYPE__ unsigned int
// AIE: #define __CHAR_BIT__ 8
// AIE: #define __CLANG_ATOMIC_BOOL_LOCK_FREE 1
// AIE: #define __CLANG_ATOMIC_CHAR16_T_LOCK_FREE 1
// AIE: #define __CLANG_ATOMIC_CHAR32_T_LOCK_FREE 1
// AIE: #define __CLANG_ATOMIC_CHAR_LOCK_FREE 1
// AIE: #define __CLANG_ATOMIC_INT_LOCK_FREE 1
// AIE: #define __CLANG_ATOMIC_LLONG_LOCK_FREE 1
// AIE: #define __CLANG_ATOMIC_LONG_LOCK_FREE 1
// AIE: #define __CLANG_ATOMIC_POINTER_LOCK_FREE 1
// AIE: #define __CLANG_ATOMIC_SHORT_LOCK_FREE 1
// AIE: #define __CLANG_ATOMIC_WCHAR_T_LOCK_FREE 1
// AIE: #define __CONSTANT_CFSTRINGS__ 1
// AIE: #define __DBL_DECIMAL_DIG__ 17
// AIE: #define __DBL_DENORM_MIN__ 4.9406564584124654e-324
// AIE: #define __DBL_DIG__ 15
// AIE: #define __DBL_EPSILON__ 2.2204460492503131e-16
// AIE: #define __DBL_HAS_DENORM__ 1
// AIE: #define __DBL_HAS_INFINITY__ 1
// AIE: #define __DBL_HAS_QUIET_NAN__ 1
// AIE: #define __DBL_MANT_DIG__ 53
// AIE: #define __DBL_MAX_10_EXP__ 308
// AIE: #define __DBL_MAX_EXP__ 1024
// AIE: #define __DBL_MAX__ 1.7976931348623157e+308
// AIE: #define __DBL_MIN_10_EXP__ (-307)
// AIE: #define __DBL_MIN_EXP__ (-1021)
// AIE: #define __DBL_MIN__ 2.2250738585072014e-308
// AIE: #define __DECIMAL_DIG__ __LDBL_DECIMAL_DIG__
// AIE: #define __ELF__ 1
// AIE: #define __FINITE_MATH_ONLY__ 0
// AIE: #define __FLT_DECIMAL_DIG__ 9
// AIE: #define __FLT_DENORM_MIN__ 1.40129846e-45F
// AIE: #define __FLT_DIG__ 6
// AIE: #define __FLT_EPSILON__ 1.19209290e-7F
// AIE: #define __FLT_HAS_DENORM__ 1
// AIE: #define __FLT_HAS_INFINITY__ 1
// AIE: #define __FLT_HAS_QUIET_NAN__ 1
// AIE: #define __FLT_MANT_DIG__ 24
// AIE: #define __FLT_MAX_10_EXP__ 38
// AIE: #define __FLT_MAX_EXP__ 128
// AIE: #define __FLT_MAX__ 3.40282347e+38F
// AIE: #define __FLT_MIN_10_EXP__ (-37)
// AIE: #define __FLT_MIN_EXP__ (-125)
// AIE: #define __FLT_MIN__ 1.17549435e-38F
// AIE: #define __FLT_RADIX__ 2
// AIE: #define __ILP32__ 1
// AIE: #define __INT16_C_SUFFIX__
// AIE: #define __INT16_FMTd__ "hd"
// AIE: #define __INT16_FMTi__ "hi"
// AIE: #define __INT16_MAX__ 32767
// AIE: #define __INT16_TYPE__ short
// AIE: #define __INT32_C_SUFFIX__
// AIE: #define __INT32_FMTd__ "d"
// AIE: #define __INT32_FMTi__ "i"
// AIE: #define __INT32_MAX__ 2147483647
// AIE: #define __INT32_TYPE__ int
// AIE: #define __INT64_C_SUFFIX__ LL
// AIE: #define __INT64_FMTd__ "lld"
// AIE: #define __INT64_FMTi__ "lli"
// AIE: #define __INT64_MAX__ 9223372036854775807LL
// AIE: #define __INT64_TYPE__ long long int
// AIE: #define __INT8_C_SUFFIX__
// AIE: #define __INT8_FMTd__ "hhd"
// AIE: #define __INT8_FMTi__ "hhi"
// AIE: #define __INT8_MAX__ 127
// AIE: #define __INT8_TYPE__ signed char
// AIE: #define __INTMAX_C_SUFFIX__ LL
// AIE: #define __INTMAX_FMTd__ "lld"
// AIE: #define __INTMAX_FMTi__ "lli"
// AIE: #define __INTMAX_MAX__ 9223372036854775807LL
// AIE: #define __INTMAX_TYPE__ long long int
// AIE: #define __INTMAX_WIDTH__ 64
// AIE: #define __INTPTR_FMTd__ "d"
// AIE: #define __INTPTR_FMTi__ "i"
// AIE: #define __INTPTR_MAX__ 2147483647
// AIE: #define __INTPTR_TYPE__ int
// AIE: #define __INTPTR_WIDTH__ 32
// AIE: #define __INT_FAST16_FMTd__ "hd"
// AIE: #define __INT_FAST16_FMTi__ "hi"
// AIE: #define __INT_FAST16_MAX__ 32767
// AIE: #define __INT_FAST16_TYPE__ short
// AIE: #define __INT_FAST16_WIDTH__ 16
// AIE: #define __INT_FAST32_FMTd__ "d"
// AIE: #define __INT_FAST32_FMTi__ "i"
// AIE: #define __INT_FAST32_MAX__ 2147483647
// AIE: #define __INT_FAST32_TYPE__ int
// AIE: #define __INT_FAST32_WIDTH__ 32
// AIE: #define __INT_FAST64_FMTd__ "lld"
// AIE: #define __INT_FAST64_FMTi__ "lli"
// AIE: #define __INT_FAST64_MAX__ 9223372036854775807LL
// AIE: #define __INT_FAST64_TYPE__ long long int
// AIE: #define __INT_FAST64_WIDTH__ 64
// AIE: #define __INT_FAST8_FMTd__ "hhd"
// AIE: #define __INT_FAST8_FMTi__ "hhi"
// AIE: #define __INT_FAST8_MAX__ 127
// AIE: #define __INT_FAST8_TYPE__ signed char
// AIE: #define __INT_FAST8_WIDTH__ 8
// AIE: #define __INT_LEAST16_FMTd__ "hd"
// AIE: #define __INT_LEAST16_FMTi__ "hi"
// AIE: #define __INT_LEAST16_MAX__ 32767
// AIE: #define __INT_LEAST16_TYPE__ short
// AIE: #define __INT_LEAST16_WIDTH__ 16
// AIE: #define __INT_LEAST32_FMTd__ "d"
// AIE: #define __INT_LEAST32_FMTi__ "i"
// AIE: #define __INT_LEAST32_MAX__ 2147483647
// AIE: #define __INT_LEAST32_TYPE__ int
// AIE: #define __INT_LEAST32_WIDTH__ 32
// AIE: #define __INT_LEAST64_FMTd__ "lld"
// AIE: #define __INT_LEAST64_FMTi__ "lli"
// AIE: #define __INT_LEAST64_MAX__ 9223372036854775807LL
// AIE: #define __INT_LEAST64_TYPE__ long long int
// AIE: #define __INT_LEAST64_WIDTH__ 64
// AIE: #define __INT_LEAST8_FMTd__ "hhd"
// AIE: #define __INT_LEAST8_FMTi__ "hhi"
// AIE: #define __INT_LEAST8_MAX__ 127
// AIE: #define __INT_LEAST8_TYPE__ signed char
// AIE: #define __INT_LEAST8_WIDTH__ 8
// AIE: #define __INT_MAX__ 2147483647
// AIE: #define __INT_WIDTH__ 32
// AIE: #define __LDBL_DECIMAL_DIG__ 17
// AIE: #define __LDBL_DENORM_MIN__ 4.9406564584124654e-324L
// AIE: #define __LDBL_DIG__ 15
// AIE: #define __LDBL_EPSILON__ 2.2204460492503131e-16L
// AIE: #define __LDBL_HAS_DENORM__ 1
// AIE: #define __LDBL_HAS_INFINITY__ 1
// AIE: #define __LDBL_HAS_QUIET_NAN__ 1
// AIE: #define __LDBL_MANT_DIG__ 53
// AIE: #define __LDBL_MAX_10_EXP__ 308
// AIE: #define __LDBL_MAX_EXP__ 1024
// AIE: #define __LDBL_MAX__ 1.7976931348623157e+308L
// AIE: #define __LDBL_MIN_10_EXP__ (-307)
// AIE: #define __LDBL_MIN_EXP__ (-1021)
// AIE: #define __LDBL_MIN__ 2.2250738585072014e-308L
// AIE: #define __LITTLE_ENDIAN__ 1
// AIE: #define __LLONG_WIDTH__ 64
// AIE: #define __LONG_LONG_MAX__ 9223372036854775807LL
// AIE: #define __LONG_MAX__ 2147483647L
// AIE: #define __LONG_WIDTH__ 32
// AIE: #define __NO_INLINE__ 1
// AIE: #define __NO_MATH_ERRNO__ 1
// AIE: #define __OBJC_BOOL_IS_BOOL 0
// AIE: #define __OPENCL_MEMORY_SCOPE_ALL_SVM_DEVICES 3
// AIE: #define __OPENCL_MEMORY_SCOPE_DEVICE 2
// AIE: #define __OPENCL_MEMORY_SCOPE_SUB_GROUP 4
// AIE: #define __OPENCL_MEMORY_SCOPE_WORK_GROUP 1
// AIE: #define __OPENCL_MEMORY_SCOPE_WORK_ITEM 0
// AIE: #define __ORDER_BIG_ENDIAN__ 4321
// AIE: #define __ORDER_LITTLE_ENDIAN__ 1234
// AIE: #define __ORDER_PDP_ENDIAN__ 3412
// AIE: #define __PEANO__ 1
// AIE: #define __POINTER_WIDTH__ 32
// AIE: #define __PRAGMA_REDEFINE_EXTNAME 1
// AIE: #define __PTRDIFF_FMTd__ "d"
// AIE: #define __PTRDIFF_FMTi__ "i"
// AIE: #define __PTRDIFF_MAX__ 2147483647
// AIE: #define __PTRDIFF_TYPE__ int
// AIE: #define __PTRDIFF_WIDTH__ 32
// AIE: #define __SCHAR_MAX__ 127
// AIE: #define __SHRT_MAX__ 32767
// AIE: #define __SHRT_WIDTH__ 16
// AIE: #define __SIG_ATOMIC_MAX__ 2147483647
// AIE: #define __SIG_ATOMIC_WIDTH__ 32
// AIE: #define __SIZEOF_DOUBLE__ 8
// AIE: #define __SIZEOF_FLOAT__ 4
// AIE: #define __SIZEOF_INT__ 4
// AIE: #define __SIZEOF_LONG_DOUBLE__ 8
// AIE: #define __SIZEOF_LONG_LONG__ 8
// AIE: #define __SIZEOF_LONG__ 4
// AIE: #define __SIZEOF_POINTER__ 4
// AIE: #define __SIZEOF_PTRDIFF_T__ 4
// AIE: #define __SIZEOF_SHORT__ 2
// AIE: #define __SIZEOF_SIZE_T__ 4
// AIE: #define __SIZEOF_WCHAR_T__ 4
// AIE: #define __SIZEOF_WINT_T__ 4
// AIE: #define __SIZE_FMTX__ "X"
// AIE: #define __SIZE_FMTo__ "o"
// AIE: #define __SIZE_FMTu__ "u"
// AIE: #define __SIZE_FMTx__ "x"
// AIE: #define __SIZE_MAX__ 4294967295U
// AIE: #define __SIZE_TYPE__ unsigned int
// AIE: #define __SIZE_WIDTH__ 32
// AIE: #define __STDC_HOSTED__ 1
// AIE: #define __STDC_UTF_16__ 1
// AIE: #define __STDC_UTF_32__ 1
// AIE: #define __STDC_VERSION__ 201710L
// AIE: #define __STDC__ 1
// AIE: #define __UINT16_C_SUFFIX__
// AIE: #define __UINT16_FMTX__ "hX"
// AIE: #define __UINT16_FMTo__ "ho"
// AIE: #define __UINT16_FMTu__ "hu"
// AIE: #define __UINT16_FMTx__ "hx"
// AIE: #define __UINT16_MAX__ 65535
// AIE: #define __UINT16_TYPE__ unsigned short
// AIE: #define __UINT32_C_SUFFIX__ U
// AIE: #define __UINT32_FMTX__ "X"
// AIE: #define __UINT32_FMTo__ "o"
// AIE: #define __UINT32_FMTu__ "u"
// AIE: #define __UINT32_FMTx__ "x"
// AIE: #define __UINT32_MAX__ 4294967295U
// AIE: #define __UINT32_TYPE__ unsigned int
// AIE: #define __UINT64_C_SUFFIX__ ULL
// AIE: #define __UINT64_FMTX__ "llX"
// AIE: #define __UINT64_FMTo__ "llo"
// AIE: #define __UINT64_FMTu__ "llu"
// AIE: #define __UINT64_FMTx__ "llx"
// AIE: #define __UINT64_MAX__ 18446744073709551615ULL
// AIE: #define __UINT64_TYPE__ long long unsigned int
// AIE: #define __UINT8_C_SUFFIX__
// AIE: #define __UINT8_FMTX__ "hhX"
// AIE: #define __UINT8_FMTo__ "hho"
// AIE: #define __UINT8_FMTu__ "hhu"
// AIE: #define __UINT8_FMTx__ "hhx"
// AIE: #define __UINT8_MAX__ 255
// AIE: #define __UINT8_TYPE__ unsigned char
// AIE: #define __UINTMAX_C_SUFFIX__ ULL
// AIE: #define __UINTMAX_FMTX__ "llX"
// AIE: #define __UINTMAX_FMTo__ "llo"
// AIE: #define __UINTMAX_FMTu__ "llu"
// AIE: #define __UINTMAX_FMTx__ "llx"
// AIE: #define __UINTMAX_MAX__ 18446744073709551615ULL
// AIE: #define __UINTMAX_TYPE__ long long unsigned int
// AIE: #define __UINTMAX_WIDTH__ 64
// AIE: #define __UINTPTR_FMTX__ "X"
// AIE: #define __UINTPTR_FMTo__ "o"
// AIE: #define __UINTPTR_FMTu__ "u"
// AIE: #define __UINTPTR_FMTx__ "x"
// AIE: #define __UINTPTR_MAX__ 4294967295U
// AIE: #define __UINTPTR_TYPE__ unsigned int
// AIE: #define __UINTPTR_WIDTH__ 32
// AIE: #define __UINT_FAST16_FMTX__ "hX"
// AIE: #define __UINT_FAST16_FMTo__ "ho"
// AIE: #define __UINT_FAST16_FMTu__ "hu"
// AIE: #define __UINT_FAST16_FMTx__ "hx"
// AIE: #define __UINT_FAST16_MAX__ 65535
// AIE: #define __UINT_FAST16_TYPE__ unsigned short
// AIE: #define __UINT_FAST32_FMTX__ "X"
// AIE: #define __UINT_FAST32_FMTo__ "o"
// AIE: #define __UINT_FAST32_FMTu__ "u"
// AIE: #define __UINT_FAST32_FMTx__ "x"
// AIE: #define __UINT_FAST32_MAX__ 4294967295U
// AIE: #define __UINT_FAST32_TYPE__ unsigned int
// AIE: #define __UINT_FAST64_FMTX__ "llX"
// AIE: #define __UINT_FAST64_FMTo__ "llo"
// AIE: #define __UINT_FAST64_FMTu__ "llu"
// AIE: #define __UINT_FAST64_FMTx__ "llx"
// AIE: #define __UINT_FAST64_MAX__ 18446744073709551615ULL
// AIE: #define __UINT_FAST64_TYPE__ long long unsigned int
// AIE: #define __UINT_FAST8_FMTX__ "hhX"
// AIE: #define __UINT_FAST8_FMTo__ "hho"
// AIE: #define __UINT_FAST8_FMTu__ "hhu"
// AIE: #define __UINT_FAST8_FMTx__ "hhx"
// AIE: #define __UINT_FAST8_MAX__ 255
// AIE: #define __UINT_FAST8_TYPE__ unsigned char
// AIE: #define __UINT_LEAST16_FMTX__ "hX"
// AIE: #define __UINT_LEAST16_FMTo__ "ho"
// AIE: #define __UINT_LEAST16_FMTu__ "hu"
// AIE: #define __UINT_LEAST16_FMTx__ "hx"
// AIE: #define __UINT_LEAST16_MAX__ 65535
// AIE: #define __UINT_LEAST16_TYPE__ unsigned short
// AIE: #define __UINT_LEAST32_FMTX__ "X"
// AIE: #define __UINT_LEAST32_FMTo__ "o"
// AIE: #define __UINT_LEAST32_FMTu__ "u"
// AIE: #define __UINT_LEAST32_FMTx__ "x"
// AIE: #define __UINT_LEAST32_MAX__ 4294967295U
// AIE: #define __UINT_LEAST32_TYPE__ unsigned int
// AIE: #define __UINT_LEAST64_FMTX__ "llX"
// AIE: #define __UINT_LEAST64_FMTo__ "llo"
// AIE: #define __UINT_LEAST64_FMTu__ "llu"
// AIE: #define __UINT_LEAST64_FMTx__ "llx"
// AIE: #define __UINT_LEAST64_MAX__ 18446744073709551615ULL
// AIE: #define __UINT_LEAST64_TYPE__ long long unsigned int
// AIE: #define __UINT_LEAST8_FMTX__ "hhX"
// AIE: #define __UINT_LEAST8_FMTo__ "hho"
// AIE: #define __UINT_LEAST8_FMTu__ "hhu"
// AIE: #define __UINT_LEAST8_FMTx__ "hhx"
// AIE: #define __UINT_LEAST8_MAX__ 255
// AIE: #define __UINT_LEAST8_TYPE__ unsigned char
// AIE: #define __USER_LABEL_PREFIX__
// AIE: #define __VERSION__ "{{.*}}Clang{{.*}}
// AIE: #define __WCHAR_MAX__ 2147483647
// AIE: #define __WCHAR_TYPE__ int
// AIE: #define __WCHAR_WIDTH__ 32
// AIE: #define __WINT_MAX__ 2147483647
// AIE: #define __WINT_TYPE__ int
// AIE: #define __WINT_WIDTH__ 32
// AIE: #define __aie__ 1
// AIE: #define __clang__ 1
// AIE: #define __clang_literal_encoding__ {{.*}}
// AIE: #define __clang_major__ {{.*}}
// AIE: #define __clang_minor__ {{.*}}
// AIE: #define __clang_patchlevel__ {{.*}}
// AIE: #define __clang_version__ {{.*}}
// AIE: #define __clang_wide_literal_encoding__ {{.*}}
// AIE: #define __llvm__ 1
