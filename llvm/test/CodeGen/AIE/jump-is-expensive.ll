;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc -O2 -mtriple=aie2   %s -o - | FileCheck %s
; RUN: llc -O2 -mtriple=aie2p  %s -o - | FileCheck %s
; RUN: llc -O2 -mtriple=aie2ps %s -o - | FileCheck %s
;
; The following runs disable the jumpIsExpensive hook to show the previous
; behaviour, where the short-circuit condition was split and the guarded
; branch duplicated into two separate conditional jumps.
; RUN: llc -O2 -mtriple=aie2   -jump-is-expensive=false %s -o - | FileCheck --check-prefix=SPLIT %s
; RUN: llc -O2 -mtriple=aie2p  -jump-is-expensive=false %s -o - | FileCheck --check-prefix=SPLIT %s
; RUN: llc -O2 -mtriple=aie2ps -jump-is-expensive=false %s -o - | FileCheck --check-prefix=SPLIT %s

; Test the effect of setJumpIsExpensive(true) for AIE. With the hook enabled a
; short-circuit `a && b` condition is not split: both comparisons are combined
; into a single guard value (or) and only one conditional jump (jnz) is emitted.
; Without the hook (SPLIT) the condition is split into two conditional jumps,
; each duplicating the branch to the same target, which increases branch count.

; CHECK-LABEL: and_cond:
; CHECK:         ge  {{.*}}
; CHECK:         ge  {{.*}}
; CHECK:         or  {{.*}}
; CHECK-COUNT-1: jnz {{.*}}
; CHECK-NOT:     jnz
; CHECK:         j   #f0

; SPLIT-LABEL: and_cond:
; SPLIT:         ge  {{.*}}
; SPLIT:         jnz {{.*}}, #[[END:.LBB[0-9_]+]]
; SPLIT:         ge  {{.*}}
; SPLIT:         jnz {{.*}}, #[[END]]
; SPLIT:         j   #f0

define void @and_cond(i32 %a, i32 %b) {
entry:
  %cmpa = icmp sgt i32 %a, 0
  %cmpb = icmp sgt i32 %b, 0
  %and = and i1 %cmpa, %cmpb
  br i1 %and, label %if.then, label %if.end

if.then:
  tail call void @f0()
  br label %if.end

if.end:
  ret void
}

declare void @f0()
