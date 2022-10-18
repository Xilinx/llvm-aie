;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 -stop-after=instruction-select -verify-machineinstrs %s -o - | FileCheck %s
; RUN: llc -mtriple=aie2 -stop-before=postmisched -verify-machineinstrs %s -o - | FileCheck %s

; Verify that memory operands make it until scheduling.

declare void @sink_i8(i8 signext, i8 signext)
define void @load_i8(i32 %idx, ptr %array) {
  ; CHECK-LABEL: name: load_i8
  ; CHECK:       LDA_S8_ag_idx
  ; CHECK-SAME:  (load (s8) from %ir.arrayidx.0)
  ; CHECK:       LDA_S8_ag_idx_imm
  ; CHECK-SAME:  (load (s8) from %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x i8], ptr %array, i32 0, i32 %idx
  %0 = load i8, ptr %arrayidx.0, align 1
  %arrayidx.1 = getelementptr inbounds [16 x i8], ptr %array, i32 0, i32 2
  %1 = load i8, ptr %arrayidx.1, align 1
  call void @sink_i8(i8 %0, i8 %1)
  ret void
}

declare void @sink_i16(i16 signext, i16 signext)
define void @load_i16(i32 %idx, ptr %array) {
  ; CHECK-LABEL: name: load_i16
  ; CHECK:       LDA_S16_ag_idx
  ; CHECK-SAME:  (load (s16) from %ir.arrayidx.0)
  ; CHECK:       LDA_S16_ag_idx_imm
  ; CHECK-SAME:  (load (s16) from %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x i16], ptr %array, i32 0, i32 %idx
  %0 = load i16, ptr %arrayidx.0, align 2
  %arrayidx.1 = getelementptr inbounds [16 x i16], ptr %array, i32 0, i32 1
  %1 = load i16, ptr %arrayidx.1, align 2
  call void @sink_i16(i16 %0, i16 %1)
  ret void
}

declare void @sink_i32(i32, i32)
define void @load_i32(i32 %idx, ptr %array) {
  ; CHECK-LABEL: name: load_i32
  ; CHECK:       LDA_dms_lda_idx
  ; CHECK-SAME:  (load (s32) from %ir.arrayidx.0)
  ; CHECK:       LDA_dms_lda_idx_imm
  ; CHECK-SAME:  (load (s32) from %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x i32], ptr %array, i32 0, i32 %idx
  %0 = load i32, ptr %arrayidx.0, align 4
  %arrayidx.1 = getelementptr inbounds [16 x i32], ptr %array, i32 0, i32 2
  %1 = load i32, ptr %arrayidx.1, align 4
  call void @sink_i32(i32 %0, i32 %1)
  ret void
}

declare void @sink_v8i32(<8 x i32>, <8 x i32>)
define void @load_v8i32(i32 %idx, ptr %array) {
  ; CHECK-LABEL: name: load_v8i32
  ; CHECK:       VLDA_dmw_lda_w_ag_idx
  ; CHECK-SAME:  (load (<8 x s32>) from %ir.arrayidx.0)
  ; CHECK:       VLDA_dmw_lda_w_ag_idx_imm
  ; CHECK-SAME:  (load (<8 x s32>) from %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <8 x i32>], ptr %array, i32 0, i32 %idx
  %0 = load <8 x i32>, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <8 x i32>], ptr %array, i32 0, i32 2
  %1 = load <8 x i32>, ptr %arrayidx.1, align 32
  call void @sink_v8i32(<8 x i32> %0, <8 x i32> %1)
  ret void
}

declare void @sink_v16i32(<16 x i32>, <16 x i32>)
define void @load_v16i32(i32 %idx, ptr %array) {
  ; CHECK-LABEL: name: load_v16i32
  ; CHECK: VLDA_dmw_lda_w_ag_idx_imm
  ; CHECK-SAME: (load (<8 x s32>) from %ir.arrayidx.1 + 32)
  ; CHECK: VLDA_dmw_lda_w_ag_idx_imm
  ; CHECK-SAME: (load (<8 x s32>) from %ir.arrayidx.1)
  ; CHECK: VLDA_dmw_lda_w_ag_idx_imm
  ; CHECK-SAME: (load (<8 x s32>) from %ir.arrayidx.0 + 32)
  ; CHECK: VLDA_dmw_lda_w_ag_idx_imm
  ; CHECK-SAME: (load (<8 x s32>) from %ir.arrayidx.0)
entry:
  %arrayidx.1 = getelementptr inbounds [16 x <16 x i32>], ptr %array, i32 0, i32 2
  %0 = load <16 x i32>, ptr %arrayidx.1, align 32
  %arrayidx.0 = getelementptr inbounds [16 x <16 x i32>], ptr %array, i32 0, i32 %idx
  %1 = load <16 x i32>, ptr %arrayidx.0, align 32
  call void @sink_v16i32(<16 x i32> %0, <16 x i32> %1)
  ret void
}

