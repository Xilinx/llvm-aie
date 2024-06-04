//===- aie2-stdlib.c ------------------------------------------*- C++ -*-===//
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
// LIBS: "{{.*}}ld.lld"
// LIBS-SAME: "{{.*}}/lib/clang/{{.*}}/lib/aie2-none-unknown-elf/libclang_rt.builtins.a"
// LIBS-SAME: "-L{{[^"]*}}/lib/aie2-none-unknown-elf"
// LIBS-SAME: "-L{{[^"]*}}lib/clang/{{..}}/lib/aie2-none-unknown-elf
// LIBS-SAME: "-lc"
// LIBS-SAME: "-lm"

// ... but not if we have --nostdlib or a target without a library.
// RUN: %clang %s -### --target=aie2-none-unknown-elf -nostdlib 2>&1 \
// RUN:   | FileCheck -check-prefix=NOLIBS %s
// NOLIBS-NOT: ld.lld{{.*}}libclang_rt.builtins.a
// NOLIBS-NOT: ld.lld{{.*}}-lm
// NOLIBS-NOT: ld.lld{{.*}}-lc