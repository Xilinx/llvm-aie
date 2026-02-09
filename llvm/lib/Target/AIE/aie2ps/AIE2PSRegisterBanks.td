//===- AIE2PSRegisterBank.td -------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

def GPRRegBank : RegisterBank<"GPRRegBank", [eR, eL, mEEs, mFs, mGGs, EXPVEC64]>;
def PTRRegBank : RegisterBank<"PTRRegBank", [eP, eSpecial20]>;
def MODRegBank : RegisterBank<"MODRegBank", [mDm]>;
def VRegBank : RegisterBank<"VRegBank", [VEC128, VEC256, VEC512, VEC1024]>;
def AccRegBank : RegisterBank<"AccRegBank", [ACC512, ACC1024, ACC2048]>;
def FifoRegBank : RegisterBank<"FifoRegBank", [FIFO512, FIFO1024]>;
