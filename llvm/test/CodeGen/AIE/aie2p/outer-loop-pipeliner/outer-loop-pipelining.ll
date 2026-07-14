; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s


; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s \
; RUN:   | llc -mtriple=aie2p -x mir -run-pass=none -o /dev/null

; Test for the AIE Outer Loop Pipelining pass.
;
; The pass rotates the outer loop body so that the prologue loads of iteration
; i+1 overlap with the epilogue stores of iteration i.
;
; Input structure:
;   outer_header (prologue):
;     v0 = load(a_ptr)
;     v1 = load(b_ptr)
;     call void @llvm.set.loop.iterations.i32(i32 %M)
;     -> inner loop
;   inner loop (hardware loop):
;     %cond = call i1 @llvm.loop.decrement.i32(i32 1)
;     br i1 %cond, label %inner.header, label %outer.latch
;   outer_latch (epilogue):
;     store(result, c_ptr)
;     -> outer_header or exit
;
; Expected output after transformation:
;
;   [stage0.top]             <- stage-0 top: DATA LOADS ONLY (no set.loop.iterations)
;       v0.top = load(a)
;       v1.top = load(b)
;       br outer.header
;
;   [outer.header]           <- PHIs for pipelined values + set.loop.iterations
;       %v0.phi = phi [v0.top, stage0.top], [v0.bottom, outer.latch]
;       %v1.phi = phi [v1.top, stage0.top], [v1.bottom, outer.latch]
;       call void @llvm.set.loop.iterations.i32(i32 %M)
;       br inner.header
;
;   [outer.latch]            <- stores + loads for NEXT iteration
;       store result
;       v0.bottom = load(a.ptr.next)   ; uses next-iteration pointer
;       v1.bottom = load(b.ptr.next)
;       br outer.header or lastiter.stage1.top
;
;   [lastiter.stage1.top]         <- set.loop.iterations for last iteration
;       call void @llvm.set.loop.iterations.i32(i32 %M)
;       br inner.header.cd
;
;   [inner.header.lastiter]        <- cloned inner loop
;       br lastiter.stage1.bottom
;
;   [lastiter.stage1.bottom]          <- stores only (no loads)
;       store result.lastiter
;       br exit

; CHECK-LABEL: define void @nested_loop_basic

; Warm-up block: DATA LOADS ONLY -- no set.loop.iterations here
; CHECK: stage0.top:
; CHECK-NEXT: %v0.steady.top = load i32, ptr %a, align 4
; CHECK-NEXT: %v1.steady.top = load i32, ptr %b, align 4
; CHECK-NOT:  call void @llvm.set.loop.iterations
; CHECK:      br label %steady.stage1.top

