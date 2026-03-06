//===- aie2ps_pp.h ----------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_PP_H__
#define __AIE2PS_PP_H__

#ifndef PP_STRINGIFY
#define PP_STRINGIFY2(x) #x
#define PP_STRINGIFY(x) PP_STRINGIFY2(x)
#endif

#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) a##b
#define PP_CAT3(a, b, c) PP_CAT(a, PP_CAT(b, c))
#define PP_CAT4(a, b, c, d) PP_CAT(a, PP_CAT3(b, c, d))
#define PP_CAT5(a, b, c, d, e) PP_CAT(a, PP_CAT4(b, c, d, e))
#define PP_CAT6(a, b, c, d, e, f) PP_CAT(a, PP_CAT5(b, c, d, e, f))
#define PP_CAT7(a, b, c, d, e, f, g) PP_CAT(a, PP_CAT6(b, c, d, e, f, g))
#define PP_CAT8(a, b, c, d, e, f, g, h) PP_CAT(a, PP_CAT7(b, c, d, e, f, g, h))
#define PP_CAT9(a, b, c, d, e, f, g, h, i)                                     \
  PP_CAT(a, PP_CAT8(b, c, d, e, f, g, h, i))
#define PP_CAT10(a, b, c, d, e, f, g, h, i, j)                                 \
  PP_CAT(a, PP_CAT9(b, c, d, e, f, g, h, i, j))

#define PP_DEF(a) PP_DEF_I(a)
#define PP_DEF_I(a) PP_DEF_II(a)
#define PP_DEF_II(a) PP_DEF_##a
#define PP_DEF_ 1
#define PP_DEF_1 1
#define PP_DEF___programmers_view__ 0

#define PP_LEN(a) PP_LEN_I(a)
#define PP_LEN_I(a) PP_LEN_II(a)
#define PP_LEN_II(a) PP_LEN_##a
#define PP_LEN_ 0
#define PP_LEN_0 1
#define PP_LEN_1 1

#define PP_BOOL(a) PP_BOOL_I(a)
#define PP_BOOL_I(a) PP_BOOL_II(a)
#define PP_BOOL_II(a) PP_BOOL_##a
#define PP_BOOL_0 0
#define PP_BOOL_1 1
#define PP_BOOL_2 1
#define PP_BOOL_3 1
#define PP_BOOL_4 1
#define PP_BOOL_5 1
#define PP_BOOL_6 1
#define PP_BOOL_7 1

#define PP_DIV(a, b)                                                           \
  PP_IF(PP_CMP(a, b), 1,                                                       \
        PP_IF(PP_LT(a, b), 0, PP_IF(PP_CMP(b, 1), a, PP_DIV_I(a, b))))

#define PP_DIV_I(a, b) PP_DIV_##a##_##b
#define PP_DIV_1024_512 2
#define PP_DIV_1024_256 4
#define PP_DIV_1024_128 8
#define PP_DIV_1024_32 32
#define PP_DIV_1024_16 64
#define PP_DIV_1024_2 512
#define PP_DIV_512_256 2
#define PP_DIV_512_128 4
#define PP_DIV_512_32 16
#define PP_DIV_512_16 32
#define PP_DIV_256_128 2
#define PP_DIV_128_64 2
#define PP_DIV_128_32 4
#define PP_DIV_128_16 8
#define PP_DIV_128_8 16
#define PP_DIV_128_4 32
#define PP_DIV_128_2 64
#define PP_DIV_128_1 128
#define PP_DIV_64_2 32
#define PP_DIV_64_4 16
#define PP_DIV_64_8 8
#define PP_DIV_64_16 4
#define PP_DIV_64_32 2
#define PP_DIV_32_2 16
#define PP_DIV_32_4 8
#define PP_DIV_32_8 4
#define PP_DIV_32_16 2
#define PP_DIV_16_2 8
#define PP_DIV_16_4 4
#define PP_DIV_16_8 2
#define PP_DIV_8_2 4
#define PP_DIV_8_4 2
#define PP_DIV_4_2 2

#define PP_MUL(a, b) PP_MUL_I(a, b)
#define PP_MUL_I(a, b) PP_MUL_##a##_##b
#define PP_MUL_1_1 1
#define PP_MUL_1_2 2
#define PP_MUL_1_4 4
#define PP_MUL_1_8 8
#define PP_MUL_1_16 16
#define PP_MUL_2_1 2
#define PP_MUL_2_2 4
#define PP_MUL_2_4 8
#define PP_MUL_2_8 16
#define PP_MUL_2_16 32
#define PP_MUL_4_1 4
#define PP_MUL_4_2 8
#define PP_MUL_4_4 16
#define PP_MUL_4_8 32
#define PP_MUL_8_1 8
#define PP_MUL_8_2 16
#define PP_MUL_8_4 32
#define PP_MUL_8_8 64
#define PP_MUL_16_1 16
#define PP_MUL_16_2 32
#define PP_MUL_16_4 64
#define PP_MUL_32_1 32
#define PP_MUL_32_2 64
#define PP_MUL_32_4 128

