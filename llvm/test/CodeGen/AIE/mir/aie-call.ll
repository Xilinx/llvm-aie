;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates

; RUN: cd %S
; RUN: llc -O2 --march=aie --filetype=obj %s -o %t.mir --stop-after=branch-relaxation
; RUN: llc -O2 --march=aie --filetype=obj %t.mir -o %t.o --start-after=branch-relaxation
; RUN: llc -O2 --march=aie %s -o %t.s
; RUN: cat %t.mir | FileCheck %s

; Check to see that we can properly serialize and de-serialize a global to mir
; CHECK:     JAL @external_function

declare i32 @external_function(i32)

define i32 @test_call_external(i32 %a) nounwind {
  %1 = call i32 @external_function(i32 %a)
  ret i32 %1
}
