; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; An in-place narrowing conversion. Both cursors are rooted at %p, but the load
; advances 64 bytes per iteration while the store advances 32. Update counting
; cannot relate them: one update means a different distance on each cursor, so
; it needs the two rates to be identical.
;
; ByteGeometry compares the address intervals instead, which rules out the
; store->load edge that otherwise pins RecMII to the length of the whole loop
; body. See VirtUnrollAliasAnalysisTest.TestDifferentSteps for the underlying
; alias queries, including the opposite direction that must stay MayAlias.
;
; RUN: llc -mtriple=aie2p -O2 %s -pass-remarks-output=- \
; RUN:   -pass-remarks-filter=pipeliner -o /dev/null | FileCheck %s
;
; RUN: llc -mtriple=aie2p -O2 %s -pass-remarks-output=- \
; RUN:   -pass-remarks-filter=pipeliner -aie-alias-analysis-byte-geometry=false \
; RUN:   -o /dev/null | FileCheck %s --check-prefix=UPDATE-COUNT

; CHECK:          - II:              '1'
; UPDATE-COUNT:   - II:              '10'

define void @narrowing_inplace(ptr noalias %p, i32 %n) {
entry:
  %nz = icmp ne i32 %n, 0
  tail call void @llvm.assume(i1 %nz)
  br label %loop

loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pl = phi ptr [ %p, %entry ], [ %pl.next, %loop ]
  %ps = phi ptr [ %p, %entry ], [ %ps.next, %loop ]
  %v = load <16 x i32>, ptr %pl, align 64
  %h = shufflevector <16 x i32> %v, <16 x i32> poison, <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
  %w = add <8 x i32> %h, splat (i32 7)
  store <8 x i32> %w, ptr %ps, align 32
  %pl.next = getelementptr inbounds i8, ptr %pl, i20 64
  %ps.next = getelementptr inbounds i8, ptr %ps, i20 32
  %i.next = add nuw nsw i32 %i, 1
  %c = icmp ult i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !0

exit:
  ret void
}

declare void @llvm.assume(i1 noundef)

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.itercount.range", i32 16, i32 1000}
