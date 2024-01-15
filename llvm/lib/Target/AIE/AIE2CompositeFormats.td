//===- AIE2CompositeFormats.td -----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
class AIE2CompositeInst<dag slot_ins>
  : AIE2Inst<(outs), slot_ins, "opcodestr", "argstr">
{
  let isComposite = true;
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;

  let AsmVariantName = "NonParsable";
  let DecoderNamespace = "Formats";
}

class AIE2_instr128_Composite<dag slot_ins> : AIE2CompositeInst<slot_ins>
{
  bits<128> Inst;
  bits<127> instr128;
  let Inst = {instr128, 0b0};
  let Size = 16;
}

class AIE2_instr48_Composite<dag slot_ins> : AIE2CompositeInst<slot_ins>
{
  bits<48> Inst;
  bits<45> instr48;
  let Inst = {instr48, 0b101};
  let Size = 6;
}

class AIE2_instr96_Composite<dag slot_ins> : AIE2CompositeInst<slot_ins>
{
  bits<96> Inst;
  bits<92> instr96;
  let Inst = {instr96, 0b0111};
  let Size = 12;
}

class AIE2_instr80_Composite<dag slot_ins> : AIE2CompositeInst<slot_ins>
{
  bits<80> Inst;
  bits<76> instr80;
  let Inst = {instr80, 0b1011};
  let Size = 10;
}

class AIE2_instr64_Composite<dag slot_ins> : AIE2CompositeInst<slot_ins>
{
  bits<64> Inst;
  bits<60> instr64;
  let Inst = {instr64, 0b0011};
  let Size = 8;
}

class AIE2_instr16_Composite<dag slot_ins> : AIE2CompositeInst<slot_ins>
{
  bits<16> Inst;
  bits<12> instr16;
  let Inst = {instr16, 0b0001};
  let Size = 2;
}

class AIE2_instr32_Composite<dag slot_ins> : AIE2CompositeInst<slot_ins>
{
  bits<32> Inst;
  bits<28> instr32;
  let Inst = {instr32, 0b1001};
  let Size = 4;
}

class AIE2_instr112_Composite<dag slot_ins> : AIE2CompositeInst<slot_ins>
{
  bits<112> Inst;
  bits<108> instr112;
  let Inst = {instr112, 0b1111};
  let Size = 14;
}

class AIE2__instr128__ldb__lda__st__lng__vec<dag slot_ins>
  : AIE2_instr128_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<21> lda;
  bits<21> st;
  bits<42> lng;
  bits<26> vec;
  // Intermediate fields
  bits<43> alu_mv;
  // Hierarchical definition
  let instr128 = {ldb, lda, st, alu_mv, vec};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr128__ldb__lda__st__alu__mv__vec<dag slot_ins>
  : AIE2_instr128_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<21> lda;
  bits<21> st;
  bits<20> alu;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr128 = {ldb, lda, st, alu_mv, vec};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr48__st__alu<dag slot_ins>
  : AIE2_instr48_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<20> alu;
  // Intermediate fields
  bits<45> inst_st_alu;
  // Hierarchical definition
  let instr48 = {inst_st_alu};
  let inst_st_alu = {st, alu, 0b0111};
}

class AIE2__instr48__lng<dag slot_ins>
  : AIE2_instr48_Composite<slot_ins>
{
  // Base slots
  bits<42> lng;
  // Intermediate fields
  bits<45> inst_lng;
  // Hierarchical definition
  let instr48 = {inst_lng};
  let inst_lng = {lng, 0b010};
}

class AIE2__instr48__ldb__st<dag slot_ins>
  : AIE2_instr48_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<21> st;
  // Intermediate fields
  bits<45> inst_ldb_st;
  // Hierarchical definition
  let instr48 = {inst_ldb_st};
  let inst_ldb_st = {0b00001 /*xxx01*/, ldb, st, 0b000 /*x00*/};
}

class AIE2__instr48__ldb__mv<dag slot_ins>
  : AIE2_instr48_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<22> mv;
  // Intermediate fields
  bits<45> inst_ldb_mv;
  // Hierarchical definition
  let instr48 = {inst_ldb_mv};
  let inst_ldb_mv = {0b00000 /*xxx00*/, ldb, mv, 0b00};
}

class AIE2__instr48__ldb__alu<dag slot_ins>
  : AIE2_instr48_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<20> alu;
  // Intermediate fields
  bits<45> inst_ldb_alu;
  // Hierarchical definition
  let instr48 = {inst_ldb_alu};
  let inst_ldb_alu = {0b00010 /*xxx10*/, ldb, alu, 0b0000 /*xx00*/};
}

class AIE2__instr48__lda__st<dag slot_ins>
  : AIE2_instr48_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<21> st;
  // Intermediate fields
  bits<45> inst_lda_st;
  // Hierarchical definition
  let instr48 = {inst_lda_st};
  let inst_lda_st = {lda, st, 0b110};
}

class AIE2__instr48__lda__mv<dag slot_ins>
  : AIE2_instr48_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<22> mv;
  // Intermediate fields
  bits<45> inst_lda_mv;
  // Hierarchical definition
  let instr48 = {inst_lda_mv};
  let inst_lda_mv = {lda, mv, 0b01};
}

