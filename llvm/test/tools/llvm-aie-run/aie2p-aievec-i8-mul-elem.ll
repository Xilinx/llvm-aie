;===- aie2p-aievec-i8-mul-elem.ll ---------------------------------------===;
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
;===---------------------------------------------------------------------===;
;
; A whole kernel from mlir-aie's aievec_tests, run and checked.
;
; @dut is byte-for-byte what mlir-aie emits for
; test/unit_tests/aievec_tests/aie2/i8xi8_mul_elem/i8xi8_mul_elem-llvm-scalar.mlir
; under that test's own pipeline (`aie-opt %vector-to-generic-llvmir%` then
; `aie-translate --mlir-to-llvmir`). Upstream that object is linked against a
; Chess-built testbench and run under `xca_udm_dbg`, which is the last Chess
; dependency in the group; here it is compiled by llc and run by this tool, so
; nothing in the path needs Vitis, a license or a device.
;
; @_start is the testbench reduced to what a bare core can do: it fills the
; inputs, calls the kernel, and recomputes the reference in place. The upstream
; testbench cannot be reused as-is -- it wants stdio, malloc, libc++ and the
; `chess_*` builtins, and Peano's AIE libc has none of those.
;
; Every mismatch increments the status word, so the check below is not merely
; "it terminated": the kernel ran, and its 1024 results agree with the
; reference. The output spot-check is there so an @out left untouched cannot
; pass as agreement.

; REQUIRES: ld_lld
; RUN: llc -mtriple=aie2p -O2 -filetype=obj %s -o %t.o
; RUN: ld.lld -e _start --section-start=.text=0x0 \
; RUN:   --section-start=.bss=0x20000 -o %t.elf %t.o
; RUN: llvm-aie-run %t.elf --dump-mem=0x21800:4 --dump-mem=0x20800:32 \
; RUN:   | FileCheck %s

@in0 = global [1024 x i8] zeroinitializer, align 32
@in1 = global [1024 x i8] zeroinitializer, align 32
@out = global [1024 x i32] zeroinitializer, align 32
@status = global i32 0, align 4

define void @dut(ptr %0, ptr %1, ptr %2) {
  br label %4

4:
  %5 = phi i64 [ %16, %7 ], [ 0, %3 ]
  %6 = icmp slt i64 %5, 1024
  br i1 %6, label %7, label %17

7:
  %8 = getelementptr inbounds nuw i8, ptr %0, i64 %5
  %9 = load i8, ptr %8, align 1
  %10 = getelementptr inbounds nuw i8, ptr %1, i64 %5
  %11 = load i8, ptr %10, align 1
  %12 = sext i8 %9 to i32
  %13 = sext i8 %11 to i32
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

; in0[i] = i, in1[i] = 3i, both truncated to i8 so the tail of the range is
; negative -- a kernel that dropped the sign extension would disagree there.
fill:
  %i = phi i32 [ 0, %entry ], [ %i.next, %fill ]
  %a = trunc i32 %i to i8
  %b3 = mul i32 %i, 3
  %b = trunc i32 %b3 to i8
  %pa = getelementptr inbounds [1024 x i8], ptr @in0, i32 0, i32 %i
  %pb = getelementptr inbounds [1024 x i8], ptr @in1, i32 0, i32 %i
  store i8 %a, ptr %pa, align 1
  store i8 %b, ptr %pb, align 1
  %i.next = add i32 %i, 1
  %filled = icmp eq i32 %i.next, 1024
  br i1 %filled, label %run, label %fill

run:
  call void @dut(ptr @in0, ptr @in1, ptr @out)
  br label %check

check:
  %j = phi i32 [ 0, %run ], [ %j.next, %check ]
  %bad = phi i32 [ 0, %run ], [ %bad.next, %check ]
  %qa = getelementptr inbounds [1024 x i8], ptr @in0, i32 0, i32 %j
  %qb = getelementptr inbounds [1024 x i8], ptr @in1, i32 0, i32 %j
  %qo = getelementptr inbounds [1024 x i32], ptr @out, i32 0, i32 %j
  %va = load i8, ptr %qa, align 1
  %vb = load i8, ptr %qb, align 1
  %vo = load i32, ptr %qo, align 4
  %ea = sext i8 %va to i32
  %eb = sext i8 %vb to i32
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

; The run reached `done` rather than the bundle cap.
; CHECK: bundles: {{[0-9]+}}

; Zero mismatches over all 1024 elements.
; CHECK: mem[0x21800] = 00 00 00 00

; out[0..7] = 0, 3, 12, 27, 48, 75, 108, 147.
; CHECK: mem[0x20800] = 00 00 00 00 03 00 00 00 0c 00 00 00 1b 00 00 00 30 00 00 00 4b 00 00 00 6c 00 00 00 93 00 00 00
