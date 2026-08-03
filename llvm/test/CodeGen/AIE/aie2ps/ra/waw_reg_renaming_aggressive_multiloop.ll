; Crash when aggressive realloc frees live-in registers in one loop and a
; subsequent loop's getHighOutputLatencyRegs accesses the now-unmapped VRegs.
; Reduced from MaxPool2D kernel with llvm-reduce.
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; XFAIL: *
; RUN: llc -mtriple=aie2ps --aie-aggressive-realloc -aie-gpr-realloc \
; RUN:   --stop-after=virtregrewriter %s -o - | FileCheck %s

define void @maxpool_multiloop() {
entry:
  br label %loop1

loop1:
  %0 = call i1 @llvm.loop.decrement.i32(i32 0)
  br i1 %0, label %loop1, label %loop2

loop2:
  %1 = load <8 x i32>, ptr addrspace(5) null, align 32
  %shuffle2 = shufflevector <8 x i32> zeroinitializer, <8 x i32> %1, <16 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 10, i32 11, i32 12, i32 13, i32 14, i32 15>
  %2 = tail call <16 x i32> @llvm.aie2ps.vshift.I512.I512(<16 x i32> %shuffle2, <16 x i32> zeroinitializer, i32 0, i32 1)
  %shuffle = shufflevector <16 x i32> %2, <16 x i32> zeroinitializer, <8 x i32> <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
  store <8 x i32> %shuffle, ptr addrspace(6) null, align 32
  %3 = call i1 @llvm.loop.decrement.i32(i32 0)
  br i1 %3, label %loop2, label %exit

exit:
  ret void
}
