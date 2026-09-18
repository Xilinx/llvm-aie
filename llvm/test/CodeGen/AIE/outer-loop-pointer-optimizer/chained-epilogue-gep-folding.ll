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

; Test foldInnerPhiBackEdgeGEPs Phase 2 and Optimization 2c.
;
; Phase 2 -- full-trip match using a constant inner loop trip count:
;   Top:    call @llvm.set.loop.iterations(N)   ; N must be a compile-time constant
;           %init = <Top-incoming of the inner PHI>
;   Inner:  %phi  = phi ptr [%init, Top] [%back, Inner]
;           %back = gep %phi, stride
;   Bottom: %full = gep %init, N*stride         ; equals %back at exit -> fold
;
; Optimization 2c -- second invocation of foldInnerPhiBackEdgeGEPs after
; linkGEPChains:  linkGEPChains may rewrite the init GEP in Top and create a
; corresponding chained epilogue GEP in Bottom.  The second call lets Phase 2
; see those chained GEPs and fold them.

; ===========================================================================
; Test 1: Phase 2 basic fold -- inner PHI init is the outer-loop PHI directly.
;
; N=4 (constant), stride=64.  Full offset = 4x64 = 256.
; %full = gep(%outer_ptr, 256) in Bottom equals %back at exit.
; Expected: %full -> %back in BOTH NOCHAIN and CHAIN runs (Phase 2 first call).
; ===========================================================================

; NOCHAIN-LABEL: define void @test_phase2_outer_phi_init
; NOCHAIN: inner:
; NOCHAIN:   %back = getelementptr inbounds i8, ptr %phi, i20 64
; NOCHAIN: bottom:
; Full-trip GEP is folded -- store uses %back directly.
; NOCHAIN-NOT: getelementptr{{.*}} %outer_ptr{{.*}} i20 256

; CHAIN-LABEL: define void @test_phase2_outer_phi_init
; CHAIN: inner:
; CHAIN:   %back = getelementptr inbounds i8, ptr %phi, i20 64
; CHAIN: bottom:
; CHAIN-NOT: getelementptr{{.*}} %outer_ptr{{.*}} i20 256

