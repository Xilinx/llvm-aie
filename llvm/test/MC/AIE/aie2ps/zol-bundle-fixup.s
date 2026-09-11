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

// Test ZOL (Zero-Overhead Loop) fixups in all VLIW bundle configurations.
// ALU slot uses 5+6 bit split encoding (add.nc), MV slot uses 4+7 bit split
// encoding (addm.nc). Each test is named after the composite format it uses.

//===----------------------------------------------------------------------===//
// 4-BYTE STANDALONE FORMATS
//===----------------------------------------------------------------------===//

// Fixup 156: I32_ALU - ALU ZOL
// CHECK-LABEL: <test_I32_ALU_alu_zol>:
// CHECK:         add.nc	ls, pc, #4
test_I32_ALU_alu_zol:
    add.nc ls, pc, #I32_ALU_alu_zol_target
I32_ALU_alu_zol_target:
    nop

// Fixup 146: I32_MV - MV ZOL
// CHECK-LABEL: <test_I32_MV_mv_zol>:
// CHECK:         addm.nc	ls, pc, #4
test_I32_MV_mv_zol:
    addm.nc ls, pc, #I32_MV_mv_zol_target
I32_MV_mv_zol_target:
    nop

//===----------------------------------------------------------------------===//
// 6-BYTE BUNDLE FORMATS
//===----------------------------------------------------------------------===//

// Fixup 159: I48_LDA_ALU - ALU ZOL
// CHECK-LABEL: <test_I48_LDA_ALU_alu_zol>:
// CHECK:         nopa{{.*}}add.nc	ls, pc, #6
test_I48_LDA_ALU_alu_zol:
    nopa; add.nc ls, pc, #I48_LDA_ALU_alu_zol_target
I48_LDA_ALU_alu_zol_target:
    nop

// Fixup 159: I48_LDB_ALU - ALU ZOL (same field position as I48_LDA_ALU)
// CHECK-LABEL: <test_I48_LDB_ALU_alu_zol>:
// CHECK:         nopb{{.*}}add.nc	ls, pc, #6
test_I48_LDB_ALU_alu_zol:
    nopb; add.nc ls, pc, #I48_LDB_ALU_alu_zol_target
I48_LDB_ALU_alu_zol_target:
    nop

// Fixup 159: I48_ST_ALU - ALU ZOL (same field position as I48_LDA_ALU)
// CHECK-LABEL: <test_I48_ST_ALU_alu_zol>:
// CHECK:         nops{{.*}}add.nc	ls, pc, #6
test_I48_ST_ALU_alu_zol:
    nops; add.nc ls, pc, #I48_ST_ALU_alu_zol_target
I48_ST_ALU_alu_zol_target:
    nop

// Fixup 170: I48_ALU_MV - ALU ZOL (ALU slot at offset 0 in 48-bit bundle, MV is nop)
// CHECK-LABEL: <test_I48_ALU_MV_alu_zol>:
// CHECK:         add.nc	ls, pc, #6{{.*}}nopm
test_I48_ALU_MV_alu_zol:
    add.nc ls, pc, #I48_ALU_MV_alu_zol_target; nopm
I48_ALU_MV_alu_zol_target:
    nop

// Fixup 163: I48_LDA_MV - MV ZOL (fields at (35,4)+(28,7))
// CHECK-LABEL: <test_I48_LDA_MV_mv_zol>:
// CHECK:         nopa{{.*}}addm.nc	ls, pc, #6
test_I48_LDA_MV_mv_zol:
    nopa; addm.nc ls, pc, #I48_LDA_MV_mv_zol_target
I48_LDA_MV_mv_zol_target:
    nop

// Fixup 148: I48_LDB_MV - MV ZOL
// CHECK-LABEL: <test_I48_LDB_MV_mv_zol>:
// CHECK:         nopb{{.*}}addm.nc	ls, pc, #6
test_I48_LDB_MV_mv_zol:
    nopb; addm.nc ls, pc, #I48_LDB_MV_mv_zol_target
I48_LDB_MV_mv_zol_target:
    nop

