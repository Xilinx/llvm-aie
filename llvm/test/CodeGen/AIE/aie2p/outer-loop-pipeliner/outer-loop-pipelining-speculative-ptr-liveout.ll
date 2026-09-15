; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s

; RUN: llc -mtriple=aie2p -O2 -aie-enable-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s \
; RUN:   | llc -mtriple=aie2p -x mir -run-pass=none -o /dev/null

; Verify pointer live-outs when a pointer update feeds an outer-header PHI
; and is also used outside the loop.
;
; For N iterations:
;   - Original: ptr stored to %out is %a + N*4.
;   - The stored ptr must be %a + N*4, not %a + (N+1)*4.
;
; The GEP is promoted to stage0.top and tracked via PHI as %ptr.next.steady.phi.
; The bottom block computes %ptr.next.steady.bottom for the next iteration's
; prefetch; after the last steady bottom this value equals %a + N*4.

; CHECK-LABEL: define void @speculative_ptr_liveout

; Stage-0 top: load + promoted GEP for the pointer pipeline.
; CHECK: stage0.top:
; CHECK:   %loaded.steady.top = load i32, ptr %a, align 4
; CHECK:   %ptr.next.steady.top = getelementptr inbounds i32, ptr %a, i32 1

; Steady-state header: ptr.steady tracks current iteration's base,
; ptr.next.steady.phi tracks next iteration's base (pipeline value).
; CHECK: steady.stage1.top:
; CHECK:   %ptr.steady = phi ptr [ %ptr.next.steady.phi, %steady.stage1.bottom.and.stage0.top ], [ %a, %stage0.top ]
; CHECK:   %ptr.next.steady.phi = phi ptr [ %ptr.next.steady.top, %stage0.top ], [ %ptr.next.steady.bottom, %steady.stage1.bottom.and.stage0.top ]

; Steady-state bottom: load from ptr.next.steady.phi, then advance the pointer
; pipeline by one more step for the subsequent iteration.
; CHECK: steady.stage1.bottom.and.stage0.top:
; CHECK:   %loaded.steady.bottom = load i32, ptr %ptr.next.steady.phi, align 4
; CHECK:   %ptr.next.steady.bottom = getelementptr inbounds i32, ptr %ptr.next.steady.phi, i32 1

; The last iteration does NOT need to recreate the GEP: the exit uses
; ptr.next.steady.bottom from the last steady-bottom block.
; CHECK: lastiter.stage1.bottom:
; CHECK:   store i32 %result.next.lastiter, ptr %ptr.next.steady.phi, align 4

; The exit stores ptr.next.steady.bottom (= %a + N*4), not poison.
; CHECK: exit:
; CHECK:   store ptr %ptr.next.steady.bottom, ptr %out, align 4

; The early exit is untouched.
; CHECK: exit.early:
; CHECK:   store ptr %a, ptr %out, align 4

define void @speculative_ptr_liveout(ptr %a, ptr %out, i32 %n, i32 %m) {
entry:
  %has.work = icmp ugt i32 %n, 1
  br i1 %has.work, label %outer.header, label %exit.early

outer.header:
  %iv = phi i32 [ %n, %entry ], [ %iv.next, %outer.latch ]
  %ptr = phi ptr [ %a, %entry ], [ %ptr.next, %outer.latch ]
  %loaded = load i32, ptr %ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %m)
  br label %inner.header

inner.header:
  %result = phi i32 [ 0, %outer.header ], [ %result.next, %inner.header ]
  %result.next = add i32 %result, %loaded
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %result.next, ptr %ptr, align 4
  %ptr.next = getelementptr inbounds i32, ptr %ptr, i32 1
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  store ptr %ptr.next, ptr %out, align 4
  ret void

exit.early:
  store ptr %a, ptr %out, align 4
  ret void
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
