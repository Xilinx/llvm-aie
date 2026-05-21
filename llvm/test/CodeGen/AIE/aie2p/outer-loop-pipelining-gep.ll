; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test for the AIE Outer Loop Pipelining pass with GEP instructions.
;
; This tests that address computation instructions (GEPs) between PHI pointers
; and loads are correctly identified as part of the data-load chain and are
; included in the warm-up and epilogue clones.
;
; Input structure:
;   %a.ptr = phi ptr [...]
;   %a.gep = getelementptr i32, ptr %a.ptr, i32 %offset   ; GEP between PHI and load
;   %v0 = load i32, ptr %a.gep
;
; Expected after transformation:
;   - Warm-up: GEP cloned with initial PHI values, load uses cloned GEP
;   - Outer header: pipelined PHIs for both GEPs and loads
;   - Epilogue: GEP cloned with next-iteration pointers, load uses cloned GEP
;
; Note: The -O2 optimization level runs LSR which transforms the inner loop,
; but the key transformation (GEPs being pipelined) is still verified.

; CHECK-LABEL: define void @nested_loop_with_gep

; Warm-up block: GEPs + loads cloned with initial values (using %a, %b)
; CHECK: outer.header.peel.pro:
; CHECK:   %a.gep.peel = getelementptr inbounds i32, ptr %a, i32 %offset
; CHECK:   %b.gep.peel = getelementptr inbounds i32, ptr %b, i32 %offset
; CHECK:   %v0.peel = load i32, ptr %a.gep.peel, align 4
; CHECK:   %v1.peel = load i32, ptr %b.gep.peel, align 4
; CHECK-NOT:  call void @llvm.set.loop.iterations
; CHECK:      br label %outer.header

; Outer header: pipelined PHIs for GEPs and loads
; CHECK: outer.header:
; CHECK-DAG:   %a.gep.phi = phi ptr [ %a.gep.peel, %outer.header.peel.pro ], [ %a.gep.epi, %outer.latch ]
; CHECK-DAG:   %b.gep.phi = phi ptr [ %b.gep.peel, %outer.header.peel.pro ], [ %b.gep.epi, %outer.latch ]
; CHECK-DAG:   %v0.phi = phi i32 [ %v0.peel, %outer.header.peel.pro ], [ %v0.epi, %outer.latch ]
; CHECK-DAG:   %v1.phi = phi i32 [ %v1.peel, %outer.header.peel.pro ], [ %v1.epi, %outer.latch ]
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %inner.header

; Outer latch: stores + GEPs + loads for NEXT iteration (uses a.ptr.next, b.ptr.next)
; CHECK: outer.latch:
; CHECK:   store i32
; CHECK:   %a.gep.epi = getelementptr inbounds i32, ptr %a.ptr.next, i32 %offset
; CHECK:   %b.gep.epi = getelementptr inbounds i32, ptr %b.ptr.next, i32 %offset
; CHECK:   %v0.epi = load i32, ptr %a.gep.epi, align 4
; CHECK:   %v1.epi = load i32, ptr %b.gep.epi, align 4
; CHECK:   br i1 %outer.cond, label %outer.header, label %cooldown.entry

; Cool-down entry: set.loop.iterations for last iteration
; CHECK: cooldown.entry:
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %inner.header.cd

; Cool-down exit: stores only (no prologue GEPs, no prologue loads)
; CHECK: cooldown.exit:
; CHECK:   store i32
; CHECK:   br label %exit

define void @nested_loop_with_gep(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                                   i32 %N, i32 %M, i32 %offset) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %b.ptr = phi ptr [ %b, %entry ], [ %b.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  ; GEP instructions between PHI pointers and loads
  %a.gep = getelementptr inbounds i32, ptr %a.ptr, i32 %offset
  %b.gep = getelementptr inbounds i32, ptr %b.ptr, i32 %offset
  ; Loads using the GEP results
  %v0 = load i32, ptr %a.gep, align 4
  %v1 = load i32, ptr %b.gep, align 4
  ; Set up hardware loop counter
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi i32 [ 0, %outer.header ], [ %acc.next, %inner.header ]
  %prod = mul i32 %v0, %v1
  %acc.next = add i32 %acc, %prod
  ; Hardware loop decrement
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
