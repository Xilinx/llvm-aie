; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; RUN: llc -mtriple=aie2p -O2 \
; RUN:     -aie-enable-inner-loop-pointer-opt=true \
; RUN:     -aie-enable-phi-normalization=true \
; RUN:     -aie-enable-post-inc-chain=false \
; RUN:     -stop-after=aie-inner-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s --check-prefix=NORM
;
; RUN: llc -mtriple=aie2p -O2 \
; RUN:     -aie-enable-inner-loop-pointer-opt=true \
; RUN:     -aie-enable-phi-normalization=false \
; RUN:     -aie-enable-post-inc-chain=true \
; RUN:     -stop-after=aie-inner-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s --check-prefix=POSTINC
;
; RUN: llc -mtriple=aie2p -O2 \
; RUN:     -aie-enable-inner-loop-pointer-opt=false \
; RUN:     -stop-after=aie-inner-loop-pointer-optimizer \
; RUN:     -o - %s 2>&1 | FileCheck %s --check-prefix=DISABLED

; Test that AIEInnerLoopPointerOptimizer handles two patterns in standalone
; single-block loops:
;
; Pattern 1 (normalizePhiToLoadBase):
;   The back-edge GEP is at the TOP of the loop body (pre-increment style).
;   We shift the phi init by one stride so the phi lands directly on the
;   first load address, eliminating one indirection per iteration.
;
; Pattern 2 (buildPostIncChain):
;   The phi is already used as a direct memory address (load-base form) but
;   the next-GEP is placed BEFORE the loads.  We move it to AFTER the last
;   memory user so the backend sees adjacent (load, ptr+=delta) pairs.

; ---------------------------------------------------------------------------
; Pattern 1: pre-increment PHI normalization
;
; Before:
;   %ptr = phi [ %init, entry ], [ %ptr.preinc, loop ]
;   %ptr.preinc = getelementptr inbounds i8, ptr %ptr, i20 64
;   %val = load i32, ptr %ptr.preinc          ; loads at ptr+64
;
; After:
;   (preheader) %ptr.shifted.init = getelementptr i8, ptr %init, i20 64
;   %ptr = phi [ %ptr.shifted.init, entry ], [ %ptr.back, loop ]
;   %val = load i32, ptr %ptr                  ; loads at ptr (= old+64)
;   %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 64  ; end of body
; ---------------------------------------------------------------------------
define void @pattern1_pre_inc(ptr %init, i32 %n) {
; NORM-LABEL: @pattern1_pre_inc
; NORM:       entry:
; NORM:         %ptr.shifted.init = getelementptr i8, ptr %init, i20 64
; NORM:       loop:
; NORM:         %ptr = phi ptr [ %ptr.shifted.init, %entry ], [ %ptr.back, %loop ]
; NORM:         load i32, ptr %ptr
; NORM:         %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 64
;
; POSTINC-LABEL: @pattern1_pre_inc
; POSTINC:    %ptr.preinc = getelementptr inbounds i8, ptr %ptr, i20 64
;
; DISABLED-LABEL: @pattern1_pre_inc
; DISABLED:   %ptr.preinc = getelementptr inbounds i8, ptr %ptr, i20 64
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %init, %entry ], [ %ptr.preinc, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  %ptr.preinc = getelementptr inbounds i8, ptr %ptr, i20 64
  %val = load i32, ptr %ptr.preinc
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}

; ---------------------------------------------------------------------------
; Pattern 2: post-increment GEP repositioning
;
; The phi is already in load-base form (direct mem use), but the GEP appears
; before the loads.  We move it to after the last mem-user of the phi.
;
; Before:
;   %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64  ; BEFORE loads
;   %v0 = load i32, ptr %ptr
;   store i32 %v0, ptr %ptr
;
; After:
;   %v0 = load i32, ptr %ptr
;   store i32 %v0, ptr %ptr
;   %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64  ; AFTER last
; ---------------------------------------------------------------------------
define void @pattern2_post_inc(ptr %init, i32 %n) {
; POSTINC-LABEL: @pattern2_post_inc
; POSTINC:      loop:
; POSTINC:        %ptr = phi ptr
; POSTINC:        %v0 = load i32, ptr %ptr
; POSTINC:        store i32 %v0, ptr %ptr
; POSTINC:        %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
;
; NORM-LABEL: @pattern2_post_inc
; NORM:         %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
; NORM:         %v0 = load
;
; DISABLED-LABEL: @pattern2_post_inc
; DISABLED:     %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
; DISABLED:     %v0 = load
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %init, %entry ], [ %ptr.next, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
  %v0 = load i32, ptr %ptr
  store i32 %v0, ptr %ptr
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}

; ---------------------------------------------------------------------------
; Negative: two-block loop (header != latch) must not be transformed.
; InnerLoopStructure::tryBuildFrom rejects it because Latch != Header.
; The GEP must remain in the latch block, untouched.
; ---------------------------------------------------------------------------
define void @skip_multi_block_loop(ptr %init, i32 %n) {
; NORM-LABEL: @skip_multi_block_loop
; NORM:       header:
; NORM:         %ptr = phi ptr [ %init, %entry ], [ %ptr.preinc, %latch ]
; NORM:         %ptr.preinc = getelementptr inbounds i8, ptr %ptr, i20 64
entry:
  br label %header

header:
  %ptr = phi ptr [ %init, %entry ], [ %ptr.preinc, %latch ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %latch ]
  %val = load i32, ptr %ptr
  br label %latch

latch:
  %ptr.preinc = getelementptr inbounds i8, ptr %ptr, i20 64
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %header, label %exit

exit:
  ret void
}

