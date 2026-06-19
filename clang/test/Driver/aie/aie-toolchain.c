//===- aie-toolchain.c ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// A basic clang -cc1 command-line, and simple environment check.

// RUN: %clang %s -### -no-canonical-prefixes --target=aie-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1 -DAIE_ARCH=aie %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1 -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2p-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1 -DAIE_ARCH=aie2p %s
// CC1: clang{{.*}} "-cc1" "-triple" "[[AIE_ARCH]]-none-unknown-elf"
// CC1: "-mllvm" "-vectorize-loops=false"
// CC1: "-mllvm" "-vectorize-slp=false"
// CC1: "-mllvm" "--two-entry-phi-node-folding-threshold=10"
// CC1: "-mllvm" "-mandatory-inlining-before-opt=false"
// CC1: "-mllvm" "-enable-loop-iter-count-assumptions=true"
// CC1: "-mllvm" "-pipeliner-pragma-as-max-ii=true"
// CC1: "-fno-builtin-memset"
// CC1: "-fno-builtin-memcpy"
// CC1: "-fno-builtin-memmove"
// CC1: "-ffunction-sections"
// CC1: "-fdata-sections"

// RUN: %clang %s -### -no-canonical-prefixes --target=aie-none-unknown-elf 2>&1 -fbuiltin \
// RUN:   | FileCheck -check-prefix=BUILTIN %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2-none-unknown-elf 2>&1 -fbuiltin \
// RUN:   | FileCheck -check-prefix=BUILTIN %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2p-none-unknown-elf 2>&1 -fbuiltin \
// RUN:   | FileCheck -check-prefix=BUILTIN %s
// BUILTIN-NOT: "-fno-builtin-memset"
// BUILTIN-NOT: "-fno-builtin-memcpy"
// BUILTIN-NOT: "-fno-builtin-memmove"

// RUN: %clang %s -### -no-canonical-prefixes --target=aie-none-unknown-elf 2>&1 -fvectorize -fslp-vectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-ALL -DAIE_ARCH=aie %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2-none-unknown-elf 2>&1 -fvectorize -fslp-vectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-ALL -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2p-none-unknown-elf 2>&1 -fvectorize -fslp-vectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-ALL -DAIE_ARCH=aie2p %s
// CC1-VECTORIZE-ALL: clang{{.*}} "-cc1" "-triple" "[[AIE_ARCH]]-none-unknown-elf"
// CC1-VECTORIZE-ALL-NOT: "-mllvm" "-vectorize-loops=false"
// CC1-VECTORIZE-ALL-NOT: "-mllvm" "-vectorize-slp=false"

// RUN: %clang %s -### -no-canonical-prefixes --target=aie-none-unknown-elf 2>&1 -fvectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-LOOPS -DAIE_ARCH=aie %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2-none-unknown-elf 2>&1 -fvectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-LOOPS -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2p-none-unknown-elf 2>&1 -fvectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-LOOPS -DAIE_ARCH=aie2p %s
// CC1-VECTORIZE-LOOPS: clang{{.*}} "-cc1" "-triple" "[[AIE_ARCH]]-none-unknown-elf"
// CC1-VECTORIZE-LOOPS-NOT: "-mllvm" "-vectorize-loops=false"
// CC1-VECTORIZE-LOOPS: "-mllvm" "-vectorize-slp=false"

// RUN: %clang %s -### -no-canonical-prefixes --target=aie-none-unknown-elf 2>&1 -fslp-vectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-SLP -DAIE_ARCH=aie %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2-none-unknown-elf 2>&1 -fslp-vectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-SLP -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2p-none-unknown-elf 2>&1 -fslp-vectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-SLP -DAIE_ARCH=aie2p %s
// CC1-VECTORIZE-SLP: clang{{.*}} "-cc1" "-triple" "[[AIE_ARCH]]-none-unknown-elf"
// CC1-VECTORIZE-SLP: "-mllvm" "-vectorize-loops=false"
// CC1-VECTORIZE-SLP-NOT: "-mllvm" "-vectorize-slp=false"

// Check that overriding the --two-entry-phi-node-folding-threshold option on command line
// adds the option to the back of the cc1 invocation. The last occurrence wins
// RUN: %clang %s -### -no-canonical-prefixes --target=aie-none-unknown-elf 2>&1 -mllvm --two-entry-phi-node-folding-threshold=5\
// RUN:   | FileCheck -check-prefix=CC1-PHI-FOLDING-OVERRIDE -DAIE_ARCH=aie %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2-none-unknown-elf 2>&1 -mllvm --two-entry-phi-node-folding-threshold=5\
// RUN:   | FileCheck -check-prefix=CC1-PHI-FOLDING-OVERRIDE -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2p-none-unknown-elf 2>&1 -mllvm --two-entry-phi-node-folding-threshold=5\
// RUN:   | FileCheck -check-prefix=CC1-PHI-FOLDING-OVERRIDE -DAIE_ARCH=aie2p %s
// CC1-PHI-FOLDING-OVERRIDE: clang{{.*}} "-cc1" "-triple" "[[AIE_ARCH]]-none-unknown-elf"
// CC1-PHI-FOLDING-OVERRIDE: "-mllvm" "--two-entry-phi-node-folding-threshold=10"
// CC1-PHI-FOLDING-OVERRIDE: "-mllvm" "--two-entry-phi-node-folding-threshold=5"

