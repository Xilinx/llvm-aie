; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2ps -O2 -aie-enable-outer-loop-pointer-opt=false -aie-enable-outer-loop-pipelining \
; RUN:     -aie-outer-loop-pipelining-lean-stage0 \
; RUN:     -stop-after=aie-outer-loop-pipeliner -o - %s | FileCheck %s

; The top block has no plain load: the data comes from a FIFO load intrinsic
; pair. Those intrinsics seed the lean stage-0 prefetch chain themselves, so
; the fill/pop pair moves to stage 0 while the vector arithmetic feeding the
; inner loop stays in stage 1.

declare { ptr addrspace(5), <32 x i32>, i32 }
    @llvm.aie2ps.fifo.ld.fill.p5.p5(ptr addrspace(5), <32 x i32>, i32)
declare { <64 x i8>, ptr addrspace(5), <32 x i32>, i32 }
    @llvm.aie2ps.fifo.ld.pop.512.unaligned.p5.p5(ptr addrspace(5), <32 x i32>,
                                                 i32)
declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

define void @fifo_load_seeds_stage0(ptr addrspace(5) %a, ptr %c, i32 %n,
                                    i32 %m) {
entry:
  %has.work = icmp ugt i32 %n, 1
  br i1 %has.work, label %outer.header, label %exit

outer.header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %outer.latch ]
  %a.ptr = phi ptr addrspace(5) [ %a, %entry ], [ %a.ptr.next, %outer.latch ]
  %fifo = phi <32 x i32> [ zeroinitializer, %entry ], [ %fifo.next, %outer.latch ]
  %state = phi i32 [ 0, %entry ], [ %state.next, %outer.latch ]
  %c.ptr = phi ptr [ %c, %entry ], [ %c.ptr.next, %outer.latch ]
  %filled = call { ptr addrspace(5), <32 x i32>, i32 }
      @llvm.aie2ps.fifo.ld.fill.p5.p5(ptr addrspace(5) %a.ptr,
                                      <32 x i32> %fifo, i32 %state)
  %filled.ptr = extractvalue { ptr addrspace(5), <32 x i32>, i32 } %filled, 0
  %filled.fifo = extractvalue { ptr addrspace(5), <32 x i32>, i32 } %filled, 1
  %filled.state = extractvalue { ptr addrspace(5), <32 x i32>, i32 } %filled, 2
  %popped = call { <64 x i8>, ptr addrspace(5), <32 x i32>, i32 }
      @llvm.aie2ps.fifo.ld.pop.512.unaligned.p5.p5(ptr addrspace(5) %filled.ptr,
                                                   <32 x i32> %filled.fifo,
                                                   i32 %filled.state)
  %data = extractvalue { <64 x i8>, ptr addrspace(5), <32 x i32>, i32 } %popped, 0
  %popped.ptr = extractvalue { <64 x i8>, ptr addrspace(5), <32 x i32>, i32 } %popped, 1
  %popped.fifo = extractvalue { <64 x i8>, ptr addrspace(5), <32 x i32>, i32 } %popped, 2
  %popped.state = extractvalue { <64 x i8>, ptr addrspace(5), <32 x i32>, i32 } %popped, 3
  %scaled = add <64 x i8> %data, %data
  call void @llvm.set.loop.iterations.i32(i32 %m)
  br label %inner.header

inner.header:
  %result = phi i32 [ 0, %outer.header ], [ %result.next, %inner.header ]
  %lane = extractelement <64 x i8> %scaled, i32 0
  %lane.ext = zext i8 %lane to i32
  %result.next = add i32 %result, %lane.ext
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner.header, label %outer.latch, !llvm.loop !1

outer.latch:
  store i32 %result.next, ptr %c.ptr, align 4
  %a.ptr.next = getelementptr inbounds <64 x i8>, ptr addrspace(5) %popped.ptr, i32 1
  %fifo.next = add <32 x i32> %popped.fifo, zeroinitializer
  %state.next = add i32 %popped.state, 0
  %c.ptr.next = getelementptr inbounds i32, ptr %c.ptr, i32 1
  %i.next = add nuw i32 %i, 1
  %outer.cond = icmp eq i32 %i.next, %n
  br i1 %outer.cond, label %exit, label %outer.header, !llvm.loop !0

exit:
  ret void
}

; CHECK-LABEL: stage0.top:
; CHECK:       @llvm.aie2ps.fifo.ld.fill
; CHECK:       @llvm.aie2ps.fifo.ld.pop.512.unaligned
; CHECK-NOT:   add <64 x i8>
; CHECK:       br label %steady.stage1.top
; CHECK-LABEL: steady.stage1.top:
; CHECK:       add <64 x i8>

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