define void @test_phase2_outer_phi_init(ptr noalias %base, ptr noalias %out,
                                        i32 %N) {
entry:
  br label %top

top:
  %outer_iv = phi i32 [ %N, %entry ], [ %outer_iv.next, %bottom ]
  %outer_ptr = phi ptr [ %base, %entry ], [ %outer_ptr.next, %bottom ]
  ; Inner PHI init = %outer_ptr directly; N = 4 iterations.
  call void @llvm.set.loop.iterations.i32(i32 4)
  br label %inner

inner:
  %phi = phi ptr [ %outer_ptr, %top ], [ %back, %inner ]
  %val = load <32 x bfloat>, ptr %phi, align 64
  ; Back-edge: stride = 64.
  %back = getelementptr inbounds i8, ptr %phi, i20 64
  %inner_cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner_cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Full-trip GEP: gep(%outer_ptr, 4*64=256) equals %back at loop exit.
  ; Phase 2 must fold this to %back.
  %full = getelementptr i8, ptr %outer_ptr, i20 256
  store <32 x bfloat> %val, ptr %full, align 64
  %outer_ptr.next = getelementptr inbounds i8, ptr %outer_ptr, i20 512
  %outer_iv.next = add i32 %outer_iv, -1
  %outer_cond = icmp eq i32 %outer_iv.next, 0
  br i1 %outer_cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ===========================================================================
; Test 2: Phase 2 NOT triggered -- no @llvm.set.loop.iterations.
;
; Without a constant trip count, Phase 2 cannot compute Nxstride and must
; leave the epilogue GEP untouched.
; ===========================================================================

; NOCHAIN-LABEL: define void @test_phase2_no_trip_count
; NOCHAIN: bottom:
; No trip count -> Phase 2 skipped -- epilogue GEP survives.
; NOCHAIN:   getelementptr{{.*}} i20 256

; CHAIN-LABEL: define void @test_phase2_no_trip_count
; CHAIN: bottom:
; CHAIN:   getelementptr{{.*}} i20 256

define void @test_phase2_no_trip_count(ptr noalias %base2, ptr noalias %out2,
                                       i32 %N2) {
entry:
  br label %top

top:
  %outer_iv = phi i32 [ %N2, %entry ], [ %outer_iv.next, %bottom ]
  %outer_ptr2 = phi ptr [ %base2, %entry ], [ %outer_ptr2.next, %bottom ]
  ; No @llvm.set.loop.iterations -> inner trip count unknown.
  br label %inner

inner:
  %phi2 = phi ptr [ %outer_ptr2, %top ], [ %back2, %inner ]
  %val2 = load <32 x bfloat>, ptr %phi2, align 64
  %back2 = getelementptr inbounds i8, ptr %phi2, i20 64
  %inner_cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner_cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Phase 2 cannot fire (no trip count) -> must NOT be folded.
  %no_fold2 = getelementptr i8, ptr %outer_ptr2, i20 256
  store <32 x bfloat> %val2, ptr %no_fold2, align 64
  %outer_ptr2.next = getelementptr inbounds i8, ptr %outer_ptr2, i20 512
  %outer_iv.next = add i32 %outer_iv, -1
  %outer_cond = icmp eq i32 %outer_iv.next, 0
  br i1 %outer_cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ===========================================================================
; Test 3: Phase 2 NOT triggered -- epilogue GEP offset != Nxstride.
;
; N=4, stride=64 -> FullOffset=256. Epilogue has offset=320 (!=256).
; Must NOT be folded.
; ===========================================================================

; NOCHAIN-LABEL: define void @test_phase2_offset_mismatch
; NOCHAIN: bottom:
; Offset mismatch (320 != 4*64=256) -- epilogue GEP survives.
; NOCHAIN:   getelementptr{{.*}} i20 320

; CHAIN-LABEL: define void @test_phase2_offset_mismatch
; CHAIN: bottom:
; CHAIN:   getelementptr{{.*}} i20 320

define void @test_phase2_offset_mismatch(ptr noalias %base3, ptr noalias %out3,
                                         i32 %N3) {
entry:
  br label %top

top:
  %outer_iv = phi i32 [ %N3, %entry ], [ %outer_iv.next, %bottom ]
  %outer_ptr3 = phi ptr [ %base3, %entry ], [ %outer_ptr3.next, %bottom ]
  ; N = 4; stride = 64 -> FullOffset = 256.
  call void @llvm.set.loop.iterations.i32(i32 4)
  br label %inner

inner:
  %phi3 = phi ptr [ %outer_ptr3, %top ], [ %back3, %inner ]
  %val3 = load <32 x bfloat>, ptr %phi3, align 64
  %back3 = getelementptr inbounds i8, ptr %phi3, i20 64
  %inner_cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner_cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Offset 320 != FullOffset 256 -> must NOT be folded.
  %no_fold3 = getelementptr i8, ptr %outer_ptr3, i20 320
  store <32 x bfloat> %val3, ptr %no_fold3, align 64
  %outer_ptr3.next = getelementptr inbounds i8, ptr %outer_ptr3, i20 512
  %outer_iv.next = add i32 %outer_iv, -1
  %outer_cond = icmp eq i32 %outer_iv.next, 0
  br i1 %outer_cond, label %exit, label %top, !llvm.loop !0

exit:
  ret void
}

; ===========================================================================
; Test 4: Optimization 2c -- Phase 2 fires in the SECOND call (after
; linkGEPChains), not in the first call.
;
; %init = gep(%outer_ptr4, 128) is an intermediate GEP in Top, used as the
; inner PHI's top-incoming.  N=4, stride=128, FullOffset=512.
;
; Before the pass (after LSR):
;   Top:    %init = gep %outer_ptr4, 128
;   Inner:  %phi4 = phi [%init, top] [%back4, inner]
;           %back4 = gep %phi4, 128
;   Bottom: gep(%outer_ptr4, 640)   <- LSR rewrites gep(%init,512) as this
;                                       (base = outer PHI, not %init)
;
; NOCHAIN run (Phase 2 first call only):
;   Base = %outer_ptr4; inner PHI's top-incoming = %init != %outer_ptr4
;   -> NO match -> NOT folded.
;
; CHAIN run (Phase 2 first call + linkGEPChains + Phase 2 second call):
;   linkGEPChains chains %init in Top and the Bottom GEP off it:
;     %.chained = gep <prev>, 128   (becomes new %init)
;     %.chained3 = gep %.chained, 512
;   Phase 2 (second call): Base=%.chained == top-incoming of inner PHI,
;   Stride=128, N=4, FullOffset=512 -> FOLD to %back4.
; ===========================================================================

; NOCHAIN-LABEL: define void @test_opt2c_chained_init
; NOCHAIN: inner:
; NOCHAIN:   %back4 = getelementptr inbounds i8, ptr %phi4, i20 128
; NOCHAIN: bottom:
; Phase 2 first call: base is outer PHI, not %init -> no match -> NOT folded.
; NOCHAIN: getelementptr{{.*}} i20 640

; CHAIN-LABEL: define void @test_opt2c_chained_init
; CHAIN: inner:
; CHAIN:   %back4 = getelementptr inbounds i8, ptr %phi4, i20 128
; CHAIN: bottom:
; linkGEPChains creates %.chained3, Phase 2 (second call) folds it to %back4.
; CHAIN-NOT: getelementptr{{.*}} i20 640
; CHAIN-NOT: getelementptr{{.*}} i20 512

define void @test_opt2c_chained_init(ptr noalias %base4, ptr noalias %out4,
                                     i32 %N4) {
entry:
  br label %top

top:
  %outer_iv = phi i32 [ %N4, %entry ], [ %outer_iv.next, %bottom ]
  %outer_ptr4 = phi ptr [ %base4, %entry ], [ %outer_ptr4.next, %bottom ]
  ; Intermediate GEP in Top: inner PHI init is %init, not %outer_ptr4.
  %init = getelementptr inbounds i8, ptr %outer_ptr4, i20 128
  ; N = 4 iterations; stride = 128; FullOffset = 4*128 = 512.
  call void @llvm.set.loop.iterations.i32(i32 4)
  br label %inner

inner:
  %phi4 = phi ptr [ %init, %top ], [ %back4, %inner ]
  %val4 = load <32 x bfloat>, ptr %phi4, align 64
  ; Back-edge: stride = 128.
  %back4 = getelementptr inbounds i8, ptr %phi4, i20 128
  %inner_cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %inner_cond, label %inner, label %bottom, !llvm.loop !1

bottom:
  ; Full-trip GEP: gep(%init, 4*128=512).
  ; After LSR this becomes gep(%outer_ptr4, 640) (base is outer PHI, not %init).
  ; Without chain-linking Phase 2 cannot match (base != init); with chain-linking
  ; linkGEPChains re-chains it as gep(%init, 512) so Phase 2 (2nd call) folds.
  %full4 = getelementptr i8, ptr %init, i20 512
  store <32 x bfloat> %val4, ptr %full4, align 64
  %outer_ptr4.next = getelementptr inbounds i8, ptr %outer_ptr4, i20 1024
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
