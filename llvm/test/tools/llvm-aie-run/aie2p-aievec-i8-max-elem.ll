; ===- aie2p-aievec-i8-max-elem.ll ---------------------------------------===;
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
;===---------------------------------------------------------------------===;
;
; The first VECTORISED aievec kernel wired up end to end. Its siblings here are
; the scalar variants; this one is mlir-aie's
; test/unit_tests/aievec_tests/aie2/i8xi8_max_elem, taken through that test's
; own pipeline with aie-target=aie2p, so @dut below is the compiler's output
; rather than anything written for this test.
;
; It exists because "the kernel runs to completion" is a much weaker claim than
; it sounds: the simulator can map zeroed scratch but cannot load memory, so a
; kernel run that way computes max(0,0) and would pass with almost any wrong
; semantics. Here the inputs are filled in-module first, so the stored bytes
; are a real answer.
;
; XFAIL, and the reason is NOT this kernel's semantics -- each opcode it uses is
; verified in isolation, and a hand-spaced vpush/vextract round trip is exact.
; It fails because the model has no BYPASS network. The itineraries carry one
; per operand: the MV-class vector ops here are [d MV_Bypass, s1 MV_Bypass, ...]
; while the memory ops are MemInstrItinData with no bypass column at all. The
; compiler schedules this chain back-to-back because the bypass forwards each
; result, and the register file alone -- read strictly, which is correct for the
; unbypassed memory ops that were measured -- hands back the stale value. Insert
; seven nops between the same ops and the answer is right.
;
; So the two rules are different per operand class and the model implements one
; of them. This flips to XPASS when the bypass network lands, which is the
; signal wanted.
;
; The inputs are chosen so a WRONG SIGNEDNESS survives neither check. in1 wraps
; past 127 at i = 41, and from there the two readings disagree on every element:
; signed keeps in0's small positive value, unsigned keeps in1's wrapped one.
; That the kernel asks for the signed form at all is the compiler's own call --
; it emits llvm.aie2p.vmax.lt8(..., i32 1), and 1 selects VMAX_LT_8_vaddSign1.
;
;===---------------------------------------------------------------------===;

; XFAIL: *
; REQUIRES: ld_lld
; RUN: llc -mtriple=aie2p -O2 -filetype=obj %s -o %t.o
; RUN: ld.lld -e _start --section-start=.text=0x1000 \
; RUN:   --section-start=.bss=0x30000 -o %t.elf %t.o
; RUN: llvm-aie-run %t.elf --scratch=0x0:0x1000 --dump-mem=0x30C00:4 \
; RUN:   --dump-mem=0x30800:8 --dump-mem=0x30828:8 | FileCheck %s

@in0 = global [1024 x i8] zeroinitializer, align 32
@in1 = global [1024 x i8] zeroinitializer, align 32
@out = global [1024 x i8] zeroinitializer, align 32
@status = global i32 0, align 4

define void @dut(ptr %0, ptr %1, i64 %2, i64 %3, i64 %4, ptr %5, ptr %6, i64 %7, i64 %8, i64 %9, ptr %10, ptr %11, i64 %12, i64 %13, i64 %14) {
  %16 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %10, 0
  %17 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %16, ptr %11, 1
  %18 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %17, i64 %12, 2
  %19 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %18, i64 %13, 3, 0
  %20 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %19, i64 %14, 4, 0
  %21 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %5, 0
  %22 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %21, ptr %6, 1
  %23 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %22, i64 %7, 2
  %24 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %23, i64 %8, 3, 0
  %25 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %24, i64 %9, 4, 0
  %26 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %0, 0
  %27 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %26, ptr %1, 1
  %28 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %27, i64 %2, 2
  %29 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %28, i64 %3, 3, 0
  %30 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %29, i64 %4, 4, 0
  br label %31

31:                                               ; preds = %34, %15
  %32 = phi i64 [ %45, %34 ], [ 0, %15 ]
  %33 = icmp slt i64 %32, 1024
  br i1 %33, label %34, label %46

34:                                               ; preds = %31
  %35 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %30, 1
  %36 = getelementptr i8, ptr %35, i64 %32
  %37 = load <64 x i8>, ptr %36, align 1
  %38 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %25, 1
  %39 = getelementptr i8, ptr %38, i64 %32
  %40 = load <64 x i8>, ptr %39, align 1
  %41 = call { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8> %37, <64 x i8> %40, i32 1)
  %42 = extractvalue { <64 x i8>, <2 x i32> } %41, 0
  %43 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %20, 1
  %44 = getelementptr i8, ptr %43, i64 %32
  store <64 x i8> %42, ptr %44, align 1
  %45 = add i64 %32, 64
  br label %31

46:                                               ; preds = %31
  ret void
}