// Check that we can override the -ffunction-section option
// RUN: %clang %s -### -no-canonical-prefixes --target=aie-none-unknown-elf 2>&1 -fno-function-sections\
// RUN:   | FileCheck -check-prefix=CC1-FNO-FUNCTION-SECTIONS -DAIE_ARCH=aie %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2-none-unknown-elf 2>&1 -fno-function-sections\
// RUN:   | FileCheck -check-prefix=CC1-FNO-FUNCTION-SECTIONS -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2p-none-unknown-elf 2>&1 -fno-function-sections\
// RUN:   | FileCheck -check-prefix=CC1-FNO-FUNCTION-SECTIONS -DAIE_ARCH=aie2p %s
// CC1-FNO-FUNCTION-SECTIONS: clang{{.*}} "-cc1" "-triple" "[[AIE_ARCH]]-none-unknown-elf"
// CC1-FNO-FUNCTION-SECTIONS-NOT: "-ffunction-sections"

// Check that we can override the -fdata-sections option
// RUN: %clang %s -### -no-canonical-prefixes --target=aie-none-unknown-elf 2>&1 -fno-data-sections\
// RUN:   | FileCheck -check-prefix=CC1-FNO-DATA-SECTIONS -DAIE_ARCH=aie %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2-none-unknown-elf 2>&1 -fno-data-sections\
// RUN:   | FileCheck -check-prefix=CC1-FNO-DATA-SECTIONS -DAIE_ARCH=aie2 %s
// RUN: %clang %s -### -no-canonical-prefixes --target=aie2p-none-unknown-elf 2>&1 -fno-data-sections\
// RUN:   | FileCheck -check-prefix=CC1-FNO-DATA-SECTIONS -DAIE_ARCH=aie2p %s
// CC1-FNO-DATA-SECTIONS: clang{{.*}} "-cc1" "-triple" "[[AIE_ARCH]]-none-unknown-elf"
// CC1-FNO-DATA-SECTIONS-NOT: "-fdata-sections"

// By default we want ctors, not init-array
// RUN: %clang %s -### --target=aie-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS %s
// RUN: %clang %s -### --target=aie2-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS %s
// CC1-XXTORS: "-cc1"{{.*}}"-fno-use-init-array"

// Also if we explicitly ask for it.
// RUN: %clang %s -### --target=aie-none-unknown-elf -fno-use-init-array 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS-EXPL-DEFAULT %s
// RUN: %clang %s -### --target=aie2-none-unknown-elf -fno-use-init-array 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS-EXPL-DEFAULT %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -fno-use-init-array 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS-EXPL-DEFAULT %s
// CC1-XXTORS-EXPL-DEFAULT: "-cc1"{{.*}}"-fno-use-init-array"

// But not if we override it
// RUN: %clang %s -### --target=aie-none-unknown-elf -fuse-init-array 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS-OVERRIDE %s
// RUN: %clang %s -### --target=aie2-none-unknown-elf -fuse-init-array 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS-OVERRIDE %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -fuse-init-array 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS-OVERRIDE %s
// CC1-XXTORS-OVERRIDE-NOT: -fno-use-init-array

// By default we don't want threadsafe statics
// RUN: %clang %s -### --target=aie-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS %s
// RUN: %clang %s -### --target=aie2-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS %s
// CC1-STATICS: "-cc1"{{.*}}"-fno-threadsafe-statics"

// Also if we explicitly ask for it.
// RUN: %clang %s -### --target=aie-none-unknown-elf -fno-threadsafe-statics 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS-EXPL-DEFAULT %s
// RUN: %clang %s -### --target=aie2-none-unknown-elf -fno-threadsafe-statics 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS-EXPL-DEFAULT %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -fno-threadsafe-statics 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS-EXPL-DEFAULT %s
// CC1-STATICS-EXPL-DEFAULT: "-cc1"{{.*}}"-fno-threadsafe-statics"

// But not if we override it
// RUN: %clang %s -### --target=aie-none-unknown-elf -fthreadsafe-statics 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS-OVERRIDE %s
// RUN: %clang %s -### --target=aie2-none-unknown-elf -fthreadsafe-statics 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS-OVERRIDE %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf -fthreadsafe-statics 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS-OVERRIDE %s
// CC1-STATICS-OVERRIDE-NOT: -fno-threadsafe-statics
