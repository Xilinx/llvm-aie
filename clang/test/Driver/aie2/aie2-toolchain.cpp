//===- aie2-toolchain.cpp --------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//


// RUN: %clangxx %s -### --target=aie2-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=CXX-INCLUDES %s
// CXX-INCLUDES: "-internal-isystem" "{{.*}}include{{/|\\\\}}aie2-none-unknown-elf{{/|\\\\}}c++{{/|\\\\}}v1"
// CXX-INCLUDES: "-internal-isystem" "{{.*}}include{{/|\\\\}}c++{{/|\\\\}}v1"
// CXX-INCLUDES: "-internal-externc-isystem" "{{.*}}include{{/|\\\\}}aie2-none-unknown-elf"