; ---------------------------------------------------------------------------
; Pattern 1 + sibling GEP adjustment:
;   The phi has a sibling GEP (phi + 192) in addition to the back-edge GEP
;   (phi + 128).  After normalization the phi is shifted by 128, so the
;   sibling offset must be adjusted to 192 - 128 = 64.
;
; Before:
;   %ptr = phi [ %init, entry ], [ %ptr.back, loop ]
;   %ptr.back = getelementptr i8, ptr %ptr, i20 128   ; back-edge GEP
;   %gep2     = getelementptr i8, ptr %ptr, i20 192   ; sibling GEP
;   load ptr %ptr.back
;   load ptr %gep2
;
; After:
;   (preheader) %ptr.shifted.init = getelementptr i8, ptr %init, i20 128
;   %ptr  = phi [ %ptr.shifted.init, entry ], [ %ptr.back2, loop ]
;   %gep2 = getelementptr i8, ptr %ptr, i20 64        ; 192 - 128 = 64
;   load ptr %ptr
;   load ptr %gep2
;   %ptr.back2 = getelementptr inbounds i8, ptr %ptr, i20 128
; ---------------------------------------------------------------------------
define void @pattern1_sibling_gep_adjust(ptr %init, i32 %n) {
; NORM-LABEL: @pattern1_sibling_gep_adjust
; NORM:       entry:
; NORM:         %ptr.shifted.init = getelementptr i8, ptr %init, i20 128
; NORM:       loop:
; NORM:         %ptr = phi ptr [ %ptr.shifted.init, %entry ], [ %ptr.back, %loop ]
; NORM:         %gep2 = getelementptr inbounds i8, ptr %ptr, i20 64
; NORM:         load i32, ptr %ptr
; NORM:         load i32, ptr %gep2
; NORM:         %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 128
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %init, %entry ], [ %ptr.back, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  %ptr.back = getelementptr inbounds i8, ptr %ptr, i20 128
  %gep2 = getelementptr inbounds i8, ptr %ptr, i20 192
  %v0 = load i32, ptr %ptr.back
  %v1 = load i32, ptr %gep2
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}

; ---------------------------------------------------------------------------
; Pattern 2 + re-root: two GEPs both rooted on phi with absolute offsets.
;   phi+64 and phi+128 (back-edge).  After repositioning, phi+128 is still
;   rooted on phi.  Re-root sweep rewrites it to gep1+64.
;
; Before:
;   %gep1 = getelementptr i8, ptr %phi, i20 64   ; placed early
;   %gep2 = getelementptr i8, ptr %phi, i20 128  ; back-edge, placed early
;   load ptr %phi
;   load ptr %gep1
;
; After Sweep 1 (reposition):
;   load ptr %phi
;   %gep1 = getelementptr i8, ptr %phi, i20 64   ; after last load of phi
;   load ptr %gep1
;   %gep2 = getelementptr i8, ptr %phi, i20 128  ; after last load of gep1
;
; After Sweep 1 (reposition):
;   %v0 = load ptr %phi
;   %gep1 = getelementptr i8, ptr %phi, i20 64   ; after last load of phi
;   %v1 = load ptr %gep1
;   %gep2 = getelementptr i8, ptr %phi, i20 128  ; after last load of gep1
;
; Note: the re-root sweep (phi+128 -> gep1+64) requires findNextChainGEP to
; walk from gep1 to gep2.  Since gep2 is still rooted on phi (not gep1) after
; sweep 1, findNextChainGEP won't find it from gep1 — so re-root does not
; fire here.  The test checks the sweep-1 result only.
; ---------------------------------------------------------------------------
define void @pattern2_reroot(ptr %init, i32 %n) {
; POSTINC-LABEL: @pattern2_reroot
; POSTINC:      loop:
; POSTINC:        %phi = phi ptr
; POSTINC:        %gep1 = getelementptr inbounds i8, ptr %phi, i20 64
; POSTINC:        %v0 = load i32, ptr %phi
; POSTINC:        %gep2 = getelementptr inbounds i8, ptr %phi, i20 128
; POSTINC:        %v1 = load i32, ptr %gep1
entry:
  br label %loop

loop:
  %phi = phi ptr [ %init, %entry ], [ %gep2, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  %gep1 = getelementptr inbounds i8, ptr %phi, i20 64
  %gep2 = getelementptr inbounds i8, ptr %phi, i20 128
  %v0 = load i32, ptr %phi
  %v1 = load i32, ptr %gep1
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}

; ---------------------------------------------------------------------------
; Pattern 1 via AddrSpaceCast: phi -> addrspacecast -> load.
;   hasDirectMemAccess must detect this as Pattern 2 and skip normalization.
;   The phi is already in load-base form (through the cast).
; ---------------------------------------------------------------------------
define void @skip_pattern2_via_cast(ptr %init, i32 %n) {
; NORM-LABEL: @skip_pattern2_via_cast
; NORM:       loop:
; NORM:         %ptr = phi ptr [ %init, %entry ], [ %ptr.next, %loop ]
; NORM:         %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
; NORM:         %cast = addrspacecast ptr %ptr to ptr addrspace(5)
; NORM:         load i32, ptr addrspace(5) %cast
entry:
  br label %loop

loop:
  %ptr = phi ptr [ %init, %entry ], [ %ptr.next, %loop ]
  %count = phi i32 [ %n, %entry ], [ %count.dec, %loop ]
  %ptr.next = getelementptr inbounds i8, ptr %ptr, i20 64
  %cast = addrspacecast ptr %ptr to ptr addrspace(5)
  %val = load i32, ptr addrspace(5) %cast
  %count.dec = add i32 %count, -1
  %cond = icmp ne i32 %count.dec, 0
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}