define void @_start() {
entry:
  br label %fill

; in0[i] = i and in1[i] = 3i+7, both wrapping in i8. in1 goes negative at
; i = 41 while in0 is still small and positive, which is where a signed and an
; unsigned max part company.
fill:
  %i = phi i32 [ 0, %entry ], [ %i.next, %fill ]
  %a = trunc i32 %i to i8
  %b3 = mul i32 %i, 3
  %b7 = add i32 %b3, 7
  %b = trunc i32 %b7 to i8
  %pa = getelementptr inbounds [1024 x i8], ptr @in0, i32 0, i32 %i
  %pb = getelementptr inbounds [1024 x i8], ptr @in1, i32 0, i32 %i
  store i8 %a, ptr %pa, align 1
  store i8 %b, ptr %pb, align 1
  %i.next = add i32 %i, 1
  %filled = icmp eq i32 %i.next, 1024
  br i1 %filled, label %run, label %fill

run:
  call void @dut(ptr @in0, ptr @in0, i64 0, i64 1024, i64 1,
                 ptr @in1, ptr @in1, i64 0, i64 1024, i64 1,
                 ptr @out, ptr @out, i64 0, i64 1024, i64 1)
  br label %check

check:
  %j = phi i32 [ 0, %run ], [ %j.next, %check ]
  %bad = phi i32 [ 0, %run ], [ %bad.next, %check ]
  %qa = getelementptr inbounds [1024 x i8], ptr @in0, i32 0, i32 %j
  %qb = getelementptr inbounds [1024 x i8], ptr @in1, i32 0, i32 %j
  %qo = getelementptr inbounds [1024 x i8], ptr @out, i32 0, i32 %j
  %va = load i8, ptr %qa, align 1
  %vb = load i8, ptr %qb, align 1
  %vo = load i8, ptr %qo, align 1
  %ge = icmp sge i8 %va, %vb
  %ref = select i1 %ge, i8 %va, i8 %vb
  %eq = icmp eq i8 %ref, %vo
  %inc = select i1 %eq, i32 0, i32 1
  %bad.next = add i32 %bad, %inc
  %j.next = add i32 %j, 1
  %checked = icmp eq i32 %j.next, 1024
  br i1 %checked, label %fin, label %check

fin:
  store i32 %bad.next, ptr @status, align 4
  call void @llvm.aie2p.done()
  unreachable
}

declare void @llvm.aie2p.done()
declare { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8>, <64 x i8>, i32)

; CHECK: bundles: {{[0-9]+}}

; Zero mismatches over all 1024 elements. Weak alone -- the kernel and the
; reference are in one module and the comparison could fold -- so the bytes
; below carry the evidence.
; CHECK: mem[0x30C00] = 00 00 00 00

; out[0..7], where in1 is still positive and larger: 3i+7 = 7, 10, 13 ...
; CHECK: mem[0x30800] = 07 0a 0d 10 13 16 19 1c

; out[40..47], the discriminating window. in1 wraps negative from i = 41, so a
; SIGNED max keeps in0 (41, 42, 43 ...) and an unsigned one would keep in1's
; 0x82, 0x85, 0x88 ... Only i = 40 is shared, where in1 is still 127.
; CHECK: mem[0x30828] = 7f 29 2a 2b 2c 2d 2e 2f
