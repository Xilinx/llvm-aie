;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: opt -mtriple=aie2 -passes=aa-eval -print-all-alias-modref-info -disable-output < %s 2>&1 | FileCheck %s --check-prefixes=CHECK,ENABLED-AIE-AA
; RUN: opt -mtriple=aie2 -passes=aa-eval -print-all-alias-modref-info --aie-enable-alias-analysis=false -disable-output < %s 2>&1 | FileCheck %s --check-prefixes=CHECK,DISABLED-AIE-AA

; CHECK-LABEL: Function: basic_may_alias
; CHECK: MayAlias:      i8* %p, i8* %p1
define void @basic_may_alias(ptr %p, ptr %p1) {
  load i8, ptr %p
  load i8, ptr %p1
  ret void
}

; CHECK-LABEL: Function: basic_no_alias
; CHECK: NoAlias:      i8* %p, i8* %p1
define void @basic_no_alias(ptr noalias %p, ptr %p1) {
  load i8, ptr %p
  load i8, ptr %p1
  ret void
}

declare { ptr, i20 } @llvm.aie2.add.2d(ptr, i20, i20, i20, i20)
declare { ptr, i20, i20 } @llvm.aie2.add.3d(ptr, i20, i20, i20, i20, i20, i20, i20)

; ENABLED-AIE-AA-LABEL: Function: add_2d_different_base
; ENABLED-AIE-AA: NoAlias:      i8* %2, i8* %p1

; DISABLED-AIE-AA-LABEL: Function: add_2d_different_base
; DISABLED-AIE-AA: MayAlias:      i8* %2, i8* %p1
define void @add_2d_different_base(ptr noalias %p, ptr %p1) {
  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %p, i20 0, i20 1, i20 2, i20 3)
  %2 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %2
  load i8, ptr %p1
  ret void
}

; ENABLED-AIE-AA-LABEL: Function: add_2d_potentially_same_base
; ENABLED-AIE-AA: MayAlias:      i8* %2, i8* %p1

; DISABLED-AIE-AA-LABEL: Function: add_2d_potentially_same_base
; DISABLED-AIE-AA: MayAlias:      i8* %2, i8* %p1
define void @add_2d_potentially_same_base(ptr %p, ptr %p1) {
  %1 = tail call { ptr, i20 } @llvm.aie2.add.2d(ptr %p, i20 0, i20 1, i20 2, i20 3)
  %2 = extractvalue { ptr, i20 } %1, 0
  load i8, ptr %2
  load i8, ptr %p1
  ret void
}

; ENABLED-AIE-AA-LABEL: Function: add_3d_different_base
; ENABLED-AIE-AA: NoAlias:      i8* %2, i8* %p1

; DISABLED-AIE-AA-LABEL: Function: add_3d_different_base
; DISABLED-AIE-AA: MayAlias:      i8* %2, i8* %p1
define void @add_3d_different_base(ptr noalias %p, ptr %p1) {
  %1 = tail call { ptr, i20, i20 } @llvm.aie2.add.3d(ptr %p, i20 0, i20 1, i20 2, i20 3, i20 4, i20 5, i20 6)
  %2 = extractvalue { ptr, i20, i20 } %1, 0
  load i8, ptr %2
  load i8, ptr %p1
  ret void
}

; ENABLED-AIE-AA-LABEL: Function: add_3d_potentially_same_base
; ENABLED-AIE-AA: MayAlias:      i8* %2, i8* %p1

; DISABLED-AIE-AA-LABEL: Function: add_3d_potentially_same_base
; DISABLED-AIE-AA: MayAlias:      i8* %2, i8* %p1
define void @add_3d_potentially_same_base(ptr %p, ptr %p1) {
  %1 = tail call { ptr, i20, i20 } @llvm.aie2.add.3d(ptr %p, i20 0, i20 1, i20 2, i20 3, i20 4, i20 5, i20 6)
  %2 = extractvalue { ptr, i20, i20 } %1, 0
  load i8, ptr %2
  load i8, ptr %p1
  ret void
}
