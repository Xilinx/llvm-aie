;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
; RUN: split-file %s %t
; RUN: not llc -mtriple=aie2ps %t/set_ctrl.ll -o /dev/null 2>&1 | FileCheck %s --check-prefix=SET-CTRL
; RUN: not llc -mtriple=aie2ps %t/get_ctrl.ll -o /dev/null 2>&1 | FileCheck %s --check-prefix=GET-CTRL

; An index that maps to no control register fails selection.

; SET-CTRL: LLVM ERROR: cannot select: {{.*}}intrinsic(@llvm.aie2ps.set.ctrl.reg)
; GET-CTRL: LLVM ERROR: cannot select: {{.*}}intrinsic(@llvm.aie2ps.get.ctrl.reg)

;--- set_ctrl.ll
define void @f(i32 %v) {
  call void @llvm.aie2ps.set.ctrl.reg(i32 43, i32 %v)
  ret void
}
declare void @llvm.aie2ps.set.ctrl.reg(i32, i32)

;--- get_ctrl.ll
define i32 @f() {
  %v = call i32 @llvm.aie2ps.get.ctrl.reg(i32 43)
  ret i32 %v
}
declare i32 @llvm.aie2ps.get.ctrl.reg(i32)
