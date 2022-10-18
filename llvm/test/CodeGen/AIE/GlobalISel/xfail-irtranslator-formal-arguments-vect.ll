;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: not --crash llc -mtriple=aie -O0 -stop-after=irtranslator -global-isel -verify-machineinstrs %s -o - 2>&1 | FileCheck %s

; REQUIRES: asserts

; CHECK: Assertion `LocVT == MVT::i32 && "VT should be split in 32-bits chunks"' failed.
define <16 x i64> @arg_v16int64(<16 x i64> %a) {
  ret <16 x i64> %a
}
