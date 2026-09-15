; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -aie-enable-loop-pointer-opt=false \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test: collectDerivedPointerUpdates promotes a top-block GEP that feeds an
; inner-loop PHI as its preheader-entry value (Case 2).
;
; %a.inner.start = GEP(%a.ptr, 4) lives in the outer header. Its base %a.ptr
; is the address operand of a stage-0 load, so %a.inner.start is "based on
; stage 0" via Stage0LoadBases and must be promoted to stage 0 so that the
; inner loop's pointer PHI can be properly pipelined.
;
; Input structure:
;   outer.header:
;     %v0 = load i32, ptr %a.ptr          ; stage-0 load -- %a.ptr in Stage0LoadBases
;     %a.inner.start = GEP(%a.ptr, 4)    ; Case 2: top-block GEP → inner-loop PHI
;     set.loop.iterations(M)
;     -> inner.header
;
;   inner.header:
;     %a.cur = phi [%a.inner.start, outer.header], [%a.cur.next, inner.header]
;     ... accumulate from %a.cur ...
;
;   outer.latch:
;     store result
;     %a.ptr.next = GEP(%a.ptr, 8)       ; lifted to top, promoted via Case 1
;

; CHECK-LABEL: define void @inner_phi_gep

; Stage-0 top: load, Case-2 inner-start GEP, and lifted pointer-advance GEP.
; CHECK: stage0.top:
; CHECK-NEXT:   %v0.steady.top = load i32, ptr %a, align 4
; CHECK-NEXT:   %a.inner.start.steady.top = getelementptr inbounds i32, ptr %a, i32 4
; CHECK-NEXT:   %a.ptr.next.steady.top = getelementptr inbounds i32, ptr %a, i32 8
; CHECK-NOT:    call void @llvm.set.loop.iterations
; CHECK:        br label %steady.stage1.top

; Steady-state header: pipeline PHIs for load, Case-2 GEP, and pointer-advance GEP.
; CHECK: steady.stage1.top:
; CHECK:   %v0.steady.phi = phi i32 [ %v0.steady.top, %stage0.top ], [ %v0.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   %a.inner.start.steady.phi = phi ptr [ %a.inner.start.steady.top, %stage0.top ], [ %a.inner.start.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   %a.ptr.next.steady.phi = phi ptr [ %a.ptr.next.steady.top, %stage0.top ], [ %a.ptr.next.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %steady.stage1.inner.inner.header

; Steady inner header: uses the pipelined %a.inner.start.steady.phi (not the raw GEP).
; CHECK: steady.stage1.inner.inner.header:
; CHECK:   %a.cur.steady = phi ptr [ %a.inner.start.steady.phi, %steady.stage1.top ], [ %a.cur.next.steady, %steady.stage1.inner.inner.header ]

; Steady bottom: prefetch next-iteration load, Case-2 GEP, and pointer-advance GEP.
; CHECK: steady.stage1.bottom.and.stage0.top:
; CHECK:   store i32
; CHECK:   %v0.steady.bottom = load i32, ptr %a.ptr.next.steady.phi, align 4
; CHECK:   %a.inner.start.steady.bottom = getelementptr inbounds i32, ptr %a.ptr.next.steady.phi, i32 4
; CHECK:   %a.ptr.next.steady.bottom = getelementptr inbounds i32, ptr %a.ptr.next.steady.phi, i32 8
; CHECK:   br i1 %outer.cond.steady, label %steady.stage1.top, label %lastiter.stage1.top

; Last-iteration inner: uses the steady bottom's prefetched %a.inner.start.steady.bottom.
; CHECK: lastiter.stage1.inner.inner.header:
; CHECK:   %a.cur.lastiter = phi ptr [ %a.inner.start.steady.bottom, %lastiter.stage1.top ], [ %a.cur.next.lastiter, %lastiter.stage1.inner.inner.header ]

define void @inner_phi_gep(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                            i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  ; Stage-0 load: anchors %a.ptr in Stage0LoadBases.
  %v0 = load i32, ptr %a.ptr, align 4
  ; Top-block GEP feeding the inner-loop PHI as preheader entry (Case 2).
  ; Must be promoted to stage 0 by collectDerivedPointerUpdates.
  %a.inner.start = getelementptr inbounds i32, ptr %a.ptr, i32 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  ; Inner-loop pointer PHI: preheader entry = %a.inner.start (top-block GEP).
  %a.cur = phi ptr [ %a.inner.start, %outer.header ], [ %a.cur.next, %inner.header ]
  %acc = phi i32 [ %v0, %outer.header ], [ %acc.next, %inner.header ]
  %v1 = load i32, ptr %a.cur, align 4
  %acc.next = add i32 %acc, %v1
  %a.cur.next = getelementptr inbounds i32, ptr %a.cur, i32 1
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  ; Advance outer pointer: lifted to top by liftBottomPointerUpdatesToTop,
  ; then promoted to stage 0 via Case 1 + Stage0LoadBases.
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 8
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add i32 %i, 1
  %outer.cond = icmp slt i32 %i.next, %N
  br i1 %outer.cond, label %outer.header, label %exit, !llvm.loop !0

exit:
  ret void
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