// Fixup 145: I48_ALU_MV - MV ZOL (fields at (34,4)+(27,7), ALU slot is nop)
// CHECK-LABEL: <test_I48_ALU_MV_mv_zol>:
// CHECK:         nopx{{.*}}addm.nc	ls, pc, #6
test_I48_ALU_MV_mv_zol:
    nopx; addm.nc ls, pc, #I48_ALU_MV_mv_zol_target
I48_ALU_MV_mv_zol_target:
    nop

//===----------------------------------------------------------------------===//
// 8-BYTE BUNDLE FORMATS
//===----------------------------------------------------------------------===//

// Fixup 157: I64_ALU_VEC - ALU ZOL
// CHECK-LABEL: <test_I64_ALU_VEC_alu_zol>:
// CHECK:         add.nc	ls, pc, #8{{.*}}nopv
test_I64_ALU_VEC_alu_zol:
    add.nc ls, pc, #I64_ALU_VEC_alu_zol_target; nopv
I64_ALU_VEC_alu_zol_target:
    nop

// Fixup 161: I64_LDA_LDB_ALU - ALU ZOL
// CHECK-LABEL: <test_I64_LDA_LDB_ALU_alu_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #8
test_I64_LDA_LDB_ALU_alu_zol:
    nopa; nopb; add.nc ls, pc, #I64_LDA_LDB_ALU_alu_zol_target
I64_LDA_LDB_ALU_alu_zol_target:
    nop

// Fixup 161: I64_ST_LDB_ALU - ALU ZOL (same field position as I64_LDA_LDB_ALU)
// CHECK-LABEL: <test_I64_ST_LDB_ALU_alu_zol>:
// CHECK:         nops{{.*}}nopb{{.*}}add.nc	ls, pc, #8
test_I64_ST_LDB_ALU_alu_zol:
    nops; nopb; add.nc ls, pc, #I64_ST_LDB_ALU_alu_zol_target
I64_ST_LDB_ALU_alu_zol_target:
    nop

// Fixup 147: I64_MV_VEC - MV ZOL
// CHECK-LABEL: <test_I64_MV_VEC_mv_zol>:
// CHECK:         addm.nc	ls, pc, #8{{.*}}nopv
test_I64_MV_VEC_mv_zol:
    addm.nc ls, pc, #I64_MV_VEC_mv_zol_target; nopv
I64_MV_VEC_mv_zol_target:
    nop

// Fixup 164: I64_ST_MV - MV ZOL (fields at (45,4)+(38,7))
// CHECK-LABEL: <test_I64_ST_MV_mv_zol>:
// CHECK:         nops{{.*}}addm.nc	ls, pc, #8
test_I64_ST_MV_mv_zol:
    nops; addm.nc ls, pc, #I64_ST_MV_mv_zol_target
I64_ST_MV_mv_zol_target:
    nop

//===----------------------------------------------------------------------===//
// 10-BYTE BUNDLE FORMATS
//===----------------------------------------------------------------------===//

// Fixup 150: I80_LDA_ST_ALU - ALU ZOL
// CHECK-LABEL: <test_I80_LDA_ST_ALU_alu_zol>:
// CHECK:         nopa{{.*}}nops{{.*}}add.nc	ls, pc, #10
test_I80_LDA_ST_ALU_alu_zol:
    nopa; nops; add.nc ls, pc, #I80_LDA_ST_ALU_alu_zol_target
I80_LDA_ST_ALU_alu_zol_target:
    nop

// Fixup 171: I80_LDA_ALU_MV - ALU ZOL (MV slot is nop)
// CHECK-LABEL: <test_I80_LDA_ALU_MV_alu_zol>:
// CHECK:         nopa{{.*}}add.nc	ls, pc, #10{{.*}}nopm
test_I80_LDA_ALU_MV_alu_zol:
    nopa; add.nc ls, pc, #I80_LDA_ALU_MV_alu_zol_target; nopm
I80_LDA_ALU_MV_alu_zol_target:
    nop

