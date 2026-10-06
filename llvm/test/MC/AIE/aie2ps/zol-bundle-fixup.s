//===- zol-bundle-fixup.s ---------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// NOTE: Test ZOL fixups in all VLIW bundle formats (comprehensive coverage)
// RUN: llvm-mc -triple aie2ps %s -filetype=obj -o %t.o
// RUN: llvm-objdump --triple=aie2ps -d --no-print-imm-hex %t.o | FileCheck %s

// NOTE: Test ZOL fixups in all VLIW bundle formats (comprehensive coverage)
// Test ZOL (Zero-Overhead Loop) fixups in all VLIW bundle configurations.
// ALU slot uses 5+6 bit split encoding (add.nc), MV slot uses 4+7 bit split
// encoding (addm.nc). Each test is named after the composite format it uses.
//
// Tests are ordered by fixup number (49-74) with ls and le interleaved.
// Label convention: test_{FORMAT}_{slot}_{ls|le}
//
// Fixup-to-Format Mapping Table:
// -----------------------------------------------------------------------
// Fixup | Size  | Slot | Composite Formats
// -----------------------------------------------------------------------
//    49 |    16B | ALU  | I128_LDA_LDB_ST_ALU_MV_VEC
//    50 |     4B | ALU  | I32_ALU
//    51 |     6B | ALU  | I48_LDA_ALU, I48_LDB_ALU, I48_ST_ALU
//    52 |     6B | ALU  | I48_ALU_MV
//    53 |     8B | ALU  | I64_ALU_VEC
//    54 |     8B | ALU  | I64_LDA_LDB_ALU, I64_ST_LDB_ALU
//    55 |    10B | ALU  | I80_LDA_ST_ALU
//    56 |    10B | ALU  | I80_LDA_ALU_MV, I80_LDA_ALU_VEC, I80_ST_ALU_MV, I80_ST_ALU_VEC
//    57 |    10B | ALU  | I80_LDB_ALU_MV, I80_LDB_ALU_VEC, I80_ALU_MV_VEC
//    58 |    12B | ALU  | I96_LDA_LDB_ALU_ST, I96_LDA_LDB_ALU_MV, I96_LDA_LDB_ALU_VEC, I96_LDA_ST_ALU_MV, I96_LDA_ST_ALU_VEC
//    59 |    12B | ALU  | I96_LDA_ALU_MV_VEC, I96_LDB_ALU_MV_VEC
//    60 |    12B | ALU  | I96_ST_LDB_ALU_MV, I96_ST_LDB_ALU_VEC, I96_ST_ALU_MV_VEC
//    61 |    14B | ALU  | I112_LDA_LDB_ALU_MV_VEC, I112_LDA_LDB_ALU_MV_ST, I112_LDA_LDB_ALU_ST_VEC, I112_ST_LDB_ALU_MV_VEC
//    62 |    16B | MV   | I128_LDA_LDB_ST_ALU_MV_VEC
//    63 |     4B | MV   | I32_MV
//    64 |     6B | MV   | I48_LDA_MV, I48_LDB_MV
//    65 |     6B | MV   | I48_ALU_MV
//    66 |     8B | MV   | I64_ST_MV
//    67 |     8B | MV   | I64_MV_VEC
//    68 |    10B | MV   | I80_LDA_LDB_MV, I80_LDA_ST_MV, I80_LDA_ALU_MV, I80_ST_LDB_MV, I80_LDB_ALU_MV, I80_ST_ALU_MV
//    69 |    10B | MV   | I80_LDA_MV_VEC, I80_LDB_MV_VEC, I80_ST_MV_VEC, I80_ALU_MV_VEC
//    70 |    12B | MV   | I96_LDA_LDB_ST_MV, I96_LDA_LDB_ALU_MV, I96_ST_LDB_ALU_MV
//    71 |    12B | MV   | I96_LDA_LDB_MV_VEC, I96_LDA_ALU_MV_VEC
//    72 |    12B | MV   | I96_LDA_ST_ALU_MV, I96_ST_LDB_MV_VEC, I96_LDB_ALU_MV_VEC
//    73 |    12B | MV   | I96_ST_ALU_MV_VEC
//    74 |    14B | MV   | I112_LDA_ST_MV_VEC, I112_LDA_LDB_ALU_MV_ST, I112_LDA_LDB_ALU_MV_VEC, I112_ST_LDB_ALU_MV_VEC
// -----------------------------------------------------------------------

// Fixup 49: I128_LDA_LDB_ST_ALU_MV_VEC - ALU ZOL, 16-byte bundle, loop start
// CHECK-LABEL: <test_I128_LDA_LDB_ST_ALU_MV_VEC_alu_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}nops{{.*}}add.nc	ls, pc, #16{{.*}}nopm{{.*}}nopv
test_I128_LDA_LDB_ST_ALU_MV_VEC_alu_ls:
    nopa; nopb; nops; add.nc ls, pc, #test_I128_LDA_LDB_ST_ALU_MV_VEC_alu_ls_target; nopm; nopv
test_I128_LDA_LDB_ST_ALU_MV_VEC_alu_ls_target:
    nop

// Fixup 49: I128_LDA_LDB_ST_ALU_MV_VEC - ALU ZOL, 16-byte bundle, loop end
// CHECK-LABEL: <test_I128_LDA_LDB_ST_ALU_MV_VEC_alu_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}nops{{.*}}add.nc	le, pc, #16{{.*}}nopm{{.*}}nopv
test_I128_LDA_LDB_ST_ALU_MV_VEC_alu_le:
    nopa; nopb; nops; add.nc le, pc, #test_I128_LDA_LDB_ST_ALU_MV_VEC_alu_le_target; nopm; nopv
test_I128_LDA_LDB_ST_ALU_MV_VEC_alu_le_target:
    nop

// Fixup 50: I32_ALU - ALU ZOL, 4-byte bundle, loop start
// CHECK-LABEL: <test_I32_ALU_alu_ls>:
// CHECK:         add.nc	ls, pc, #4
test_I32_ALU_alu_ls:
    add.nc ls, pc, #test_I32_ALU_alu_ls_target
test_I32_ALU_alu_ls_target:
    nop

// Fixup 50: I32_ALU - ALU ZOL, 4-byte bundle, loop end
// CHECK-LABEL: <test_I32_ALU_alu_le>:
// CHECK:         add.nc	le, pc, #4
test_I32_ALU_alu_le:
    add.nc le, pc, #test_I32_ALU_alu_le_target
test_I32_ALU_alu_le_target:
    nop

// Fixup 51: I48_LDA_ALU - ALU ZOL, 6-byte bundle, loop start
// CHECK-LABEL: <test_I48_LDA_ALU_alu_ls>:
// CHECK:         nopa{{.*}}add.nc	ls, pc, #6
test_I48_LDA_ALU_alu_ls:
    nopa; add.nc ls, pc, #test_I48_LDA_ALU_alu_ls_target
test_I48_LDA_ALU_alu_ls_target:
    nop

// Fixup 51: I48_LDA_ALU - ALU ZOL, 6-byte bundle, loop end
// CHECK-LABEL: <test_I48_LDA_ALU_alu_le>:
// CHECK:         nopa{{.*}}add.nc	le, pc, #6
test_I48_LDA_ALU_alu_le:
    nopa; add.nc le, pc, #test_I48_LDA_ALU_alu_le_target
test_I48_LDA_ALU_alu_le_target:
    nop

// Fixup 51: I48_LDB_ALU - ALU ZOL, 6-byte bundle, loop start
// CHECK-LABEL: <test_I48_LDB_ALU_alu_ls>:
// CHECK:         nopb{{.*}}add.nc	ls, pc, #6
test_I48_LDB_ALU_alu_ls:
    nopb; add.nc ls, pc, #test_I48_LDB_ALU_alu_ls_target
