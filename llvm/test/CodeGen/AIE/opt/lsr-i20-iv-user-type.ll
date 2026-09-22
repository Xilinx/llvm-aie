;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: opt -mtriple=aie2p -passes='print<iv-users>' -disable-output %s 2>&1 | FileCheck %s

; Verify that AIE accepts index-sized (i20) integers as valid IV user types
; via the isValidIVUserType TTI hook. There is no trunc here, so the
; look-through hook cannot fire: this isolates isValidIVUserType.

; With the hook, the index-sized i20 recurrences are collected as IV users.
; CHECK: IV Users for loop %loop with backedge-taken count 99:
; CHECK-NEXT: %iv.next = {1,+,1}<nuw><nsw><%loop> in %c = icmp eq i20 %iv.next, 100
; CHECK-NEXT: %idx = {%step,+,1}<nw><%loop> in %g = getelementptr i32, ptr %p, i20 %idx

define void @i20_iv(ptr %p, i20 %step) {
entry:
  br label %loop
loop:
  %iv = phi i20 [ 0, %entry ], [ %iv.next, %loop ]
  %idx = add i20 %iv, %step
  %g = getelementptr i32, ptr %p, i20 %idx
  store i32 42, ptr %g
  %iv.next = add i20 %iv, 1
  %c = icmp eq i20 %iv.next, 100
  br i1 %c, label %exit, label %loop
exit:
  ret void
}
