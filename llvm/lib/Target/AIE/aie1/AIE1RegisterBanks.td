//===- AIERegisterBank.td ----------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

// General purpose registers: R, mCL, mCH
def GPRRegBank : RegisterBank<"GPRRegBank", [mRCm]>;

def PTRRegBank : RegisterBank<"PTRRegBank", [PTR]>;
def MODRegBank : RegisterBank<"MODRegBank", [MOD]>;
def CBRegBank : RegisterBank<"CBRegBank", [mCS]>;
