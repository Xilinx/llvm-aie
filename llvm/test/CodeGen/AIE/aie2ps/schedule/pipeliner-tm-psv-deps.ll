; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 -mtriple=aie2ps %s -o /dev/null --debug-only=pipeliner 2>&1 \
; RUN:   | FileCheck %s
; REQUIRES: asserts

; Tile memory stores only carry the TileMemory pseudo source value, which
; cannot alias data memory. The MachinePipeliner must not add loop-carried
; order edges between them and data memory accesses; otherwise the data load
; feeding the tile memory store forms a recurrence with the load latency.

; CHECK:       SU(1): {{.*}}LDA_dms_lda_scalar_ld_pstm_nrm_imm {{.*}}(load (s32) from %ir.pin)
; CHECK:       SU(2): {{.*}}ST_TM_idx_imm {{.*}}(store (s32) into custom "TileMemory")
; CHECK:       ===== Loop Carried Edges Begin =====
; CHECK-NEXT:  ===== Loop Carried Edges End =====
; CHECK-NEXT:  calculateResMII:
; CHECK-NEXT:  Return Res MII:1
; CHECK-NEXT:  MII = 1 {{.*}} (rec=1, res=1)
define void @load_to_write_tm(ptr %in, ptr %tm) {
entry:
  call void @llvm.set.loop.iterations.i32(i32 32)
  br label %loop

loop:
  %pin = phi ptr [ %in, %entry ], [ %pin.next, %loop ]
  %v = load i32, ptr %pin, align 4
  call void @llvm.aie2ps.write.tm(i32 %v, ptr %tm)
  %pin.next = getelementptr inbounds i8, ptr %pin, i20 4
  %c = call i1 @llvm.loop.decrement.i32(i32 1)
  br i1 %c, label %loop, label %exit, !llvm.loop !0

exit:
  ret void
}

declare void @llvm.aie2ps.write.tm(i32, ptr)
declare void @llvm.set.loop.iterations.i32(i32)
declare i1 @llvm.loop.decrement.i32(i32)

!0 = distinct !{!0, !1, !2}
!1 = !{!"llvm.loop.mustprogress"}
!2 = !{!"llvm.loop.itercount.range", i64 10}
