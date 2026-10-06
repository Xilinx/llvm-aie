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

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
