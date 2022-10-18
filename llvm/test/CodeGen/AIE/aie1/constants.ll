;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie -verify-machineinstrs < %s \
; RUN:     | FileCheck %s --check-prefixes=CHECK,CHECK-SDAG
; RUN: llc -mtriple=aie -global-isel -verify-machineinstrs < %s \
; RUN:     | FileCheck %s  --check-prefixes=CHECK,CHECK-GISEL

define i32 @min_s12() nounwind {
; CHECK-LABEL: min_s12:
; CHECK:         mov.s12 r0, #-2048
; CHECK-NEXT:    ret lr
  ret i32 -2048
}

define i32 @min_s12_minus_1() nounwind {
; CHECK-LABEL: min_s12_minus_1:
; CHECK:         mov.u20 r0, #1046527
; CHECK-SDAG-NEXT:    movt.s12 r0, #-1
; CHECK-GISEL-NEXT:    movt.s12 r0, #4095
; CHECK-NEXT:    ret lr
  ret i32 -2049
}

define i32 @max_s12() nounwind {
; CHECK-LABEL: max_s12:
; CHECK:         mov.u20 r0, #2047
; CHECK-NEXT:    ret lr
  ret i32 2047
}

define i32 @min_s20() nounwind {
; CHECK-LABEL: min_s20:
; CHECK:         mov.u20 r0, #524288
; CHECK-SDAG-NEXT:    movt.s12 r0, #-1
; CHECK-GISEL-NEXT:    movt.s12 r0, #4095
; CHECK-NEXT:    ret lr
  ret i32 -524288
}

define i32 @max_s20() nounwind {
; CHECK-LABEL: max_s20:
; CHECK:         mov.u20 r0, #524287
; CHECK-NEXT:    ret lr
  ret i32 524287
}

define i32 @max_u20() nounwind {
; CHECK-LABEL: max_u20:
; CHECK:         mov.u20 r0, #1048575
; CHECK-NEXT:    ret lr
  ret i32 1048575
}

define i32 @max_s32() nounwind {
; CHECK-LABEL: max_s32:
; CHECK:         mov.u20 r0, #1048575
; CHECK-NEXT:    movt.s12 r0, #2047
; CHECK-NEXT:    ret lr
  ret i32 2147483647
}

define i32 @max_u32() nounwind {
; CHECK-LABEL: max_u32:
; CHECK:         mov.s12 r0, #-1
; CHECK-NEXT:    ret lr
  ret i32 4294967295
}
