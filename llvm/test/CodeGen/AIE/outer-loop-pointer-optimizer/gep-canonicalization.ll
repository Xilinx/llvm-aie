; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=true \
; RUN:     -aie-enable-gep-chain-linking=false \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -stop-after=aie-outer-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s

; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=false \
; RUN:     -stop-after=aie-outer-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s --check-prefix=DISABLED

; Test GEP canonicalization: converts non-i8 GEPs to i8-based GEPs.
;
; The pass converts:
;   getelementptr <32 x bfloat>, ptr %p, i20 %idx
; To:
;   %byte_offset = mul i20 %idx, 64  ; 64 = sizeof(<32 x bfloat>)
;   getelementptr i8, ptr %p, i20 %byte_offset

; ============================================================================
; Test 1: Basic GEP canonicalization with <32 x bfloat> (64 bytes)
;
; Expected: GEP with <32 x bfloat> is converted to i8-based GEP with mul
; ============================================================================

; CHECK-LABEL: define void @test_gep_canonicalization_v32bf16
; The mul should be hoisted to preheader since index is loop-invariant
; CHECK: top.preheader:
; CHECK:   %byte_offset = mul i20 %offset, 64
; CHECK: top:
; CHECK:   getelementptr inbounds i8, ptr %base, i20 %byte_offset

; DISABLED-LABEL: define void @test_gep_canonicalization_v32bf16
; DISABLED: top:
; DISABLED:   getelementptr inbounds <32 x bfloat>, ptr %base, i20 %offset

define void @test_gep_canonicalization_v32bf16(ptr noalias %base, ptr noalias %out,
                                                i32 %N, i32 %M, i20 %offset) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr [ %base, %entry ], [ %ptr.next, %bottom ]
  ; GEP with non-i8 type - should be canonicalized
  %ptr.next = getelementptr inbounds <32 x bfloat>, ptr %base, i20 %offset
  %v = load <32 x bfloat>, ptr %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = fadd <32 x bfloat> %acc, %v
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
; Test 2: GEP canonicalization with i32 element type (4 bytes)
;
; Expected: GEP with i32 is converted to i8-based GEP with mul by 4
; ============================================================================

; CHECK-LABEL: define void @test_gep_canonicalization_i32
; The mul should be hoisted to preheader since index is loop-invariant
; CHECK: top.preheader:
; CHECK:   %byte_offset = mul i20 %offset, 4
; CHECK: top:
; CHECK:   getelementptr inbounds i8, ptr %base, i20 %byte_offset

define void @test_gep_canonicalization_i32(ptr noalias %base, ptr noalias %out,
                                            i32 %N, i32 %M, i20 %offset) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr [ %base, %entry ], [ %ptr.next, %bottom ]
  ; GEP with i32 type - should be canonicalized with mul by 4
  %ptr.next = getelementptr inbounds i32, ptr %base, i20 %offset
  %v = load i32, ptr %ptr, align 4
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi i32 [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = add i32 %acc, %v
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store i32 %acc.next, ptr %out, align 4
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 3: GEP already i8-based should NOT be modified
;
; Expected: i8 GEP passes through unchanged
; ============================================================================

; CHECK-LABEL: define void @test_gep_already_i8
; CHECK: top:
; CHECK:   %ptr.next = getelementptr inbounds i8, ptr %base, i20 %offset
; CHECK-NOT: mul

define void @test_gep_already_i8(ptr noalias %base, ptr noalias %out,
                                  i32 %N, i32 %M, i20 %offset) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr [ %base, %entry ], [ %ptr.next, %bottom ]
  ; Already i8-based GEP - should NOT be modified
  %ptr.next = getelementptr inbounds i8, ptr %base, i20 %offset
  %v = load i8, ptr %ptr, align 1
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi i8 [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = add i8 %acc, %v
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store i8 %acc.next, ptr %out, align 1
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 4: Multiple GEPs with same index and type - CSE of mul
;
; Expected: Only one mul instruction should be generated (hoisted to preheader)
; ============================================================================

; CHECK-LABEL: define void @test_gep_cse_same_index
; Only ONE mul in preheader (CSE)
; CHECK: top.preheader:
; CHECK:   %byte_offset = mul i20 %offset, 64
; CHECK: top:
; Both GEPs use the same byte_offset
; CHECK:   getelementptr inbounds i8, ptr %base1, i20 %byte_offset
; CHECK:   getelementptr inbounds i8, ptr %base2, i20 %byte_offset
; CHECK-NOT: mul i20 %offset, 64

define void @test_gep_cse_same_index(ptr noalias %base1, ptr noalias %base2,
                                      ptr noalias %out, i32 %N, i32 %M, i20 %offset) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr1 = phi ptr [ %base1, %entry ], [ %ptr1.next, %bottom ]
  %ptr2 = phi ptr [ %base2, %entry ], [ %ptr2.next, %bottom ]
  ; Two GEPs with same index and type - mul should be CSE'd
  %ptr1.next = getelementptr inbounds <32 x bfloat>, ptr %base1, i20 %offset
  %ptr2.next = getelementptr inbounds <32 x bfloat>, ptr %base2, i20 %offset
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
; Test 5: GEP in bottom block with non-loop-invariant index
;
; Expected: mul should stay local (not hoisted to preheader)
; ============================================================================

; CHECK-LABEL: define void @test_gep_non_loop_invariant
; CHECK: bottom:
; Non-loop-invariant index, mul stays in bottom block
; CHECK:   %byte_offset = mul i32 %acc.next.i32, 64
; CHECK:   getelementptr inbounds i8, ptr %base, i32 %byte_offset

define void @test_gep_non_loop_invariant(ptr noalias %base, ptr noalias %out,
                                          i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr [ %base, %entry ], [ %ptr.next, %bottom ]
  %v = load <32 x bfloat>, ptr %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi i32 [ 0, %top ], [ %acc.next.i32, %inner ]
  %acc.next.i32 = add i32 %acc, 1
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x bfloat> %v, ptr %out, align 64
  ; GEP with non-loop-invariant index (depends on inner loop result)
  %ptr.next = getelementptr inbounds <32 x bfloat>, ptr %base, i32 %acc.next.i32
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