class AIE2__instr48__lda__ldb<dag slot_ins>
  : AIE2_instr48_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  // Intermediate fields
  bits<45> inst_lda_ldb;
  // Hierarchical definition
  let instr48 = {inst_lda_ldb};
  let inst_lda_ldb = {lda, ldb, 0b00001111 /*xxxx1111*/};
}

class AIE2__instr48__lda__alu<dag slot_ins>
  : AIE2_instr48_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<20> alu;
  // Intermediate fields
  bits<45> inst_lda_alu;
  // Hierarchical definition
  let instr48 = {inst_lda_alu};
  let inst_lda_alu = {lda, alu, 0b0011 /*x011*/};
}

class AIE2__instr96__st__lng__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<42> lng;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_st_alu_mv_vec;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr96 = {inst_st_alu_mv_vec};
  let inst_st_alu_mv_vec = {st, alu_mv, vec, 0b01};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr96__st__alu__mv__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<20> alu;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_st_alu_mv_vec;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr96 = {inst_st_alu_mv_vec};
  let inst_st_alu_mv_vec = {st, alu_mv, vec, 0b01};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr96__st__ldb__mv__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<16> ldb;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_ldb_st_mv_vec;
  // Hierarchical definition
  let instr96 = {inst_ldb_st_mv_vec};
  let inst_ldb_st_mv_vec = {st, ldb, mv, 0b00110 /*x0110*/, vec, 0b10};
}

class AIE2__instr96__st__ldb__alu__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<16> ldb;
  bits<20> alu;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_ldb_st_alu_vec;
  // Hierarchical definition
  let instr96 = {inst_ldb_st_alu_vec};
  let inst_ldb_st_alu_vec = {st, ldb, 0b00000 /*xxxxx*/, alu, 0b01, vec, 0b10};
}

class AIE2__instr96__st__ldb__lng<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<16> ldb;
  bits<42> lng;
  // Intermediate fields
  bits<92> inst_ldb_st_alu_mv;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr96 = {inst_ldb_st_alu_mv};
  let inst_ldb_st_alu_mv = {st, ldb, 0b00000 /*xxxxx*/, alu_mv, 0b0000011 /*xx00011*/};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr96__st__ldb__alu__mv<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<16> ldb;
  bits<20> alu;
  bits<22> mv;
  // Intermediate fields
  bits<92> inst_ldb_st_alu_mv;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr96 = {inst_ldb_st_alu_mv};
  let inst_ldb_st_alu_mv = {st, ldb, 0b00000 /*xxxxx*/, alu_mv, 0b0000011 /*xx00011*/};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr96__ldb__lng__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<42> lng;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_ldb_alu_mv_vec;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr96 = {inst_ldb_alu_mv_vec};
  let inst_ldb_alu_mv_vec = {ldb, 0b0 /*x*/, alu_mv, 0b1010, vec, 0b10};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr96__ldb__alu__mv__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<20> alu;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_ldb_alu_mv_vec;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr96 = {inst_ldb_alu_mv_vec};
  let inst_ldb_alu_mv_vec = {ldb, 0b0 /*x*/, alu_mv, 0b1010, vec, 0b10};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr96__lda__st__alu__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<21> st;
  bits<20> alu;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_lda_st_alu_vec;
  // Hierarchical definition
  let instr96 = {inst_lda_st_alu_vec};
  let inst_lda_st_alu_vec = {lda, st, alu, 0b00, vec, 0b10};
}

class AIE2__instr96__lda__st__lng<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<21> st;
  bits<42> lng;
  // Intermediate fields
  bits<92> inst_lda_st_alu_mv;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr96 = {inst_lda_st_alu_mv};
  let inst_lda_st_alu_mv = {lda, st, alu_mv, 0b0000111 /*xx00111*/};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr96__lda__st__alu__mv<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<21> st;
  bits<20> alu;
  bits<22> mv;
  // Intermediate fields
  bits<92> inst_lda_st_alu_mv;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr96 = {inst_lda_st_alu_mv};
  let inst_lda_st_alu_mv = {lda, st, alu_mv, 0b0000111 /*xx00111*/};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr96__lda__ldb__st__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<21> st;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_lda_ldb_st_vec;
  // Hierarchical definition
  let instr96 = {inst_lda_ldb_st_vec};
  let inst_lda_ldb_st_vec = {lda, ldb, st, 0b001110 /*xx1110*/, vec, 0b10};
}

class AIE2__instr96__lda__ldb__st__mv<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<21> st;
  bits<22> mv;
  // Intermediate fields
  bits<92> inst_lda_ldb_st_mv;
  // Hierarchical definition
  let instr96 = {inst_lda_ldb_st_mv};
  let inst_lda_ldb_st_mv = {lda, ldb, st, 0b0000 /*xxxx*/, mv, 0b00001111 /*xxx01111*/};
}

class AIE2__instr96__lda__ldb__alu__st<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<20> alu;
  bits<21> st;
  // Intermediate fields
  bits<92> inst_lda_ldb_st_alu;
  // Hierarchical definition
  let instr96 = {inst_lda_ldb_st_alu};
  let inst_lda_ldb_st_alu = {lda, ldb, 0b00000 /*xxxxx*/, alu, st, 0b000010011 /*xxxx10011*/};
}

