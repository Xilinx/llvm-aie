//===- AIE2GenInstrFormats.td ------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
include "AIEBaseInstrFormats.td"

class AIE2Inst<dag outs, dag ins, string opcodestr, string argstr, list<dag> pattern>
    : AIEBaseInst <outs, ins, pattern> {
  let Namespace = "AIE2";
  let AsmString = opcodestr # "\t" # argstr;
}

class AIE2_instr32_me < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  field bits<32> SoftFail = 0;
  field bits<32> Inst;
   field bits<28> instr32;

  let Inst = {instr32,0b1001};
}

class AIE2_instr48_me < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  field bits<48> SoftFail = 0;
  field bits<48> Inst;
  let Size = 6;
   field bits<45> instr48;

  let Inst = {instr48,0b101};
}

class AIE2_instr64_me < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  field bits<64> SoftFail = 0;
  field bits<64> Inst;
  let Size = 8;
   field bits<60> instr64;

  let Inst = {instr64,0b0011};
}

class AIE2_instr96_me < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  field bits<96> SoftFail = 0;
  field bits<96> Inst;
  let Size = 12;
   field bits<92> instr96;

  let Inst = {instr96,0b0111};
}

class AIE2_instr112_me < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  field bits<112> SoftFail = 0;
  field bits<112> Inst;
  let Size = 14;
   field bits<108> instr112;

  let Inst = {instr112,0b1111};
}

class AIE2_inst_alu_instr32 < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_instr32_me <outs, ins, pattern, opcodestr, argstr> {
  bits<20> alu;

  let instr32 = {0b00010,alu,0b001};
  let Slot = alu_slot;
}

class AIE2_add_r_ri_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<17> add_r_ri;
  bits<5> mRx;
  bits<7> c7s;
  bits<5> mRx0;

  let alu = {mRx0,mRx,c7s,0b11,0b0};
}

class AIE2_inst_lda_instr32 < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_instr32_me <outs, ins, pattern, opcodestr, argstr> {
  bits<21> lda;

  let instr32 = {0b00000,lda,0b01};
  let Slot = lda_slot;
}

class AIE2_dmhb_sts_inst_lda < bits<1> op, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<1> eOpSt;
  bits<5> mRv;

  field bits<9> ag_nospill;

  let lda = {ag_nospill,op,0b100,mRv,0b000};
}

class AIE2_inst_st_instr32 < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_instr32_me <outs, ins, pattern, opcodestr, argstr> {
  bits<21> st;
  field bits<1> dontcare1;
  let instr32 = {dontcare1,0b0001,st,dontcare1,0b1};
  let Slot = st_slot;
}

class AIE2_dms_sts_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<7> mSclSt;
  bits<13> ag_all;

  let st = {ag_all, mSclSt, 0b0};
}

class AIE2_dmw_sts_pack_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<8> mv_pack_src;
  bits<9> ags_memw_pack;
  bits<9> ag_nospill = 0;
  let ags_memw_pack = {ag_nospill};
  let st = {ags_memw_pack,mv_pack_src,0b101,0b0};
}

class AIE2_dmv_sts_w_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> src;
  bits<13> ag_all;
  field bits<13> ags_memw = {ag_all};

  let st = {ags_memw,src,0b011};
}

class AIE2_dmw_sts_srs_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<10> mWsrs;
  bits<9> ags_memw_srs;
  bits<9> ag_nospill = 0;
  let ags_memw_srs = {ag_nospill};

  let st = {ags_memw_srs,mWsrs,0b1,0b0};
}

class AIE2_dmv_sts_q_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<2> src;
  bits<13> ag_all;
  field bits<13> ags_memv = {ag_all};
  let st = {ags_memv,src,0b10101,0b0};
}

class AIE2_dmhb_lda_inst_lda < bits<2> op, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> mRa;

  field bits<9> ag_nospill = 0;

  let lda = {ag_nospill, op, 0b00, mRa, 0b111};
}

class AIE2_dms_lda_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<13> ag_all;
  bits<7> mLdaScl;

  let lda = {ag_all, mLdaScl, 0b1};
}

class AIE2_aga_mems_dms_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_dms_lda_inst_lda <outs, ins, pattern, opcodestr, argstr> {
  field bits<13> ag_all;
}

class AIE2_dmw_lda_am_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<6> dst;
  bits<13> ag_all;
  field bits<13> aga_memw = {ag_all};

  let lda = {aga_memw,dst,0b10};
}

class AIE2_dmv_lda_q_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<2> dst;
  bits<13> ag_all;
  field bits<13> aga_memv = {ag_all};

  let lda = {aga_memv,dst,0b00001, 0b1};
}

