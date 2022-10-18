//===- instruction-validation.s ---------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: not llvm-mc -triple aie2 %s -o - 2>&1 | FileCheck %s

// CHECK: PADDA cannot handle immediates > +-2^12
padda [p0], #2060

// CHECK: PADDA immediates must be multiples of 4
padda [p0], #6

// CHECK: PADDA cannot handle immediates > +-2^17 on the stack pointer
padda [sp], #150000

// CHECK: PADDA immediates must be multiples of 32 when incrementing the stack pointer
padda [sp], #22

// CHECK: MOVA cannot handle immediates > +-2^11
mova r0, #2000
