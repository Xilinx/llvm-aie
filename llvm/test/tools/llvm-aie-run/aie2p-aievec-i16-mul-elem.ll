;===- aie2p-aievec-i16-mul-elem.ll --------------------------------------===;
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
;===---------------------------------------------------------------------===;
;
; A second aievec kernel, for the loads the first one does not reach.
;
; @dut is mlir-aie's
; test/unit_tests/aievec_tests/aie2/i16xi16_mul_elem_2/i16xi16_mul_elem-llvm-scalar.mlir
; through that test's own pipeline. It indexes with lda.s16 where
; aie2p-aievec-i8-mul-elem.ll uses lda.s8, and widens to i32 before the
; multiply, so it still stores through the 32-bit form.
;
; That last part is why this one is here and its i16-OUT sibling is not.
; ST_dms_sts_idx reads its source at operand cycle 1, so the compiler has to
; schedule the store after the multiply's result has landed and the executor
; reaches the same answer. ST_s16_idx and ST_s8_idx read theirs at cycle 7,
; late enough for the compiler to place the store inside the load's own
; 7-cycle window; the executor commits register writes on issue, so it reads
; the freshly loaded input there and stores that instead. Wiring those two
; kernels needs a value model that resolves a read at issue + its operand
; cycle, which does not exist yet.

; REQUIRES: ld_lld
; RUN: llc -mtriple=aie2p -O2 -filetype=obj %s -o %t.o
; RUN: ld.lld -e _start --section-start=.text=0x1000 \
; RUN:   --section-start=.bss=0x30000 -o %t.elf %t.o
; RUN: llvm-aie-run %t.elf --scratch=0x0:0x1000 --dump-mem=0x32000:4 \
; RUN:   --dump-mem=0x31000:32 --dump-mem=0x1000:8 | FileCheck %s

@in0 = global [1024 x i16] zeroinitializer, align 32
@in1 = global [1024 x i16] zeroinitializer, align 32
@out = global [1024 x i32] zeroinitializer, align 32
@status = global i32 0, align 4

define void @dut(ptr %0, ptr %1, ptr %2) {
  br label %4

4:
  %5 = phi i64 [ %16, %7 ], [ 0, %3 ]
  %6 = icmp slt i64 %5, 1024
  br i1 %6, label %7, label %17

7:
  %8 = getelementptr inbounds nuw i16, ptr %0, i64 %5
  %9 = load i16, ptr %8, align 2
  %10 = getelementptr inbounds nuw i16, ptr %1, i64 %5
  %11 = load i16, ptr %10, align 2
  %12 = sext i16 %9 to i32
  %13 = sext i16 %11 to i32
  %14 = mul i32 %12, %13
  %15 = getelementptr inbounds nuw i32, ptr %2, i64 %5
  store i32 %14, ptr %15, align 4
  %16 = add i64 %5, 1
  br label %4

17:
  ret void
}

define void @_start() {
entry:
  br label %fill

; in0[i] = i and in1[i] = 3i, both wrapping in i16, so the tail of the range
; is negative and a kernel that dropped the sign extension disagrees there.
fill:
  %i = phi i32 [ 0, %entry ], [ %i.next, %fill ]
  %a = trunc i32 %i to i16
  %b3 = mul i32 %i, 3
  %b = trunc i32 %b3 to i16
  %pa = getelementptr inbounds [1024 x i16], ptr @in0, i32 0, i32 %i
  %pb = getelementptr inbounds [1024 x i16], ptr @in1, i32 0, i32 %i
  store i16 %a, ptr %pa, align 2
  store i16 %b, ptr %pb, align 2
  %i.next = add i32 %i, 1
  %filled = icmp eq i32 %i.next, 1024
  br i1 %filled, label %run, label %fill

run:
  call void @dut(ptr @in0, ptr @in1, ptr @out)
  br label %check

check:
  %j = phi i32 [ 0, %run ], [ %j.next, %check ]
  %bad = phi i32 [ 0, %run ], [ %bad.next, %check ]
  %qa = getelementptr inbounds [1024 x i16], ptr @in0, i32 0, i32 %j
  %qb = getelementptr inbounds [1024 x i16], ptr @in1, i32 0, i32 %j
  %qo = getelementptr inbounds [1024 x i32], ptr @out, i32 0, i32 %j
  %va = load i16, ptr %qa, align 2
  %vb = load i16, ptr %qb, align 2
  %vo = load i32, ptr %qo, align 4
  %ea = sext i16 %va to i32
  %eb = sext i16 %vb to i32
  %ref = mul i32 %ea, %eb
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
; CHECK: mem[0x32000] = 00 00 00 00

; out[0..7] = 0, 3, 12, 27, 48, 75, 108, 147.
; CHECK: mem[0x31000] = 00 00 00 00 03 00 00 00 0c 00 00 00 1b 00 00 00 30 00 00 00 4b 00 00 00 6c 00 00 00 93 00 00 00

; @dut's first bundle, still the code the linker wrote: nothing initialises sp,
; so a `.text` at 0 would have the prologue spill lr over it, and 0x0000 being
; the AIE2P NOP encoding the run would fall through the hole instead of failing.
; CHECK: mem[0x1000] = {{[0-9a-f ]*[1-9a-f][0-9a-f ]*$}}