class AIE2_inst_ldb_instr32 < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_instr32_me <outs, ins, pattern, opcodestr, argstr> {
  bits<16> ldb;

  let instr32 = {0b00111,ldb,0b0000001};
  let Slot = ldb_slot;
}

class AIE2_dmw_ldb_inst_ldb < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_ldb_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> dst;
  bits<9> ag_nospill;
  field bits<9> aga_memw = {ag_nospill};

  let ldb = {aga_memw,dst,0b01};
}


class AIE2_dmv_ldb_ptr_inst_ldb < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_ldb_instr32 <outs, ins, pattern, opcodestr, argstr> {

  bits<5> dst;
  bits<8> ag_no_imm;
  field bits<8> agb_memw_no_imm = {ag_no_imm};

  let ldb = {agb_memw_no_imm,0b0,dst,0b11};
}

class AIE2_lda_ptr_inc_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<19> aga_sa;

  let lda = {aga_sa, 0b10};
}

class AIE2_st_ptr_inc_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<19> ags_sa;

  let st = {ags_sa,0b01};
}

class AIE2_ldb_ptr_inc_inst_ldb < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_ldb_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<16> agb_sa;

  let ldb = {agb_sa};
}

class AIE2_dmv_lda_w_ptr_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<3> ptr;
  bits<5> dst;

  let lda = {ptr,0b1000000000,dst,0b111};
}

class AIE2_dmw_ldb_compr_inst_ldb < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_ldb_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<16> dmw_ldb_compr;

  let ldb = {dmw_ldb_compr};
}

class AIE2_tm_sts_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  field bits<1> dontcare1;
  bits<3> ptr;
  bits<7> mSclSt;

  let st = {ptr,0b1,dontcare1,0b0111,0b1000,mSclSt,0b0};
}

class AIE2_alu_cg_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<11> i;
  bits<6> dst;

  let alu = {i{10-6},dst,i{5-0},0b01,0b0};
}

class AIE2_alu_dstep_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<15> alu_dstep;
  bits<5> d0;
  bits<5> s0;
  bits<5> s1;

  let alu = {s0,d0,s1,0b1010,0b0};
}

class AIE2_alu_r_r_inst_alu < bits<4> op, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<14> alu_r_r;
  bits<5> mRx;
  bits<4> eUniArith;
  bits<5> mRx0;

  let alu = {mRx0,mRx,op,0b11100,0b0};
}

class AIE2_alu_r_rr_inst_alu < bits<4> op, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<19> alu_r_rr;
  bits<5> mRx;
  bits<4> eBinArith;
  bits<5> mRx0;
  bits<5> mRy;

  let alu = {mRx0,mRx,mRy,op,0b1};
}

class AIE2_inst_lng_instr48 < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_instr48_me <outs, ins, pattern, opcodestr, argstr> {
  bits<42> lng;

  let instr48 = {lng,0b010};
  let Slot = lng_slot;
}

class AIE2_lng_cg_lng < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lng_instr48 <outs, ins, pattern, opcodestr, argstr> {
  bits<32> i;
  bits<7> mMvSclDstCg;

  let lng = {i{31-12},mMvSclDstCg,i{11-0},0b001};
}

class AIE2_inst_mv_instr32 < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_instr32_me <outs, ins, pattern, opcodestr, argstr> {
  bits<22> mv;

  let instr32 = {0b00011,mv,0b1};
  let Slot = mv_slot;
}

class AIE2_mv_add_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<6> imm;
  bits<7> dst;
  bits<5> s0;
  let mv = {dst,s0,imm{5-2},0b11,imm{1-0},0b11};
}

class AIE2_mv_cg_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<10> i;
  bits<7> dst;
  let mv = {dst,i{9-1},0b11,i{0},0b1,0b00};
}

class AIE2_mv_delay_inst_mv <bits<3> delay, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {

  bits<5> dst;
  bits<7> src;
  field bits<15> mv_delay = {dst,delay{2-1},src,delay{0}};
  let mv = {mv_delay,0b1,0b1100,0b10};
}

class AIE2_dmv_ldb_4x_inst_ldb <bits<2> sz, bits<1> off, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_ldb_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> src;
  bits<5> dst;

  field bits<16> dmv_ldb_4x = {src, off, sz, 0b1, dst, 0b11};
  let ldb = {dmv_ldb_4x};
}

