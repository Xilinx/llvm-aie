//===- aie2-startup.c ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: %clang %s -### --target=aie2-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=LIBS %s
// LIBS: "{{[^"]*}}ld.lld{{[^"]*}}"
// LIBS-SAME: "{{.*}}lib{{.*}}{{(aie2-none-unknown-elf.*libclang_rt.builtins.a)|(libclang_rt.builtins-aie2.a)}}"
// LIBS-SAME: "{{.*}}lib{{.*}}aie2-none-unknown-elf{{.*}}crt0.o"
// LIBS-SAME: "{{.*}}lib{{.*}}aie2-none-unknown-elf{{.*}}crt1.o"
