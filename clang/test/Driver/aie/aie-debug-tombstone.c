//===- aie-debug-tombstone.c ------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: %clang %s -### --target=aie2-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=TOMB %s
// RUN: %clang %s -### --target=aie2p-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=TOMB %s
// RUN: %clang %s -### --target=aie2ps-none-unknown-elf 2>&1 \
// RUN:   | FileCheck -check-prefix=TOMB %s
// TOMB: "{{[^"]*}}ld.lld{{[^"]*}}"
// TOMB-SAME: "-z" "dead-reloc-in-nonalloc=.debug_info=0xfffff"

// A user-supplied override is placed after the linker inputs, and LLD uses the
// last matching dead-reloc-in-nonalloc= entry, so the user's value wins.
// RUN: %clang %s -### --target=aie2p-none-unknown-elf \
// RUN:     -Wl,-z,dead-reloc-in-nonalloc=.debug_info=0x12345 2>&1 \
// RUN:   | FileCheck -check-prefix=OVERRIDE %s
// OVERRIDE: "{{[^"]*}}ld.lld{{[^"]*}}"
// OVERRIDE-SAME: "-z" "dead-reloc-in-nonalloc=.debug_info=0xfffff"
// OVERRIDE-SAME: "dead-reloc-in-nonalloc=.debug_info=0x12345"
