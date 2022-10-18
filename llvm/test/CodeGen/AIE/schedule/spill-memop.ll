;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie -O2 -stop-before=post-RA-sched < %s | \
; RUN:   FileCheck --check-prefix=CHECK-MIR %s
; RUN: llc -mtriple=aie -O2 < %s | FileCheck %s

; Check that we properly annotate memoperands on spill and restore
; These are pro/epilog spills for callee saved regs

; CHECK-MIR: ST_SPIL_PTR killed {{.*}} :: (store (s32) into %stack.0)
; CHECK-MIR: ST_SPIL_PTR killed {{.*}} :: (store (s32) into %stack.1)
; CHECK_MIR: LDA_SPIL_PTR {{.*}}, implicit $sp :: (load (s32) from %stack.1)
; CHECK_MIR: LDB_spis {{.*}}, implicit $sp :: (load (s32) from %stack.0)

; Check that spill code is disambiguated from other loads and store
; We force spill of the link register by an external call, and see that
; it doesn't interfere with a load at the start and the store at the end.
; The scheduler should push the initial load before the spill of the
; link register, and should push the restore of the link register
; before the final store

; CHECK-LABEL: function:
; CHECK: lda     r12, [p0]
; CHECK: st.spil p6, [sp,
; CEHCK: st.spil lr, [sp,
; CHECK: ldb     lr, [sp,
; CHECK: st      r0, [p6]


define dso_local void @function(i32* nocapture %p) local_unnamed_addr {
entry:
  %0 = load i32, i32* %p
  %mul = mul nsw i32 %0, 5
  %call = tail call i32 @f0(i32 noundef %mul)
  store i32 %call, i32* %p
  ret void
}

declare dso_local i32 @f0(i32 noundef) local_unnamed_addr
