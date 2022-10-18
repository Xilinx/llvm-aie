;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 --mtriple=aie2 --filetype=obj --issue-limit=3 %s -o %t
; RUN: llvm-objdump -dr --triple=aie2 %t | FileCheck %s

; AIECC-212: We didn't register external symbols referred from vliw subinstructions
; As a result, the relocations were emitted against the dummy symbol entry
; We check both the vliw case and the stand-alone case

; CHECK-LABEL: vliw
; CHECK: [[ADDR1:[0-9a-f]*]]: 3b 10 00 18 00 00 00 08 0b 01 paddb [p1], m0; movxm p0, #0x0
; CHECK-NEXT: [[ADDR1]]:  R_AIE_44     ExternalName
; CHECK-LABEL: standalone
; CHECK: [[ADDR2:[0-9a-f]*]]: 55 00 60 00 00 00 movxm p0, #0x0
; CHECK-NEXT: [[ADDR2]]:  R_AIE_42     OtherExternalName

@ExternalName = external dso_local global [10 x i32], align 4
@OtherExternalName = external dso_local global [10 x i32], align 4

define dso_local void @vliw(ptr %p) {
entry:
  %add.ptr = getelementptr i32, ptr %p, i20 512
  tail call void @f0(ptr @ExternalName, ptr %add.ptr)
  ret void
}

define dso_local ptr @standalone() {
entry:
  ret ptr @OtherExternalName
}

declare dso_local void @f0(ptr, ptr)
