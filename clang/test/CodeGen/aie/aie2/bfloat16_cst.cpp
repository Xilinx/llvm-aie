//===- bfloat16_cst.cpp -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: %clang --target=aie2 -S -emit-llvm %s -O2 -o - | FileCheck %s

// CHECK: @_ZL5inits{{.*}}zeroinitializer{{.*}}0xR3F80{{.*}}0xRBF80{{.*}}0xR3FDA{{.*}}0xRBFDA{{.*}}0xR4124{{.*}}0xRC124{{.*}}

const bfloat16 inits[] = {
  0.0,
  1.0,
  -1.0,
  1.7,
  -1.7,
  10.25,
  -10.25,
};

bfloat16 lu(int i) {
  return inits[i];
}

// CHECK-LABEL: @_Z4zerov
// CHECK: zeroinitializer
bfloat16 zero() {
  return 0.0;
}

// CHECK-LABEL: @_Z3onev
// CHECK: bfloat 0xR3F80
bfloat16 one() {
  return 1.0;
}

// CHECK-LABEL: @_Z4monev
// CHECK: bfloat 0xRBF80
bfloat16 mone() {
  return -1.0;
}

// CHECK-LABEL: @_Z3twov
// CHECK: bfloat 0xR4000
bfloat16 two() {
  return 2.0;
}

// CHECK-LABEL: @_Z11oddStickyUpv
// CHECK: bfloat 0xR4000
bfloat16 oddStickyUp() {
  // This is the smallest odd sticky part, 3FFF8001. It should
  // round up to 2.0
  return 1.99609386920928955078125;
}

// CHECK-LABEL: @_Z8oddTieUpv
// CHECK: bfloat 0xR4000
bfloat16 oddTieUp() {
  // This is the odd tie case, 3FFF8000. It should round up to 2.0
  return 1.99609375;
}

// CHECK-LABEL: @_Z7oddDownv
// CHECK: bfloat 0xR3FFF
bfloat16 oddDown() {
  // This is just below the odd tie case, 3FFF7FFF it should round down
  return 1.99609363079071044921875;
}

// CHECK-LABEL: @_Z4evenv
// CHECK: bfloat 0xR3FFE
bfloat16 even() {
  return 1.984375;
}

// CHECK-LABEL: @_Z12evenStickyUpv
// CHECK: bfloat 0xR3FFF
bfloat16 evenStickyUp() {
  // This is the smallest even sticky value 0x3FFE8001. It should
  // round up to 1.9921875
  return 1.98828136920928955078125;
}

// CHECK-LABEL: @_Z11evenTieDownv
// CHECK: bfloat 0xR3FFE
bfloat16 evenTieDown() {
  // This is the even tie case 0x3FFE8000. It should
  // round down to 1.984375
  return 1.98828125;
}

// CHECK-LABEL: @_Z8evenDownv
// CHECK: bfloat 0xR3FFE
bfloat16 evenDown() {
  // This is just below the even tie case, 3FFE7FFF. It should
  // round down to 1.984375
  return 1.98828113079071044921875;
}
