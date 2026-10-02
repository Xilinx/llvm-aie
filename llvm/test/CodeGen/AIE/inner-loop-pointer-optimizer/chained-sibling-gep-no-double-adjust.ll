; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 \
; RUN:     -aie-enable-phi-normalization=true \
; RUN:     -aie-enable-post-inc-chain=false \
; RUN:     -stop-after=aie-inner-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s
;
; Regression test: when normalizePhiToLoadBase shifts a PHI by Stride,
; only GEPs whose pointer operand is the PHI itself should have their
; offset adjusted. Transitive GEPs (rooted on an already-adjusted GEP)
; inherit the shift through their base and must NOT be touched.
;
; Before (stride = 128):
;   %g1 = gep %phi, 192    ; rooted on phi -> adjust to 192-128=64
;   %g2 = gep %g1,  64     ; rooted on g1  -> do NOT adjust
;
; After:
;   %g1 = gep %phi, 64     ; adjusted
;   %g2 = gep %g1,  64     ; unchanged (transitively shifted via g1)

; CHECK-LABEL: define void @chained_sibling_gep
; CHECK:       loop:
; CHECK:         %ptr = phi ptr
; CHECK:         %g1 = getelementptr inbounds i8, ptr %ptr, i20 64
; CHECK:         %g2 = getelementptr inbounds i8, ptr %g1, i20 64
; CHECK:         load i32, ptr %ptr
; CHECK:         load i32, ptr %g1
; CHECK:         load i32, ptr %g2

define void @chained_sibling_gep(ptr %init, ptr %out, i32 %n) {
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %init, %entry ], [ %ptr.back, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  ; Back-edge GEP (stride = 128)
  %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 128
  ; Sibling rooted on phi (will be adjusted: 192 - 128 = 64)
  %g1 = getelementptr inbounds i8, ptr %ptr, i20 192
  ; Chained GEP rooted on g1 (must NOT be adjusted)
  %g2 = getelementptr inbounds i8, ptr %g1, i20 64
  %v0 = load i32, ptr %ptr.back
  %v1 = load i32, ptr %g1
  %v2 = load i32, ptr %g2
  store i32 %v0, ptr %out
  store i32 %v1, ptr %out
  store i32 %v2, ptr %out
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}
