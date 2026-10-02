; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=true \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -stop-after=aie-outer-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test GEP chain linking: creates chains of GEPs for post-increment addressing.
;
; The pass converts:
;   %ptr64 = getelementptr i8, ptr %base, i20 64
;   %ptr128 = getelementptr i8, ptr %base, i20 128
;   %ptr192 = getelementptr i8, ptr %base, i20 192
; To:
;   %ptr64 = getelementptr i8, ptr %base, i20 64
;   %ptr128 = getelementptr i8, ptr %ptr64, i20 64   ; delta
;   %ptr192 = getelementptr i8, ptr %ptr128, i20 64  ; delta

; ============================================================================
; Test 1: Basic GEP chain linking with same base pointer
;
; Expected: GEPs with same base are chained with delta offsets
; ============================================================================

; CHECK-LABEL: define void @test_gep_chain_basic
; CHECK: top:
; First GEP uses base
; CHECK:   %ptr64 = getelementptr inbounds i8, ptr %base, i20 64
; Second GEP chains off first (delta = 128-64 = 64)
; CHECK:   %ptr128.chained = getelementptr inbounds i8, ptr %ptr64, i20 64
; Third GEP chains off second (delta = 192-128 = 64)
; CHECK:   %ptr192.chained = getelementptr inbounds i8, ptr %ptr128.chained, i20 64

define void @test_gep_chain_basic(ptr noalias %base, ptr noalias %out,
                                   i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  ; Three GEPs with same base and increasing offsets
  %ptr64 = getelementptr inbounds i8, ptr %base, i20 64
  %ptr128 = getelementptr inbounds i8, ptr %base, i20 128
  %ptr192 = getelementptr inbounds i8, ptr %base, i20 192
  %v1 = load <32 x bfloat>, ptr %ptr64, align 64
  %v2 = load <32 x bfloat>, ptr %ptr128, align 64
  %v3 = load <32 x bfloat>, ptr %ptr192, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %v1, %top ], [ %acc.next, %inner ]
  %sum1 = fadd <32 x bfloat> %acc, %v2
  %acc.next = fadd <32 x bfloat> %sum1, %v3
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x bfloat> %acc.next, ptr %out, align 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 2: GEP chain with non-uniform deltas
;
; Expected: Chain respects different delta values
; ============================================================================

; CHECK-LABEL: define void @test_gep_chain_nonuniform
; CHECK: top:
; CHECK:   %ptr64 = getelementptr inbounds i8, ptr %base, i20 64
; delta = 128-64 = 64
; CHECK:   %ptr128.chained = getelementptr inbounds i8, ptr %ptr64, i20 64
; delta = 320-128 = 192
; CHECK:   %ptr320.chained = getelementptr inbounds i8, ptr %ptr128.chained, i20 192

define void @test_gep_chain_nonuniform(ptr noalias %base, ptr noalias %out,
                                        i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  ; GEPs with non-uniform offsets
  %ptr64 = getelementptr inbounds i8, ptr %base, i20 64
  %ptr128 = getelementptr inbounds i8, ptr %base, i20 128
  %ptr320 = getelementptr inbounds i8, ptr %base, i20 320
  %v1 = load <32 x bfloat>, ptr %ptr64, align 64
  %v2 = load <32 x bfloat>, ptr %ptr128, align 64
  %v3 = load <32 x bfloat>, ptr %ptr320, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %v1, %top ], [ %acc.next, %inner ]
  %sum1 = fadd <32 x bfloat> %acc, %v2
  %acc.next = fadd <32 x bfloat> %sum1, %v3
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x bfloat> %acc.next, ptr %out, align 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 3: GEP chain should NOT link non-constant index GEPs
;
; Expected: GEPs with variable indices are not chained
; ============================================================================

; CHECK-LABEL: define void @test_gep_no_chain_variable_idx
; CHECK: top:
; GEPs with variable indices should NOT be chained
; CHECK:   %ptr1 = getelementptr inbounds i8, ptr %base, i20 %off1
; CHECK:   %ptr2 = getelementptr inbounds i8, ptr %base, i20 %off2