#define PP_CMPLX(c) PP_CMPLX_I(c)
#define PP_CMPLX_I(c) PP_CMPLX_##c
#define PP_CMPLX_0
#define PP_CMPLX_1 c
#define PP_CMPLX_
#define PP_CMPLX_c c

#define PP_IF(bit, a, b) PP_IF_I(PP_BOOL(bit), a, b)
#define PP_IF_I(bit, a, b) PP_IF_II(bit, a, b)
#define PP_IF_II(bit, a, b) PP_IF_##bit(a, b)
#define PP_IF_0(a, b) b
#define PP_IF_1(a, b) a

#define PP_NOT(bit) PP_NOT_I(PP_BOOL(bit))
#define PP_NOT_I(bit) PP_NOT_II(bit)
#define PP_NOT_II(bit) PP_NOT_##bit
#define PP_NOT_0 1
#define PP_NOT_1 0

#define PP_AND(x, y) PP_IF(x, PP_IF(y, 1, 0), 0)
#define PP_AND3(x, y, z) PP_AND(x, PP_AND(y, z))
#define PP_AND4(w, x, y, z) PP_AND(PP_AND(w, x), PP_AND(y, z))

#define PP_OR(x, y) PP_IF(x, 1, PP_IF(y, 1, 0))
#define PP_OR3(x, y, z) PP_OR(x, PP_OR(y, z))
#define PP_XOR(x, y) PP_IF(x, PP_NOT(y), y)

#define PP_IFAND(x, y, a, b) PP_IF(PP_AND(x, y), a, b)
#define PP_IFAND3(x, y, z, a, b) PP_IF(PP_AND3(x, y, z), a, b)
#define PP_IFAND4(x, y, z, xx, a, b) PP_IF(PP_AND4(x, y, z, xx), a, b)
#define PP_IFOR(x, y, a, b) PP_IF(PP_OR(x, y), a, b)
#define PP_IFOR3(x, y, z, a, b) PP_IF(PP_OR3(x, y, z), a, b)

#define PP_CMP(a, b) PP_CMP_I(a, b)
#define PP_CMP_I(a, b) PP_CMP_##a##_##b

#define PP_CMP_3072__ 0
#define PP_CMP_1536__ 0

#define PP_CMP_1024_512 0
#define PP_CMP_1024_256 0
#define PP_CMP_1024_128 0
#define PP_CMP_1024_32 0
#define PP_CMP_1024_16 0
#define PP_CMP_1024__ 0
#define PP_CMP_64_64 1
#define PP_CMP_1024_1024 1
#define PP_CMP_2048_2048 1

#define PP_CMP_512_256 0
#define PP_CMP_512_128 0
#define PP_CMP_512_32 0
#define PP_CMP_512_16 0
#define PP_CMP_512_1 0
#define PP_CMP_512__ 0

#define PP_CMP_508__ 0
#define PP_CMP_448__ 0
#define PP_CMP_400__ 0
#define PP_CMP_384__ 0
#define PP_CMP_352__ 0

#define PP_CMP_256_128 0
#define PP_CMP_256_1 0
#define PP_CMP_256__ 0

#define PP_CMP_192__ 0

#define PP_CMP_128_128 0
#define PP_CMP_128_64 0
#define PP_CMP_128_32 0
#define PP_CMP_128_16 0
#define PP_CMP_128_8 0
#define PP_CMP_128_4 0
#define PP_CMP_128_2 0
#define PP_CMP_128_1 0
#define PP_CMP_128__ 0

#define PP_CMP_80_80 1
#define PP_CMP_80_48 0

#define PP_CMP_64_64 1
#define PP_CMP_64_32 0
#define PP_CMP_64_16 0
#define PP_CMP_64_8 0
#define PP_CMP_64_4 0
#define PP_CMP_64_2 0
#define PP_CMP_64_1 0
#define PP_CMP_64__ 0

#define PP_CMP_48_80 0
#define PP_CMP_48_48 1

#define PP_CMP_32_64 0
#define PP_CMP_32_32 1
#define PP_CMP_32_16 0
#define PP_CMP_32_8 0
#define PP_CMP_32_4 0
#define PP_CMP_32_2 0
#define PP_CMP_32_1 0
#define PP_CMP_32__ 0

