//===- invalid-sp-offset.s ----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Verify that SP-relative instructions with out-of-range immediates produce
// a diagnostic instead of crashing (see github.com/Xilinx/llvm-aie/issues/858).

// RUN: not llvm-mc -triple aie2 %s -filetype=obj -o /dev/null 2>&1 | FileCheck %s

// Zero offset is not encodable for negative-only SP-relative addressing modes.
// CHECK: LLVM ERROR: immediate operand value 0 is out of range [-131072, -32]
vlda wl0, [sp, #0]
