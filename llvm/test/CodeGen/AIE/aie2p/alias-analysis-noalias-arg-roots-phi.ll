;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; Loop-carried noalias arg roots, with aie-aa alone so BasicAA does not hide
; the tracer. Expectations are what the current tracer returns.
;
; RUN: opt -mtriple=aie2p -passes=aa-eval -aa-pipeline=aie-aa -print-all-alias-modref-info --aie-alias-analysis-noalias-arg-roots=true -disable-output < %s 2>&1 | FileCheck %s

; A self-loop GEP is the same object, but a revisit drops the root.
; CHECK-LABEL: Function: config_copy_loop
; CHECK: MayAlias:{{.*}}%ip{{.*}},{{.*}}%op

define void @config_copy_loop(ptr noalias %input, ptr noalias %output) {
entry:
  %ip0 = load ptr, ptr %input, align 4
  %op0 = load ptr, ptr %output, align 4
  br label %for.body

for.body:
  %i = phi i32 [ 0, %entry ], [ %i.next, %for.body ]
  %ip = phi ptr [ %ip0, %entry ], [ %ip.next, %for.body ]
  %op = phi ptr [ %op0, %entry ], [ %op.next, %for.body ]
  %v = load <32 x i32>, ptr %ip, align 64
  store <32 x i32> %v, ptr %op, align 64
  %ip.next = getelementptr inbounds i8, ptr %ip, i20 128
  %op.next = getelementptr inbounds i8, ptr %op, i20 128
  %i.next = add i32 %i, 1
  %done = icmp eq i32 %i.next, 32
  br i1 %done, label %exit, label %for.body

exit:
  ret void
}

; Same copy, with the step in a different block from the header.
; CHECK-LABEL: Function: config_copy_separate_latch
; CHECK: MayAlias:{{.*}}%ip{{.*}},{{.*}}%op

define void @config_copy_separate_latch(ptr noalias %input,
                                        ptr noalias %output) {
entry:
  %ip0 = load ptr, ptr %input, align 4
  %op0 = load ptr, ptr %output, align 4
  br label %header

header:
  %i = phi i32 [ 0, %entry ], [ %i.next, %latch ]
  %ip = phi ptr [ %ip0, %entry ], [ %ip.next, %latch ]
  %op = phi ptr [ %op0, %entry ], [ %op.next, %latch ]
  %v = load <32 x i32>, ptr %ip, align 64
  store <32 x i32> %v, ptr %op, align 64
  br label %latch

latch:
  %ip.next = getelementptr inbounds i8, ptr %ip, i20 128
  %op.next = getelementptr inbounds i8, ptr %op, i20 128
  %i.next = add i32 %i, 1
  %done = icmp eq i32 %i.next, 32
  br i1 %done, label %exit, label %header

exit:
  ret void
}

; The back edge can be %out. MayAlias is required.
; CHECK-LABEL: Function: loop_carried_select_self_loop
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @loop_carried_select_self_loop(ptr noalias %in, ptr noalias %out, i1 %cond) {
entry:
  br label %loop
loop:
  %p = phi ptr [ %in, %entry ], [ %p.next, %loop ]
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  %p.inc = getelementptr i8, ptr %p, i20 4
  %p.next = select i1 %cond, ptr %p.inc, ptr %out
  br i1 %cond, label %exit, label %loop
exit:
  ret void
}

; CHECK-LABEL: Function: loop_carried_increment
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @loop_carried_increment(ptr noalias %in, ptr noalias %out, i1 %cond) {
entry:
  br label %header
header:
  %p = phi ptr [ %in, %entry ], [ %p.next, %latch ]
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  br label %latch
latch:
  %p.next = getelementptr i8, ptr %p, i20 4
  br i1 %cond, label %exit, label %header
exit:
  ret void
}

; CHECK-LABEL: Function: nested_gep
; CHECK: MayAlias:{{.*}}%ii,{{.*}}%op{{$}}

define void @nested_gep(ptr noalias %in, ptr noalias %out, i1 %c1, i1 %c2) {
entry:
  br label %outer
outer:
  %ip = phi ptr [ %in,  %entry ], [ %ip.n, %oul ]
  %op = phi ptr [ %out, %entry ], [ %op.n, %oul ]
  br label %inner
inner:
  %ii = phi ptr [ %ip, %outer ], [ %ii.n, %inner ]
  load i32, ptr %ii, align 4
  store i32 0, ptr %op, align 4
  %ii.n = getelementptr i8, ptr %ii, i20 4
  br i1 %c2, label %oul, label %inner
oul:
  %ip.n = getelementptr i8, ptr %ip, i20 128
  %op.n = getelementptr i8, ptr %op, i20 128
  br i1 %c1, label %exit, label %outer
exit:
  ret void
}