#define PP_CMP_16_64 0
#define PP_CMP_16_32 0
#define PP_CMP_16_16 1
#define PP_CMP_16_8 0
#define PP_CMP_16_4 0
#define PP_CMP_16_2 0
#define PP_CMP_16_1 0
#define PP_CMP_16_0 0
#define PP_CMP_16__ 0

#define PP_CMP_14__ 0

#define PP_CMP_13_32 0
#define PP_CMP_13_16 0
#define PP_CMP_13_13 1
#define PP_CMP_13_8 0
#define PP_CMP_13_4 0

#define PP_CMP_10_10 1
#define PP_CMP_10_9 0
#define PP_CMP_10_8 0
#define PP_CMP_10_7 0
#define PP_CMP_10_6 0
#define PP_CMP_10_5 0
#define PP_CMP_10_4 0
#define PP_CMP_10_3 0
#define PP_CMP_10_2 0
#define PP_CMP_10_1 0
#define PP_CMP_10_0 0
#define PP_CMP_10__ 0

#define PP_CMP_9_10 0
#define PP_CMP_9_9 1
#define PP_CMP_9_8 0
#define PP_CMP_9_7 0
#define PP_CMP_9_6 0
#define PP_CMP_9_5 0
#define PP_CMP_9_4 0
#define PP_CMP_9_3 0
#define PP_CMP_9_2 0
#define PP_CMP_9_1 0
#define PP_CMP_9_0 0
#define PP_CMP_9__ 0

#define PP_CMP_8_64 0
#define PP_CMP_8_32 0
#define PP_CMP_8_16 0
#define PP_CMP_8_10 0
#define PP_CMP_8_9 0
#define PP_CMP_8_8 1
#define PP_CMP_8_7 0
#define PP_CMP_8_6 0
#define PP_CMP_8_5 0
#define PP_CMP_8_4 0
#define PP_CMP_8_3 0
#define PP_CMP_8_2 0
#define PP_CMP_8_1 0
#define PP_CMP_8_0 0
#define PP_CMP_8__ 0

#define PP_CMP_7_10 0
#define PP_CMP_7_9 0
#define PP_CMP_7_8 0
#define PP_CMP_7_7 1
#define PP_CMP_7_6 0
#define PP_CMP_7_5 0
#define PP_CMP_7_4 0
#define PP_CMP_7_3 0
#define PP_CMP_7_2 0
#define PP_CMP_7_1 0
#define PP_CMP_7_0 0
#define PP_CMP_7__ 0

#define PP_CMP_6_10 0
#define PP_CMP_6_9 0
#define PP_CMP_6_8 0
#define PP_CMP_6_7 0
#define PP_CMP_6_6 1
#define PP_CMP_6_5 0
#define PP_CMP_6_4 0
#define PP_CMP_6_3 0
#define PP_CMP_6_2 0
#define PP_CMP_6_1 0
#define PP_CMP_6_0 0
#define PP_CMP_6__ 0

#define PP_CMP_5_10 0
#define PP_CMP_5_9 0
#define PP_CMP_5_8 0
#define PP_CMP_5_7 0
#define PP_CMP_5_6 0
#define PP_CMP_5_5 1
#define PP_CMP_5_4 0
#define PP_CMP_5_3 0
#define PP_CMP_5_2 0
#define PP_CMP_5_1 0
#define PP_CMP_5_0 0
#define PP_CMP_5__ 0

#define PP_CMP_4_16 0
#define PP_CMP_4_10 0
#define PP_CMP_4_9 0
#define PP_CMP_4_8 0
#define PP_CMP_4_7 0
#define PP_CMP_4_6 0
#define PP_CMP_4_5 0
#define PP_CMP_4_4 1
#define PP_CMP_4_3 0
#define PP_CMP_4_2 0
#define PP_CMP_4_1 0
#define PP_CMP_4_0 0
#define PP_CMP_4__ 0

#define PP_CMP_3_10 0
#define PP_CMP_3_9 0
#define PP_CMP_3_8 0
#define PP_CMP_3_7 0
#define PP_CMP_3_6 0
#define PP_CMP_3_5 0
#define PP_CMP_3_4 0
#define PP_CMP_3_3 1
#define PP_CMP_3_2 0
#define PP_CMP_3_1 0
#define PP_CMP_3_0 0
#define PP_CMP_3__ 0

