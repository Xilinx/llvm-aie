;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc < %s -mtriple=aie | FileCheck %s

; Should only insert 4 lanes
define <8 x float>  @foo(float %a, float %b, float %c, float %d) nounwind {
; CHECK-LABEL: foo:
; CHECK:       // %bb.0:
; Should be CHECK-NEXT, but currently this goes through BUILD_VECTOR
; CHECK:        vshl0.32        wr0, r9
; CHECK:        vshl0.32        wr0, r8
; CHECK:        vshl0.32        wr0, r7
; CHECK:        vshl0.32        wr0, r6
; CHECK:        ret lr
  %1 = insertelement <8 x float> undef, float %a, i32 0
  %2 = insertelement <8 x float> %1, float %b, i32 1
  %3 = insertelement <8 x float> %2, float %c, i32 2
  %4 = insertelement <8 x float> %3, float %d, i32 3
  ret <8 x float> %4
}

define <8 x i32> @build_v8i32(<8 x i32> %a) #1 {
; CHECK-LABEL: build_v8i32:
; CHECK:       // %bb.0:
; CHECK-NEXT:  mov.u20 r12, #8
; CHECK-NEXT:  vshl0.32        wr0, r12
; CHECK-NEXT:  mov.u20 r12, #7
; CHECK-NEXT:  vshl0.32        wr0, r12
; CHECK-NEXT:  mov.u20 r12, #6
; CHECK-NEXT:  vshl0.32        wr0, r12
; CHECK-NEXT:  mov.u20 r12, #5
; CHECK-NEXT:  vshl0.32        wr0, r12
; CHECK-NEXT:  mov.u20 r12, #4
; CHECK-NEXT:  vshl0.32        wr0, r12
; CHECK-NEXT:  mov.u20 r12, #3
; CHECK-NEXT:  vshl0.32        wr0, r12
; CHECK-NEXT:  mov.u20 r12, #2
; CHECK-NEXT:  vshl0.32        wr0, r12
; CHECK-NEXT:  mov.u20 r12, #1
; CHECK-NEXT:  vshl0.32        wr0, r12
; CHECK-NEXT:  ret lr
  ret <8 x i32> <i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8>
}

define <8 x i1> @build_v8i1() #1 {
; CHECK-LABEL: build_v8i1:
; CHECK:       // %bb.0:
; CHECK-NEXT:  mov.u20 r0, #141
; CHECK-NEXT:  ret lr
  ret <8 x i1> <i1 1, i1 0, i1 1, i1 1, i1 0, i1 0, i1 0, i1 1>
}

define <8 x i1> @build_v8i1_val(i1 %a) #1 {
; CHECK-LABEL: build_v8i1_val:
; CHECK:       // %bb.0:
; CHECK-NEXT:  mov.u20 r0, #141
; CHECK-NEXT:  mov.u20 r12, #1
; CHECK-NEXT:  mov r0[r12], r6[0]
; CHECK-NEXT:  ret lr
  %1 = insertelement <8 x i1> <i1 1, i1 0, i1 1, i1 1, i1 0, i1 0, i1 0, i1 1>, i1 %a, i32 1
  ret <8 x i1> %1
}
