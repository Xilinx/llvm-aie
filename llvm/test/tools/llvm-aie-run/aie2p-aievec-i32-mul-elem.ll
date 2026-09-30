;===- aie2p-aievec-i32-mul-elem.ll --------------------------------------===;
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
;===---------------------------------------------------------------------===;
;
; The third aievec kernel, and the only one that reaches the word-wide load.
;
; @dut is mlir-aie's
; test/unit_tests/aievec_tests/aie2/i32xi32_mul_elem/i32xi32_mul_elem-llvm-scalar.mlir
; through that test's own pipeline. Its subscript lowers to plain `lda`
; (LDA_dms_lda_idx); the i8 and i16 tests beside it reach lda.s8 and lda.s16
; and never this one, so between the three every register-offset load form a
; scalar dut issues is executed by a real kernel rather than by hand-written
; assembly.
;
; No sign extension here either -- i32 in, i32 out, so a wrong-width load shows
; up directly in the product rather than only in the negative half of the range.

; REQUIRES: ld_lld
; RUN: llc -mtriple=aie2p -O2 -filetype=obj %s -o %t.o
; RUN: ld.lld -e _start --section-start=.text=0x1000 \
; RUN:   --section-start=.bss=0x40000 -o %t.elf %t.o
; RUN: llvm-aie-run %t.elf --scratch=0x0:0x1000 --dump-mem=0x43000:4 \
; RUN:   --dump-mem=0x42000:32 --dump-mem=0x1000:8 | FileCheck %s

@in0 = global [1024 x i32] zeroinitializer, align 32
@in1 = global [1024 x i32] zeroinitializer, align 32
@out = global [1024 x i32] zeroinitializer, align 32
@status = global i32 0, align 4

define void @dut(ptr %0, ptr %1, ptr %2) {
  br label %4

4:
  %5 = phi i64 [ %14, %7 ], [ 0, %3 ]
  %6 = icmp slt i64 %5, 1024
  br i1 %6, label %7, label %15

7:
  %8 = getelementptr inbounds nuw i32, ptr %0, i64 %5
  %9 = load i32, ptr %8, align 4
  %10 = getelementptr inbounds nuw i32, ptr %1, i64 %5
  %11 = load i32, ptr %10, align 4
  %12 = mul i32 %9, %11
  %13 = getelementptr inbounds nuw i32, ptr %2, i64 %5
  store i32 %12, ptr %13, align 4
  %14 = add i64 %5, 1
  br label %4

15:
  ret void
}

define void @_start() {
entry:
  br label %fill

fill:
  %i = phi i32 [ 0, %entry ], [ %i.next, %fill ]
  %b3 = mul i32 %i, 3
  %pa = getelementptr inbounds [1024 x i32], ptr @in0, i32 0, i32 %i
  %pb = getelementptr inbounds [1024 x i32], ptr @in1, i32 0, i32 %i
  store i32 %i, ptr %pa, align 4
  store i32 %b3, ptr %pb, align 4
  %i.next = add i32 %i, 1
  %filled = icmp eq i32 %i.next, 1024
  br i1 %filled, label %run, label %fill

run:
  call void @dut(ptr @in0, ptr @in1, ptr @out)
  br label %check

check:
  %j = phi i32 [ 0, %run ], [ %j.next, %check ]
  %bad = phi i32 [ 0, %run ], [ %bad.next, %check ]
  %qa = getelementptr inbounds [1024 x i32], ptr @in0, i32 0, i32 %j
  %qb = getelementptr inbounds [1024 x i32], ptr @in1, i32 0, i32 %j
  %qo = getelementptr inbounds [1024 x i32], ptr @out, i32 0, i32 %j
  %va = load i32, ptr %qa, align 4
  %vb = load i32, ptr %qb, align 4
  %vo = load i32, ptr %qo, align 4
  %ref = mul i32 %va, %vb
  %eq = icmp eq i32 %ref, %vo
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

; CHECK: bundles: {{[0-9]+}}

; Zero mismatches over all 1024 elements. Weak on its own -- LLVM sees the
; kernel and the reference in one module and can fold the comparison -- so the
; output bytes below are the evidence.
; CHECK: mem[0x43000] = 00 00 00 00

; out[i] = i * 3i, so out[0..7] = 0, 3, 12, 27, 48, 75, 108, 147.
; CHECK: mem[0x42000] = 00 00 00 00 03 00 00 00 0c 00 00 00 1b 00 00 00 30 00 00 00 4b 00 00 00 6c 00 00 00 93 00 00 00

; @dut's first bundle, still the code the linker wrote: nothing initialises sp,
; so a `.text` at 0 would have the prologue spill lr over it, and 0x0000 being
; the AIE2P NOP encoding the run would fall through the hole instead of failing.
; CHECK: mem[0x1000] = {{[0-9a-f ]*[1-9a-f][0-9a-f ]*$}}