#define PP_CMP_2_16 0
#define PP_CMP_2_10 0
#define PP_CMP_2_9 0
#define PP_CMP_2_8 0
#define PP_CMP_2_7 0
#define PP_CMP_2_6 0
#define PP_CMP_2_5 0
#define PP_CMP_2_4 0
#define PP_CMP_2_3 0
#define PP_CMP_2_2 1
#define PP_CMP_2_1 0
#define PP_CMP_2_0 0
#define PP_CMP_2__ 0
#define PP_CMP_2_ 0
#define PP_CMP_2__ 0
#define PP_CMP_2__ 0

#define PP_CMP_1_16 0
#define PP_CMP_1_10 0
#define PP_CMP_1_9 0
#define PP_CMP_1_8 0
#define PP_CMP_1_7 0
#define PP_CMP_1_6 0
#define PP_CMP_1_5 0
#define PP_CMP_1_4 0
#define PP_CMP_1_3 0
#define PP_CMP_1_2 0
#define PP_CMP_1_1 1
#define PP_CMP_1_0 0
#define PP_CMP_1__ 0

#define PP_CMP_0_10 0
#define PP_CMP_0_9 0
#define PP_CMP_0_8 0
#define PP_CMP_0_7 0
#define PP_CMP_0_6 0
#define PP_CMP_0_5 0
#define PP_CMP_0_4 0
#define PP_CMP_0_3 0
#define PP_CMP_0_2 0
#define PP_CMP_0_1 0
#define PP_CMP_0_0 1

#define PP_CMP__ 1
#define PP_CMP____ 1

#define PP_CMP_c_c 1
#define PP_CMP__c 0
#define PP_CMP_c_ 0
#define PP_CMP_m_v 0
#define PP_CMP_n_v 0
#define PP_CMP_a_v 0
#define PP_CMP_b_v 0
#define PP_CMP_s_v 0
#define PP_CMP_f_v 0

#define PP_CMP_v_v 1
#define PP_CMP_v_w 0
#define PP_CMP_v_x 0
#define PP_CMP_v_y 0

#define PP_CMP_w_v 0
#define PP_CMP_w_w 1
#define PP_CMP_w_x 0
#define PP_CMP_w_y 0

#define PP_CMP_x_v 0
#define PP_CMP_x_w 0
#define PP_CMP_x_x 1
#define PP_CMP_x_y 0

#define PP_CMP_y_v 0
#define PP_CMP_y_w 0
#define PP_CMP_y_x 0
#define PP_CMP_y_y 1

#define PP_CMP_float_float 1
#define PP_CMP_int_float 0
#define PP_CMP_uint_float 0
#define PP_CMP_acc_float 0
#define PP_CMP_w_float 0
#define PP_CMP_int_int 1
#define PP_CMP_uint_int 0
#define PP_CMP_int_uint 0
#define PP_CMP_uint_uint 1
#define PP_CMP_float_int 0
#define PP_CMP_float_uint 0

#define PP_CMP_v4cfloat_v4cfloat 1
#define PP_CMP_v8float_v4cfloat 0

#define PP_CMP_mul_mul 1
#define PP_CMP_mul_mac 0
#define PP_CMP_mac_mul 0
#define PP_CMP_mac_mac 1

#define PP_CMP___abs 0
#define PP_CMP__abs__abs 1
#define PP_CMP__sym__abs 0
#define PP_CMP__antisym__abs 0
#define PP_CMP__conj_sym__abs 0
#define PP_CMP__conj_antisym__abs 0
#define PP_CMP__max__abs 0
#define PP_CMP__min__abs 0
#define PP_CMP__maxdiff__abs 0

#define PP_GT(a, b) PP_LT(b, a)
#define PP_GE(a, b) PP_OR(PP_GT(a, b), PP_CMP(a, b))
#define PP_LE(a, b) PP_GE(b, a)

#define PP_LT(a, b) PP_IF(PP_CMP(a, b), 0, PP_LT_I(a, b))
#define PP_LT_I(a, b) PP_LT_##a##_##b

#define PP_LT_1024_512 0
#define PP_LT_1024_256 0
#define PP_LT_1024_128 0
#define PP_LT_1024_32 0
#define PP_LT_1024_16 0

#define PP_LT_512_256 0
#define PP_LT_512_128 0
#define PP_LT_512_32 0
#define PP_LT_512_16 0

#define PP_LT_256_128 0

#define PP_LT_128_64 0
#define PP_LT_128_32 0
#define PP_LT_128_16 0
#define PP_LT_128_8 0
#define PP_LT_128_4 0
#define PP_LT_128_2 0
#define PP_LT_128_1 0

#define PP_LT_64_32 0
#define PP_LT_64_16 0
#define PP_LT_64_8 0
#define PP_LT_64_4 0
#define PP_LT_64_2 0
#define PP_LT_64_1 0

