; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s


; RUN: llc -mtriple=aie2p -O2 -stop-after=aie-outer-loop-pipeliner -o - %s \
; RUN:   | llc -mtriple=aie2p -x mir -run-pass=none -o /dev/null

; Tests for per-loop metadata control of the AIE Outer Loop Pipelining pass
; via !llvm.loop.hint.aie-enable-outer-loop-pipelining.
;
; The global flag (-aie-enable-outer-loop-pipelining) is intentionally NOT
; passed in any RUN line. Pipelining is controlled entirely by per-loop
; metadata:
;
;   !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1}  -> opt-in
;   !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 0}  -> opt-out
;
; Precedence rules (from LoopOptionOverrides):
;   1. Explicit command-line setting (highest)
;   2. Per-loop metadata override
;   3. cl::opt default value (lowest)
;
; Because the global flag is at its default (false), the metadata value is
; authoritative for each loop independently.

; ============================================================================
; Test 1: metadata opt-in
;
; The outer loop carries:
;   !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1}
;
; Expected: the pass pipelines the loop even without the global flag.
; The warm-up block (outer.header.peel.pro) must be present.
; ============================================================================

; CHECK-LABEL: define void @metadata_opt_in

; Warm-up block must be created (metadata opt-in triggered pipelining).
; CHECK: outer.header.peel.pro:
; CHECK:   %v0.peel = load i32, ptr %a, align 4
; CHECK:   %v1.peel = load i32, ptr %b, align 4
; CHECK:   br label %outer.header

; Outer header must have pipelined PHIs.
; CHECK: outer.header:
; CHECK:   %v0.phi = phi i32 [ %v0.peel, %outer.header.peel.pro ], [ %v0.epi, %outer.latch ]
; CHECK:   %v1.phi = phi i32 [ %v1.peel, %outer.header.peel.pro ], [ %v1.epi, %outer.latch ]

define void @metadata_opt_in(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                              i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %b.ptr = phi ptr [ %b, %entry ], [ %b.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %v0 = load i32, ptr %a.ptr, align 4
  %v1 = load i32, ptr %b.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %prod = mul i32 %v0, %v1
  %acc.next = add i32 %acc, %prod
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %b.ptr.next = getelementptr inbounds i32, ptr %b.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add i32 %i, 1
  %outer.cond = icmp slt i32 %i.next, %N
  ; Outer loop opts in via metadata (global flag is OFF).
  br i1 %outer.cond, label %outer.header, label %exit, !llvm.loop !0

exit:
  ret void
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

; Outer loop metadata: mustprogress + itercount.range=2 + opt-in hint
!0 = distinct !{!0, !2, !3, !4}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
!4 = !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1}

; ============================================================================
; Test 2: metadata opt-out
;
; The outer loop carries:
;   !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 0}
;
; Expected: the pass skips the loop (global flag is OFF, metadata says 0).
; The warm-up block (outer.header.peel.pro) must NOT appear inside this
; function (checked between the function label and the first outer.header).
; ============================================================================

; CHECK-LABEL: define void @metadata_opt_out

; No warm-up block between the function entry and outer.header.
; CHECK-NOT: outer.header.peel.pro
; The outer header must retain the original loads (no pipelined PHIs).
; CHECK: outer.header:
; CHECK:   %v0 = load i32, ptr %a.ptr, align 4
; CHECK:   %v1 = load i32, ptr %b.ptr, align 4

; The opt-out loop's metadata must still carry the enable hint (not consumed)
; and must NOT carry the success marker (loop was not pipelined).
; (Metadata section checks are at the bottom of this file, after both
;  function bodies, because the metadata section appears last in the output.)

define void @metadata_opt_out(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                               i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %b.ptr = phi ptr [ %b, %entry ], [ %b.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %v0 = load i32, ptr %a.ptr, align 4
  %v1 = load i32, ptr %b.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %prod = mul i32 %v0, %v1
  %acc.next = add i32 %acc, %prod
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !6

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %b.ptr.next = getelementptr inbounds i32, ptr %b.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add i32 %i, 1
  %outer.cond = icmp slt i32 %i.next, %N
  ; Outer loop opts out via metadata (global flag is OFF, metadata says 0).
  br i1 %outer.cond, label %outer.header, label %exit, !llvm.loop !5

exit:
  ret void
}

; Outer loop metadata: mustprogress + itercount.range=2 + opt-out hint
!5 = distinct !{!5, !2, !3, !7}
!6 = distinct !{!6, !2}
!7 = !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 0}

; ============================================================================
; Metadata section checks (appear after both function bodies in the output)
;
; For @metadata_opt_in (pipelined):
;   - The success marker must be present.
;   - The consumed enable hint (i64 1) must have been dropped.
; For @metadata_opt_out (not pipelined):
;   - The enable hint (i64 0) must still be present (not consumed).
;   - The success marker must NOT be present.
; ============================================================================

; Success marker present for the pipelined loop.
; CHECK: "llvm.loop.hint.aie_outerloop_pipeliner_success", i64 1

; The consumed enable hint (i64 1) must NOT appear anywhere in the output.
; CHECK-NOT: "llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1

; The opt-out loop's enable hint (i64 0) must still be present.
; CHECK: "llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 0

; No success marker for the opt-out loop (appears after the enable hint above).
; CHECK-NOT: "llvm.loop.hint.aie_outerloop_pipeliner_success"
