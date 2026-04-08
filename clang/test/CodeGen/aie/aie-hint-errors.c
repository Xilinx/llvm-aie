// --- aie-hint-errors.c ----------------------------------------------------///
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
// --------------------------------------------------------------------------///

// Verify parsing diagnostics for ill-formed hint pragmas.

// RUN: %clang_cc1 -triple aie2ps -fsyntax-only -verify %s

void test_errors(int *p, int n) {
  int s = 0;

  // expected-error@+1 {{missing argument; expected a hint key identifier}}
  #pragma clang loop hint()
  for (int i = 0; i < n; i++) { s += p[i]; }

  // expected-error@+1 {{expected expression}}
  #pragma clang loop hint(key, )
  for (int i = 0; i < n; i++) { s += p[i]; }


  // expected-warning@+1 {{extra tokens at end of '#pragma clang loop hint'}}
  #pragma clang loop hint(key extra)
  for (int i = 0; i < n; i++) { s += p[i]; }
}
