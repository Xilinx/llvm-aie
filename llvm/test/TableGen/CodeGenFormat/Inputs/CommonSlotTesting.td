//===- CommonSlotTesting.td --------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// Common target boilerplate for CodeGenFormat tests.

include "llvm/Target/Target.td"
include "llvm/Target/CodeGenFormat.td"

def TESTInstrInfo : InstrInfo;

def TEST : Target {
  let InstructionSet = TESTInstrInfo;
}

def TESTCodeGenFormat : CodeGenFormat {
  let FormatClassEmitted = "";
}