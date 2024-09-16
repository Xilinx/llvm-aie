;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: opt -mtriple=aie2 -passes=aa-eval -print-all-alias-modref-info --aie-alias-analysis-addrspace=true  -disable-output < %s 2>&1 | FileCheck %s --check-prefixes=CHECK,ENABLED-AIE-AS-AA
; RUN: opt -mtriple=aie2 -passes=aa-eval -print-all-alias-modref-info --aie-alias-analysis-addrspace=false -disable-output < %s 2>&1 | FileCheck %s --check-prefixes=CHECK,DISABLED-AIE-AS-AA

; CHECK-LABEL: Function: basic_without_AS
; CHECK: MayAlias:      i8* %p, i8* %p1
define void @basic_without_AS(ptr %p, ptr %p1) {
  load i8, ptr %p
  load i8, ptr %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_56
; ENABLED-AIE-AS-AA: NoAlias:	i8 addrspace(5)* %p, i8 addrspace(6)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_56
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(5)* %p, i8 addrspace(6)* %p1
define void @basic_withAS_56(ptr addrspace(5) %p, ptr addrspace(6) %p1) {
  load i8, ptr addrspace(5) %p 
  load i8, ptr addrspace(6) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_67
; ENABLED-AIE-AS-AA: NoAlias:	i8 addrspace(6)* %p, i8 addrspace(7)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_67
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(6)* %p, i8 addrspace(7)* %p1
define void @basic_withAS_67(ptr addrspace(6) %p, ptr addrspace(7) %p1) {
  load i8, ptr addrspace(6) %p 
  load i8, ptr addrspace(7) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_78
; ENABLED-AIE-AS-AA: NoAlias:	i8 addrspace(8)* %p, i8 addrspace(7)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_78
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(8)* %p, i8 addrspace(7)* %p1
define void @basic_withAS_78(ptr addrspace(8) %p, ptr addrspace(7) %p1) {
  load i8, ptr addrspace(8) %p 
  load i8, ptr addrspace(7) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_58
; ENABLED-AIE-AS-AA: NoAlias:	i8 addrspace(5)* %p, i8 addrspace(8)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_58
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(5)* %p, i8 addrspace(8)* %p1
define void @basic_withAS_58(ptr addrspace(5) %p, ptr addrspace(8) %p1) {
  load i8, ptr addrspace(5) %p 
  load i8, ptr addrspace(8) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_compund_A_AB
; ENABLED-AIE-AS-AA: MayAlias: 	i8 addrspace(5)* %p, i8 addrspace(9)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_compund_A_AB
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(5)* %p, i8 addrspace(9)* %p1
define void @basic_withAS_compund_A_AB(ptr addrspace(5) %p, ptr addrspace(9) %p1) {
  load i8, ptr addrspace(5) %p 
  load i8, ptr addrspace(9) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_compund_B_AB
; ENABLED-AIE-AS-AA: MayAlias: 	i8 addrspace(6)* %p, i8 addrspace(9)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_compund_B_AB
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(6)* %p, i8 addrspace(9)* %p1
define void @basic_withAS_compund_B_AB(ptr addrspace(6) %p, ptr addrspace(9) %p1) {
  load i8, ptr addrspace(6) %p 
  load i8, ptr addrspace(9) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_compund_C_AB
; ENABLED-AIE-AS-AA: NoAlias: 	i8 addrspace(7)* %p, i8 addrspace(9)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_compund_C_AB
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(7)* %p, i8 addrspace(9)* %p1
define void @basic_withAS_compund_C_AB(ptr addrspace(7) %p, ptr addrspace(9) %p1) {
  load i8, ptr addrspace(7) %p 
  load i8, ptr addrspace(9) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_compund_AB_CD
; ENABLED-AIE-AS-AA: NoAlias: 	i8 addrspace(9)* %p, i8 addrspace(14)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_compund_AB_CD
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(9)* %p, i8 addrspace(14)* %p1
define void @basic_withAS_compund_AB_CD(ptr addrspace(9) %p, ptr addrspace(14) %p1) {
  load i8, ptr addrspace(9) %p 
  load i8, ptr addrspace(14) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_TM_noAS
; ENABLED-AIE-AS-AA: NoAlias: 	i8 addrspace(15)* %p, i8* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_TM_noAS
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(15)* %p, i8* %p1
define void @basic_TM_noAS(ptr addrspace(15) %p, ptr %p1) {
  load i8, ptr addrspace(15) %p 
  load i8, ptr %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_noAS_TM
; ENABLED-AIE-AS-AA: NoAlias: 	i8* %p, i8 addrspace(15)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_noAS_TM
; DISABLED-AIE-AS-AA: MayAlias:	  i8* %p, i8 addrspace(15)* %p1
define void @basic_noAS_TM(ptr %p, ptr  addrspace(15) %p1) {
  load i8, ptr %p 
  load i8, ptr addrspace(15) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_TM_TM
; ENABLED-AIE-AS-AA: MayAlias: 	i8 addrspace(15)* %p, i8 addrspace(15)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_TM_TM
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(15)* %p, i8 addrspace(15)* %p1
define void @basic_withAS_TM_TM(ptr addrspace(15) %p, ptr addrspace(15) %p1) {
  load i8, ptr addrspace(15) %p 
  load i8, ptr addrspace(15) %p1
  ret void
}

; ENABLED-AIE-AS-AA-LABEL: Function: basic_withAS_A_TM
; ENABLED-AIE-AS-AA: NoAlias: 	i8 addrspace(5)* %p, i8 addrspace(15)* %p1

; DISABLED-AIE-AS-AA-LABEL: Function: basic_withAS_A_TM
; DISABLED-AIE-AS-AA: MayAlias:	  i8 addrspace(5)* %p, i8 addrspace(15)* %p1
define void @basic_withAS_A_TM(ptr addrspace(5) %p, ptr addrspace(15) %p1) {
  load i8, ptr addrspace(5) %p 
  load i8, ptr addrspace(15) %p1
  ret void
}
