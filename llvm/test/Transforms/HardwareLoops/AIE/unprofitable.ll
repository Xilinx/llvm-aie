;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: opt -mtriple=aie2 --passes="hardware-loops" --enable-aie-hardware-loops %s -S -o - | \
; RUN:     FileCheck %s
; RUN: opt -mtriple=aie2 --passes="hardware-loops" --enable-aie-hardware-loops \
; RUN:                                   --enable-aie-zero-overhead-loops %s -S -o - | \
; RUN:     FileCheck --check-prefix=CHECK-ZOL %s
; RUN: opt -mtriple=aie2 --passes="hardware-loops" --enable-aie-hardware-loops \
; RUN:     -pass-remarks-analysis=hardware-loops  %s -S -o - 2>&1 | \
; RUN:     FileCheck %s --check-prefix=CHECK-REMARKS


; CHECK-REMARKS: remark: <unknown>:0:0: hardware-loop not created: it's not profitable to create a hardware-loop
; CHECK-REMARKS: remark: <unknown>:0:0: hardware-loop not created: it's not profitable to create a hardware-loop
; CHECK-REMARKS: remark: <unknown>:0:0: hardware-loop not created: it's not profitable to create a hardware-loop



; Backedge taken count cannot be computed
define i32 @early_exit(ptr nocapture readonly %a, i32 %max, i32 %n) {
; CHECK-LABEL: @early_exit(
; CHECK-NOT: llvm.start.loop.iterations
; CHECK-NOT: llvm.set.loop.iterations
; CHECK-NOT: llvm.loop.decrement
;
; CHECK-ZOL-NOT: llvm.start.loop.iterations
; CHECK-ZOL-NOT: llvm.set.loop.iterations
; CHECK-ZOL-NOT: llvm.loop.decrement
entry:
  br label %do.body

do.body:
  %i.0 = phi i32 [ 0, %entry ], [ %inc, %if.end ]
  %arrayidx = getelementptr inbounds i32, ptr %a, i32 %i.0
  %0 = load i32, ptr %arrayidx, align 4
  %cmp = icmp sgt i32 %0, %max
  br i1 %cmp, label %do.end, label %if.end

if.end:
  %inc = add nuw i32 %i.0, 1
  %cmp1 = icmp ult i32 %inc, %n
  br i1 %cmp1, label %do.body, label %if.end.do.end_crit_edge

if.end.do.end_crit_edge:
  %arrayidx2.phi.trans.insert = getelementptr inbounds i32, ptr %a, i32 %inc
  %.pre = load i32, ptr %arrayidx2.phi.trans.insert, align 4
  br label %do.end

do.end:
  %1 = phi i32 [ %.pre, %if.end.do.end_crit_edge ], [ %0, %do.body ]
  ret i32 %1
}


; FIXME: We want to support nested loops, but need to check if the current
; loop level already contains hardware loop intrinsics. Just checking all
; instruction of the blocks contained in the loop would give false positives
define i32 @pre_existing(i32 %n, ptr nocapture %p, ptr nocapture readonly %q) {
; CHECK-LABEL: @pre_existing(
; CHECK: llvm.start.loop.iterations
; CHECK-NEXT: llvm.start.loop.iterations
; CHECK-ZOL:      llvm.start.loop.iterations
; CHECK-ZOL-NEXT: llvm.set.loop.iterations
entry:
  %start = call i32 @llvm.start.loop.iterations.i32(i32 %n)
  br label %while.body

while.body:                                       ; preds = %while.body, %entry
  %q.addr.05 = phi ptr [ %incdec.ptr, %while.body ], [ %q, %entry ]
  %p.addr.04 = phi ptr [ %incdec.ptr1, %while.body ], [ %p, %entry ]
  %0 = phi i32 [ %start, %entry ], [ %2, %while.body ]
  %incdec.ptr = getelementptr inbounds i32, ptr %q.addr.05, i32 1
  %1 = load i32, ptr %q.addr.05, align 4
  %incdec.ptr1 = getelementptr inbounds i32, ptr %p.addr.04, i32 1
  store i32 %1, ptr %p.addr.04, align 4
  %2 = call i32 @llvm.loop.decrement.reg.i32(i32 %0, i32 1)
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %while.body, label %while.end

while.end:                                        ; preds = %while.body
  ret i32 0
}

; FIXME: same as above: detect pre-existing loop
define i32 @pre_existing_test_set(i32 %n, ptr nocapture %p, ptr nocapture readonly %q) {
; CHECK-LABEL: @pre_existing_test_set(
; CHECK:    llvm.test.start.loop.iterations
; CHECK:    llvm.start.loop.iterations
; CHECK:    llvm.loop.decrement
; CHECK:    llvm.loop.decrement
; CHECK:       while.end:
;
; CHECK-ZOL-LABEL: @pre_existing_test_set(
; CHECK-ZOL:    llvm.test.start.loop.iterations
; CHECK-ZOL:    llvm.set.loop.iterations
; CHECK-ZOL:    llvm.loop.decrement.reg
; CHECK-ZOL:    llvm.loop.decrement
; CHECK-ZOL-NEXT:    br i1
;
entry:
  %guard = call { i32, i1 } @llvm.test.start.loop.iterations.i32(i32 %n)
  %g0 = extractvalue { i32, i1 } %guard, 0
  %g1 = extractvalue { i32, i1 } %guard, 1
  br i1 %g1, label %while.preheader, label %while.end

while.preheader:
  br label %while.body

while.body:                                       ; preds = %while.body, %entry
  %q.addr.05 = phi ptr [ %incdec.ptr, %while.body ], [ %q, %while.preheader ]
  %p.addr.04 = phi ptr [ %incdec.ptr1, %while.body ], [ %p, %while.preheader ]
  %0 = phi i32 [ %g0, %while.preheader ], [ %2, %while.body ]
  %incdec.ptr = getelementptr inbounds i32, ptr %q.addr.05, i32 1
  %1 = load i32, ptr %q.addr.05, align 4
  %incdec.ptr1 = getelementptr inbounds i32, ptr %p.addr.04, i32 1
  store i32 %1, ptr %p.addr.04, align 4
  %2 = call i32 @llvm.loop.decrement.reg.i32(i32 %0, i32 1)
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %while.body, label %while.end

while.end:                                        ; preds = %while.body
  ret i32 0
}

; FIXME: same as above: detect pre-existing loop
define void @pre_existing_inner(ptr nocapture %A, i32 %N) {
; CHECK-LABEL: @pre_existing_inner(
; CHECK:    llvm.start.loop.iterations
; CHECK:    llvm.start.loop.iterations
; CHECK:    llvm.start.loop.iterations
; CHECK:    call i32 @llvm.loop.decrement.reg
; CHECK:    call i32 @llvm.loop.decrement.reg
; CHECK:    icmp ne
; CHECK:    br i1
; CHECK:    llvm.loop.decrement.reg
; CHECK:    icmp ne
; CHECK:    br i1
;
; CHECK-ZOL-LABEL: @pre_existing_inner(
; CHECK-ZOL:    llvm.start.loop.iterations
; CHECK-ZOL:    llvm.start.loop.iterations
; CHECK-ZOL:    llvm.set.loop.iterations
; CHECK-ZOL:    llvm.loop.decrement.reg
; CHECK-ZOL:    llvm.loop.decrement
; CHECK-ZOL:    br i1
; CHECK-ZOL:    llvm.loop.decrement.reg
; CHECK-ZOL:    icmp ne
; CHECK-ZOL:    br i1
;
entry:
  %cmp20 = icmp eq i32 %N, 0
  br i1 %cmp20, label %while.end7, label %while.cond1.preheader.us

while.cond1.preheader.us:
  %i.021.us = phi i32 [ %inc6.us, %while.cond1.while.end_crit_edge.us ], [ 0, %entry ]
  %mul.us = mul i32 %i.021.us, %N
  %start = call i32 @llvm.start.loop.iterations.i32(i32 %N)
  br label %while.body3.us

while.body3.us:
  %j.019.us = phi i32 [ 0, %while.cond1.preheader.us ], [ %inc.us, %while.body3.us ]
  %0 = phi i32 [ %start, %while.cond1.preheader.us ], [ %1, %while.body3.us ]
  %add.us = add i32 %j.019.us, %mul.us
  %arrayidx.us = getelementptr inbounds i32, ptr %A, i32 %add.us
  store i32 %add.us, ptr %arrayidx.us, align 4
  %inc.us = add nuw i32 %j.019.us, 1
  %1 = call i32 @llvm.loop.decrement.reg.i32(i32 %0, i32 1)
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %while.body3.us, label %while.cond1.while.end_crit_edge.us

while.cond1.while.end_crit_edge.us:
  %inc6.us = add nuw i32 %i.021.us, 1
  %exitcond23 = icmp eq i32 %inc6.us, %N
  br i1 %exitcond23, label %while.end7, label %while.cond1.preheader.us

while.end7:
  ret void
}

define void @multi_latch(ptr %a, ptr %b, i32 %N) {
; CHECK-LABEL: @multi_latch(
; CHECK-NOT: llvm.start.loop.iterations
; CHECK-NOT: llvm.set.loop.iterations
; CHECK-NOT: llvm.loop.decrement
; CHECK-LABEL: exit
;
; CHECK-ZOL-LABEL: @multi_latch(
; CHECK-ZOL-NOT: llvm.start.loop.iterations
; CHECK-ZOL-NOT: llvm.set.loop.iterations
; CHECK-ZOL-NOT: llvm.loop.decrement
; CHECK-ZOL-LABEL: exit
entry:
  %half = lshr i32 %N, 1
  br label %header

header:
  %iv = phi i32 [ 0, %entry ], [ %count.next, %latch.0 ], [ %count.next, %latch.1 ]
  %cmp = icmp ult i32 %iv, %half
  %addr.a = getelementptr i32, ptr %a, i32 %iv
  %addr.b = getelementptr i32, ptr %b, i32 %iv
  br i1 %cmp, label %if.then, label %if.else

if.then:
  store i32 %iv, ptr %addr.a
  br label %latch.0

if.else:
  store i32 %iv, ptr %addr.b
  br label %latch.0

latch.0:
  %count.next = add nuw i32 %iv, 1
  %cmp.1 = icmp ult i32 %count.next, %half
  br i1 %cmp.1, label %header, label %latch.1

latch.1:
  %ld = load i32, ptr %addr.a
  store i32 %ld, ptr %addr.b
  %cmp.2 = icmp ult i32 %count.next, %N
  br i1 %cmp.2, label %header, label %latch.1

exit:
  ret void
}

declare i32 @llvm.start.loop.iterations.i32(i32) #0
declare { i32, i1 } @llvm.test.start.loop.iterations.i32(i32) #0
declare i32 @llvm.loop.decrement.reg.i32(i32, i32) #0
