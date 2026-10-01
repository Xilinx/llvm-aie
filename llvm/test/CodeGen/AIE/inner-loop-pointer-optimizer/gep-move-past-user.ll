; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 \
; RUN:     -aie-enable-phi-normalization=false \
; RUN:     -aie-enable-post-inc-chain=true \
; RUN:     -stop-after=aie-inner-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s
;
; Regression test: buildPostIncChain must not move a GEP past instructions
; that use it. In this test, %ptr.next is used by a load that appears between
; %ptr.next and the last memory use of %ptr (the store). Moving %ptr.next
; after the store would break the load's operand.
;
; The GEP must remain before its load user.

; CHECK-LABEL: define void @gep_used_between_base_loads
; CHECK:       loop:
; CHECK:         %ptr = phi ptr
; CHECK:         %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
; CHECK-NEXT:    %v = load <16 x i32>, ptr %ptr.next
; CHECK-NEXT:    store <16 x i32> %v, ptr %ptr

define void @gep_used_between_base_loads(ptr %in, i32 %n) {
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %in, %entry ], [ %ptr.next, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
  %v = load <16 x i32>, ptr %ptr.next
  store <16 x i32> %v, ptr %ptr
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}
