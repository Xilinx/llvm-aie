//===- aie2-mangle-bf16.cpp -------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: %clang --target=aie2-none-unknown-elf -S -emit-llvm -o - %s | FileCheck %s

// CHECK-LABEL: define dso_local void @_Z3foo8bfloat16(
// CHECK-SAME: bfloat noundef [[B:%.*]]) #[[ATTR0:[0-9]+]] {
void foo(__bf16 b) {}

// CHECK-LABEL: define dso_local void @_Z3fooDv2_8bfloat16(
// CHECK-SAME: <2 x bfloat> noundef [[B:%.*]]) #[[ATTR0]] {
void foo(v2bfloat16 b) {}

// CHECK-LABEL: define dso_local void @_Z3fooDv4_8bfloat16(
// CHECK-SAME: <4 x bfloat> noundef [[B:%.*]]) #[[ATTR0]] {
void foo(v4bfloat16 b) {}

// CHECK-LABEL: define dso_local void @_Z3fooDv8_8bfloat16(
// CHECK-SAME: <8 x bfloat> noundef [[B:%.*]]) #[[ATTR0]] {
void foo(v8bfloat16 b) {}

// CHECK-LABEL: define dso_local void @_Z3fooDv16_8bfloat16(
// CHECK-SAME: <16 x bfloat> noundef [[B:%.*]]) #[[ATTR0]] {
void foo(v16bfloat16 b) {}

// CHECK-LABEL: define dso_local void @_Z3fooDv32_8bfloat16(
// CHECK-SAME: <32 x bfloat> noundef [[B:%.*]]) #[[ATTR0]] {
void foo(v32bfloat16 b) {}

// CHECK-LABEL: define dso_local void @_Z3fooDv64_8bfloat16(
// CHECK-SAME: <64 x bfloat> noundef [[B:%.*]]) #[[ATTR0]] {
void foo(v64bfloat16 b) {}