; Outer header: PHI nodes for pipelined values + set.loop.iterations stays
; CHECK: steady.stage1.top:
; CHECK:   phi i32 [ %i.next.steady, %steady.stage1.bottom.and.stage0.top ], [ 0, %stage0.top ]
; CHECK:   phi ptr [ %a.ptr.next.steady, %steady.stage1.bottom.and.stage0.top ], [ %a, %stage0.top ]
; CHECK:   %v0.steady.phi = phi i32 [ %v0.steady.top, %stage0.top ], [ %v0.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   %v1.steady.phi = phi i32 [ %v1.steady.top, %stage0.top ], [ %v1.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %steady.stage1.inner.inner.header

; Outer latch: stores + loads for NEXT iteration (uses a.ptr.next.steady, b.ptr.next.steady)
; CHECK: steady.stage1.bottom.and.stage0.top:
; CHECK:   store i32
; CHECK:   %v0.steady.bottom = load i32, ptr %a.ptr.next.steady, align 4
; CHECK:   %v1.steady.bottom = load i32, ptr %b.ptr.next.steady, align 4
; CHECK:   br i1 %outer.cond.steady, label %steady.stage1.top, label %lastiter.stage1.top

; Cool-down entry: set.loop.iterations for last iteration
; CHECK: lastiter.stage1.top:
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %lastiter.stage1.inner.inner.header

; Cloned inner loop
; CHECK: lastiter.stage1.inner.inner.header:
; CHECK:   br i1 %inner.cond.lastiter, label %lastiter.stage1.inner.inner.header, label %lastiter.stage1.bottom

; Cool-down exit: stores only (no loads), branches to exit
; CHECK: lastiter.stage1.bottom:
; CHECK:   store i32
; CHECK-NOT: load
; CHECK:   br label %exit

define void @nested_loop_basic(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                                i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %b.ptr = phi ptr [ %b, %entry ], [ %b.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  ; Prologue: loads for the inner loop
  %v0 = load i32, ptr %a.ptr, align 4
  %v1 = load i32, ptr %b.ptr, align 4
  ; Set up hardware loop counter
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %prod = mul i32 %v0, %v1
  %acc.next = add i32 %acc, %prod
  ; Hardware loop decrement: returns i1 (true = continue loop)
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  ; Epilogue: store result
  store i32 %acc.next, ptr %c.ptr, align 4
  ; Advance pointers
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %b.ptr.next = getelementptr inbounds i32, ptr %b.ptr, i32 1
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

; ============================================================================
; Test 2: Values computed in outer.header from loads flow to inner.header via
; PHI nodes. The computed value (%init_acc = v0 * v1) is used as the initial
; value of the inner loop accumulator PHI.
;
; After transformation:
;   - %init_acc.top = mul %v0.top, %v1.top  (in stage0.top)
;   - %init_acc.phi = phi [top, top], [epi, latch]  (in outer.header)
;   - %acc = phi [%init_acc.phi, outer.header], [%acc.next, inner.header]
;   - %acc.lastiter = phi [%init_acc.bottom, lastiter.stage1.top], [...]  (in last-iteration)
; ============================================================================

; CHECK-LABEL: define void @outer_to_inner_phi

; Warm-up: loads + dependent computation, no set.loop.iterations
; CHECK: stage0.top:
; CHECK:   %v0.steady.top = load i32, ptr %a, align 4
; CHECK:   %v1.steady.top = load i32, ptr %b, align 4
; CHECK:   %init_acc.steady.top = mul i32 %v0.steady.top, %v1.steady.top
; CHECK-NOT: call void @llvm.set.loop.iterations
; CHECK:   br label %steady.stage1.top

; Outer header: pipelined PHIs for loads AND the dependent computation
; CHECK: steady.stage1.top:
; CHECK:   %v0.steady.phi = phi i32 [ %v0.steady.top, %stage0.top ], [ %v0.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   %v1.steady.phi = phi i32 [ %v1.steady.top, %stage0.top ], [ %v1.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   %init_acc.steady.phi = phi i32 [ %init_acc.steady.top, %stage0.top ], [ %init_acc.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %steady.stage1.inner.inner.header

; Inner header: PHI uses the pipelined %init_acc.steady.phi from steady.stage1.top
; CHECK: steady.stage1.inner.inner.header:
; CHECK:   %acc.steady = phi i32 [ %init_acc.steady.phi, %steady.stage1.top ], [ %acc.next.steady, %steady.stage1.inner.inner.header ]

; Outer latch: epilogue loads + dependent computation for next iteration
; CHECK: steady.stage1.bottom.and.stage0.top:
; CHECK:   store i32
; CHECK:   %v0.steady.bottom = load i32, ptr %a.ptr.next.steady, align 4
; CHECK:   %v1.steady.bottom = load i32, ptr %b.ptr.next.steady, align 4
; CHECK:   %init_acc.steady.bottom = mul i32 %v0.steady.bottom, %v1.steady.bottom
; CHECK:   br i1 %outer.cond.steady, label %steady.stage1.top, label %lastiter.stage1.top

; Cool-down: inner loop uses last epilogue's init_acc value
; CHECK: lastiter.stage1.top:
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %lastiter.stage1.inner.inner.header

; CHECK: lastiter.stage1.inner.inner.header:
; CHECK:   %acc.lastiter = phi i32 [ %init_acc.steady.bottom, %lastiter.stage1.top ], [ %acc.next.lastiter, %lastiter.stage1.inner.inner.header ]
; CHECK:   br i1 %inner.cond.lastiter, label %lastiter.stage1.inner.inner.header, label %lastiter.stage1.bottom

; CHECK: lastiter.stage1.bottom:
; CHECK:   store i32 %acc.next.lastiter
; CHECK:   br label %exit

define void @outer_to_inner_phi(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                                 i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %b.ptr = phi ptr [ %b, %entry ], [ %b.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  ; Prologue: loads + a computation that flows to inner.header via PHI
  %v0 = load i32, ptr %a.ptr, align 4
  %v1 = load i32, ptr %b.ptr, align 4
  ; This value is computed from the loads and used as the inner loop's
  ; initial accumulator value (flows via inner.header PHI node)
  %init_acc = mul i32 %v0, %v1
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  ; PHI takes initial value from outer.header computation (not a constant)
  %acc = phi i32 [ %init_acc, %outer.header ], [ %acc.next, %inner.header ]
  %prod = mul i32 %acc, %v1
  %acc.next = add i32 %acc, %prod
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !5

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %b.ptr.next = getelementptr inbounds i32, ptr %b.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add i32 %i, 1
  %outer.cond = icmp slt i32 %i.next, %N
  br i1 %outer.cond, label %outer.header, label %exit, !llvm.loop !4

exit:
  ret void
}

!4 = distinct !{!4, !6, !7}
!5 = distinct !{!5, !6}
!6 = !{!"llvm.loop.mustprogress"}
!7 = !{!"llvm.loop.itercount.range", i32 2}

; ============================================================================
; Test 3: Inner loop llvm.loop.itercount.range is preserved after transformation.
;
; The outer loop has itercount.range=2 (dropped after transformation since
; the trip count changes to N-1). The inner loop has itercount.range=8 which
; must be preserved in both the original inner loop and the cool-down clone.
;
; Key checks:
;   - Both inner.header and inner.header.cd reference the SAME !llvm.loop node
;   - That node contains "llvm.loop.itercount.range", i32 8
;   - The outer loop's !llvm.loop node does NOT contain itercount.range
; ============================================================================

; CHECK-LABEL: define void @inner_range_preserved

; Capture the inner loop's metadata reference from the original inner loop branch.
; CHECK: steady.stage1.inner.inner.header:
; CHECK:   br i1 %inner.cond{{.*}}, !llvm.loop [[INNER_MD:![0-9]+]]

; Capture the outer loop's metadata reference (to verify itercount.range was dropped).
; CHECK: steady.stage1.bottom.and.stage0.top:
; CHECK:   br i1 %outer.cond{{.*}}, !llvm.loop [[OUTER_MD:![0-9]+]]

; The cool-down clone must reference the SAME inner loop metadata node.
; CHECK: lastiter.stage1.inner.inner.header:
; CHECK:   br i1 %inner.cond.lastiter{{.*}}, !llvm.loop [[INNER_MD]]

; Outer loop's itercount.range=2 must be UPDATED to 1 (trip count changed to N-1).
; CHECK-NOT: "llvm.loop.itercount.range", i32 2

; Inner loop's itercount.range=8 must be PRESERVED in the metadata section.
; CHECK: "llvm.loop.itercount.range", i32 8

define void @inner_range_preserved(ptr noalias %a, ptr noalias %b, ptr noalias %c,
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
  ; Inner loop has itercount.range=8 -- must be preserved after transformation
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !9

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds i32, ptr %a.ptr, i32 1
  %b.ptr.next = getelementptr inbounds i32, ptr %b.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add i32 %i, 1
  %outer.cond = icmp slt i32 %i.next, %N
  ; Outer loop has itercount.range=2 -- must be DROPPED after transformation
  br i1 %outer.cond, label %outer.header, label %exit, !llvm.loop !8

exit:
  ret void
}

!8 = distinct !{!8, !10, !11}
!9 = distinct !{!9, !10, !12}
!10 = !{!"llvm.loop.mustprogress"}
!11 = !{!"llvm.loop.itercount.range", i32 2}
!12 = !{!"llvm.loop.itercount.range", i32 8}
