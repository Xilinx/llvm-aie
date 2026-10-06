; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-outer-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-addrspace-canon=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=false \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -stop-after=aie-outer-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s


; Test GEP address space canonicalization: keeps GEPs in the PHI's canonical
; address space and moves addrspacecast to point-of-use.
;
; The pass transforms round-trip casts:
;   %phi = phi ptr addrspace(5) ...
;   %cast = addrspacecast ptr addrspace(5) %phi to ptr addrspace(7)
;   %gep = getelementptr i8, ptr addrspace(7) %cast, i20 128
;   %cast_back = addrspacecast ptr addrspace(7) %gep to ptr addrspace(5)
;   use ptr addrspace(5) %cast_back
; Becomes:
;   %phi = phi ptr addrspace(5) ...
;   %gep = getelementptr i8, ptr addrspace(5) %phi, i20 128
;   use ptr addrspace(5) %gep

; ============================================================================
; Test 1: Round-trip cast elimination
;
; Expected: GEP is rebuilt in AS5 (PHI's canonical AS), cast-back is eliminated
; ============================================================================

; CHECK-LABEL: define void @test_roundtrip_elimination
; CHECK:       %ptr = phi ptr addrspace(5)
; The GEP should now operate directly in AS5 (PHI's address space)
; CHECK:       getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 128
; Store should use the GEP in AS5 (round-trip cast eliminated)
; CHECK:       store <32 x i16> %{{.*}}, ptr addrspace(5) %{{.*}}, align 64

define void @test_roundtrip_elimination(ptr addrspace(5) noalias %ptr_init,
                                         i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  ; Pointer PHI - this is the canonical address space (AS5)
  %ptr = phi ptr addrspace(5) [ %ptr_init, %entry ], [ %next_ptr, %bottom ]
  ; Pattern: addrspacecast AS5->AS7, GEP, addrspacecast AS7->AS5 (round-trip)
  %cast0 = addrspacecast ptr addrspace(5) %ptr to ptr addrspace(7)
  %gep = getelementptr inbounds i8, ptr addrspace(7) %cast0, i20 128
  %cast_back = addrspacecast ptr addrspace(7) %gep to ptr addrspace(5)
  %v = load <32 x i16>, ptr addrspace(5) %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x i16> [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = add <32 x i16> %acc, %v
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Store uses the cast-back result - round-trip should be eliminated
  store <32 x i16> %acc.next, ptr addrspace(5) %cast_back, align 64
  ; Compute next pointer for the PHI
  %next_ptr = getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 2: Multiple GEPs through same addrspacecast (PHI-driven)
;
; Expected: All GEPs are rebuilt in AS5, all cast-backs eliminated
; ============================================================================

; CHECK-LABEL: define void @test_multiple_geps_from_phi
; CHECK:       %ptr = phi ptr addrspace(5)
; All GEPs should operate in AS5 (PHI's canonical address space)
; CHECK:       getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 128
; CHECK:       store <32 x i16> %{{.*}}, ptr addrspace(5) %{{.*}}, align 64
; CHECK:       getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 256
; CHECK:       store <32 x i16> %{{.*}}, ptr addrspace(5) %{{.*}}, align 64
; CHECK:       getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 384
; CHECK:       store <32 x i16> %{{.*}}, ptr addrspace(5) %{{.*}}, align 64

define void @test_multiple_geps_from_phi(ptr addrspace(5) noalias %ptr_init,
                                          i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr addrspace(5) [ %ptr_init, %entry ], [ %next_ptr, %bottom ]
  ; Multiple GEPs through same addrspacecast
  %cast0 = addrspacecast ptr addrspace(5) %ptr to ptr addrspace(7)
  %gep1 = getelementptr inbounds i8, ptr addrspace(7) %cast0, i20 128
  %cast1 = addrspacecast ptr addrspace(7) %gep1 to ptr addrspace(5)
  %gep2 = getelementptr inbounds i8, ptr addrspace(7) %cast0, i20 256
  %cast2 = addrspacecast ptr addrspace(7) %gep2 to ptr addrspace(5)
  %gep3 = getelementptr inbounds i8, ptr addrspace(7) %cast0, i20 384
  %cast3 = addrspacecast ptr addrspace(7) %gep3 to ptr addrspace(5)
  %v = load <32 x i16>, ptr addrspace(5) %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x i16> [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = add <32 x i16> %acc, %v
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x i16> %acc.next, ptr addrspace(5) %cast1, align 64
  store <32 x i16> %acc.next, ptr addrspace(5) %cast2, align 64
  store <32 x i16> %acc.next, ptr addrspace(5) %cast3, align 64
  %next_ptr = getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 3: GEP with variable index
;
; Expected: Variable index GEPs are also transformed
; ============================================================================

; CHECK-LABEL: define void @test_variable_index
; CHECK:       %ptr = phi ptr addrspace(5)
; CHECK:       getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 %idx
; CHECK:       store <32 x i16> %{{.*}}, ptr addrspace(5) %{{.*}}, align 64

define void @test_variable_index(ptr addrspace(5) noalias %ptr_init,
                                  i20 %idx, i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr addrspace(5) [ %ptr_init, %entry ], [ %next_ptr, %bottom ]
  ; GEP with variable index
  %cast0 = addrspacecast ptr addrspace(5) %ptr to ptr addrspace(7)
  %gep = getelementptr inbounds i8, ptr addrspace(7) %cast0, i20 %idx
  %cast_back = addrspacecast ptr addrspace(7) %gep to ptr addrspace(5)
  %v = load <32 x i16>, ptr addrspace(5) %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x i16> [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = add <32 x i16> %acc, %v
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  store <32 x i16> %acc.next, ptr addrspace(5) %cast_back, align 64
  %next_ptr = getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 4: Cross-block round-trip cast elimination
;
; This tests the case where the PHI is in the Top (header) block, but the
; addrspacecast + GEP + cast_back are all in the Bottom (epilogue) block.
; This is the real-world pattern from GEMM kernels.
;
; Expected: GEP is rebuilt in AS5 (PHI's canonical AS), cast-back is eliminated
; ============================================================================

; CHECK-LABEL: define void @test_crossblock_roundtrip
; CHECK:       top:
; CHECK:       %ptr = phi ptr addrspace(5)
; CHECK:       inner:
; CHECK:       bottom:
; The GEP should now operate directly in AS5 (PHI's address space)
; CHECK:       getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 128
; Store should use the GEP in AS5 (round-trip cast eliminated)
; CHECK:       store <32 x i16> %{{.*}}, ptr addrspace(5) %{{.*}}, align 64
; The old addrspacecast from PHI to non-canonical AS should be eliminated
; CHECK-NOT:   addrspacecast ptr addrspace(5) %ptr to ptr addrspace(7)

define void @test_crossblock_roundtrip(ptr addrspace(5) noalias %ptr_init,
                                        i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  ; Pointer PHI - this is the canonical address space (AS5)
  %ptr = phi ptr addrspace(5) [ %ptr_init, %entry ], [ %next_ptr, %bottom ]
  ; No addrspacecast here - it's in the bottom block
  %v = load <32 x i16>, ptr addrspace(5) %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x i16> [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = add <32 x i16> %acc, %v
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Pattern: addrspacecast AS5->AS7, GEP, addrspacecast AS7->AS5 (round-trip)
  ; All in the BOTTOM block, PHI is in TOP block
  %cast0 = addrspacecast ptr addrspace(5) %ptr to ptr addrspace(7)
  %gep = getelementptr inbounds i8, ptr addrspace(7) %cast0, i20 128
  %cast_back = addrspacecast ptr addrspace(7) %gep to ptr addrspace(5)
  ; Store uses the cast-back result - round-trip should be eliminated
  store <32 x i16> %acc.next, ptr addrspace(5) %cast_back, align 64
  ; Compute next pointer for the PHI
  %next_ptr = getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 64
  %iv.next = add i32 %iv, -1
  %outer.cond = icmp eq i32 %iv.next, 0
  br i1 %outer.cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ============================================================================
; Test 5: Cross-block with multiple GEPs in Bottom
;
; Tests multiple GEPs using the same addrspacecast in the Bottom block.
;
; Expected: All GEPs are rebuilt in AS5, all cast-backs eliminated
; ============================================================================

; CHECK-LABEL: define void @test_crossblock_multiple_geps
; CHECK:       bottom:
; All GEPs should operate in AS5 (PHI's canonical address space)
; CHECK:       getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 128
; CHECK:       getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 256
; Stores should use the GEPs in AS5 (round-trip casts eliminated)
; CHECK:       store <32 x i16> %{{.*}}, ptr addrspace(5) %{{.*}}, align 64
; CHECK:       store <32 x i16> %{{.*}}, ptr addrspace(5) %{{.*}}, align 64
; The old addrspacecast should be eliminated
; CHECK-NOT:   addrspacecast ptr addrspace(5) %ptr to ptr addrspace(7)

define void @test_crossblock_multiple_geps(ptr addrspace(5) noalias %ptr_init,
                                            i32 %N, i32 %M) {
entry:
  %cmp.outer = icmp sgt i32 %N, 1
  br i1 %cmp.outer, label %top, label %exit

top:
  %iv = phi i32 [ %N, %entry ], [ %iv.next, %bottom ]
  %ptr = phi ptr addrspace(5) [ %ptr_init, %entry ], [ %next_ptr, %bottom ]
  %v = load <32 x i16>, ptr addrspace(5) %ptr, align 64
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %acc = phi <32 x i16> [ %v, %top ], [ %acc.next, %inner ]
  %acc.next = add <32 x i16> %acc, %v
  %inner.cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner.cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Multiple round-trip patterns all in Bottom block
  %cast0 = addrspacecast ptr addrspace(5) %ptr to ptr addrspace(7)
  %gep1 = getelementptr inbounds i8, ptr addrspace(7) %cast0, i20 128
  %cast1 = addrspacecast ptr addrspace(7) %gep1 to ptr addrspace(5)
  %gep2 = getelementptr inbounds i8, ptr addrspace(7) %cast0, i20 256
  %cast2 = addrspacecast ptr addrspace(7) %gep2 to ptr addrspace(5)
  store <32 x i16> %acc.next, ptr addrspace(5) %cast1, align 64
  store <32 x i16> %acc.next, ptr addrspace(5) %cast2, align 64
  %next_ptr = getelementptr inbounds i8, ptr addrspace(5) %ptr, i20 64
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
