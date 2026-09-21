//===- aie2p-vector-shuffle-wide-shapes.s -------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// vshuffle's transpose shapes are every mode whose R * C is 1024 / W, so the
// R x C matrix tiles the (s1, s2) pair with nothing over. aie2p-vector-shuffle.s
// covers the twelve 2-wide ones; these ten are the shapes with C > 2 or R > 32,
// which sit among the half-pair and _flip modes rather than in one run.
//
// The input carries its own index: 16-bit lane i holds 0xF000 | i, so each
// expected value below IS the permutation, readable as a list of source lanes
// rather than as an opaque bit pattern. The high nibble is set on every lane so
// no result can start with a zero the register dump would drop.
//
// Two of these have an oracle outside this model:
//
//  - T16_16x4_lo is what linear_approx.hpp shuffles a load_lut_2x_int8 result
//    with to "discard unused 48b gaps". Its low half below is lanes 0, 4, 8 ...
//    -- every 4th, i.e. the useful 16 bits of each 16b-value-plus-48b-gap entry,
//    landed contiguously in the low 256 bits, which is what the T256_2x2_lo on
//    the next line of that file consumes.
//
//  - T32_8x4_lo/_hi is the shuffle fft_dit_radix4.hpp puts across a butterfly
//    pair, and the low half below is lanes 0, 4, 8 ... at 32-bit granularity --
//    the stride-4 deinterleave a radix-4 DIT stage needs.
//
//===----------------------------------------------------------------------===//

// RUN: llvm-mc -triple aie2p -filetype=obj %s -o %t.o
// RUN: llvm-aie-run %t.o --print-regs --coverage --scratch=0x10000:128 \
// RUN:   | FileCheck %s

// Mode 39 is T16_4x4: 16 elements where the pair holds 64, so it tiles only a
// quarter of it and is a different construction. It says which mode it wanted.
// RUN: llvm-mc -triple aie2p -filetype=obj --defsym BADMODE=1 %s -o %t.bad.o
// RUN: not llvm-aie-run %t.bad.o --scratch=0x10000:128 2>&1 \
// RUN:   | FileCheck %s --check-prefix=BADMODE

	.text
	.globl _start
