//===- base-register-bank-class.td -------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

// RUN: llvm-tblgen -gen-register-bank -I %p/../../include %s \
// RUN:     | FileCheck %s --check-prefix=DEFAULT
// RUN: llvm-tblgen -gen-register-bank -I %p/../../include %s -base-register-bank-class BaseRegBankInfo \
// RUN:     | FileCheck %s --check-prefix=BASE

include "llvm/Target/Target.td"

def MyTarget : Target;
def R0 : Register<"r0">;
let Size = 32 in {
  def ClassA : RegisterClass<"MyTarget", [i32], 32, (add R0)>;
}

// Verify the -base-register-bank-class option.

// DEFAULT: MyTargetGenRegisterBankInfo::MyTargetGenRegisterBankInfo(unsigned HwMode)
// DEFAULT:     : RegisterBankInfo(RegBanks, MyTarget::NumRegisterBanks, Sizes, HwMode) {

// BASE: MyTargetGenRegisterBankInfo::MyTargetGenRegisterBankInfo(unsigned HwMode)
// BASE:     : BaseRegBankInfo(RegBanks, MyTarget::NumRegisterBanks, Sizes, HwMode) {
