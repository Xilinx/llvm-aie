; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=true \
; RUN:     -stop-after=aie-outer-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test loop structure detection: validates the expected loop pattern
;
; Required structure:
;   preheader -> top (prologue) -> inner (single block) -> bottom (epilogue)
;
; Pass should ONLY apply to loops matching this pattern.

; ============================================================================
; Test 1: Valid loop structure - should be optimized
;
; Expected: GEPs are canonicalized because loop structure matches
; ============================================================================

; CHECK-LABEL: define void @test_valid_loop_structure
; CHECK: top.preheader:
; mul hoisted to preheader - confirms optimization applied
; CHECK:   %byte_offset = mul i20 %offset, 64
; CHECK: top:
; GEP canonicalized to i8-based
; CHECK:   getelementptr inbounds i8, ptr %base, i20 %byte_offset

define void @test_valid_loop_structure(ptr noalias %base, ptr noalias %out,
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
; Test 2: Simple loop without inner loop - should NOT be optimized
;
; Expected: GEP stays as-is because there's no matching loop structure
; ============================================================================

; CHECK-LABEL: define void @test_simple_loop_no_inner
; No optimization - GEP remains with original type
; CHECK: loop:
; CHECK:   getelementptr inbounds <32 x bfloat>, ptr %base, i20 %offset
; CHECK-NOT: mul{{.*}}64

define void @test_simple_loop_no_inner(ptr noalias %base, ptr noalias %out,
                                        i32 %N, i20 %offset) {
entry:
  %cmp = icmp sgt i32 %N, 0
  br i1 %cmp, label %loop, label %exit

loop:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %loop ]
  %ptr = phi ptr [ %base, %entry ], [ %ptr.next, %loop ]
  ; No inner loop - should NOT be optimized
  %ptr.next = getelementptr inbounds <32 x bfloat>, ptr %base, i20 %offset
  %v = load <32 x bfloat>, ptr %ptr, align 64
  store <32 x bfloat> %v, ptr %out, align 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %loop

exit:
  ret void
}

; ============================================================================
; Test 3: Multi-block inner loop - should NOT be optimized
;
; Expected: GEP stays as-is because inner loop is not single-block
; ============================================================================

; CHECK-LABEL: define void @test_multiblock_inner
; No optimization - GEP remains with original type
; CHECK: top:
; CHECK:   getelementptr inbounds <32 x bfloat>, ptr %base, i20 %offset
; CHECK-NOT: mul{{.*}}64

define void @test_multiblock_inner(ptr noalias %base, ptr noalias %out,
                                    i32 %N, i32 %M, i20 %offset) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr [ %base, %entry ], [ %ptr.next, %bottom ]
  ; GEP should NOT be canonicalized - multi-block inner
  %ptr.next = getelementptr inbounds <32 x bfloat>, ptr %base, i20 %offset
  %v = load <32 x bfloat>, ptr %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner.header

inner.header:
  %acc = phi <32 x bfloat> [ %v, %top ], [ %acc.next, %inner.body ]
  %inner.iv = phi i32 [ %M, %top ], [ %inner.iv.next, %inner.body ]
  %check = icmp sgt i32 %inner.iv, 0
  br i1 %check, label %inner.body, label %bottom

inner.body:
  ; Multi-block inner loop
  %acc.next = fadd <32 x bfloat> %acc, %v
  %inner.iv.next = add i32 %inner.iv, -1
  br label %inner.header

bottom:
  store <32 x bfloat> %acc, ptr %out, align 64
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
