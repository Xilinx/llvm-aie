//===- base-registerinfo-class.td --------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-tblgen -gen-register-info -I %p/../../include -I %p/Common %s \
// RUN:     | FileCheck %s --check-prefix=DEFAULT
// RUN: llvm-tblgen -gen-register-info -I %p/../../include -I %p/Common %s -base-registerinfo-class BaseRegInfoClass \
// RUN:     | FileCheck %s --check-prefix=BASE

// Verify the -base-registerinfo-class option

include "reg-with-subregs-common.td"

// DEFAULT: struct TestTargetGenRegisterInfo : public TargetRegisterInfo
// DEFAULT: TestTargetGenRegisterInfo(
// DEFAULT:  : TargetRegisterInfo(

// BASE: struct TestTargetGenRegisterInfo : public BaseRegInfoClass
// BASE: TestTargetGenRegisterInfo(
// BASE:  : BaseRegInfoClass(