// Fixup 171: I80_LDA_ALU_VEC - ALU ZOL (same field position as I80_LDA_ALU_MV)
// CHECK-LABEL: <test_I80_LDA_ALU_VEC_alu_zol>:
// CHECK:         nopa{{.*}}add.nc	ls, pc, #10{{.*}}nopv
test_I80_LDA_ALU_VEC_alu_zol:
    nopa; add.nc ls, pc, #I80_LDA_ALU_VEC_alu_zol_target; nopv
I80_LDA_ALU_VEC_alu_zol_target:
    nop

// Fixup 171: I80_LDB_ALU_MV - ALU ZOL (MV slot is nop)
// CHECK-LABEL: <test_I80_LDB_ALU_MV_alu_zol>:
// CHECK:         nopb{{.*}}add.nc	ls, pc, #10{{.*}}nopm
test_I80_LDB_ALU_MV_alu_zol:
    nopb; add.nc ls, pc, #I80_LDB_ALU_MV_alu_zol_target; nopm
I80_LDB_ALU_MV_alu_zol_target:
    nop

// Fixup 171: I80_LDB_ALU_VEC - ALU ZOL (same field position as I80_LDB_ALU_MV)
// CHECK-LABEL: <test_I80_LDB_ALU_VEC_alu_zol>:
// CHECK:         nopb{{.*}}add.nc	ls, pc, #10{{.*}}nopv
test_I80_LDB_ALU_VEC_alu_zol:
    nopb; add.nc ls, pc, #I80_LDB_ALU_VEC_alu_zol_target; nopv
I80_LDB_ALU_VEC_alu_zol_target:
    nop

// Fixup 171: I80_ST_ALU_MV - ALU ZOL (MV slot is nop)
// CHECK-LABEL: <test_I80_ST_ALU_MV_alu_zol>:
// CHECK:         nops{{.*}}add.nc	ls, pc, #10{{.*}}nopm
test_I80_ST_ALU_MV_alu_zol:
    nops; add.nc ls, pc, #I80_ST_ALU_MV_alu_zol_target; nopm
I80_ST_ALU_MV_alu_zol_target:
    nop

// Fixup 171: I80_ST_ALU_VEC - ALU ZOL (same field position as I80_ST_ALU_MV)
// CHECK-LABEL: <test_I80_ST_ALU_VEC_alu_zol>:
// CHECK:         nops{{.*}}add.nc	ls, pc, #10{{.*}}nopv
test_I80_ST_ALU_VEC_alu_zol:
    nops; add.nc ls, pc, #I80_ST_ALU_VEC_alu_zol_target; nopv
I80_ST_ALU_VEC_alu_zol_target:
    nop

// Fixup 172: I80_ALU_MV_VEC - ALU ZOL (ALU at offset 1 in 80-bit bundle, MV and VEC are nops)
// CHECK-LABEL: <test_I80_ALU_MV_VEC_alu_zol>:
// CHECK:         add.nc	ls, pc, #10{{.*}}nopm{{.*}}nopv
test_I80_ALU_MV_VEC_alu_zol:
    add.nc ls, pc, #I80_ALU_MV_VEC_alu_zol_target; nopm; nopv
I80_ALU_MV_VEC_alu_zol_target:
    nop

// Fixup 168: I80_LDA_LDB_MV - MV ZOL (fields at (60,4)+(53,7), no VEC slot)
// CHECK-LABEL: <test_I80_LDA_LDB_MV_mv_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}addm.nc	ls, pc, #10
test_I80_LDA_LDB_MV_mv_zol:
    nopa; nopb; addm.nc ls, pc, #I80_LDA_LDB_MV_mv_zol_target
I80_LDA_LDB_MV_mv_zol_target:
    nop

// Fixup 167: I80_LDB_MV_VEC - MV ZOL (fields at (35,4)+(28,7), has VEC slot)
// CHECK-LABEL: <test_I80_LDB_MV_VEC_mv_zol>:
// CHECK:         nopb{{.*}}addm.nc	ls, pc, #10{{.*}}nopv
test_I80_LDB_MV_VEC_mv_zol:
    nopb; addm.nc ls, pc, #I80_LDB_MV_VEC_mv_zol_target; nopv
I80_LDB_MV_VEC_mv_zol_target:
    nop