; CHECK-LABEL: Function: nested_select
; CHECK: MayAlias:{{.*}}%ii,{{.*}}%op{{$}}

define void @nested_select(ptr noalias %in, ptr noalias %out, i1 %c1, i1 %c2) {
entry:
  br label %outer
outer:
  %ip = phi ptr [ %in,  %entry ], [ %ip.n, %oul ]
  %op = phi ptr [ %out, %entry ], [ %op.n, %oul ]
  br label %inner
inner:
  %ii = phi ptr [ %ip, %outer ], [ %ii.n, %inner ]
  load i32, ptr %ii, align 4
  store i32 0, ptr %op, align 4
  %ii.g = getelementptr i8, ptr %ii, i20 4
  %ii.n = select i1 %c2, ptr %ii.g, ptr %op
  br i1 %c2, label %oul, label %inner
oul:
  %ip.n = getelementptr i8, ptr %ip, i20 128
  %op.n = getelementptr i8, ptr %op, i20 128
  br i1 %c1, label %exit, label %outer
exit:
  ret void
}

; CHECK-LABEL: Function: mutual_swap
; CHECK: MayAlias:{{.*}}%a,{{.*}}%b{{$}}

define void @mutual_swap(ptr noalias %in, ptr noalias %out, i1 %c) {
entry:
  br label %loop
loop:
  %a = phi ptr [ %in,  %entry ], [ %b, %loop ]
  %b = phi ptr [ %out, %entry ], [ %a, %loop ]
  load i32, ptr %a, align 4
  store i32 0, ptr %b, align 4
  br i1 %c, label %exit, label %loop
exit:
  ret void
}

; CHECK-LABEL: Function: mutual_disjoint
; CHECK: MayAlias:{{.*}}%a,{{.*}}%b{{$}}

define void @mutual_disjoint(ptr noalias %in, ptr noalias %out, i1 %c) {
entry:
  br label %loop
loop:
  %a = phi ptr [ %in,  %entry ], [ %a.n, %loop ]
  %b = phi ptr [ %out, %entry ], [ %b.n, %loop ]
  load i32, ptr %a, align 4
  store i32 0, ptr %b, align 4
  %a.n = getelementptr i8, ptr %a, i20 4
  %b.n = getelementptr i8, ptr %b, i20 4
  br i1 %c, label %exit, label %loop
exit:
  ret void
}

; CHECK-LABEL: Function: interlocked_nested_gep
; CHECK: MayAlias:{{.*}}%it_in.l,{{.*}}%it_out.l{{$}}

define void @interlocked_nested_gep(ptr noalias %in_meta, ptr noalias %out_meta, i1 %c1, i1 %c2) {
entry:
  %src = load ptr, ptr %in_meta, align 4
  %dst = load ptr, ptr %out_meta, align 4
  br label %outer
outer:
  %it_in = phi ptr [ %src, %entry ], [ %add.in, %outer.latch ]
  %it_out = phi ptr [ %dst, %entry ], [ %add.out, %outer.latch ]
  br label %inner
inner:
  %it_in.l = phi ptr [ %it_in, %outer ], [ %add.in, %inner ]
  %it_out.l = phi ptr [ %it_out, %outer ], [ %add.out, %inner ]
  %add.in = getelementptr inbounds i8, ptr %it_in.l, i20 32
  %v = load <8 x i32>, ptr %it_in.l, align 64
  %add.out = getelementptr inbounds i8, ptr %it_out.l, i20 32
  store <8 x i32> %v, ptr %it_out.l, align 64
  br i1 %c2, label %inner, label %outer.latch
outer.latch:
  br i1 %c1, label %exit, label %outer
exit:
  ret void
}

; CHECK-LABEL: Function: interlocked_nested_select
; CHECK: MayAlias:{{.*}}%it_in.l,{{.*}}%it_out.l{{$}}