class AIE2_dmw_lda_ups_w2b_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<9> aga_memw_ups;
  bits<9> ag_nospill = 0;
  bits<5> dst;
  bits<4> lda_ups_base;
  bits<4> mv_ups_base = 0;
  let aga_memw_ups = {ag_nospill};
  let lda_ups_base = {mv_ups_base};
  let lda = {aga_memw_ups,lda_ups_base,dst,0b100};
}

class AIE2_dmw_lda_ups_w2c_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<9> aga_memw_ups;
  bits<9> ag_nospill = 0;
  bits<4> dst;
  bits<4> lda_ups_base;
  bits<4> mv_ups_base = 0;
  let aga_memw_ups = {ag_nospill};
  let lda_ups_base = {mv_ups_base};
  let lda = {aga_memw_ups,lda_ups_base{3-1},0b1,dst,lda_ups_base{0},0b000};
}

class AIE2_dmw_lda_w_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> dst;
  bits<13> ag_all;
  field bits<13> aga_memw = {ag_all};

  let lda = {aga_memw,dst,0b111};
}

class AIE2_dmw_lda_ups_bf_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  field bits<1> dontcare1;
  bits<5> dst;
  bits<9> ags_memw_ups;
  bits<9> ag_nospill = 0;
  let ags_memw_ups = {ag_nospill};

  let lda = {ags_memw_ups, dontcare1, 0b010, dst, 0b000};
}

class AIE2_dmw_ldb_sparse_inst_ldb < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_ldb_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<16> dmw_ldb_sparse;

  let ldb = {dmw_ldb_sparse};
}

class AIE2_dmw_ldb_sparse_op_inst_ldb <bits<2> op, bits<2> smode, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_dmw_ldb_sparse_inst_ldb <outs, ins, pattern, opcodestr, argstr> {
  bits<3> ptr;
  bits<3> dst;
  field bits<1> dontcare1;
  let dmw_ldb_sparse = {ptr, op, smode, 0b1000, dst, dontcare1, 0b0};

  let ldb = {dmw_ldb_sparse};
}

class AIE2_dmw_ldb_sparse_init_inst_ldb <bits<2> op, bits<2> smode, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_dmw_ldb_sparse_inst_ldb <outs, ins, pattern, opcodestr, argstr> {
  bits<3> ptr;
  field bits<3> dontcare3;
  field bits<1> dontcare1;
  let dmw_ldb_sparse = {ptr, op, smode, 0b1000, dontcare3, dontcare1, 0b0};

  let ldb = {dmw_ldb_sparse};
}

class AIE2_dmw_ldb_unpack_inst_ldb < bits<1> sgn, bits<1> sz, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_ldb_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst;
  bits<8> ag_no_imm;
  field bits<8> agb_memw_noimm = {ag_no_imm};
  field bits<16> dmw_ldb_unpack = {agb_memw_noimm, 0b1, dst, sgn, sz, 0b0};
  let ldb = {dmw_ldb_unpack};
}

class AIE2_dmw_sts_am_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<6> src;
  bits<13> ag_all;
  field bits<13> ags_memw = {ag_all};

  let st = {ags_memw,src,0b01};
}

class AIE2_dmw_sts_srs_bf_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  field bits<1> dontcare1;
  bits<5> src;
  bits<9> ags_memw_pack;
  bits<9> ag_nospill = 0;
  let ags_memw_pack = {ag_nospill};

  let st = {ags_memw_pack, dontcare1, 0b100, src, 0b111};
}

class AIE2_dmw_sts_w_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> src;
  bits<13> ag_all;
  field bits<13> ags_memw = {ag_all};

  let st = {ags_memw,src,0b111};
}

class AIE2_done_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {

  let alu = {0b0000,0b0000,0b000000,0b01000,0b0};
}

class AIE2_event_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<2> eEvent;

  let alu = {eEvent,0b0000,0b0000,0b0000,0b10000,0b0};
}

class AIE2_instr80_me < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  field bits<80> SoftFail = 0;
  field bits<80> Inst;
  let Size = 10;
   field bits<76> instr80;

  let Inst = {instr80,0b1011};
}

class AIE2_inst_vec_instr32 < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_instr32_me <outs, ins, pattern, opcodestr, argstr> {
  bits<26> vec;

  let instr32 = {vec,0b00};
  let Slot = vec_slot;
}

class AIE2_instr128_me < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  field bits<128> SoftFail = 0;
  field bits<128> Inst;
  let Size = 16;
   field bits<127> instr128;
  bits<21> lda;
  bits<16> ldb;
  bits<43> alu_mv;
  bits<21> st;
  bits<26> vec;

  let Inst = {ldb,lda,st,alu_mv,vec,0b0};
  let Slot = unknown_slot;
}

