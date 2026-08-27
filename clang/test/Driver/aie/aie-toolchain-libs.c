//===- aie-toolchain-libs.c --------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: %clang %s -### --target=aie2-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=LIBS -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=LIBS -DAIE_ARCH=aie2p %s
// LIBS: "{{[^"]*}}ld.lld{{[^"]*}}"
// LIBS-SAME: "-L{{[^"]*}}lib{{.*}}[[AIE_ARCH]]-none-unknown-elf"
// LIBS-SAME: "{{.*}}lib{{.*}}{{(aie[a-z0-9]*-none-unknown-elf.*libclang_rt.builtins.a)|(libclang_rt.builtins-aie[a-z0-9]*.a)}}"
// LIBS-SAME: "-lc"
// LIBS-SAME: "-lm"
// LIBS-SAME: "{{.*}}lib{{.*}}[[AIE_ARCH]]-none-unknown-elf{{.*}}crt0.o"
// LIBS-SAME: "{{.*}}lib{{.*}}[[AIE_ARCH]]-none-unknown-elf{{.*}}crt1.o"

// ... for -nostdlib or a target without a library.
// RUN: %clang %s -### --target=aie2-none-unknown-elf -nostdlib -nodefaultlibs 2>&1 \
// RUN:   | FileCheck -check-prefix=NOLIBS %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -nostdlib -nodefaultlibs 2>&1 \
// RUN:   | FileCheck -check-prefix=NOLIBS %s
// NOLIBS-NOT: libclang_rt.builtins
// NOLIBS-NOT: "-lm"
// NOLIBS-NOT: "-lc"

// ... for -nostartfiles
// RUN: %clang %s -### --target=aie2-none-unknown-elf -nostartfiles -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=NOSTART -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -nostartfiles -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=NOSTART -DAIE_ARCH=aie2p %s
// NOSTART: "{{[^"]*}}ld.lld{{[^"]*}}"
// NOSTART-SAME: "-L{{[^"]*}}lib{{.*}}[[AIE_ARCH]]-none-unknown-elf"
// NOSTART-SAME: "{{.*}}lib{{.*}}{{(aie[a-z0-9]*-none-unknown-elf.*libclang_rt.builtins.a)|(libclang_rt.builtins-aie[a-z0-9]*.a)}}"
// NOSTART-SAME: "-lc"
// NOSTART-SAME: "-lm"
// NOSTART-NOT: "{{.*}}lib{{.*}}[[AIE_ARCH]]-none-unknown-elf{{.*}}crt0.o"
// NOSTART-NOT: "{{.*}}lib{{.*}}[[AIE_ARCH]]-none-unknown-elf{{.*}}crt1.o"

// RUN: %clang %s -### --target=aie2-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=C-INCLUDES -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=C-INCLUDES -DAIE_ARCH=aie2p %s
// C-INCLUDES: "-internal-externc-isystem" "{{.*}}include{{/|\\\\}}[[AIE_ARCH]]-none-unknown-elf"

// Verify we are not using default page alignment.
// RUN: %clang %s -### --target=aie2-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=NMAGIC %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -ccc-install-dir  %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=NMAGIC %s
// NMAGIC: "{{[^"]*}}ld.lld{{[^"]*}}"
// NMAGIC-SAME: "--nmagic"

// Verify we can override "not using" the default page alignment.
// RUN: %clang %s -### --target=aie2-none-unknown-elf -Wl,--no-nmagic -ccc-install-dir %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=NONMAGIC %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -Wl,--no-nmagic -ccc-install-dir %S/../Inputs/basic_aie_tree/bin 2>&1 \
// RUN:   | FileCheck -check-prefix=NONMAGIC %s
// NONMAGIC: "{{[^"]*}}ld.lld{{[^"]*}}"
// NONMAGIC-SAME: "--nmagic{{.*}}--no-nmagic"
