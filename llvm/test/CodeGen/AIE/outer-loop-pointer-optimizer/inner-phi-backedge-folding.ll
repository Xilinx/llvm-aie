; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; RUN: llc -mtriple=aie2ps -O2 -aie-enable-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=false \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -aie-enable-inner-phi-backedge-folding=true \
; RUN:     -stop-after=aie-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s --check-prefix=NOCHAIN

; RUN: llc -mtriple=aie2ps -O2 -aie-enable-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=true \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -aie-enable-inner-phi-backedge-folding=true \
; RUN:     -stop-after=aie-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s --check-prefix=CHAIN

; Test foldInnerPhiBackEdgeGEPs: fold redundant epilogue GEPs in nested loops.
;
; The pass folds a GEP in the epilogue (Bottom block) that exactly duplicates
; the inner loop's back-edge GEP:
;
;   inner:  %phi   = phi ptr [%init, Top] [%back, inner]
;           %back  = gep %phi, stride
;   bottom: %dup   = gep %phi, stride    <- same base and offset as %back
;
; %dup is replaced by %back directly, removing the standalone paddb/padda
; from the epilogue and seeding linkGEPChains to chain subsequent epilogue
; GEPs off the inner loop's natural exit pointer value.
;
; The fold fires only when:
;   (a) the epilogue GEP's base is the inner-loop PHI (not any other value),
;   (b) the offset exactly matches the back-edge GEP's offset.

; ===========================================================================
; Test 1: Basic fold — epilogue GEP is an exact duplicate of the back-edge GEP.
;
; The inner PHI %phi_inner has back-edge GEP %back = gep(%phi_inner, 64).
; The epilogue has %dup = gep(%phi_inner, 64) — identical.
; Expected: %dup is replaced by %back; no standalone GEP remains in Bottom.
; ===========================================================================

; NOCHAIN-LABEL: define void @test_basic_fold
; NOCHAIN: inner:
; The back-edge GEP is kept.
; NOCHAIN:   %back = getelementptr inbounds i8, ptr %phi_inner, i20 64
; NOCHAIN: bottom:
; The epilogue duplicate is gone — store now uses %back directly.
; NOCHAIN-NOT: getelementptr{{.*}} %phi_inner{{.*}} i20 64

; CHAIN-LABEL: define void @test_basic_fold
; CHAIN: inner:
; CHAIN:   %back = getelementptr inbounds i8, ptr %phi_inner, i20 64
; CHAIN: bottom:
; CHAIN-NOT: getelementptr{{.*}} %phi_inner{{.*}} i20 64

