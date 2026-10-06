; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=false \
; RUN:     -aie-enable-gep-hoisting=true \
; RUN:     -stop-after=aie-outer-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Test GEP hoisting: moves GEPs from bottom (epilogue) to top (prologue).
;
; Conditions for hoisting:
; 1. GEP's pointer operand must be an instruction PRODUCED in Top
; 2. Pointer operand must NOT be used by any memory operation in Bottom
; 3. Pointer operand must NOT be used in Inner loop
; 4. All index operands must be available at end of Top

; ============================================================================
; Test 1: Basic GEP hoisting - GEP in bottom with pointer produced in top
;
; Expected: GEP is hoisted from bottom to top
; ============================================================================

; CHECK-LABEL: define void @test_gep_hoist_basic
; CHECK: top:
; GEP hoisted from bottom to top
; CHECK:   %ptr.in.top = getelementptr inbounds i8, ptr %base, i20 64
; CHECK:   %next.ptr = getelementptr inbounds i8, ptr %ptr.in.top, i20 128
; CHECK:   br label %inner
; CHECK: bottom:
; GEP should NOT be in bottom anymore
; CHECK-NOT: getelementptr{{.*}}%ptr.in.top{{.*}}128

define void @test_gep_hoist_basic(ptr noalias %base, ptr noalias %out,
                                   i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr [ %base, %entry ], [ %next.ptr, %bottom ]
  ; GEP produced in top - base for hoisting
  %ptr.in.top = getelementptr inbounds i8, ptr %base, i20 64
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
  ; GEP in bottom using pointer from top - should be hoisted
  ; ptr.in.top is NOT used by any memory op in bottom
  %next.ptr = getelementptr inbounds i8, ptr %ptr.in.top, i20 128
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 2: NO hoisting when pointer is a PHI (not produced in top)
;
; Expected: GEP stays in bottom because base is a PHI, not produced in top
; ============================================================================

; CHECK-LABEL: define void @test_gep_no_hoist_phi_base
; CHECK: top:
; PHI is not "produced" in top
; CHECK:   %ptr.phi = phi ptr
; CHECK: bottom:
; GEP should stay in bottom because base is PHI
; CHECK:   %next.ptr = getelementptr inbounds i8, ptr %ptr.phi, i20 128

define void @test_gep_no_hoist_phi_base(ptr noalias %base, ptr noalias %out,
                                         i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr.phi = phi ptr [ %base, %entry ], [ %next.ptr, %bottom ]
  %v = load <32 x bfloat>, ptr %ptr.phi, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = fadd <32 x bfloat> %acc, %v
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x bfloat> %acc.next, ptr %out, align 64
  ; GEP using PHI as base - should NOT be hoisted (PHI is not produced)
  %next.ptr = getelementptr inbounds i8, ptr %ptr.phi, i20 128
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 3: NO hoisting when pointer has memory use in bottom
;
; Expected: GEP stays in bottom to preserve post-increment folding opportunity
; ============================================================================

; CHECK-LABEL: define void @test_gep_no_hoist_memory_use
; CHECK: top:
; CHECK:   %ptr.in.top = getelementptr inbounds i8, ptr %base, i20 64
; CHECK: bottom:
; GEP should stay because ptr.in.top is used by store in bottom
; CHECK:   store <32 x bfloat> %acc.next, ptr %ptr.in.top
; CHECK:   %next.ptr = getelementptr inbounds i8, ptr %ptr.in.top, i20 128

define void @test_gep_no_hoist_memory_use(ptr noalias %base, ptr noalias %out,
                                           i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr [ %base, %entry ], [ %next.ptr, %bottom ]
  ; GEP produced in top
  %ptr.in.top = getelementptr inbounds i8, ptr %base, i20 64
  %v = load <32 x bfloat>, ptr %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = fadd <32 x bfloat> %acc, %v
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; ptr.in.top is used by store - prevents hoisting of GEP
  store <32 x bfloat> %acc.next, ptr %ptr.in.top, align 64
  ; GEP using ptr.in.top - should NOT be hoisted (ptr has memory use)
  %next.ptr = getelementptr inbounds i8, ptr %ptr.in.top, i20 128
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 4: NO hoisting when pointer is used in inner loop
;
; Expected: GEP stays in bottom because base is used in inner loop
; ============================================================================

; CHECK-LABEL: define void @test_gep_no_hoist_inner_use
; CHECK: top:
; CHECK:   %ptr.in.top = getelementptr inbounds i8, ptr %base, i20 64
; CHECK: inner:
; ptr.in.top is used in inner loop
; CHECK:   load <32 x bfloat>, ptr %ptr.in.top
; CHECK: bottom:
; GEP should stay because ptr.in.top is used in inner loop
; CHECK:   %next.ptr = getelementptr inbounds i8, ptr %ptr.in.top, i20 128

define void @test_gep_no_hoist_inner_use(ptr noalias %base, ptr noalias %out,
                                          i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr [ %base, %entry ], [ %next.ptr, %bottom ]
  ; GEP produced in top
  %ptr.in.top = getelementptr inbounds i8, ptr %base, i20 64
  %v = load <32 x bfloat>, ptr %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x bfloat> [ %v, %top ], [ %acc.next, %inner ]
  ; Using ptr.in.top in inner loop - prevents hoisting
  %v2 = load <32 x bfloat>, ptr %ptr.in.top, align 64
  %acc.next = fadd <32 x bfloat> %acc, %v2
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x bfloat> %acc.next, ptr %out, align 64
  ; GEP using ptr.in.top - should NOT be hoisted (ptr used in inner)
  %next.ptr = getelementptr inbounds i8, ptr %ptr.in.top, i20 128
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