class AIE2__instr96__lda__ldb__mv__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_lda_ldb_mv_vec;
  // Hierarchical definition
  let instr96 = {inst_lda_ldb_mv_vec};
  let inst_lda_ldb_mv_vec = {lda, ldb, mv, 0b00010 /*x0010*/, vec, 0b10};
}

class AIE2__instr96__lda__ldb__alu__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<20> alu;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_lda_ldb_alu_vec;
  // Hierarchical definition
  let instr96 = {inst_lda_ldb_alu_vec};
  let inst_lda_ldb_alu_vec = {lda, ldb, 0b00000 /*xxxxx*/, alu, 0b11, vec, 0b10};
}

class AIE2__instr96__lda__ldb__lng<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<42> lng;
  // Intermediate fields
  bits<92> inst_lda_ldb_alu_mv;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr96 = {inst_lda_ldb_alu_mv};
  let inst_lda_ldb_alu_mv = {lda, ldb, 0b00000 /*xxxxx*/, alu_mv, 0b0001011 /*xx01011*/};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr96__lda__ldb__alu__mv<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<20> alu;
  bits<22> mv;
  // Intermediate fields
  bits<92> inst_lda_ldb_alu_mv;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr96 = {inst_lda_ldb_alu_mv};
  let inst_lda_ldb_alu_mv = {lda, ldb, 0b00000 /*xxxxx*/, alu_mv, 0b0001011 /*xx01011*/};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr96__lda__lng__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<42> lng;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_lda_alu_mv_vec;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr96 = {inst_lda_alu_mv_vec};
  let inst_lda_alu_mv_vec = {lda, alu_mv, vec, 0b00};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr96__lda__alu__mv__vec<dag slot_ins>
  : AIE2_instr96_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<20> alu;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<92> inst_lda_alu_mv_vec;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr96 = {inst_lda_alu_mv_vec};
  let inst_lda_alu_mv_vec = {lda, alu_mv, vec, 0b00};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr80__st__mv__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_st_mv_vec;
  // Hierarchical definition
  let instr80 = {inst_st_mv_vec};
  let inst_st_mv_vec = {st, mv, 0b00100 /*xx100*/, vec, 0b00};
}

class AIE2__instr80__st__alu__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<20> alu;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_st_alu_vec;
  // Hierarchical definition
  let instr80 = {inst_st_alu_vec};
  let inst_st_alu_vec = {st, 0b00000 /*xxxxx*/, alu, 0b01, vec, 0b10};
}

class AIE2__instr80__st__lng<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<42> lng;
  // Intermediate fields
  bits<76> inst_st_alu_mv;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr80 = {inst_st_alu_mv};
  let inst_st_alu_mv = {st, 0b00000 /*xxxxx*/, alu_mv, 0b0010011 /*xx10011*/};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr80__st__alu__mv<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<20> alu;
  bits<22> mv;
  // Intermediate fields
  bits<76> inst_st_alu_mv;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr80 = {inst_st_alu_mv};
  let inst_st_alu_mv = {st, 0b00000 /*xxxxx*/, alu_mv, 0b0010011 /*xx10011*/};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr80__st__ldb__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<16> ldb;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_ldb_st_vec;
  // Hierarchical definition
  let instr80 = {inst_ldb_st_vec};
  let inst_ldb_st_vec = {st, ldb, 0b00000 /*xxxxx*/, 0b000001 /*xx0001*/, vec, 0b00};
}

class AIE2__instr80__st__ldb__mv<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<16> ldb;
  bits<22> mv;
  // Intermediate fields
  bits<76> inst_ldb_st_mv;
  // Hierarchical definition
  let instr80 = {inst_ldb_st_mv};
  let inst_ldb_st_mv = {st, ldb, 0b000000000 /*xxxxxxxxx*/, mv, 0b01101011};
}

class AIE2__instr80__ldb__mv__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_ldb_mv_vec;
  // Hierarchical definition
  let instr80 = {inst_ldb_mv_vec};
  let inst_ldb_mv_vec = {0b00000 /*xxxxx*/, ldb, mv, 0b00110 /*xx110*/, vec, 0b00};
}

class AIE2__instr80__ldb__alu__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<20> alu;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_ldb_alu_vec;
  // Hierarchical definition
  let instr80 = {inst_ldb_alu_vec};
  let inst_ldb_alu_vec = {0b00000 /*xxxxx*/, ldb, 0b00000 /*xxxxx*/, alu, 0b10, vec, 0b10};
}

class AIE2__instr80__ldb__lng<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<42> lng;
  // Intermediate fields
  bits<76> inst_ldb_alu_mv;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr80 = {inst_ldb_alu_mv};
  let inst_ldb_alu_mv = {0b00000 /*xxxxx*/, ldb, 0b00000 /*xxxxx*/, alu_mv, 0b0000011 /*xx00011*/};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr80__ldb__alu__mv<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<20> alu;
  bits<22> mv;
  // Intermediate fields
  bits<76> inst_ldb_alu_mv;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr80 = {inst_ldb_alu_mv};
  let inst_ldb_alu_mv = {0b00000 /*xxxxx*/, ldb, 0b00000 /*xxxxx*/, alu_mv, 0b0000011 /*xx00011*/};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr80__lda__st__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<21> st;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_lda_st_vec;
  // Hierarchical definition
  let instr80 = {inst_lda_st_vec};
  let inst_lda_st_vec = {lda, st, 0b001001 /*xx1001*/, vec, 0b00};
}

