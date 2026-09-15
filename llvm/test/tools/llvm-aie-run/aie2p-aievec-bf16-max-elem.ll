; ===- aie2p-aievec-bf16-max-elem.ll -------------------------------------===;
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
;===---------------------------------------------------------------------===;
;
; mlir-aie's test/unit_tests/aievec_tests/aie2/bf16xbf16_max_elem through its
; own pipeline. This is the only end-to-end exercise of the bf16 compare path,
; where the model converts each lane with APFloat::BFloat rather than comparing
; bit patterns.
;
; The four input pairs are chosen so neither integer shortcut can pass:
;
;   i%4 == 0   1.0 vs -1.0    -> 1.0    an UNSIGNED bit compare picks -1.0,
;                                       whose 0xBF80 is the larger word
;   i%4 == 1  -2.0 vs  0.5    -> 0.5
;   i%4 == 2  -1.0 vs -2.0    -> -1.0   a SIGNED bit compare picks -2.0, since
;                                       0xC000 > 0xBF80 read as i16
;   i%4 == 3   2.0 vs  1.0    -> 2.0
;
; The both-negative pair is the one that matters: sign-magnitude order reverses
; below zero, so an integer compare that survives the mixed-sign pairs still
; fails there. Inputs are written as raw i16 patterns, which is what bf16 is.
;
;===---------------------------------------------------------------------===;

; REQUIRES: ld_lld
; RUN: llc -mtriple=aie2p -O2 -filetype=obj %s -o %t.o
; RUN: ld.lld -e _start --section-start=.text=0x1000 \
; RUN:   --section-start=.bss=0x30000 -o %t.elf %t.o
; RUN: llvm-aie-run %t.elf --scratch=0x0:0x1000 --dump-mem=0x31000:8 \
; RUN:   --dump-mem=0x31800:4 | FileCheck %s

@in0 = global [1024 x i16] zeroinitializer, align 32
@in1 = global [1024 x i16] zeroinitializer, align 32
@out = global [1024 x i16] zeroinitializer, align 32
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
  %36 = getelementptr bfloat, ptr %35, i64 %32
  %37 = load <32 x bfloat>, ptr %36, align 2
  %38 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %25, 1
  %39 = getelementptr bfloat, ptr %38, i64 %32
  %40 = load <32 x bfloat>, ptr %39, align 2
  %41 = call { <32 x bfloat>, i32 } @llvm.aie2p.vmax.ltbf16(<32 x bfloat> %37, <32 x bfloat> %40)
  %42 = extractvalue { <32 x bfloat>, i32 } %41, 0
  %43 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %20, 1
  %44 = getelementptr bfloat, ptr %43, i64 %32
  store <32 x bfloat> %42, ptr %44, align 2
  %45 = add i64 %32, 32
  br label %31

46:                                               ; preds = %31
  ret void
}

define void @_start() {
entry:
  br label %fill

fill:
  %i = phi i32 [ 0, %entry ], [ %i.next, %fill ]
  %m = and i32 %i, 3
  %is0 = icmp eq i32 %m, 0
  %is1 = icmp eq i32 %m, 1
  %is2 = icmp eq i32 %m, 2
  ; a = 1.0, -2.0, -1.0, 2.0
  %a2 = select i1 %is2, i16 -16512, i16 16384
  %a1 = select i1 %is1, i16 -16384, i16 %a2
  %a = select i1 %is0, i16 16256, i16 %a1
  ; b = -1.0, 0.5, -2.0, 1.0
  %b2 = select i1 %is2, i16 -16384, i16 16256
  %b1 = select i1 %is1, i16 16128, i16 %b2
  %b = select i1 %is0, i16 -16512, i16 %b1
  %pa = getelementptr inbounds [1024 x i16], ptr @in0, i32 0, i32 %i
  %pb = getelementptr inbounds [1024 x i16], ptr @in1, i32 0, i32 %i
  store i16 %a, ptr %pa, align 2
  store i16 %b, ptr %pb, align 2
  %i.next = add i32 %i, 1
  %filled = icmp eq i32 %i.next, 1024
  br i1 %filled, label %run, label %fill

run:
  call void @dut(ptr @in0, ptr @in0, i64 0, i64 1024, i64 1,
                 ptr @in1, ptr @in1, i64 0, i64 1024, i64 1,
                 ptr @out, ptr @out, i64 0, i64 1024, i64 1)
  br label %check

; Expected = 1.0, 0.5, -1.0, 2.0 by the same index, written out rather than
; recomputed from a compare -- the point is not to re-derive the ordering here.
check:
  %j = phi i32 [ 0, %run ], [ %j.next, %check ]
  %bad = phi i32 [ 0, %run ], [ %bad.next, %check ]
  %n = and i32 %j, 3
  %e0 = icmp eq i32 %n, 0
  %e1 = icmp eq i32 %n, 1
  %e2 = icmp eq i32 %n, 2
  %r2 = select i1 %e2, i16 -16512, i16 16384
  %r1 = select i1 %e1, i16 16128, i16 %r2
  %ref = select i1 %e0, i16 16256, i16 %r1
  %qo = getelementptr inbounds [1024 x i16], ptr @out, i32 0, i32 %j
  %vo = load i16, ptr %qo, align 2
  %eq = icmp eq i16 %ref, %vo
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
declare { <32 x bfloat>, i32 } @llvm.aie2p.vmax.ltbf16(<32 x bfloat>, <32 x bfloat>)

; CHECK: bundles: {{[0-9]+}}

; out[0..3] = 1.0, 0.5, -1.0, 2.0 as bf16, little-endian.
; CHECK: mem[0x31000] = 80 3f 00 3f 80 bf 00 40

; CHECK: mem[0x31800] = 00 00 00 00
