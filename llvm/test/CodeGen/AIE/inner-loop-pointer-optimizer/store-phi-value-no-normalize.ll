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
; Regression test: normalizePhiToLoadBase must bail out when the pointer PHI
; is stored as a VALUE (not used as a memory address). Shifting the phi by
; the stride would change the pointer value saved to memory.
;
; In this test, `store ptr %ptr, ptr %slot` saves the cursor pointer.
; The pass must NOT normalize (shift) %ptr, because that would store
; %in + 64 on the first iteration instead of %in.

; CHECK-LABEL: define void @store_phi_value_no_normalize
; CHECK:       loop:
; CHECK:         %ptr = phi ptr
; CHECK:         %ptr.preinc = getelementptr inbounds i8, ptr %ptr, i20 64
; CHECK:         store ptr %ptr, ptr %slot
; CHECK:         %v = load i32, ptr %ptr.preinc

define void @store_phi_value_no_normalize(ptr %in, ptr %slot, ptr %out, i32 %n) {
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %in, %entry ], [ %ptr.preinc, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  %ptr.preinc = getelementptr inbounds i8, ptr %ptr, i20 64
  store ptr %ptr, ptr %slot
  %v = load i32, ptr %ptr.preinc
  store i32 %v, ptr %out
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}
