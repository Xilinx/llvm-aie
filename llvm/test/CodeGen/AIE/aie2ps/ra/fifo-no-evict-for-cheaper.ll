; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -mtriple=aie2ps -stop-after=virtregrewriter %s -o - | FileCheck %s --check-prefix=BOOST
; RUN: llc -mtriple=aie2ps -aie-fifo-spill-weight-scale=1.0 -stop-after=virtregrewriter %s -o - | FileCheck %s --check-prefix=NOBOOST

; Verify that -aie-boost-fifo-spill-weight prevents the greedy RA from
; evicting the ePSRFLdF FIFO composite register.
;
; The ePSRFLdF composite register (plfr0 = {lf0, r24, p0}, plfr1 = {lf1, r25, p1})
; holds the full FIFO load state: a 1024-bit FIFO buffer, a 32-bit availability
; count, and a 20-bit pointer. Without spill weight inflation, the greedy RA
; may evict this very expensive register in favour of a scalar/pointer, for example.
;
; With boost (default): the FIFO is NOT spilled -- no VST_PLFR_SPILL.
; Without boost:        the FIFO IS spilled -- VST_PLFR_SPILL appears.

; BOOST-NOT: VST_PLFR_SPILL
; BOOST-NOT: VLDA_PLFR_SPILL

; NOBOOST: VST_PLFR_SPILL
; NOBOOST: VLDA_PLFR_SPILL

define void @fifo_no_evict_for_ptr() {
entry:
  br label %for.body.i42

for.body.i42:                                     ; preds = %for.cond.cleanup14.i, %entry
  %j.0132.i = phi i32 [ 1, %for.cond.cleanup14.i ], [ 0, %entry ]
  %0 = bitcast <32 x i64> zeroinitializer to <64 x float>
  br label %for.body15.i

for.cond.cleanup14.i:                             ; preds = %for.body15.i
  %inc68.i = add i32 %j.0132.i, 1
  %exitcond141.not.i = icmp eq i32 %inc68.i, 0
  br i1 %exitcond141.not.i, label %if.end21, label %for.body.i42, !llvm.loop !0

for.body15.i:                                     ; preds = %for.body15.i, %for.body.i42
  %k.0113.i = phi i32 [ 0, %for.body.i42 ], [ %inc.i44, %for.body15.i ]
  %pW.1110.i = phi ptr addrspace(13) [ null, %for.body.i42 ], [ %10, %for.body15.i ]
  %1 = tail call { ptr addrspace(5), <32 x i32>, i32, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } @llvm.aie2ps.fifo.ld.pop.BFP768.p5.p5(ptr addrspace(5) null, <32 x i32> zeroinitializer, i32 0)
  %2 = extractvalue { ptr addrspace(5), <32 x i32>, i32, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } %1, 0
  %3 = tail call { ptr addrspace(5), <32 x i32>, i32, i20, i20, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } @llvm.aie2ps.fifo.ld.pop.3d.BFP768.p5.p5(ptr addrspace(5) %2, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0)
  %4 = tail call { ptr addrspace(13), <32 x i32>, i32 } @llvm.aie2ps.fifo.ld.fill.p13.p13(ptr addrspace(13) %pW.1110.i, <32 x i32> zeroinitializer, i32 0)
  %5 = extractvalue { ptr addrspace(13), <32 x i32>, i32 } %4, 0
  %6 = extractvalue { ptr addrspace(13), <32 x i32>, i32 } %4, 1
  %7 = tail call { ptr addrspace(13), <32 x i32>, i32, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } @llvm.aie2ps.fifo.ld.pop.BFP768.p13.p13(ptr addrspace(13) null, <32 x i32> %6, i32 0)
  %8 = extractvalue { ptr addrspace(13), <32 x i32>, i32, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } %7, 1
  %9 = tail call { ptr addrspace(13), <32 x i32>, i32, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } @llvm.aie2ps.fifo.ld.pop.BFP768.p13.p13(ptr addrspace(13) null, <32 x i32> %8, i32 0)
  %10 = extractvalue { ptr addrspace(13), <32 x i32>, i32, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } %9, 0
  %11 = tail call <64 x float> @llvm.aie2ps.BFP768.BFP1536.ACC2048.bf.mac.conf(<8 x i32> zeroinitializer, <8 x i32> zeroinitializer, <2 x i32> zeroinitializer, <2 x i32> zeroinitializer, i32 0, i32 0, i32 0, i32 0, <8 x i32> zeroinitializer, <8 x i32> zeroinitializer, <8 x i32> zeroinitializer, <8 x i32> zeroinitializer, <2 x i32> zeroinitializer, <2 x i32> zeroinitializer, <2 x i32> zeroinitializer, <2 x i32> zeroinitializer, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, i32 0, <64 x float> %0, i32 1)
  %inc.i44 = add i32 %k.0113.i, 1
  %exitcond.not.i45 = icmp eq i32 %inc.i44, 0
  br i1 %exitcond.not.i45, label %for.cond.cleanup14.i, label %for.body15.i

if.end21:                                         ; preds = %for.cond.cleanup14.i
  ret void
}

declare { ptr addrspace(5), <32 x i32>, i32, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } @llvm.aie2ps.fifo.ld.pop.BFP768.p5.p5(ptr addrspace(5), <32 x i32>, i32)
declare { ptr addrspace(5), <32 x i32>, i32, i20, i20, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } @llvm.aie2ps.fifo.ld.pop.3d.BFP768.p5.p5(ptr addrspace(5), <32 x i32>, i32, i20, i20, i20, i20, i20, i20, i20)
declare { ptr addrspace(13), <32 x i32>, i32 } @llvm.aie2ps.fifo.ld.fill.p13.p13(ptr addrspace(13), <32 x i32>, i32)
declare { ptr addrspace(13), <32 x i32>, i32, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32 } @llvm.aie2ps.fifo.ld.pop.BFP768.p13.p13(ptr addrspace(13), <32 x i32>, i32)
declare <64 x float> @llvm.aie2ps.BFP768.BFP1536.ACC2048.bf.mac.conf(<8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32, <8 x i32>, <8 x i32>, <8 x i32>, <8 x i32>, <2 x i32>, <2 x i32>, <2 x i32>, <2 x i32>, i32, i32, i32, i32, i32, i32, i32, i32, <64 x float>, i32)

; uselistorder directives
uselistorder ptr @llvm.aie2ps.fifo.ld.pop.BFP768.p13.p13, { 1, 0 }

!0 = distinct !{!0, !1, !2, !3}
!1 = !{!"llvm.loop.mustprogress"}
!2 = !{!"llvm.loop.itercount.range", i64 3}
!3 = !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1}
