//===- invalid-addressing-modes.s --------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: not llvm-mc -triple aie2p %s -o - 2>&1 | FileCheck %s

// Too many operands
// CHECK: error: unexpected operand
// CHECK: vlda.fill.512 [p1, lf1, r25, 100]
// CHECK:                              ^
vlda.fill.512 [p1, lf1, r25, 100]

// Wrong operands (register expected, got an immediate)
// CHECK: error: unexpected operand
// CHECK: vlda wl1, [0], m1
// CHECK:           ^
vlda wl1, [0], m1

// Invalid operands (gpr register instead of a ptr)
// CHECK: error: invalid operand for instruction
// CHECK: vlda wl1, [r0]
// CHECK:           ^
vlda wl1, [r0]

// Invalid operands (unknown register for this target)
// CHECK: error: operand is not a register, nor a known identifier
// CHECK: vlda wl1, [b10]
// CHECK:           ^
vlda wl1, [b10]
