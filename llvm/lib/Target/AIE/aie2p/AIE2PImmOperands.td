//===-- AIE2PImmSDNodes.td - -----------------------------*- tablegen --*-==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//


def c1u : txxu<1, i32>;
def c2u : txxu<2, i32>;
def c3u : txxu<3, i32>;
def c3s : txxs<3, i32>;
def c4u : txxu<4, i32>;
def c4s : txxs<4, i20>;
def c5u : txxu<5, i32>;
def c6u : txxu<6, i32>;
def c6s : txxs<6, i32>;
def c7s : txxs<7, i32>;
def c7s_negated : txxsn<7,i32>;
def c8s : txxs<8, i32>;
def c8u : txxu<8, i32>;
def c9s : txxs<9, i32>;
def c10s : txxs<10, i32>;
def c11s : txxs<11, i32>;
def c12s : txxs<12, i32>;
def c12u : txxu<12, i32>;
def c16s : txxs<16, i32>;
def c16u : txxu<16, i32>;
def c18u : txxu<18, i32>;
def c20s : txxs<20, i32>;
def c20u : txxu<20, i32>;
def c24s : txxs<24, i32>;
def c24u : txxu<24, i32>;
def c26s : txxs<26, i32>;
def c40u : txxu<40, i32>;
def c32u : txxu<32, i32>;
def c32s : txxs<32, i32>;

def m_c04s : txxs<4, i32>;

// A signed immediate with n+1 bits and the low-order 1 bits as 0.
// This results in an n-bit encoded number
class immx2 <int n, ValueType vt = i32> : immxstep<n, 2 /*step*/, 1 /**lower-order as 0**/, vt>;

// A signed immediate with n+6 bits and the low-order 6 bits as 0.
// This results in an n-bit encoded number
class immx64 <int n, ValueType vt = i32> : immxstep<n, 64 /**step**/, 6 /**lower-order as 0**/, vt>;

// A signed negative immediate with n+7 bits and the low-order 6 bits as 0 and sign-bit as 1.
// This results in an n-bit encoded number
class immx64_negative <int n> : immxstep<n, 64 /*step*/, 7 /**1 sign-bit + 6 lower-order bits as 0**/>
{
  let isNegative = true;
}

// Immediate for stack adjustments, thirteen bits scaled by 64
def c19s_step64 : immx64<13, i32>;

// Immediate for stack spills, 9 bits scaled by 4
def c12n_step4 : immx4_negative<9, i32>;
// Immediate for stack spills, 9 bits scaled by 16
def c14n_step16 : immx16_negative<9>;
// Immediate for stack spills, 9 bits scaled by 32
def c15n_step32 : immx32_negative<9>;
// Immediate for stack spills, 9 bits scaled by 64
def c16n_step64 : immx64_negative<9>;

// Immediate for indexed are mapped to i20, for encoding/decoding constraints,
// the types are vastly irrelevant. However, if we want to use those predicates
// for ISel patterns, types are relevant.
// Immediate for indexed addressing, 4 bits scaled by 2
def c5s_step2 : immx2<4, i20>;
// Immediate for indexed addressing, 4 bits scaled by 4
def c6s_step4 : immx4<4, i20>;
// Immediate for indexed addressing, 4 bits scaled by 16
def c8s_step16 : immx16<4, i20>;
// Immediate for indexed addressing, 4 bits scaled by 32
def c9s_step32 : immx32<4, i20>;
// Immediate for indexed addressing, 4 bits scaled by 64
def c10s_step64 : immx64<4, i20>;
// Immediate for indexed addressing, 10 bits scaled by 4
def c12s_step4 : immx4<10, i20>;


