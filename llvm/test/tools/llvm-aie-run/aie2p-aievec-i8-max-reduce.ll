; ===- aie2p-aievec-i8-max-reduce.ll -------------------------------------===;
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
;===---------------------------------------------------------------------===;
;
; mlir-aie's test/unit_tests/aievec_tests/aie2/i8_max_reduce through its own
; pipeline, so @dut is the compiler's output. This is the end-to-end exercise of
; VSHIFT: the kernel maxes the array into 64 lanes and then collapses those with
; a shift tree of 32, 16, 8, 4, 2 and 1 BYTES, reading lane 0 at the end.
;
; The inputs are built so every way of getting VSHIFT wrong prints a different
; byte. The winning value sits in lane 63, the furthest possible from the lane
; that is finally read, so it only arrives if all six shifts move the right
; distance in the right direction:
;
;   in[63] = 42    -> the answer, 0x2a
;   in[10] = 20    -> what a tree that loses lane 63 prints, 0x14
;   elsewhere -100 -> what a tree that loses both prints, 0x9c
;
; 0x9c is also what an UNSIGNED max prints, since -100 and the -128 the kernel
; seeds with are 156 and 128 read that way, both above 42. So one byte
; distinguishes four outcomes.
;
;===---------------------------------------------------------------------===;

; REQUIRES: ld_lld
; RUN: llc -mtriple=aie2p -O2 -filetype=obj %s -o %t.o
; RUN: ld.lld -e _start --section-start=.text=0x1000 \
; RUN:   --section-start=.bss=0x30000 -o %t.elf %t.o
; RUN: llvm-aie-run %t.elf --scratch=0x0:0x1000 --dump-mem=0x30400:1 \
; RUN:   --dump-mem=0x30404:4 | FileCheck %s

@in = global [1024 x i8] zeroinitializer, align 32
@out = global i8 0, align 4
@status = global i32 0, align 4

define void @dut(ptr %0, ptr %1, i64 %2, i64 %3, i64 %4, ptr %5, ptr %6, i64 %7) {
  %9 = insertvalue { ptr, ptr, i64 } poison, ptr %5, 0
  %10 = insertvalue { ptr, ptr, i64 } %9, ptr %6, 1
  %11 = insertvalue { ptr, ptr, i64 } %10, i64 %7, 2
  %12 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } poison, ptr %0, 0
  %13 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %12, ptr %1, 1
  %14 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %13, i64 %2, 2
  %15 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %14, i64 %3, 3, 0
  %16 = insertvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %15, i64 %4, 4, 0
  br label %17

17:                                               ; preds = %21, %8
  %18 = phi i64 [ %27, %21 ], [ 0, %8 ]
  %19 = phi <64 x i8> [ %26, %21 ], [ splat (i8 -128), %8 ]
  %20 = icmp slt i64 %18, 1024
  br i1 %20, label %21, label %28

21:                                               ; preds = %17
  %22 = extractvalue { ptr, ptr, i64, [1 x i64], [1 x i64] } %16, 1
  %23 = getelementptr i8, ptr %22, i64 %18
  %24 = load <64 x i8>, ptr %23, align 1
  %25 = call { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8> %19, <64 x i8> %24, i32 1)
  %26 = extractvalue { <64 x i8>, <2 x i32> } %25, 0
  %27 = add i64 %18, 64
  br label %17

