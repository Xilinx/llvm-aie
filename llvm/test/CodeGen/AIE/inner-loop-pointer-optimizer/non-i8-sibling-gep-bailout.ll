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
; Regression test: normalizePhiToLoadBase must bail out when a sibling GEP
; off the PHI is not a simple i8-based GEP.  The normalization subtracts the
; byte stride directly from each sibling's last index, which is only correct
; for i8 GEPs.  A non-i8 GEP (e.g. getelementptr <16 x i32>) has element
; indices, not byte indices, so applying a byte delta would corrupt addresses.
;
; The test verifies the loop body is left UNCHANGED (no normalization applied).

; CHECK-LABEL: define void @non_i8_sibling_gep
; CHECK:       loop:
; CHECK:         %ptr = phi ptr
; CHECK:         %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
; CHECK:         %v0 = load <16 x i32>, ptr %ptr.next
; CHECK:         %lane = getelementptr <16 x i32>, ptr %ptr, i20 0, i20 1
; CHECK:         %v1 = load i32, ptr %lane

define void @non_i8_sibling_gep(ptr %in, ptr %out0, ptr %out1, i32 %n) {
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %in, %entry ], [ %ptr.next, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
  %v0 = load <16 x i32>, ptr %ptr.next
  ; Non-i8 GEP: element index, not byte index — must prevent normalization.
  %lane = getelementptr <16 x i32>, ptr %ptr, i20 0, i20 1
  %v1 = load i32, ptr %lane
  store <16 x i32> %v0, ptr %out0
  store i32 %v1, ptr %out1
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}
