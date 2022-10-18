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
; CHECK:     renamable $p0 = MOV_U20_I20 target-flags(aie-global) @a

@a = external dso_local global i32, align 4
define dso_local i32 @_main() {
entry:
  %r1 = load volatile i32, i32* @a, align 4
  ret i32 %r1
}
