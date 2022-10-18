;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie --issue-limit=1 < %s | FileCheck %s
; CHECK: _main:
; CHECK:             mov.u20 p0, #a
; CHECK-NEXT:        lda     r12, [p0]
; CHECK-NEXT:        mov.u20 r13, #7
; CHECK-NEXT:        mov.u20 p0, #b
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        mul     r0, r12, r13
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        st      r0, [p0]
; CHECK-NEXT:        ret lr

@x = dso_local local_unnamed_addr global i32* null, align 4
@a = external dso_local local_unnamed_addr global i32, align 4
@b = external dso_local local_unnamed_addr global i32, align 4

; Function Attrs: nofree norecurse nounwind mustprogress
define dso_local i32 @_main() local_unnamed_addr #0 {
  %1 = load i32, i32* @a, align 4
  %2 = mul nsw i32 %1, 7
  store i32 %2, i32* @b, align 4
  ret i32 %2
}

attributes #0 = { nofree norecurse nounwind mustprogress "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
