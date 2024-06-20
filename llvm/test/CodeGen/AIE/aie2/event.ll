;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 < %s | FileCheck %s

target triple = "aie2"

define void @test() local_unnamed_addr {
; CHECK-LABEL: test:
; CHECK:       // %bb.0:
; CHECK-DAG:        event   #2
; CHECK-DAG:        ret lr
entry:
  tail call void @llvm.aie2.event(i32 2)
  ret void
}

; Function Attrs: nounwind
declare void @llvm.aie2.event(i32)