class AIE2_instr16_me < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2Inst <outs, ins, opcodestr, argstr, pattern> {
  field bits<16> SoftFail = 0;
  field bits<16> Inst;
  field bits<12> instr16;
  let Size = 2;

  let Inst = {instr16,0b0001};
}

class AIE2_inst_nop_instr16 < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_instr16_me <outs, ins, pattern, opcodestr, argstr> {
  field bits<12> dontcare12;

  field bits<12> nop16 = {dontcare12};
  let instr16 = {nop16};
  let Slot = nop16_slot;
}

class AIE2_jump_imm_lng < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lng_instr48 <outs, ins, pattern, opcodestr, argstr> {
   field bits<20> jump_imm;
  bits<20> cpmaddr;

  let lng = {0b00000,cpmaddr,0b0,0b0000,0b0000,0b00000,0b010};
}

class AIE2_jump_ind_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<3> jump_ind;
  bits<3> mPm;

  let alu = {0b0000000000,mPm,0b000100,0b0};
}

class AIE2_jump_link_imm_lng < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lng_instr48 <outs, ins, pattern, opcodestr, argstr> {
  field bits<20> jump_link_imm;
  bits<20> cpmaddr;

  let lng = {0b00000,cpmaddr,0b0,0b0000,0b0000,0b00000,0b100};
}

class AIE2_jump_link_ind_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<3> jump_link_ind;
  bits<3> mPm;

  let alu = {0b0000000000,mPm,0b010100,0b0};
}

class AIE2_jump_return_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {

  let alu = {0b0000,0b0000,0b000000,0b11000,0b0};
}

class AIE2_jumpc_decr_ind_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<13> jumpc_decr_ind;
  bits<3> mPm;
  bits<5> mRx;
  bits<5> mRx0;

  let alu = {mRx0,mRx,mPm,0b001100,0b0};
}

class AIE2_jumpc_imm_lng < bits<1> op, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lng_instr48 <outs, ins, pattern, opcodestr, argstr> {
  field bits<26> jumpc_imm;
  bits<20> cpmaddr;
  bits<1> eOpBr;
  bits<5> mRx;

  let lng = {mRx,cpmaddr,op,0b0000,0b0000,0b00000,0b110};
}

class AIE2_lda_cg_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<20> lda_cg;
  bits<7> mLdaCg;
  bits<11> c11s;

  let lda = {c11s,0b00,mLdaCg,0b1};
}

class AIE2_mv_w_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<7> dst;
  bits<9> src;

  let mv = {dst,src,0b1110,0b10};
}

class AIE2_vacc_bf_inst_vec < bits<1> md2, bits<1> md1, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> c;
  bits<5> acc1;
  bits<5> acc2;
  bits<5> dst;
  field bits<2> dontcare2;

  field bits<25> vacc_bf = {c, acc1, md1, dontcare2, acc2, dst, md2, 0b1};
  let vec = {vacc_bf,0b0};
}

class AIE2_mv_x_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<6> dst;
  bits<9> src;

  let mv = {dst,0b0,src,0b1101,0b10};
}

class AIE2_mv_scd_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<6> dst;
  field bits<11> dontcare11;

  let lda = {dontcare11,0b00,dst,0b10};
}

class AIE2_mv_scdl_inst_lda < bits<1> hilo, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst;
  field bits<4> dontcare4;
  field bits<6> dontcare6;

  let lda = {dontcare6,0b111,dontcare4,dst,hilo,0b100};
}

class AIE2_mv_mcd_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<6> src;

  let st = {0b0000,0b11111,0b0000,src,0b1,0b0};
}

class AIE2_vec_broadcast_shuffle_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<2> vec_insert_src_0;
  bits<5> vec_insert_src_1;
  bits<5> dst; // OP_mShflDst
  field bits<2> dontcare2;

  let mv = {dst{4-1},vec_insert_src_0,0b0,vec_insert_src_1,dst{0},0b1,dontcare2,0b1111,0b01};
}

class AIE2_mv_cm_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst;
  bits<4> src;
  field bits<5> dontcare5;

  // Note: the dontcare5 is important, otherwise the disassembler generator
  // finds decoding conflicts which it cannot resolve.
  // Decoding Conflict:
  //                 00011....................1011001
  //                 .........................1011001
  //                 ................................
  //
  // MOV_mv_scl          00011______________1011001011001
  // VADD_32             00011____00000000________1011001
  // VMOV_mv_cm          00011____001____0000011011011001
  // VMOV_mv_w           00011________________11101011001
  // VMOV_mv_x           00011______0_________11011011001
  // VSUB_32             00011____00000010________1011001
  let mv = {dst,0b0,0b01,src,dontcare5,0b1101,0b10};
}

