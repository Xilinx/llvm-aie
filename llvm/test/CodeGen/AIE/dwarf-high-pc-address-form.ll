;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
;
; AIE emits DW_AT_high_pc as an address even for DWARF v4, so that the linker
; tombstones both ends of a function discarded by --gc-sections.

; RUN: llc -mtriple=aie2p -filetype=obj %s -o %t.aie2p.o
; RUN: llvm-dwarfdump --debug-info -v %t.aie2p.o | FileCheck %s
; RUN: llc -mtriple=aie2ps -filetype=obj %s -o %t.aie2ps.o
; RUN: llvm-dwarfdump --debug-info -v %t.aie2ps.o | FileCheck %s

; CHECK:      DW_TAG_compile_unit
; CHECK:        DW_AT_low_pc [DW_FORM_addr]
; CHECK-NEXT:   DW_AT_high_pc [DW_FORM_addr]
; CHECK:      DW_TAG_subprogram
; CHECK-NEXT:   DW_AT_low_pc [DW_FORM_addr]
; CHECK-NEXT:   DW_AT_high_pc [DW_FORM_addr]

define i32 @foo(i32 %x) !dbg !6 {
entry:
  %xor = xor i32 %x, 5, !dbg !9
  ret i32 %xor, !dbg !10
}

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3}

!0 = distinct !DICompileUnit(language: DW_LANG_C11, file: !1, producer: "clang", isOptimized: true, runtimeVersion: 0, emissionKind: LineTablesOnly, nameTableKind: None, debugInfoForProfiling: true)
!1 = !DIFile(filename: "t.c", directory: "/")
!2 = !{i32 7, !"Dwarf Version", i32 4}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!6 = distinct !DISubprogram(name: "foo", scope: !1, file: !1, line: 1, type: !7, scopeLine: 1, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !0)
!7 = !DISubroutineType(types: !8)
!8 = !{}
!9 = !DILocation(line: 1, column: 31, scope: !6)
!10 = !DILocation(line: 1, column: 22, scope: !6)
