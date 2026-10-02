; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates

; The loop below has an MII of 32 (sixteen 512-bit stores), above the
; target-independent -pipeliner-max-mii default of 27. AIE raises the bound
; through -aie-pipeliner-max-mii, so the prepipeliner takes the loop. An
; explicit -pipeliner-max-mii overrides the target in either direction.

; RUN: llc -mtriple=aie2 -O2 %s -o /dev/null -pass-remarks-output=- \
; RUN:   -pass-remarks-filter=pipeliner \
; RUN:   | FileCheck %s --check-prefixes=ACCEPT,PRE
; RUN: llc -mtriple=aie2 -O2 %s -o /dev/null -pass-remarks-output=- \
; RUN:   -pass-remarks-filter=pipeliner -aie-pipeliner-max-mii=30 \
; RUN:   | FileCheck %s --check-prefixes=REJECT30,POST
; RUN: llc -mtriple=aie2 -O2 %s -o /dev/null -pass-remarks-output=- \
; RUN:   -pass-remarks-filter=pipeliner -pipeliner-max-mii=30 \
; RUN:   | FileCheck %s --check-prefixes=REJECT30,POST
; RUN: llc -mtriple=aie2 -O2 %s -o /dev/null -pass-remarks-output=- \
; RUN:   -pass-remarks-filter=pipeliner -aie-pipeliner-max-mii=30 \
; RUN:   -pipeliner-max-mii=50 \
; RUN:   | FileCheck %s --check-prefixes=ACCEPT,PRE

; REJECT30:      - String: 'Minimal Initiation Interval too large: '
; REJECT30-NEXT: - MII: '32'
; REJECT30-NEXT: - String: ' > '
; REJECT30-NEXT: - SwpMaxMii: '30'
; REJECT30-NOT:  Schedule found with Initiation Interval

; ACCEPT-NOT:    Minimal Initiation Interval too large
; ACCEPT:        - String: 'Schedule found with Initiation Interval: '
; ACCEPT-NEXT:   - II: '32'

; POST:          - Pipeliner: postpipeliner
; POST-NEXT:     - II: '32'
; PRE:           - Pipeliner: prepipeliner
; PRE-NEXT:      - II: '49'

define void @wide(ptr addrspace(5) noalias %a, ptr addrspace(6) noalias %b, i32 %n) {
entry:
  br label %loop
loop:
  %i = phi i32 [ 0, %entry ], [ %i.next, %loop ]
  %pa = phi ptr addrspace(5) [ %a, %entry ], [ %pa.next, %loop ]
  %pb = phi ptr addrspace(6) [ %b, %entry ], [ %pb.next, %loop ]
  %x = load <16 x i32>, ptr addrspace(5) %pa, align 64
  %pb1 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 1
  %pb2 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 2
  %pb3 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 3
  %pb4 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 4
  %pb5 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 5
  %pb6 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 6
  %pb7 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 7
  %pb8 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 8
  %pb9 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 9
  %pb10 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 10
  %pb11 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 11
  %pb12 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 12
  %pb13 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 13
  %pb14 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 14
  %pb15 = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 15
  store <16 x i32> %x, ptr addrspace(6) %pb, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb1, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb2, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb3, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb4, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb5, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb6, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb7, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb8, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb9, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb10, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb11, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb12, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb13, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb14, align 64
  store <16 x i32> %x, ptr addrspace(6) %pb15, align 64
  %pa.next = getelementptr <16 x i32>, ptr addrspace(5) %pa, i32 1
  %pb.next = getelementptr <16 x i32>, ptr addrspace(6) %pb, i32 16
  %i.next = add i32 %i, 1
  %c = icmp ne i32 %i.next, %n
  br i1 %c, label %loop, label %exit, !llvm.loop !0
exit:
  ret void
}

!0 = distinct !{!0, !1}
!1 = !{!"llvm.loop.itercount.range", i64 8}