#define PP_LT_32_64 1
#define PP_LT_32_16 0
#define PP_LT_32_8 0
#define PP_LT_32_4 0
#define PP_LT_32_2 0
#define PP_LT_32_1 0

#define PP_LT_16_64 1
#define PP_LT_16_32 1
#define PP_LT_16_8 0
#define PP_LT_16_4 0
#define PP_LT_16_2 0
#define PP_LT_16_1 0

#define PP_LT_8_64 1
#define PP_LT_8_32 1
#define PP_LT_8_16 1
#define PP_LT_8_4 0
#define PP_LT_8_2 0
#define PP_LT_8_1 0

#define PP_LT_4_16 1
#define PP_LT_4_8 1
#define PP_LT_4_2 0
#define PP_LT_4_1 0

#define PP_LT_2_16 1
#define PP_LT_2_8 1
#define PP_LT_2_4 1
#define PP_LT_2_1 0

#define PP_LT_1_16 1
#define PP_LT_1_8 1
#define PP_LT_1_4 1
#define PP_LT_1_2 1
#define PP_LT_1_1 0
#define PP_LT_1_0 0

#define PP_LT_0_2 1
#define PP_LT_0_1 1
#define PP_LT_0_0 0

#define PP_MAX(a, b) PP_IF(PP_LT(a, b), b, a)
#define PP_MIN(a, b) PP_IF(PP_LT(a, b), a, b)

#define PP_LOG2(a) PP_LOG2_I(a)
#define PP_LOG2_I(a) PP_LOG2_##a
#define PP_LOG2_32 5
#define PP_LOG2_16 4
#define PP_LOG2_8 3
#define PP_LOG2_4 2
#define PP_LOG2_2 1
#define PP_LOG2_1 0

#define PP_COMMA() ,
#define PP_EMPTY()
#define PP_COMMA_IF(bit) PP_IF(bit, PP_COMMA, PP_EMPTY)()
#define PP_PREFIX_COMMA(a) , a

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define DOXY_SKIP(x) x
#else
#define DOXY_SKIP(x)
#endif

#define PP_CONCAT(a, b) a##b
#define PP_EQUAL(x, y) PP_EQUAL_IMPL(x, y)
#define PP_EQUAL_IMPL(x, y) PP_CAT(PP_EQUAL_, PP_CONCAT(x, y))

#define PP_EQUAL_3232 1
#define PP_EQUAL_6464 1
#define PP_EQUAL_256256 1
#define PP_EQUAL_512512 1
#define PP_EQUAL_10241024 1
#define PP_EQUAL_20482048 1
#define PP_EQUAL_5121024 0
#define PP_EQUAL_5122048 0
#define PP_EQUAL_1024512 0
#define PP_EQUAL_10242048 0
#define PP_EQUAL_2048512 0
#define PP_EQUAL_20481024 0
#define PP_EQUAL_1664 0
#define PP_EQUAL_3264 0
#define PP_EQUAL_DEFAULT 0

// Default case for any other comparisons
#define PP_EQUAL_IMPL(x, y) PP_CAT(PP_EQUAL_, PP_CONCAT(x, y))
#define PP_EQUAL_(...) PP_EQUAL_DEFAULT

