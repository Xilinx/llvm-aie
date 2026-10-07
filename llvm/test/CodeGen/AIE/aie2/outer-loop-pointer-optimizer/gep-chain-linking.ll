; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2 -O2 -aie-enable-outer-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=true \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -stop-after=aie-outer-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Do not rebase when the emitted step is outside imm10x4.
;
; %live is %base+2048 and stays live. %next is %live+64, which encodes. After
; one inner iteration %p.step is %base+64, so rebasing %next would emit
; %p.step+2048. 2048 is outside imm10x4 (a multiple of 4 in [-2048, 2044])
; and would need a modifier register, so %next stays on %live.

; CHECK-LABEL: define i8 @test_no_rebase_past_imm10x4
; CHECK: top:
; CHECK:   %[[LIVE:.*]] = getelementptr inbounds i8, ptr %base, i20 2048
; CHECK: inner:
; CHECK:   %p.step = getelementptr inbounds i8, ptr %p.inner, i20 64
; CHECK: bottom:
; CHECK:   %[[NEXT:.*]] = getelementptr inbounds i8, ptr %p.step, i20 2048
; CHECK:   %value = load i8, ptr %[[NEXT]]

define i8 @test_no_rebase_past_imm10x4(ptr %base, i32 %N) {
entry:
  br label %top

top:
  %outer.iv = phi i32 [ 0, %entry ], [ %outer.iv.next, %bottom ]
  %live = getelementptr inbounds i8, ptr %base, i20 2048
  %kept = load i8, ptr %live
  br label %inner

inner:
  %inner.iv = phi i32 [ 0, %top ], [ %inner.iv.next, %inner ]
  %p.inner = phi ptr [ %base, %top ], [ %p.step, %inner ]
  %p.inner.value = load volatile i8, ptr %p.inner
  %p.step = getelementptr inbounds i8, ptr %p.inner, i20 64
  %inner.iv.next = add nuw nsw i32 %inner.iv, 1
  %inner.done = icmp eq i32 %inner.iv.next, 1
  br i1 %inner.done, label %bottom, label %inner

bottom:
  %next = getelementptr inbounds i8, ptr %live, i20 64
  %value = load i8, ptr %next
  %sum = add i8 %kept, %value
  %outer.iv.next = add nuw nsw i32 %outer.iv, 1
  %outer.done = icmp eq i32 %outer.iv.next, %N
  br i1 %outer.done, label %exit, label %top

exit:
  ret i8 %sum
}
