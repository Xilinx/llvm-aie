; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 -mtriple=aie2p %s -o /dev/null --debug-only=pipeliner 2>&1 \
; RUN:   | FileCheck %s
; REQUIRES: asserts

; The loop body is a single chain whose last node only feeds ExitSU through an
; artificial edge, so the lone node set has no node without successors.
; FIXME: the bottom-up order is never seeded, the node order comes out empty
; and no II is ever tried.

; CHECK:       SU(9): {{.*}}VSRS
; CHECK:       Successors:
; CHECK-NEXT:    ExitSU: Ord {{.*}} Artificial
; CHECK:       NodeSet size 10
; CHECK-NEXT:    Bottom up (all) {{$}}
; CHECK-NEXT:  Done with Nodeset
; CHECK-NEXT:  Node order: {{$}}
; CHECK-NOT:   Try to schedule with

define void @single_chain() {
entry:
  br label %for.body

for.cond.cleanup:
  ret void

for.body:
  %j = phi i32 [ 0, %entry ], [ %inc, %for.body ]
  %0 = load <32 x i8>, ptr addrspace(5) null, align 32
  %1 = tail call <32 x i16> @llvm.aie2p.unpack.I512.I16.I8(<32 x i8> %0, i32 0)
  %2 = tail call { <32 x i16>, i32 } @llvm.aie2p.vmin.ge16(<32 x i16> %1, <32 x i16> splat (i16 1), i32 0)
  %3 = extractvalue { <32 x i16>, i32 } %2, 0
  %4 = tail call { <32 x i16>, i32 } @llvm.aie2p.vmax.lt16(<32 x i16> %3, <32 x i16> zeroinitializer, i32 0)
  %5 = extractvalue { <32 x i16>, i32 } %4, 0
  %6 = bitcast <32 x i16> %5 to <16 x i32>
  %wide = shufflevector <16 x i32> %6, <16 x i32> %6, <32 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15, i32 16, i32 17, i32 18, i32 19, i32 20, i32 21, i32 22, i32 23, i32 24, i32 25, i32 26, i32 27, i32 28, i32 29, i32 30, i32 31>
  %7 = tail call <32 x i64> @llvm.aie2p.I1024.I1024.ACC2048.mac.conf(<32 x i32> %wide, <64 x i16> zeroinitializer, <32 x i64> zeroinitializer, i32 0)
  %8 = bitcast <32 x i32> %wide to <64 x i16>
  %9 = tail call <32 x i64> @llvm.aie2p.I1024.I1024.ACC2048.msc.conf(<32 x i32> zeroinitializer, <64 x i16> %8, <32 x i64> %7, i32 0)
  %lo = shufflevector <32 x i64> %9, <32 x i64> zeroinitializer, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15>
  %10 = bitcast <16 x i64> %lo to <32 x i32>
  %11 = tail call <32 x i8> @llvm.aie2p.I256.v32.acc32.srs(<32 x i32> %10, i32 0, i32 0)
  %inc = add i32 %j, 1
  %exitcond.not = icmp eq i32 %inc, 0
  br i1 %exitcond.not, label %for.cond.cleanup, label %for.body, !llvm.loop !0
}

declare <32 x i16> @llvm.aie2p.unpack.I512.I16.I8(<32 x i8>, i32)
declare { <32 x i16>, i32 } @llvm.aie2p.vmin.ge16(<32 x i16>, <32 x i16>, i32)
declare { <32 x i16>, i32 } @llvm.aie2p.vmax.lt16(<32 x i16>, <32 x i16>, i32)
declare <32 x i64> @llvm.aie2p.I1024.I1024.ACC2048.mac.conf(<32 x i32>, <64 x i16>, <32 x i64>, i32)
declare <32 x i64> @llvm.aie2p.I1024.I1024.ACC2048.msc.conf(<32 x i32>, <64 x i16>, <32 x i64>, i32)
declare <32 x i8> @llvm.aie2p.I256.v32.acc32.srs(<32 x i32>, i32, i32) memory(inaccessiblemem: read)

!0 = distinct !{!0, !1, !2}
!1 = !{!"llvm.loop.mustprogress"}
!2 = !{!"llvm.loop.itercount.range", i64 8}