class AIE2_mDm_dms_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_dms_lda_inst_lda <outs, ins, pattern, opcodestr, argstr> {
   field bits<5> mDm;

  let ag_all = {mDm,0b10,0b10,0b10,0b10};
}

class AIE2_mv_scl2ms_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<7> src; // mSclMS
  bits<3> dst;
  field bits<1> dontcare1;

  let st = {dst, 0b0, dontcare1, 0b0111, 0b1000, src, 0b0};
}
class AIE2_mv_scl2ms_b_noTlast < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_mv_scl2ms_inst_st <outs, ins, pattern, opcodestr, argstr> {
  let dst = {0b000};
}
class AIE2_mv_scl2ms_nb_noTlast < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_mv_scl2ms_inst_st <outs, ins, pattern, opcodestr, argstr> {
  let dst = {0b100};
}
class AIE2_mv_scl2ms_b_doTlast < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_mv_scl2ms_inst_st <outs, ins, pattern, opcodestr, argstr> {
  let dst = {0b001};
}
class AIE2_mv_scl2ms_nb_doTlast < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_mv_scl2ms_inst_st <outs, ins, pattern, opcodestr, argstr> {
  let dst = {0b101};
}
class AIE2_mv_scl2ms_b_doTlast_reg < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_mv_scl2ms_inst_st <outs, ins, pattern, opcodestr, argstr> {
  let dst = {0b01, dontcare1};
}
class AIE2_mv_scl2ms_nb_doTlast_reg < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_mv_scl2ms_inst_st <outs, ins, pattern, opcodestr, argstr> {
  let dst = {0b11, dontcare1};
}

class AIE2_vec_extract_broadcast_inst_mv <bits<2> word_size,  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {

  bits<4> dst;
  bits<4> src;
  bits<6> mExtractIdx ;

  let mv = {dst,word_size,mExtractIdx,src,0b1110,0b01};
}

class AIE2_mv_float_to_int_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {

  bits<1> mFl2FxSrc_0;
  bits<6> mFl2FxSrc_1;
  bits<4> dst;
  bits<2> shft;
  field bits<4> dontcare4;
  field bits<19> mv_float_to_int = {dst,mFl2FxSrc_0,dontcare4,shft,0b00,mFl2FxSrc_1};

  let st = {mv_float_to_int,0b01};
}

class AIE2_mv_cntr2c_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<3> mv_cntr2c;
  bits<3> dst;

  let mv = {0b0000,dst,0b0,0b0000,0b0100,0b1100,0b10};
}

class AIE2_vaddmac_bf_inst_vec < bits<1> mdm, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> acc1;
  bits<5> acc2;
  bits<5> vmac_bm_core_0;
  bits<8> vmac_bm_core_1;

  field bits<25> vaddmac_bf = {vmac_bm_core_0, acc1, vmac_bm_core_1, acc2, mdm, 0b1};
  let vec = {vaddmac_bf,0b1};
}

class AIE2_vec_extract_inst_mv<bits<1> sign, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<2> vec_extract_dest_0;
  bits<7> vec_extract_dest_1;
  bits<2> idx;
  bits<4> s1;

  let mv = {vec_extract_dest_1,vec_extract_dest_0,sign,idx,s1,0b1101,0b01};
}

class AIE2_vec_abs_not_inst_mv < bits<5> op, bits<2> word_size, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<3> cmp;
  bits<4> d;
  bits<4> s1;
  field bits<4> dontcare4;

  let mv = {d,cmp,op,dontcare4,s1,word_size};
}

class AIE2_mv_ss2scl_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> mRa;
  bits<1> mSStream;

  let lda = {mSStream,0b000000000110,mRa,0b000};
}

class AIE2_mv_ss2scl_b < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_mv_ss2scl_inst_lda <outs, ins, pattern, opcodestr, argstr> {

  let mSStream = {0b0};
}

class AIE2_mv_ss2scl_nb < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_mv_ss2scl_inst_lda <outs, ins, pattern, opcodestr, argstr> {

  let mSStream = {0b1};
}

class AIE2_mv_w_srs1_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> dst;
  bits<9> mWlsrsh;
  field bits<14> mv_w_srs1 = {dst,mWlsrsh};
  let mv = {mv_w_srs1,0b00,0b1110,0b10};
}

class AIE2_vec_shift_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> d;
  bits<4> s1;
  bits<4> s2;
  bits<5> shift;

  let mv = {d,0b010,shift,s1,s2,0b11};
}