class AIE2__instr80__lda__st__mv<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<21> st;
  bits<22> mv;
  // Intermediate fields
  bits<76> inst_lda_st_mv;
  // Hierarchical definition
  let instr80 = {inst_lda_st_mv};
  let inst_lda_st_mv = {lda, st, 0b0000 /*xxxx*/, mv, 0b00101011 /*x0101011*/};
}

class AIE2__instr80__lda__st__alu<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<21> st;
  bits<20> alu;
  // Intermediate fields
  bits<76> inst_lda_st_alu;
  // Hierarchical definition
  let instr80 = {inst_lda_st_alu};
  let inst_lda_st_alu = {lda, st, 0b00000000000 /*xxxxxxxxxxx*/, alu, 0b111};
}

class AIE2__instr80__lda__mv__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_lda_mv_vec;
  // Hierarchical definition
  let instr80 = {inst_lda_mv_vec};
  let inst_lda_mv_vec = {lda, mv, 0b00010 /*xx010*/, vec, 0b00};
}

class AIE2__instr80__lda__ldb__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_lda_ldb_vec;
  // Hierarchical definition
  let instr80 = {inst_lda_ldb_vec};
  let inst_lda_ldb_vec = {lda, ldb, 0b00000011101 /*xxxxxx11101*/, vec, 0b00};
}

class AIE2__instr80__lda__ldb__mv<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<22> mv;
  // Intermediate fields
  bits<76> inst_lda_ldb_mv;
  // Hierarchical definition
  let instr80 = {inst_lda_ldb_mv};
  let inst_lda_ldb_mv = {lda, ldb, 0b000000000 /*xxxxxxxxx*/, mv, 0b11101011};
}

class AIE2__instr80__lda__alu__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<20> alu;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_lda_alu_vec;
  // Hierarchical definition
  let instr80 = {inst_lda_alu_vec};
  let inst_lda_alu_vec = {lda, 0b00000 /*xxxxx*/, alu, 0b00, vec, 0b10};
}

class AIE2__instr80__lda__lng<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<42> lng;
  // Intermediate fields
  bits<76> inst_lda_alu_mv;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr80 = {inst_lda_alu_mv};
  let inst_lda_alu_mv = {lda, 0b00000 /*xxxxx*/, alu_mv, 0b0001011 /*x001011*/};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr80__lda__alu__mv<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<20> alu;
  bits<22> mv;
  // Intermediate fields
  bits<76> inst_lda_alu_mv;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr80 = {inst_lda_alu_mv};
  let inst_lda_alu_mv = {lda, 0b00000 /*xxxxx*/, alu_mv, 0b0001011 /*x001011*/};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr80__lng__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<42> lng;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_alu_mv_vec;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr80 = {inst_alu_mv_vec};
  let inst_alu_mv_vec = {0b00000 /*xxxxx*/, alu_mv, vec, 0b01};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr80__alu__mv__vec<dag slot_ins>
  : AIE2_instr80_Composite<slot_ins>
{
  // Base slots
  bits<20> alu;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<76> inst_alu_mv_vec;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr80 = {inst_alu_mv_vec};
  let inst_alu_mv_vec = {0b00000 /*xxxxx*/, alu_mv, vec, 0b01};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr64__st__vec<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<26> vec;
  // Intermediate fields
  bits<60> inst_st_vec;
  // Hierarchical definition
  let instr64 = {inst_st_vec};
  let inst_st_vec = {st, 0b00000000010 /*xxxxxxxx010*/, vec, 0b10};
}

class AIE2__instr64__st__mv<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<22> mv;
  // Intermediate fields
  bits<60> inst_st_mv;
  // Hierarchical definition
  let instr64 = {inst_st_mv};
  let inst_st_mv = {st, 0b000000000 /*xxxxxxxxx*/, mv, 0b00000000 /*xxxxx000*/};
}

class AIE2__instr64__mv__vec<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<60> inst_mv_vec;
  // Hierarchical definition
  let instr64 = {inst_mv_vec};
  let inst_mv_vec = {0b00000 /*xxxxx*/, mv, 0b00110 /*xx110*/, vec, 0b10};
}

class AIE2__instr64__ldb__vec<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  bits<26> vec;
  // Intermediate fields
  bits<60> inst_ldb_vec;
  // Hierarchical definition
  let instr64 = {inst_ldb_vec};
  let inst_ldb_vec = {0b00000 /*xxxxx*/, ldb, 0b00000000000 /*xxxxxxxx000*/, vec, 0b10};
}

class AIE2__instr64__st__ldb__alu<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<16> ldb;
  bits<20> alu;
  // Intermediate fields
  bits<60> inst_ldb_st_alu;
  // Hierarchical definition
  let instr64 = {inst_ldb_st_alu};
  let inst_ldb_st_alu = {st, ldb, alu, 0b101};
}

class AIE2__instr64__lda__vec<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<26> vec;
  // Intermediate fields
  bits<60> inst_lda_vec;
  // Hierarchical definition
  let instr64 = {inst_lda_vec};
  let inst_lda_vec = {lda, 0b00000000100 /*xxxxxxxx100*/, vec, 0b10};
}

class AIE2__instr64__lda__ldb__st<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<21> st;
  // Intermediate fields
  bits<60> inst_lda_ldb_st;
  // Hierarchical definition
  let instr64 = {inst_lda_ldb_st};
  let inst_lda_ldb_st = {lda, ldb, st, 0b11};
}