// Fixup 168: I80_LDA_ST_MV - MV ZOL (fields at (60,4)+(53,7), same as I80_LDA_LDB_MV)
// CHECK-LABEL: <test_I80_LDA_ST_MV_mv_zol>:
// CHECK:         nopa{{.*}}nops{{.*}}addm.nc	ls, pc, #10
test_I80_LDA_ST_MV_mv_zol:
    nopa; nops; addm.nc ls, pc, #I80_LDA_ST_MV_mv_zol_target
I80_LDA_ST_MV_mv_zol_target:
    nop

// Fixup 168: I80_ST_LDB_MV - MV ZOL (fields at (60,4)+(53,7), same as I80_LDA_LDB_MV)
// CHECK-LABEL: <test_I80_ST_LDB_MV_mv_zol>:
// CHECK:         nops{{.*}}nopb{{.*}}addm.nc	ls, pc, #10
test_I80_ST_LDB_MV_mv_zol:
    nops; nopb; addm.nc ls, pc, #I80_ST_LDB_MV_mv_zol_target
I80_ST_LDB_MV_mv_zol_target:
    nop

// Fixup 168: I80_LDA_ALU_MV - MV ZOL (ALU slot is nop, fields at (60,4)+(53,7))
// CHECK-LABEL: <test_I80_LDA_ALU_MV_mv_zol>:
// CHECK:         nopa{{.*}}nopx{{.*}}addm.nc	ls, pc, #10
test_I80_LDA_ALU_MV_mv_zol:
    nopa; nopx; addm.nc ls, pc, #I80_LDA_ALU_MV_mv_zol_target
I80_LDA_ALU_MV_mv_zol_target:
    nop

// Fixup 168: I80_LDB_ALU_MV - MV ZOL (ALU slot is nop, fields at (60,4)+(53,7))
// CHECK-LABEL: <test_I80_LDB_ALU_MV_mv_zol>:
// CHECK:         nopb{{.*}}nopx{{.*}}addm.nc	ls, pc, #10
test_I80_LDB_ALU_MV_mv_zol:
    nopb; nopx; addm.nc ls, pc, #I80_LDB_ALU_MV_mv_zol_target
I80_LDB_ALU_MV_mv_zol_target:
    nop

// Fixup 168: I80_ST_ALU_MV - MV ZOL (ALU slot is nop, fields at (60,4)+(53,7))
// CHECK-LABEL: <test_I80_ST_ALU_MV_mv_zol>:
// CHECK:         nops{{.*}}nopx{{.*}}addm.nc	ls, pc, #10
test_I80_ST_ALU_MV_mv_zol:
    nops; nopx; addm.nc ls, pc, #I80_ST_ALU_MV_mv_zol_target
I80_ST_ALU_MV_mv_zol_target:
    nop

// Fixup 167: I80_LDA_MV_VEC - MV ZOL (fields at (35,4)+(28,7), same as I80_LDB_MV_VEC)
// CHECK-LABEL: <test_I80_LDA_MV_VEC_mv_zol>:
// CHECK:         nopa{{.*}}addm.nc	ls, pc, #10{{.*}}nopv
test_I80_LDA_MV_VEC_mv_zol:
    nopa; addm.nc ls, pc, #I80_LDA_MV_VEC_mv_zol_target; nopv
I80_LDA_MV_VEC_mv_zol_target:
    nop

// Fixup 167: I80_ST_MV_VEC - MV ZOL (fields at (35,4)+(28,7), same as I80_LDB_MV_VEC)
// CHECK-LABEL: <test_I80_ST_MV_VEC_mv_zol>:
// CHECK:         nops{{.*}}addm.nc	ls, pc, #10{{.*}}nopv
test_I80_ST_MV_VEC_mv_zol:
    nops; addm.nc ls, pc, #I80_ST_MV_VEC_mv_zol_target; nopv
I80_ST_MV_VEC_mv_zol_target:
    nop

