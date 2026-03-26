; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2ps -stop-before=virtregrewriter %s -o - | FileCheck %s

; This test verifies that rematerialization validation correctly handles
; register class compatibility. Without the fix, this would crash during
; register allocation when attempting to rematerialize an instruction with
; incompatible register classes.
; CHECK: name: remat_cross_class_test

define void @remat_cross_class_test() {
entry:
  br label %for.body.i

for.body.i:                                       ; preds = %for.cond.cleanup51.i, %entry
  %dims_in_L1.sroa.12.0308.i = phi i32 [ 0, %entry ], [ %11, %for.cond.cleanup51.i ]
  %dims_out_L1.sroa.15.0307.i = phi i32 [ 0, %entry ], [ %8, %for.cond.cleanup51.i ]
  %dims_in_L2_01.sroa.12.0303.i = phi i32 [ 0, %entry ], [ %22, %for.cond.cleanup51.i ]
  %dims_out_L2_23.sroa.15.0296.i = phi i32 [ 0, %entry ], [ %26, %for.cond.cleanup51.i ]
  br label %for.cond54.preheader.i

for.cond54.preheader.i:                           ; preds = %for.cond54.preheader.i, %for.body.i
  %dims_in_L1.sroa.12.1251.i = phi i32 [ %dims_in_L1.sroa.12.0308.i, %for.body.i ], [ %11, %for.cond54.preheader.i ]
  %dims_out_L1.sroa.15.1250.i = phi i32 [ %dims_out_L1.sroa.15.0307.i, %for.body.i ], [ %8, %for.cond54.preheader.i ]
  %0 = trunc i32 %dims_out_L1.sroa.15.1250.i to i20
  %1 = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr null, i20 0, i20 0, i20 0, i20 0, i20 0, i20 1, i20 %0)
  %2 = extractvalue { ptr, i20, i20 } %1, 1
  %3 = trunc i32 %dims_in_L1.sroa.12.1251.i to i20
  %4 = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr null, i20 0, i20 0, i20 0, i20 1, i20 %3, i20 0, i20 0)
  %5 = extractvalue { ptr, i20, i20 } %4, 1
  %6 = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr null, i20 0, i20 0, i20 0, i20 0, i20 %2, i20 1, i20 0)
  %7 = extractvalue { ptr, i20, i20 } %6, 2
  %8 = zext i20 %7 to i32
  %9 = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr null, i20 0, i20 0, i20 0, i20 1, i20 %5, i20 0, i20 0)
  %10 = extractvalue { ptr, i20, i20 } %9, 1
  %11 = zext i20 %10 to i32
  %12 = call i1 @llvm.loop.decrement.i32(i32 0)
  br i1 %12, label %for.cond54.preheader.i, label %for.cond.cleanup51.i

for.cond.cleanup51.i:                             ; preds = %for.cond54.preheader.i
  %13 = trunc i32 %dims_in_L2_01.sroa.12.0303.i to i20
  %14 = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr null, i20 0, i20 0, i20 0, i20 0, i20 %13, i20 0, i20 1)
  %15 = extractvalue { ptr, i20, i20 } %14, 1
  %16 = extractvalue { ptr, i20, i20 } %14, 2
  %17 = trunc i32 %dims_out_L2_23.sroa.15.0296.i to i20
  %18 = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr null, i20 0, i20 0, i20 0, i20 0, i20 0, i20 1, i20 %17)
  %19 = extractvalue { ptr, i20, i20 } %18, 1
  %20 = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr null, i20 0, i20 0, i20 0, i20 0, i20 0, i20 0, i20 %16)
  %21 = extractvalue { ptr, i20, i20 } %20, 1
  %22 = zext i20 %21 to i32
  %23 = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr null, i20 0, i20 0, i20 0, i20 0, i20 %19, i20 0, i20 0)
  %24 = extractvalue { ptr, i20, i20 } %18, 1
  %25 = extractvalue { ptr, i20, i20 } %23, 2
  %26 = zext i20 %25 to i32
  br label %for.body.i

; uselistorder directives
  uselistorder i32 %8, { 1, 0 }
  uselistorder i32 %11, { 1, 0 }
}

declare { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr, i20, i20, i20, i20, i20, i20, i20)

; Function Attrs: nocallback noduplicate nofree nosync nounwind willreturn
declare i1 @llvm.loop.decrement.i32(i32) #0

; uselistorder directives
uselistorder ptr @llvm.aie2ps.add.3d, { 7, 6, 5, 4, 3, 2, 1, 0 }

attributes #0 = { nocallback noduplicate nofree nosync nounwind willreturn }