class AIE2__instr64__lda__ldb__alu<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<20> alu;
  // Intermediate fields
  bits<60> inst_lda_ldb_alu;
  // Hierarchical definition
  let instr64 = {inst_lda_ldb_alu};
  let inst_lda_ldb_alu = {lda, ldb, alu, 0b001};
}

class AIE2__instr64__alu__vec<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> alu;
  bits<26> vec;
  // Intermediate fields
  bits<60> inst_alu_vec;
  // Hierarchical definition
  let instr64 = {inst_alu_vec};
  let inst_alu_vec = {0b0000000000 /*xxxxxxxxxx*/, alu, 0b01, vec, 0b10};
}

class AIE2__instr64__lng<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<42> lng;
  // Intermediate fields
  bits<60> inst_alu_mv;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr64 = {inst_alu_mv};
  let inst_alu_mv = {0b0000000000 /*xxxxxxxxxx*/, alu_mv, 0b0000100 /*xxxx100*/};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr64__alu__mv<dag slot_ins>
  : AIE2_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> alu;
  bits<22> mv;
  // Intermediate fields
  bits<60> inst_alu_mv;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr64 = {inst_alu_mv};
  let inst_alu_mv = {0b0000000000 /*xxxxxxxxxx*/, alu_mv, 0b0000100 /*xxxx100*/};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr32__vec<dag slot_ins>
  : AIE2_instr32_Composite<slot_ins>
{
  // Base slots
  bits<26> vec;
  // Intermediate fields
  bits<28> inst_vec;
  // Hierarchical definition
  let instr32 = {inst_vec};
  let inst_vec = {vec, 0b00 /*x0*/};
}

class AIE2__instr32__st<dag slot_ins>
  : AIE2_instr32_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  // Intermediate fields
  bits<28> inst_st;
  // Hierarchical definition
  let instr32 = {inst_st};
  let inst_st = {0b00001 /*x0001*/, st, 0b01 /*x1*/};
}

class AIE2__instr32__mv<dag slot_ins>
  : AIE2_instr32_Composite<slot_ins>
{
  // Base slots
  bits<22> mv;
  // Intermediate fields
  bits<28> inst_mv;
  // Hierarchical definition
  let instr32 = {inst_mv};
  let inst_mv = {0b00011 /*x0011*/, mv, 0b1};
}

class AIE2__instr32__ldb<dag slot_ins>
  : AIE2_instr32_Composite<slot_ins>
{
  // Base slots
  bits<16> ldb;
  // Intermediate fields
  bits<28> inst_ldb;
  // Hierarchical definition
  let instr32 = {inst_ldb};
  let inst_ldb = {0b00111 /*xx111*/, ldb, 0b0000001 /*xxxxxx1*/};
}

class AIE2__instr32__lda<dag slot_ins>
  : AIE2_instr32_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  // Intermediate fields
  bits<28> inst_lda;
  // Hierarchical definition
  let instr32 = {inst_lda};
  let inst_lda = {0b00000 /*x0000*/, lda, 0b01 /*x1*/};
}

class AIE2__instr32__alu<dag slot_ins>
  : AIE2_instr32_Composite<slot_ins>
{
  // Base slots
  bits<20> alu;
  // Intermediate fields
  bits<28> inst_alu;
  // Hierarchical definition
  let instr32 = {inst_alu};
  let inst_alu = {0b00010 /*x0010*/, alu, 0b001 /*xx1*/};
}

class AIE2__instr16__nop<dag slot_ins>
  : AIE2_instr16_Composite<slot_ins>
{
  // Only slot
  bits<12> nop16;
  // There is just one insn which is nop
  let instr16 = {nop16};
}

class AIE2__instr112__st__ldb__lng__vec<dag slot_ins>
  : AIE2_instr112_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<16> ldb;
  bits<42> lng;
  bits<26> vec;
  // Intermediate fields
  bits<108> inst_ldb_st_alu_mv_vec;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr112 = {inst_ldb_st_alu_mv_vec};
  let inst_ldb_st_alu_mv_vec = {st, ldb, alu_mv, vec, 0b01};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr112__st__ldb__alu__mv__vec<dag slot_ins>
  : AIE2_instr112_Composite<slot_ins>
{
  // Base slots
  bits<21> st;
  bits<16> ldb;
  bits<20> alu;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<108> inst_ldb_st_alu_mv_vec;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr112 = {inst_ldb_st_alu_mv_vec};
  let inst_ldb_st_alu_mv_vec = {st, ldb, alu_mv, vec, 0b01};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr112__lda__st__mv__vec<dag slot_ins>
  : AIE2_instr112_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<21> st;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<108> inst_lda_st_mv_vec;
  // Hierarchical definition
  let instr112 = {inst_lda_st_mv_vec};
  let inst_lda_st_mv_vec = {lda, st, 0b0000000000000001 /*xxxxxxxxxxxxxxx1*/, mv, vec, 0b10};
}

class AIE2__instr112__lda__ldb__alu__st__vec<dag slot_ins>
  : AIE2_instr112_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<20> alu;
  bits<21> st;
  bits<26> vec;
  // Intermediate fields
  bits<108> inst_lda_ldb_st_alu_vec;
  // Hierarchical definition
  let instr112 = {inst_lda_ldb_st_alu_vec};
  let inst_lda_ldb_st_alu_vec = {lda, ldb, alu, 0b00 /*0x*/, st, vec, 0b10};
}

