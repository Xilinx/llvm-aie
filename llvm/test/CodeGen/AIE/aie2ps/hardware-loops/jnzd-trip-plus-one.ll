; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -O2 -mtriple=aie2ps %s -o - | FileCheck %s --check-prefix=AIE2PS

declare i32 @llvm.start.loop.iterations.i32(i32)

; JNZD wants the backedge-taken count, but llvm.start.loop.iterations receives
; the trip count. When that trip count is an add of 1, instruction selection
; still emits the add and a compensating subtract. A follow-up can use the
; other operand of the add as the counter and drop the subtract.

define i32 @fold_dead_add(i32 %backedge) {
; AIE2PS-LABEL: fold_dead_add:
; AIE2PS:       // %bb.0:
; AIE2PS-NEXT:    nopa ; nopb ; nops ; ret lr; nopm ; nopv
; AIE2PS-NEXT:    nopx // Delay Slot 5
; AIE2PS-NEXT:    nop // Delay Slot 4
; AIE2PS-NEXT:    nop // Delay Slot 3
; AIE2PS-NEXT:    add r0, r1, #1 // Delay Slot 2
; AIE2PS-NEXT:    addm.nc r0, r0, #-1 // Delay Slot 1
  %trip = add i32 %backedge, 1
  %cnt = call i32 @llvm.start.loop.iterations.i32(i32 %trip)
  ret i32 %cnt
}

; The incremented value is still used, so both the add and the subtract remain.
define i32 @fold_live_add(i32 %backedge) {
; AIE2PS-LABEL: fold_live_add:
; AIE2PS:       // %bb.0:
; AIE2PS-NEXT:    nopa ; nopb ; nops ; ret lr; nopm ; nopv
; AIE2PS-NEXT:    nop // Delay Slot 5
; AIE2PS-NEXT:    nop // Delay Slot 4
; AIE2PS-NEXT:    add r0, r1, #1 // Delay Slot 3
; AIE2PS-NEXT:    addm.nc r2, r0, #-1 // Delay Slot 2
; AIE2PS-NEXT:    add r0, r2, r0 // Delay Slot 1
  %trip = add i32 %backedge, 1
  %cnt = call i32 @llvm.start.loop.iterations.i32(i32 %trip)
  %sum = add i32 %cnt, %trip
  ret i32 %sum
}

; An add of anything other than 1 is a real trip count and keeps the adjustment.
define i32 @keep_adjust(i32 %backedge) {
; AIE2PS-LABEL: keep_adjust:
; AIE2PS:       // %bb.0:
; AIE2PS-NEXT:    nopa ; nopb ; nops ; ret lr; nopm ; nopv
; AIE2PS-NEXT:    nopx // Delay Slot 5
; AIE2PS-NEXT:    nop // Delay Slot 4
; AIE2PS-NEXT:    nop // Delay Slot 3
; AIE2PS-NEXT:    add r0, r1, #2 // Delay Slot 2
; AIE2PS-NEXT:    addm.nc r0, r0, #-1 // Delay Slot 1
  %trip = add i32 %backedge, 2
  %cnt = call i32 @llvm.start.loop.iterations.i32(i32 %trip)
  ret i32 %cnt
}
