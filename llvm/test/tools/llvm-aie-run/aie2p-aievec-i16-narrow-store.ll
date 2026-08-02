;===- aie2p-aievec-i16-narrow-store.ll ----------------------------------===;
;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
;===---------------------------------------------------------------------===;
;
; The kernel that only works if the exposed pipeline is modelled.
;
; @dut is mlir-aie's
; test/unit_tests/aievec_tests/aie2/i16xi16_mul_elem/i16xi16_mul_elem-llvm-scalar.mlir
; through that test's own pipeline. It is i16 in and i16 OUT, so it stores
; through ST_s16_idx, which reads its source at operand cycle 7 rather than 1.
; The compiler uses that: it schedules the store BEFORE the multiply whose
; result the store commits, because the multiply lands at cycle 2 from four
; bundles later and so arrives first.
;
; An executor that writes on issue and reads on issue stores the freshly
; loaded input here instead, and this test is what catches that. Its sibling
; aie2p-aievec-i16-mul-elem.ll widens to i32 before storing and is insensitive
; to all of it, which is why it passed throughout and this one did not.
;
; Ground truth is the device, not the model: the same kernel object was run on
; XDNA2 and stores the product, 1024/1024.

; REQUIRES: ld_lld
; RUN: llc -mtriple=aie2p -O2 -filetype=obj %s -o %t.o
; RUN: ld.lld -e _start --section-start=.text=0x1000 \
; RUN:   --section-start=.bss=0x30000 -o %t.elf %t.o
; RUN: llvm-aie-run %t.elf --scratch=0x0:0x1000 --dump-mem=0x31000:16 \
; RUN:   | FileCheck %s

@in0 = global [1024 x i16] zeroinitializer, align 32
@in1 = global [1024 x i16] zeroinitializer, align 32
@out = global [1024 x i16] zeroinitializer, align 32

define void @dut(ptr %0, ptr %1, ptr %2) {
  br label %4

4:
  %5 = phi i64 [ %14, %7 ], [ 0, %3 ]
  %6 = icmp slt i64 %5, 1024
  br i1 %6, label %7, label %15

7:
  %8 = getelementptr inbounds nuw i16, ptr %0, i64 %5
  %9 = load i16, ptr %8, align 2
  %10 = getelementptr inbounds nuw i16, ptr %1, i64 %5
  %11 = load i16, ptr %10, align 2
  %12 = mul i16 %9, %11
  %13 = getelementptr inbounds nuw i16, ptr %2, i64 %5
  store i16 %12, ptr %13, align 2
  %14 = add i64 %5, 1
  br label %4

15:
  ret void
}

; No in-module reference: LLVM would fold a comparison against the kernel it
; can see, so the expected bytes below are computed outside and checked
; directly.
define void @_start() {
entry:
  br label %fill

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
  call void @llvm.aie2p.done()
  unreachable
}

declare void @llvm.aie2p.done()

; CHECK: bundles: {{[0-9]+}}

; in0[i] = i, in1[i] = 3i, so out[i] = 3i*i as int16:
; 0, 3, 12, 27, 48, 75, 108, 147. Storing in0[i] instead -- what an
; issue-time model does -- would read 0, 1, 2, 3, 4, 5, 6, 7 here.
; CHECK: mem[0x31000] = 00 00 03 00 0c 00 1b 00 30 00 4b 00 6c 00 93 00
