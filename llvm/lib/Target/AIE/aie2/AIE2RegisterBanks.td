//===- AIE2RegisterBank.td ---------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

include "AIEBaseRegisterBanks.td"
def GPRRegBank : RegisterBank<"GPRRegBank", [eR, eL]>;
def AccRegBank : RegisterBank<"AccRegBank", [ACC256, ACC512, ACC1024]>;
