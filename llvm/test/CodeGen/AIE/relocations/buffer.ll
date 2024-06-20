;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -march=aie -O0 %s --filetype=obj -o %t.o
; RUN: llvm-objdump -dr --arch-name=aie %t.o | FileCheck --check-prefix=CHECK %s
; We don't actually have ld.lld, unless it's been compiled!
; UN: ld.lld %t.o --defsym=a=64 -o %t.out
; UN: llvm-objdump -dr --arch-name=aie %t.out | FileCheck --check-prefix=CHECK_LINK %s

; Before linking, this should have a relocation with offset 64.
; CHECK: jal #0
; CHECK-NEXT: R_AIE_1 core11
; After linking, this should have a load from address 128=a+64
; CHECK_LINK: mov.u20 {{.*}}, #128
@a = external dso_local global [256 x i32]

define void @core11() {
  store i32 1, i32* getelementptr inbounds ([256 x i32], [256 x i32]* @a, i64 0, i64 16), align 4
  ret void
}

define void @_main() {
  call void @core11()
  ret void
}

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
