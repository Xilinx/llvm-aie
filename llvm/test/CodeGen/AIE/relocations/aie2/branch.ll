;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -march=aie2 -O0 %s --filetype=obj -o %t.o
; RUN: llvm-objdump -dr --triple=aie2 %t.o | FileCheck --check-prefix=CHECK %s

; CHECK-LABEL: foo
; CHECK: jnz {{.*}}, #0
; CHECK-NEXT: R_AIE_1 .LBB0_2
; CHECK: j #0
; CHECK-NEXT: R_AIE_1 .LBB0_1
; CHECK: j #0
; CHECK-NEXT: R_AIE_1 .LBB0_2

define void @foo(i32 %a, i32 %b, i1 %c) {
  %tst1 = icmp eq i32 %a, %b
  br i1 %tst1, label %end, label %test2

test2:
  br label %end

end:
  ret void
}
