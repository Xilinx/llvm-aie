;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie --issue-limit=1 < %s | FileCheck %s
; RUN: llc -mtriple=aie --issue-limit=1 --filetype=obj < %s \
; RUN:   | llvm-objdump -dr --triple=aie - | FileCheck %s

; Specifically check the disassembly of some instructions

target triple = "aie"

define i32 @test(i32 inreg %a) local_unnamed_addr {
; CHECK-LABEL: test
; CHECK:         mov.s12 r1, #0
; CHECK-NEXT:    mov.u20 r12, #123
; CHECK-NEXT:    mov md0[#8], r1[0]
; CHECK-NEXT:    eq r0, r6, r12
; CHECK-NEXT:    mov0 r12, SS.md0[8]
; CHECK-NEXT:    mov md0[#10], r1[0]
; CHECK-NEXT:    padda [sp], #32
; CHECK-NEXT:    ite_nez r12, r6, r12, r0
; CHECK-NEXT:    mov r13, md0
; CHECK-NEXT:    st.spil r13, [sp, #-32]
; CHECK-NEXT:    PKTHD MS.md0[10], r12, #3, #0
; CHECK-NEXT:    mov.s12 r0, #1
; CHECK-NEXT:    mov md0[#10], r0[0]
; CHECK-NEXT:    mov.u20 p0, #0
; CHECK-NEXT:    mov r12, md0
; CHECK-NEXT:    mov m0, p0
; CHECK-NEXT:    st.spil r12, [sp, #-28]
; CHECK-NEXT:    mov.u20 r12, #31
; CHECK-NEXT:    CPKTHD MS.md0[10], m0, #3, #0, r12, #0
; CHECK-NEXT:    lda.spil r12, [sp, #-32]
; CHECK-NEXT:    mov.u20 r0, #1
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    mov md0, r12
; CHECK-NEXT:    movs MS.md0[10], r6, #0
; CHECK-NEXT:    lda.spil r12, [sp, #-28]
; CHECK-NEXT:    padda [sp], #-32
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    nop
; CHECK-NEXT:    mov md0, r12
; CHECK-NEXT:    movs MS.md0[10], r6, #0
; CHECK-NEXT:    ret lr
entry:
	%0 = tail call i32 @llvm.aie.get.ss(i32 0)
  %x = tail call i32 @llvm.aie.get.ss(i32 1)
  %cmp = icmp eq i32 %a, 123
  %cond = select i1 %cmp, i32 %a, i32 %0
  %1 = tail call i32 @llvm.aie.packet.header(i32 %cond, i32 3)
  tail call void @llvm.aie.put.ms(i32 0, i32 %1)
  %2 = tail call i32 @llvm.aie.ctrl.packet.header(i20 0, i32 3, i32 0, i32 31)
  tail call void @llvm.aie.put.ms(i32 1, i32 %2)
  tail call void @llvm.aie.put.ms(i32 0, i32 %a)
  tail call void @llvm.aie.put.ms(i32 1, i32 %a)
  ret i32 1
}

declare i32 @llvm.aie.get.ss(i32)
declare void @llvm.aie.put.ms(i32, i32)
declare i32 @llvm.aie.packet.header(i32, i32)
declare i32 @llvm.aie.ctrl.packet.header(i20, i32, i32, i32)
