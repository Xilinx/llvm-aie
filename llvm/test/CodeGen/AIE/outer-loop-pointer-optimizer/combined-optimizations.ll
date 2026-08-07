; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=true \
; RUN:     -aie-enable-gep-chain-linking=true \
; RUN:     -aie-enable-gep-hoisting=true \
; RUN:     -stop-after=aie-outer-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test all optimizations working together on a realistic multi-pointer loop pattern.
;
; This tests the interaction of:
; 1. GEP Canonicalization - converts typed GEPs to i8-based
; 2. GEP Chain Linking - chains consecutive GEPs for post-increment
; 3. GEP Hoisting - moves GEPs from bottom to top when safe

; ============================================================================
; Test 1: Combined canonicalization + chain linking
;
; Multiple GEPs with same base but different types should be:
; 1. First canonicalized to i8-based GEPs
; 2. Then chained together for post-increment
; ============================================================================

; CHECK-LABEL: define void @test_combined_canon_chain
; CHECK: top:
; First i8-based GEP (canonicalized from v32bf16 offset 1 = 64 bytes)
; Constant index is folded directly without mul
; CHECK:   getelementptr inbounds i8, ptr %base, i20 64
; Second GEP chained (offset 2*64=128, chained as delta 128-64=64)
; CHECK:   getelementptr inbounds i8, ptr %ptr64{{.*}}, i20 64

define void @test_combined_canon_chain(ptr noalias %base, ptr noalias %out,
                                        i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  ; Multiple GEPs with different typed indices - all should be canonicalized
  ; Offset 1 with <32 x bfloat> = 64 bytes
  %ptr64 = getelementptr inbounds <32 x bfloat>, ptr %base, i20 1
  ; Offset 2 with <32 x bfloat> = 128 bytes
  %ptr128 = getelementptr inbounds <32 x bfloat>, ptr %base, i20 2
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
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 2: Multi-pointer loop pattern with all three optimizations
;
; Simulates a typical compute outer loop with:
; - Multiple pointer increments in prologue and epilogue
; - Post-increment opportunities
; ============================================================================

; CHECK-LABEL: define void @test_multi_pointer_loop
; CHECK: top.preheader:
; CHECK:   br label %top
; CHECK: top:
; All pointer operations optimized
; CHECK:   %a.ptr.phi = phi ptr
; CHECK:   %b.ptr.phi = phi ptr
; CHECK:   %c.ptr.phi = phi ptr
; GEPs in top block (some hoisted from bottom)
; CHECK:   getelementptr
; CHECK:   getelementptr
; CHECK:   br label %inner

define void @test_multi_pointer_loop(ptr noalias %a, ptr noalias %b, ptr noalias %c,
                                i32 %M, i32 %N, i32 %K) {
entry:
  %cmp.outer = icmp sgt i32 %M, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %M, %entry ], [ %iv.next, %bottom ]
  %a.ptr.phi = phi ptr [ %a, %entry ], [ %a.ptr.next, %bottom ]
  %b.ptr.phi = phi ptr [ %b, %entry ], [ %b.ptr.next, %bottom ]
  %c.ptr.phi = phi ptr [ %c, %entry ], [ %c.ptr.next, %bottom ]
  
  ; Load from A at current position
  %a.val = load <32 x bfloat>, ptr %a.ptr.phi, align 64
  
  ; Calculate next A pointer (row stride)
  %a.ptr.row = getelementptr inbounds i8, ptr %a.ptr.phi, i20 256
  
  ; Load from B at current position
  %b.val = load <32 x bfloat>, ptr %b.ptr.phi, align 64
  
  call void @llvm.set.loop.iterations.i32(i32 %K)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %a.val, %top ], [ %acc.next, %inner ]
  %acc.next = fadd <32 x bfloat> %acc, %b.val
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Store result to C
  store <32 x bfloat> %acc.next, ptr %c.ptr.phi, align 64
  
  ; Advance pointers for next outer iteration
  %a.ptr.next = getelementptr inbounds i8, ptr %a.ptr.phi, i20 512
  %b.ptr.next = getelementptr inbounds i8, ptr %b.ptr.phi, i20 64
  %c.ptr.next = getelementptr inbounds i8, ptr %c.ptr.phi, i20 64
  
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 3: Chain linking with canonicalization disabled
;
; Tests that chain linking works on pre-existing i8 GEPs
; ============================================================================

; CHECK-LABEL: define void @test_chain_only_i8_geps
; CHECK: top:
; First GEP at offset 64
; CHECK:   %ptr64 = getelementptr inbounds i8, ptr %base, i20 64
; Second GEP chained (128-64=64)
; CHECK:   %ptr128.chained = getelementptr inbounds i8, ptr %ptr64, i20 64
; Third GEP chained (192-128=64)
; CHECK:   %ptr192.chained = getelementptr inbounds i8, ptr %ptr128.chained, i20 64

define void @test_chain_only_i8_geps(ptr noalias %base, ptr noalias %out,
                                      i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  ; Pre-existing i8-based GEPs with regular stride
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

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
