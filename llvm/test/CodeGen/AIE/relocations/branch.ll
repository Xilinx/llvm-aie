;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -march=aie -O0 %s --filetype=obj -o %t.o
; RUN: llvm-objdump -dr --arch-name=aie %t.o | FileCheck --check-prefix=CHECK %s
; We don't actually have ld.lld, unless it's been compiled!
; UN: ld.lld %t.o -o %t.out
; UN: llvm-objdump -dr --arch-name=aie %t.out | FileCheck --check-prefix=CHECK_LINK %s

; Before linking, this should have a relocation with offset 64.
; CHECK: bnez {{.*}}, #0
; CHECK-NEXT: R_AIE_1 .LBB0_2
; CHECK: j #0
; CHECK-NEXT: R_AIE_1 .LBB0_1
; CHECK: j #0
; CHECK-NEXT: R_AIE_1 .LBB0_2
; After linking, we should have some reasonable addresses.
; These may change slightly as instruction scheduling changes, but
; the first and second always have to point to the same spot
; CHECK_LINK: bnez {{.*}}, #[[LOC:.*]]
; CHECK_LINK: j #{{.*}}
; CHECK_LINK: j #[[LOC]]

define void @foo(i32 %a, i32 %b, i1 %c) {
  %tst1 = icmp eq i32 %a, %b
  br i1 %tst1, label %end, label %test2

test2:
  br label %end

end:
  ret void
}
