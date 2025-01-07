// RUN: %clang_cc1 -triple aie2p-none-unknown-elf -emit-llvm -o - -x c++ %s | FileCheck %s

// CHECK: target triple = "aie2p-none-unknown-elf"
