//===- aie2p-sparse-fifo-ld.cpp -------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: %clang -O2 %s --target=aie2p -nostdlibinc -S -emit-llvm -o - | FileCheck %s

// CHECK-LABEL: test_fifo_ld_pop_sparse
// CHECK: call { ptr, <32 x i32>, i32, <64 x i8>, <16 x i8> } @llvm.aie2p.fifo.ld.pop.640.unaligned.sparse.p0.p0(
// CHECK: extractvalue { ptr, <32 x i32>, i32, <64 x i8>, <16 x i8> } {{.*}}, 3
// CHECK: extractvalue { ptr, <32 x i32>, i32, <64 x i8>, <16 x i8> } {{.*}}, 4
// CHECK: bitcast <16 x i8> {{.*}} to i128
// CHECK: insertvalue {{.*}} <64 x i8> {{.*}}, 0
// CHECK: insertvalue {{.*}} i128 {{.*}}, 1
v128int8_sparse test_fifo_ld_pop_sparse(v128int8_sparse_unaligned *&p,
                                        fifo_state_t &s) {
  return fifo_ld_pop(p, s);
}

// CHECK-LABEL: test_fifo_ld_pop_sparse_wide
// CHECK: call { ptr, <32 x i32>, i32, <64 x i8>, <16 x i8> } @llvm.aie2p.fifo.ld.pop.640.unaligned.sparse.p0.p0(
// CHECK: call { ptr, <32 x i32>, i32, <64 x i8>, <16 x i8> } @llvm.aie2p.fifo.ld.pop.640.unaligned.sparse.p0.p0(
// CHECK: insertvalue {{.*}} <64 x i8> {{.*}}, 0, 0
// CHECK: insertvalue {{.*}} i128 {{.*}}, 0, 1
// CHECK: insertvalue {{.*}} <64 x i8> {{.*}}, 1, 0
// CHECK: insertvalue {{.*}} i128 {{.*}}, 1, 1
v256int8_sparse test_fifo_ld_pop_sparse_wide(v256int8_sparse_unaligned *&p,
                                             fifo_state_t &s) {
  return fifo_ld_pop(p, s);
}
