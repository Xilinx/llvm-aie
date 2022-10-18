//===- invalid-bundles.s ----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: not llvm-mc -triple aie2 %s -o - 2>&1 | FileCheck %s

// CHECK: incorrect bundle
padda [p0], #4; padda [p0], #4

// CHECK: incorrect bundle
add r0, r0, r0; add r0, r0, r0

// CHECK: incorrect bundle
padda [p1], #4; add r0, r0, r1; mov r2, r1; lda r0, [p0, #0]