test_I48_LDB_ALU_alu_ls_target:
    nop

// Fixup 51: I48_LDB_ALU - ALU ZOL, 6-byte bundle, loop end
// CHECK-LABEL: <test_I48_LDB_ALU_alu_le>:
// CHECK:         nopb{{.*}}add.nc	le, pc, #6
test_I48_LDB_ALU_alu_le:
    nopb; add.nc le, pc, #test_I48_LDB_ALU_alu_le_target
test_I48_LDB_ALU_alu_le_target:
    nop

// Fixup 51: I48_ST_ALU - ALU ZOL, 6-byte bundle, loop start
// CHECK-LABEL: <test_I48_ST_ALU_alu_ls>:
// CHECK:         nops{{.*}}add.nc	ls, pc, #6
test_I48_ST_ALU_alu_ls:
    nops; add.nc ls, pc, #test_I48_ST_ALU_alu_ls_target
test_I48_ST_ALU_alu_ls_target:
    nop

// Fixup 51: I48_ST_ALU - ALU ZOL, 6-byte bundle, loop end
// CHECK-LABEL: <test_I48_ST_ALU_alu_le>:
// CHECK:         nops{{.*}}add.nc	le, pc, #6
test_I48_ST_ALU_alu_le:
    nops; add.nc le, pc, #test_I48_ST_ALU_alu_le_target
test_I48_ST_ALU_alu_le_target:
    nop

// Fixup 52: I48_ALU_MV - ALU ZOL, 6-byte bundle, loop start
// CHECK-LABEL: <test_I48_ALU_MV_alu_ls>:
// CHECK:         add.nc	ls, pc, #6{{.*}}nopm
test_I48_ALU_MV_alu_ls:
    add.nc ls, pc, #test_I48_ALU_MV_alu_ls_target; nopm
test_I48_ALU_MV_alu_ls_target:
    nop

// Fixup 52: I48_ALU_MV - ALU ZOL, 6-byte bundle, loop end
// CHECK-LABEL: <test_I48_ALU_MV_alu_le>:
// CHECK:         add.nc	le, pc, #6{{.*}}nopm
test_I48_ALU_MV_alu_le:
    add.nc le, pc, #test_I48_ALU_MV_alu_le_target; nopm
test_I48_ALU_MV_alu_le_target:
    nop

// Fixup 53: I64_ALU_VEC - ALU ZOL, 8-byte bundle, loop start
// CHECK-LABEL: <test_I64_ALU_VEC_alu_ls>:
// CHECK:         add.nc	ls, pc, #8{{.*}}nopv
test_I64_ALU_VEC_alu_ls:
    add.nc ls, pc, #test_I64_ALU_VEC_alu_ls_target; nopv
test_I64_ALU_VEC_alu_ls_target:
    nop

// Fixup 53: I64_ALU_VEC - ALU ZOL, 8-byte bundle, loop end
// CHECK-LABEL: <test_I64_ALU_VEC_alu_le>:
// CHECK:         add.nc	le, pc, #8{{.*}}nopv
test_I64_ALU_VEC_alu_le:
    add.nc le, pc, #test_I64_ALU_VEC_alu_le_target; nopv
test_I64_ALU_VEC_alu_le_target:
    nop

// Fixup 54: I64_LDA_LDB_ALU - ALU ZOL, 8-byte bundle, loop start
// CHECK-LABEL: <test_I64_LDA_LDB_ALU_alu_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #8
test_I64_LDA_LDB_ALU_alu_ls:
    nopa; nopb; add.nc ls, pc, #test_I64_LDA_LDB_ALU_alu_ls_target
test_I64_LDA_LDB_ALU_alu_ls_target:
    nop

// Fixup 54: I64_LDA_LDB_ALU - ALU ZOL, 8-byte bundle, loop end
// CHECK-LABEL: <test_I64_LDA_LDB_ALU_alu_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	le, pc, #8
test_I64_LDA_LDB_ALU_alu_le:
    nopa; nopb; add.nc le, pc, #test_I64_LDA_LDB_ALU_alu_le_target
test_I64_LDA_LDB_ALU_alu_le_target:
    nop

// Fixup 54: I64_ST_LDB_ALU - ALU ZOL, 8-byte bundle, loop start
// CHECK-LABEL: <test_I64_ST_LDB_ALU_alu_ls>:
// CHECK:         nops{{.*}}nopb{{.*}}add.nc	ls, pc, #8
test_I64_ST_LDB_ALU_alu_ls:
    nops; nopb; add.nc ls, pc, #test_I64_ST_LDB_ALU_alu_ls_target
test_I64_ST_LDB_ALU_alu_ls_target:
    nop

// Fixup 54: I64_ST_LDB_ALU - ALU ZOL, 8-byte bundle, loop end
// CHECK-LABEL: <test_I64_ST_LDB_ALU_alu_le>:
// CHECK:         nops{{.*}}nopb{{.*}}add.nc	le, pc, #8
test_I64_ST_LDB_ALU_alu_le:
    nops; nopb; add.nc le, pc, #test_I64_ST_LDB_ALU_alu_le_target
test_I64_ST_LDB_ALU_alu_le_target:
    nop

// Fixup 55: I80_LDA_ST_ALU - ALU ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDA_ST_ALU_alu_ls>:
// CHECK:         nopa{{.*}}nops{{.*}}add.nc	ls, pc, #10
test_I80_LDA_ST_ALU_alu_ls:
    nopa; nops; add.nc ls, pc, #test_I80_LDA_ST_ALU_alu_ls_target
test_I80_LDA_ST_ALU_alu_ls_target:
    nop

// Fixup 55: I80_LDA_ST_ALU - ALU ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDA_ST_ALU_alu_le>:
// CHECK:         nopa{{.*}}nops{{.*}}add.nc	le, pc, #10
test_I80_LDA_ST_ALU_alu_le:
    nopa; nops; add.nc le, pc, #test_I80_LDA_ST_ALU_alu_le_target
test_I80_LDA_ST_ALU_alu_le_target:
    nop

// Fixup 56: I80_LDA_ALU_MV - ALU ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDA_ALU_MV_alu_ls>:
// CHECK:         nopa{{.*}}add.nc	ls, pc, #10{{.*}}nopm
test_I80_LDA_ALU_MV_alu_ls:
    nopa; add.nc ls, pc, #test_I80_LDA_ALU_MV_alu_ls_target; nopm
test_I80_LDA_ALU_MV_alu_ls_target:
    nop

// Fixup 56: I80_LDA_ALU_MV - ALU ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDA_ALU_MV_alu_le>:
// CHECK:         nopa{{.*}}add.nc	le, pc, #10{{.*}}nopm
test_I80_LDA_ALU_MV_alu_le:
    nopa; add.nc le, pc, #test_I80_LDA_ALU_MV_alu_le_target; nopm
test_I80_LDA_ALU_MV_alu_le_target:
    nop

// Fixup 56: I80_LDA_ALU_VEC - ALU ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDA_ALU_VEC_alu_ls>:
// CHECK:         nopa{{.*}}add.nc	ls, pc, #10{{.*}}nopv
test_I80_LDA_ALU_VEC_alu_ls:
    nopa; add.nc ls, pc, #test_I80_LDA_ALU_VEC_alu_ls_target; nopv
test_I80_LDA_ALU_VEC_alu_ls_target:
    nop

// Fixup 56: I80_LDA_ALU_VEC - ALU ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDA_ALU_VEC_alu_le>:
// CHECK:         nopa{{.*}}add.nc	le, pc, #10{{.*}}nopv
test_I80_LDA_ALU_VEC_alu_le:
    nopa; add.nc le, pc, #test_I80_LDA_ALU_VEC_alu_le_target; nopv
test_I80_LDA_ALU_VEC_alu_le_target:
    nop