_start:
	movxm	p0, #0x10000
	movxm	p1, #0x10020
	movxm	p2, #0x10040
	movxm	p3, #0x10060
	// 16-bit lane i of the pair holds 0xF000 | i.
	movxm	r0, #0xF001F000
	st	r0, [p0, #0]
	movxm	r0, #0xF003F002
	st	r0, [p0, #4]
	movxm	r0, #0xF005F004
	st	r0, [p0, #8]
	movxm	r0, #0xF007F006
	st	r0, [p0, #12]
	movxm	r0, #0xF009F008
	st	r0, [p0, #16]
	movxm	r0, #0xF00BF00A
	st	r0, [p0, #20]
	movxm	r0, #0xF00DF00C
	st	r0, [p0, #24]
	movxm	r0, #0xF00FF00E
	st	r0, [p0, #28]
	movxm	r0, #0xF011F010
	st	r0, [p1, #0]
	movxm	r0, #0xF013F012
	st	r0, [p1, #4]
	movxm	r0, #0xF015F014
	st	r0, [p1, #8]
	movxm	r0, #0xF017F016
	st	r0, [p1, #12]
	movxm	r0, #0xF019F018
	st	r0, [p1, #16]
	movxm	r0, #0xF01BF01A
	st	r0, [p1, #20]
	movxm	r0, #0xF01DF01C
	st	r0, [p1, #24]
	movxm	r0, #0xF01FF01E
	st	r0, [p1, #28]
	movxm	r0, #0xF021F020
	st	r0, [p2, #0]
	movxm	r0, #0xF023F022
	st	r0, [p2, #4]
	movxm	r0, #0xF025F024
	st	r0, [p2, #8]
	movxm	r0, #0xF027F026
	st	r0, [p2, #12]
	movxm	r0, #0xF029F028
	st	r0, [p2, #16]
	movxm	r0, #0xF02BF02A
	st	r0, [p2, #20]
	movxm	r0, #0xF02DF02C
	st	r0, [p2, #24]
	movxm	r0, #0xF02FF02E
	st	r0, [p2, #28]
	movxm	r0, #0xF031F030
	st	r0, [p3, #0]
	movxm	r0, #0xF033F032
	st	r0, [p3, #4]
	movxm	r0, #0xF035F034
	st	r0, [p3, #8]
	movxm	r0, #0xF037F036
	st	r0, [p3, #12]
	movxm	r0, #0xF039F038
	st	r0, [p3, #16]
	movxm	r0, #0xF03BF03A
	st	r0, [p3, #20]
	movxm	r0, #0xF03DF03C
	st	r0, [p3, #24]
	movxm	r0, #0xF03FF03E
	st	r0, [p3, #28]
	nop
	nop
	nop
	nop
	nop
	nop
	vlda	x0, [p0, #0]
	vlda	x1, [p2, #0]
	nop
	nop
	nop
	nop
	nop
	nop
.ifdef BADMODE
	movxm	r0, #39
	nop
	nop
	nop
	nop
	nop
	nop
	vshuffle	x2, x0, x1, r0
.else
	movxm	r0, #24
	movxm	r1, #25
	movxm	r2, #26
	movxm	r3, #27
	movxm	r4, #30
	movxm	r5, #31
	movxm	r6, #32
	movxm	r7, #33
	movxm	r8, #52
	movxm	r9, #53
	nop
	nop
	nop
	nop
	nop
	nop
	vshuffle	x2, x0, x1, r0
	vshuffle	x3, x0, x1, r1
	vshuffle	x4, x0, x1, r2
	vshuffle	x5, x0, x1, r3
	vshuffle	x6, x0, x1, r4
	vshuffle	x7, x0, x1, r5
	vshuffle	x8, x0, x1, r6
	vshuffle	x9, x0, x1, r7
	vshuffle	x10, x0, x1, r8
	vshuffle	x11, x0, x1, r9
.endif
	nop
	nop
	nop
	nop
	nop
	nop
	done

// CHECK-DAG: modelled   VSHUFFLE_vec_shuffle_x

// The dump names a 512-bit x register by its two 256-bit halves, wl<N> and
// wh<N>, so each result is two lines of sixteen 16-bit lanes, high lane first.

// mode 24, T16_16x4_lo: the pair as 16x4 of 16-bit elements, transposed;
// this half is columns 0, 1.
// CHECK-DAG: wl2 = 0x{{F03CF038F034F030F02CF028F024F020F01CF018F014F010F00CF008F004F000$}}
// CHECK-DAG: wh2 = 0x{{F03DF039F035F031F02DF029F025F021F01DF019F015F011F00DF009F005F001$}}

// mode 25, T16_16x4_hi: the pair as 16x4 of 16-bit elements, transposed;
// this half is columns 2, 3.
// CHECK-DAG: wl3 = 0x{{F03EF03AF036F032F02EF02AF026F022F01EF01AF016F012F00EF00AF006F002$}}
// CHECK-DAG: wh3 = 0x{{F03FF03BF037F033F02FF02BF027F023F01FF01BF017F013F00FF00BF007F003$}}

// mode 26, T16_4x16_lo: the pair as 4x16 of 16-bit elements, transposed;
// this half is columns 0, 1, 2, 3, 4, 5, 6, 7.
// CHECK-DAG: wl4 = 0x{{F033F023F013F003F032F022F012F002F031F021F011F001F030F020F010F000$}}
// CHECK-DAG: wh4 = 0x{{F037F027F017F007F036F026F016F006F035F025F015F005F034F024F014F004$}}

// mode 27, T16_4x16_hi: the pair as 4x16 of 16-bit elements, transposed;
// this half is columns 8, 9, 10, 11, 12, 13, 14, 15.
// CHECK-DAG: wl5 = 0x{{F03BF02BF01BF00BF03AF02AF01AF00AF039F029F019F009F038F028F018F008$}}
// CHECK-DAG: wh5 = 0x{{F03FF02FF01FF00FF03EF02EF01EF00EF03DF02DF01DF00DF03CF02CF01CF00C$}}

// mode 30, T32_8x4_lo: the pair as 8x4 of 32-bit elements, transposed;
// this half is columns 0, 1.
// CHECK-DAG: wl6 = 0x{{F039F038F031F030F029F028F021F020F019F018F011F010F009F008F001F000$}}
// CHECK-DAG: wh6 = 0x{{F03BF03AF033F032F02BF02AF023F022F01BF01AF013F012F00BF00AF003F002$}}

// mode 31, T32_8x4_hi: the pair as 8x4 of 32-bit elements, transposed;
// this half is columns 2, 3.
// CHECK-DAG: wl7 = 0x{{F03DF03CF035F034F02DF02CF025F024F01DF01CF015F014F00DF00CF005F004$}}
// CHECK-DAG: wh7 = 0x{{F03FF03EF037F036F02FF02EF027F026F01FF01EF017F016F00FF00EF007F006$}}

// mode 32, T32_4x8_lo: the pair as 4x8 of 32-bit elements, transposed;
// this half is columns 0, 1, 2, 3.
// CHECK-DAG: wl8 = 0x{{F033F032F023F022F013F012F003F002F031F030F021F020F011F010F001F000$}}
// CHECK-DAG: wh8 = 0x{{F037F036F027F026F017F016F007F006F035F034F025F024F015F014F005F004$}}

// mode 33, T32_4x8_hi: the pair as 4x8 of 32-bit elements, transposed;
// this half is columns 4, 5, 6, 7.
// CHECK-DAG: wl9 = 0x{{F03BF03AF02BF02AF01BF01AF00BF00AF039F038F029F028F019F018F009F008$}}
// CHECK-DAG: wh9 = 0x{{F03FF03EF02FF02EF01FF01EF00FF00EF03DF03CF02DF02CF01DF01CF00DF00C$}}

// mode 52, T16_8x8_lo: the pair as 8x8 of 16-bit elements, transposed;
// this half is columns 0, 1, 2, 3.
// CHECK-DAG: wl10 = 0x{{F039F031F029F021F019F011F009F001F038F030F028F020F018F010F008F000$}}
// CHECK-DAG: wh10 = 0x{{F03BF033F02BF023F01BF013F00BF003F03AF032F02AF022F01AF012F00AF002$}}

// mode 53, T16_8x8_hi: the pair as 8x8 of 16-bit elements, transposed;
// this half is columns 4, 5, 6, 7.
// CHECK-DAG: wl11 = 0x{{F03DF035F02DF025F01DF015F00DF005F03CF034F02CF024F01CF014F00CF004$}}
// CHECK-DAG: wh11 = 0x{{F03FF037F02FF027F01FF017F00FF007F03EF036F02EF026F01EF016F00EF006$}}

// BADMODE: mode 39 is not one of the transpose shapes this models
