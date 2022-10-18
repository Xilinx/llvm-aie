;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc < %s -verify-machineinstrs -mtriple=aie | FileCheck %s
; RUN: llc %s -verify-machineinstrs -mtriple=aie --filetype=obj -o %t
; RUN: llvm-objdump --triple=aie -d %t | FileCheck --check-prefix=OBJECT %s

define i32 @extractv4i32(<4 x i32> %A) {
    ; CHECK-LABEL: extractv4i32:
    ; CHECK:  vext.32 r0, vl0[#0]
    ; OBJECT-LABEL: <extractv4i32>:
    ; OBJECT: vext.32 r0, vl0[#0]
    %r0 = extractelement <4 x i32> %A, i32 0    ; yields i32
    ret i32 %r0
}
define i32 @extractv8i32(<8 x i32> %A) {
    ; CHECK-LABEL: extractv8i32:
    ; CHECK:  vext.32_w       r0, wr0[#2]
    ; OBJECT-LABEL: <extractv8i32>:
    ; OBJECT: vext.32 r0, vl0[#2]
    %r0 = extractelement <8 x i32> %A, i32 2    ; yields i32
    ret i32 %r0
}
define i32 @extractv16i32(<16 x i32> %A) {
    ; CHECK-LABEL: extractv16i32:
    ; CHECK:  vext.32_x       r0, xa[#4]
    ; OBJECT-LABEL: <extractv16i32>:
    ; OBJECT: vext.32 r0, vh0[#0]
    %r0 = extractelement <16 x i32> %A, i32 4    ; yields i32
    ret i32 %r0
}


define i16 @extractv4i16(<8 x i16> %A) {
    ; CHECK-LABEL: extractv4i16:
    ; CHECK:  vext.16 r0, vl0[#0]
    ; OBJECT-LABEL: <extractv4i16>:
    ; OBJECT: vext.16 r0, vl0[#0]
    %r0 = extractelement <8 x i16> %A, i16 0    ; yields i16
    ret i16 %r0
}
define i16 @extractv8i16(<16 x i16> %A) {
    ; CHECK-LABEL: extractv8i16:
    ; CHECK:  vext.16_w       r0, wr0[#4]
    ; OBJECT-LABEL: <extractv8i16>:
    ; OBJECT: vext.16 r0, vl0[#4]
    %r0 = extractelement <16 x i16> %A, i16 4    ; yields i16
    ret i16 %r0
}
define i16 @extractv16i16(<32 x i16> %A) {
    ; CHECK-LABEL: extractv16i16:
    ; CHECK:  vext.16_x       r0, xa[#8]
    ; OBJECT-LABEL: <extractv16i16>:
    ; OBJECT: vext.16 r0, vh0[#0]
    %r0 = extractelement <32 x i16> %A, i16 8    ; yields i16
    ret i16 %r0
}