#define TYPE_NAME_v2float I64
#define TYPE_NAME_v2cbfloat16 I64
#define TYPE_NAME_v32int4 I128
#define TYPE_NAME_v32uint4 I128
#define TYPE_NAME_v16int8 I128
#define TYPE_NAME_v16uint8 I128
#define TYPE_NAME_v4cint16 I128
#define TYPE_NAME_v8int16 I128
#define TYPE_NAME_v8uint16 I128
#define TYPE_NAME_v2cint32 I128
#define TYPE_NAME_v4int32 I128
#define TYPE_NAME_v4uint32 I128
#define TYPE_NAME_v8bfloat16 bf16_I128
#define TYPE_NAME_v8float16 fp16_I128
#define TYPE_NAME_v16bfloat8 fp8_I128
#define TYPE_NAME_v16float8 fp8_I128
#define TYPE_NAME_v4float I128
#define TYPE_NAME_v4cbfloat16 I128
#define TYPE_NAME_v2cfloat I128
#define TYPE_NAME_v64int4 I256
#define TYPE_NAME_v64uint4 I256
#define TYPE_NAME_v32int8 I256
#define TYPE_NAME_v32uint8 I256
#define TYPE_NAME_v8cint16 I256
#define TYPE_NAME_v16int16 I256
#define TYPE_NAME_v16uint16 I256
#define TYPE_NAME_v4cint32 I256
#define TYPE_NAME_v8int32 I256
#define TYPE_NAME_v8uint32 I256
#define TYPE_NAME_v8accfloat ACC256
#define TYPE_NAME_v4caccfloat ACC256
#define TYPE_NAME_v8acc32 ACC256
#define TYPE_NAME_v4acc64 ACC256
#define TYPE_NAME_v2cacc64 ACC256
#define TYPE_NAME_v16bfloat16 bf16_I256
#define TYPE_NAME_v16float16 fp16_I256
#define TYPE_NAME_v32bfloat8 fp8_I256
#define TYPE_NAME_v32float8 fp8_I256
#define TYPE_NAME_v8float I256
#define TYPE_NAME_v8cbfloat16 I256
#define TYPE_NAME_v4cfloat I256
#define TYPE_NAME_v32mx9 BFP320
#define TYPE_NAME_v64mx6 BFP384
#define TYPE_NAME_v64mx4 BFP384
#define TYPE_NAME_v128int4 I512
#define TYPE_NAME_v128uint4 I512
#define TYPE_NAME_v64int8 I512
#define TYPE_NAME_v64uint8 I512
#define TYPE_NAME_v16cint16 I512
#define TYPE_NAME_v32int16 I512
#define TYPE_NAME_v32uint16 I512
#define TYPE_NAME_v8cint32 I512
#define TYPE_NAME_v16int32 I512
#define TYPE_NAME_v16uint32 I512
#define TYPE_NAME_v16accfloat ACC512
#define TYPE_NAME_v8caccfloat ACC512
#define TYPE_NAME_v16acc32 ACC512
#define TYPE_NAME_v8acc64 ACC512
#define TYPE_NAME_v4cacc64 ACC512
#define TYPE_NAME_v32bfloat16 bf16_I512
#define TYPE_NAME_v32float16 fp16_I512
#define TYPE_NAME_v64bfloat8 fp8_I512
#define TYPE_NAME_v64float8 fp8_I512
#define TYPE_NAME_v16float I512
#define TYPE_NAME_v16cbfloat16 I512
#define TYPE_NAME_v8cfloat I512
#define TYPE_NAME__v256int4_sparse _I_SPARSE640
#define TYPE_NAME__v256uint4_sparse _I_SPARSE640
#define TYPE_NAME__v128int8_sparse _I_SPARSE640
#define TYPE_NAME__v128uint8_sparse _I_SPARSE640
#define TYPE_NAME__v64int16_sparse _I_SPARSE640
#define TYPE_NAME__v64uint16_sparse _I_SPARSE640
#define TYPE_NAME_v64mx9 BFP640
#define TYPE_NAME__v64bfloat16_sparse _I_SPARSE640
#define TYPE_NAME__v64float16_sparse _I_SPARSE640
#define TYPE_NAME__v128bfloat8_sparse _I_SPARSE640
#define TYPE_NAME__v128float8_sparse _I_SPARSE640
#define TYPE_NAME_v128mx6 BFP768
#define TYPE_NAME_v128mx4 BFP768
#define TYPE_NAME_v256int4 I1024
#define TYPE_NAME_v256uint4 I1024
#define TYPE_NAME_v128int8 I1024
#define TYPE_NAME_v128uint8 I1024
#define TYPE_NAME_v32cint16 I1024
#define TYPE_NAME_v64int16 I1024
#define TYPE_NAME_v64uint16 I1024
#define TYPE_NAME_v16cint32 I1024
#define TYPE_NAME_v32int32 I1024
#define TYPE_NAME_v32uint32 I1024
#define TYPE_NAME_v32accfloat ACC1024
#define TYPE_NAME_v16caccfloat ACC1024
#define TYPE_NAME_v32acc32 ACC1024
#define TYPE_NAME_v16acc64 ACC1024
#define TYPE_NAME_v8cacc64 ACC1024
#define TYPE_NAME_v64bfloat16 bf16_I1024
#define TYPE_NAME_v64float16 fp16_I1024
#define TYPE_NAME_v128bfloat8 fp8_I1024
#define TYPE_NAME_v128float8 fp8_I1024
#define TYPE_NAME_v32float I1024
#define TYPE_NAME_v32cbfloat16 I1024
#define TYPE_NAME_v16cfloat I1024
#define TYPE_NAME_v512int4_sparse _I_SPARSE1280
#define TYPE_NAME_v512uint4_sparse _I_SPARSE1280
#define TYPE_NAME_v256int8_sparse _I_SPARSE1280
#define TYPE_NAME_v256uint8_sparse _I_SPARSE1280
#define TYPE_NAME_v128int16_sparse _I_SPARSE1280
#define TYPE_NAME_v128uint16_sparse _I_SPARSE1280
#define TYPE_NAME_v128mx9 BFP1280
#define TYPE_NAME_v128bfloat16_sparse _I_SPARSE1280
#define TYPE_NAME_v128float16_sparse _I_SPARSE1280
#define TYPE_NAME_v256bfloat8_sparse _I_SPARSE1280
#define TYPE_NAME_v256float8_sparse _I_SPARSE1280
#define TYPE_NAME_v256mx6 BFP1536
#define TYPE_NAME_v256mx4 BFP1536
#define TYPE_NAME_v64accfloat ACC2048
#define TYPE_NAME_v32caccfloat ACC2048
#define TYPE_NAME_v64acc32 ACC2048
#define TYPE_NAME_v32acc64 ACC2048
#define TYPE_NAME_v16cacc64 ACC2048
#define TYPE_NAME_v64float I2048
#define TYPE_NAME_v64cbfloat16 I2048
#define TYPE_NAME_v32cfloat I2048
#define TYPE_NAME_v256mx9 BFP2560

