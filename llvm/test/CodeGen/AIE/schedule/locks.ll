;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -mtriple=aie --issue-limit=1 < %s | FileCheck %s
; RUN: llc -mtriple=aie --issue-limit=1 -verify-machineinstrs < %s \
; RUN:   | FileCheck %s
; RUN: llc -mtriple=aie --issue-limit=8 < %s \
; RUN:   | FileCheck %s --check-prefix PACKET

; CHECK-LABEL: test:
; CHECK:             mov.u20 r12, #0
; CHECK-NEXT:        acq     #51, #1, r12
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        acq     #53, #1, r12
; CHECK-NEXT:        mov.u20 r12, #1
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        nop
; CHECK-NEXT:        rel     #53, #1, r12
; CHECK:             ret lr

; PACKET:        acq     #53, #1, r12; mov.u20 r12, #1

@a = external dso_local global [256 x i32]

declare i8* @malloc(i64)

declare void @free(i8*)

declare void @debug_i32(i32)

; Function Attrs: norecurse nounwind readnone mustprogress
declare void @llvm.aie.lock.acquire.reg(i32, i32) #0

; Function Attrs: norecurse nounwind readnone mustprogress
declare void @llvm.aie.lock.release.reg(i32, i32) #0

define void @test() {
  call void @llvm.aie.lock.acquire.reg(i32 51, i32 0)
  call void @llvm.aie.lock.acquire.reg(i32 53, i32 0)
  call void @llvm.aie.lock.release.reg(i32 53, i32 1)
  ret void
}

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }

!llvm.module.flags = !{!0}

!0 = !{i32 2, !"Debug Info Version", i32 3}