// Fixup 56: I80_ST_ALU_MV - ALU ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_ST_ALU_MV_alu_ls>:
// CHECK:         nops{{.*}}add.nc	ls, pc, #10{{.*}}nopm
test_I80_ST_ALU_MV_alu_ls:
    nops; add.nc ls, pc, #test_I80_ST_ALU_MV_alu_ls_target; nopm
test_I80_ST_ALU_MV_alu_ls_target:
    nop

// Fixup 56: I80_ST_ALU_MV - ALU ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_ST_ALU_MV_alu_le>:
// CHECK:         nops{{.*}}add.nc	le, pc, #10{{.*}}nopm
test_I80_ST_ALU_MV_alu_le:
    nops; add.nc le, pc, #test_I80_ST_ALU_MV_alu_le_target; nopm
test_I80_ST_ALU_MV_alu_le_target:
    nop

// Fixup 56: I80_ST_ALU_VEC - ALU ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_ST_ALU_VEC_alu_ls>:
// CHECK:         nops{{.*}}add.nc	ls, pc, #10{{.*}}nopv
test_I80_ST_ALU_VEC_alu_ls:
    nops; add.nc ls, pc, #test_I80_ST_ALU_VEC_alu_ls_target; nopv
test_I80_ST_ALU_VEC_alu_ls_target:
    nop

// Fixup 56: I80_ST_ALU_VEC - ALU ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_ST_ALU_VEC_alu_le>:
// CHECK:         nops{{.*}}add.nc	le, pc, #10{{.*}}nopv
test_I80_ST_ALU_VEC_alu_le:
    nops; add.nc le, pc, #test_I80_ST_ALU_VEC_alu_le_target; nopv
test_I80_ST_ALU_VEC_alu_le_target:
    nop

// Fixup 57: I80_LDB_ALU_MV - ALU ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDB_ALU_MV_alu_ls>:
// CHECK:         nopb{{.*}}add.nc	ls, pc, #10{{.*}}nopm
test_I80_LDB_ALU_MV_alu_ls:
    nopb; add.nc ls, pc, #test_I80_LDB_ALU_MV_alu_ls_target; nopm
test_I80_LDB_ALU_MV_alu_ls_target:
    nop

// Fixup 57: I80_LDB_ALU_MV - ALU ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDB_ALU_MV_alu_le>:
// CHECK:         nopb{{.*}}add.nc	le, pc, #10{{.*}}nopm
test_I80_LDB_ALU_MV_alu_le:
    nopb; add.nc le, pc, #test_I80_LDB_ALU_MV_alu_le_target; nopm
test_I80_LDB_ALU_MV_alu_le_target:
    nop

// Fixup 57: I80_LDB_ALU_VEC - ALU ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDB_ALU_VEC_alu_ls>:
// CHECK:         nopb{{.*}}add.nc	ls, pc, #10{{.*}}nopv
test_I80_LDB_ALU_VEC_alu_ls:
    nopb; add.nc ls, pc, #test_I80_LDB_ALU_VEC_alu_ls_target; nopv
test_I80_LDB_ALU_VEC_alu_ls_target:
    nop

// Fixup 57: I80_LDB_ALU_VEC - ALU ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDB_ALU_VEC_alu_le>:
// CHECK:         nopb{{.*}}add.nc	le, pc, #10{{.*}}nopv
test_I80_LDB_ALU_VEC_alu_le:
    nopb; add.nc le, pc, #test_I80_LDB_ALU_VEC_alu_le_target; nopv
test_I80_LDB_ALU_VEC_alu_le_target:
    nop

// Fixup 57: I80_ALU_MV_VEC - ALU ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_ALU_MV_VEC_alu_ls>:
// CHECK:         add.nc	ls, pc, #10{{.*}}nopm{{.*}}nopv
test_I80_ALU_MV_VEC_alu_ls:
    add.nc ls, pc, #test_I80_ALU_MV_VEC_alu_ls_target; nopm; nopv
test_I80_ALU_MV_VEC_alu_ls_target:
    nop

// Fixup 57: I80_ALU_MV_VEC - ALU ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_ALU_MV_VEC_alu_le>:
// CHECK:         add.nc	le, pc, #10{{.*}}nopm{{.*}}nopv
test_I80_ALU_MV_VEC_alu_le:
    add.nc le, pc, #test_I80_ALU_MV_VEC_alu_le_target; nopm; nopv
test_I80_ALU_MV_VEC_alu_le_target:
    nop

// Fixup 58: I96_LDA_LDB_ALU_ST - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_ST_alu_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #12{{.*}}nops
test_I96_LDA_LDB_ALU_ST_alu_ls:
    nopa; nopb; nops; add.nc ls, pc, #test_I96_LDA_LDB_ALU_ST_alu_ls_target
test_I96_LDA_LDB_ALU_ST_alu_ls_target:
    nop

// Fixup 58: I96_LDA_LDB_ALU_ST - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_ST_alu_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	le, pc, #12{{.*}}nops
test_I96_LDA_LDB_ALU_ST_alu_le:
    nopa; nopb; nops; add.nc le, pc, #test_I96_LDA_LDB_ALU_ST_alu_le_target
test_I96_LDA_LDB_ALU_ST_alu_le_target:
    nop

// Fixup 58: I96_LDA_LDB_ALU_MV - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_MV_alu_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #12{{.*}}nopm
test_I96_LDA_LDB_ALU_MV_alu_ls:
    nopa; nopb; add.nc ls, pc, #test_I96_LDA_LDB_ALU_MV_alu_ls_target; nopm
test_I96_LDA_LDB_ALU_MV_alu_ls_target:
    nop

// Fixup 58: I96_LDA_LDB_ALU_MV - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_MV_alu_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	le, pc, #12{{.*}}nopm
test_I96_LDA_LDB_ALU_MV_alu_le:
    nopa; nopb; add.nc le, pc, #test_I96_LDA_LDB_ALU_MV_alu_le_target; nopm
test_I96_LDA_LDB_ALU_MV_alu_le_target:
    nop

// Fixup 58: I96_LDA_LDB_ALU_VEC - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_VEC_alu_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #12{{.*}}nopv
test_I96_LDA_LDB_ALU_VEC_alu_ls:
    nopa; nopb; add.nc ls, pc, #test_I96_LDA_LDB_ALU_VEC_alu_ls_target; nopv
test_I96_LDA_LDB_ALU_VEC_alu_ls_target:
    nop

// Fixup 58: I96_LDA_LDB_ALU_VEC - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_VEC_alu_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	le, pc, #12{{.*}}nopv
test_I96_LDA_LDB_ALU_VEC_alu_le:
    nopa; nopb; add.nc le, pc, #test_I96_LDA_LDB_ALU_VEC_alu_le_target; nopv
test_I96_LDA_LDB_ALU_VEC_alu_le_target:
    nop

// Fixup 58: I96_LDA_ST_ALU_MV - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_ST_ALU_MV_alu_ls>:
// CHECK:         nopa{{.*}}nops{{.*}}add.nc	ls, pc, #12{{.*}}nopm
test_I96_LDA_ST_ALU_MV_alu_ls:
    nopa; nops; add.nc ls, pc, #test_I96_LDA_ST_ALU_MV_alu_ls_target; nopm
test_I96_LDA_ST_ALU_MV_alu_ls_target:
    nop

// Fixup 58: I96_LDA_ST_ALU_MV - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_ST_ALU_MV_alu_le>:
// CHECK:         nopa{{.*}}nops{{.*}}add.nc	le, pc, #12{{.*}}nopm
test_I96_LDA_ST_ALU_MV_alu_le:
    nopa; nops; add.nc le, pc, #test_I96_LDA_ST_ALU_MV_alu_le_target; nopm
test_I96_LDA_ST_ALU_MV_alu_le_target:
    nop

