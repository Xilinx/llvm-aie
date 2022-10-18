;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie < %s | FileCheck %s

target triple = "aie"

define i32 @test(i32 inreg %a) local_unnamed_addr {
; CHECK-LABEL: test:
; CHECK:       // %bb.0:
; CHECK:        event   #1
; CHECK:         ret lr
entry:
  tail call void @llvm.aie.event(i32 1)
  ret i32 1
}

; Function Attrs: nounwind
declare void @llvm.aie.event(i32)