#define TYPE_NAME_unsigned
#define TYPE_NAME_signed
#define TYPE_NAME_bfloat8 fp8
#define TYPE_NAME_float8 fp8
#define TYPE_NAME_short I16
#define TYPE_NAME_bfloat16 bf16
#define TYPE_NAME_float16 fp16
#define TYPE_NAME_int I32

// For unsupported types:
#define TYPE_NAME_UNKNOWN UNKNOWN

#define TYPE_NAME(x) TYPE_NAME_IMPL(x)
#define TYPE_NAME_IMPL(x) TYPE_NAME_##x

#define PP_IF_VA_ARGS(cond, ...) PP_IF_VA_ARGS_I(PP_BOOL(cond), __VA_ARGS__)
#define PP_IF_VA_ARGS_I(bit, ...) PP_IF_VA_ARGS_II(bit, __VA_ARGS__)
#define PP_IF_VA_ARGS_II(bit, ...) PP_IF_VA_ARGS_##bit(__VA_ARGS__)
#define PP_IF_VA_ARGS_0(...)
#define PP_IF_VA_ARGS_1(...) __VA_ARGS__

#define VARIANT_BFP16xBFP16_1_4x8_8x16T_v64mx9_v256mx9 8
#define VARIANT_BFP13xBFP13_1_4x16_16x16T_v128mx6_v256mx6 9
#define VARIANT_BFP13xBFP13_1_4x16_16x16T_v128mx4_v256mx4 10

// Map variant detection IDs to configuration enum values
#define VARIANT_TO_CONF(V)                                                     \
  PP_IF(IS_MX9_VARIANT(V), VARIANT_BFP16xBFP16_1_4x8_8x16T,                    \
        PP_IF(IS_MX6_VARIANT(V), VARIANT_BFP13xBFP13_1_4x16_16x16T,            \
              PP_IF(IS_MX4_VARIANT(V), VARIANT_BFP13xBFP13_1_4x16_16x16T, V)))

#define UNPACK_V64MX4(a) a.mantissa, a.sign, a.tileShift, a.exponent
#define UNPACK_V128MX4(a)                                                      \
  a.mantissaX0, a.mantissaX1, a.signF0, a.signF1, a.tileShiftG0,               \
      a.tileShiftG1, a.exponentE0, a.exponentE1
#define UNPACK_V256MX4(a)                                                      \
  a.mantissaX0, a.mantissaX1, a.mantissaX2, a.mantissaX3, a.signF0, a.signF1,  \
      a.signF2, a.signF3, a.tileShiftG0, a.tileShiftG1, a.tileShiftG2,         \
      a.tileShiftG3, a.exponentE0, a.exponentE1, a.exponentE2, a.exponentE3
#define UNPACK_V128MX6(a)                                                      \
  a.mantissaX0, a.mantissaX1, a.signF0, a.signF1, a.tileShiftG0,               \
      a.tileShiftG1, a.exponentE0, a.exponentE1
#define UNPACK_V256MX6(a)                                                      \
  a.mantissaX0, a.mantissaX1, a.mantissaX2, a.mantissaX3, a.signF0, a.signF1,  \
      a.signF2, a.signF3, a.tileShiftG0, a.tileShiftG1, a.tileShiftG2,         \
      a.tileShiftG3, a.exponentE0, a.exponentE1, a.exponentE2, a.exponentE3
