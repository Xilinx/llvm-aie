; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -O2 -mtriple=aie2ps %s -o - | FileCheck %s --check-prefix=AIE2PS

declare i32 @llvm.start.loop.iterations.i32(i32)

; JNZD wants the backedge-taken count, while llvm.start.loop.iterations
; receives the trip count. When that trip count is an add of 1, the counter is
; the other operand and the compensating subtract is not emitted. An add of any
; other constant still gets the subtract.

define i32 @fold_dead_add(i32 %backedge) {
; AIE2PS-LABEL: fold_dead_add:
; AIE2PS:       // %bb.0:
; AIE2PS-NEXT:    ret lr
; AIE2PS-NEXT:    nop // Delay Slot 5
; AIE2PS-NEXT:    nop // Delay Slot 4
; AIE2PS-NEXT:    nop // Delay Slot 3
; AIE2PS-NEXT:    nop // Delay Slot 2
; AIE2PS-NEXT:    mov r0, r1 // Delay Slot 1
  %trip = add i32 %backedge, 1
  %cnt = call i32 @llvm.start.loop.iterations.i32(i32 %trip)
  ret i32 %cnt
}

; The incremented value is still used, so the add stays and only the subtract
; is dropped. The counter is the value before the add.
define i32 @fold_live_add(i32 %backedge) {
; AIE2PS-LABEL: fold_live_add:
; AIE2PS:       // %bb.0:
; AIE2PS-NEXT:    nopa ; nopb ; nops ; ret lr; nopm ; nopv
; AIE2PS-NEXT:    nopx // Delay Slot 5
; AIE2PS-NEXT:    nop // Delay Slot 4
; AIE2PS-NEXT:    nop // Delay Slot 3
; AIE2PS-NEXT:    add r0, r1, #1 // Delay Slot 2
; AIE2PS-NEXT:    add r0, r1, r0 // Delay Slot 1
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

; The backedge count is a PHI in a block that is not the loop preheader. The
; counter copy goes right after the PHIs in that block, so that the now empty
; preheader can be removed.
define void @phi_backedge(i1 %c, i32 %x, i32 %y, ptr %p, i1 %d) {
; AIE2PS-LABEL: phi_backedge:
; AIE2PS:       // %bb.0: // %entry
; AIE2PS-NEXT:    mova r6, #1; nopb ; nopxm ; nops
; AIE2PS-NEXT:    and r27, r0, r6
; AIE2PS-NEXT:    mova r2, #0; sel.nez r0, r1, r2, r27
; AIE2PS-NEXT:    lt r4, r0, r2
; AIE2PS-NEXT:    jnz r4, #.LBB3_5
; AIE2PS-NEXT:    nop // Delay Slot 5
; AIE2PS-NEXT:    nop // Delay Slot 4
; AIE2PS-NEXT:    nop // Delay Slot 3
; AIE2PS-NEXT:    nop // Delay Slot 2
; AIE2PS-NEXT:    nop // Delay Slot 1
; AIE2PS-NEXT:  // %bb.1: // %header.preheader
; AIE2PS-NEXT:    xor r16, r3, r6
; AIE2PS-NEXT:    movxm p1, #.LBB3_2
; AIE2PS-NEXT:    mova r16, #0; and r6, r16, r6; mov r4, #2
; AIE2PS-NEXT:  .LBB3_2: // Block address taken
; AIE2PS-NEXT:    // %header
; AIE2PS-NEXT:    // =>This Inner Loop Header: Depth=1
; AIE2PS-NEXT:    nopa ; nopb ; jnz r6, #.LBB3_4
; AIE2PS-NEXT:    nop // Delay Slot 5
; AIE2PS-NEXT:    nop // Delay Slot 4
; AIE2PS-NEXT:    lshl r18, r16, r4 // Delay Slot 3
; AIE2PS-NEXT:    mov dj0, r18 // Delay Slot 2
; AIE2PS-NEXT:    st r16, [p0, dj0] // Delay Slot 1
; AIE2PS-NEXT:  // %bb.3: // %then
; AIE2PS-NEXT:    // in Loop: Header=BB3_2 Depth=1
; AIE2PS-NEXT:    st r2, [p0, #0]
; AIE2PS-NEXT:  .LBB3_4: // %latch
; AIE2PS-NEXT:    // in Loop: Header=BB3_2 Depth=1
; AIE2PS-NEXT:    jnzd r0, r0, p1
; AIE2PS-NEXT:    nop // Delay Slot 5
; AIE2PS-NEXT:    nop // Delay Slot 4
; AIE2PS-NEXT:    nop // Delay Slot 3
; AIE2PS-NEXT:    nop // Delay Slot 2
; AIE2PS-NEXT:    add r16, r16, #1 // Delay Slot 1
; AIE2PS-NEXT:  .LBB3_5: // %exit
; AIE2PS-NEXT:    nopa ; ret lr
; AIE2PS-NEXT:    nop // Delay Slot 5
; AIE2PS-NEXT:    nop // Delay Slot 4
; AIE2PS-NEXT:    nop // Delay Slot 3
; AIE2PS-NEXT:    nop // Delay Slot 2
; AIE2PS-NEXT:    nop // Delay Slot 1
entry:
  br i1 %c, label %a, label %b

a:
  br label %merge

b:
  br label %merge

merge:
  %be = phi i32 [ %x, %a ], [ %y, %b ]
  %skip = icmp slt i32 %be, 0
  br i1 %skip, label %exit, label %header

header:
  %i = phi i32 [ 0, %merge ], [ %i.next, %latch ]
  %q = getelementptr i32, ptr %p, i32 %i
  store i32 %i, ptr %q
  br i1 %d, label %then, label %latch

then:
  store i32 0, ptr %p
  br label %latch

latch:
  %i.next = add nuw i32 %i, 1
  %done = icmp eq i32 %i, %be
  br i1 %done, label %exit, label %header

exit:
  ret void
}