// Fixup 58: I96_LDA_ST_ALU_VEC - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_ST_ALU_VEC_alu_ls>:
// CHECK:         nopa{{.*}}nops{{.*}}add.nc	ls, pc, #12{{.*}}nopv
test_I96_LDA_ST_ALU_VEC_alu_ls:
    nopa; nops; add.nc ls, pc, #test_I96_LDA_ST_ALU_VEC_alu_ls_target; nopv
test_I96_LDA_ST_ALU_VEC_alu_ls_target:
    nop

// Fixup 58: I96_LDA_ST_ALU_VEC - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_ST_ALU_VEC_alu_le>:
// CHECK:         nopa{{.*}}nops{{.*}}add.nc	le, pc, #12{{.*}}nopv
test_I96_LDA_ST_ALU_VEC_alu_le:
    nopa; nops; add.nc le, pc, #test_I96_LDA_ST_ALU_VEC_alu_le_target; nopv
test_I96_LDA_ST_ALU_VEC_alu_le_target:
    nop

// Fixup 59: I96_LDA_ALU_MV_VEC - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_ALU_MV_VEC_alu_ls>:
// CHECK:         nopa{{.*}}add.nc	ls, pc, #12{{.*}}nopm{{.*}}nopv
test_I96_LDA_ALU_MV_VEC_alu_ls:
    nopa; add.nc ls, pc, #test_I96_LDA_ALU_MV_VEC_alu_ls_target; nopm; nopv
test_I96_LDA_ALU_MV_VEC_alu_ls_target:
    nop

// Fixup 59: I96_LDA_ALU_MV_VEC - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_ALU_MV_VEC_alu_le>:
// CHECK:         nopa{{.*}}add.nc	le, pc, #12{{.*}}nopm{{.*}}nopv
test_I96_LDA_ALU_MV_VEC_alu_le:
    nopa; add.nc le, pc, #test_I96_LDA_ALU_MV_VEC_alu_le_target; nopm; nopv
test_I96_LDA_ALU_MV_VEC_alu_le_target:
    nop

// Fixup 59: I96_LDB_ALU_MV_VEC - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDB_ALU_MV_VEC_alu_ls>:
// CHECK:         nopb{{.*}}add.nc	ls, pc, #12{{.*}}nopm{{.*}}nopv
test_I96_LDB_ALU_MV_VEC_alu_ls:
    nopb; add.nc ls, pc, #test_I96_LDB_ALU_MV_VEC_alu_ls_target; nopm; nopv
test_I96_LDB_ALU_MV_VEC_alu_ls_target:
    nop

// Fixup 59: I96_LDB_ALU_MV_VEC - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDB_ALU_MV_VEC_alu_le>:
// CHECK:         nopb{{.*}}add.nc	le, pc, #12{{.*}}nopm{{.*}}nopv
test_I96_LDB_ALU_MV_VEC_alu_le:
    nopb; add.nc le, pc, #test_I96_LDB_ALU_MV_VEC_alu_le_target; nopm; nopv
test_I96_LDB_ALU_MV_VEC_alu_le_target:
    nop

// Fixup 60: I96_ST_LDB_ALU_MV - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_ST_LDB_ALU_MV_alu_ls>:
// CHECK:         nops{{.*}}nopb{{.*}}add.nc	ls, pc, #12{{.*}}nopm
test_I96_ST_LDB_ALU_MV_alu_ls:
    nops; nopb; add.nc ls, pc, #test_I96_ST_LDB_ALU_MV_alu_ls_target; nopm
test_I96_ST_LDB_ALU_MV_alu_ls_target:
    nop

// Fixup 60: I96_ST_LDB_ALU_MV - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_ST_LDB_ALU_MV_alu_le>:
// CHECK:         nops{{.*}}nopb{{.*}}add.nc	le, pc, #12{{.*}}nopm
test_I96_ST_LDB_ALU_MV_alu_le:
    nops; nopb; add.nc le, pc, #test_I96_ST_LDB_ALU_MV_alu_le_target; nopm
test_I96_ST_LDB_ALU_MV_alu_le_target:
    nop

// Fixup 60: I96_ST_LDB_ALU_VEC - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_ST_LDB_ALU_VEC_alu_ls>:
// CHECK:         nops{{.*}}nopb{{.*}}add.nc	ls, pc, #12{{.*}}nopv
test_I96_ST_LDB_ALU_VEC_alu_ls:
    nops; nopb; add.nc ls, pc, #test_I96_ST_LDB_ALU_VEC_alu_ls_target; nopv
test_I96_ST_LDB_ALU_VEC_alu_ls_target:
    nop

// Fixup 60: I96_ST_LDB_ALU_VEC - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_ST_LDB_ALU_VEC_alu_le>:
// CHECK:         nops{{.*}}nopb{{.*}}add.nc	le, pc, #12{{.*}}nopv
test_I96_ST_LDB_ALU_VEC_alu_le:
    nops; nopb; add.nc le, pc, #test_I96_ST_LDB_ALU_VEC_alu_le_target; nopv
test_I96_ST_LDB_ALU_VEC_alu_le_target:
    nop

// Fixup 60: I96_ST_ALU_MV_VEC - ALU ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_ST_ALU_MV_VEC_alu_ls>:
// CHECK:         nops{{.*}}add.nc	ls, pc, #12{{.*}}nopm{{.*}}nopv
test_I96_ST_ALU_MV_VEC_alu_ls:
    nops; add.nc ls, pc, #test_I96_ST_ALU_MV_VEC_alu_ls_target; nopm; nopv
test_I96_ST_ALU_MV_VEC_alu_ls_target:
    nop

// Fixup 60: I96_ST_ALU_MV_VEC - ALU ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_ST_ALU_MV_VEC_alu_le>:
// CHECK:         nops{{.*}}add.nc	le, pc, #12{{.*}}nopm{{.*}}nopv
test_I96_ST_ALU_MV_VEC_alu_le:
    nops; add.nc le, pc, #test_I96_ST_ALU_MV_VEC_alu_le_target; nopm; nopv
test_I96_ST_ALU_MV_VEC_alu_le_target:
    nop

// Fixup 61: I112_LDA_LDB_ALU_MV_VEC - ALU ZOL, 14-byte bundle, loop start
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_MV_VEC_alu_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #14{{.*}}nopm{{.*}}nopv
test_I112_LDA_LDB_ALU_MV_VEC_alu_ls:
    nopa; nopb; add.nc ls, pc, #test_I112_LDA_LDB_ALU_MV_VEC_alu_ls_target; nopm; nopv
test_I112_LDA_LDB_ALU_MV_VEC_alu_ls_target:
    nop

// Fixup 61: I112_LDA_LDB_ALU_MV_VEC - ALU ZOL, 14-byte bundle, loop end
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_MV_VEC_alu_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	le, pc, #14{{.*}}nopm{{.*}}nopv
test_I112_LDA_LDB_ALU_MV_VEC_alu_le:
    nopa; nopb; add.nc le, pc, #test_I112_LDA_LDB_ALU_MV_VEC_alu_le_target; nopm; nopv
test_I112_LDA_LDB_ALU_MV_VEC_alu_le_target:
    nop

// Fixup 61: I112_LDA_LDB_ALU_MV_ST - ALU ZOL, 14-byte bundle, loop start
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_MV_ST_alu_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #14{{.*}}nopm{{.*}}nops
test_I112_LDA_LDB_ALU_MV_ST_alu_ls:
    nopa; nopb; nops; add.nc ls, pc, #test_I112_LDA_LDB_ALU_MV_ST_alu_ls_target; nopm
test_I112_LDA_LDB_ALU_MV_ST_alu_ls_target:
    nop

