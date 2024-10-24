; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie --issue-limit=1 < %s \
; RUN:   | FileCheck %s

target triple = "aie"

define i32 @test(i32 inreg %a) local_unnamed_addr {
; CHECK-LABEL: test:
; CHECK:         .p2align 4
; CHECK-NEXT:  // %bb.0: // %entry
; CHECK-NEXT:    mov r0, mc0[#2]
; CHECK-NEXT:    mov r1, mc0[#1]
; CHECK-NEXT:    add r12, r1, r0
; CHECK-NEXT:    mov r0, mc0[#3]
; CHECK-NEXT:    add r12, r12, r0
; CHECK-NEXT:    mov r0, mc0[#4]
; CHECK-NEXT:    add r0, r12, r0
; CHECK-NEXT:    ret lr
  entry:
    %0 = tail call i32 @llvm.aie.bitget.mc0(i32 1)
    %1 = tail call i32 @llvm.aie.bitget.mc0(i32 2)
    %2 = add nsw i32 %0, %1
    %3 = tail call i32 @llvm.aie.bitget.mc0(i32 3)
    %4 = add nsw i32 %2, %3
    %5 = tail call i32 @llvm.aie.bitget.mc0(i32 4)
    %6 = add nsw i32 %4, %5
    ret i32 %6
}

declare i32 @llvm.aie.bitget.mc0(i32)

define i32 @test_bitset() local_unnamed_addr {
; CHECK-LABEL: test_bitset:
; CHECK:         .p2align 4
; CHECK-NEXT:  // %bb.0: // %entry
; CHECK-NEXT:    mov.u20 r0, #11
; CHECK-NEXT:    mov md0[#2], r0[0]
; CHECK-NEXT:    padda [sp], #32
; CHECK-NEXT:    mov r12, md0
; CHECK-NEXT:    st.spil r12, [sp, #-32]
; CHECK-NEXT:    lda.spil r13, [sp, #-32]
; CHECK-NEXT:    mov.u20 r0, #10
; CHECK-NEXT:    mov md0[#1], r0[0]
; CHECK-NEXT:    mov.u20 r0, #12
; CHECK-NEXT:    mov r12, md0
; CHECK-NEXT:    mov md1[#1], r0[0]
; CHECK-NEXT:    mov.u20 r0, #13
; CHECK-NEXT:    padda [sp], #-32
; CHECK-NEXT:    add r12, r12, r13
; CHECK-NEXT:    mov r13, md1
; CHECK-NEXT:    mov md1[#2], r0[0]
; CHECK-NEXT:    add r12, r12, r13
; CHECK-NEXT:    mov r13, md1
; CHECK-NEXT:    add r0, r12, r13
; CHECK-NEXT:    ret lr
  entry:
    %0 = tail call i32 @llvm.aie.bitset.md0(i32 1, i32 10)
    %1 = tail call i32 @llvm.aie.bitset.md0(i32 2, i32 11)
    %2 = add nsw i32 %0, %1
    %3 = tail call i32 @llvm.aie.bitset.md1(i32 1, i32 12)
    %4 = add nsw i32 %2, %3
    %5 = tail call i32 @llvm.aie.bitset.md1(i32 2, i32 13)
    %6 = add nsw i32 %4, %5
    ret i32 %6
}

declare i32 @llvm.aie.bitset.md0(i32, i32)
declare i32 @llvm.aie.bitset.md1(i32, i32)