define void @interlocked_nested_select(ptr noalias %in_meta, ptr noalias %out_meta, i1 %c1, i1 %c2) {
entry:
  %src = load ptr, ptr %in_meta, align 4
  %dst = load ptr, ptr %out_meta, align 4
  br label %outer
outer:
  %it_in = phi ptr [ %src, %entry ], [ %add.in, %outer.latch ]
  %it_out = phi ptr [ %dst, %entry ], [ %add.out, %outer.latch ]
  br label %inner
inner:
  %it_in.l = phi ptr [ %it_in, %outer ], [ %add.in, %inner ]
  %it_out.l = phi ptr [ %it_out, %outer ], [ %add.out, %inner ]
  %gep.in = getelementptr inbounds i8, ptr %it_in.l, i20 32
  %add.in = select i1 %c2, ptr %gep.in, ptr %it_out.l
  %v = load <8 x i32>, ptr %it_in.l, align 64
  %add.out = getelementptr inbounds i8, ptr %it_out.l, i20 32
  store <8 x i32> %v, ptr %it_out.l, align 64
  br i1 %c2, label %inner, label %outer.latch
outer.latch:
  br i1 %c1, label %exit, label %outer
exit:
  ret void
}

; CHECK-LABEL: Function: three_way_ok
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @three_way_ok(ptr noalias %in, ptr noalias %out, i1 %c0, i1 %c) {
entry:
  br i1 %c0, label %p1, label %p2
p1:
  %a = getelementptr i8, ptr %in, i20 4
  br label %loop
p2:
  %b = getelementptr i8, ptr %in, i20 8
  br label %loop
loop:
  %p = phi ptr [ %a, %p1 ], [ %b, %p2 ], [ %p.n, %loop ]
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  %p.n = getelementptr i8, ptr %p, i20 4
  br i1 %c, label %exit, label %loop
exit:
  ret void
}

; CHECK-LABEL: Function: three_way_bad
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @three_way_bad(ptr noalias %in, ptr noalias %out, i1 %c0, i1 %c) {
entry:
  br i1 %c0, label %p1, label %p2
p1:
  %a = getelementptr i8, ptr %in, i20 4
  br label %loop
p2:
  %b = getelementptr i8, ptr %out, i20 8
  br label %loop
loop:
  %p = phi ptr [ %a, %p1 ], [ %b, %p2 ], [ %p.n, %loop ]
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  %p.n = getelementptr i8, ptr %p, i20 4
  br i1 %c, label %exit, label %loop
exit:
  ret void
}

; CHECK-LABEL: Function: irreducible_ok
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @irreducible_ok(ptr noalias %in, ptr noalias %out, i1 %c1, i1 %c2, i1 %c3) {
entry:
  br i1 %c1, label %a, label %b
a:
  %pa = phi ptr [ %in, %entry ], [ %pb.n, %b ]
  %pa.n = getelementptr i8, ptr %pa, i20 4
  br i1 %c2, label %b, label %exit
b:
  %pb = phi ptr [ %in, %entry ], [ %pa.n, %a ]
  %pb.n = getelementptr i8, ptr %pb, i20 8
  br i1 %c3, label %a, label %exit
exit:
  %p = phi ptr [ %pa, %a ], [ %pb, %b ]
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  ret void
}

; CHECK-LABEL: Function: irreducible_bad
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @irreducible_bad(ptr noalias %in, ptr noalias %out, i1 %c1, i1 %c2, i1 %c3) {
entry:
  br i1 %c1, label %a, label %b
a:
  %pa = phi ptr [ %in, %entry ], [ %pb.n, %b ]
  %pa.n = getelementptr i8, ptr %pa, i20 4
  br i1 %c2, label %b, label %exit
b:
  %pb = phi ptr [ %out, %entry ], [ %pa.n, %a ]
  %pb.n = getelementptr i8, ptr %pb, i20 8
  br i1 %c3, label %a, label %exit
exit:
  %p = phi ptr [ %pa, %a ], [ %pb, %b ]
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  ret void
}

; CHECK-LABEL: Function: three_phi_rotate
; CHECK: MayAlias:{{.*}}%a,{{.*}}%out{{$}}

define void @three_phi_rotate(ptr noalias %in, ptr noalias %out, ptr noalias %third, i1 %cc) {
entry:
  br label %loop
loop:
  %a = phi ptr [ %in, %entry ], [ %b, %loop ]
  %b = phi ptr [ %in, %entry ], [ %d, %loop ]
  %d = phi ptr [ %third, %entry ], [ %a, %loop ]
  load i32, ptr %a, align 4
  store i32 0, ptr %out, align 4
  br i1 %cc, label %exit, label %loop
exit:
  ret void
}

; CHECK-LABEL: Function: three_phi_rotate_ok
; CHECK: MayAlias:{{.*}}%a,{{.*}}%out{{$}}

define void @three_phi_rotate_ok(ptr noalias %in, ptr noalias %out, i1 %cc) {
entry:
  br label %loop
loop:
  %a = phi ptr [ %in, %entry ], [ %b, %loop ]
  %b = phi ptr [ %in, %entry ], [ %d, %loop ]
  %d = phi ptr [ %in, %entry ], [ %a, %loop ]
  load i32, ptr %a, align 4
  store i32 0, ptr %out, align 4
  br i1 %cc, label %exit, label %loop
exit:
  ret void
}

