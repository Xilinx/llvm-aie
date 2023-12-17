//===- chess-compat.cpp -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: %clang -O1 %s --target=aie -S -emit-llvm

void test(int bound, int *A) {
  chess_protect_access v8int32 chess_storage(XA) vec_a_lo; // = undef_v8int32();
  for (int i = 0; i < bound; i++)
    chess_prepare_for_pipelining
    chess_loop_range(16) {
      int *restrict rA = chess_copy(A);
    }
}