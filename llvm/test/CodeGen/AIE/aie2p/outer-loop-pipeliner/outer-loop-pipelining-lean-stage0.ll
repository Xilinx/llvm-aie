; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s --check-prefix=DEFAULT
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pipelining-lean-stage0 \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s --check-prefix=LEAN
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pipelining-lean-stage0 \
; RUN:     -aie-outer-loop-pipelining-speculative=false \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s --check-prefix=LEAN-NOSPEC

; The shuffle feeds the 2048-bit split point. Default anchored collection puts
; it in stage 0, while lean stage-0 collection keeps it in stage 1 because it
; is neither a load-address chain nor a target-selected intrinsic.

declare <32 x i64> @llvm.aie2p.I512.I512.ACC2048.mul.conf(
    <16 x i32>, <32 x i16>, i32) #0
declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

define void @lean_stage0_selection(ptr %a, ptr %b, ptr %c, i32 %n, i32 %m,
                                   i32 %conf) {
entry:
  %has.work = icmp ugt i32 %n, 1
  br i1 %has.work, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %b.ptr = phi ptr [ %b, %entry ], [ %b.ptr.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %a.loaded = load <16 x i32>, ptr %a.ptr, align 64
  %b.loaded = load <32 x i16>, ptr %b.ptr, align 64
  %a.shuffled = shufflevector <16 x i32> %a.loaded, <16 x i32> poison,
                                 <16 x i32> <i32 0, i32 1, i32 2, i32 3,
                                             i32 4, i32 5, i32 6, i32 7,
                                             i32 8, i32 9, i32 10, i32 11,
                                             i32 12, i32 13, i32 14, i32 15>
  %acc = call <32 x i64> @llvm.aie2p.I512.I512.ACC2048.mul.conf(
      <16 x i32> %a.shuffled, <32 x i16> %b.loaded, i32 %conf)
  call void @llvm.set.loop.iterations.i32(i32 %m)
  br label %inner.header

inner.header:
  %result = phi i32 [ 0, %outer.header ], [ %result.next, %inner.header ]
  %lane = extractelement <32 x i64> %acc, i32 0
  %lane.trunc = trunc i64 %lane to i32
  %result.next = add i32 %result, %lane.trunc
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %result.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds <16 x i32>, ptr %a.ptr, i32 1
  %b.ptr.next = getelementptr inbounds <32 x i16>, ptr %b.ptr, i32 1
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add nuw i32 %i, 1
  %outer.cond = icmp eq i32 %i.next, %n
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

; DEFAULT-LABEL: stage0.top:
; DEFAULT:       load <16 x i32>
; DEFAULT:       load <32 x i16>
; DEFAULT:       shufflevector <16 x i32>
; DEFAULT:       br label %steady.stage1.top
; DEFAULT-LABEL: steady.stage1.top:
; DEFAULT:       @llvm.aie2p.I512.I512.ACC2048.mul.conf

; LEAN-LABEL: stage0.top:
; LEAN:       load <16 x i32>
; LEAN:       load <32 x i16>
; LEAN-NOT:   shufflevector
; LEAN:       br label %steady.stage1.top
; LEAN-LABEL: steady.stage1.top:
; LEAN:       shufflevector <16 x i32>
; LEAN:       @llvm.aie2p.I512.I512.ACC2048.mul.conf
; LEAN-NOT:   lastiter.stage1.top

; An explicit speculative setting overrides lean mode's speculative default.
; LEAN-NOSPEC: lastiter.stage1.top:

attributes #0 = { nocallback nofree nosync nounwind willreturn memory(none) }

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
