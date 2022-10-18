;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie < %s | FileCheck %s

; ModuleID = 'acdc_project/core_1_3.ll'
source_filename = "LLVMDialectModule"
target triple = "aie"

@a = external global [256 x float]

; Function Attrs: mustprogress nofree nosync nounwind willreturn
define void @_main() local_unnamed_addr #1 {
; CHECK-LABEL: _main:
; CHECK:    vfpmul wr2, r0, ya, r15, cl1, wc0, #0, cl1, #0, cl0

  %constexpr = and i64 ptrtoint (ptr @a to i64), 31
  %constexpr1 = icmp eq i64 %constexpr, 0
  tail call void @llvm.assume(i1 %constexpr1) #2
  store float 1.400000e+01, float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 3), align 4
  store float 8.000000e+00, float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 5), align 4
  store float 1.400000e+01, float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 9), align 4
  %1 = load <8 x float>, <8 x float>* bitcast ([256 x float]* @a to <8 x float>*), align 4
  %2 = fmul <8 x float> %1, %1
  store <8 x float> %2, <8 x float>* bitcast ([256 x float]* @a to <8 x float>*), align 4
  %3 = load <8 x float>, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 8) to <8 x float>*), align 4
  %4 = fmul <8 x float> %3, %3
  store <8 x float> %4, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 8) to <8 x float>*), align 4
  %5 = load <8 x float>, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 16) to <8 x float>*), align 4
  %6 = fmul <8 x float> %5, %5
  store <8 x float> %6, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 16) to <8 x float>*), align 4
  %7 = load <8 x float>, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 24) to <8 x float>*), align 4
  %8 = fmul <8 x float> %7, %7
  store <8 x float> %8, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 24) to <8 x float>*), align 4
  %9 = load <8 x float>, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 32) to <8 x float>*), align 4
  %10 = fmul <8 x float> %9, %9
  store <8 x float> %10, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 32) to <8 x float>*), align 4
  %11 = load <8 x float>, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 40) to <8 x float>*), align 4
  %12 = fmul <8 x float> %11, %11
  store <8 x float> %12, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 40) to <8 x float>*), align 4
  %13 = load <8 x float>, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 48) to <8 x float>*), align 4
  %14 = fmul <8 x float> %13, %13
  store <8 x float> %14, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 48) to <8 x float>*), align 4
  %15 = load <8 x float>, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 56) to <8 x float>*), align 4
  %16 = fmul <8 x float> %15, %15
  store <8 x float> %16, <8 x float>* bitcast (float* getelementptr inbounds ([256 x float], [256 x float]* @a, i64 0, i64 56) to <8 x float>*), align 4
  ret void
}

; Function Attrs: inaccessiblememonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.assume(i1 noundef) #2

attributes #0 = { nofree nosync nounwind }
attributes #1 = { mustprogress nofree nosync nounwind willreturn }
attributes #2 = { inaccessiblememonly mustprogress nocallback nofree nosync nounwind willreturn }
attributes #3 = { nounwind }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
