//===- aie2-toolchain.c ------------------------------------------*- C++ -*-===//
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
// LIBS-SAME: "-L{{[^"]*}}lib{{.*}}aie2-none-unknown-elf"
// LIBS-SAME: "{{.*}}lib{{.*}}{{(aie2-none-unknown-elf.*libclang_rt.builtins.a)|(libclang_rt.builtins-aie2.a)}}"
// LIBS-SAME: "-lc"
// LIBS-SAME: "-lm"
// LIBS-SAME: "{{.*}}lib{{.*}}aie2-none-unknown-elf{{.*}}crt0.o"
// LIBS-SAME: "{{.*}}lib{{.*}}aie2-none-unknown-elf{{.*}}crt1.o"

// ... for -nostdlib or a target without a library.
// RUN: %clang %s -### --target=aie2-none-unknown-elf -nostdlib -nodefaultlibs 2>&1 \
// RUN:   | FileCheck -check-prefix=NOLIBS %s
// NOLIBS-NOT: ld.lld{{.*}}libclang_rt.builtins.a
// NOLIBS-NOT: ld.lld{{.*}}-lm
// NOLIBS-NOT: ld.lld{{.*}}-lc

// ... for -nostartfiles
// RUN: %clang %s -### --target=aie2-none-unknown-elf -nostartfiles -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=NOSTART %s
// NOSTART: "{{[^"]*}}ld.lld{{[^"]*}}"
// NOSTART-SAME: "-L{{[^"]*}}lib{{.*}}aie2-none-unknown-elf"
// NOSTART-SAME: "{{.*}}lib{{.*}}{{(aie2-none-unknown-elf.*libclang_rt.builtins.a)|(libclang_rt.builtins-aie2.a)}}"
// NOSTART-SAME: "-lc"
// NOSTART-SAME: "-lm"
// NOSTART-NOT: "{{.*}}lib{{.*}}aie2-none-unknown-elf{{.*}}crt0.o"
// NOSTART-NOT: "{{.*}}lib{{.*}}aie2-none-unknown-elf{{.*}}crt1.o"

// RUN: %clang %s -### --target=aie2-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=C-INCLUDES %s
// C-INCLUDES: "-internal-externc-isystem" "{{.*}}include{{/|\\\\}}aie2-none-unknown-elf"

// Verify we are not using default page alignment.
// RUN: %clang %s -### --target=aie2-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=NMAGIC %s
// NMAGIC: "{{[^"]*}}ld.lld{{[^"]*}}"
// NMAGIC-SAME: "--nmagic"

// Verify we can override "not using" the default page alignment.
// RUN: %clang %s -### --target=aie2-none-unknown-elf -Wl,--no-nmagic -ccc-install-dir %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=NONMAGIC %s
// NONMAGIC: "{{[^"]*}}ld.lld{{[^"]*}}"
// NONMAGIC-SAME: "--nmagic{{.*}}--no-nmagic"