define void @test_gep_no_chain_variable_idx(ptr noalias %base, ptr noalias %out,
                                             i32 %N, i32 %M, i20 %off1, i20 %off2) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  ; GEPs with variable indices - should NOT be chained
  %ptr1 = getelementptr inbounds i8, ptr %base, i20 %off1
  %ptr2 = getelementptr inbounds i8, ptr %base, i20 %off2
  %v1 = load <32 x bfloat>, ptr %ptr1, align 64
  %v2 = load <32 x bfloat>, ptr %ptr2, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %v1, %top ], [ %acc.next, %inner ]
  %acc.next = fadd <32 x bfloat> %acc, %v2
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x bfloat> %acc.next, ptr %out, align 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 4: GEP chain should NOT link zero-offset GEPs
;
; Expected: Zero offset GEP is not linked
; ============================================================================

; CHECK-LABEL: define void @test_gep_no_chain_zero_offset
; CHECK: top:
; Zero offset is optimized away or converted to bitcast
; Non-zero offset - starts chain (not linked since no previous chain element)
; CHECK:   %ptr64 = getelementptr inbounds i8, ptr %base, i20 64

define void @test_gep_no_chain_zero_offset(ptr noalias %base, ptr noalias %out,
                                            i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  ; Zero offset GEP - should be skipped
  %ptr0 = getelementptr inbounds i8, ptr %base, i20 0
  %ptr64 = getelementptr inbounds i8, ptr %base, i20 64
  %v1 = load <32 x bfloat>, ptr %ptr0, align 64
  %v2 = load <32 x bfloat>, ptr %ptr64, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %v1, %top ], [ %acc.next, %inner ]
  %acc.next = fadd <32 x bfloat> %acc, %v2
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x bfloat> %acc.next, ptr %out, align 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 5: GEP chain with PHI base (starts new chain)
;
; Expected: PHI base starts a new chain
; ============================================================================

; CHECK-LABEL: define void @test_gep_chain_phi_base
; CHECK: top:
; PHI uses the chained GEP from bottom
; CHECK:   %ptr.phi = phi ptr [ %ptr.next.chained, %bottom ], [ %base, %top.preheader ]
; PHI base starts chain
; CHECK:   %ptr64 = getelementptr inbounds i8, ptr %ptr.phi, i20 64
; Chains off previous (delta = 128-64 = 64)
; CHECK:   %ptr128.chained = getelementptr inbounds i8, ptr %ptr64, i20 64
; CHECK: bottom:
; GEP in bottom is also chained (delta = 192-128 = 64)
; CHECK:   %ptr.next.chained = getelementptr inbounds i8, ptr %ptr128.chained, i20 64

define void @test_gep_chain_phi_base(ptr noalias %base, ptr noalias %out,
                                      i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr.phi = phi ptr [ %base, %entry ], [ %ptr.next, %bottom ]
  ; GEPs based on PHI - should form a chain
  %ptr64 = getelementptr inbounds i8, ptr %ptr.phi, i20 64
  %ptr128 = getelementptr inbounds i8, ptr %ptr.phi, i20 128
  %v1 = load <32 x bfloat>, ptr %ptr64, align 64
  %v2 = load <32 x bfloat>, ptr %ptr128, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %v1, %top ], [ %acc.next, %inner ]
  %acc.next = fadd <32 x bfloat> %acc, %v2
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x bfloat> %acc.next, ptr %out, align 64
  %ptr.next = getelementptr inbounds i8, ptr %ptr.phi, i20 192
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 6: Chain epilogue GEPs from final inner-loop pointer values
;
; Expected: address recomputations equal to the inner-loop exit pointers are
; removed, and later epilogue addresses become +128 chains.
; ============================================================================

; CHECK-LABEL: define i8 @test_chain_from_inner_exit
; The outer pointer PHIs must carry the rebased epilogue pointers.
; CHECK: top:
; CHECK:   %a.outer = phi ptr [ %a, %entry ], [ %a.next.inner.chained, %bottom ]
; CHECK:   %b.outer = phi ptr [ %b, %entry ], [ %b.next2.inner.chained, %bottom ]
; CHECK: inner:
; CHECK:   %a.step = getelementptr inbounds i8, ptr %a.inner, i20 128
; CHECK:   %b.step = getelementptr inbounds i8, ptr %b.inner, i20 128
; CHECK: bottom:
; CHECK-NOT: %a.at.exit =
; CHECK-NOT: %b.at.exit =
; CHECK:   %a.value = load i8, ptr %a.step
; CHECK:   %a.next.inner.chained = getelementptr inbounds i8, ptr %a.step, i20 128
; CHECK:   %b.value0 = load i8, ptr %b.step
; CHECK:   %b.next1.inner.chained = getelementptr inbounds i8, ptr %b.step, i20 128
; CHECK:   %b.value1 = load i8, ptr %b.next1.inner.chained
; CHECK:   %b.next2.inner.chained = getelementptr inbounds i8, ptr %b.next1.inner.chained, i20 128
; CHECK:   %b.value2 = load i8, ptr %b.next2.inner.chained

