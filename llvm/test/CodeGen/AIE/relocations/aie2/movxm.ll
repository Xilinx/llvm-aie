;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -march=aie2 -O2 %s --filetype=obj -o %t.o
; RUN: llvm-objdump -dr --triple=aie2 %t.o | FileCheck --check-prefix=CHECK %s
; CHECK-LABEL: getData
; CHECK: nopa	;		nopb	;		movxm	p0, #0x0;		nop
; CHECK-NEXT: R_AIE_49 data+0x4

@data = dso_local local_unnamed_addr global [2 x i32] [i32 10, i32 20], align 4

define dso_local noundef i32 @getData() {
entry:
  %0 = load i32, ptr getelementptr inbounds ([2 x i32], ptr @data, i20 0, i20 1), align 4
  ret i32 %0
}