class AIE2__instr112__lda__ldb__lng__st<dag slot_ins>
  : AIE2_instr112_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<42> lng;
  bits<21> st;
  // Intermediate fields
  bits<108> inst_lda_ldb_st_alu_mv;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr112 = {inst_lda_ldb_st_alu_mv};
  let inst_lda_ldb_st_alu_mv = {lda, ldb, alu_mv, st, 0b0000111 /*xxx0111*/};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr112__lda__ldb__alu__mv__st<dag slot_ins>
  : AIE2_instr112_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<20> alu;
  bits<22> mv;
  bits<21> st;
  // Intermediate fields
  bits<108> inst_lda_ldb_st_alu_mv;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr112 = {inst_lda_ldb_st_alu_mv};
  let inst_lda_ldb_st_alu_mv = {lda, ldb, alu_mv, st, 0b0000111 /*xxx0111*/};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}

class AIE2__instr112__lda__ldb__lng__vec<dag slot_ins>
  : AIE2_instr112_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<42> lng;
  bits<26> vec;
  // Intermediate fields
  bits<108> inst_lda_ldb_alu_mv_vec;
  bits<43> alu_mv;
  // Hierarchical definition
  let instr112 = {inst_lda_ldb_alu_mv_vec};
  let inst_lda_ldb_alu_mv_vec = {lda, ldb, alu_mv, vec, 0b00};
  let alu_mv = {lng, 0b0};
}

class AIE2__instr112__lda__ldb__alu__mv__vec<dag slot_ins>
  : AIE2_instr112_Composite<slot_ins>
{
  // Base slots
  bits<21> lda;
  bits<16> ldb;
  bits<20> alu;
  bits<22> mv;
  bits<26> vec;
  // Intermediate fields
  bits<108> inst_lda_ldb_alu_mv_vec;
  bits<43> alu_mv;
  bits<42> alumv;
  // Hierarchical definition
  let instr112 = {inst_lda_ldb_alu_mv_vec};
  let inst_lda_ldb_alu_mv_vec = {lda, ldb, alu_mv, vec, 0b00};
  let alu_mv = {alumv, 0b1};
  let alumv = {alu, mv};
}