// Fixup 167: I80_ALU_MV_VEC - MV ZOL (ALU slot is nop, fields at (35,4)+(28,7))
// CHECK-LABEL: <test_I80_ALU_MV_VEC_mv_zol>:
// CHECK:         nopx{{.*}}addm.nc	ls, pc, #10{{.*}}nopv
test_I80_ALU_MV_VEC_mv_zol:
    nopx; addm.nc ls, pc, #I80_ALU_MV_VEC_mv_zol_target; nopv
I80_ALU_MV_VEC_mv_zol_target:
    nop

//===----------------------------------------------------------------------===//
// 12-BYTE BUNDLE FORMATS
//===----------------------------------------------------------------------===//

// Fixup 155: I96_LDA_LDB_ALU_ST - ALU ZOL
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_ST_alu_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #12{{.*}}nops
test_I96_LDA_LDB_ALU_ST_alu_zol:
    nopa; nopb; nops; add.nc ls, pc, #I96_LDA_LDB_ALU_ST_alu_zol_target
I96_LDA_LDB_ALU_ST_alu_zol_target:
    nop

// Fixup 158: I96_LDB_ALU_MV_VEC - ALU ZOL
// CHECK-LABEL: <test_I96_LDB_ALU_MV_VEC_alu_zol>:
// CHECK:         nopb{{.*}}add.nc	ls, pc, #12{{.*}}nopm{{.*}}nopv
test_I96_LDB_ALU_MV_VEC_alu_zol:
    nopb; add.nc ls, pc, #I96_LDB_ALU_MV_VEC_alu_zol_target; nopm; nopv
I96_LDB_ALU_MV_VEC_alu_zol_target:
    nop

// Fixup 160: I96_LDA_ALU_MV_VEC - ALU ZOL
// CHECK-LABEL: <test_I96_LDA_ALU_MV_VEC_alu_zol>:
// CHECK:         nopa{{.*}}add.nc	ls, pc, #12{{.*}}nopm{{.*}}nopv
test_I96_LDA_ALU_MV_VEC_alu_zol:
    nopa; add.nc ls, pc, #I96_LDA_ALU_MV_VEC_alu_zol_target; nopm; nopv
I96_LDA_ALU_MV_VEC_alu_zol_target:
    nop

// Fixup 155: I96_LDA_LDB_ALU_MV - ALU ZOL (MV slot is nop)
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_MV_alu_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #12{{.*}}nopm
test_I96_LDA_LDB_ALU_MV_alu_zol:
    nopa; nopb; add.nc ls, pc, #I96_LDA_LDB_ALU_MV_alu_zol_target; nopm
I96_LDA_LDB_ALU_MV_alu_zol_target:
    nop

// Fixup 155: I96_LDA_LDB_ALU_VEC - ALU ZOL (same field position as I96_LDA_LDB_ALU_MV)
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_VEC_alu_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #12{{.*}}nopv
test_I96_LDA_LDB_ALU_VEC_alu_zol:
    nopa; nopb; add.nc ls, pc, #I96_LDA_LDB_ALU_VEC_alu_zol_target; nopv
I96_LDA_LDB_ALU_VEC_alu_zol_target:
    nop

// Fixup 155: I96_LDA_ST_ALU_MV - ALU ZOL (MV slot is nop)
// CHECK-LABEL: <test_I96_LDA_ST_ALU_MV_alu_zol>:
// CHECK:         nopa{{.*}}nops{{.*}}add.nc	ls, pc, #12{{.*}}nopm
test_I96_LDA_ST_ALU_MV_alu_zol:
    nopa; nops; add.nc ls, pc, #I96_LDA_ST_ALU_MV_alu_zol_target; nopm
I96_LDA_ST_ALU_MV_alu_zol_target:
    nop

// Fixup 155: I96_LDA_ST_ALU_VEC - ALU ZOL (same field position as I96_LDA_ST_ALU_MV)
// CHECK-LABEL: <test_I96_LDA_ST_ALU_VEC_alu_zol>:
// CHECK:         nopa{{.*}}nops{{.*}}add.nc	ls, pc, #12{{.*}}nopv
test_I96_LDA_ST_ALU_VEC_alu_zol:
    nopa; nops; add.nc ls, pc, #I96_LDA_ST_ALU_VEC_alu_zol_target; nopv
