;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
; RUN: llc %s -mtriple=aie --issue-limit=1 -filetype=obj -o %t
; RUN: llc -mtriple=aie --issue-limit=1 < %s | FileCheck %s
; RUN: llc -mtriple=aie --issue-limit=8 < %s \
; RUN:   | FileCheck %s --check-prefix=PACKET

; CHECK-LABEL: core33
; CHECK:      mov.u20 p0, #(a+32)
; CHECK-NEXT: lda     r12, [p0]
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: add     r12, r12, #9
; CHECK-NEXT: st      r12, [p0]
; CHECK-NEXT: mov.u20 p0, #(a+28)
; CHECK-NEXT: lda     r12, [p0]
; CHECK-NEXT: nop

; PACKET: st      r12, [p0]; mov.u20 p0, #(a+28)

; ModuleID = 'LLVMDialectModule'
source_filename = "LLVMDialectModule"

@a = external dso_local global [16 x i32]

define void @core33() {
  %t1 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 0), align 4
  %t2 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 1), align 4
  %t3 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 2), align 4
  %t4 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 3), align 4
  %t5 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 4), align 4
  %t6 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 5), align 4
  %t7 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 6), align 4
  %t8 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 7), align 4
  %t9 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 8), align 4
  %t10 = add i32 1, 1
  %t11 = add i32 %t1, 1
  %t12 = add i32 %t2, 2
  %t13 = add i32 %t3, 3
  %t14 = add i32 %t4, 4
  %t15 = add i32 %t5, 5
  %t16 = add i32 %t6, 6
  %t17 = add i32 %t7, 7
  %t18 = add i32 %t8, 8
  %t19 = add i32 %t9, 9
  store i32 %t11, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 0), align 4
  store i32 %t12, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 1), align 4
  store i32 %t13, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 2), align 4
  store i32 %t14, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 3), align 4
  store i32 %t15, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 4), align 4
  store i32 %t16, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 5), align 4
  store i32 %t17, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 6), align 4
  store i32 %t18, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 7), align 4
  store i32 %t19, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 8), align 4
  ret void
}

; define void @core43() {
;   ret void
; }
; define void @core33() !dbg !3 {
;   store i32 377, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 0), align 4, !dbg !7
;   ret void, !dbg !9
; }

; define void @core43() !dbg !10 {
;   %1 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @a, i64 0, i64 0), align 4, !dbg !11
;   ret void, !dbg !13
; }

attributes #0 = { nounwind }
attributes #1 = { nounwind readnone }

; !llvm.dbg.cu = !{!0}
; !llvm.module.flags = !{!2}

; !0 = distinct !DICompileUnit(language: DW_LANG_C, file: !1, producer: "mlir", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug)
; !1 = !DIFile(filename: "LLVMDialectModule", directory: "/")
; !2 = !{i32 2, !"Debug Info Version", i32 3}
; !3 = distinct !DISubprogram(name: "core33", linkageName: "core33", scope: null, file: !4, line: 16, type: !5, scopeLine: 16, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
; !4 = !DIFile(filename: "<stdin>", directory: "/wrk/hdstaff/stephenn/training-architectures/src/acdc/aie")
; !5 = !DISubroutineType(types: !6)
; !6 = !{}
; !7 = !DILocation(line: 23, column: 5, scope: !8)
; !8 = !DILexicalBlockFile(scope: !3, file: !4, discriminator: 0)
; !9 = !DILocation(line: 24, column: 5, scope: !8)
; !10 = distinct !DISubprogram(name: "core43", linkageName: "core43", scope: null, file: !4, line: 26, type: !5, scopeLine: 26, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0, retainedNodes: !6)
; !11 = !DILocation(line: 32, column: 10, scope: !12)
; !12 = !DILexicalBlockFile(scope: !10, file: !4, discriminator: 0)
; !13 = !DILocation(line: 33, column: 5, scope: !12)
