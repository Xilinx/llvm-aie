//===- aie-unaligned-pointer-arith.cpp -------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// RUN: %clang -O1 %s --target=aie2p -nostdlibinc -S -emit-llvm -o - | FileCheck %s --check-prefix=COMMON
// RUN: %clang -O1 %s --target=aie2ps -nostdlibinc -S -emit-llvm -o - | FileCheck %s --check-prefix=COMMON

extern "C" {

// COMMON-LABEL: @sizeof_unaligned_v64bfp16ebs8(
// COMMON:       ret i32 4
int sizeof_unaligned_v64bfp16ebs8() {
  return sizeof(v64bfp16ebs8_unaligned);
}

// COMMON-LABEL: @sizeof_unaligned_v64mx9(
// COMMON:       ret i32 4
int sizeof_unaligned_v64mx9() {
  return sizeof(v64mx9_unaligned);
}

// COMMON-LABEL: @advance_v64bfp16ebs8_unaligned(
// COMMON:       [[IDX_EXT:%.*]] = trunc i32 [[N:%.*]] to i20
// COMMON:       getelementptr inbounds %struct.v64bfp16ebs8_unaligned, ptr %p, i20 [[IDX_EXT]]
// COMMON:       ret ptr
v64bfp16ebs8_unaligned *advance_v64bfp16ebs8_unaligned(
    v64bfp16ebs8_unaligned *p, int n) {
  return p + n;
}

// COMMON-LABEL: @advance_v64bfp16ebs8_unaligned_const(
// COMMON:       getelementptr inbounds nuw i8, ptr %p, i20 12
// COMMON:       ret ptr
v64bfp16ebs8_unaligned *
advance_v64bfp16ebs8_unaligned_const(v64bfp16ebs8_unaligned *p) {
  return p + 3;
}

// COMMON-LABEL: @advance_v64mx9_unaligned(
// COMMON:       [[IDX_EXT:%.*]] = trunc i32 [[N:%.*]] to i20
// COMMON:       getelementptr inbounds %struct.v64mx9_unaligned, ptr %p, i20 [[IDX_EXT]]
// COMMON:       ret ptr
v64mx9_unaligned *advance_v64mx9_unaligned(v64mx9_unaligned *p, int n) {
  return p + n;
}

// COMMON-LABEL: @advance_v64mx9_unaligned_const(
// COMMON:       getelementptr inbounds nuw i8, ptr %p, i20 12
// COMMON:       ret ptr
v64mx9_unaligned *advance_v64mx9_unaligned_const(v64mx9_unaligned *p) {
  return p + 3;
}

} // extern "C"
