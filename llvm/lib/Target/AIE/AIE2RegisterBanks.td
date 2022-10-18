//===- AIE2RegisterBank.td ---------------------------------*- tablegen -*-===//
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

def GPRRegBank : RegisterBank<"GPRRegBank", [eR, eL]>;
def PTRRegBank : RegisterBank<"PTRRegBank", [eP]>;
def MODRegBank : RegisterBank<"MODRegBank", [mDm]>;
def VRegBank : RegisterBank<"VRegBank", [VEC128, VEC256, VEC512, VEC1024]>;
def AccRegBank : RegisterBank<"AccRegBank", [ACC256, ACC512, ACC1024]>;
