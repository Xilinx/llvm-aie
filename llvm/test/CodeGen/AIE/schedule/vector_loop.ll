;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie < %s | FileCheck %s
; CHECK:   vcmp    xc, r0, ya.s32, r15, c1, r5, c1, c0

source_filename = "LLVMDialectModule"

@buf5 = external global [64 x float]
@buf4 = external global [64 x float]
@buf3 = external global [64 x float]
@buf11 = external global [64 x float]
@buf10 = external global [64 x float]
@buf9 = external global [64 x float]
@buf8 = external global [64 x float]
@buf7 = external global [64 x float]
@buf6 = external global [64 x float]

declare i8* @malloc(i64)

declare void @free(i8*)

declare void @debug_i32(i32)

; Function Attrs: nounwind
declare void @llvm.aie.put.ms(i32, i32) #0

declare void @llvm.aie.put.wms(i32, i128)

declare void @llvm.aie.put.fms(i32, float)

; Function Attrs: nounwind readnone
declare i32 @llvm.aie.get.ss(i32) #1

declare i128 @llvm.aie.get.wss(i32)

declare float @llvm.aie.get.fss(i32)

declare void @llvm.aie.put.mcd(i384)

declare i384 @llvm.aie.get.scd()

; Function Attrs: nounwind
declare void @llvm.aie.lock.acquire.reg(i32, i32) #0

; Function Attrs: nounwind
declare void @llvm.aie.lock.release.reg(i32, i32) #0

define <8 x i32> @test(i32 %r3) {
  %r6 = getelementptr float, float* getelementptr inbounds ([64 x float], [64 x float]* @buf9, i64 0, i64 0), i32 %r3
  %r7 = bitcast float* %r6 to <8 x float>*
  %r9 = insertelement <8 x i32> undef, i32 %r3, i32 0
  %r10 = shufflevector <8 x i32> %r9, <8 x i32> undef, <8 x i32> zeroinitializer
  %r11 = add <8 x i32> %r10, <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
;  %r12 = icmp slt <8 x i32> %r11, <i32 64, i32 64, i32 64, i32 64, i32 64, i32 64, i32 64, i32 64>
  ret <8 x i32> %r11
}

; define void @core83() {
;   br label %r1

; r1:                                                ; preds = %r31, %r0
;   call void @llvm.aie.lock.acquire.reg(i32 48, i32 1)
;   call void @llvm.aie.lock.acquire.reg(i32 49, i32 1)
;   call void @llvm.aie.lock.acquire.reg(i32 50, i32 0)
;   br label %r2

; r2:                                                ; preds = %r5, %r1
;   %r3 = phi i64 [ %r30, %r5 ], [ 0, %r1 ]
;   %r4 = icmp slt i64 %r3, 64
;   br i1 %r4, label %r5, label %r31

; r5:                                                ; preds = %r2
;   %r6 = getelementptr float, float* getelementptr inbounds ([64 x float], [64 x float]* @buf9, i64 0, i64 0), i64 %r3
;   %r7 = bitcast float* %r6 to <8 x float>*
;   ; %r8 = trunc i64 %r3 to i32
;   ; %r9 = insertelement <8 x i32> undef, i32 %r8, i32 0
;   ; %r10 = shufflevector <8 x i32> %r9, <8 x i32> undef, <8 x i32> zeroinitializer
;   ; %r11 = add <8 x i32> %r10, <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
;   ; %r12 = icmp slt <8 x i32> %r11, <i32 64, i32 64, i32 64, i32 64, i32 64, i32 64, i32 64, i32 64>
;   ; %r13 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %r7, i32 4, <8 x i1> %r12, <8 x float> zeroinitializer)
;    %r13 = load <8 x float>, <8 x float>* %r7, align 32
;   %r14 = getelementptr float, float* getelementptr inbounds ([64 x float], [64 x float]* @buf10, i64 0, i64 0), i64 %r3
;   %r15 = bitcast float* %r14 to <8 x float>*
;   ; %r16 = trunc i64 %r3 to i32
;   ; %r17 = insertelement <8 x i32> undef, i32 %r16, i32 0
;   ; %r18 = shufflevector <8 x i32> %r17, <8 x i32> undef, <8 x i32> zeroinitializer
;   ; %r19 = add <8 x i32> %r18, <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
;   ; %r20 = icmp slt <8 x i32> %r19, <i32 64, i32 64, i32 64, i32 64, i32 64, i32 64, i32 64, i32 64>
;   ; %r21 = call <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>* %r15, i32 4, <8 x i1> %r20, <8 x float> zeroinitializer)
;   %r21 = load <8 x float>, <8 x float>* %r15, align 32
;   %r22 = fmul <8 x float> %r13, %r21
;   %r23 = getelementptr float, float* getelementptr inbounds ([64 x float], [64 x float]* @buf11, i64 0, i64 0), i64 %r3
;   %r24 = bitcast float* %r23 to <8 x float>*
;   ; %r25 = trunc i64 %r3 to i32
;   ; %r26 = insertelement <8 x i32> undef, i32 %r25, i32 0
;   ; %r27 = shufflevector <8 x i32> %r26, <8 x i32> undef, <8 x i32> zeroinitializer
;   ; %r28 = add <8 x i32> %r27, <i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7>
;   ; %r29 = icmp slt <8 x i32> %r28, <i32 64, i32 64, i32 64, i32 64, i32 64, i32 64, i32 64, i32 64>
;   ; call void @llvm.masked.store.v8f32.p0v8f32(<8 x float> %r22, <8 x float>* %r24, i32 4, <8 x i1> %r29)
;   store <8 x float> %r22, <8 x float>* %r24, align 32
;   %r30 = add i64 %r3, 8
;   br label %r2

; r31:                                               ; preds = %r2
;   call void @llvm.aie.lock.release.reg(i32 50, i32 1)
;   call void @llvm.aie.lock.release.reg(i32 49, i32 0)
;   call void @llvm.aie.lock.release.reg(i32 48, i32 0)
;   br label %r1
; }

; define void @_main() {
;   call void @core83()
;   ret void
; }

; Function Attrs: argmemonly nofree nosync nounwind readonly willreturn
declare <8 x float> @llvm.masked.load.v8f32.p0v8f32(<8 x float>*, i32 immarg, <8 x i1>, <8 x float>) #2

; Function Attrs: argmemonly nofree nosync nounwind willreturn writeonly
declare void @llvm.masked.store.v8f32.p0v8f32(<8 x float>, <8 x float>*, i32 immarg, <8 x i1>) #3

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }
attributes #2 = { argmemonly nofree nosync nounwind readonly willreturn }
attributes #3 = { argmemonly nofree nosync nounwind willreturn writeonly }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