define i8 @test_chain_from_inner_exit(ptr %a, ptr %b, i32 %N) {
entry:
  br label %top

top:
  %outer.iv = phi i32 [ 0, %entry ], [ %outer.iv.next, %bottom ]
  %a.outer = phi ptr [ %a, %entry ], [ %a.next, %bottom ]
  %b.outer = phi ptr [ %b, %entry ], [ %b.next2, %bottom ]
  %a.start = getelementptr inbounds i8, ptr %a.outer, i20 256
  %b.start = getelementptr inbounds i8, ptr %b.outer, i20 128
  br label %inner

inner:
  %inner.iv = phi i32 [ 0, %top ], [ %inner.iv.next, %inner ]
  %a.inner = phi ptr [ %a.start, %top ], [ %a.step, %inner ]
  %b.inner = phi ptr [ %b.start, %top ], [ %b.step, %inner ]
  %a.inner.value = load volatile i8, ptr %a.inner
  %b.inner.value = load volatile i8, ptr %b.inner
  %a.step = getelementptr inbounds i8, ptr %a.inner, i20 128
  %b.step = getelementptr inbounds i8, ptr %b.inner, i20 128
  %inner.iv.next = add nuw nsw i32 %inner.iv, 1
  %inner.done = icmp eq i32 %inner.iv.next, 29
  br i1 %inner.done, label %bottom, label %inner

bottom:
  %a.at.exit = getelementptr inbounds i8, ptr %a.outer, i20 3968
  %a.value = load i8, ptr %a.at.exit
  %a.next = getelementptr inbounds i8, ptr %a.outer, i20 4096
  %b.at.exit = getelementptr inbounds i8, ptr %b.inner, i20 128
  %b.value0 = load i8, ptr %b.at.exit
  %b.next1 = getelementptr inbounds i8, ptr %b.inner, i20 256
  %b.value1 = load i8, ptr %b.next1
  %b.next2 = getelementptr inbounds i8, ptr %b.inner, i20 384
  %b.value2 = load i8, ptr %b.next2
  %sum0 = add i8 %a.value, %b.value0
  %sum1 = add i8 %sum0, %b.value1
  %sum2 = add i8 %sum1, %b.value2
  %outer.iv.next = add nuw nsw i32 %outer.iv, 1
  %outer.done = icmp eq i32 %outer.iv.next, %N
  br i1 %outer.done, label %exit, label %top

exit:
  ret i8 %sum2
}

; ============================================================================
; Test 7: Same-base inner exit pointers rebase onto the nearest anchor
;
; p and q step through one buffer, with q 128 bytes ahead. After 2 iterations
; p.step is base+256 and q.step is base+384. base+384 matches both; the nearer
; anchor is q.step. base+512 chains +128 from that same anchor.
; ============================================================================

; CHECK-LABEL: define i8 @test_chain_from_nearest_same_base
; CHECK: inner:
; CHECK:   %p.step = getelementptr inbounds i8, ptr %p.inner, i20 128
; CHECK:   %q.step = getelementptr inbounds i8, ptr %q.inner, i20 128
; CHECK: bottom:
; CHECK-NOT: %at.p =
; CHECK-NOT: %at.q =
; CHECK:   %p.value = load i8, ptr %p.step
; CHECK:   %q.value = load i8, ptr %q.step
; CHECK:   %past.q.inner.chained = getelementptr inbounds i8, ptr %q.step, i20 128
; CHECK:   %past.value = load i8, ptr %past.q.inner.chained