// Fixup 61: I112_LDA_LDB_ALU_MV_ST - ALU ZOL, 14-byte bundle, loop end
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_MV_ST_alu_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	le, pc, #14{{.*}}nopm{{.*}}nops
test_I112_LDA_LDB_ALU_MV_ST_alu_le:
    nopa; nopb; nops; add.nc le, pc, #test_I112_LDA_LDB_ALU_MV_ST_alu_le_target; nopm
test_I112_LDA_LDB_ALU_MV_ST_alu_le_target:
    nop

// Fixup 61: I112_LDA_LDB_ALU_ST_VEC - ALU ZOL, 14-byte bundle, loop start
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_ST_VEC_alu_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #14{{.*}}nops{{.*}}nopv
test_I112_LDA_LDB_ALU_ST_VEC_alu_ls:
    nopa; nopb; nops; add.nc ls, pc, #test_I112_LDA_LDB_ALU_ST_VEC_alu_ls_target; nopv
test_I112_LDA_LDB_ALU_ST_VEC_alu_ls_target:
    nop

// Fixup 61: I112_LDA_LDB_ALU_ST_VEC - ALU ZOL, 14-byte bundle, loop end
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_ST_VEC_alu_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	le, pc, #14{{.*}}nops{{.*}}nopv
test_I112_LDA_LDB_ALU_ST_VEC_alu_le:
    nopa; nopb; nops; add.nc le, pc, #test_I112_LDA_LDB_ALU_ST_VEC_alu_le_target; nopv
test_I112_LDA_LDB_ALU_ST_VEC_alu_le_target:
    nop

// Fixup 61: I112_ST_LDB_ALU_MV_VEC - ALU ZOL, 14-byte bundle, loop start
// CHECK-LABEL: <test_I112_ST_LDB_ALU_MV_VEC_alu_ls>:
// CHECK:         nops{{.*}}nopb{{.*}}add.nc	ls, pc, #14{{.*}}nopm{{.*}}nopv
test_I112_ST_LDB_ALU_MV_VEC_alu_ls:
    nops; nopb; add.nc ls, pc, #test_I112_ST_LDB_ALU_MV_VEC_alu_ls_target; nopm; nopv
test_I112_ST_LDB_ALU_MV_VEC_alu_ls_target:
    nop

// Fixup 61: I112_ST_LDB_ALU_MV_VEC - ALU ZOL, 14-byte bundle, loop end
// CHECK-LABEL: <test_I112_ST_LDB_ALU_MV_VEC_alu_le>:
// CHECK:         nops{{.*}}nopb{{.*}}add.nc	le, pc, #14{{.*}}nopm{{.*}}nopv
test_I112_ST_LDB_ALU_MV_VEC_alu_le:
    nops; nopb; add.nc le, pc, #test_I112_ST_LDB_ALU_MV_VEC_alu_le_target; nopm; nopv
test_I112_ST_LDB_ALU_MV_VEC_alu_le_target:
    nop

// Fixup 62: I128_LDA_LDB_ST_ALU_MV_VEC - MV ZOL, 16-byte bundle, loop start
// CHECK-LABEL: <test_I128_LDA_LDB_ST_ALU_MV_VEC_mv_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}nops{{.*}}nopx{{.*}}addm.nc	ls, pc, #16{{.*}}nopv
test_I128_LDA_LDB_ST_ALU_MV_VEC_mv_ls:
    nopa; nopb; nops; nopx; addm.nc ls, pc, #test_I128_LDA_LDB_ST_ALU_MV_VEC_mv_ls_target; nopv
test_I128_LDA_LDB_ST_ALU_MV_VEC_mv_ls_target:
    nop

// Fixup 62: I128_LDA_LDB_ST_ALU_MV_VEC - MV ZOL, 16-byte bundle, loop end
// CHECK-LABEL: <test_I128_LDA_LDB_ST_ALU_MV_VEC_mv_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}nops{{.*}}nopx{{.*}}addm.nc	le, pc, #16{{.*}}nopv
test_I128_LDA_LDB_ST_ALU_MV_VEC_mv_le:
    nopa; nopb; nops; nopx; addm.nc le, pc, #test_I128_LDA_LDB_ST_ALU_MV_VEC_mv_le_target; nopv
test_I128_LDA_LDB_ST_ALU_MV_VEC_mv_le_target:
    nop

// Fixup 63: I32_MV - MV ZOL, 4-byte bundle, loop start
// CHECK-LABEL: <test_I32_MV_mv_ls>:
// CHECK:         addm.nc	ls, pc, #4
test_I32_MV_mv_ls:
    addm.nc ls, pc, #test_I32_MV_mv_ls_target
test_I32_MV_mv_ls_target:
    nop

// Fixup 63: I32_MV - MV ZOL, 4-byte bundle, loop end
// CHECK-LABEL: <test_I32_MV_mv_le>:
// CHECK:         addm.nc	le, pc, #4
test_I32_MV_mv_le:
    addm.nc le, pc, #test_I32_MV_mv_le_target
test_I32_MV_mv_le_target:
    nop

// Fixup 64: I48_LDA_MV - MV ZOL, 6-byte bundle, loop start
// CHECK-LABEL: <test_I48_LDA_MV_mv_ls>:
// CHECK:         nopa{{.*}}addm.nc	ls, pc, #6
test_I48_LDA_MV_mv_ls:
    nopa; addm.nc ls, pc, #test_I48_LDA_MV_mv_ls_target
test_I48_LDA_MV_mv_ls_target:
    nop

// Fixup 64: I48_LDA_MV - MV ZOL, 6-byte bundle, loop end
// CHECK-LABEL: <test_I48_LDA_MV_mv_le>:
// CHECK:         nopa{{.*}}addm.nc	le, pc, #6
test_I48_LDA_MV_mv_le:
    nopa; addm.nc le, pc, #test_I48_LDA_MV_mv_le_target
test_I48_LDA_MV_mv_le_target:
    nop

// Fixup 64: I48_LDB_MV - MV ZOL, 6-byte bundle, loop start
// CHECK-LABEL: <test_I48_LDB_MV_mv_ls>:
// CHECK:         nopb{{.*}}addm.nc	ls, pc, #6
test_I48_LDB_MV_mv_ls:
    nopb; addm.nc ls, pc, #test_I48_LDB_MV_mv_ls_target
test_I48_LDB_MV_mv_ls_target:
    nop

// Fixup 64: I48_LDB_MV - MV ZOL, 6-byte bundle, loop end
// CHECK-LABEL: <test_I48_LDB_MV_mv_le>:
// CHECK:         nopb{{.*}}addm.nc	le, pc, #6
test_I48_LDB_MV_mv_le:
    nopb; addm.nc le, pc, #test_I48_LDB_MV_mv_le_target
test_I48_LDB_MV_mv_le_target:
    nop

// Fixup 65: I48_ALU_MV - MV ZOL, 6-byte bundle, loop start
// CHECK-LABEL: <test_I48_ALU_MV_mv_ls>:
// CHECK:         nopx{{.*}}addm.nc	ls, pc, #6
test_I48_ALU_MV_mv_ls:
    nopx; addm.nc ls, pc, #test_I48_ALU_MV_mv_ls_target
test_I48_ALU_MV_mv_ls_target:
    nop

// Fixup 65: I48_ALU_MV - MV ZOL, 6-byte bundle, loop end
// CHECK-LABEL: <test_I48_ALU_MV_mv_le>:
// CHECK:         nopx{{.*}}addm.nc	le, pc, #6
test_I48_ALU_MV_mv_le:
    nopx; addm.nc le, pc, #test_I48_ALU_MV_mv_le_target
test_I48_ALU_MV_mv_le_target:
    nop

// Fixup 66: I64_ST_MV - MV ZOL, 8-byte bundle, loop start
// CHECK-LABEL: <test_I64_ST_MV_mv_ls>:
// CHECK:         nops{{.*}}addm.nc	ls, pc, #8
test_I64_ST_MV_mv_ls:
    nops; addm.nc ls, pc, #test_I64_ST_MV_mv_ls_target