class AIE2_vec_shift_align_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> d;
  bits<2> pre;
  bits<4> s1;
  bits<4> s2;
  bits<5> shift;

  let mv = {d,pre,0b1,shift,s1,s2,0b11};
}

class AIE2_mv_x_srs_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst;
  bits<8> mXdlsrs;
  field bits<17> mv_x_srs = {dst, 0b0, 0b1111, mXdlsrs};
  let st = {mv_x_srs,0b001,0b0};
}

class AIE2_mul_r_rr_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<15> mul_r_rr;
  bits<5> mRx;
  bits<5> mRx0;
  bits<5> mRy;

  let alu = {mRx0,mRx,mRy,0b1111,0b1};
}

class AIE2_mv_ph2ms_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<18> mv_ph2ms;

  let st = {mv_ph2ms,0b100};
}

class AIE2_mv_scl_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<14> mv_scl;
  bits<7> mMvSclDst;
  bits<7> mMvSclSrc;

  let mv = {mMvSclDst,mMvSclSrc,0b10,0b1100,0b10};
}

class AIE2_mv_unpack_inst_ldb < bits<1> sgn, bits<1> sz, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_ldb_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst;
  bits<5> src;
  field bits<1> dontcare1;

  let ldb = {src,dontcare1,0b1,0b00,dst,sgn,sz,0b0};
}

class AIE2_mv_pack_inst_st < bits<1> sgn, bits<1> sz, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> dst;
  bits<4> src;

  let st = {dst, 0b1111, sgn, sz, 0b00, src, 0b101, 0b0};
}

class AIE2_mv_ups_w2b_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> mv_ups_base;
  bits<5> dst;
  bits<5> src;

  let mv = {dst,mv_ups_base,0b01,src,0b1111,0b10};
}

class AIE2_mv_ups_bf_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> dst;
  bits<5> src;

  let mv = {dst,0b0000,0b00,src,0b1111,0b10};
}

class AIE2_mv_ups_w2c_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> mv_ups_base;
  bits<4> dst;
  bits<5> src;

  let mv = {dst,0b0,mv_ups_base,0b11,src,0b1111,0b10};
}

class AIE2_mv_ups_x2c_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  field bits<1> dontcare;
  bits<4> mv_ups_base;
  bits<4> dst;
  bits<4> src;


  let mv = {dst,0b0,mv_ups_base,0b10,dontcare,src,0b1111,0b10};
}

class AIE2_mv_w_srs_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> dst;
  bits<10> mWsrs;
  field bits<19> mv_w_srs = {dst, 0b0111, mWsrs};

  let st = {mv_w_srs,0b1,0b0};
}

class AIE2_mv_w_srs_bf_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {
  field bits<1> dontcare1;
  field bits<4> dontcare4;
  bits<5> dst;
  bits<5> src;
  field bits<18> mv_w_srs_bf = {dst, dontcare4, dontcare1, 0b000, src};

  let st = {mv_w_srs_bf,0b111};
}

class AIE2_mvx_scl_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<9> mvx_scl;
   field bits<4> dontcare4;
  bits<4> mCRm;
  bits<5> mRx;

  let alu = {mRx,mCRm,dontcare4,0b1,0b00000,0b0};
}

class AIE2_nop_alu_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {

  let alu = {0b0000,0b0000,0b000000,0b00000,0b0};
}

class AIE2_nop_lda_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {

  let lda = {0b0000000000,0b000,0b00000,0b000};
}

class AIE2_nop_ldb_inst_ldb < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_ldb_instr32 <outs, ins, pattern, opcodestr, argstr> {

  let ldb = {0b000000000,0b0000000};
}

class AIE2_nop_lng_lng < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lng_instr48 <outs, ins, pattern, opcodestr, argstr> {

  let lng = {0b00000,0b00000,0b0000,0b0000,0b0000,0b0000,0b0000,0b0000,0b00000,0b000};
}

class AIE2_nop_mv_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {

  let mv = {0b0000,0b0000,0b0000,0b0000,0b1110,0b00};
}

class AIE2_nop_sts_inst_st < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_st_instr32 <outs, ins, pattern, opcodestr, argstr> {

  let st = {0b000000000,0b0000,0b00000,0b000};
}

class AIE2_nop_vec_inst_vec < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {

  let vec = {0b00000,0b0000,0b0000,0b0000,0b1111,0b0,0b0,0b0,0b0,0b0};
}


// Locks
class AIE2_mLockId {
  bits<7> mLockId;
}
class AIE2_mLockId_reg : AIE2_mLockId {
  bits<5> mRx;
  field bits<1> dontcare1;
  let mLockId = {mRx, dontcare1, 0b1};
}
class AIE2_mLockId_imm : AIE2_mLockId {
  bits<6> id;
  let mLockId = {id, 0b0};
}

