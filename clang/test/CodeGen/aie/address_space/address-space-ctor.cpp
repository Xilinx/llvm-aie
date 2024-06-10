//===- address-space-ctor.cpp -----------------------------------*- C++ -*-===//
//
//  Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
//  See https://llvm.org/LICENSE.txt for license information.
//  SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: %clang_cc1 -triple aie %s -std=c++14 -verify -fsyntax-only
// RUN: %clang_cc1 -triple aie %s -std=c++17 -verify -fsyntax-only
// RUN: %clang_cc1 -triple aie2 %s -std=c++14 -verify -fsyntax-only
// RUN: %clang_cc1 -triple aie2 %s -std=c++17 -verify -fsyntax-only

struct MyType {
  MyType(int i) : i(i) {}
  int i;
};

// Supported by AIE target note@-5{{candidate constructor (the implicit copy constructor) not viable: no known conversion from 'int' to 'const MyType &' for 1st argument}}
// Supported by AIE target note@-6{{candidate constructor (the implicit move constructor) not viable: no known conversion from 'int' to 'MyType &&' for 1st argument}}
// Supported by AIE target note@-6{{candidate constructor ignored: cannot be used to construct an object in address space '__attribute__((address_space(10)))'}}
// Supported by AIE target note@-8{{candidate constructor ignored: cannot be used to construct an object in address space '__attribute__((address_space(10)))'}}
// Supported by AIE target note@-9{{candidate constructor ignored: cannot be used to construct an object in address space '__attribute__((address_space(10)))'}}
// Supported by AIE target note@-9{{candidate constructor ignored: cannot be used to construct an object in address space '__attribute__((address_space(10)))'}}

// FIXME: We can't implicitly convert between address spaces yet.
MyType __attribute__((address_space(10))) m1 = 123; // expected-no-diagnostics // Supported by AIE target
MyType __attribute__((address_space(10))) m2(123);  // expected-no-diagnostics // Supported by AIE target