define void @test_basic_fold(ptr noalias %base, ptr noalias %out,
                              i32 %N, i32 %M) {
entry:
  br label %top

top:
  %outer_iv = phi i32 [ %N, %entry ], [ %outer_iv.next, %bottom ]
  %outer_ptr = phi ptr [ %base, %entry ], [ %outer_ptr.next, %bottom ]
  call void @llvm.set.loop.iterations.i32(i32 %M)
  br label %inner

inner:
  %phi_inner = phi ptr [ %outer_ptr, %top ], [ %back, %inner ]
  %val = load <32 x bfloat>, ptr %phi_inner, align 64
  ; Back-edge GEP: stride = 64
  %back = getelementptr inbounds i8, ptr %phi_inner, i20 64
  %inner_cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner_cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Epilogue GEP: same base (%phi_inner) and same offset (64) as %back.
  ; Must be folded to %back.
  %dup = getelementptr inbounds i8, ptr %phi_inner, i20 64
  store <32 x bfloat> %val, ptr %dup, align 64
  %outer_ptr.next = getelementptr inbounds i8, ptr %outer_ptr, i20 512
  %outer_iv.next = add i32 %outer_iv, -1
  %outer_cond = icmp eq i32 %outer_iv.next, 0
  br i1 %outer_cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ===========================================================================
; Test 2: Negative — epilogue GEP offset does NOT match the back-edge GEP.
;
; back-edge: gep(%phi2, 64).  Epilogue: gep(%phi2, 128).
; 128 ≠ 64 → condition (b) fails → no fold.
; ===========================================================================

; NOCHAIN-LABEL: define void @test_no_fold_offset_mismatch
; NOCHAIN: inner:
; NOCHAIN:   %back2 = getelementptr inbounds i8, ptr %phi2, i20 64
; NOCHAIN: bottom:
; Offset mismatch — epilogue GEP is preserved.
; NOCHAIN:   %no_fold2 = getelementptr inbounds i8, ptr %phi2, i20 128

; CHAIN-LABEL: define void @test_no_fold_offset_mismatch
; CHAIN: inner:
; CHAIN:   %back2 = getelementptr inbounds i8, ptr %phi2, i20 64
; CHAIN: bottom:
; Phase 1 did not fold (128 ≠ 64). linkGEPChains then chains it off %back2.
; CHAIN-NOT: getelementptr{{.*}} %phi2{{.*}} i20 128
; CHAIN:   %no_fold2.chained = getelementptr inbounds i8, ptr %back2, i20 64

define void @test_no_fold_offset_mismatch(ptr noalias %base2, ptr noalias %out2,
                                          i32 %N2, i32 %M2) {
entry:
  br label %top

top:
  %outer_iv = phi i32 [ %N2, %entry ], [ %outer_iv.next, %bottom ]
  %outer_ptr2 = phi ptr [ %base2, %entry ], [ %outer_ptr2.next, %bottom ]
  call void @llvm.set.loop.iterations.i32(i32 %M2)
  br label %inner

inner:
  %phi2 = phi ptr [ %outer_ptr2, %top ], [ %back2, %inner ]
  %val2 = load <32 x bfloat>, ptr %phi2, align 64
  ; Back-edge GEP: stride = 64
  %back2 = getelementptr inbounds i8, ptr %phi2, i20 64
  %inner_cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner_cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Offset 128 ≠ back-edge offset 64 → must NOT be folded.
  %no_fold2 = getelementptr inbounds i8, ptr %phi2, i20 128
  store <32 x bfloat> %val2, ptr %no_fold2, align 64
  %outer_ptr2.next = getelementptr inbounds i8, ptr %outer_ptr2, i20 512
  %outer_iv.next = add i32 %outer_iv, -1
  %outer_cond = icmp eq i32 %outer_iv.next, 0
  br i1 %outer_cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ===========================================================================
; Test 3: Negative — epilogue GEP base is NOT the inner-loop PHI.
;
; The epilogue has gep(%outer_ptr3, 64) where %outer_ptr3 is the outer-loop
; PHI, not the inner PHI %phi3.
; Condition (a) fails → no fold.
; ===========================================================================

; NOCHAIN-LABEL: define void @test_no_fold_base_not_inner_phi
; NOCHAIN: inner:
; NOCHAIN:   %back3 = getelementptr inbounds i8, ptr %phi3, i20 64
; NOCHAIN: bottom:
; Base is outer PHI, not inner PHI — epilogue GEP is preserved.
; NOCHAIN:   getelementptr{{.*}} %outer_ptr3{{.*}} i20 64

; CHAIN-LABEL: define void @test_no_fold_base_not_inner_phi
; CHAIN: inner:
; CHAIN:   %back3 = getelementptr inbounds i8, ptr %phi3, i20 64
; CHAIN: bottom:
; CHAIN:   getelementptr{{.*}} %outer_ptr3{{.*}} i20 64

define void @test_no_fold_base_not_inner_phi(ptr noalias %base3,
                                              ptr noalias %out3,
                                              i32 %N3, i32 %M3) {
entry:
  br label %top

top:
  %outer_iv = phi i32 [ %N3, %entry ], [ %outer_iv.next, %bottom ]
  %outer_ptr3 = phi ptr [ %base3, %entry ], [ %outer_ptr3.next, %bottom ]
  call void @llvm.set.loop.iterations.i32(i32 %M3)
  br label %inner

inner:
  %phi3 = phi ptr [ %outer_ptr3, %top ], [ %back3, %inner ]
  %val3 = load <32 x bfloat>, ptr %phi3, align 64
  ; Back-edge: stride = 64
  %back3 = getelementptr inbounds i8, ptr %phi3, i20 64
  %inner_cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner_cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Base is %outer_ptr3 (outer PHI), NOT %phi3 (inner PHI) → no fold.
  %no_fold3 = getelementptr inbounds i8, ptr %outer_ptr3, i20 64
  store <32 x bfloat> %val3, ptr %no_fold3, align 64
  %outer_ptr3.next = getelementptr inbounds i8, ptr %outer_ptr3, i20 512
  %outer_iv.next = add i32 %outer_iv, -1
  %outer_cond = icmp eq i32 %outer_iv.next, 0
  br i1 %outer_cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ===========================================================================
; Test 4: Fold + chain-linking interaction.
;
; Two epilogue GEPs:
;   %dup4   = gep(%phi4,  64)  <- matches back-edge → folded to %back4
;   %ep128  = gep(%phi4, 128)  <- larger offset; after fold, linkGEPChains
;                                  should chain %ep128 off %back4
;
; Without chain-linking (NOCHAIN): %dup4 is folded; %ep128 stays as-is.
; With chain-linking (CHAIN): %dup4 is folded and %ep128 is chained off
;   %back4 (delta = 128-64 = 64).
; ===========================================================================

; NOCHAIN-LABEL: define void @test_fold_and_chain
; NOCHAIN: inner:
; NOCHAIN:   %back4 = getelementptr inbounds i8, ptr %phi4, i20 64
; NOCHAIN: bottom:
; %dup4 (offset 64) is folded away.
; NOCHAIN-NOT: getelementptr{{.*}} %phi4{{.*}} i20 64
; NOCHAIN: {{.*}}getelementptr{{.*}} i20 128

; CHAIN-LABEL: define void @test_fold_and_chain
; CHAIN: inner:
; CHAIN:   %back4 = getelementptr inbounds i8, ptr %phi4, i20 64
; CHAIN: bottom:
; %dup4 is folded; %ep128 is chained off %back4 (delta=64).
; CHAIN-NOT: getelementptr{{.*}} %phi4{{.*}} i20 64
; CHAIN-NOT: getelementptr{{.*}} %phi4{{.*}} i20 128
; %ep128 is now chained: gep(%back4, 64)
; CHAIN:   getelementptr{{.*}} %back4{{.*}} i20 64

define void @test_fold_and_chain(ptr noalias %base4, ptr noalias %out4,
                                  i32 %N4, i32 %M4) {
entry:
  br label %top

top:
  %outer_iv = phi i32 [ %N4, %entry ], [ %outer_iv.next, %bottom ]
  %outer_ptr4 = phi ptr [ %base4, %entry ], [ %outer_ptr4.next, %bottom ]
  call void @llvm.set.loop.iterations.i32(i32 %M4)
  br label %inner

inner:
  %phi4 = phi ptr [ %outer_ptr4, %top ], [ %back4, %inner ]
  %val4 = load <32 x bfloat>, ptr %phi4, align 64
  ; Back-edge GEP: stride = 64
  %back4 = getelementptr inbounds i8, ptr %phi4, i20 64
  %inner_cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner_cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; This one matches %back4 exactly → folded.
  %dup4 = getelementptr inbounds i8, ptr %phi4, i20 64
  ; This one has a larger offset → not a Phase 1 fold, but linkGEPChains
  ; (CHAIN run) will chain it off %back4.
  %ep128 = getelementptr inbounds i8, ptr %phi4, i20 128
  store <32 x bfloat> %val4, ptr %dup4, align 64
  store <32 x bfloat> %val4, ptr %ep128, align 64
  %outer_ptr4.next = getelementptr inbounds i8, ptr %outer_ptr4, i20 512
  %outer_iv.next = add i32 %outer_iv, -1
  %outer_cond = icmp eq i32 %outer_iv.next, 0
  br i1 %outer_cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !2, !3}
!1 = distinct !{!1, !2}
!2 = !{!"llvm.loop.mustprogress"}
!3 = !{!"llvm.loop.itercount.range", i32 2}
