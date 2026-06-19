;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; AIE AA noalias-arg-roots + software pipelining:
; inner loop load/store through data pointers loaded from two distinct noalias
; "metadata" pointers (double dereference / io_buffer style). With
; --aie-alias-analysis-noalias-arg-roots=true the post-pipeliner finds a tight
; modulo schedule (low II); with it false, cross-iteration memory deps force a
; much higher II (serialized).
;
; RUN: llc -O2 -mtriple=aie2p --aie-alias-analysis-noalias-arg-roots=true %s \
; RUN:     -pass-remarks-output=- -pass-remarks-filter=pipeliner -o /dev/null 2>&1 \
; RUN:   | FileCheck %s --check-prefix=ON
; RUN: llc -O2 -mtriple=aie2p --aie-alias-analysis-noalias-arg-roots=false %s \
; RUN:     -pass-remarks-output=- -pass-remarks-filter=pipeliner -o /dev/null 2>&1 \
; RUN:   | FileCheck %s --check-prefix=OFF

target triple = "aie2p-none-unknown-elf"

define void @io_buffer_copy_postpipe(ptr noalias nocapture readonly %in_meta, ptr noalias %out_meta, i32 noundef %rows, i32 noundef %cols) local_unnamed_addr #0 {
entry:
  %src = load ptr, ptr %in_meta, align 4
  %dst = load ptr, ptr %out_meta, align 4
  %shr = ashr i32 %cols, 4
  %cmp34 = icmp sgt i32 %rows, 0
  br i1 %cmp34, label %for.cond2.preheader.lr.ph, label %for.cond.cleanup

for.cond2.preheader.lr.ph:
  br label %for.cond2.preheader

for.cond2.preheader:
  %lsr.iv41 = phi i32 [ %rows, %for.cond2.preheader.lr.ph ], [ %lsr.iv.next42, %for.cond.cleanup4 ]
  %it_in = phi ptr [ %src, %for.cond2.preheader.lr.ph ], [ %add.ptr.in, %for.cond.cleanup4 ]
  %it_out = phi ptr [ %dst, %for.cond2.preheader.lr.ph ], [ %add.ptr.out, %for.cond.cleanup4 ]
  call void @llvm.set.loop.iterations.i32(i32 %shr)
  br label %for.body5

for.cond.cleanup:
  ret void

for.cond.cleanup4:
  %lsr.iv.next42 = add i32 %lsr.iv41, -1
  %exitcond40.not = icmp eq i32 %lsr.iv.next42, 0
  br i1 %exitcond40.not, label %for.cond.cleanup, label %for.cond2.preheader, !llvm.loop !2

for.body5:
  %it_in.l = phi ptr [ %it_in, %for.cond2.preheader ], [ %add.ptr.in, %for.body5 ]
  %it_out.l = phi ptr [ %it_out, %for.cond2.preheader ], [ %add.ptr.out, %for.body5 ]
  %add.ptr.in = getelementptr inbounds i8, ptr %it_in.l, i20 32
  %in_elems = load <8 x i32>, ptr %it_in.l, align 64
  %shuffle = shufflevector <8 x i32> %in_elems, <8 x i32> poison, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15>
  %0 = bitcast <16 x i32> %shuffle to <32 x bfloat>
  %1 = tail call noundef <32 x float> @llvm.aie2p.I512.I512.ACC1024.bf.mul.conf(<32 x bfloat> %0, <32 x bfloat> <bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat 0xR3F80, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison, bfloat poison>, i32 828)
  %2 = shufflevector <32 x float> %1, <32 x float> poison, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15>
  %3 = tail call noundef <16 x bfloat> @llvm.aie2p.v16accfloat.to.v16bf16(<16 x float> %2)
  %add.ptr.out = getelementptr inbounds i8, ptr %it_out.l, i20 32
  store <16 x bfloat> %3, ptr %it_out.l, align 64
  %4 = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %4, label %for.body5, label %for.cond.cleanup4, !llvm.loop !5
}

declare <32 x float> @llvm.aie2p.I512.I512.ACC1024.bf.mul.conf(<32 x bfloat>, <32 x bfloat>, i32) #1
declare <16 x bfloat> @llvm.aie2p.v16accfloat.to.v16bf16(<16 x float>) #2
declare void @llvm.set.loop.iterations.i32(i32) #3
declare i1 @llvm.loop.decrement.i32(i32) #3

attributes #0 = { mustprogress nofree nounwind }
attributes #1 = { nocallback nofree nosync nounwind willreturn memory(inaccessiblemem: read) }
attributes #2 = { nounwind memory(inaccessiblemem: read) }
attributes #3 = { nocallback noduplicate nofree nosync nounwind willreturn }

!2 = distinct !{!2, !3, !4}
!3 = !{!"llvm.loop.mustprogress"}
!4 = !{!"llvm.loop.unroll.disable"}
!5 = distinct !{!5, !3, !6, !4}
!6 = !{!"llvm.loop.itercount.range", i64 4}

; ON-DAG: Function:        io_buffer_copy_postpipe
; ON-DAG: Schedule found
; ON-DAG: Pipeliner:       postpipeliner
; ON-DAG: II:              '4'
; ON-DAG: NS:              '4'

; OFF-DAG: Function:        io_buffer_copy_postpipe
; OFF-DAG: Schedule found
; OFF-DAG: Pipeliner:       postpipeliner
; OFF-DAG: II:              '14'
; OFF-DAG: NS:              '1'