// Composite Instructions of AIE2. These list all the possible formats. The
// decodertables of the formats are isolated in a different namespace, and
// used by the top-level disassembler. For disassembling the instruction slots,
// we use the regular decoder tables from top-level hand-crafted decoder
// routines attached to the slot operands here.
def I128_LDB_LDA_ST_LNG_VEC : AIE2__instr128__ldb__lda__st__lng__vec<(ins ldb_slot:$ldb, lda_slot:$lda, st_slot:$st, lng_slot:$lng, vec_slot:$vec)>;
def I128_LDB_LDA_ST_ALU_MV_VEC : AIE2__instr128__ldb__lda__st__alu__mv__vec<(ins ldb_slot:$ldb, lda_slot:$lda, st_slot:$st, alu_slot:$alu, mv_slot:$mv, vec_slot:$vec)>;
def I48_ST_ALU : AIE2__instr48__st__alu<(ins st_slot:$st, alu_slot:$alu)>;
def I48_LNG : AIE2__instr48__lng<(ins lng_slot:$lng)>;
def I48_LDB_ST : AIE2__instr48__ldb__st<(ins ldb_slot:$ldb, st_slot:$st)>;
def I48_LDB_MV : AIE2__instr48__ldb__mv<(ins ldb_slot:$ldb, mv_slot:$mv)>;
def I48_LDB_ALU : AIE2__instr48__ldb__alu<(ins ldb_slot:$ldb, alu_slot:$alu)>;
def I48_LDA_ST : AIE2__instr48__lda__st<(ins lda_slot:$lda, st_slot:$st)>;
def I48_LDA_MV : AIE2__instr48__lda__mv<(ins lda_slot:$lda, mv_slot:$mv)>;
def I48_LDA_LDB : AIE2__instr48__lda__ldb<(ins lda_slot:$lda, ldb_slot:$ldb)>;
def I48_LDA_ALU : AIE2__instr48__lda__alu<(ins lda_slot:$lda, alu_slot:$alu)>;
def I96_ST_LNG_VEC : AIE2__instr96__st__lng__vec<(ins st_slot:$st, lng_slot:$lng, vec_slot:$vec)>;
def I96_ST_ALU_MV_VEC : AIE2__instr96__st__alu__mv__vec<(ins st_slot:$st, alu_slot:$alu, mv_slot:$mv, vec_slot:$vec)>;
def I96_ST_LDB_MV_VEC : AIE2__instr96__st__ldb__mv__vec<(ins st_slot:$st, ldb_slot:$ldb, mv_slot:$mv, vec_slot:$vec)>;
def I96_ST_LDB_ALU_VEC : AIE2__instr96__st__ldb__alu__vec<(ins st_slot:$st, ldb_slot:$ldb, alu_slot:$alu, vec_slot:$vec)>;
def I96_ST_LDB_LNG : AIE2__instr96__st__ldb__lng<(ins st_slot:$st, ldb_slot:$ldb, lng_slot:$lng)>;
def I96_ST_LDB_ALU_MV : AIE2__instr96__st__ldb__alu__mv<(ins st_slot:$st, ldb_slot:$ldb, alu_slot:$alu, mv_slot:$mv)>;
def I96_LDB_LNG_VEC : AIE2__instr96__ldb__lng__vec<(ins ldb_slot:$ldb, lng_slot:$lng, vec_slot:$vec)>;
def I96_LDB_ALU_MV_VEC : AIE2__instr96__ldb__alu__mv__vec<(ins ldb_slot:$ldb, alu_slot:$alu, mv_slot:$mv, vec_slot:$vec)>;
def I96_LDA_ST_ALU_VEC : AIE2__instr96__lda__st__alu__vec<(ins lda_slot:$lda, st_slot:$st, alu_slot:$alu, vec_slot:$vec)>;
def I96_LDA_ST_LNG : AIE2__instr96__lda__st__lng<(ins lda_slot:$lda, st_slot:$st, lng_slot:$lng)>;
def I96_LDA_ST_ALU_MV : AIE2__instr96__lda__st__alu__mv<(ins lda_slot:$lda, st_slot:$st, alu_slot:$alu, mv_slot:$mv)>;
def I96_LDA_LDB_ST_VEC : AIE2__instr96__lda__ldb__st__vec<(ins lda_slot:$lda, ldb_slot:$ldb, st_slot:$st, vec_slot:$vec)>;
def I96_LDA_LDB_ST_MV : AIE2__instr96__lda__ldb__st__mv<(ins lda_slot:$lda, ldb_slot:$ldb, st_slot:$st, mv_slot:$mv)>;
def I96_LDA_LDB_ALU_ST : AIE2__instr96__lda__ldb__alu__st<(ins lda_slot:$lda, ldb_slot:$ldb, alu_slot:$alu, st_slot:$st)>;
def I96_LDA_LDB_MV_VEC : AIE2__instr96__lda__ldb__mv__vec<(ins lda_slot:$lda, ldb_slot:$ldb, mv_slot:$mv, vec_slot:$vec)>;
def I96_LDA_LDB_ALU_VEC : AIE2__instr96__lda__ldb__alu__vec<(ins lda_slot:$lda, ldb_slot:$ldb, alu_slot:$alu, vec_slot:$vec)>;
def I96_LDA_LDB_LNG : AIE2__instr96__lda__ldb__lng<(ins lda_slot:$lda, ldb_slot:$ldb, lng_slot:$lng)>;
def I96_LDA_LDB_ALU_MV : AIE2__instr96__lda__ldb__alu__mv<(ins lda_slot:$lda, ldb_slot:$ldb, alu_slot:$alu, mv_slot:$mv)>;
def I96_LDA_LNG_VEC : AIE2__instr96__lda__lng__vec<(ins lda_slot:$lda, lng_slot:$lng, vec_slot:$vec)>;
def I96_LDA_ALU_MV_VEC : AIE2__instr96__lda__alu__mv__vec<(ins lda_slot:$lda, alu_slot:$alu, mv_slot:$mv, vec_slot:$vec)>;
def I80_ST_MV_VEC : AIE2__instr80__st__mv__vec<(ins st_slot:$st, mv_slot:$mv, vec_slot:$vec)>;
def I80_ST_ALU_VEC : AIE2__instr80__st__alu__vec<(ins st_slot:$st, alu_slot:$alu, vec_slot:$vec)>;
def I80_ST_LNG : AIE2__instr80__st__lng<(ins st_slot:$st, lng_slot:$lng)>;
def I80_ST_ALU_MV : AIE2__instr80__st__alu__mv<(ins st_slot:$st, alu_slot:$alu, mv_slot:$mv)>;
def I80_ST_LDB_VEC : AIE2__instr80__st__ldb__vec<(ins st_slot:$st, ldb_slot:$ldb, vec_slot:$vec)>;
def I80_ST_LDB_MV : AIE2__instr80__st__ldb__mv<(ins st_slot:$st, ldb_slot:$ldb, mv_slot:$mv)>;
def I80_LDB_MV_VEC : AIE2__instr80__ldb__mv__vec<(ins ldb_slot:$ldb, mv_slot:$mv, vec_slot:$vec)>;
def I80_LDB_ALU_VEC : AIE2__instr80__ldb__alu__vec<(ins ldb_slot:$ldb, alu_slot:$alu, vec_slot:$vec)>;
def I80_LDB_LNG : AIE2__instr80__ldb__lng<(ins ldb_slot:$ldb, lng_slot:$lng)>;
def I80_LDB_ALU_MV : AIE2__instr80__ldb__alu__mv<(ins ldb_slot:$ldb, alu_slot:$alu, mv_slot:$mv)>;
def I80_LDA_ST_VEC : AIE2__instr80__lda__st__vec<(ins lda_slot:$lda, st_slot:$st, vec_slot:$vec)>;
def I80_LDA_ST_MV : AIE2__instr80__lda__st__mv<(ins lda_slot:$lda, st_slot:$st, mv_slot:$mv)>;
def I80_LDA_ST_ALU : AIE2__instr80__lda__st__alu<(ins lda_slot:$lda, st_slot:$st, alu_slot:$alu)>;
def I80_LDA_MV_VEC : AIE2__instr80__lda__mv__vec<(ins lda_slot:$lda, mv_slot:$mv, vec_slot:$vec)>;
def I80_LDA_LDB_VEC : AIE2__instr80__lda__ldb__vec<(ins lda_slot:$lda, ldb_slot:$ldb, vec_slot:$vec)>;
def I80_LDA_LDB_MV : AIE2__instr80__lda__ldb__mv<(ins lda_slot:$lda, ldb_slot:$ldb, mv_slot:$mv)>;
def I80_LDA_ALU_VEC : AIE2__instr80__lda__alu__vec<(ins lda_slot:$lda, alu_slot:$alu, vec_slot:$vec)>;
def I80_LDA_LNG : AIE2__instr80__lda__lng<(ins lda_slot:$lda, lng_slot:$lng)>;
def I80_LDA_ALU_MV : AIE2__instr80__lda__alu__mv<(ins lda_slot:$lda, alu_slot:$alu, mv_slot:$mv)>;
def I80_LNG_VEC : AIE2__instr80__lng__vec<(ins lng_slot:$lng, vec_slot:$vec)>;
def I80_ALU_MV_VEC : AIE2__instr80__alu__mv__vec<(ins alu_slot:$alu, mv_slot:$mv, vec_slot:$vec)>;
def I64_ST_VEC : AIE2__instr64__st__vec<(ins st_slot:$st, vec_slot:$vec)>;
def I64_ST_MV : AIE2__instr64__st__mv<(ins st_slot:$st, mv_slot:$mv)>;
def I64_MV_VEC : AIE2__instr64__mv__vec<(ins mv_slot:$mv, vec_slot:$vec)>;
def I64_LDB_VEC : AIE2__instr64__ldb__vec<(ins ldb_slot:$ldb, vec_slot:$vec)>;
def I64_ST_LDB_ALU : AIE2__instr64__st__ldb__alu<(ins st_slot:$st, ldb_slot:$ldb, alu_slot:$alu)>;
def I64_LDA_VEC : AIE2__instr64__lda__vec<(ins lda_slot:$lda, vec_slot:$vec)>;
def I64_LDA_LDB_ST : AIE2__instr64__lda__ldb__st<(ins lda_slot:$lda, ldb_slot:$ldb, st_slot:$st)>;
def I64_LDA_LDB_ALU : AIE2__instr64__lda__ldb__alu<(ins lda_slot:$lda, ldb_slot:$ldb, alu_slot:$alu)>;
def I64_ALU_VEC : AIE2__instr64__alu__vec<(ins alu_slot:$alu, vec_slot:$vec)>;
def I64_LNG : AIE2__instr64__lng<(ins lng_slot:$lng)>;
def I64_ALU_MV : AIE2__instr64__alu__mv<(ins alu_slot:$alu, mv_slot:$mv)>;
// nop_slot has all don't care bits
def I16_NOP : AIE2__instr16__nop<(ins nop_slot:$nop16)>;
def I32_VEC : AIE2__instr32__vec<(ins vec_slot:$vec)>;
def I32_ST : AIE2__instr32__st<(ins st_slot:$st)>;
def I32_MV : AIE2__instr32__mv<(ins mv_slot:$mv)>;
def I32_LDB : AIE2__instr32__ldb<(ins ldb_slot:$ldb)>;
def I32_LDA : AIE2__instr32__lda<(ins lda_slot:$lda)>;
def I32_ALU : AIE2__instr32__alu<(ins alu_slot:$alu)>;
def I112_ST_LDB_LNG_VEC : AIE2__instr112__st__ldb__lng__vec<(ins st_slot:$st, ldb_slot:$ldb, lng_slot:$lng, vec_slot:$vec)>;
def I112_ST_LDB_ALU_MV_VEC : AIE2__instr112__st__ldb__alu__mv__vec<(ins st_slot:$st, ldb_slot:$ldb, alu_slot:$alu, mv_slot:$mv, vec_slot:$vec)>;
def I112_LDA_ST_MV_VEC : AIE2__instr112__lda__st__mv__vec<(ins lda_slot:$lda, st_slot:$st, mv_slot:$mv, vec_slot:$vec)>;
def I112_LDA_LDB_ALU_ST_VEC : AIE2__instr112__lda__ldb__alu__st__vec<(ins lda_slot:$lda, ldb_slot:$ldb, alu_slot:$alu, st_slot:$st, vec_slot:$vec)>;
def I112_LDA_LDB_LNG_ST : AIE2__instr112__lda__ldb__lng__st<(ins lda_slot:$lda, ldb_slot:$ldb, lng_slot:$lng, st_slot:$st)>;
def I112_LDA_LDB_ALU_MV_ST : AIE2__instr112__lda__ldb__alu__mv__st<(ins lda_slot:$lda, ldb_slot:$ldb, alu_slot:$alu, mv_slot:$mv, st_slot:$st)>;
def I112_LDA_LDB_LNG_VEC : AIE2__instr112__lda__ldb__lng__vec<(ins lda_slot:$lda, ldb_slot:$ldb, lng_slot:$lng, vec_slot:$vec)>;
def I112_LDA_LDB_ALU_MV_VEC : AIE2__instr112__lda__ldb__alu__mv__vec<(ins lda_slot:$lda, ldb_slot:$ldb, alu_slot:$alu, mv_slot:$mv, vec_slot:$vec)>;