declare void @sink_v32i32(<32 x i32>, <32 x i32>)
define void @load_v32i32(i32 %idx, ptr %array) {
  ; CHECK-LABEL: name: load_v32i32
  ; CHECK-COUNT-4: VLDA_dmw_lda_w_ag_idx_imm {{.*}} (load (<32 x s32>) from %ir.arrayidx.0, align 32)
  ; CHECK-COUNT-4: VLDA_dmw_lda_w_ag_idx_imm {{.*}} (load (<32 x s32>) from %ir.arrayidx.1, align 32)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <32 x i32>], ptr %array, i32 0, i32 %idx
  %0 = load <32 x i32>, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <32 x i32>], ptr %array, i32 0, i32 2
  %1 = load <32 x i32>, ptr %arrayidx.1, align 32
  call void @sink_v32i32(<32 x i32> %0, <32 x i32> %1)
  ret void
}

declare void @sink_v4i64(<4 x i64>, <4 x i64>)
define void @load_v4i64(i32 %idx, ptr %array) {
  ; CHECK-LABEL: name: load_v4i64
  ; CHECK:       VLDA_dmw_lda_am_ag_idx
  ; CHECK-SAME:  (load (<4 x s64>) from %ir.arrayidx.0)
  ; CHECK:       VLDA_dmw_lda_am_ag_idx_imm
  ; CHECK-SAME:  (load (<4 x s64>) from %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <4 x i64>], ptr %array, i32 0, i32 %idx
  %0 = load <4 x i64>, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <4 x i64>], ptr %array, i32 0, i32 2
  %1 = load <4 x i64>, ptr %arrayidx.1, align 32
  call void @sink_v4i64(<4 x i64> %0, <4 x i64> %1)
  ret void
}

declare void @sink_v8i64(<8 x i64>, <8 x i64>)
define void @load_v8i64(i32 %idx, ptr %array) {
  ; CHECK-LABEL: name: load_v8i64
  ; CHECK: VLDA_dmw_lda_am_ag_idx_imm
  ; CHECK-SAME: (load (<4 x s64>) from %ir.arrayidx.1 + 32)
  ; CHECK: VLDA_dmw_lda_am_ag_idx_imm
  ; CHECK-SAME: (load (<4 x s64>) from %ir.arrayidx.1)
  ; CHECK: VLDA_dmw_lda_am_ag_idx_imm
  ; CHECK-SAME: (load (<4 x s64>) from %ir.arrayidx.0 + 32)
  ; CHECK: VLDA_dmw_lda_am_ag_idx_imm
  ; CHECK-SAME: (load (<4 x s64>) from %ir.arrayidx.0)
entry:
  %arrayidx.1 = getelementptr inbounds [16 x <8 x i64>], ptr %array, i32 0, i32 2
  %0 = load <8 x i64>, ptr %arrayidx.1, align 32
  %arrayidx.0 = getelementptr inbounds [16 x <8 x i64>], ptr %array, i32 0, i32 %idx
  %1 = load <8 x i64>, ptr %arrayidx.0, align 32
  call void @sink_v8i64(<8 x i64> %0, <8 x i64> %1)
  ret void
}

declare void @sink_v16i64(<16 x i64>, <16 x i64>)
define void @load_v16i64(i32 %idx, ptr %array) {
  ; CHECK-LABEL: name: load_v16i64
  ; CHECK-COUNT-4: VLDA_dmw_lda_am_ag_idx_imm {{.*}} (load (<16 x s64>) from %ir.arrayidx.0, align 32)
  ; CHECK-COUNT-4: VLDA_dmw_lda_am_ag_idx_imm {{.*}} (load (<16 x s64>) from %ir.arrayidx.1, align 32)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <16 x i64>], ptr %array, i32 0, i32 %idx
  %0 = load <16 x i64>, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <16 x i64>], ptr %array, i32 0, i32 2
  %1 = load <16 x i64>, ptr %arrayidx.1, align 32
  call void @sink_v16i64(<16 x i64> %0, <16 x i64> %1)
  ret void
}
