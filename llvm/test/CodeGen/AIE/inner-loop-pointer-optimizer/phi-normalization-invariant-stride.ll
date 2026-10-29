; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2ps -O2 \
; RUN:     -stop-after=aie-inner-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Pre-increment pointer PHIs whose stride is a runtime value.
; Checks are the IR immediately after the inner-loop pointer optimizer.

; CHECK-LABEL: define void @invariant_stride_load(
; CHECK:         %ptr.shifted.init = getelementptr i8, ptr %p, i20 %s
; CHECK:         %ptr = phi ptr [ %ptr.shifted.init, %entry ], [ %ptr.back, %loop ]
; CHECK:         %v = load i32, ptr %ptr
; CHECK:         %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 %s

define void @invariant_stride_load(ptr %p, ptr %out, i32 %stride, i32 %n) {
entry:
  %s = trunc i32 %stride to i20
  br label %loop

loop:
  %ptr = phi ptr [ %p, %entry ], [ %ptr.next, %loop ]
  %acc = phi i32 [ 0, %entry ], [ %acc.next, %loop ]
  %i = phi i32 [ %n, %entry ], [ %i.dec, %loop ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
  %v = load i32, ptr %ptr.next
  %acc.next = add i32 %acc, %v
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  store i32 %acc.next, ptr %out
  ret void
}

; CHECK-LABEL: define void @invariant_stride_store(
; CHECK:         %ptr.shifted.init = getelementptr i8, ptr %p, i20 %s
; CHECK:         %ptr = phi ptr [ %ptr.shifted.init, %entry ], [ %ptr.back, %loop ]
; CHECK:         store i32 %val, ptr %ptr
; CHECK:         %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 %s

define void @invariant_stride_store(ptr %p, i32 %val, i32 %stride, i32 %n) {
entry:
  %s = trunc i32 %stride to i20
  br label %loop

loop:
  %ptr = phi ptr [ %p, %entry ], [ %ptr.next, %loop ]
  %i = phi i32 [ %n, %entry ], [ %i.dec, %loop ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
  store i32 %val, ptr %ptr.next
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}

; A sibling GEP would need its offset reduced by the stride, which is not a
; constant here.
; CHECK-LABEL: define void @invariant_stride_with_sibling(
; CHECK-NOT:     shifted.init
; CHECK:         %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
; CHECK:         %sibling = getelementptr inbounds i8, ptr %ptr, i20 64
; CHECK:         %v0 = load i32, ptr %ptr.next

define void @invariant_stride_with_sibling(ptr %p, ptr %out, i32 %stride, i32 %n) {
entry:
  %s = trunc i32 %stride to i20
  br label %loop

loop:
  %ptr = phi ptr [ %p, %entry ], [ %ptr.next, %loop ]
  %acc = phi i32 [ 0, %entry ], [ %acc.next, %loop ]
  %i = phi i32 [ %n, %entry ], [ %i.dec, %loop ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
  %sibling = getelementptr inbounds i8, ptr %ptr, i20 64
  %v0 = load i32, ptr %ptr.next
  %v1 = load i32, ptr %sibling
  %sum = add i32 %v0, %v1
  %acc.next = add i32 %acc, %sum
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  store i32 %acc.next, ptr %out
  ret void
}

; CHECK-LABEL: define void @variant_stride(
; CHECK-NOT:     shifted.init
; CHECK:         %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %i20
; CHECK:         %v = load i32, ptr %ptr.next

define void @variant_stride(ptr %p, ptr %out, i32 %n) {
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %p, %entry ], [ %ptr.next, %loop ]
  %acc = phi i32 [ 0, %entry ], [ %acc.next, %loop ]
  %i = phi i32 [ %n, %entry ], [ %i.dec, %loop ]
  %i20 = trunc i32 %i to i20
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %i20
  %v = load i32, ptr %ptr.next
  %acc.next = add i32 %acc, %v
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  store i32 %acc.next, ptr %out
  ret void
}

; CHECK-LABEL: define void @live_out_phi(
; CHECK-NOT:     shifted.init
; CHECK:         %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
; CHECK:         %v = load i32, ptr %ptr.next
; CHECK:         store ptr %ptr, ptr %out.ptr

define void @live_out_phi(ptr %p, ptr %out, ptr %out.ptr, i32 %stride, i32 %n) {
entry:
  %s = trunc i32 %stride to i20
  br label %loop

loop:
  %ptr = phi ptr [ %p, %entry ], [ %ptr.next, %loop ]
  %acc = phi i32 [ 0, %entry ], [ %acc.next, %loop ]
  %i = phi i32 [ %n, %entry ], [ %i.dec, %loop ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
  %v = load i32, ptr %ptr.next
  %acc.next = add i32 %acc, %v
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  store i32 %acc.next, ptr %out
  store ptr %ptr, ptr %out.ptr
  ret void
}

; The inner loop starts at %base + 192 and walks by %stride. The shifted
; start is %base + (192 + %stride), with the sum computed before the outer
; loop.
; CHECK-LABEL: define void @shift_constant_offset_init(
; CHECK:         %ptr.shift = add i20 192, %s
; CHECK:       outer:
; CHECK-NOT:     add i20
; CHECK:         %ptr.shifted.init = getelementptr i8, ptr %base, i20 %ptr.shift
; CHECK:         %v = load i32, ptr %ptr
; CHECK:         %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 %s

define void @shift_constant_offset_init(ptr %a, ptr %out, i32 %stride, i32 %n, i32 %m) {
entry:
  %s = trunc i32 %stride to i20
  br label %outer

outer:
  %base = phi ptr [ %a, %entry ], [ %base.next, %outer.latch ]
  %j = phi i32 [ %m, %entry ], [ %j.dec, %outer.latch ]
  %init = getelementptr inbounds i8, ptr %base, i20 192
  %v0 = load i32, ptr %init
  br label %inner

inner:
  %ptr = phi ptr [ %init, %outer ], [ %ptr.next, %inner ]
  %acc = phi i32 [ %v0, %outer ], [ %acc.next, %inner ]
  %i = phi i32 [ %n, %outer ], [ %i.dec, %inner ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
  %v = load i32, ptr %ptr.next
  %acc.next = add i32 %acc, %v
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %inner, label %outer.latch

outer.latch:
  store i32 %acc.next, ptr %out
  %base.next = getelementptr inbounds i8, ptr %base, i20 64
  %j.dec = add i32 %j, -1
  %outer.cond = icmp ne i32 %j.dec, 0
  br i1 %outer.cond, label %outer, label %exit

exit:
  ret void
}

; The stride is only invariant in the inner loop, so 192 + %s cannot be
; computed before the outer loop. Keep the shifted init as
; (%base + 192) + %s instead of adding the offsets every outer iteration.
; CHECK-LABEL: define void @stride_defined_in_outer_loop(
; CHECK-NOT:     add i20
; CHECK:         %ptr.shifted.init = getelementptr i8, ptr %init, i20 %s
; CHECK:         %v = load i32, ptr %ptr
; CHECK:         %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 %s

define void @stride_defined_in_outer_loop(ptr %a, ptr %out, ptr %strides, i32 %n, i32 %m) {
entry:
  br label %outer

outer:
  %base = phi ptr [ %a, %entry ], [ %base.next, %outer.latch ]
  %sp = phi ptr [ %strides, %entry ], [ %sp.next, %outer.latch ]
  %j = phi i32 [ %m, %entry ], [ %j.dec, %outer.latch ]
  %stride = load i32, ptr %sp
  %s = trunc i32 %stride to i20
  %init = getelementptr inbounds i8, ptr %base, i20 192
  %v0 = load i32, ptr %init
  br label %inner

inner:
  %ptr = phi ptr [ %init, %outer ], [ %ptr.next, %inner ]
  %acc = phi i32 [ %v0, %outer ], [ %acc.next, %inner ]
  %i = phi i32 [ %n, %outer ], [ %i.dec, %inner ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
  %v = load i32, ptr %ptr.next
  %acc.next = add i32 %acc, %v
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %inner, label %outer.latch

outer.latch:
  store i32 %acc.next, ptr %out
  %base.next = getelementptr inbounds i8, ptr %base, i20 64
  %sp.next = getelementptr inbounds i8, ptr %sp, i20 4
  %j.dec = add i32 %j, -1
  %outer.cond = icmp ne i32 %j.dec, 0
  br i1 %outer.cond, label %outer, label %exit

exit:
  ret void
}

; The start offset and the stride have different integer types, so they are
; not folded into one offset.
; CHECK-LABEL: define void @init_offset_type_mismatch(
; CHECK-NOT:     add i20
; CHECK:         %init = getelementptr inbounds i8, ptr %base, i32 192
; CHECK:         %ptr.shifted.init = getelementptr i8, ptr %init, i20 %s
; CHECK:         %v = load i32, ptr %ptr

define void @init_offset_type_mismatch(ptr %a, ptr %out, i32 %stride, i32 %n, i32 %m) {
entry:
  %s = trunc i32 %stride to i20
  br label %outer

outer:
  %base = phi ptr [ %a, %entry ], [ %base.next, %outer.latch ]
  %j = phi i32 [ %m, %entry ], [ %j.dec, %outer.latch ]
  %init = getelementptr inbounds i8, ptr %base, i32 192
  %v0 = load i32, ptr %init
  br label %inner

inner:
  %ptr = phi ptr [ %init, %outer ], [ %ptr.next, %inner ]
  %acc = phi i32 [ %v0, %outer ], [ %acc.next, %inner ]
  %i = phi i32 [ %n, %outer ], [ %i.dec, %inner ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
  %v = load i32, ptr %ptr.next
  %acc.next = add i32 %acc, %v
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %inner, label %outer.latch

outer.latch:
  store i32 %acc.next, ptr %out
  %base.next = getelementptr inbounds i8, ptr %base, i20 64
  %j.dec = add i32 %j, -1
  %outer.cond = icmp ne i32 %j.dec, 0
  br i1 %outer.cond, label %outer, label %exit

exit:
  ret void
}

; Outer-loop pipelined loop nest where the second pointer starts at the
; first one plus 192.
; CHECK-LABEL: define void @olp_second_pointer_from_first(
; CHECK:         %q.shift = add i20 192, %stride
; CHECK:       outer:
; CHECK:         %q.shifted.init = getelementptr i8, ptr %pa.flat, i20 %q.shift
; CHECK:         %y3 = load <32 x i16>, ptr addrspace(5) %q.next.as
; CHECK:         %q.back = getelementptr inbounds i8, ptr %q, i20 %stride

define void @olp_second_pointer_from_first(ptr addrspace(5) %a, i20 %stride, i20 %d1, i20 %d2, <32 x i16> %w, i32 %m, i32 %n) {
entry:
  br label %outer

outer:
  %j = phi i32 [ 0, %entry ], [ %j.next, %outer.latch ]
  %pa = phi ptr addrspace(5) [ %a, %entry ], [ %pa.inner.next, %outer.latch ]
  %pa.flat = addrspacecast ptr addrspace(5) %pa to ptr
  %pa2.init = getelementptr i8, ptr %pa.flat, i20 192
  %pa2.init.as = addrspacecast ptr %pa2.init to ptr addrspace(5)
  %x0 = load <32 x i16>, ptr addrspace(5) %pa, align 64
  %pa.128 = getelementptr i8, ptr %pa.flat, i20 128
  %pa.128.as = addrspacecast ptr %pa.128 to ptr addrspace(5)
  %x2 = load <32 x i16>, ptr addrspace(5) %pa.128.as, align 64
  %pa.3d = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr %pa.128, i20 0, i20 %d1, i20 0, i20 %d2, i20 0, i20 0, i20 0)
  %x3 = load <32 x i16>, ptr addrspace(5) %pa2.init.as, align 64
  %acc0 = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.addmac.conf(<32 x i16> %x0, <32 x i16> %w, <64 x i32> zeroinitializer, <64 x i32> zeroinitializer, i32 0)
  %acc2 = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.addmac.conf(<32 x i16> %x2, <32 x i16> %w, <64 x i32> zeroinitializer, <64 x i32> zeroinitializer, i32 0)
  %acc3 = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.addmac.conf(<32 x i16> %x3, <32 x i16> %w, <64 x i32> zeroinitializer, <64 x i32> zeroinitializer, i32 0)
  %pa.next.flat = extractvalue { ptr, i20, i20 } %pa.3d, 0
  %pa.next = addrspacecast ptr %pa.next.flat to ptr addrspace(5)
  br label %inner

inner:
  %pa.i = phi ptr addrspace(5) [ %pa.next, %outer ], [ %pa.inner.next, %inner ]
  %i = phi i32 [ 0, %outer ], [ %i.next, %inner ]
  %q = phi ptr [ %pa2.init, %outer ], [ %q.next, %inner ]
  %c3 = phi <64 x i32> [ %acc3, %outer ], [ %c3.next, %inner ]
  %c2 = phi <64 x i32> [ %acc2, %outer ], [ %c2.next, %inner ]
  %c0 = phi <64 x i32> [ %acc0, %outer ], [ %c0.next, %inner ]
  %q.next = getelementptr i8, ptr %q, i20 %stride
  %q.next.as = addrspacecast ptr %q.next to ptr addrspace(5)
  %y0 = load <32 x i16>, ptr addrspace(5) %pa.i, align 64
  %pa.i.flat = addrspacecast ptr addrspace(5) %pa.i to ptr
  %pa.i.3d = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr %pa.i.flat, i20 0, i20 %d1, i20 0, i20 %d2, i20 0, i20 0, i20 0)
  %y3 = load <32 x i16>, ptr addrspace(5) %q.next.as, align 64
  %c0.next = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.mac.conf(<32 x i16> %y0, <32 x i16> %w, <64 x i32> %c0, i32 0)
  %c2.next = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.mac.conf(<32 x i16> %y3, <32 x i16> %w, <64 x i32> %c2, i32 0)
  %c3.next = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.mac.conf(<32 x i16> %w, <32 x i16> %w, <64 x i32> %c3, i32 0)
  %i.next = add i32 %i, 1
  %pa.i.next.flat = extractvalue { ptr, i20, i20 } %pa.i.3d, 0
  %pa.inner.next = addrspacecast ptr %pa.i.next.flat to ptr addrspace(5)
  %inner.done = icmp eq i32 %i, %n
  br i1 %inner.done, label %outer.latch, label %inner

outer.latch:
  %r0 = tail call <64 x i8> @llvm.aie2ps.I512.v64.acc32.srs(<64 x i32> %c0.next, i32 0, i32 0)
  %r2 = tail call <64 x i8> @llvm.aie2ps.I512.v64.acc32.srs(<64 x i32> %c2.next, i32 0, i32 0)
  %r3 = tail call <64 x i8> @llvm.aie2ps.I512.v64.acc32.srs(<64 x i32> %c3.next, i32 0, i32 0)
  %j.next = add i32 %j, 1
  %outer.done = icmp eq i32 %j.next, %m
  br i1 %outer.done, label %exit, label %outer, !llvm.loop !0

exit:
  ret void
}

; Five inner pointers start at %base + C and share a stride that is invariant
; in the outer loop. Each folded C + %s stays live across the outer loop.
; CHECK-LABEL: define void @many_pointers_folded_shift(
; CHECK:         %p0.shift = add i20 64, %s
; CHECK-NOT:     add i20
; CHECK:         %p0.shifted.init = getelementptr i8, ptr %base, i20 %p0.shift
; CHECK:         %p1.shifted.init = getelementptr i8, ptr %init1, i20 %s
; CHECK:         %p2.shifted.init = getelementptr i8, ptr %init2, i20 %s
; CHECK:         %p3.shifted.init = getelementptr i8, ptr %init3, i20 %s
; CHECK:         %p4.shifted.init = getelementptr i8, ptr %init4, i20 %s
; CHECK:         %v0 = load i32, ptr %p0

define void @many_pointers_folded_shift(ptr %a, ptr %out, i32 %stride, i32 %n, i32 %m) {
entry:
  %s = trunc i32 %stride to i20
  br label %outer

outer:
  %base = phi ptr [ %a, %entry ], [ %base.next, %outer.latch ]
  %j = phi i32 [ %m, %entry ], [ %j.dec, %outer.latch ]
  %init0 = getelementptr inbounds i8, ptr %base, i20 64
  %init1 = getelementptr inbounds i8, ptr %base, i20 128
  %init2 = getelementptr inbounds i8, ptr %base, i20 192
  %init3 = getelementptr inbounds i8, ptr %base, i20 256
  %init4 = getelementptr inbounds i8, ptr %base, i20 320
  br label %inner

inner:
  %p0 = phi ptr [ %init0, %outer ], [ %p0.next, %inner ]
  %p1 = phi ptr [ %init1, %outer ], [ %p1.next, %inner ]
  %p2 = phi ptr [ %init2, %outer ], [ %p2.next, %inner ]
  %p3 = phi ptr [ %init3, %outer ], [ %p3.next, %inner ]
  %p4 = phi ptr [ %init4, %outer ], [ %p4.next, %inner ]
  %acc = phi i32 [ 0, %outer ], [ %acc.next, %inner ]
  %i = phi i32 [ %n, %outer ], [ %i.dec, %inner ]
  %p0.next = getelementptr inbounds i8, ptr %p0, i20 %s
  %p1.next = getelementptr inbounds i8, ptr %p1, i20 %s
  %p2.next = getelementptr inbounds i8, ptr %p2, i20 %s
  %p3.next = getelementptr inbounds i8, ptr %p3, i20 %s
  %p4.next = getelementptr inbounds i8, ptr %p4, i20 %s
  %v0 = load i32, ptr %p0.next
  %v1 = load i32, ptr %p1.next
  %v2 = load i32, ptr %p2.next
  %v3 = load i32, ptr %p3.next
  %v4 = load i32, ptr %p4.next
  %s01 = add i32 %v0, %v1
  %s23 = add i32 %v2, %v3
  %s0123 = add i32 %s01, %s23
  %sum = add i32 %s0123, %v4
  %acc.next = add i32 %acc, %sum
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %inner, label %outer.latch

outer.latch:
  store i32 %acc.next, ptr %out
  %base.next = getelementptr inbounds i8, ptr %base, i20 64
  %j.dec = add i32 %j, -1
  %outer.cond = icmp ne i32 %j.dec, 0
  br i1 %outer.cond, label %outer, label %exit

exit:
  ret void
}

; The inner loop is guarded and may not run in a given outer iteration.
; CHECK-LABEL: define void @inner_loop_may_be_skipped(
; CHECK:         %ptr.shift = add i20 192, %s
; CHECK:       inner.ph:
; CHECK:         %ptr.shifted.init = getelementptr i8, ptr %base, i20 %ptr.shift
; CHECK:         %v = load i32, ptr %ptr
; CHECK:         %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 %s

define void @inner_loop_may_be_skipped(ptr %a, ptr %out, i32 %stride, i32 %n, i32 %m) {
entry:
  %s = trunc i32 %stride to i20
  %skip = icmp eq i32 %n, 0
  br label %outer

outer:
  %base = phi ptr [ %a, %entry ], [ %base.next, %outer.latch ]
  %j = phi i32 [ %m, %entry ], [ %j.dec, %outer.latch ]
  %init = getelementptr inbounds i8, ptr %base, i20 192
  br i1 %skip, label %outer.latch, label %inner.ph

inner.ph:
  br label %inner

inner:
  %ptr = phi ptr [ %init, %inner.ph ], [ %ptr.next, %inner ]
  %acc = phi i32 [ 0, %inner.ph ], [ %acc.next, %inner ]
  %i = phi i32 [ %n, %inner.ph ], [ %i.dec, %inner ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
  %v = load i32, ptr %ptr.next
  %acc.next = add i32 %acc, %v
  %i.dec = add i32 %i, -1
  %cond = icmp ne i32 %i.dec, 0
  br i1 %cond, label %inner, label %outer.latch

outer.latch:
  %res = phi i32 [ 0, %outer ], [ %acc.next, %inner ]
  store i32 %res, ptr %out
  %base.next = getelementptr inbounds i8, ptr %base, i20 64
  %j.dec = add i32 %j, -1
  %outer.cond = icmp ne i32 %j.dec, 0
  br i1 %outer.cond, label %outer, label %exit

exit:
  ret void
}

; The back-edge pointer also feeds the exit compare.
; CHECK-LABEL: define void @exit_on_pointer_compare(
; CHECK:         %ptr.shifted.init = getelementptr i8, ptr %p, i20 %s
; CHECK:         %ptr = phi ptr [ %ptr.shifted.init, %entry ], [ %ptr.back, %loop ]
; CHECK:         %v = load i32, ptr %ptr
; CHECK:         %done = icmp eq ptr %ptr, %end
; CHECK:         %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 %s

define void @exit_on_pointer_compare(ptr %p, ptr %end, ptr %out, i32 %stride) {
entry:
  %s = trunc i32 %stride to i20
  br label %loop

loop:
  %ptr = phi ptr [ %p, %entry ], [ %ptr.next, %loop ]
  %acc = phi i32 [ 0, %entry ], [ %acc.next, %loop ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 %s
  %v = load i32, ptr %ptr.next
  %acc.next = add i32 %acc, %v
  %done = icmp eq ptr %ptr.next, %end
  br i1 %done, label %exit, label %loop

exit:
  store i32 %acc.next, ptr %out
  ret void
}

; Like olp_second_pointer_from_first, with two pointers derived from the
; first one, at +192 and +256.
; CHECK-LABEL: define void @olp_two_pointers_from_first(
; CHECK:         %q.shift = add i20 192, %stride
; CHECK-NOT:     add i20
; CHECK:         %q.shifted.init = getelementptr i8, ptr %pa.flat, i20 %q.shift
; CHECK:         %r.shifted.init = getelementptr i8, ptr %pa3.init, i20 %stride
; CHECK:         %y3 = load <32 x i16>, ptr addrspace(5) %q.next.as
; CHECK:         %y4 = load <32 x i16>, ptr addrspace(5) %r.next.as

define void @olp_two_pointers_from_first(ptr addrspace(5) %a, i20 %stride, i20 %d1, i20 %d2, <32 x i16> %w, i32 %m, i32 %n) {
entry:
  br label %outer

outer:
  %j = phi i32 [ 0, %entry ], [ %j.next, %outer.latch ]
  %pa = phi ptr addrspace(5) [ %a, %entry ], [ %pa.inner.next, %outer.latch ]
  %pa.flat = addrspacecast ptr addrspace(5) %pa to ptr
  %pa2.init = getelementptr i8, ptr %pa.flat, i20 192
  %pa2.init.as = addrspacecast ptr %pa2.init to ptr addrspace(5)
  %pa3.init = getelementptr i8, ptr %pa.flat, i20 256
  %pa3.init.as = addrspacecast ptr %pa3.init to ptr addrspace(5)
  %x0 = load <32 x i16>, ptr addrspace(5) %pa, align 64
  %pa.128 = getelementptr i8, ptr %pa.flat, i20 128
  %pa.128.as = addrspacecast ptr %pa.128 to ptr addrspace(5)
  %x2 = load <32 x i16>, ptr addrspace(5) %pa.128.as, align 64
  %pa.3d = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr %pa.128, i20 0, i20 %d1, i20 0, i20 %d2, i20 0, i20 0, i20 0)
  %x3 = load <32 x i16>, ptr addrspace(5) %pa2.init.as, align 64
  %x4 = load <32 x i16>, ptr addrspace(5) %pa3.init.as, align 64
  %acc0 = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.addmac.conf(<32 x i16> %x0, <32 x i16> %w, <64 x i32> zeroinitializer, <64 x i32> zeroinitializer, i32 0)
  %acc2 = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.addmac.conf(<32 x i16> %x2, <32 x i16> %w, <64 x i32> zeroinitializer, <64 x i32> zeroinitializer, i32 0)
  %acc3 = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.addmac.conf(<32 x i16> %x3, <32 x i16> %x4, <64 x i32> zeroinitializer, <64 x i32> zeroinitializer, i32 0)
  %pa.next.flat = extractvalue { ptr, i20, i20 } %pa.3d, 0
  %pa.next = addrspacecast ptr %pa.next.flat to ptr addrspace(5)
  br label %inner

inner:
  %pa.i = phi ptr addrspace(5) [ %pa.next, %outer ], [ %pa.inner.next, %inner ]
  %i = phi i32 [ 0, %outer ], [ %i.next, %inner ]
  %q = phi ptr [ %pa2.init, %outer ], [ %q.next, %inner ]
  %r = phi ptr [ %pa3.init, %outer ], [ %r.next, %inner ]
  %c3 = phi <64 x i32> [ %acc3, %outer ], [ %c3.next, %inner ]
  %c2 = phi <64 x i32> [ %acc2, %outer ], [ %c2.next, %inner ]
  %c0 = phi <64 x i32> [ %acc0, %outer ], [ %c0.next, %inner ]
  %q.next = getelementptr i8, ptr %q, i20 %stride
  %q.next.as = addrspacecast ptr %q.next to ptr addrspace(5)
  %r.next = getelementptr i8, ptr %r, i20 %stride
  %r.next.as = addrspacecast ptr %r.next to ptr addrspace(5)
  %y0 = load <32 x i16>, ptr addrspace(5) %pa.i, align 64
  %pa.i.flat = addrspacecast ptr addrspace(5) %pa.i to ptr
  %pa.i.3d = tail call { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr %pa.i.flat, i20 0, i20 %d1, i20 0, i20 %d2, i20 0, i20 0, i20 0)
  %y3 = load <32 x i16>, ptr addrspace(5) %q.next.as, align 64
  %y4 = load <32 x i16>, ptr addrspace(5) %r.next.as, align 64
  %c0.next = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.mac.conf(<32 x i16> %y0, <32 x i16> %w, <64 x i32> %c0, i32 0)
  %c2.next = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.mac.conf(<32 x i16> %y3, <32 x i16> %w, <64 x i32> %c2, i32 0)
  %c3.next = tail call <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.mac.conf(<32 x i16> %y4, <32 x i16> %w, <64 x i32> %c3, i32 0)
  %i.next = add i32 %i, 1
  %pa.i.next.flat = extractvalue { ptr, i20, i20 } %pa.i.3d, 0
  %pa.inner.next = addrspacecast ptr %pa.i.next.flat to ptr addrspace(5)
  %inner.done = icmp eq i32 %i, %n
  br i1 %inner.done, label %outer.latch, label %inner

outer.latch:
  %r0 = tail call <64 x i8> @llvm.aie2ps.I512.v64.acc32.srs(<64 x i32> %c0.next, i32 0, i32 0)
  %r2 = tail call <64 x i8> @llvm.aie2ps.I512.v64.acc32.srs(<64 x i32> %c2.next, i32 0, i32 0)
  %r3 = tail call <64 x i8> @llvm.aie2ps.I512.v64.acc32.srs(<64 x i32> %c3.next, i32 0, i32 0)
  %j.next = add i32 %j, 1
  %outer.done = icmp eq i32 %j.next, %m
  br i1 %outer.done, label %exit, label %outer, !llvm.loop !4

exit:
  ret void
}

declare { ptr, i20, i20 } @llvm.aie2ps.add.3d(ptr, i20, i20, i20, i20, i20, i20, i20)
declare <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.addmac.conf(<32 x i16>, <32 x i16>, <64 x i32>, <64 x i32>, i32)
declare <64 x i32> @llvm.aie2ps.I512.I512.ACC2048.mac.conf(<32 x i16>, <32 x i16>, <64 x i32>, i32)
declare <64 x i8> @llvm.aie2ps.I512.v64.acc32.srs(<64 x i32>, i32, i32)

!0 = distinct !{!0, !1, !2, !3}
!1 = !{!"llvm.loop.mustprogress"}
!2 = !{!"llvm.loop.itercount.range", i64 2}
!3 = !{!"llvm.loop.hint.aie-enable-outer-loop-pipelining", i64 1}
!4 = distinct !{!4, !1, !2, !3}