test_I64_ST_MV_mv_ls_target:
    nop

// Fixup 66: I64_ST_MV - MV ZOL, 8-byte bundle, loop end
// CHECK-LABEL: <test_I64_ST_MV_mv_le>:
// CHECK:         nops{{.*}}addm.nc	le, pc, #8
test_I64_ST_MV_mv_le:
    nops; addm.nc le, pc, #test_I64_ST_MV_mv_le_target
test_I64_ST_MV_mv_le_target:
    nop

// Fixup 67: I64_MV_VEC - MV ZOL, 8-byte bundle, loop start
// CHECK-LABEL: <test_I64_MV_VEC_mv_ls>:
// CHECK:         addm.nc	ls, pc, #8{{.*}}nopv
test_I64_MV_VEC_mv_ls:
    addm.nc ls, pc, #test_I64_MV_VEC_mv_ls_target; nopv
test_I64_MV_VEC_mv_ls_target:
    nop

// Fixup 67: I64_MV_VEC - MV ZOL, 8-byte bundle, loop end
// CHECK-LABEL: <test_I64_MV_VEC_mv_le>:
// CHECK:         addm.nc	le, pc, #8{{.*}}nopv
test_I64_MV_VEC_mv_le:
    addm.nc le, pc, #test_I64_MV_VEC_mv_le_target; nopv
test_I64_MV_VEC_mv_le_target:
    nop

// Fixup 68: I80_LDA_LDB_MV - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDA_LDB_MV_mv_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}addm.nc	ls, pc, #10
test_I80_LDA_LDB_MV_mv_ls:
    nopa; nopb; addm.nc ls, pc, #test_I80_LDA_LDB_MV_mv_ls_target
test_I80_LDA_LDB_MV_mv_ls_target:
    nop

// Fixup 68: I80_LDA_LDB_MV - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDA_LDB_MV_mv_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}addm.nc	le, pc, #10
test_I80_LDA_LDB_MV_mv_le:
    nopa; nopb; addm.nc le, pc, #test_I80_LDA_LDB_MV_mv_le_target
test_I80_LDA_LDB_MV_mv_le_target:
    nop

// Fixup 68: I80_LDA_ST_MV - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDA_ST_MV_mv_ls>:
// CHECK:         nopa{{.*}}nops{{.*}}addm.nc	ls, pc, #10
test_I80_LDA_ST_MV_mv_ls:
    nopa; nops; addm.nc ls, pc, #test_I80_LDA_ST_MV_mv_ls_target
test_I80_LDA_ST_MV_mv_ls_target:
    nop

// Fixup 68: I80_LDA_ST_MV - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDA_ST_MV_mv_le>:
// CHECK:         nopa{{.*}}nops{{.*}}addm.nc	le, pc, #10
test_I80_LDA_ST_MV_mv_le:
    nopa; nops; addm.nc le, pc, #test_I80_LDA_ST_MV_mv_le_target
test_I80_LDA_ST_MV_mv_le_target:
    nop

// Fixup 68: I80_LDA_ALU_MV - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDA_ALU_MV_mv_ls>:
// CHECK:         nopa{{.*}}nopx{{.*}}addm.nc	ls, pc, #10
test_I80_LDA_ALU_MV_mv_ls:
    nopa; nopx; addm.nc ls, pc, #test_I80_LDA_ALU_MV_mv_ls_target
test_I80_LDA_ALU_MV_mv_ls_target:
    nop

// Fixup 68: I80_LDA_ALU_MV - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDA_ALU_MV_mv_le>:
// CHECK:         nopa{{.*}}nopx{{.*}}addm.nc	le, pc, #10
test_I80_LDA_ALU_MV_mv_le:
    nopa; nopx; addm.nc le, pc, #test_I80_LDA_ALU_MV_mv_le_target
test_I80_LDA_ALU_MV_mv_le_target:
    nop

// Fixup 68: I80_ST_LDB_MV - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_ST_LDB_MV_mv_ls>:
// CHECK:         nops{{.*}}nopb{{.*}}addm.nc	ls, pc, #10
test_I80_ST_LDB_MV_mv_ls:
    nops; nopb; addm.nc ls, pc, #test_I80_ST_LDB_MV_mv_ls_target
test_I80_ST_LDB_MV_mv_ls_target:
    nop

// Fixup 68: I80_ST_LDB_MV - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_ST_LDB_MV_mv_le>:
// CHECK:         nops{{.*}}nopb{{.*}}addm.nc	le, pc, #10
test_I80_ST_LDB_MV_mv_le:
    nops; nopb; addm.nc le, pc, #test_I80_ST_LDB_MV_mv_le_target
test_I80_ST_LDB_MV_mv_le_target:
    nop

// Fixup 68: I80_LDB_ALU_MV - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDB_ALU_MV_mv_ls>:
// CHECK:         nopb{{.*}}nopx{{.*}}addm.nc	ls, pc, #10
test_I80_LDB_ALU_MV_mv_ls:
    nopb; nopx; addm.nc ls, pc, #test_I80_LDB_ALU_MV_mv_ls_target
test_I80_LDB_ALU_MV_mv_ls_target:
    nop

// Fixup 68: I80_LDB_ALU_MV - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDB_ALU_MV_mv_le>:
// CHECK:         nopb{{.*}}nopx{{.*}}addm.nc	le, pc, #10
test_I80_LDB_ALU_MV_mv_le:
    nopb; nopx; addm.nc le, pc, #test_I80_LDB_ALU_MV_mv_le_target
test_I80_LDB_ALU_MV_mv_le_target:
    nop

// Fixup 68: I80_ST_ALU_MV - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_ST_ALU_MV_mv_ls>:
// CHECK:         nops{{.*}}nopx{{.*}}addm.nc	ls, pc, #10
test_I80_ST_ALU_MV_mv_ls:
    nops; nopx; addm.nc ls, pc, #test_I80_ST_ALU_MV_mv_ls_target
test_I80_ST_ALU_MV_mv_ls_target:
    nop

// Fixup 68: I80_ST_ALU_MV - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_ST_ALU_MV_mv_le>:
// CHECK:         nops{{.*}}nopx{{.*}}addm.nc	le, pc, #10
test_I80_ST_ALU_MV_mv_le:
    nops; nopx; addm.nc le, pc, #test_I80_ST_ALU_MV_mv_le_target
test_I80_ST_ALU_MV_mv_le_target:
    nop

// Fixup 69: I80_LDA_MV_VEC - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDA_MV_VEC_mv_ls>:
// CHECK:         nopa{{.*}}addm.nc	ls, pc, #10{{.*}}nopv
test_I80_LDA_MV_VEC_mv_ls:
    nopa; addm.nc ls, pc, #test_I80_LDA_MV_VEC_mv_ls_target; nopv
test_I80_LDA_MV_VEC_mv_ls_target:
    nop

// Fixup 69: I80_LDA_MV_VEC - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDA_MV_VEC_mv_le>:
// CHECK:         nopa{{.*}}addm.nc	le, pc, #10{{.*}}nopv
test_I80_LDA_MV_VEC_mv_le:
    nopa; addm.nc le, pc, #test_I80_LDA_MV_VEC_mv_le_target; nopv
test_I80_LDA_MV_VEC_mv_le_target:
    nop

// Fixup 69: I80_LDB_MV_VEC - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_LDB_MV_VEC_mv_ls>:
// CHECK:         nopb{{.*}}addm.nc	ls, pc, #10{{.*}}nopv
test_I80_LDB_MV_VEC_mv_ls:
    nopb; addm.nc ls, pc, #test_I80_LDB_MV_VEC_mv_ls_target; nopv
test_I80_LDB_MV_VEC_mv_ls_target:
    nop

