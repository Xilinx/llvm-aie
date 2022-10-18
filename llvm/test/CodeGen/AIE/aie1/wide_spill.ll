;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie < %s | FileCheck %s

; These tests check that we can spill vector registers in a reasonable way.
; These spills are awkward, since they require multiple small spills to complete.
; Calling this function can overwrite any vector register, so
; induces a spill of the vector register to get to the return value.
declare i32 @external_func(i32) nounwind

define i64 @test_i64(i32 %a, i64 %d) nounwind {
; CHECK-LABEL: test_i64:
; CHECK:         st.spil r10, [sp, #-28]
; CHECK-NEXT:    st.spil r11, [sp, #-32]
; CHECK:         lda.spil r11, [sp, #-32]
; CHECK-NEXT:    lda.spil r10, [sp, #-28]
  %1 = call i32 @external_func(i32 %a)
  ret i64 %d
}

define <4 x i32> @test_128vec(i32 %a, <4 x i32> %d) nounwind {
; CHECK-LABEL: test_128vec:
; CHECK:         vst.spil vl0, [sp, #-16]
; CHECK:         vlda.spil vl0, [sp, #-16]
  %1 = call i32 @external_func(i32 %a)
  ret <4 x i32> %d
}

define <8 x i32> @test_256vec(i32 %a, <8 x i32> %d) nounwind {
; CHECK-LABEL: test_256vec:
; CHECK:         vst.spil wr0, [sp, #-32]
; CHECK:         vlda.spil wr0, [sp, #-32]
  %1 = call i32 @external_func(i32 %a)
  ret <8 x i32> %d
}

define <16 x i32> @test_512vec(i32 %a, <16 x i32> %d) nounwind {
; CHECK-LABEL: test_512vec:
; CHECK:         vst.spil wr0, [sp, #-64]
; CHECK-NEXT:    vst.spil wr1, [sp, #-32]
; CHECK:         vlda.spil wr0, [sp, #-64]
; CHECK-NEXT:    vlda.spil wr1, [sp, #-32]
  %1 = call i32 @external_func(i32 %a)
  ret <16 x i32> %d
}

define <32 x i32> @test_1024vec(i32 %a, <32 x i32> %d) nounwind {
; CHECK-LABEL: test_1024vec:
; CHECK:         vst.spil wr0, [sp, #-128]
; CHECK-NEXT:    vst.spil wr1, [sp, #-96]
; CHECK-NEXT:    vst.spil wr2, [sp, #-64]
; CHECK-NEXT:    vst.spil wr3, [sp, #-32]
; CHECK:         vlda.spil wr0, [sp, #-128]
; CHECK-NEXT:    vlda.spil wr1, [sp, #-96]
; CHECK-NEXT:    vlda.spil wr2, [sp, #-64]
; CHECK-NEXT:    vlda.spil wr3, [sp, #-32]
  %1 = call i32 @external_func(i32 %a)
  ret <32 x i32> %d
}
