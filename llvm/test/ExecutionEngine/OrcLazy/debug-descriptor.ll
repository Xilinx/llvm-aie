; REQUIRES: native && target-x86_64

; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
; 
; Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates


; RUN: lli --jit-linker=rtdyld --orc-lazy-debug=jit-debug-descriptor %s 2>&1 | FileCheck %s
; RUN: lli --jit-linker=jitlink --orc-lazy-debug=jit-debug-descriptor %s 2>&1 | FileCheck %s
; XFAIL: *
;
; Initial entry should be empty:
; CHECK: jit_debug_descriptor 0x0000000000000000
;
; After adding the module it must not be empty anymore:
; CHECK: jit_debug_descriptor 0x
; CHECK-NOT:                    000000000000000
; CHECK-SAME:                               {{[048c]}}

define i32 @main() !dbg !9 {
  ret i32 0, !dbg !14
}

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.dbg.cu = !{!5}
!llvm.ident = !{!8}

!0 = !{i32 2, !"SDK Version", [3 x i32] [i32 10, i32 15, i32 6]}
!1 = !{i32 7, !"Dwarf Version", i32 4}
!2 = !{i32 2, !"Debug Info Version", i32 3}
!3 = !{i32 1, !"wchar_size", i32 4}
!4 = !{i32 7, !"PIC Level", i32 2}
!5 = distinct !DICompileUnit(language: DW_LANG_C99, file: !6, producer: "compiler version", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !7, nameTableKind: None)
!6 = !DIFile(filename: "source-file.c", directory: "/workspace")
!7 = !{}
!8 = !{!"compiler version"}
!9 = distinct !DISubprogram(name: "main", scope: !6, file: !6, line: 4, type: !10, scopeLine: 4, spFlags: DISPFlagDefinition, unit: !5, retainedNodes: !7)
!10 = !DISubroutineType(types: !11)
!11 = !{!12}
!12 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!13 = !DILocation(line: 5, column: 3, scope: !9)
!14 = !DILocation(line: 6, column: 3, scope: !9)