// Fixup 69: I80_LDB_MV_VEC - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_LDB_MV_VEC_mv_le>:
// CHECK:         nopb{{.*}}addm.nc	le, pc, #10{{.*}}nopv
test_I80_LDB_MV_VEC_mv_le:
    nopb; addm.nc le, pc, #test_I80_LDB_MV_VEC_mv_le_target; nopv
test_I80_LDB_MV_VEC_mv_le_target:
    nop

// Fixup 69: I80_ST_MV_VEC - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_ST_MV_VEC_mv_ls>:
// CHECK:         nops{{.*}}addm.nc	ls, pc, #10{{.*}}nopv
test_I80_ST_MV_VEC_mv_ls:
    nops; addm.nc ls, pc, #test_I80_ST_MV_VEC_mv_ls_target; nopv
test_I80_ST_MV_VEC_mv_ls_target:
    nop

// Fixup 69: I80_ST_MV_VEC - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_ST_MV_VEC_mv_le>:
// CHECK:         nops{{.*}}addm.nc	le, pc, #10{{.*}}nopv
test_I80_ST_MV_VEC_mv_le:
    nops; addm.nc le, pc, #test_I80_ST_MV_VEC_mv_le_target; nopv
test_I80_ST_MV_VEC_mv_le_target:
    nop

// Fixup 69: I80_ALU_MV_VEC - MV ZOL, 10-byte bundle, loop start
// CHECK-LABEL: <test_I80_ALU_MV_VEC_mv_ls>:
// CHECK:         nopx{{.*}}addm.nc	ls, pc, #10{{.*}}nopv
test_I80_ALU_MV_VEC_mv_ls:
    nopx; addm.nc ls, pc, #test_I80_ALU_MV_VEC_mv_ls_target; nopv
test_I80_ALU_MV_VEC_mv_ls_target:
    nop

// Fixup 69: I80_ALU_MV_VEC - MV ZOL, 10-byte bundle, loop end
// CHECK-LABEL: <test_I80_ALU_MV_VEC_mv_le>:
// CHECK:         nopx{{.*}}addm.nc	le, pc, #10{{.*}}nopv
test_I80_ALU_MV_VEC_mv_le:
    nopx; addm.nc le, pc, #test_I80_ALU_MV_VEC_mv_le_target; nopv
test_I80_ALU_MV_VEC_mv_le_target:
    nop

// Fixup 70: I96_LDA_LDB_ST_MV - MV ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_LDB_ST_MV_mv_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}nops{{.*}}addm.nc	ls, pc, #12
test_I96_LDA_LDB_ST_MV_mv_ls:
    nopa; nopb; nops; addm.nc ls, pc, #test_I96_LDA_LDB_ST_MV_mv_ls_target
test_I96_LDA_LDB_ST_MV_mv_ls_target:
    nop

// Fixup 70: I96_LDA_LDB_ST_MV - MV ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_LDB_ST_MV_mv_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}nops{{.*}}addm.nc	le, pc, #12
test_I96_LDA_LDB_ST_MV_mv_le:
    nopa; nopb; nops; addm.nc le, pc, #test_I96_LDA_LDB_ST_MV_mv_le_target
test_I96_LDA_LDB_ST_MV_mv_le_target:
    nop

// Fixup 70: I96_LDA_LDB_ALU_MV - MV ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_MV_mv_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	ls, pc, #12
test_I96_LDA_LDB_ALU_MV_mv_ls:
    nopa; nopb; nopx; addm.nc ls, pc, #test_I96_LDA_LDB_ALU_MV_mv_ls_target
test_I96_LDA_LDB_ALU_MV_mv_ls_target:
    nop

// Fixup 70: I96_LDA_LDB_ALU_MV - MV ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_MV_mv_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	le, pc, #12
test_I96_LDA_LDB_ALU_MV_mv_le:
    nopa; nopb; nopx; addm.nc le, pc, #test_I96_LDA_LDB_ALU_MV_mv_le_target
test_I96_LDA_LDB_ALU_MV_mv_le_target:
    nop

// Fixup 70: I96_ST_LDB_ALU_MV - MV ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_ST_LDB_ALU_MV_mv_ls>:
// CHECK:         nops{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	ls, pc, #12
test_I96_ST_LDB_ALU_MV_mv_ls:
    nops; nopb; nopx; addm.nc ls, pc, #test_I96_ST_LDB_ALU_MV_mv_ls_target
test_I96_ST_LDB_ALU_MV_mv_ls_target:
    nop

// Fixup 70: I96_ST_LDB_ALU_MV - MV ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_ST_LDB_ALU_MV_mv_le>:
// CHECK:         nops{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	le, pc, #12
test_I96_ST_LDB_ALU_MV_mv_le:
    nops; nopb; nopx; addm.nc le, pc, #test_I96_ST_LDB_ALU_MV_mv_le_target
test_I96_ST_LDB_ALU_MV_mv_le_target:
    nop

// Fixup 71: I96_LDA_LDB_MV_VEC - MV ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_LDB_MV_VEC_mv_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}addm.nc	ls, pc, #12{{.*}}nopv
test_I96_LDA_LDB_MV_VEC_mv_ls:
    nopa; nopb; addm.nc ls, pc, #test_I96_LDA_LDB_MV_VEC_mv_ls_target; nopv
test_I96_LDA_LDB_MV_VEC_mv_ls_target:
    nop

// Fixup 71: I96_LDA_LDB_MV_VEC - MV ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_LDB_MV_VEC_mv_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}addm.nc	le, pc, #12{{.*}}nopv
test_I96_LDA_LDB_MV_VEC_mv_le:
    nopa; nopb; addm.nc le, pc, #test_I96_LDA_LDB_MV_VEC_mv_le_target; nopv
test_I96_LDA_LDB_MV_VEC_mv_le_target:
    nop

// Fixup 71: I96_LDA_ALU_MV_VEC - MV ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_ALU_MV_VEC_mv_ls>:
// CHECK:         nopa{{.*}}nopx{{.*}}addm.nc	ls, pc, #12{{.*}}nopv
test_I96_LDA_ALU_MV_VEC_mv_ls:
    nopa; nopx; addm.nc ls, pc, #test_I96_LDA_ALU_MV_VEC_mv_ls_target; nopv
test_I96_LDA_ALU_MV_VEC_mv_ls_target:
    nop

// Fixup 71: I96_LDA_ALU_MV_VEC - MV ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_ALU_MV_VEC_mv_le>:
// CHECK:         nopa{{.*}}nopx{{.*}}addm.nc	le, pc, #12{{.*}}nopv
test_I96_LDA_ALU_MV_VEC_mv_le:
    nopa; nopx; addm.nc le, pc, #test_I96_LDA_ALU_MV_VEC_mv_le_target; nopv
test_I96_LDA_ALU_MV_VEC_mv_le_target:
    nop

// Fixup 72: I96_LDA_ST_ALU_MV - MV ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDA_ST_ALU_MV_mv_ls>:
// CHECK:         nopa{{.*}}nops{{.*}}nopx{{.*}}addm.nc	ls, pc, #12
test_I96_LDA_ST_ALU_MV_mv_ls:
    nopa; nops; nopx; addm.nc ls, pc, #test_I96_LDA_ST_ALU_MV_mv_ls_target
test_I96_LDA_ST_ALU_MV_mv_ls_target:
    nop

// Fixup 72: I96_LDA_ST_ALU_MV - MV ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDA_ST_ALU_MV_mv_le>:
// CHECK:         nopa{{.*}}nops{{.*}}nopx{{.*}}addm.nc	le, pc, #12
test_I96_LDA_ST_ALU_MV_mv_le:
    nopa; nops; nopx; addm.nc le, pc, #test_I96_LDA_ST_ALU_MV_mv_le_target
test_I96_LDA_ST_ALU_MV_mv_le_target:
    nop