define i8 @test_chain_from_nearest_same_base(ptr %base, i32 %N) {
entry:
  br label %top

top:
  %outer.iv = phi i32 [ 0, %entry ], [ %outer.iv.next, %bottom ]
  %q.start = getelementptr inbounds i8, ptr %base, i20 128
  br label %inner

inner:
  %inner.iv = phi i32 [ 0, %top ], [ %inner.iv.next, %inner ]
  %p.inner = phi ptr [ %base, %top ], [ %p.step, %inner ]
  %q.inner = phi ptr [ %q.start, %top ], [ %q.step, %inner ]
  %p.inner.value = load volatile i8, ptr %p.inner
  %q.inner.value = load volatile i8, ptr %q.inner
  %p.step = getelementptr inbounds i8, ptr %p.inner, i20 128
  %q.step = getelementptr inbounds i8, ptr %q.inner, i20 128
  %inner.iv.next = add nuw nsw i32 %inner.iv, 1
  %inner.done = icmp eq i32 %inner.iv.next, 2
  br i1 %inner.done, label %bottom, label %inner

bottom:
  %at.p = getelementptr inbounds i8, ptr %base, i20 256
  %p.value = load i8, ptr %at.p
  %at.q = getelementptr inbounds i8, ptr %base, i20 384
  %q.value = load i8, ptr %at.q
  %past.q = getelementptr inbounds i8, ptr %base, i20 512
  %past.value = load i8, ptr %past.q
  %sum0 = add i8 %p.value, %q.value
  %sum1 = add i8 %sum0, %past.value
  %outer.iv.next = add nuw nsw i32 %outer.iv, 1
  %outer.done = icmp eq i32 %outer.iv.next, %N
  br i1 %outer.done, label %exit, label %top

exit:
  ret i8 %sum1
}

; ============================================================================
; Test 8: A later, smaller epilogue offset still rebases from the anchor
;
; After 2 iterations p.step is base+256. The epilogue mentions base+512 before
; base+384, so the offsets go 256 then 128. The smaller GEP cannot chain from
; the larger one; both rebase from p.step.
; ============================================================================

; CHECK-LABEL: define i8 @test_chain_from_out_of_order_inner_exit
; CHECK: inner:
; CHECK:   %p.step = getelementptr inbounds i8, ptr %p.inner, i20 128
; CHECK: bottom:
; An earlier pass may rename the first epilogue GEP before this one.
; CHECK:   %[[FAR_PTR:.*]] = getelementptr inbounds i8, ptr %p.step, i20 256
; CHECK:   %far.value = load i8, ptr %[[FAR_PTR]]
; CHECK:   %near.inner.chained = getelementptr inbounds i8, ptr %p.step, i20 128
; CHECK:   %near.value = load i8, ptr %near.inner.chained

define i8 @test_chain_from_out_of_order_inner_exit(ptr %base, i32 %N) {
entry:
  br label %top

top:
  %outer.iv = phi i32 [ 0, %entry ], [ %outer.iv.next, %bottom ]
  br label %inner

inner:
  %inner.iv = phi i32 [ 0, %top ], [ %inner.iv.next, %inner ]
  %p.inner = phi ptr [ %base, %top ], [ %p.step, %inner ]
  %p.inner.value = load volatile i8, ptr %p.inner
  %p.step = getelementptr inbounds i8, ptr %p.inner, i20 128
  %inner.iv.next = add nuw nsw i32 %inner.iv, 1
  %inner.done = icmp eq i32 %inner.iv.next, 2
  br i1 %inner.done, label %bottom, label %inner

bottom:
  %far = getelementptr inbounds i8, ptr %base, i20 512
  %far.value = load i8, ptr %far
  %near = getelementptr inbounds i8, ptr %base, i20 384
  %near.value = load i8, ptr %near
  %sum = add i8 %far.value, %near.value
  %outer.iv.next = add nuw nsw i32 %outer.iv, 1
  %outer.done = icmp eq i32 %outer.iv.next, %N
  br i1 %outer.done, label %exit, label %top

exit:
  ret i8 %sum
}

; ============================================================================
; Test 9: One inner iteration uses the backedge step, not the PHI
;
; The loop body runs once, so the PHI still holds %ptr.outer and %p.step is
; %ptr.outer+128. The epilogue address %ptr.outer+128 must be %p.step itself.
; Using the PHI as the exit value would produce getelementptr %p.inner, 128.
; %ptr.outer+256 is then +128 from %p.step, and that pointer feeds %ptr.outer.
; ============================================================================

