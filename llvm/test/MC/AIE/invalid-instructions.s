//===- invalid-instructions.s -----------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: not llvm-mc -triple aie2 %s -o - 2>&1 | FileCheck %s

// CHECK: error: invalid operand for instruction
padda [p0], MCD

// CHECK: error: invalid operand for instruction
vmov.hi cm0, MCD