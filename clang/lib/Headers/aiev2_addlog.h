//===- aiev2_addlog.h -------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_ADDLOG_H__
#define __AIEV2_ADDLOG_H__

INTRINSIC(char)
select(bool sel, char a, char b) { return (sel ? a : b); }
INTRINSIC(unsigned char)
select(bool sel, unsigned char a, unsigned char b) { return (sel ? a : b); }
INTRINSIC(signed short)
select(bool sel, signed short a, signed short b) { return (sel ? a : b); }
INTRINSIC(unsigned short)
select(bool sel, unsigned short a, unsigned short b) { return (sel ? a : b); }
INTRINSIC(int)
select(bool sel, int a, int b) { return (sel ? a : b); }
INTRINSIC(unsigned int)
select(bool sel, unsigned int a, unsigned int b) { return (sel ? a : b); }
INTRINSIC(long)
select(bool sel, long a, long b) { return (sel ? a : b); }
INTRINSIC(unsigned long)
select(bool sel, unsigned long a, unsigned long b) { return (sel ? a : b); }
INTRINSIC(void *)
select(bool sel, void *a, void *b) { return (sel ? a : b); }

#endif // __AIEV2_ADDLOG_H__