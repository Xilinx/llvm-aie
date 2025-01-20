//===- AIE2PRegisterBank.td -------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

include "AIEBaseRegisterBanks.td"
def GPRRegBank : RegisterBank<"GPRRegBank", [eR, eL, eE, EXPVEC64]>;
def AccRegBank : RegisterBank<"AccRegBank", [ACC512, ACC1024, ACC2048]>;
def FifoRegBank : RegisterBank<"FifoRegBank", [FIFO512, FIFO1024]>;