class AIE2_acquire_inst_alu <dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
  : AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<7> mLockId;
  bits<5> mRy;
  field bits<1> dontcare1;

  let alu = {mLockId, dontcare1, 0b01, mRy, 0b0010, 0b0};
}

class AIE2_acquire_cond_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
  : AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<7> mLockId;
  bits<5> mRy;
  field bits<1> dontcare1;

  let alu = {mLockId, dontcare1, 0b11, mRy, 0b0010, 0b0};
}

class AIE2_release_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
  : AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<7> mLockId;
  bits<5> mRy;
  field bits<1> dontcare1;

  let alu = {mLockId, dontcare1, 0b00, mRy, 0b0010, 0b0};
}

class AIE2_release_cond_inst_alu < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
  : AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<7> mLockId;
  bits<5> mRy;
  field bits<1> dontcare1;

  let alu = {mLockId, dontcare1, 0b10, mRy, 0b0010, 0b0};
}

class AIE2_select_r_rr_inst_alu < bits<1> eSelect, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_alu_instr32 <outs, ins, pattern, opcodestr, argstr> {
   field bits<16> select_r_rr;
  bits<5> mRx;
  bits<5> mRx0;
  bits<5> mRy;

  let alu = {mRx0,mRx,mRy,eSelect,0b110,0b0};
}

class AIE2_tm_lda_inst_lda < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_lda_instr32 <outs, ins, pattern, opcodestr, argstr> {
  field bits<6> dontcare6;
  bits<3> ptr;
  bits<5> dst;

  let lda = {ptr,dontcare6,0b1110,dst,0b000};
}

class AIE2_vacc_inst_vec < bits<1> md2, bits<1> md1, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> c;
  bits<4> acc1;
  bits<4> acc2;
  bits<4> dst;

  field bits<25> vacc = {c, acc1, 0b0, 0b1110, acc2, dst, md2, md1, 0b0};
  let vec = {vacc,0b0};
}

class AIE2_vaddmac_inst_vec < bits<1> mdm, bits<1> md2, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> acc1; // this is also the "dst"
  bits<4> acc2;
  bits<5> vmac_cm_core_0;
  bits<8> vmac_cm_core_1;

  field bits<25> vaddmac = {vmac_cm_core_0, acc1, 0b1, vmac_cm_core_1, acc2, md2, mdm, 0b0};
  let vec = {vaddmac,0b0};
}

class AIE2_vclr_inst_vec < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst;
  field bits<5> dontcare5;
  field bits<4> dontcare4;

  field bits<25> vclr = {dontcare5, dontcare4, 0b0, 0b1111, 0b0000, dst, 0b1, 0b0, 0b0};
  let vec = {vclr,0b0};
}

class AIE2_vclr_bf_inst_vec < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> dst;
  field bits<5> dontcare5;

  field bits<25> vclr_bf = {dontcare5, 0b11111, 0b1111, 0b0000, dst, 0b0, 0b0};
  let vec = {vclr_bf,0b1};
}

class AIE2_vec_add_inst_mv < bits<5> op, bits<2> vec_size, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {

  bits<4> d;
  bits<4> s1;
  bits<4> s2;

  field bits<22> vec_add = {d,/*xxx*/0b000,op,s1,s2,vec_size};
  let mv = {vec_add};
}

class AIE2_vec_add_select_inst_mv < bits<5> op, bits<2> word_size,  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {

  bits<4> d;
  bits<3> sel;
  bits<4> s1;
  bits<4> s2;

  field bits<22> vec_add_select = {d,sel,op,s1,s2,word_size};
  let mv = {vec_add_select};
}

class AIE2_vec_bf_compare_inst_mv < bits<5> op, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  field bits<4> dontcare4;
  bits<3> cmp;
  bits<4> s1;
  bits<4> s2;
  field bits<22> vec_bf_compare = {dontcare4, cmp, op, s1, s2, 0b01};

  let mv = {vec_bf_compare};
}

class AIE2_vec_bf_min_max_inst_mv < bits<5> op, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> d;
  bits<3> cmp;
  bits<4> s1;
  bits<4> s2;
  field bits<22> vec_bf_min_max = {d, cmp, op, s1, s2, 0b01};

  let mv = {vec_bf_min_max};
}

class AIE2_vec_broadcast_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<2> vec_insert_src_0;
  bits<5> vec_insert_src_1;
  bits<4> dst;
  field bits<1> dontcare1;
  field bits<2> dontcare2;
  let mv = {dst,vec_insert_src_0,0b0,vec_insert_src_1,dontcare1,0b0,dontcare2,0b1111,0b01};
}

