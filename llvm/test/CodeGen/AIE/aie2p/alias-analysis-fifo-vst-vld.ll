;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
; RUN: opt -mtriple=aie2p -print-all-alias-modref-info -disable-output -passes=aa-eval < %s 2>&1  | FileCheck %s



; Coverage: verify AA for every single fifo load/store intrinsic, under 
; the assumption that load and store operate on different (noalias) pointers

; CHECK-LABEL: coverageTestConcurrentLoad
; CHECK: NoAlias:      i32* %L.0, i32* %S.0

; Function Attrs: mustprogress noinline
define weak_odr dso_local void @coverageTestConcurrentLoad(ptr noalias %in, ptr noalias %out) local_unnamed_addr #0  {
entry:
  br label %bb.exit

bb.exit:                                         ; preds = %entry
  %0 = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.ld.fill(ptr %in, <32 x i32> zeroinitializer, i32 0) 
  %1 = extractvalue { ptr, <32 x i32>, i32 } %0, 0
  %2 = call { <64 x i8>, ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.ld.pop.unaligned(ptr %1, <32 x i32> zeroinitializer, i32 0) 
  %3 = extractvalue { <64 x i8>, ptr, <32 x i32>, i32 } %2, 1
  %4 = call { <64 x i8>, ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.ld.pop.1d.unaligned(ptr %3, <32 x i32> zeroinitializer, i32 0, i20 0)
  %5 = extractvalue { <64 x i8>, ptr, <32 x i32>, i32 } %4, 1
  %6 = call { <64 x i8>, ptr, <32 x i32>, i32, i20 } @llvm.aie2p.fifo.ld.pop.2d.unaligned(ptr %5, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0)
  %7 = extractvalue { <64 x i8>, ptr, <32 x i32>, i32, i20 } %6, 1
  %8 = call { <64 x i8>, ptr, <32 x i32>, i32, i20, i20 } @llvm.aie2p.fifo.ld.pop.3d.unaligned(ptr %7, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0)
  %9 = extractvalue { <64 x i8>, ptr, <32 x i32>, i32, i20, i20 } %8, 1
  %10 = call { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.576.bfp16(ptr %9, <32 x i32> zeroinitializer, i32 0)
  %11 = extractvalue { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } %10, 0
  %12 = call { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.576.1d.bfp16(ptr %11, <32 x i32> zeroinitializer, i32 0, i20 0)
  %13 = extractvalue { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } %12, 0
  %14 = call { ptr, <32 x i32>, i32, i20, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.576.2d.bfp16(ptr %13, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0)
  %15 = extractvalue { ptr, <32 x i32>, i32, i20, <64 x i8>, <8 x i8> } %14, 0
  %16 = call { ptr, <32 x i32>, i32, i20, i20, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.576.3d.bfp16(ptr %15, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0)
  %17 = extractvalue { ptr, <32 x i32>, i32, i20, i20, <64 x i8>, <8 x i8> } %16, 0
  %18 = call { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.544.bfp16(ptr %17, <32 x i32> zeroinitializer, i32 0)
  %19 = extractvalue { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } %18, 0
  %20 = call { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.544.1d.bfp16(ptr %19, <32 x i32> zeroinitializer, i32 0, i20 0)
  %21 = extractvalue { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } %20, 0
  %22 = call { ptr, <32 x i32>, i32, i20, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.544.2d.bfp16(ptr %21, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0)
  %23 = extractvalue { ptr, <32 x i32>, i32, i20, <64 x i8>, <8 x i8> } %22, 0
  %24 = call { ptr, <32 x i32>, i32, i20, i20, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.544.3d.bfp16(ptr %23, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0)
  %25 = extractvalue { ptr, <32 x i32>, i32, i20, i20, <64 x i8>, <8 x i8> } %24, 0


  
  %26 = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.push.512.bfp16(ptr %out, <16 x i32> zeroinitializer, <32 x i32> zeroinitializer, i32 0)
  %27 = extractvalue { ptr, <32 x i32>, i32 } %26, 0
  %28 = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.flush(ptr %27, <32 x i32> zeroinitializer, i32 0)
  %29 = extractvalue { ptr, <32 x i32>, i32 } %28, 0
  %30 = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.flush.1d(ptr %29, <32 x i32> zeroinitializer, i32 0, i20 0)
  %31 = extractvalue { ptr, <32 x i32>, i32 } %30, 0
  %32 = call { ptr, <32 x i32>, i32, i20 } @llvm.aie2p.fifo.st.flush.2d.conv(ptr %31, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0)
  %33 = extractvalue { ptr, <32 x i32>, i32, i20 } %32, 0
  %34 = call { ptr, <32 x i32>, i32, i20, i20 } @llvm.aie2p.fifo.st.flush.3d(ptr %33, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0)
  %35 = extractvalue { ptr, <32 x i32>, i32, i20, i20 } %34, 0
  %36 = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.flush.conv(ptr %35, <32 x i32> zeroinitializer, i32 0)
  %37 = extractvalue { ptr, <32 x i32>, i32 } %36, 0
  %38 = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.flush.1d.conv(ptr %37, <32 x i32> zeroinitializer, i32 0, i20 0)
  %39 = extractvalue { ptr, <32 x i32>, i32 } %38, 0
  %40 = call { ptr, <32 x i32>, i32, i20 } @llvm.aie2p.fifo.st.flush.2d(ptr %39, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0)
  %41 = extractvalue { ptr, <32 x i32>, i32, i20 } %40, 0
  %42 = call { ptr, <32 x i32>, i32, i20, i20 } @llvm.aie2p.fifo.st.flush.3d.conv(ptr %41, <32 x i32> zeroinitializer, i32 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0)
  %43 = extractvalue { ptr, <32 x i32>, i32, i20, i20 } %42, 0
  %44 = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.push.576.bfp16(ptr %43, <64 x i8> zeroinitializer, <8 x i8> zeroinitializer, <32 x i32> zeroinitializer, i32 0)
  %45 = extractvalue { ptr, <32 x i32>, i32 } %44, 0
  %46 = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.push.544.bfp16(ptr %45, <64 x i8> zeroinitializer, <8 x i8> zeroinitializer, <32 x i32> zeroinitializer, i32 0)
  %47 = extractvalue { ptr, <32 x i32>, i32 } %46, 0
  
  %L.0 = getelementptr inbounds i32, i32* %25, i32 8
  %R.0 = load i32, ptr %L.0

  %S.0 = getelementptr inbounds i32, i32* %47, i32 8
  store i32 zeroinitializer, ptr %S.0
 
  ret void
}

; Function Attrs: nounwind memory(none)
declare <32 x float> @llvm.aie2p.v32bf16.to.v32accfloat(<32 x bfloat>) #2

; Function Attrs: nounwind memory(inaccessiblemem: read)
declare { <64 x i8>, <8 x i8> } @llvm.aie2p.v64accfloat.to.v64bfp16ebs8(<64 x i8>, <8 x i8>, <64 x float>) #4


; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.push.512.bfp16(ptr, <16 x i32>, <32 x i32>, i32) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.flush(ptr, <32 x i32>, i32) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.flush.1d(ptr, <32 x i32>, i32, i20) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32, i20 } @llvm.aie2p.fifo.st.flush.2d.conv(ptr, <32 x i32>, i32, i20, i20, i20, i20) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32, i20, i20 } @llvm.aie2p.fifo.st.flush.3d(ptr, <32 x i32>, i32, i20, i20, i20, i20, i20, i20, i20) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.flush.conv(ptr, <32 x i32>, i32) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.flush.1d.conv(ptr, <32 x i32>, i32, i20) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32, i20 } @llvm.aie2p.fifo.st.flush.2d(ptr, <32 x i32>, i32, i20, i20, i20, i20) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32, i20, i20 } @llvm.aie2p.fifo.st.flush.3d.conv(ptr, <32 x i32>, i32, i20, i20, i20, i20, i20, i20, i20) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.push.576.bfp16(ptr, <64 x i8>, <8 x i8>, <32 x i32>, i32) #6

; Function Attrs: nounwind memory(argmem: write)
declare { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.st.push.544.bfp16(ptr, <64 x i8>, <8 x i8>, <32 x i32>, i32) #6

; Function Attrs: nofree nounwind memory(argmem: read)
declare { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.ld.fill(ptr, <32 x i32>, i32) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { <64 x i8>, ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.ld.pop.unaligned(ptr, <32 x i32>, i32) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { <64 x i8>, ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.ld.pop.1d.unaligned(ptr, <32 x i32>, i32, i20) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { <64 x i8>, ptr, <32 x i32>, i32, i20 } @llvm.aie2p.fifo.ld.pop.2d.unaligned(ptr, <32 x i32>, i32, i20, i20, i20, i20) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { <64 x i8>, ptr, <32 x i32>, i32, i20, i20 } @llvm.aie2p.fifo.ld.pop.3d.unaligned(ptr, <32 x i32>, i32, i20, i20, i20, i20, i20, i20, i20) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.576.bfp16(ptr, <32 x i32>, i32) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.576.1d.bfp16(ptr, <32 x i32>, i32, i20) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { ptr, <32 x i32>, i32, i20, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.576.2d.bfp16(ptr, <32 x i32>, i32, i20, i20, i20, i20) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { ptr, <32 x i32>, i32, i20, i20, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.576.3d.bfp16(ptr, <32 x i32>, i32, i20, i20, i20, i20, i20, i20, i20) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.544.bfp16(ptr, <32 x i32>, i32) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { ptr, <32 x i32>, i32, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.544.1d.bfp16(ptr, <32 x i32>, i32, i20) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { ptr, <32 x i32>, i32, i20, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.544.2d.bfp16(ptr, <32 x i32>, i32, i20, i20, i20, i20) #7

; Function Attrs: nofree nounwind memory(argmem: read)
declare { ptr, <32 x i32>, i32, i20, i20, <64 x i8>, <8 x i8> } @llvm.aie2p.fifo.ld.pop.544.3d.bfp16(ptr, <32 x i32>, i32, i20, i20, i20, i20, i20, i20, i20) #7

attributes #0 = { mustprogress noinline "no-jump-tables"="true" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }
attributes #1 = { nounwind memory(argmem: read) }
attributes #2 = { nounwind memory(none) }
attributes #3 = { nounwind memory(argmem: write) }
attributes #4 = { nounwind memory(inaccessiblemem: read) }

!llvm.linker.options = !{}
!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 19.0.0git (/scratch/llvm-aie/clang 5c68f0e173ec03a2b4f908018663ca52923e52fb)"}
