;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: opt < %s -aa-pipeline=basic-aa -passes=aa-eval -print-all-alias-modref-info -disable-output 2>&1 | FileCheck %s
; RUN: opt < %s -aa-pipeline=basic-aa -passes=aa-eval -print-all-alias-modref-info -basic-aa-full-phi-analysis \
; RUN:       -disable-output 2>&1 | FileCheck %s --check-prefix=FULL-PHI

@X = common global i32 0
@Y = common global i32 0

; The goal of this set of tests is to test the differences in the behavior of
; the BasicAA with and without full phi analysis in a dual diamond scenario.


; Test 1: The basic shape of the test is the following (simp. without phis):

; if (...) {
;    if (...) {
;      %P = getelementptr @X ...
;    } else {
;      %P = getelementptr @X ...
;    }    
; } else {
;   %P = getelementptr @X ...
; }
;
; // Use of %P and @Y 
;

; CHECK: MayAlias: i32* %P, i32* @Y
; FULL-PHI: NoAlias: i32* %P, i32* @Y
define void @test1(i32 %cond) nounwind {
entry:
  %"alloca point" = bitcast i32 0 to i32
  %tmp = icmp ne i32 %cond, 0
  br i1 %tmp, label %bbtrue, label %bbfalse

bbtrue:
  %tmp1 = icmp ne i32 %cond, 2
  br i1 %tmp, label %bbtruetrue, label %bbtruefalse

bbtruetrue:
  %p1 = getelementptr i32, ptr @X, i64 0
  br label %bbtrueend

bbtruefalse:
  %p2 = getelementptr i32, ptr @X, i64 0
  br label %bbtrueend

bbtrueend:
  %p3 = phi ptr [ %p1, %bbtruetrue ], [ %p2, %bbtruefalse ]
  br label %bblast

bbfalse:
  %p4 = getelementptr i32, ptr @X, i64 0
  br label %bblast

bblast:
  %P = phi ptr [ %p3, %bbtrueend ], [ %p4, %bbfalse ]
  %tmp2 = load i32, ptr @Y, align 4
  store i32 123, ptr %P, align 4
  %tmp3 = load i32, ptr @Y, align 4
  br label %return

return:
  ret void
}


; Test 2: The basic shape of the test is the following (simp. without phis):

; if (...) {
;    if (...) {
;      %P = getelementptr @Y ... (cause of MayAlias)
;    } else {
;      %P = getelementptr @X ...
;    }    
; } else {
;   %P = getelementptr @X ...
; }
;
; // Use of %P and @Y 
;

; CHECK: MayAlias: i32* %P, i32* @Y
; FULL-PHI: MayAlias: i32* %P, i32* @Y
define void @test2(i32 %cond) nounwind {
entry:
  %"alloca point" = bitcast i32 0 to i32
  %tmp = icmp ne i32 %cond, 0
  br i1 %tmp, label %bbtrue, label %bbfalse

bbtrue:
  %tmp1 = icmp ne i32 %cond, 2
  br i1 %tmp, label %bbtruetrue, label %bbtruefalse

bbtruetrue:
  %p1 = getelementptr i32, ptr @Y, i64 0
  br label %bbtrueend

bbtruefalse:
  %p2 = getelementptr i32, ptr @X, i64 0
  br label %bbtrueend

bbtrueend:
  %p3 = phi ptr [ %p1, %bbtruetrue ], [ %p2, %bbtruefalse ]
  br label %bblast

bbfalse:
  %p4 = getelementptr i32, ptr @X, i64 0
  br label %bblast

bblast:
  %P = phi ptr [ %p3, %bbtrueend ], [ %p4, %bbfalse ]
  %tmp2 = load i32, ptr @Y, align 4
  store i32 123, ptr %P, align 4
  %tmp3 = load i32, ptr @Y, align 4
  br label %return

return:
  ret void
}
