//===- PacketFormat.td -------------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// Modifications (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// RUN: llvm-tblgen --gen-instr-format -I %p -I %p/../ -I %p/../../../../include %s 2>&1 | FileCheck %s
include "llvm/Target/Target.td"
include "llvm/Target/CodeGenFormat.td"

let Namespace = "TEST" in {
def instr16_slot : InstSlot<"xxxx", 14>;
def lda_slot     : InstSlot<"aaaaaaa", 18>;
def ldb_slot     : InstSlot<"bbbbb", 17>;
def alu_slot     : InstSlot<"cccc", 19>;
def lng_slot     : InstSlot<"ddddd", 28>;
def veca_slot    : InstSlot<"sdasdadd", 37>;
}

include "Inputs/CommonFormatTesting.td"

def I128_LDA_LDB_NOP_ALU_LNG_NOP_VEC_ALL
    : TEST__instr128__V_formats__V_move__ldb_alu_st_mv_all_vec_all
        <(outs),
         (ins lda_slot:$lda,
              ldb_slot:$ldb,
              alu_slot:$alu,
              lng_slot:$lng,
              veca_slot:$vec_all),
         [],
         "i128_lda_ldb_nop_alu_lng_nop_vec_all",
         "$lda, $ldb, $alu, $lng, $vec_all">
{
   let hasSideEffects = 0;
   let mayLoad = 0;
   let mayStore = 0;

   bits<19> alu;
   bits<28> lng;
   let alu_st = {0b0, alu, 0};
   let mv_all = {0b000, lng};
}

// CHECK:        TEST::I128_LDA_LDB_NOP_ALU_LNG_NOP_VEC_ALL - Index : 0
// CHECK:        "lda"
// CHECK-SAME:        { 0, 17 }
// CHECK-SAME:        TESTSlotKind(TESTSlotKind::TEST_SLOT_AAAAAAA)
// CHECK:        "ldb"
// CHECK-SAME:        { 18, 34 }
// CHECK-SAME:        TESTSlotKind(TESTSlotKind::TEST_SLOT_BBBBB)
// CHECK:        "alu"
// CHECK-SAME:        { 36, 54 }
// CEHCK-SAME:        TESTSlotKind(TESTSlotKind::TEST_SLOT_CCCC)
// CHECK:        "lng"
// CHECK-SAME:        { 60, 87 }
// CHECK-SAME:        TESTSlotKind(TESTSlotKind::TEST_SLOT_DDDDD)
// CHECK:        "vec_all"
// CHECK-SAME:        { 89, 125 }
// CHECK-SAME:        TESTSlotKind(TESTSlotKind::TEST_SLOT_SDASDADD)

def I16 : TEST_instr16<(ins instr16_slot:$instr16), "i16", "$instr16">
{
  bits<14> instr16;
  let Inst = {instr16, 0b01};
}

// CHECK:        TEST::I16 - Index : 1
// CHECK:        "instr16"
// CHECK:        { 0, 13 }
// CHECK:        TESTSlotKind(TESTSlotKind::TEST_SLOT_XXXX)

def I64_LDA_LDB_ALU
    : TEST_i64_lda_fmts_i64_ldb
        <(ins lda_slot:$lda,
              ldb_slot:$ldb,
              alu_slot:$alu),
    "i64_lda_ldb_alu", "$lda, $ldb, $alu">
{
  bits<17> ldb;
  bits<19> alu;
  field bits<21> alu_st;
  field bits<23> alu_st_mv;
  let alu_st = {0b0, alu, 0b0};
  let alu_st_mv = {alu_st, 0b00};
  let i64_ldb = {ldb, alu_st_mv};
}

// CHECK:        TEST::I64_LDA_LDB_ALU - Index : 2
// CHECK:        "lda"
// CHECK:        { 0, 17 }
// CHECK:        TESTSlotKind(TESTSlotKind::TEST_SLOT_AAAAAAA)
// CHECK:        "ldb"
// CHECK:        { 18, 34 }
// CHECK:        TESTSlotKind(TESTSlotKind::TEST_SLOT_BBBBB)
// CHECK:        "alu"
// CHECK:        { 36, 54 }
// CEHCK:        TESTSlotKind(TESTSlotKind::TEST_SLOT_CCCC)


// CHECK:        TEST::I128_LDA_LDB_NOP_ALU_LNG_NOP_VEC_ALL - Index : 0
// CHECK:        true /* isComposite */
// CHECK:        TEST::I16 - Index : 1
// CHECK:        true /* isComposite */
// CHECK:        TEST::I64_LDA_LDB_ALU - Index : 2
// CHECK:        true /* isComposite */

// Check the auto-generated table:
// CHECK:        #ifdef GET_FORMATS_PACKETS_TABLE
// CHECK-LABEL: FormatSlotData
// CHECK: I16 : 0
// I16 : 0
// CHECK: TESTSlotKind::TEST_SLOT_XXXX,
// CHECK: I64_LDA_LDB_ALU : 1
// CHECK: TESTSlotKind::TEST_SLOT_AAAAAAA,
// CHECK: TESTSlotKind::TEST_SLOT_BBBBB,
// CHECK: TESTSlotKind::TEST_SLOT_CCCC,
// CHECK: I128_LDA_LDB_NOP_ALU_LNG_NOP_VEC_ALL : 4
// CHECK: TESTSlotKind::TEST_SLOT_AAAAAAA,
// CHECK: TESTSlotKind::TEST_SLOT_BBBBB,
// CHECK: TESTSlotKind::TEST_SLOT_CCCC,
// CHECK: TESTSlotKind::TEST_SLOT_DDDDD,
// CHECK: TESTSlotKind::TEST_SLOT_SDASDADD,

// CHECK-LABEL: FormatData
// CHECK:        TEST::I16
// CHECK:        "I16"
// CHECK:        [0]{{.*}}[1]
// CHECK:        TEST::I64_LDA_LDB_ALU
// CHECK:        "I64_LDA_LDB_ALU"
// CHECK:        [1]{{.*}}[4]
// CHECK:        TEST::I128_LDA_LDB_NOP_ALU_LNG_NOP_VEC_ALL
// CHECK:        "I128_LDA_LDB_NOP_ALU_LNG_NOP_VEC_ALL"
// CHECK:        [4]{{.*}}[9]
