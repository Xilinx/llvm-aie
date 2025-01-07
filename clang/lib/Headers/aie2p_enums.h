//===- aie2p_enums.h -------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#ifndef __AIE2P_ENUMS_H__
#define __AIE2P_ENUMS_H__

// ------------------------------------------------------------
// Transpose/shuffle modes

// previous interleaving and deinterleaving modes
#define T8_64x2_lo 0
#define T8_64x2_hi 1
#define T16_32x2_lo 2
#define T16_32x2_hi 3
#define T32_16x2_lo 4
#define T32_16x2_hi 5
#define T64_8x2_lo 6
#define T64_8x2_hi 7
#define T128_4x2_lo 8
#define T128_4x2_hi 9
#define T256_2x2_lo 10
#define T256_2x2_hi 11
#define T128_2x4_lo 12
#define T128_2x4_hi 13
#define T64_2x8_lo 14
#define T64_2x8_hi 15
#define T32_2x16_lo 16
#define T32_2x16_hi 17
#define T16_2x32_lo 18
#define T16_2x32_hi 19
#define T8_2x64_lo 20
#define T8_2x64_hi 21
// previous bypass modes
#define T512_1x2_lo 22
#define T512_1x2_hi 23
// new modes for NL functionality
#define T16_16x4_lo 24
#define T16_16x4_hi 25
#define T16_4x16_lo 26
#define T16_4x16_hi 27
// new modes for FFT and Sparsity functionality
#define T16_8x4 28
#define T16_4x8 29
#define T32_8x4_lo 30
#define T32_8x4_hi 31
#define T32_4x8_lo 32
#define T32_4x8_hi 33
#define T32_4x4 34
#define T8_8x8 35
#define T8_16x4 36
#define T8_4x16 37
#define T16_1x2_flip 38
// new permute reduction modes
#define T16_4x4 39
#define T16_4x2 40
#define T16_2x4 41
#define T16_8x2 42
#define T16_2x8 43
#define T16_16x2 44
#define T16_2x16 45
#define T8_8x4 46
#define T8_4x8 47
// Additional modes in AIE2p
#define T32_8x2 48
#define T32_2x8 49
#define T64_4x2 50
#define T64_2x4 51
#define T16_8x8_lo 52
#define T16_8x8_hi 53
#define T32_1x16_flip 54
#define T8_1x4_flip 55

// Emulated modes for sparse MAC porting
#define T8_8x16_hilo -1
#define T8_16x8_hilo -2

#define T8_64x2 T8_64x2_lo
#define T16_32x2 T16_32x2_lo
#define T32_16x2 T32_16x2_lo
#define T64_8x2 T64_8x2_lo
#define T128_4x2 T128_4x2_lo
#define T256_2x2 T256_2x2_lo
#define T128_2x4 T128_2x4_lo
#define T64_2x8 T64_2x8_lo
#define T32_2x16 T32_2x16_lo
#define T16_2x32 T16_2x32_lo
#define T8_2x64 T8_2x64_lo
#define T512_1x2 T512_1x2_lo
#define T16_16x4 T16_16x4_lo
#define T16_4x16 T16_4x16_lo
#define T32_8x4 T32_8x4_lo
#define T32_4x8 T32_4x8_lo

#define DINTLV_lo_8o16 T8_64x2_lo
#define DINTLV_lo_16o32 T16_32x2_lo
#define DINTLV_lo_32o64 T32_16x2_lo
#define DINTLV_lo_64o128 T64_8x2_lo
#define DINTLV_lo_128o256 T128_4x2_lo
#define DINTLV_lo_256o512 T256_2x2_lo
#define DINTLV_lo_512o1024 T512_1x2_lo
#define DINTLV_hi_8o16 T8_64x2_hi
#define DINTLV_hi_16o32 T16_32x2_hi
#define DINTLV_hi_32o64 T32_16x2_hi
#define DINTLV_hi_64o128 T64_8x2_hi
#define DINTLV_hi_128o256 T128_4x2_hi
#define DINTLV_hi_256o512 T256_2x2_hi
#define DINTLV_hi_512o1024 T512_1x2_hi

#define INTLV_lo_8o16 T8_2x64_lo
#define INTLV_lo_16o32 T16_2x32_lo
#define INTLV_lo_32o64 T32_2x16_lo
#define INTLV_lo_64o128 T64_2x8_lo
#define INTLV_lo_128o256 T128_2x4_lo
#define INTLV_lo_256o512 T256_2x2_lo
#define INTLV_lo_512o1024 T512_1x2_lo
#define INTLV_hi_8o16 T8_2x64_hi
#define INTLV_hi_16o32 T16_2x32_hi
#define INTLV_hi_32o64 T32_2x16_hi
#define INTLV_hi_64o128 T64_2x8_hi
#define INTLV_hi_128o256 T128_2x4_hi
#define INTLV_hi_256o512 T256_2x2_hi
#define INTLV_hi_512o1024 T512_1x2_hi

// interleaving and deinterleaving modes for 64-bit E part of 576-bit EX
#define T64_1x2_lo (0 << 8)
#define T64_1x2_hi (1 << 8)
#define T32_2x2_lo (2 << 8)
#define T32_2x2_hi (3 << 8)
#define T16_4x2_lo (4 << 8)
#define T16_4x2_hi (5 << 8)
#define T8_8x2_lo (6 << 8)
#define T8_8x2_hi (7 << 8)
#define T16_2x4_lo (8 << 8)
#define T16_2x4_hi (9 << 8)
#define T8_2x8_lo (10 << 8)
#define T8_2x8_hi (11 << 8)

#endif // __AIE2P_ENUMS_H__
