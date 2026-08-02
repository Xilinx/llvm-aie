;===- aie2p-unlinked-object.ll ------------------------------------------===;
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
;===---------------------------------------------------------------------===;
;
; An object that still needs linking is refused, not run.
;
; Sections load where they say they live, and in a relocatable object every
; allocatable section says 0 -- so they land on top of each other, and the
; fields the relocations would have filled are zero. That does not fail: AIE
; spells branch targets and loop bounds as absolute addresses, so a zeroed one
; is a jump to 0, and the run spins to the bundle cap looking like a hang with
; no mention of the actual problem.
;
; The loop is what makes this object carry relocations at all. Straight-line
; hand-written tests in this directory resolve everything at assembly time,
; which is why they are unaffected.

; RUN: llc -mtriple=aie2p -O2 -filetype=obj %s -o %t.o
; RUN: not llvm-aie-run %t.o 2>&1 | FileCheck %s

@out = global [64 x i32] zeroinitializer, align 32

define void @_start() {
entry:
  br label %loop

loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %p = getelementptr inbounds [64 x i32], ptr @out, i32 0, i32 %i
  store i32 %i, ptr %p, align 4
  %i.next = add i32 %i, 1
  %done = icmp eq i32 %i.next, 64
  br i1 %done, label %fin, label %loop

fin:
  call void @llvm.aie2p.done()
  unreachable
}

declare void @llvm.aie2p.done()

; CHECK: error: unapplied relocations in .text: link this object before running it