class AIE2_vec_compare_inst_mv < bits<5> op, bits<2> vec_size, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  field bits<4> dontcare4;
  bits<3> cmp;
  bits<4> s1;
  bits<4> s2;
  field bits<22> vec_compare = {dontcare4, cmp, op, s1, s2, vec_size};
  let mv = {vec_compare};
}

class AIE2_vec_eqz_inst_mv < bits<2> vec_size, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  field bits<4> dontcare4;
  bits<3> cmp;
  bits<4> s2;
  field bits<22> vec_eqz = {dontcare4, cmp, 0b01111, dontcare4, s2, vec_size};
  let mv = {vec_eqz};
}

class AIE2_vec_insert_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst;
  bits<2> vec_insert_src_0;
  bits<5> vec_insert_src_1;
  bits<4> s1;
  field bits<1> dontcare1;

  let mv = {dst,vec_insert_src_0,dontcare1,vec_insert_src_1,s1,0b1100,0b01};
}

class AIE2_vec_log_inst_mv < bits<5> op, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> d;
  bits<4> s1;
  bits<4> s2;
  field bits<3> dontcare3;

  let mv = {d,dontcare3,op,s1,s2,0b10};
}

class AIE2_vec_push_hi_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<16> vec_push_hi;
  let mv = {vec_push_hi,0b1111,0b01};
}

class AIE2_vec_push_lo_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<20> vec_push_lo;
  let mv = {vec_push_lo,0b11};
}

class AIE2_vec_shuffle_inst_mv < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> dst;
  bits<5> mod;
  bits<4> s1;
  bits<4> s2;

  let mv = {dst,0b00,mod,s1,s2,0b11};
}

class AIE2_vec_sub_min_max_inst_mv < bits<5> op, bits<2> vec_size, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_mv_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<3> cmp;
  bits<4> d;
  bits<4> s1;
  bits<4> s2;

  let mv = {d,cmp,op,s1,s2,vec_size};
}

class AIE2_vmac_inst_vec < bits<1> mdm, bits<1> md1, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> acc1;
  bits<4> dst;
  bits<5> vmac_cm_core_0;
  bits<8> vmac_cm_core_1;

  field bits<25> vmac = {vmac_cm_core_0, acc1, 0b0, vmac_cm_core_1, dst, mdm, md1, 0b0};

  let vec = {vmac,0b0};
}

class AIE2_vmac_bf_inst_vec < bits<1> mdm, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> acc1;
  bits<5> dst;
  bits<5> vmac_bm_core_0;
  bits<8> vmac_bm_core_1;

  field bits<25> vmac = {vmac_bm_core_0, acc1, vmac_bm_core_1, dst, mdm, 0b0};

  let vec = {vmac,0b1};
}

class AIE2_vmov_inst_vec < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> csrc;
  bits<4> cdst;
  field bits<5> dontcare5;
  field bits<25> vmov = {dontcare5, csrc, 0b0, 0b1111, 0b0000, cdst, 0b0, 0b0, 0b0};
  let vec = {vmov,0b0};
}

class AIE2_vmul_inst_vec < bits<1> mdm, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst;
  bits<5> vmac_cm_core_0;
  bits<8> vmac_cm_core_1;

  field bits<25> vmul = {vmac_cm_core_0, 0b1111, 0b0, vmac_cm_core_1, dst, mdm, 0b0, 0b0};

  let vec = {vmul,0b0};
}

class AIE2_vmul_bf_inst_vec < bits<1> mdm, dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> dst;
  bits<5> vmac_bm_core_0;
  bits<8> vmac_bm_core_1;

  field bits<25> vmul = {vmac_bm_core_0, 0b11111, vmac_bm_core_1, dst, mdm, 0b0};

  let vec = {vmul,0b1};
}

class AIE2_vneg_inst_vec < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<4> dst;
  bits<4> acc1;
  bits<5> c;

  field bits<25> vneg = {c, acc1, 0b1, 0b1111, 0b0000, dst, 0b0, 0b1, 0b0};
  let vec = {vneg,0b0};
}

class AIE2_vneg_bf_inst_vec < dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">:
    AIE2_inst_vec_instr32 <outs, ins, pattern, opcodestr, argstr> {
  bits<5> c;
  bits<5> acc1;
  bits<5> dst;

  field bits<25> vneg_bf = {c, acc1, 0b1111, 0b0000, dst, 0b0, 0b0};
  let vec = {vneg_bf,0b1};
}
