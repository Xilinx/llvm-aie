//===- address_space_print_macro.c ------------------------------*- C ---*-===//
//
//  Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
//  See https://llvm.org/LICENSE.txt for license information.
//  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: %clang_cc1 -triple aie %s -fsyntax-only -verify
// RUN: %clang_cc1 -triple aie2 %s -fsyntax-only -verify

#define AS1 __attribute__((address_space(1)))
#define AS2 __attribute__((address_space(2), annotate("foo")))
#define AS_ND __attribute__((address_space(2), noderef))

#define AS(i) address_space(i)
#define AS3 __attribute__((AS(3)))
#define AS5 __attribute__((address_space(5))) char

void normal_case(void) {
  int *p = 0;
  __attribute__((address_space(1))) int *q = p; // expected-error{{initializing '__attribute__((address_space(1))) int *' with an expression of type 'int *' changes address space of pointer}}
}

char *cmp(AS1 char *x, AS2 char *y) {
  return x < y ? x : y; // expected-error{{conditional operator with the second and third operands of type  ('AS1 char *' and 'AS2 char *') which are pointers to non-overlapping address spaces}}
}

__attribute__((address_space(1))) char test_array[10];
void test3(void) {
  extern void test3_helper(char *p); // Supported by AIE target
  test3_helper(test_array);          // Supported by AIE target
}

char AS2 *test4_array;
void test4(void) {
  extern void test3_helper(char *p); // Supported by AIE target
  test3_helper(test4_array);         // Supported by AIE target
}

void func(void) {
  char AS1 *x;
  char AS3 *x2;
  AS5 *x3;
  char *y;
  y = x;  // Supported by AIE target
  y = x2; // Supported by AIE target
  y = x3; // Supported by AIE target
}

void multiple_attrs(AS_ND int *x) {
  __attribute__((address_space(2))) int *y = x; // expected-warning{{casting to dereferenceable pointer removes 'noderef' attribute}}
}

void override_macro_name(void) {
#define ATTRS __attribute__((noderef)) // expected-note{{previous definition is here}}
  ATTRS
#define ATTRS __attribute__((address_space(1))) // expected-warning{{'ATTRS' macro redefined}}
  ATTRS
  int *x;

  int AS_ND *y = x; // expected-error{{initializing 'AS_ND int *' with an expression of type 'ATTRS int *' changes address space of pointer}}
}

void partial_macro_declaration(void) {
#define ATTRS2 __attribute__((noderef))
  ATTRS2 __attribute__((address_space(1))) int *x;

  int AS_ND *y = x; // expected-error{{initializing 'AS_ND int *' with an expression of type 'ATTRS2 int __attribute__((address_space(1))) *' changes address space of pointer}}

  // The attribute not wrapped with a macro should be printed regularly.
#define ATTRS3 __attribute__((address_space(1)))
  ATTRS3 __attribute__((noderef)) int *x2;

  int AS_ND *y2 = x2; // expected-error{{initializing 'AS_ND int *' with an expression of type 'ATTRS3 int * __attribute__((noderef))' changes address space of pointer}}
}