#define UNPACK_V64MX9(a) a.mantissa, a.tileShift, a.exponent
#define UNPACK_V256MX9(a)                                                      \
  a.l.mantissaX0, a.l.mantissaX1, a.h.mantissaX0, a.h.mantissaX1,              \
      a.l.tileShiftG0, a.l.tileShiftG1, a.h.tileShiftG0, a.h.tileShiftG1,      \
      a.l.exponentE0, a.l.exponentE1, a.h.exponentE0, a.h.exponentE1

#define IS_MX4_VARIANT(V)                                                      \
  PP_CMP(V, VARIANT_BFP13xBFP13_1_4x16_16x16T_v128mx4_v256mx4)
#define IS_MX6_VARIANT(V)                                                      \
  PP_CMP(V, VARIANT_BFP13xBFP13_1_4x16_16x16T_v128mx6_v256mx6)
#define IS_MX9_VARIANT(V)                                                      \
  PP_CMP(V, VARIANT_BFP16xBFP16_1_4x8_8x16T_v64mx9_v256mx9)
#define NONE_VARIANT_MATCH(V)                                                  \
  PP_NOT(PP_OR3(IS_MX4_VARIANT(V), IS_MX6_VARIANT(V), IS_MX9_VARIANT(V)))
#define IS_FP8_MODE(M) PP_CMP(M, BMODE_FP32_BF9)

#define EMIT_BODY_MUL(OPNAME, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                      VARIANT)                                                 \
  PP_IF_VA_ARGS(IS_MX4_VARIANT(VARIANT),                                       \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    UNPACK_V128MX4(a), UNPACK_V256MX4(b), conf));              \
  PP_IF_VA_ARGS(IS_MX6_VARIANT(VARIANT),                                       \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    UNPACK_V128MX6(a), UNPACK_V256MX6(b), conf));              \
  PP_IF_VA_ARGS(IS_MX9_VARIANT(VARIANT),                                       \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    UNPACK_V64MX9(a), UNPACK_V256MX9(b), conf));               \
  PP_IF_VA_ARGS(NONE_VARIANT_MATCH(VARIANT),                                   \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    PP_IF(IS_FP8_MODE(BMODE), a.data, a),                      \
                    PP_IF(IS_FP8_MODE(BMODE), b.data, b), conf));

#define EMIT_BODY_MAC(OPNAME, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,     \
                      VARIANT)                                                 \
  PP_IF_VA_ARGS(IS_MX4_VARIANT(VARIANT),                                       \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    UNPACK_V128MX4(a), UNPACK_V256MX4(b), acc, conf));         \
  PP_IF_VA_ARGS(IS_MX6_VARIANT(VARIANT),                                       \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    UNPACK_V128MX6(a), UNPACK_V256MX6(b), acc, conf));         \
  PP_IF_VA_ARGS(IS_MX9_VARIANT(VARIANT),                                       \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    UNPACK_V64MX9(a), UNPACK_V256MX9(b), acc, conf));          \
  PP_IF_VA_ARGS(NONE_VARIANT_MATCH(VARIANT),                                   \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    PP_IF(IS_FP8_MODE(BMODE), a.data, a),                      \
                    PP_IF(IS_FP8_MODE(BMODE), b.data, b), acc, conf));

#define EMIT_BODY_ADDMAC(OPNAME, ACC, ACCNUM, BF, CMPLX, DATAX, DATAY, BMODE,  \
                         VARIANT)                                              \
  PP_IF_VA_ARGS(IS_MX4_VARIANT(VARIANT),                                       \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    UNPACK_V128MX4(a), UNPACK_V256MX4(b), acc1, acc2, conf));  \
  PP_IF_VA_ARGS(IS_MX6_VARIANT(VARIANT),                                       \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    UNPACK_V128MX6(a), UNPACK_V256MX6(b), acc1, acc2, conf));  \
  PP_IF_VA_ARGS(IS_MX9_VARIANT(VARIANT),                                       \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    UNPACK_V64MX9(a), UNPACK_V256MX9(b), acc1, acc2, conf));   \
  PP_IF_VA_ARGS(NONE_VARIANT_MATCH(VARIANT),                                   \
                return PP_CAT7(__builtin_aie2ps_, TYPE_NAME(DATAX), _,         \
                               TYPE_NAME(DATAY), _, TYPE_NAME(ACC),            \
                               PP_CAT(PP_IF(BF, _bf##OPNAME, OPNAME), _conf))( \
                    PP_IF(IS_FP8_MODE(BMODE), a.data, a),                      \
                    PP_IF(IS_FP8_MODE(BMODE), b.data, b), acc1, acc2, conf));

#endif // __AIE2PS_PP_H__
