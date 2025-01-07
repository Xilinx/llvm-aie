//===- aie2-abi-unaligned.cpp ----------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: %clang --target=aie2p -nostdlibinc -S -emit-llvm %s -o - | FileCheck %s

#include <stdint.h>

extern "C" {

// CHECK-LABEL: define {{[^@]*}}%struct.v256bfp16ebs16_sparse_unaligned @ret_v256bfp16ebs16_sparse_unaligned
// CHECK-SAME: () #[[ATTR0:[0-9]+]] {
v256bfp16ebs16_sparse_unaligned ret_v256bfp16ebs16_sparse_unaligned(void) { return {}; }

// CHECK-LABEL: define {{[^@]*}}void @pass_v256bfp16ebs16_sparse_unaligned
// CHECK-SAME: ([[STRUCT_v256bfp16ebs16_sparse_unaligned:%.*]] [[DOTCOERCE:%.*]]) #[[ATTR0:[0-9]+]] {
void pass_v256bfp16ebs16_sparse_unaligned(v256bfp16ebs16_sparse_unaligned) {}


// CHECK-LABEL: define {{[^@]*}}%struct.v256bfp16ebs8_sparse_unaligned @ret_v256bfp16ebs8_sparse_unaligned
// CHECK-SAME: () #[[ATTR0:[0-9]+]] {
v256bfp16ebs8_sparse_unaligned ret_v256bfp16ebs8_sparse_unaligned(void) { return {}; }

// CHECK-LABEL: define {{[^@]*}}void @pass_v256bfp16ebs8_sparse_unaligned
// CHECK-SAME: ([[STRUCT_v256bfp16ebs8_sparse_unaligned:%.*]] [[DOTCOERCE:%.*]]) #[[ATTR0:[0-9]+]] {
void pass_v256bfp16ebs8_sparse_unaligned(v256bfp16ebs8_sparse_unaligned) {}


// CHECK-LABEL: define {{[^@]*}}%struct.v128bfp16ebs16_sparse_unaligned @ret_v128bfp16ebs16_sparse_unaligned
// CHECK-SAME: () #[[ATTR0:[0-9]+]] {
v128bfp16ebs16_sparse_unaligned ret_v128bfp16ebs16_sparse_unaligned(void) { return {}; }
// CHECK-LABEL: define {{[^@]*}}void @pass_v128bfp16ebs16_sparse_unaligned
// CHECK-SAME: ([[STRUCT_v128bfp16ebs16_sparse_unaligned:%.*]] [[DOTCOERCE:%.*]]) #[[ATTR0:[0-9]+]] {
void pass_v128bfp16ebs16_sparse_unaligned(v128bfp16ebs16_sparse_unaligned) {}


// CHECK-LABEL: define {{[^@]*}}%struct.v128bfp16ebs8_sparse_unaligned @ret_v128bfp16ebs8_sparse_unaligned
// CHECK-SAME: () #[[ATTR0:[0-9]+]] {
v128bfp16ebs8_sparse_unaligned ret_v128bfp16ebs8_sparse_unaligned(void) { return {}; }
// CHECK-LABEL: define {{[^@]*}}void @pass_v128bfp16ebs8_sparse_unaligned
// CHECK-SAME: ([[STRUCT_v128bfp16ebs8_sparse_unaligned:%.*]] [[DOTCOERCE:%.*]]) #[[ATTR0:[0-9]+]] {
void pass_v128bfp16ebs8_sparse_unaligned(v128bfp16ebs8_sparse_unaligned) {}


// CHECK-LABEL: define {{[^@]*}}%struct.v128bfp16ebs16_unaligned @ret_v128bfp16ebs16_unaligned
// CHECK-SAME: () #[[ATTR0:[0-9]+]] {
v128bfp16ebs16_unaligned ret_v128bfp16ebs16_unaligned(void) { return {}; }
// CHECK-LABEL: define {{[^@]*}}void @pass_v128bfp16ebs16_unaligned
// CHECK-SAME: ([[STRUCT_v128bfp16ebs16_unaligned:%.*]] [[DOTCOERCE:%.*]]) #[[ATTR0:[0-9]+]] {
void pass_v128bfp16ebs16_unaligned(v128bfp16ebs16_unaligned) {}


// CHECK-LABEL: define {{[^@]*}}%struct.v128bfp16ebs8_unaligned @ret_v128bfp16ebs8_unaligned
// CHECK-SAME: () #[[ATTR0:[0-9]+]] {
v128bfp16ebs8_unaligned ret_v128bfp16ebs8_unaligned(void) { return {}; }
// CHECK-LABEL: define {{[^@]*}}void @pass_v128bfp16ebs8_unaligned
// CHECK-SAME: ([[STRUCT_v128bfp16ebs8_unaligned:%.*]] [[DOTCOERCE:%.*]]) #[[ATTR0:[0-9]+]] {
void pass_v128bfp16ebs8_unaligned(v128bfp16ebs8_unaligned) {}

// CHECK-LABEL: define {{[^@]*}}%struct.v64bfp16ebs16_unaligned @ret_v64bfp16ebs16_unaligned
// CHECK-SAME: () #[[ATTR0:[0-9]+]] {
v64bfp16ebs16_unaligned ret_v64bfp16ebs16_unaligned(void) { return {}; }
// CHECK-LABEL: define {{[^@]*}}void @pass_v64bfp16ebs16_unaligned
// CHECK-SAME: ([[STRUCT_v64bfp16ebs16_unaligned:%.*]] [[DOTCOERCE:%.*]]) #[[ATTR0:[0-9]+]] {
void pass_v64bfp16ebs16_unaligned(v64bfp16ebs16_unaligned) {}

// CHECK-LABEL: define {{[^@]*}}%struct.v64bfp16ebs8_unaligned @ret_v64bfp16ebs8_unaligned
// CHECK-SAME: () #[[ATTR0:[0-9]+]] {
v64bfp16ebs8_unaligned ret_v64bfp16ebs8_unaligned(void) { return {}; }
// CHECK-LABEL: define {{[^@]*}}void @pass_v64bfp16ebs8_unaligned
// CHECK-SAME: ([[STRUCT_v64bfp16ebs8_unaligned:%.*]] [[DOTCOERCE:%.*]]) #[[ATTR0:[0-9]+]] {
void pass_v64bfp16ebs8_unaligned(v64bfp16ebs8_unaligned) {}

}
