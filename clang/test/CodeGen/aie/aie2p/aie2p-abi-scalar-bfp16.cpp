//===- aie2p-abi-scalar-bfp16.cpp -------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: %clang -target aie2p -nostdlibinc -fsyntax-only %s -o -

// Check the sizeof and alignment of the scalar bfp16 types according to their
// type specification.

static_assert(sizeof(bfp16ebs8) == 9, "unexpected sizeof(bfp16ebs8)");
static_assert(__alignof(bfp16ebs8) == 1, "unexpected alignof(bfp16ebs8)");

static_assert(sizeof(bfp16ebs16) == 17, "unexpected sizeof(bfp16ebs16)");
static_assert(__alignof(bfp16ebs16) == 1, "unexpected alignof(bfp16ebs16)");
