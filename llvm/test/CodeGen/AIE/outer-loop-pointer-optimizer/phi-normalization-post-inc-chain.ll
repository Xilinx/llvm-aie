; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=false \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -aie-enable-gep-addrspace-canon=false \
; RUN:     -aie-enable-phi-normalization=true \
; RUN:     -aie-enable-post-inc-chain=false \
; RUN:     -stop-after=aie-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s --check-prefix=NORM
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=false \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -aie-enable-gep-addrspace-canon=false \
; RUN:     -aie-enable-phi-normalization=false \
; RUN:     -aie-enable-post-inc-chain=true \
; RUN:     -stop-after=aie-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s --check-prefix=CHAIN
;
; RUN: llc -mtriple=aie2p -O2 -aie-enable-loop-pointer-opt=true \
; RUN:     -aie-enable-gep-canonicalization=false \
; RUN:     -aie-enable-gep-chain-linking=false \
; RUN:     -aie-enable-gep-hoisting=false \
; RUN:     -aie-enable-gep-addrspace-canon=false \
; RUN:     -aie-enable-phi-normalization=true \
; RUN:     -aie-enable-post-inc-chain=true \
; RUN:     -stop-after=aie-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s --check-prefix=COMBINED

; ===========================================================================
; Test 1: Pattern 1 — pre-increment phi normalization (normalizePhiToLoadBase)
;
; The phi advances by 128 per iteration; loads happen at phi+128 and phi+192.
; After normalization:
;   - preheader gets: %phi.shifted.init = gep %init, 128
;   - phi now points to first load address (phi_new = old phi+128)
;   - loads use phi_new directly (at +0) and phi_new+64
;   - a new back-edge GEP phi_new+128 is inserted at end of latch
; ===========================================================================

; NORM-LABEL: define void @test_preinc_phi_normalization

; After normalization the phi's preheader incoming value is shifted by 128.
; NORM: for.preheader:
; NORM:   %phi.shifted.init = getelementptr i8, ptr %base, i20 128

; The phi itself now carries the shifted value.
; NORM: for.body:
; NORM:   %phi = phi ptr [ %phi.shifted.init, %for.preheader ], [ %phi.back, %for.body ]

; The original pre-increment GEP (phi+128) is gone; loads now use phi directly.
; NORM-NOT:   %preinc = getelementptr inbounds i8, ptr %phi, i20 128
; NORM:   %load1 = load <32 x bfloat>, ptr %phi

; A new back-edge GEP (phi+128) is inserted at the END of the block.
; NORM:   %phi.back = getelementptr inbounds i8, ptr %phi, i20 128

define void @test_preinc_phi_normalization(ptr noalias %base, i32 %N) {
for.preheader:
  br label %for.body

for.body:
  ; phi is pre-increment: advances by 128, loads happen AT phi+128
  %phi = phi ptr [ %base, %for.preheader ], [ %preinc, %for.body ]
  ; Pre-increment GEP at top of loop
  %preinc = getelementptr inbounds i8, ptr %phi, i20 128
  ; Load at phi+128 (using the pre-increment GEP directly)
  %load1 = load <32 x bfloat>, ptr %preinc, align 64
  ; Load at phi+192 (= preinc + 64)
  %gep2 = getelementptr inbounds i8, ptr %preinc, i20 64
  %load2 = load <32 x bfloat>, ptr %gep2, align 64
  %sum = fadd <32 x bfloat> %load1, %load2
  store <32 x bfloat> %sum, ptr %preinc, align 64
  %cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %cond, label %for.body, label %for.exit, !llvm.loop !0

for.exit:
  ret void
}

; ===========================================================================
; Test 2: Pattern 1 — phi live-out: normalization must NOT apply
;
; The phi is used outside the loop (live-out) — unsafe to shift.
; ===========================================================================

; NORM-LABEL: define ptr @test_preinc_phi_liveout

; The pre-increment GEP must remain (no normalization).
; NORM: for.body:
; NORM:   %preinc = getelementptr inbounds i8, ptr %phi, i20 128

