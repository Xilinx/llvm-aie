;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; Plain AA queries may relate any two dynamic instances of the accesses, so
; cursors advancing at different rates must not be disambiguated by the byte
; model. Here the load covers [8n+4, 8n+8) and the store [4m, 4m+2): they are
; disjoint for n == m, but collide for the load of iteration 0 and the store of
; iteration 1.
;
; RUN: opt -mtriple=aie2p -passes=aa-eval -print-all-alias-modref-info -disable-output < %s 2>&1 | FileCheck %s

; CHECK-LABEL: Function: different_steps
; CHECK:         MayAlias: i32* %pl.mid, i16* %ps

define void @different_steps(ptr noalias %p, i32 %n) {
entry:
  br label %loop

loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pl = phi ptr [ %p, %entry ], [ %pl.next, %loop ]
  %ps = phi ptr [ %p, %entry ], [ %ps.next, %loop ]
  %pl.mid = getelementptr inbounds i8, ptr %pl, i20 4
  %v = load i32, ptr %pl.mid, align 4
  %w = trunc i32 %v to i16
  store i16 %w, ptr %ps, align 2
  %pl.next = getelementptr inbounds i8, ptr %pl.mid, i20 4
  %ps.next = getelementptr inbounds i8, ptr %ps, i20 4
  %i.next = add nuw nsw i32 %i, 1
  %c = icmp ult i32 %i.next, %n
  br i1 %c, label %loop, label %exit

exit:
  ret void
}