// Fixup 72: I96_ST_LDB_MV_VEC - MV ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_ST_LDB_MV_VEC_mv_ls>:
// CHECK:         nops{{.*}}nopb{{.*}}addm.nc	ls, pc, #12{{.*}}nopv
test_I96_ST_LDB_MV_VEC_mv_ls:
    nops; nopb; addm.nc ls, pc, #test_I96_ST_LDB_MV_VEC_mv_ls_target; nopv
test_I96_ST_LDB_MV_VEC_mv_ls_target:
    nop

// Fixup 72: I96_ST_LDB_MV_VEC - MV ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_ST_LDB_MV_VEC_mv_le>:
// CHECK:         nops{{.*}}nopb{{.*}}addm.nc	le, pc, #12{{.*}}nopv
test_I96_ST_LDB_MV_VEC_mv_le:
    nops; nopb; addm.nc le, pc, #test_I96_ST_LDB_MV_VEC_mv_le_target; nopv
test_I96_ST_LDB_MV_VEC_mv_le_target:
    nop

// Fixup 72: I96_LDB_ALU_MV_VEC - MV ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_LDB_ALU_MV_VEC_mv_ls>:
// CHECK:         nopb{{.*}}nopx{{.*}}addm.nc	ls, pc, #12{{.*}}nopv
test_I96_LDB_ALU_MV_VEC_mv_ls:
    nopb; nopx; addm.nc ls, pc, #test_I96_LDB_ALU_MV_VEC_mv_ls_target; nopv
test_I96_LDB_ALU_MV_VEC_mv_ls_target:
    nop

// Fixup 72: I96_LDB_ALU_MV_VEC - MV ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_LDB_ALU_MV_VEC_mv_le>:
// CHECK:         nopb{{.*}}nopx{{.*}}addm.nc	le, pc, #12{{.*}}nopv
test_I96_LDB_ALU_MV_VEC_mv_le:
    nopb; nopx; addm.nc le, pc, #test_I96_LDB_ALU_MV_VEC_mv_le_target; nopv
test_I96_LDB_ALU_MV_VEC_mv_le_target:
    nop

// Fixup 73: I96_ST_ALU_MV_VEC - MV ZOL, 12-byte bundle, loop start
// CHECK-LABEL: <test_I96_ST_ALU_MV_VEC_mv_ls>:
// CHECK:         nops{{.*}}nopx{{.*}}addm.nc	ls, pc, #12{{.*}}nopv
test_I96_ST_ALU_MV_VEC_mv_ls:
    nops; nopx; addm.nc ls, pc, #test_I96_ST_ALU_MV_VEC_mv_ls_target; nopv
test_I96_ST_ALU_MV_VEC_mv_ls_target:
    nop

// Fixup 73: I96_ST_ALU_MV_VEC - MV ZOL, 12-byte bundle, loop end
// CHECK-LABEL: <test_I96_ST_ALU_MV_VEC_mv_le>:
// CHECK:         nops{{.*}}nopx{{.*}}addm.nc	le, pc, #12{{.*}}nopv
test_I96_ST_ALU_MV_VEC_mv_le:
    nops; nopx; addm.nc le, pc, #test_I96_ST_ALU_MV_VEC_mv_le_target; nopv
test_I96_ST_ALU_MV_VEC_mv_le_target:
    nop

// Fixup 74: I112_LDA_ST_MV_VEC - MV ZOL, 14-byte bundle, loop start
// CHECK-LABEL: <test_I112_LDA_ST_MV_VEC_mv_ls>:
// CHECK:         nopa{{.*}}nops{{.*}}addm.nc	ls, pc, #14{{.*}}nopv
test_I112_LDA_ST_MV_VEC_mv_ls:
    nopa; nops; addm.nc ls, pc, #test_I112_LDA_ST_MV_VEC_mv_ls_target; nopv
test_I112_LDA_ST_MV_VEC_mv_ls_target:
    nop

// Fixup 74: I112_LDA_ST_MV_VEC - MV ZOL, 14-byte bundle, loop end
// CHECK-LABEL: <test_I112_LDA_ST_MV_VEC_mv_le>:
// CHECK:         nopa{{.*}}nops{{.*}}addm.nc	le, pc, #14{{.*}}nopv
test_I112_LDA_ST_MV_VEC_mv_le:
    nopa; nops; addm.nc le, pc, #test_I112_LDA_ST_MV_VEC_mv_le_target; nopv
test_I112_LDA_ST_MV_VEC_mv_le_target:
    nop

// Fixup 74: I112_LDA_LDB_ALU_MV_ST - MV ZOL, 14-byte bundle, loop start
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_MV_ST_mv_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	ls, pc, #14{{.*}}nops
test_I112_LDA_LDB_ALU_MV_ST_mv_ls:
    nopa; nopb; nops; nopx; addm.nc ls, pc, #test_I112_LDA_LDB_ALU_MV_ST_mv_ls_target
test_I112_LDA_LDB_ALU_MV_ST_mv_ls_target:
    nop

// Fixup 74: I112_LDA_LDB_ALU_MV_ST - MV ZOL, 14-byte bundle, loop end
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_MV_ST_mv_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	le, pc, #14{{.*}}nops
test_I112_LDA_LDB_ALU_MV_ST_mv_le:
    nopa; nopb; nops; nopx; addm.nc le, pc, #test_I112_LDA_LDB_ALU_MV_ST_mv_le_target
test_I112_LDA_LDB_ALU_MV_ST_mv_le_target:
    nop

// Fixup 74: I112_LDA_LDB_ALU_MV_VEC - MV ZOL, 14-byte bundle, loop start
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_MV_VEC_mv_ls>:
// CHECK:         nopa{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	ls, pc, #14{{.*}}nopv
test_I112_LDA_LDB_ALU_MV_VEC_mv_ls:
    nopa; nopb; nopx; addm.nc ls, pc, #test_I112_LDA_LDB_ALU_MV_VEC_mv_ls_target; nopv
test_I112_LDA_LDB_ALU_MV_VEC_mv_ls_target:
    nop

// Fixup 74: I112_LDA_LDB_ALU_MV_VEC - MV ZOL, 14-byte bundle, loop end
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_MV_VEC_mv_le>:
// CHECK:         nopa{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	le, pc, #14{{.*}}nopv
test_I112_LDA_LDB_ALU_MV_VEC_mv_le:
    nopa; nopb; nopx; addm.nc le, pc, #test_I112_LDA_LDB_ALU_MV_VEC_mv_le_target; nopv
test_I112_LDA_LDB_ALU_MV_VEC_mv_le_target:
    nop

// Fixup 74: I112_ST_LDB_ALU_MV_VEC - MV ZOL, 14-byte bundle, loop start
// CHECK-LABEL: <test_I112_ST_LDB_ALU_MV_VEC_mv_ls>:
// CHECK:         nops{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	ls, pc, #14{{.*}}nopv
test_I112_ST_LDB_ALU_MV_VEC_mv_ls:
    nops; nopb; nopx; addm.nc ls, pc, #test_I112_ST_LDB_ALU_MV_VEC_mv_ls_target; nopv
test_I112_ST_LDB_ALU_MV_VEC_mv_ls_target:
    nop

// Fixup 74: I112_ST_LDB_ALU_MV_VEC - MV ZOL, 14-byte bundle, loop end
// CHECK-LABEL: <test_I112_ST_LDB_ALU_MV_VEC_mv_le>:
// CHECK:         nops{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	le, pc, #14{{.*}}nopv
test_I112_ST_LDB_ALU_MV_VEC_mv_le:
    nops; nopb; nopx; addm.nc le, pc, #test_I112_ST_LDB_ALU_MV_VEC_mv_le_target; nopv
test_I112_ST_LDB_ALU_MV_VEC_mv_le_target:
    nop
