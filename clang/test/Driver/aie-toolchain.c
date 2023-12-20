//===- aie-toolchain.c ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// A basic clang -cc1 command-line, and simple environment check.

// RUN: %clang %s -### -no-canonical-prefixes --target=aie 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1 %s
// CC1: clang{{.*}} "-cc1" "-triple" "aie"
// CC1: "-mllvm" "-vectorize-loops=false"
// CC1: "-mllvm" "-vectorize-slp=false"
// CC1: "-mllvm" "--two-entry-phi-node-folding-threshold=10"

// RUN: %clang %s -### -no-canonical-prefixes --target=aie 2>&1 -fvectorize -fslp-vectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-ALL %s
// CC1-VECTORIZE-ALL: clang{{.*}} "-cc1" "-triple" "aie"
// CC1-VECTORIZE-ALL-NOT: "-mllvm" "-vectorize-loops=false"
// CC1-VECTORIZE-ALL-NOT: "-mllvm" "-vectorize-slp=false"

// RUN: %clang %s -### -no-canonical-prefixes --target=aie 2>&1 -fvectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-LOOPS %s
// CC1-VECTORIZE-LOOPS: clang{{.*}} "-cc1" "-triple" "aie"
// CC1-VECTORIZE-LOOPS-NOT: "-mllvm" "-vectorize-loops=false"
// CC1-VECTORIZE-LOOPS: "-mllvm" "-vectorize-slp=false"

// RUN: %clang %s -### -no-canonical-prefixes --target=aie 2>&1 -fslp-vectorize \
// RUN:   | FileCheck -check-prefix=CC1-VECTORIZE-SLP %s
// CC1-VECTORIZE-SLP: clang{{.*}} "-cc1" "-triple" "aie"
// CC1-VECTORIZE-SLP: "-mllvm" "-vectorize-loops=false"
// CC1-VECTORIZE-SLP-NOT: "-mllvm" "-vectorize-slp=false"

// Check that overriding the --two-entry-phi-node-folding-threshold option on command line
// adds the option to the back of the cc1 invocation. The last occurrence wins
// RUN: %clang %s -### -no-canonical-prefixes --target=aie 2>&1 -mllvm --two-entry-phi-node-folding-threshold=5\
// RUN:   | FileCheck -check-prefix=CC1-PHI-FOLDING-OVERRIDE %s
// CC1-PHI-FOLDING-OVERRIDE: clang{{.*}} "-cc1" "-triple" "aie"
// CC1-PHI-FOLDING-OVERRIDE: "-mllvm" "--two-entry-phi-node-folding-threshold=10"
// CC1-PHI-FOLDING-OVERRIDE: "-mllvm" "--two-entry-phi-node-folding-threshold=5"

// By default we want ctors, not init-array
// RUN: %clang %s -### --target=aie 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS %s
// CC1-XXTORS: "-cc1"{{.*}}"-fno-use-init-array"

// Also if we explicitly ask for it.
// RUN: %clang %s -### --target=aie -fno-use-init-array 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS-EXPL-DEFAULT %s
// CC1-XXTORS-EXPL-DEFAULT: "-cc1"{{.*}}"-fno-use-init-array"

// But not if we override it
// RUN: %clang %s -### --target=aie -fuse-init-array 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-XXTORS-OVERRIDE %s
// CC1-XXTORS-OVERRIDE-NOT: -fno-use-init-array

// By default we don't want threadsafe statics
// RUN: %clang %s -### --target=aie 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS %s
// CC1-STATICS: "-cc1"{{.*}}"-fno-threadsafe-statics"

// Also if we explicitly ask for it.
// RUN: %clang %s -### --target=aie -fno-threadsafe-statics 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS-EXPL-DEFAULT %s
// CC1-STATICS-EXPL-DEFAULT: "-cc1"{{.*}}"-fno-threadsafe-statics"

// But not if we override it
// RUN: %clang %s -### --target=aie -fthreadsafe-statics 2>&1 \
// RUN:   | FileCheck -check-prefix=CC1-STATICS-OVERRIDE %s
// CC1-STATICS-OVERRIDE-NOT: -fno-threadsafe-statics
