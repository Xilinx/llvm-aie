//===-- AIE2PSImmOperands.td - -----------------------------*- tablegen --*-==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

def c17n_step128 : immx128_negative<9>;
def c16n_step64 : immx64_negative<9>;
def c19s_step64 : immx64<13, i20>;
def c10s_step64 : immx64<4, i20>;
def c3u : txxu<3, i32>;
def c15n_step32 : immx32_negative<9>;
def c2u : txxu<2, i32>;
def c5u : txxu<5, i32>;
def c32s : txxs<32, i32>;
def c6u : txxu<6, i32>;
def c11u : txxu<11, i32>;
def c8s : txxs<8, i32>;
def c5s_step2 : immx2<4, i20>;
def c7s : txxs<7, i32>;
def c7s_negated : txxsn<7,i32>;
def c11s : txxs<11, i32>;
def c13n_step8 : immx8_negative<9>;
def c4s : txxs<4, i20>;
def c6s_step4 : immx4<4, i20>;
def c9s_step32 : immx32<4, i20>;
def c7s_step8 : immx8<4, i20>;
def c12n_step4 : immx4_negative<9, i32>;
def c8s_step16 : immx16<4, i20>;
def c14n_step16 : immx16_negative<9>;
