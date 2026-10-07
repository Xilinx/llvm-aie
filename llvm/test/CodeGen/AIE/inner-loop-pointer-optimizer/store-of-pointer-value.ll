; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 \
; RUN:     -stop-after=aie-inner-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s
;
; Regression test: a store whose *value* operand is the pointer PHI must not
; be treated as a memory address use. Previously, `store ptr %phi, ptr %slot`
; was incorrectly classified as %phi being a load/store base, which caused
; the post-increment GEP to be repositioned after that store, breaking the
; definition order (%ptr.next used before its GEP is placed).
;
; The critical invariant: %ptr.next GEP must remain BEFORE the load that uses
; it (%v1 = load ... ptr %ptr.next).

; CHECK-LABEL: define void @store_pointer
; CHECK:       loop:
; CHECK:         %ptr = phi ptr
; CHECK:         %v0 = load i32, ptr %ptr
; CHECK-NEXT:    %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
; CHECK-NEXT:    %v1 = load i32, ptr %ptr.next
; CHECK-NEXT:    store ptr %ptr, ptr %slot
; CHECK-NEXT:    store i32 %v0, ptr %out0
; CHECK-NEXT:    store i32 %v1, ptr %out1

define void @store_pointer(ptr %in, ptr %slot, ptr %out0, ptr %out1, i32 %n) {
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %in, %entry ], [ %ptr.next, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  %v0 = load i32, ptr %ptr
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
  %v1 = load i32, ptr %ptr.next
  store ptr %ptr, ptr %slot
  store i32 %v0, ptr %out0
  store i32 %v1, ptr %out1
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}
