; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: opt -mtriple=aie2p -passes='print<iv-users>' -disable-output %s 2>&1 | FileCheck %s

; With i20 IV users enabled, traversal continues through the trunc and
; collects both the non-GEP and GEP uses of the i20 recurrence.

; CHECK: IV Users for loop %loop
; CHECK-NEXT: %next = {1,+,1}<nuw><nsw><%loop> in {{ *}}%done = icmp eq i32 %next, %n
; CHECK-NEXT: %idx = {0,+,1}<%loop> in {{ *}}%other = udiv i20 %idx, %denom
; CHECK-NEXT: %idx = {0,+,1}<%loop> in {{ *}}%ptr = getelementptr i8, ptr %base, i20 %idx

define void @mixed_trunc_user(ptr %base, i32 %n, i20 %denom) {
entry:
  br label %loop

loop:
  %iv = phi i32 [ 0, %entry ], [ %next, %loop ]
  %idx = trunc i32 %iv to i20
  %ptr = getelementptr i8, ptr %base, i20 %idx
  %value = load i8, ptr %ptr, align 1
  %other = udiv i20 %idx, %denom
  %next = add nuw nsw i32 %iv, 1
  %done = icmp eq i32 %next, %n
  br i1 %done, label %exit, label %loop

exit:
  ret void
}