28:                                               ; preds = %17
  %29 = bitcast <64 x i8> %19 to <16 x i32>
  %30 = bitcast <64 x i8> %19 to <16 x i32>
  %31 = call <16 x i32> @llvm.aie2p.vshift.I512.I512(<16 x i32> %29, <16 x i32> %30, i32 0, i32 32)
  %32 = bitcast <16 x i32> %31 to <64 x i8>
  %33 = call { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8> %19, <64 x i8> %32, i32 1)
  %34 = extractvalue { <64 x i8>, <2 x i32> } %33, 0
  %35 = bitcast <64 x i8> %34 to <16 x i32>
  %36 = bitcast <64 x i8> %34 to <16 x i32>
  %37 = call <16 x i32> @llvm.aie2p.vshift.I512.I512(<16 x i32> %35, <16 x i32> %36, i32 0, i32 16)
  %38 = bitcast <16 x i32> %37 to <64 x i8>
  %39 = call { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8> %34, <64 x i8> %38, i32 1)
  %40 = extractvalue { <64 x i8>, <2 x i32> } %39, 0
  %41 = bitcast <64 x i8> %40 to <16 x i32>
  %42 = bitcast <64 x i8> %40 to <16 x i32>
  %43 = call <16 x i32> @llvm.aie2p.vshift.I512.I512(<16 x i32> %41, <16 x i32> %42, i32 0, i32 8)
  %44 = bitcast <16 x i32> %43 to <64 x i8>
  %45 = call { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8> %40, <64 x i8> %44, i32 1)
  %46 = extractvalue { <64 x i8>, <2 x i32> } %45, 0
  %47 = bitcast <64 x i8> %46 to <16 x i32>
  %48 = bitcast <64 x i8> %46 to <16 x i32>
  %49 = call <16 x i32> @llvm.aie2p.vshift.I512.I512(<16 x i32> %47, <16 x i32> %48, i32 0, i32 4)
  %50 = bitcast <16 x i32> %49 to <64 x i8>
  %51 = call { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8> %46, <64 x i8> %50, i32 1)
  %52 = extractvalue { <64 x i8>, <2 x i32> } %51, 0
  %53 = bitcast <64 x i8> %52 to <16 x i32>
  %54 = bitcast <64 x i8> %52 to <16 x i32>
  %55 = call <16 x i32> @llvm.aie2p.vshift.I512.I512(<16 x i32> %53, <16 x i32> %54, i32 0, i32 2)
  %56 = bitcast <16 x i32> %55 to <64 x i8>
  %57 = call { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8> %52, <64 x i8> %56, i32 1)
  %58 = extractvalue { <64 x i8>, <2 x i32> } %57, 0
  %59 = bitcast <64 x i8> %58 to <16 x i32>
  %60 = bitcast <64 x i8> %58 to <16 x i32>
  %61 = call <16 x i32> @llvm.aie2p.vshift.I512.I512(<16 x i32> %59, <16 x i32> %60, i32 0, i32 1)
  %62 = bitcast <16 x i32> %61 to <64 x i8>
  %63 = call { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8> %58, <64 x i8> %62, i32 1)
  %64 = extractvalue { <64 x i8>, <2 x i32> } %63, 0
  %65 = extractelement <64 x i8> %64, i32 0
  %66 = extractvalue { ptr, ptr, i64 } %11, 1
  store i8 %65, ptr %66, align 1
  ret void
}

define void @_start() {
entry:
  br label %fill

fill:
  %i = phi i32 [ 0, %entry ], [ %i.next, %fill ]
  %p = getelementptr inbounds [1024 x i8], ptr @in, i32 0, i32 %i
  store i8 -100, ptr %p, align 1
  %i.next = add i32 %i, 1
  %filled = icmp eq i32 %i.next, 1024
  br i1 %filled, label %seed, label %fill

seed:
  %p10 = getelementptr inbounds [1024 x i8], ptr @in, i32 0, i32 10
  %p63 = getelementptr inbounds [1024 x i8], ptr @in, i32 0, i32 63
  store i8 20, ptr %p10, align 1
  store i8 42, ptr %p63, align 1
  br label %run

run:
  call void @dut(ptr @in, ptr @in, i64 0, i64 1024, i64 1,
                 ptr @out, ptr @out, i64 0)
  br label %check

; The reference reduction, seeded the way the kernel seeds it.
check:
  %j = phi i32 [ 0, %run ], [ %j.next, %check ]
  %acc = phi i8 [ -128, %run ], [ %acc.next, %check ]
  %q = getelementptr inbounds [1024 x i8], ptr @in, i32 0, i32 %j
  %v = load i8, ptr %q, align 1
  %gt = icmp sgt i8 %v, %acc
  %acc.next = select i1 %gt, i8 %v, i8 %acc
  %j.next = add i32 %j, 1
  %done = icmp eq i32 %j.next, 1024
  br i1 %done, label %fin, label %check

fin:
  %got = load i8, ptr @out, align 1
  %eq = icmp eq i8 %got, %acc.next
  %bad = select i1 %eq, i32 0, i32 1
  store i32 %bad, ptr @status, align 4
  call void @llvm.aie2p.done()
  unreachable
}

declare void @llvm.aie2p.done()
declare <16 x i32> @llvm.aie2p.vshift.I512.I512(<16 x i32>, <16 x i32>, i32, i32)
declare { <64 x i8>, <2 x i32> } @llvm.aie2p.vmax.lt8(<64 x i8>, <64 x i8>, i32)

; CHECK: bundles: {{[0-9]+}}

; 42: the lane-63 value travelled the whole tree.
; CHECK: mem[0x30400] = 2a

; And it agrees with the reference reduction.
; CHECK: mem[0x30404] = 00 00 00 00
