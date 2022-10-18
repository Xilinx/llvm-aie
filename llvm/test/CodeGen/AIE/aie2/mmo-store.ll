;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie2 -stop-after=instruction-select --issue-limit=1 \
; RUN:    -verify-machineinstrs %s -o - | FileCheck %s
; RUN: llc -mtriple=aie2 -stop-after=postmisched --issue-limit=1 \
; RUN:    -verify-machineinstrs %s -o - | FileCheck %s

; Verify that memory operands make it until scheduling.
; FIXME: We are losing MMOs for 1024-bit stores.
define void @store_i8(i32 %idx, ptr %array, i8 signext %val) {
  ; CHECK-LABEL: name: store_i8
  ; CHECK:       ST_S8_ag_idx
  ; CHECK-SAME:  (store (s8) into %ir.arrayidx.0)
  ; CHECK:       ST_S8_ag_idx_imm
  ; CHECK-SAME:  (store (s8) into %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x i8], ptr %array, i32 0, i32 %idx
  store i8 %val, ptr %arrayidx.0, align 1
  %arrayidx.1 = getelementptr inbounds [16 x i8], ptr %array, i32 0, i32 2
  store i8 %val, ptr %arrayidx.1, align 1
  ret void
}

define void @store_i16(i32 %idx, ptr %array, i16 signext %val) {
  ; CHECK-LABEL: name: store_i16
  ; CHECK:       ST_S16_ag_idx
  ; CHECK-SAME:  (store (s16) into %ir.arrayidx.0)
  ; CHECK:       ST_S16_ag_idx_imm
  ; CHECK-SAME:  (store (s16) into %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x i16], ptr %array, i32 0, i32 %idx
  store i16 %val, ptr %arrayidx.0, align 2
  %arrayidx.1 = getelementptr inbounds [16 x i16], ptr %array, i32 0, i32 1
  store i16 %val, ptr %arrayidx.1, align 2
  ret void
}

define void @store_i32(i32 %idx, ptr %array, i32 %val) {
  ; CHECK-LABEL: name: store_i32
  ; CHECK:       ST_dms_sts_idx
  ; CHECK-SAME:  (store (s32) into %ir.arrayidx.0)
  ; CHECK:       ST_dms_sts_idx_imm
  ; CHECK-SAME:  (store (s32) into %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x i32], ptr %array, i32 0, i32 %idx
  store i32 %val, ptr %arrayidx.0, align 4
  %arrayidx.1 = getelementptr inbounds [16 x i32], ptr %array, i32 0, i32 2
  store i32 %val, ptr %arrayidx.1, align 4
  ret void
}

define void @store_v8i32(i32 %idx, ptr %array, <8 x i32> %val) {
  ; CHECK-LABEL: name: store_v8i32
  ; CHECK:       VST_dmw_sts_w_ag_idx
  ; CHECK-SAME:  (store (<8 x s32>) into %ir.arrayidx.0)
  ; CHECK:       VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME:  (store (<8 x s32>) into %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <8 x i32>], ptr %array, i32 0, i32 %idx
  store <8 x i32> %val, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <8 x i32>], ptr %array, i32 0, i32 2
  store <8 x i32> %val, ptr %arrayidx.1, align 32
  ret void
}

define void @store_v16i32(i32 %idx, ptr %array, <16 x i32> %val) {
  ; CHECK-LABEL: name: store_v16i32
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.0 + 32)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.0)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.1 + 32)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <16 x i32>], ptr %array, i32 0, i32 %idx
  store <16 x i32> %val, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <16 x i32>], ptr %array, i32 0, i32 2
  store <16 x i32> %val, ptr %arrayidx.1, align 32
  ret void
}

define void @store_v32i32(i32 %idx, ptr %array, <32 x i32> %val) {
  ; CHECK-LABEL: name: store_v32i32
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.0 + 96)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.0 + 64)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.0 + 32)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.0)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.1 + 96)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.1 + 64)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.1 + 32)
  ; CHECK: VST_dmw_sts_w_ag_idx_imm
  ; CHECK-SAME: (store (<8 x s32>) into %ir.arrayidx.1)
  ; CHECK-NOT: (store
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <32 x i32>], ptr %array, i32 0, i32 %idx
  store <32 x i32> %val, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <32 x i32>], ptr %array, i32 0, i32 2
  store <32 x i32> %val, ptr %arrayidx.1, align 32
  ret void
}

define void @store_v4i64(i32 %idx, ptr %array, <4 x i64> %val) {
  ; CHECK-LABEL: name: store_v4i64
  ; CHECK:       VST_dmw_sts_am_ag_idx
  ; CHECK-SAME:  (store (<4 x s64>) into %ir.arrayidx.0)
  ; CHECK:       VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME:  (store (<4 x s64>) into %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <4 x i64>], ptr %array, i32 0, i32 %idx
  store <4 x i64> %val, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <4 x i64>], ptr %array, i32 0, i32 2
  store <4 x i64> %val, ptr %arrayidx.1, align 32
  ret void
}

define void @store_v8i64(i32 %idx, ptr %array, <8 x i64> %val) {
  ; CHECK-LABEL: name: store_v8i64
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.0 + 32)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.0)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.1 + 32)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <8 x i64>], ptr %array, i32 0, i32 %idx
  store <8 x i64> %val, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <8 x i64>], ptr %array, i32 0, i32 2
  store <8 x i64> %val, ptr %arrayidx.1, align 32
  ret void
}

define void @store_v16i64(i32 %idx, ptr %array, <16 x i64> %val) {
  ; CHECK-LABEL: name: store_v16i64
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.0 + 96)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.0 + 64)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.0 + 32)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.0)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.1 + 96)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.1 + 64)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.1 + 32)
  ; CHECK: VST_dmw_sts_am_ag_idx_imm
  ; CHECK-SAME: (store (<4 x s64>) into %ir.arrayidx.1)
entry:
  %arrayidx.0 = getelementptr inbounds [16 x <16 x i64>], ptr %array, i32 0, i32 %idx
  store <16 x i64> %val, ptr %arrayidx.0, align 32
  %arrayidx.1 = getelementptr inbounds [16 x <16 x i64>], ptr %array, i32 0, i32 2
  store <16 x i64> %val, ptr %arrayidx.1, align 32
  ret void
}
