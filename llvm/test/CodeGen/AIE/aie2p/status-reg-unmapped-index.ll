;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
; RUN: split-file %s %t
; RUN: not llc -mtriple=aie2p %t/set_status.ll -o /dev/null 2>&1 | FileCheck %s --check-prefix=SET-STATUS
; RUN: not llc -mtriple=aie2p %t/get_status.ll -o /dev/null 2>&1 | FileCheck %s --check-prefix=GET-STATUS

; An index that maps to no status register fails selection.

; SET-STATUS: LLVM ERROR: cannot select: {{.*}}intrinsic(@llvm.aie2p.set.status.reg)
; GET-STATUS: LLVM ERROR: cannot select: {{.*}}intrinsic(@llvm.aie2p.get.status.reg)

;--- set_status.ll
define void @f(i32 %v) {
  call void @llvm.aie2p.set.status.reg(i32 24, i32 %v)
  ret void
}
declare void @llvm.aie2p.set.status.reg(i32, i32)

;--- get_status.ll
define i32 @f() {
  %v = call i32 @llvm.aie2p.get.status.reg(i32 24)
  ret i32 %v
}
declare i32 @llvm.aie2p.get.status.reg(i32)
