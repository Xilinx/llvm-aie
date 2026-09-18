; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=true \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -aie-enable-inner-phi-backedge-folding=false \
; RUN:     -aie-enable-gep-addrspace-canon=false \
; RUN:     -aie-enable-phi-normalization=false \
; RUN:     -aie-enable-post-inc-chain=false \
; RUN:     -stop-after=aie-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s

; Tests for the external-GEP-user guard in linkGEPChains.
;
; When a loop-header PHI has a GEP user OUTSIDE the loop, linking the
; in-loop GEPs would leave that external GEP as a standalone computation,
; creating an extra pointer-copy instruction and interfering with the global
; combiner.  linkGEPChains must therefore skip the chain entirely for that
; PHI.
;
; A non-GEP external user (load, store, etc.) is harmless and must NOT
; block chain formation.

; ============================================================================
; Test 1: PHI has an external GEP user → chain must NOT be formed.
;
; The pointer PHI %ptr is used by:
;   - %ptr64  = gep(%ptr, 64)   }  inside the loop
;   - %ptr128 = gep(%ptr, 128)  }
;   - %ptr.next = gep(%ptr, 192)}  back-edge (also inside)
;   - %ext_gep = gep(%ptr, 64)     outside (for.exit)  <-- blocks chain
;
; Expected: no .chained GEPs; all in-loop GEPs keep %ptr as their base.
; ============================================================================

; CHECK-LABEL: define void @test_no_chain_external_gep_user
; CHECK: for.body:
; All three in-loop GEPs still reference %ptr directly (no linking).
; CHECK:   %ptr64 = getelementptr inbounds i8, ptr %ptr, i20 64
; CHECK:   %ptr128 = getelementptr inbounds i8, ptr %ptr, i20 128
; CHECK:   %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 192
; CHECK: for.exit:
; The external GEP is unchanged.
; CHECK:   %ext_gep = getelementptr inbounds i8, ptr %ptr, i20 64

define void @test_no_chain_external_gep_user(ptr noalias %base, ptr noalias %out,
                                              i32 %N) {
for.preheader:
  br label %for.body

for.body:
  %ptr = phi ptr [ %base, %for.preheader ], [ %ptr.next, %for.body ]
  %ptr64  = getelementptr inbounds i8, ptr %ptr, i20 64
  %ptr128 = getelementptr inbounds i8, ptr %ptr, i20 128
  %v1 = load <32 x bfloat>, ptr %ptr64, align 64
  %v2 = load <32 x bfloat>, ptr %ptr128, align 64
  store <32 x bfloat> %v1, ptr %out, align 64
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 192
  %cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %cond, label %for.body, label %for.exit, !llvm.loop !0

for.exit:
  ; External GEP using the loop's pointer PHI -- this blocks chain formation.
  %ext_gep = getelementptr inbounds i8, ptr %ptr, i20 64
  store <32 x bfloat> %v2, ptr %ext_gep, align 64
  ret void
}

; ============================================================================
; Test 2: PHI has an external non-GEP user (load) → chain MUST be formed.
;
; The pointer PHI %ptr is used by:
;   - %ptr64  = gep(%ptr, 64)   }  inside the loop
;   - %ptr128 = gep(%ptr, 128)  }
;   - %ptr.next = gep(%ptr, 192)}  back-edge (also inside)
;   - load <32 x bfloat>, ptr %ptr  outside (for.exit)  <-- NOT a GEP, allowed
;
; Expected: normal chain formation; %ptr128 and %ptr.next gain .chained suffix.
; ============================================================================

; CHECK-LABEL: define void @test_chain_external_load_user
; CHECK: for.body:
; Back-edge phi uses the final chained GEP.
; CHECK:   %ptr = phi ptr [ %base, %for.preheader ], [ %ptr.next.chained, %for.body ]
; First GEP anchors the chain.
; CHECK:   %ptr64 = getelementptr inbounds i8, ptr %ptr, i20 64
; Second GEP is linked off the first (delta = 128-64 = 64).
; CHECK:   %ptr128.chained = getelementptr inbounds i8, ptr %ptr64, i20 64
; Back-edge GEP is linked off the second (delta = 192-128 = 64).
; CHECK:   %ptr.next.chained = getelementptr inbounds i8, ptr %ptr128.chained, i20 64

define void @test_chain_external_load_user(ptr noalias %base, ptr noalias %out,
                                           i32 %N) {
for.preheader:
  br label %for.body

for.body:
  %ptr = phi ptr [ %base, %for.preheader ], [ %ptr.next, %for.body ]
  %ptr64  = getelementptr inbounds i8, ptr %ptr, i20 64
  %ptr128 = getelementptr inbounds i8, ptr %ptr, i20 128
  %v1 = load <32 x bfloat>, ptr %ptr64, align 64
  %v2 = load <32 x bfloat>, ptr %ptr128, align 64
  store <32 x bfloat> %v1, ptr %out, align 64
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 192
  %cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %cond, label %for.body, label %for.exit, !llvm.loop !0

for.exit:
  ; External LOAD of the loop's pointer PHI -- non-GEP, chain is not blocked.
  %v_ext = load <32 x bfloat>, ptr %ptr, align 64
  store <32 x bfloat> %v_ext, ptr %out, align 64
  ret void
}

declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.mustprogress"}
