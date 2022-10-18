;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie -O0 < %s | FileCheck %s


; Make sure that each block label is preceded by an
; alignment directive.
define void @foo(i32 %a, i1 zeroext %c) {
; CHECK-LABEL: foo:
; CHECK-NEXT: .p2align 4
; CHECK: .p2align 4
; CHECK-NEXT: .LBB0_1:
; CHECK: .p2align 4
; CHECK-NEXT: .LBB0_2:
; CHECK: .p2align 4
; CHECK-NEXT: .LBB0_3:
; CHECK: .p2align 4
; CHECK-NEXT: .LBB0_4:
; CHECK: .p2align 4
; CHECK-NEXT: .LBB0_5:

  %tst1 = icmp eq i32 %a, 1
  br i1 %tst1, label %then1, label %else1

then1:
  br label %end1

else1:
  br label %end1

end1:
  %tst2 = icmp eq i32 %a, 2
  br i1 %tst2, label %then2, label %end2

then2:
  br label %end2

end2:
  ret void
}
