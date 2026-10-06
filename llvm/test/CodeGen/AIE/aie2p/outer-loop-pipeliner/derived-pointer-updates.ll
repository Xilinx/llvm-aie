; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -aie-enable-outer-loop-pointer-opt=false \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test for outer loop pipelining with add.2d/add.3d intrinsics.
;
; The pass rotates the outer loop to overlap prologue loads with epilogue
; stores. The add.2d/add.3d intrinsics are pointer update instructions that
; compute the next iteration's pointer values.

; ============================================================================
; Test 1: add.2d intrinsic in outer loop header is preserved during pipelining.
; The add.2d uses the PHI pointer (not a load result), so it stays in the
; header block and is executed for each iteration.
; ============================================================================

; CHECK-LABEL: define void @add_2d_basic_pipelining

; Stage-0 top block: load only (add.2d is not stage-0)
; CHECK: stage0.top:
; CHECK:   %v0.steady.top = load i32, ptr %a, align 4
; CHECK:   br label %steady.stage1.top

; Steady-state header: add.2d is in the loop header
; CHECK: steady.stage1.top:
; CHECK:   %v0.steady.phi = phi i32 [ %v0.steady.top, %stage0.top ], [ %v0.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   %ptr_update.steady = call { ptr, i20 } @llvm.aie2p.add.2d
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %steady.stage1.inner.inner.header

; Steady-state bottom: store + load for NEXT iteration
; CHECK: steady.stage1.bottom.and.stage0.top:
; CHECK:   store i32
; CHECK:   %v0.steady.bottom = load i32
; CHECK:   br i1

define void @add_2d_basic_pipelining(ptr noalias %a, ptr noalias %c,
                                       i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %dim_cnt = phi i20 [ 0, %entry ], [ %dim_cnt.next, %outer.latch ]

  ; Load is stage-0
  %v0 = load i32, ptr %a.ptr, align 4

  ; add.2d uses the load's ptr (via a.ptr), which is in stage-0
  ; This should be recognized as a derived pointer update
  %ptr_update = call { ptr, i20 } @llvm.aie2p.add.2d(
    ptr %a.ptr, i20 %dim_cnt, i20 1, i20 4, i20 8)
  %a.ptr.next = extractvalue { ptr, i20 } %ptr_update, 0
  %dim_cnt.next = extractvalue { ptr, i20 } %ptr_update, 1

  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %v0
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add i32 %i, 1
  %outer.cond = icmp slt i32 %i.next, %N
  br i1 %outer.cond, label %outer.header, label %exit, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 2: GEP followed by add.2d - GEP is in the header, add.2d uses GEP result.
; Both stay in the loop header since they don't derive from stage-0 load results.
; ============================================================================

; CHECK-LABEL: define void @gep_then_add_2d_chain

; Stage-0 top block: load only
; CHECK: stage0.top:
; CHECK:   %v0.steady.top = load i32, ptr %a, align 4
; CHECK:   br label %steady.stage1.top

; Steady-state header: GEP and add.2d are in the loop header
; CHECK: steady.stage1.top:
; CHECK:   %v0.steady.phi = phi i32 [ %v0.steady.top, %stage0.top ], [ %v0.steady.bottom, %steady.stage1.bottom.and.stage0.top ]
; CHECK:   %gep.steady = getelementptr
; CHECK:   %ptr_update.steady = call { ptr, i20 } @llvm.aie2p.add.2d
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %steady.stage1.inner.inner.header

; Steady-state bottom: store + load for NEXT iteration
; CHECK: steady.stage1.bottom.and.stage0.top:
; CHECK:   store i32
; CHECK:   %v0.steady.bottom = load i32
; CHECK:   br i1

define void @gep_then_add_2d_chain(ptr noalias %a, ptr noalias %c,
                                    i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %dim_cnt = phi i20 [ 0, %entry ], [ %dim_cnt.next, %outer.latch ]

  ; Load is stage-0
  %v0 = load i32, ptr %a.ptr, align 4

  ; GEP derives from the load's ptr (a.ptr)
  %gep = getelementptr inbounds i32, ptr %a.ptr, i32 1

  ; add.2d uses the GEP result - should also be promoted
  %ptr_update = call { ptr, i20 } @llvm.aie2p.add.2d(
    ptr %gep, i20 %dim_cnt, i20 1, i20 4, i20 8)
  %a.ptr.next = extractvalue { ptr, i20 } %ptr_update, 0
  %dim_cnt.next = extractvalue { ptr, i20 } %ptr_update, 1

  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %acc.next = add i32 %acc, %v0
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !3

outer.latch:
  store i32 %acc.next, ptr %c.ptr, align 4
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add i32 %i, 1
  %outer.cond = icmp slt i32 %i.next, %N
  br i1 %outer.cond, label %outer.header, label %exit, !llvm.loop !2

exit:
  ret void
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)
declare { ptr, i20 } @llvm.aie2p.add.2d(ptr, i20, i20, i20, i20)
declare { ptr, i20, i20 } @llvm.aie2p.add.3d(ptr, i20, i20, i20, i20, i20, i20, i20)

!0 = distinct !{!0, !4, !5}
!1 = distinct !{!1, !4}
!2 = distinct !{!2, !4, !5}
!3 = distinct !{!3, !4}
!4 = !{!"llvm.loop.mustprogress"}
!5 = !{!"llvm.loop.itercount.range", i32 2}
