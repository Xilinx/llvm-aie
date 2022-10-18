;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: not --crash llc -mtriple=aie -O0 -stop-after=irtranslator -global-isel -verify-machineinstrs %s -o - 2>&1 | FileCheck %s

; The v2i128 ValueType doesn't exist in LLVM at the moment, and needs to be added before we can support it for AIE

; CHECK: Could not allocate 8 GPRs for split type ValVT=i32 LocVT=i32
; CHECK: UNREACHABLE executed
define <2 x i128> @ret_v2int128() {
  ret <2 x i128> zeroinitializer
}
