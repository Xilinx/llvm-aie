; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s


; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s \
; RUN:   | llc -mtriple=aie2p -x mir -run-pass=none -o /dev/null

; Test for pointer update lifting in the AIE Outer Loop Pipeliner pass.
;
; When pointer update instructions (GEPs, addrspacecasts, add.2d, add.3d, etc.)
; are in the epilogue but feed outer header PHIs, they should be lifted to
; the prologue IF they have no uses in the epilogue (outside the chain).
;
; Key behaviors tested:
;   1. Pointer-type PHI chains ARE lifted if no epilogue uses
;   2. Integer-type PHI chains (loop counters) are NOT lifted - they're used by icmp
;   3. Chains with inner loop dependencies cannot be lifted
;
; ============================================================================
; Test 1: Basic pointer update lifting with loop counter preservation
;
; The GEPs (a.ptr.next, c.ptr.next) are lifted because:
;   - They have NO uses in the epilogue (stores use c.ptr, not c.ptr.next)
;
; The loop counter (iv.next) is NOT lifted because:
;   - It's used by the icmp in the epilogue (epilogue use)
; ============================================================================

; CHECK-LABEL: define void @ptr_lifting_basic

; Stage-0 top block: loads only, plus hardware loop setup
; CHECK: stage0.top:
; CHECK:   %v0.steady.top = load i32, ptr %a, align 4
; CHECK:   %outer.jnzd.tc = sub i32 %N, 1
; CHECK:   %outer.ctr.init = call i32 @llvm.start.loop.iterations.i32(i32 %outer.jnzd.tc)
; CHECK:   br label %steady.stage1.top

; Steady-state header: GEPs are lifted here (no epilogue uses)
; CHECK: steady.stage1.top:
; CHECK:   %a.ptr.steady = phi ptr
; CHECK:   %c.ptr.steady = phi ptr
; CHECK:   %v0.steady.phi = phi i32 [ %v0.steady.top, %stage0.top ], [ %v0.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; Hardware loop counter PHI (replaces software %iv)
; CHECK:   %outer.ctr = phi i32
; GEPs lifted from epilogue (no epilogue uses - stores use c.ptr, not c.ptr.next)
; CHECK:   %a.ptr.next.steady = getelementptr inbounds i8, ptr %a.ptr.steady, i64 128
; CHECK:   %c.ptr.next.steady = getelementptr inbounds i8, ptr %c.ptr.steady, i64 128
; CHECK:   br label %steady.stage1.inner.inner.header

; Steady-state bottom: GEPs are NOT here (lifted), hardware loop counter replaces software iv
; CHECK: steady.stage1.bottom.and.stage0.top:
; CHECK:   store i32
; GEPs are NOT here (they were lifted)
; CHECK-NOT: getelementptr{{.*}}128
; Hardware loop counter update (replaces software %iv.next and icmp eq)
; CHECK:   %v0.steady.bottom = load i32, ptr %a.ptr.next.steady, align 4
; CHECK:   %outer.ctr.next = call i32 @llvm.loop.decrement.reg.i32
; CHECK:   %outer.loop.cond = icmp ne i32 %outer.ctr.next, 0

define void @ptr_lifting_basic(ptr noalias %a, ptr noalias %c, i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %v0 = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %v0
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  ; Pointer updates - lifted to prologue (no epilogue uses)
  %a.ptr.next = getelementptr inbounds i8, ptr %a.ptr, i64 128
  %c.ptr.next = getelementptr inbounds i8, ptr %c.ptr, i64 128
  ; Loop counter update - NOT lifted (used by icmp below)
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

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
; Test 2: Inner loop dependency prevents lifting
;
; When a pointer update chain depends on values produced by the inner loop,
; it cannot be lifted to the prologue. The lifting should be skipped for
; such chains while other independent chains can still be lifted.
; ============================================================================

; CHECK-LABEL: define void @ptr_lifting_inner_dep

; Stage-0 top: load only, plus hardware loop setup
; CHECK: stage0.top:
; CHECK:   %v0.steady.top = load i32, ptr %a, align 4
; CHECK:   %outer.jnzd.tc = sub i32 %N, 1
; CHECK:   %outer.ctr.init = call i32 @llvm.start.loop.iterations.i32(i32 %outer.jnzd.tc)
; CHECK:   br label %steady.stage1.top

; Steady-state header: c.ptr.next is lifted (no inner loop dependency, no epilogue uses)
; CHECK: steady.stage1.top:
; The a.ptr update has inner loop dependency, so NOT lifted
; CHECK:   %a.ptr.steady = phi ptr [ %a.ptr.next.steady, %steady.stage1.bottom.and.stage0.top ], [ %a, %stage0.top ]
; CHECK:   %c.ptr.next.steady = getelementptr inbounds i8, ptr %c.ptr.steady, i64 64
; CHECK:   br label %steady.stage1.inner.inner.header

; Steady-state bottom: a.ptr.next stays here (inner loop dependency)
; CHECK: steady.stage1.bottom.and.stage0.top:
; An LCSSA phi may be inserted for acc.next before the sext
; CHECK:   %offset.steady = sext i32 %{{[a-z0-9.]+}} to i64
; CHECK:   %a.ptr.next.steady = getelementptr inbounds i8, ptr %a.ptr.steady, i64 %offset.steady

define void @ptr_lifting_inner_dep(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                                    i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %b.ptr = phi ptr [ %b, %entry ], [ %b.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %v0 = load i32, ptr %a.ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %v0
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !5

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  ; This pointer update depends on inner loop result - CANNOT be lifted
  %offset = sext i32 %acc.next to i64
  %a.ptr.next = getelementptr inbounds i8, ptr %a.ptr, i64 %offset
  ; This pointer update has no inner loop dependency - CAN be lifted
  %b.ptr.next = getelementptr inbounds i8, ptr %b.ptr, i64 64
  %c.ptr.next = getelementptr inbounds i8, ptr %c.ptr, i64 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !4

exit:
  ret void
}

!4 = distinct !{!4, !6, !7}
!5 = distinct !{!5, !6}
!6 = !{!"llvm.loop.mustprogress"}
!7 = !{!"llvm.loop.itercount.range", i32 2}
