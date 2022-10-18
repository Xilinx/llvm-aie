;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie --issue-limit=1 -verify-machineinstrs < %s \
; RUN:   | FileCheck %s

define i16 @shift16_16(i16 %a, i16 %b) nounwind {
; CHECK-LABEL: shift16_16:
; CHECK:       // %bb.0:
; CHECK-NEXT:  ze.16 r12, r7
; CHECK-NEXT:  lshl r12, r6, r12
; CHECK-NEXT:  ze.16 r12, r12
; CHECK-NEXT:  mov.u20 r13, #0
; CHECK-NEXT:  sub r12, r13, r12
; CHECK-NEXT:  ze.16 r14, r6
; CHECK-NEXT:  lshl r12, r14, r12
; CHECK-NEXT:  sub r12, r13, r12
; CHECK-NEXT:  se.16 r13, r6
; CHECK-NEXT:  ashl r0, r13, r12
; CHECK-NEXT:  ret lr
  %1 = shl i16 %a, %b
  %2 = lshr i16 %a,%1
  %3 = ashr i16 %a,%2
  ret i16 %3
}

define i32 @shift32_32(i32 %a,i32 %b) nounwind {
; CHECK-LABEL: shift32_32:
; CHECK:       // %bb.0:
; CHECK-NEXT:  lshl r12, r7, r6
; CHECK-NEXT:  mov.u20 r13, #0
; CHECK-NEXT:  sub r12, r13, r12
; CHECK-NEXT:  ashl r12, r7, r12
; CHECK-NEXT:  sub r12, r13, r12
; CHECK-NEXT:  lshl r0, r7, r12
; CHECK:       ret lr
;  %j = alloca i32, align 4
; %c = load i32, i32* %j, align 4
  %1 = shl i32 %b, %a
  %2 = ashr i32 %b,%1
  %3 = lshr i32 %b,%2
  ret i32 %3
}

define i8 @shift8_16(i16 %a, i8 %b) nounwind {
; CHECK-LABEL: shift8_16:
; CHECK:       // %bb.0:
; CHECK-NEXT:  mov.u20 r12, #1
; CHECK-NEXT:  and r0, r7, r12
; CHECK:       ret lr
  %1 = shl i8 %b, 7
  %2 = ashr i8 %1, 7
  %3 = lshr i8 %2, 7
  ret i8 %3
}

define i16 @check_setcc(i16 %a) nounwind {
; CHECK-LABEL: check_setcc:
; CHECK:       // %bb.0:
; CHECK-NEXT:  se.16 r12, r6
; CHECK-NEXT:  mov.u20 r13, #2
; CHECK-NEXT:  lt r0, r12, r13
; CHECK:       ret lr
  %1 = icmp slt i16 %a, 2
  %2 = zext i1 %1 to i16
  ret i16 %2
}

define i16 @check_setcc_1(i16 %a, i16 %b) nounwind {
; CHECK-LABEL: check_setcc_1:
; CHECK:       // %bb.0:
; CHECK-NEXT:  se.16 r12, r7
; CHECK-NEXT:  se.16 r13, r6
; CHECK-NEXT:  lt r0, r13, r12
; CHECK:       ret lr
  %1 = icmp slt i16 %a, %b
  %2 = zext i1 %1 to i16
  ret i16 %2
}