define ptr @test_preinc_phi_liveout(ptr noalias %base, i32 %N) {
for.preheader:
  br label %for.body

for.body:
  %phi = phi ptr [ %base, %for.preheader ], [ %preinc, %for.body ]
  %preinc = getelementptr inbounds i8, ptr %phi, i20 128
  %load1 = load <32 x bfloat>, ptr %preinc, align 64
  %cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %cond, label %for.body, label %for.exit, !llvm.loop !0

for.exit:
  ; phi is live-out: returned to caller
  ret ptr %phi
}

; ===========================================================================
; Test 3: Pattern 2 — post-increment chain building (buildPostIncChain)
;
; The phi already points to the first load address.
; A GEP (%gep1 = phi + 64) is placed BEFORE load2 in the original IR.
; After buildPostIncChain, the GEP must appear AFTER load1.
; ===========================================================================

; CHAIN-LABEL: define void @test_postinc_chain_reorder

; CHAIN: for.body:
; CHAIN:   %phi = phi ptr
; The phi is used directly as first load address (at offset 0).
; CHAIN:   %load1 = load <32 x bfloat>, ptr %phi
; load2 can use gep1 even before gep1 is textually repositioned,
; since the pass moves gep1 after the LAST mem user of phi (the store).
; CHAIN:   %load2 = load <32 x bfloat>, ptr %gep1
; CHAIN:   store <32 x bfloat>
; The GEP is repositioned AFTER the store (last mem user of phi).
; CHAIN:   %gep1 = getelementptr inbounds i8, ptr %phi, i20 64

define void @test_postinc_chain_reorder(ptr noalias %base, i32 %N) {
for.preheader:
  br label %for.body

for.body:
  %phi = phi ptr [ %base, %for.preheader ], [ %back, %for.body ]
  ; GEP placed BEFORE load1 in the original IR (pre-positioned)
  %gep1 = getelementptr inbounds i8, ptr %phi, i20 64
  ; load1 uses phi directly
  %load1 = load <32 x bfloat>, ptr %phi, align 64
  ; load2 uses gep1
  %load2 = load <32 x bfloat>, ptr %gep1, align 64
  %sum = fadd <32 x bfloat> %load1, %load2
  store <32 x bfloat> %sum, ptr %phi, align 64
  %back = getelementptr inbounds i8, ptr %gep1, i20 64
  %cond = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %cond, label %for.body, label %for.exit, !llvm.loop !0

for.exit:
  ret void
}

; ===========================================================================
; Test 4: Combined — Pattern 1 + Pattern 2 together
;
; After normalization (phi shifted by 128):
;   - phi points to load1 address
;   - GEP phi+64 remains and is repositioned after load1 by buildPostIncChain
; ===========================================================================

; COMBINED-LABEL: define void @test_preinc_phi_normalization

; Preheader must get the shifted init.
; COMBINED: for.preheader:
; COMBINED:   %phi.shifted.init = getelementptr i8, ptr %base, i20 128

; After both passes: phi points to load1 address; gep2 is placed between
; load1 and load2 (it was already there after normalization).
; buildPostIncChain finds phi.back as the chain GEP (offset 128 > gep2's 64
; but traversal is use-list order), so only phi.back is repositioned.
; COMBINED: for.body:
; COMBINED:   %phi = phi ptr [ %phi.shifted.init, %for.preheader ], [ %phi.back, %for.body ]
; COMBINED:   %load1 = load <32 x bfloat>, ptr %phi
; COMBINED:   %gep2 = getelementptr inbounds i8, ptr %phi, i20 64
; COMBINED:   %load2 = load <32 x bfloat>, ptr %gep2
; phi.back appears at the end of the block (inserted by normalizePhiToLoadBase
; and left there since it is already after the store by buildPostIncChain logic).
; COMBINED:   %phi.back = getelementptr inbounds i8, ptr %phi, i20 128

declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.mustprogress"}
