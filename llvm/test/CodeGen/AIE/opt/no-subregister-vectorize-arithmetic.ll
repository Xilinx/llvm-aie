; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
;
; Regression test for llvm-aie#480: vectorizers must not create sub-512-bit
; vector arithmetic on AIE2. The GlobalISel Legalizer only has rules for
; 512-bit vector arithmetic (V64S8, V32S16, V16S32).
;
; Three TTI overrides prevent this:
;   - getRegisterBitWidth(RGK_FixedWidthVector) = 512: prevents the loop
;     vectorizer from choosing sub-512-bit VF (e.g., VF=4 -> <4 x i8>).
;   - getMinVectorRegisterBitWidth() = 512: prevents the SLP vectorizer
;     from creating sub-512-bit vectors via its minimum size check.
;   - getArithmeticInstrCost() = Invalid for sub-512-bit: prevents the SLP
;     vectorizer's cost model from treating <8 x i8> adds as profitable
;     (after loop unrolling, SLP may find adjacent scalar ops to vectorize).
;
; The first RUN line verifies the optimizer creates 512-bit vectors (V64S8),
; not sub-512-bit ones. The second RUN line verifies the full pipeline compiles.

; RUN: opt -mtriple=aie2 -O2 -S < %s | FileCheck %s
; RUN: opt -mtriple=aie2 -O2 -S < %s | \
; RUN:   llc --march=aie2 -O2 --function-sections --filetype=obj -o /dev/null

; --------------------------------------------------------------------------
; Test 1: Large trip count loop (vectorized by LoopVectorize).
; Without getRegisterBitWidth = 512, the vectorizer creates <4 x i8> adds.
; With the fix, it creates <64 x i8> adds (512-bit, matching V64S8).

; CHECK-LABEL: @add_i8_kernel
; CHECK: add <64 x i8>
; CHECK-NOT: add <4 x i8>
; CHECK-NOT: add <8 x i8>
; CHECK-NOT: add <16 x i8>
; CHECK-NOT: add <32 x i8>

@buf_in = external global [4096 x i8]
@buf_out = external global [4096 x i8]

define void @add_i8_kernel() {
  br label %outer

outer:
  %i = phi i64 [ %i.next, %inner.exit ], [ 0, %0 ]
  %cmp.outer = icmp slt i64 %i, 64
  br i1 %cmp.outer, label %inner, label %done

inner:
  %j = phi i64 [ %j.next, %inner.body ], [ 0, %outer ]
  %cmp.inner = icmp slt i64 %j, 64
  br i1 %cmp.inner, label %inner.body, label %inner.exit

inner.body:
  %idx = mul nuw nsw i64 %i, 64
  %off = add nuw nsw i64 %idx, %j
  %in.ptr = getelementptr inbounds nuw i8, ptr @buf_in, i64 %off
  %val = load i8, ptr %in.ptr, align 1
  %add = add i8 %val, 12
  %out.ptr = getelementptr inbounds nuw i8, ptr @buf_out, i64 %off
  store i8 %add, ptr %out.ptr, align 1
  %j.next = add i64 %j, 1
  br label %inner

inner.exit:
  %i.next = add i64 %i, 1
  br label %outer

done:
  ret void
}

; --------------------------------------------------------------------------
; Test 2: Small trip count loop (unrolled, then SLP attempts to vectorize).
; After unrolling, SLP finds 8 adjacent scalar adds. Without the
; getArithmeticInstrCost override returning Invalid for sub-512-bit vectors,
; SLP would create <8 x i8> adds (64-bit, illegal) because its cost model
; thinks vector add costs 1 vs scalar cost 8.

; CHECK-LABEL: @add_i8_small_kernel
; CHECK-NOT: add <8 x i8>
; CHECK-NOT: add <4 x i8>

@small_in = external global [8 x i8]
@small_out = external global [8 x i8]

declare void @llvm.aie2.acquire(i32, i32)
declare void @llvm.aie2.release(i32, i32)

define void @add_i8_small_kernel() {
  call void @llvm.aie2.acquire(i32 49, i32 -1)
  call void @llvm.aie2.acquire(i32 50, i32 -1)
  br label %loop

loop:
  %i = phi i64 [ %i.next, %loop ], [ 0, %0 ]
  %in.ptr = getelementptr inbounds nuw i8, ptr @small_in, i64 %i
  %val = load i8, ptr %in.ptr, align 1
  %add = add i8 %val, 21
  %out.ptr = getelementptr inbounds nuw i8, ptr @small_out, i64 %i
  store i8 %add, ptr %out.ptr, align 1
  %i.next = add i64 %i, 1
  %done = icmp eq i64 %i.next, 8
  br i1 %done, label %exit, label %loop

exit:
  call void @llvm.aie2.release(i32 49, i32 1)
  call void @llvm.aie2.release(i32 50, i32 1)
  ret void
}

; --------------------------------------------------------------------------
; Test 3: i16 variant to verify 512-bit vectorization (V32S16).

; CHECK-LABEL: @add_i16_kernel
; CHECK: add <32 x i16>
; CHECK-NOT: add <2 x i16>
; CHECK-NOT: add <4 x i16>

@buf_in_i16 = external global [1024 x i16]
@buf_out_i16 = external global [1024 x i16]

define void @add_i16_kernel() {
  br label %loop

loop:
  %i = phi i64 [ %i.next, %loop ], [ 0, %0 ]
  %in.ptr = getelementptr inbounds nuw i16, ptr @buf_in_i16, i64 %i
  %val = load i16, ptr %in.ptr, align 2
  %add = add i16 %val, 42
  %out.ptr = getelementptr inbounds nuw i16, ptr @buf_out_i16, i64 %i
  store i16 %add, ptr %out.ptr, align 2
  %i.next = add i64 %i, 1
  %done = icmp eq i64 %i.next, 1024
  br i1 %done, label %exit, label %loop

exit:
  ret void
}
