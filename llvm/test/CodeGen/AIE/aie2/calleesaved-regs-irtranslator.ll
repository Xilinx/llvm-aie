;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 -O2  -verify-machineinstrs %s -o - 2>&1 --stop-after=irtranslator | FileCheck %s

; r16-r19 are used in the function due to the CC. They are also part of the
; callee-saved registers.
; Make sure the CC takes precedence and they are not saved.
define <2 x i32> @no_save_L(<2 x i32> %a) {
  ; CHECK-LABEL: name: no_save_L
  ; CHECK: calleeSavedRegisters: [ '$lr', '$r20', '$r21', '$r22', '$r23', '$p6', '$p7' ]
  ret <2 x i32> %a
}

; r16-r21 are used by CC, so they aren't saved by PEI even though they are
; callee-saved registers.
; Here we make sure %a is preserved accross the call to foo.
define <2 x i32> @preserve_L(<2 x i32> %a, <2 x i32> %b) {
  ; CHECK-LABEL: name: preserve_L
  ; CHECK: calleeSavedRegisters: [ '$lr', '$r22', '$r23', '$p6', '$p7' ]
  ; CHECK: JL @foo, CustomRegMask($lr,$l1,$l2,$l3,$p6,$p7,$r18,$r19,$r20,$r21,$r22,$r23)
  call void @foo(<2 x i32> %b)
  ret <2 x i32> %a
}

define i32 @preserve_R(i32 %a, <2 x i32> %b) {
  ; CHECK-LABEL: name: preserve_R
  ; CHECK: calleeSavedRegisters: [ '$lr', '$r18', '$r19', '$r20', '$r21', '$r22',
  ; CHECK-NEXT                     '$r23', '$p6', '$p7' ]
  ; CHECK: JL @foo, CustomRegMask($lr,$l1,$l2,$l3,$p6,$p7,$r18,$r19,$r20,$r21,$r22,$r23)
  call void @foo(<2 x i32> %b)
  ret i32 %a
}

define i32 @no_CC_CSR_overlap(i32 %a) {
  ; CHECK-LABEL: name: no_CC_CSR_overlap
  ; CHECK: calleeSavedRegisters: [ '$lr', '$r16', '$r17', '$r18', '$r19', '$r20',
  ; CHECK-NEXT                     '$r21', '$r22', '$r23', '$p6', '$p7' ]
  ret i32 %a
}

define void @no_CC_regs() {
  ; CHECK-LABEL: name: no_CC_regs
  ; CHECK-NOT: calleeSavedRegisters
  ret void
}

; Verify the call-preserved regmask of bar, it should not include its CC regs
; (l0, l1, l2)
define <2 x i32> @call_preserved_mask(<2 x i32> %a, <2 x i32> %b) {
  ; CHECK-LABEL: name: call_preserved_mask
  ; CHECK: calleeSavedRegisters: [ '$lr', '$r22', '$r23', '$p6', '$p7' ]
  ; CHECK: JL @bar, CustomRegMask($lr,$l3,$p6,$p7,$r22,$r23)
  call <2 x i32> @bar(<2 x i32> %a, <2 x i32> %b)
  ret <2 x i32> %a
}

declare void @foo(<2 x i32>)
declare <2 x i32> @bar(<2 x i32>, <2 x i32>)