I96_LDA_ST_ALU_VEC_alu_zol_target:
    nop

// Fixup 169: I96_LDA_LDB_MV_VEC - MV ZOL (fields at (52,4)+(45,7))
// CHECK-LABEL: <test_I96_LDA_LDB_MV_VEC_mv_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}addm.nc	ls, pc, #12{{.*}}nopv
test_I96_LDA_LDB_MV_VEC_mv_zol:
    nopa; nopb; addm.nc ls, pc, #I96_LDA_LDB_MV_VEC_mv_zol_target; nopv
I96_LDA_LDB_MV_VEC_mv_zol_target:
    nop

// Fixup 153: I96_LDA_ST_ALU_MV - MV ZOL (ALU slot is nop)
// CHECK-LABEL: <test_I96_LDA_ST_ALU_MV_mv_zol>:
// CHECK:         nopa{{.*}}nops{{.*}}nopx{{.*}}addm.nc	ls, pc, #12
test_I96_LDA_ST_ALU_MV_mv_zol:
    nopa; nops; nopx; addm.nc ls, pc, #I96_LDA_ST_ALU_MV_mv_zol_target
I96_LDA_ST_ALU_MV_mv_zol_target:
    nop

// Fixup 144: I96_LDA_LDB_ST_MV - MV ZOL (fields at (77,4)+(70,7))
// CHECK-LABEL: <test_I96_LDA_LDB_ST_MV_mv_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}nops{{.*}}addm.nc	ls, pc, #12
test_I96_LDA_LDB_ST_MV_mv_zol:
    nopa; nopb; nops; addm.nc ls, pc, #I96_LDA_LDB_ST_MV_mv_zol_target
I96_LDA_LDB_ST_MV_mv_zol_target:
    nop

// Fixup 144: I96_LDA_LDB_ALU_MV - MV ZOL (ALU slot is nop, fields at (77,4)+(70,7))
// CHECK-LABEL: <test_I96_LDA_LDB_ALU_MV_mv_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}nopx{{.*}}addm.nc	ls, pc, #12
test_I96_LDA_LDB_ALU_MV_mv_zol:
    nopa; nopb; nopx; addm.nc ls, pc, #I96_LDA_LDB_ALU_MV_mv_zol_target
I96_LDA_LDB_ALU_MV_mv_zol_target:
    nop

//===----------------------------------------------------------------------===//
// 14-BYTE BUNDLE FORMATS
//===----------------------------------------------------------------------===//

// Fixup 162: I112_LDA_LDB_ALU_MV_VEC - ALU ZOL
// CHECK-LABEL: <test_I112_LDA_LDB_ALU_MV_VEC_alu_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}add.nc	ls, pc, #14{{.*}}nopm{{.*}}nopv
test_I112_LDA_LDB_ALU_MV_VEC_alu_zol:
    nopa; nopb; add.nc ls, pc, #I112_LDA_LDB_ALU_MV_VEC_alu_zol_target; nopm; nopv
I112_LDA_LDB_ALU_MV_VEC_alu_zol_target:
    nop

//===----------------------------------------------------------------------===//
// 16-BYTE BUNDLE FORMATS (Full VLIW)
//===----------------------------------------------------------------------===//

// Fixup 154: LNG - ALU ZOL (full 16-byte bundle)
// CHECK-LABEL: <test_LNG_alu_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}nops{{.*}}add.nc	ls, pc, #16{{.*}}nopm{{.*}}nopv
test_LNG_alu_zol:
    nopa; nopb; nops; add.nc ls, pc, #LNG_alu_zol_target; nopm; nopv
LNG_alu_zol_target:
    nop

// Fixup 142: LNG - MV ZOL (full 16-byte bundle)
// CHECK-LABEL: <test_LNG_mv_zol>:
// CHECK:         nopa{{.*}}nopb{{.*}}nops{{.*}}nopx{{.*}}addm.nc	ls, pc, #16{{.*}}nopv
test_LNG_mv_zol:
    nopa; nopb; nops; nopx; addm.nc ls, pc, #LNG_mv_zol_target; nopv
LNG_mv_zol_target:
    nop