; CHECK-LABEL: define i8 @test_chain_from_one_inner_iteration
; CHECK: top:
; CHECK:   %ptr.outer = phi ptr [ %base, %entry ], [ %ptr.next.inner.chained, %bottom ]
; CHECK: inner:
; CHECK:   %p.inner = phi ptr [ %ptr.outer, %top ], [ %p.step, %inner ]
; CHECK:   %p.step = getelementptr inbounds i8, ptr %p.inner, i20 128
; CHECK: bottom:
; CHECK:   %value = load i8, ptr %p.step
; CHECK:   %ptr.next.inner.chained = getelementptr inbounds i8, ptr %p.step, i20 128

define i8 @test_chain_from_one_inner_iteration(ptr %base, i32 %N) {
entry:
  br label %top

top:
  %outer.iv = phi i32 [ 0, %entry ], [ %outer.iv.next, %bottom ]
  %ptr.outer = phi ptr [ %base, %entry ], [ %ptr.next, %bottom ]
  br label %inner

inner:
  %inner.iv = phi i32 [ 0, %top ], [ %inner.iv.next, %inner ]
  %p.inner = phi ptr [ %ptr.outer, %top ], [ %p.step, %inner ]
  %p.inner.value = load volatile i8, ptr %p.inner
  %p.step = getelementptr inbounds i8, ptr %p.inner, i20 128
  %inner.iv.next = add nuw nsw i32 %inner.iv, 1
  %inner.done = icmp eq i32 %inner.iv.next, 1
  br i1 %inner.done, label %bottom, label %inner

bottom:
  %at.exit = getelementptr inbounds i8, ptr %ptr.outer, i20 128
  %value = load i8, ptr %at.exit
  %ptr.next = getelementptr inbounds i8, ptr %ptr.outer, i20 256
  %outer.iv.next = add nuw nsw i32 %outer.iv, 1
  %outer.done = icmp eq i32 %outer.iv.next, %N
  br i1 %outer.done, label %exit, label %top

exit:
  ret i8 %value
}

; ============================================================================
; Test 10: Unknown inner-loop trip count does not rebase epilogue GEPs
;
; %K is not a constant, so the final %ptr.step is not a known offset from
; %ptr.outer. The volatile load keeps that step live. Chain linking may still
; attach the epilogue to %ptr.start (3968 - 256 = 3712), but not to %ptr.step.
; ============================================================================

; CHECK-LABEL: define i8 @test_no_chain_from_unknown_inner_exit
; CHECK: inner:
; CHECK:   %ptr.step = getelementptr inbounds i8, ptr %ptr.inner, i20 128
; CHECK: bottom:
; CHECK-NOT: %ptr.step
; CHECK:   %at.constant.offset.chained = getelementptr inbounds i8, ptr %ptr.start, i20 3712
; CHECK-NOT: %ptr.step
; CHECK:   %value = load i8, ptr %at.constant.offset.chained
; CHECK-NOT: %ptr.step
; CHECK:   %ptr.next.chained = getelementptr inbounds i8, ptr %at.constant.offset.chained, i20 128

define i8 @test_no_chain_from_unknown_inner_exit(ptr %base, i32 %N, i32 %K) {
entry:
  br label %top

top:
  %outer.iv = phi i32 [ 0, %entry ], [ %outer.iv.next, %bottom ]
  %ptr.outer = phi ptr [ %base, %entry ], [ %ptr.next, %bottom ]
  %ptr.start = getelementptr inbounds i8, ptr %ptr.outer, i20 256
  br label %inner

inner:
  %inner.iv = phi i32 [ 0, %top ], [ %inner.iv.next, %inner ]
  %ptr.inner = phi ptr [ %ptr.start, %top ], [ %ptr.step, %inner ]
  %ptr.inner.value = load volatile i8, ptr %ptr.inner
  %ptr.step = getelementptr inbounds i8, ptr %ptr.inner, i20 128
  %inner.iv.next = add nuw nsw i32 %inner.iv, 1
  %inner.done = icmp eq i32 %inner.iv.next, %K
  br i1 %inner.done, label %bottom, label %inner

bottom:
  %at.constant.offset = getelementptr inbounds i8, ptr %ptr.outer, i20 3968
  %value = load i8, ptr %at.constant.offset
  %ptr.next = getelementptr inbounds i8, ptr %ptr.outer, i20 4096
  %outer.iv.next = add nuw nsw i32 %outer.iv, 1
  %outer.done = icmp eq i32 %outer.iv.next, %N
  br i1 %outer.done, label %exit, label %top

exit:
  ret i8 %value
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