; CHECK-LABEL: Function: cycle_through_load
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @cycle_through_load(ptr noalias %in, ptr noalias %out, i1 %c) {
entry:
  br label %loop
loop:
  %p = phi ptr [ %in, %entry ], [ %q, %loop ]
  %q = load ptr, ptr %p, align 8
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  br i1 %c, label %exit, label %loop
exit:
  ret void
}

; CHECK-LABEL: Function: closed_phi_cycle
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @closed_phi_cycle(ptr noalias %out, i1 %c) {
entry:
  store i32 0, ptr %out, align 4
  ret void
dead1:
  %p = phi ptr [ %q, %dead2 ]
  load i32, ptr %p, align 4
  br label %dead2
dead2:
  %q = phi ptr [ %p, %dead1 ]
  br label %dead1
}

; CHECK-LABEL: Function: phi_undef_incoming
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @phi_undef_incoming(ptr noalias %in, ptr noalias %out, i1 %c) {
entry:
  br i1 %c, label %t, label %f
t:
  br label %j
f:
  br label %j
j:
  %p = phi ptr [ %in, %t ], [ poison, %f ]
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  ret void
}

; A covering insert of a sub-aggregate is not traced yet.
; CHECK-LABEL: Function: insert_prefix_overwrite
; CHECK: MayAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @insert_prefix_overwrite(ptr noalias %in, ptr noalias %out) {
entry:
  %i0 = insertvalue { { ptr, i32 }, i32 } poison, ptr %in, 0, 0
  %inner = insertvalue { ptr, i32 } poison, ptr %out, 0
  %i1 = insertvalue { { ptr, i32 }, i32 } %i0, { ptr, i32 } %inner, 0
  %p = extractvalue { { ptr, i32 }, i32 } %i1, 0, 0
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  ret void
}

; CHECK-LABEL: Function: insert_prefix_same_root
; CHECK: NoAlias:{{.*}}%out,{{.*}}%p{{$}}

define void @insert_prefix_same_root(ptr noalias %in, ptr noalias %out) {
entry:
  %i0 = insertvalue { { ptr, i32 }, i32 } poison, ptr %out, 0, 0
  %inner = insertvalue { ptr, i32 } poison, ptr %in, 0
  %i1 = insertvalue { { ptr, i32 }, i32 } %i0, { ptr, i32 } %inner, 0
  %p = extractvalue { { ptr, i32 }, i32 } %i1, 0, 0
  load i32, ptr %p, align 4
  store i32 0, ptr %out, align 4
  ret void
}

; CHECK-LABEL: Function: struct_field_gep_loop
; CHECK: MayAlias:{{.*}}%a,{{.*}}%out{{$}}

define void @struct_field_gep_loop(ptr noalias %in, ptr noalias %out, i1 %c) {
entry:
  %s0 = insertvalue { ptr, i32 } poison, ptr %in, 0
  br label %loop
loop:
  %s = phi { ptr, i32 } [ %s0, %entry ], [ %s.next, %loop ]
  %a = extractvalue { ptr, i32 } %s, 0
  %a.n = getelementptr i8, ptr %a, i20 4
  load i32, ptr %a, align 4
  store i32 0, ptr %out, align 4
  %s.next = insertvalue { ptr, i32 } %s, ptr %a.n, 0
  br i1 %c, label %exit, label %loop
exit:
  ret void
}

; CHECK-LABEL: Function: fifo_loop_phi_data_root
; CHECK: MayAlias:{{.*}}%pIn{{.*}},{{.*}}%pOut

define void @fifo_loop_phi_data_root(ptr noalias %in, ptr noalias %out, i1 %cond) {
entry:
  %init = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.ld.fill(ptr %in, <32 x i32> zeroinitializer, i32 0)
  br label %loop

loop:
  %state = phi { ptr, <32 x i32>, i32 } [ %init, %entry ], [ %next, %loop ]
  %pIn = extractvalue { ptr, <32 x i32>, i32 } %state, 0
  %next = call { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.ld.fill(ptr %pIn, <32 x i32> zeroinitializer, i32 0)
  %pOut = load ptr, ptr %out, align 8
  load i32, ptr %pIn, align 4
  store i32 0, ptr %pOut, align 4
  br i1 %cond, label %loop, label %exit

exit:
  ret void
}

declare { ptr, <32 x i32>, i32 } @llvm.aie2p.fifo.ld.fill(ptr, <32 x i32>, i32)
