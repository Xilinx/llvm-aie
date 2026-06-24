; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pipelining-split-prologue \
; RUN:     -stop-after=aie-outer-loop-pipeliner \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test for split-prologue mode of the AIE Outer Loop Pipeliner.
;
; The prologue contains a 2048-bit-producing call (the "anchor") fed by two
; loads. The pass splits the prologue into Part 1 (the loads, which are
; pipelined) and Part 2 (the anchor + its descendants, which stay in
; outer.header and are cloned into cooldown.entry for the last iteration).
;
; Expected structure after transformation:
;
;   [outer.header.peel.pro]  <- warm-up: Part-1 loads only, no anchor call
;   [outer.header]           <- PHIs for pipelined loads + anchor call remains
;   [outer.latch]            <- Part-1 epilogue loads for next iteration
;   [cooldown.entry]         <- anchor call cloned (uses epilogue loads)
;   [inner.header.cd / cooldown.exit]  <- cloned inner loop + final store

; CHECK-LABEL: define void @split_prologue_basic

; Warm-up: Part-1 loads only -- anchor call and set.loop.iterations absent
; CHECK: steady.preheader:
; CHECK-NEXT:   %a_loaded.steady.peel = load <16 x i32>, ptr %a_ptr_init, align 64
; CHECK-NEXT:   %b_loaded.steady.peel = load <32 x i16>, ptr %b_ptr_init, align 64
; CHECK-NOT:    call {{.*}}ACC2048
; CHECK-NOT:    call void @llvm.set.loop.iterations
; CHECK:        br label %steady.header

; Outer header: pipelined PHIs for the loads, anchor call stays, set.loop.iter stays
; CHECK: steady.header:
; CHECK:   %a_loaded.steady.phi = phi <16 x i32> [ %a_loaded.steady.peel, %steady.preheader ], [ %a_loaded.steady.epi, %steady.latch ]
; CHECK:   %b_loaded.steady.phi = phi <32 x i16> [ %b_loaded.steady.peel, %steady.preheader ], [ %b_loaded.steady.epi, %steady.latch ]
; CHECK:   %acc.steady = call <32 x i64> @llvm.aie2p.I512.I512.ACC2048.mul.conf(<16 x i32> %a_loaded.steady.phi, <32 x i16> %b_loaded.steady.phi, i32 %conf)
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   br label %steady.inner.header

; Outer latch: epilogue loads for next iteration, no anchor call
; CHECK: steady.latch:
; CHECK:   store i32
; CHECK:   %a_loaded.steady.epi = load <16 x i32>
; CHECK:   %b_loaded.steady.epi = load <32 x i16>
; CHECK:   br i1 %outer.cond.steady, label %steady.header, label %cooldown.entry

; The cool-down (last iteration) blocks follow the steady loop.
; Cooldown entry: anchor cloned using epilogue loads + set.loop.iterations
; CHECK: cooldown.entry:
; CHECK:   call void @llvm.set.loop.iterations.i32(i32 %M)
; CHECK:   %acc.steady.cd = call <32 x i64> @llvm.aie2p.I512.I512.ACC2048.mul.conf(<16 x i32> %a_loaded.steady.epi, <32 x i16> %b_loaded.steady.epi, i32 %conf)
; CHECK:   br label %steady.inner.header.cd

; Cloned inner loop: uses cooldown accumulator
; CHECK: steady.inner.header.cd:
; CHECK:   %result.steady.cd = phi i32 [ 0, %cooldown.entry ], [ %result.next.steady.cd, %steady.inner.header.cd ]

; Cooldown exit: store only, no loads
; CHECK: cooldown.exit:
; CHECK:   store i32
; CHECK:   br label %exit

declare <32 x i64> @llvm.aie2p.I512.I512.ACC2048.mul.conf(<16 x i32>, <32 x i16>, i32) #0
declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

define void @split_prologue_basic(ptr noalias %a_ptr_init,
                                   ptr noalias %b_ptr_init,
                                   ptr noalias %c_ptr_init,
                                   i32 %N, i32 %M, i32 %conf) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %outer.header, label %exit

outer.header:
  %i       = phi i32  [ 0,           %entry      ], [ %i.next,     %outer.latch ]
  %a_ptr   = phi ptr  [ %a_ptr_init, %entry      ], [ %a_ptr.next, %outer.latch ]
  %b_ptr   = phi ptr  [ %b_ptr_init, %entry      ], [ %b_ptr.next, %outer.latch ]
  %c_ptr   = phi ptr  [ %c_ptr_init, %entry      ], [ %c_ptr.next, %outer.latch ]
  ; Part 1: loads (pipelined into warm-up and epilogue)
  %a_loaded = load <16 x i32>, ptr %a_ptr, align 64
  %b_loaded = load <32 x i16>, ptr %b_ptr, align 64
  ; Part 2 anchor: 2048-bit producing call — stays in outer.header, cloned to cooldown.entry
  %acc = call <32 x i64> @llvm.aie2p.I512.I512.ACC2048.mul.conf(
             <16 x i32> %a_loaded, <32 x i16> %b_loaded, i32 %conf)
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %result = phi i32 [ 0, %outer.header ], [ %result.next, %inner.header ]
  %lane = extractelement <32 x i64> %acc, i32 0
  %lane.trunc = trunc i64 %lane to i32
  %result.next = add i32 %result, %lane.trunc
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %result.next, ptr %c_ptr, align 4
  %a_ptr.next = getelementptr inbounds <16 x i32>, ptr %a_ptr, i32 1
  %b_ptr.next = getelementptr inbounds <32 x i16>, ptr %b_ptr, i32 1
  %c_ptr.next = getelementptr inbounds i32, ptr %c_ptr, i32 1
  %i.next = add i32 %i, 1
  %outer.cond = icmp slt i32 %i.next, %N
  br i1 %outer.cond, label %outer.header, label %exit, !llvm.loop !0

exit:
  ret void
}

attributes #0 = { nocallback nofree nosync nounwind willreturn memory(none) }

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
