//===- AIECompositeFormats.td ------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
class AIECompositeInst<dag slot_ins>
  : AIEInst<(outs), slot_ins, "opcodestr", "argstr", []>
{
  let isComposite = true;
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
}

class AIE_instr128_Composite<dag slot_ins> : AIECompositeInst<slot_ins>
{
  bits<128> Inst;
  bits<127> instr128;
  let Inst = {instr128, 0b0};
  let Size = 16;
}

class AIE_instr32_Composite<dag slot_ins> : AIECompositeInst<slot_ins>
{
  bits<32> Inst;
  bits<29> instr32;
  let Inst = {instr32, 0b011};
  let Size = 4;
}

class AIE_instr96_Composite<dag slot_ins> : AIECompositeInst<slot_ins>
{
  bits<96> Inst;
  bits<92> instr96;
  let Inst = {instr96, 0b1111};
  let Size = 12;
}

class AIE_instr64_Composite<dag slot_ins> : AIECompositeInst<slot_ins>
{
  bits<64> Inst;
  bits<60> instr64;
  let Inst = {instr64, 0b0111};
  let Size = 8;
}

class AIE__instr128__lda__ldb__st__mv0__mv1__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__ldb__st__lng__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<28> lng_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<28> lng;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__ldb__alu__mv0__mv1__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__ldb__alu__lng__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<28> lng_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<28> lng;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__ldb__st__mv0__mv1ssb__alu__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<19> alu_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_med_vec;
  bits<17> ldb;
  bits<20> st;
  bits<16> mv0;
  bits<7> mv1_ssb;
  bits<19> alu;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_med_vec, 0b1};
  let V_med_vec = {ldb, 0b0 /*x*/, st, 0b00 /*xx*/, mv0, 0b0 /*x*/, mv1_ssb, alu, vec_med};
  let ldb = {ldb_base};
  let st = {st_base};
  let mv0 = {mv0_base};
  let mv1_ssb = {mv1ssb_base};
  let alu = {alu_base};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__ldb__st__alu__mv1__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<13> mv1_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__mv0__st__alu__mv1__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<13> mv1_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
}

class AIE__instr32__lng<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<28> lng_base;
  // Intermediate fields
  bits<29> i32_base;
  bits<28> lng;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {lng, 0b1};
  let lng = {lng_base};
}

class AIE__instr32__vecm<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<25> vecm_base;
  // Intermediate fields
  bits<29> i32_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {vec_med, 0b0010 /*xx10*/};
  let vec_med = {vecm_base};
}

class AIE__instr32__st<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  // Intermediate fields
  bits<29> i32_base;
  bits<21> i32_shrt;
  bits<21> alu_st;
  bits<20> st;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {i32_shrt, 0b0000 /*xxxx*/, 0b0000 /*xx00*/};
  let i32_shrt = {alu_st};
  let alu_st = {0b1, st};
  let st = {st_base};
}

class AIE__instr32__alu<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<19> alu_base;
  // Intermediate fields
  bits<29> i32_base;
  bits<21> i32_shrt;
  bits<21> alu_st;
  bits<19> alu;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {i32_shrt, 0b0000 /*xxxx*/, 0b0000 /*xx00*/};
  let i32_shrt = {alu_st};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
}

class AIE__instr32__ldb<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<17> ldb_base;
  // Intermediate fields
  bits<29> i32_base;
  bits<21> i32_shrt;
  bits<18> mv0_ldb;
  bits<17> ldb;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {i32_shrt, 0b0000 /*xxxx*/, 0b0000 /*xx00*/};
  let i32_shrt = {0b0, mv0_ldb, 0b11};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
}

class AIE__instr32__mv0<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<16> mv0_base;
  // Intermediate fields
  bits<29> i32_base;
  bits<21> i32_shrt;
  bits<18> mv0_ldb;
  bits<16> mv0;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {i32_shrt, 0b0000 /*xxxx*/, 0b0000 /*xx00*/};
  let i32_shrt = {0b0, mv0_ldb, 0b11};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
}

class AIE__instr32__lda<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  // Intermediate fields
  bits<29> i32_base;
  bits<21> i32_shrt;
  bits<18> i32_lda;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {i32_shrt, 0b0000 /*xxxx*/, 0b0000 /*xx00*/};
  let i32_shrt = {0b0, i32_lda, 0b01};
  let i32_lda = {lda};
}

class AIE__instr32__mv1<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<13> mv1_base;
  // Intermediate fields
  bits<29> i32_base;
  bits<21> i32_shrt;
  bits<13> mv1;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {i32_shrt, 0b0000 /*xxxx*/, 0b0000 /*xx00*/};
  let i32_shrt = {0b0, mv1, 0b0001011 /*xxx1011*/};
  let mv1 = {mv1_base};
}

class AIE__instr96__st__alu__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<19> alu_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_st;
  bits<20> st;
  bits<20> alu_mv0;
  bits<19> alu;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_st};
  let i96_st = {st, alu_mv0, 0b00000000001111 /*xxxxxxxxxx1111*/, vec_all};
  let st = {st_base};
  let alu_mv0 = {alu, 0b0};
  let alu = {alu_base};
  let vec_all = {veca_base};
}

class AIE__instr96__st__mv0__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<16> mv0_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_st;
  bits<20> st;
  bits<20> alu_mv0;
  bits<16> mv0;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_st};
  let i96_st = {st, alu_mv0, 0b00000000001111 /*xxxxxxxxxx1111*/, vec_all};
  let st = {st_base};
  let alu_mv0 = {mv0, 0b0001 /*xxx1*/};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__alu__mv0__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<35> i96_alu_mv0_vec;
  bits<19> alu;
  bits<16> mv0;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_alu_mv0_vec, 0b0};
  let i96_alu_mv0_vec = {alu, mv0};
  let alu = {alu_base};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__st__mv1__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<20> st_base;
  bits<13> mv1_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<20> st;
  bits<13> mv1;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__alu__mv1__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<13> mv1_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<19> alu;
  bits<13> mv1;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__ldb__mv0__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<16> mv0_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<33> i96_ldb_mv0;
  bits<17> ldb;
  bits<16> mv0;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_ldb_mv0, 0b011};
  let i96_ldb_mv0 = {ldb, mv0};
  let ldb = {ldb_base};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__mv0__mv1__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<13> mv1_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<31> i96_mv_all;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv_all, 0b00111 /*x0111*/};
  let i96_mv_all = {mv_all};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__lng__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<28> lng_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<31> i96_mv_all;
  bits<31> mv_all;
  bits<28> lng;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv_all, 0b00111 /*x0111*/};
  let i96_mv_all = {mv_all};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__ldb__st__alu__mv0<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<72> i96_alu_st_m0;
  bits<17> ldb;
  bits<20> st;
  bits<19> alu;
  bits<16> mv0;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_alu_st_m0, 0b0};
  let i96_alu_st_m0 = {ldb, st, alu, mv0};
  let ldb = {ldb_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv0 = {mv0_base};
}

class AIE__instr96__lda__ldb__st__alu__mv1<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<13> mv1_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_mv0_ldb;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_mv0_ldb, 0b011 /*x11*/};
  let i96_mv0_ldb = {mv0_ldb, st, alu, mv1};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
}

class AIE__instr96__lda__mv0__st__alu__mv1<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<13> mv1_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_mv0_ldb;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_mv0_ldb, 0b011 /*x11*/};
  let i96_mv0_ldb = {mv0_ldb, st, alu, mv1};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
}

class AIE__instr96__lda__ldb__st__mv0__mv1<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_move, 0b001 /*x01*/};
  let i96_move = {ldb, alu_st, 0b0 /*x*/, mv_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
}

class AIE__instr96__lda__ldb__st__lng<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<28> lng_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<28> lng;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_move, 0b001 /*x01*/};
  let i96_move = {ldb, alu_st, 0b0 /*x*/, mv_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
}

class AIE__instr96__lda__ldb__alu__mv0__mv1<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_move, 0b001 /*x01*/};
  let i96_move = {ldb, alu_st, 0b0 /*x*/, mv_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
}

class AIE__instr96__lda__ldb__alu__lng<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<28> lng_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<28> lng;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_move, 0b001 /*x01*/};
  let i96_move = {ldb, alu_st, 0b0 /*x*/, mv_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
}

class AIE__instr64__lda__ldb__st<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<40> i64_ldb;
  bits<17> ldb;
  bits<23> alu_st_mv;
  bits<21> alu_st;
  bits<20> st;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_ldb, 0b0};
  let i64_ldb = {ldb, alu_st_mv};
  let ldb = {ldb_base};
  let alu_st_mv = {alu_st, 0b00 /*x0*/};
  let alu_st = {0b1, st};
  let st = {st_base};
}

class AIE__instr64__lda__ldb__alu<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<40> i64_ldb;
  bits<17> ldb;
  bits<23> alu_st_mv;
  bits<21> alu_st;
  bits<19> alu;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_ldb, 0b0};
  let i64_ldb = {ldb, alu_st_mv};
  let ldb = {ldb_base};
  let alu_st_mv = {alu_st, 0b00 /*x0*/};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
}

class AIE__instr64__lda__ldb__mv0<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<16> mv0_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<40> i64_ldb;
  bits<17> ldb;
  bits<23> alu_st_mv;
  bits<17> mv;
  bits<16> mv0;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_ldb, 0b0};
  let i64_ldb = {ldb, alu_st_mv};
  let ldb = {ldb_base};
  let alu_st_mv = {mv, 0b000001 /*xxxxx1*/};
  let mv = {mv0, 0b0};
  let mv0 = {mv0_base};
}

class AIE__instr64__lda__ldb__mv1<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<13> mv1_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<40> i64_ldb;
  bits<17> ldb;
  bits<23> alu_st_mv;
  bits<17> mv;
  bits<13> mv1;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_ldb, 0b0};
  let i64_ldb = {ldb, alu_st_mv};
  let ldb = {ldb_base};
  let alu_st_mv = {mv, 0b000001 /*xxxxx1*/};
  let mv = {mv1, 0b0001 /*xx01*/};
  let mv1 = {mv1_base};
}

class AIE__instr64__lda__alu__st<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<20> st_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<39> i64_alu_st;
  bits<19> alu;
  bits<20> st;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_alu_st, 0b01};
  let i64_alu_st = {alu, st};
  let alu = {alu_base};
  let st = {st_base};
}

class AIE__instr64__lda__mv0__st<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<20> st_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<38> i64_as_mv;
  bits<17> mv;
  bits<16> mv0;
  bits<21> alu_st;
  bits<20> st;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_as_mv, 0b011};
  let i64_as_mv = {mv, alu_st};
  let mv = {mv0, 0b0};
  let mv0 = {mv0_base};
  let alu_st = {0b1, st};
  let st = {st_base};
}

class AIE__instr64__lda__mv0__alu<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<19> alu_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<38> i64_as_mv;
  bits<17> mv;
  bits<16> mv0;
  bits<21> alu_st;
  bits<19> alu;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_as_mv, 0b011};
  let i64_as_mv = {mv, alu_st};
  let mv = {mv0, 0b0};
  let mv0 = {mv0_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
}

class AIE__instr64__lda__mv1__st<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<13> mv1_base;
  bits<20> st_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<38> i64_as_mv;
  bits<17> mv;
  bits<13> mv1;
  bits<21> alu_st;
  bits<20> st;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_as_mv, 0b011};
  let i64_as_mv = {mv, alu_st};
  let mv = {mv1, 0b0001 /*xx01*/};
  let mv1 = {mv1_base};
  let alu_st = {0b1, st};
  let st = {st_base};
}

class AIE__instr64__lda__mv1__alu<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<13> mv1_base;
  bits<19> alu_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<38> i64_as_mv;
  bits<17> mv;
  bits<13> mv1;
  bits<21> alu_st;
  bits<19> alu;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_as_mv, 0b011};
  let i64_as_mv = {mv, alu_st};
  let mv = {mv1, 0b0001 /*xx01*/};
  let mv1 = {mv1_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
}

class AIE__instr64__lda__mv0__mv1<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<13> mv1_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {mv_all, 0b0000011111 /*xxxx011111*/};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
}

class AIE__instr64__lda__lng<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<28> lng_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<31> mv_all;
  bits<28> lng;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {mv_all, 0b0000011111 /*xxxx011111*/};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
}

class AIE__instr64__st__alu__mv0<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<56> i64_nold;
  bits<20> st;
  bits<19> alu;
  bits<17> mv;
  bits<16> mv0;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_nold, 0b111};
  let i64_nold = {st, alu, mv};
  let st = {st_base};
  let alu = {alu_base};
  let mv = {mv0, 0b0};
  let mv0 = {mv0_base};
}

class AIE__instr64__st__alu__mv1<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<19> alu_base;
  bits<13> mv1_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<56> i64_nold;
  bits<20> st;
  bits<19> alu;
  bits<17> mv;
  bits<13> mv1;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_nold, 0b111};
  let i64_nold = {st, alu, mv};
  let st = {st_base};
  let alu = {alu_base};
  let mv = {mv1, 0b0001 /*xx01*/};
  let mv1 = {mv1_base};
}

class AIE__instr64__st__mv0__mv1<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<52> i64_mv;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_mv, 0b0111111 /*x111111*/};
  let i64_mv = {alu_st, mv_all};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
}

class AIE__instr64__st__lng<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<28> lng_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<52> i64_mv;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<28> lng;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_mv, 0b0111111 /*x111111*/};
  let i64_mv = {alu_st, mv_all};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
}

class AIE__instr64__alu__mv0__mv1<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<52> i64_mv;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_mv, 0b0111111 /*x111111*/};
  let i64_mv = {alu_st, mv_all};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
}

class AIE__instr64__alu__lng<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<19> alu_base;
  bits<28> lng_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<52> i64_mv;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<28> lng;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_mv, 0b0111111 /*x111111*/};
  let i64_mv = {alu_st, mv_all};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
}

class AIE__instr64__st__veca<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<21> alu_st;
  bits<20> st;
  bits<37> vec_all;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {alu_st};
  let alu_st = {0b1, st};
  let st = {st_base};
  let vec_all = {veca_base};
}

class AIE__instr64__alu__veca<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<19> alu_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<21> alu_st;
  bits<19> alu;
  bits<37> vec_all;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {alu_st};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let vec_all = {veca_base};
}

class AIE__instr64__ldb__veca<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<17> ldb_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<37> vec_all;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv0_ldb, 0b11};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let vec_all = {veca_base};
}

class AIE__instr64__mv0__veca<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<16> mv0_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<37> vec_all;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv0_ldb, 0b11};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
}

class AIE__instr64__lda__veca<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<37> veca_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<18> i32_lda;
  bits<37> vec_all;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, i32_lda, 0b01};
  let i32_lda = {lda};
  let vec_all = {veca_base};
}

class AIE__instr64__mv1__veca<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<13> mv1_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<13> mv1;
  bits<37> vec_all;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv1, 0b0001011 /*xxx1011*/};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__ldb__st__mv0__mv1ssb__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__ldb__st__mv0__mv1ssb__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__ldb__st__mv0__mv1ssb__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__ldb__st__mv0__mv1__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__ldb__st__mv0__mv1__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__ldb__st__lng__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<28> lng_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<28> lng;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__ldb__st__lng__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<28> lng_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<28> lng;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__ldb__alu__mv0__mv1ssb__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__ldb__alu__mv0__mv1ssb__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__ldb__alu__mv0__mv1ssb__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__ldb__alu__mv0__mv1__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__ldb__alu__mv0__mv1__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<13> mv1_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__ldb__alu__lng__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<28> lng_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<28> lng;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__ldb__alu__lng__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<28> lng_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<28> lng;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_move, 0b0};
  let V_move = {ldb, alu_st, 0b0 /*x*/, mv_all, 0b1, vec_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__ldb__st__mv0__mv1ssb__alu__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<19> alu_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_med_vec;
  bits<17> ldb;
  bits<20> st;
  bits<16> mv0;
  bits<7> mv1_ssb;
  bits<19> alu;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_med_vec, 0b1};
  let V_med_vec = {ldb, 0b0 /*x*/, st, 0b00 /*xx*/, mv0, 0b0 /*x*/, mv1_ssb, alu, vec_med};
  let ldb = {ldb_base};
  let st = {st_base};
  let mv0 = {mv0_base};
  let mv1_ssb = {mv1ssb_base};
  let alu = {alu_base};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__ldb__st__alu__mv1ssb__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__ldb__st__alu__mv1ssb__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__ldb__st__alu__mv1ssb__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__ldb__st__alu__mv1__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<13> mv1_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__ldb__st__alu__mv1__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<13> mv1_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__mv0__st__alu__mv1ssb__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__mv0__st__alu__mv1ssb__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr128__lda__mv0__st__alu__mv1ssb__veca<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
}

class AIE__instr128__lda__mv0__st__alu__mv1__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<13> mv1_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr128__lda__mv0__st__alu__mv1__vecm<dag slot_ins>
  : AIE_instr128_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<13> mv1_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<109> V_formats;
  bits<108> V_alu_st;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr128 = {lda, V_formats};
  let V_formats = {V_alu_st, 0b0};
  let V_alu_st = {mv0_ldb, st, alu, mv1, 0b0, vec_all};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr32__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<29> i32_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {vec_med, 0b0010 /*xx10*/};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr32__mv1ssb<dag slot_ins>
  : AIE_instr32_Composite<slot_ins>
{
  // Base slots
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<29> i32_base;
  bits<21> i32_shrt;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr32 = {i32_base};
  let i32_base = {i32_shrt, 0b0000 /*xxxx*/, 0b0000 /*xx00*/};
  let i32_shrt = {0b0, mv1, 0b0001011 /*xxx1011*/};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr96__st__alu__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<19> alu_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_st;
  bits<20> st;
  bits<20> alu_mv0;
  bits<19> alu;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_st};
  let i96_st = {st, alu_mv0, 0b00000000001111 /*xxxxxxxxxx1111*/, vec_all};
  let st = {st_base};
  let alu_mv0 = {alu, 0b0};
  let alu = {alu_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__st__alu__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<19> alu_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_st;
  bits<20> st;
  bits<20> alu_mv0;
  bits<19> alu;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_st};
  let i96_st = {st, alu_mv0, 0b00000000001111 /*xxxxxxxxxx1111*/, vec_all};
  let st = {st_base};
  let alu_mv0 = {alu, 0b0};
  let alu = {alu_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__st__mv0__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<16> mv0_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_st;
  bits<20> st;
  bits<20> alu_mv0;
  bits<16> mv0;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_st};
  let i96_st = {st, alu_mv0, 0b00000000001111 /*xxxxxxxxxx1111*/, vec_all};
  let st = {st_base};
  let alu_mv0 = {mv0, 0b0001 /*xxx1*/};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__st__mv0__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<16> mv0_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_st;
  bits<20> st;
  bits<20> alu_mv0;
  bits<16> mv0;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_st};
  let i96_st = {st, alu_mv0, 0b00000000001111 /*xxxxxxxxxx1111*/, vec_all};
  let st = {st_base};
  let alu_mv0 = {mv0, 0b0001 /*xxx1*/};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__alu__mv0__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<35> i96_alu_mv0_vec;
  bits<19> alu;
  bits<16> mv0;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_alu_mv0_vec, 0b0};
  let i96_alu_mv0_vec = {alu, mv0};
  let alu = {alu_base};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__lda__alu__mv0__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<35> i96_alu_mv0_vec;
  bits<19> alu;
  bits<16> mv0;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_alu_mv0_vec, 0b0};
  let i96_alu_mv0_vec = {alu, mv0};
  let alu = {alu_base};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__st__mv1ssb__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<20> st_base;
  bits<7> mv1ssb_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<20> st;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__lda__st__mv1ssb__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<20> st_base;
  bits<7> mv1ssb_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<20> st;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__st__mv1ssb__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<20> st_base;
  bits<7> mv1ssb_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<20> st;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__st__mv1__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<20> st_base;
  bits<13> mv1_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<20> st;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__lda__st__mv1__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<20> st_base;
  bits<13> mv1_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<20> st;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__alu__mv1ssb__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__lda__alu__mv1ssb__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__alu__mv1ssb__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__alu__mv1__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<13> mv1_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<19> alu;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__lda__alu__mv1__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<19> alu_base;
  bits<13> mv1_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<34> i96_mv1;
  bits<21> alu_st;
  bits<19> alu;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv1, 0b01};
  let i96_mv1 = {alu_st, mv1};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__ldb__mv0__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<16> mv0_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<33> i96_ldb_mv0;
  bits<17> ldb;
  bits<16> mv0;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_ldb_mv0, 0b011};
  let i96_ldb_mv0 = {ldb, mv0};
  let ldb = {ldb_base};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__lda__ldb__mv0__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<16> mv0_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<33> i96_ldb_mv0;
  bits<17> ldb;
  bits<16> mv0;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_ldb_mv0, 0b011};
  let i96_ldb_mv0 = {ldb, mv0};
  let ldb = {ldb_base};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__mv0__mv1ssb__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<31> i96_mv_all;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv_all, 0b00111 /*x0111*/};
  let i96_mv_all = {mv_all};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__lda__mv0__mv1ssb__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<31> i96_mv_all;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv_all, 0b00111 /*x0111*/};
  let i96_mv_all = {mv_all};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__mv0__mv1ssb__veca<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<31> i96_mv_all;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv_all, 0b00111 /*x0111*/};
  let i96_mv_all = {mv_all};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
}

class AIE__instr96__lda__mv0__mv1__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<13> mv1_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<31> i96_mv_all;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv_all, 0b00111 /*x0111*/};
  let i96_mv_all = {mv_all};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__lda__mv0__mv1__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<13> mv1_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<31> i96_mv_all;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv_all, 0b00111 /*x0111*/};
  let i96_mv_all = {mv_all};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__lng__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<28> lng_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<31> i96_mv_all;
  bits<31> mv_all;
  bits<28> lng;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv_all, 0b00111 /*x0111*/};
  let i96_mv_all = {mv_all};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr96__lda__lng__vecm<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<28> lng_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<91> i96_vec;
  bits<91> i96_lda;
  bits<36> i96_fmt;
  bits<31> i96_mv_all;
  bits<31> mv_all;
  bits<28> lng;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr96 = {i96_vec, 0b0};
  let i96_vec = {i96_lda};
  let i96_lda = {lda, i96_fmt, vec_all};
  let i96_fmt = {i96_mv_all, 0b00111 /*x0111*/};
  let i96_mv_all = {mv_all};
  let mv_all = {0b000 /*0xx*/, lng};
  let lng = {lng_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr96__lda__ldb__st__alu__mv1ssb<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_mv0_ldb;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_mv0_ldb, 0b011 /*x11*/};
  let i96_mv0_ldb = {mv0_ldb, st, alu, mv1};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr96__lda__mv0__st__alu__mv1ssb<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<20> st_base;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_mv0_ldb;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<20> st;
  bits<19> alu;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_mv0_ldb, 0b011 /*x11*/};
  let i96_mv0_ldb = {mv0_ldb, st, alu, mv1};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let st = {st_base};
  let alu = {alu_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr96__lda__ldb__st__mv0__mv1ssb<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<20> st_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_move, 0b001 /*x01*/};
  let i96_move = {ldb, alu_st, 0b0 /*x*/, mv_all};
  let ldb = {ldb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr96__lda__ldb__alu__mv0__mv1ssb<dag slot_ins>
  : AIE_instr96_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<91> i96_novec;
  bits<73> i96_nv_fmt;
  bits<70> i96_move;
  bits<17> ldb;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr96 = {i96_novec, 0b1};
  let i96_novec = {lda, i96_nv_fmt};
  let i96_nv_fmt = {i96_move, 0b001 /*x01*/};
  let i96_move = {ldb, alu_st, 0b0 /*x*/, mv_all};
  let ldb = {ldb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr64__lda__ldb__mv1ssb<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<17> ldb_base;
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<40> i64_ldb;
  bits<17> ldb;
  bits<23> alu_st_mv;
  bits<17> mv;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_ldb, 0b0};
  let i64_ldb = {ldb, alu_st_mv};
  let ldb = {ldb_base};
  let alu_st_mv = {mv, 0b000001 /*xxxxx1*/};
  let mv = {mv1, 0b0001 /*xx01*/};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr64__lda__mv1ssb__st<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<7> mv1ssb_base;
  bits<20> st_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<38> i64_as_mv;
  bits<17> mv;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<21> alu_st;
  bits<20> st;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_as_mv, 0b011};
  let i64_as_mv = {mv, alu_st};
  let mv = {mv1, 0b0001 /*xx01*/};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let alu_st = {0b1, st};
  let st = {st_base};
}

class AIE__instr64__lda__mv1ssb__alu<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<7> mv1ssb_base;
  bits<19> alu_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<38> i64_as_mv;
  bits<17> mv;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<21> alu_st;
  bits<19> alu;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {i64_as_mv, 0b011};
  let i64_as_mv = {mv, alu_st};
  let mv = {mv1, 0b0001 /*xx01*/};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
}

class AIE__instr64__lda__mv0__mv1ssb<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<59> i64_lda;
  bits<41> i64_lda_fmts;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_lda};
  let i64_lda = {lda, i64_lda_fmts};
  let i64_lda_fmts = {mv_all, 0b0000011111 /*xxxx011111*/};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr64__st__alu__mv1ssb<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<19> alu_base;
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<56> i64_nold;
  bits<20> st;
  bits<19> alu;
  bits<17> mv;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_nold, 0b111};
  let i64_nold = {st, alu, mv};
  let st = {st_base};
  let alu = {alu_base};
  let mv = {mv1, 0b0001 /*xx01*/};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr64__st__mv0__mv1ssb<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<52> i64_mv;
  bits<21> alu_st;
  bits<20> st;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_mv, 0b0111111 /*x111111*/};
  let i64_mv = {alu_st, mv_all};
  let alu_st = {0b1, st};
  let st = {st_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr64__alu__mv0__mv1ssb<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<19> alu_base;
  bits<16> mv0_base;
  bits<7> mv1ssb_base;
  // Intermediate fields
  bits<59> i64_novec;
  bits<52> i64_mv;
  bits<21> alu_st;
  bits<19> alu;
  bits<31> mv_all;
  bits<30> mv_parallel;
  bits<16> mv0;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  // Hierarchical definition
  let instr64 = {i64_novec, 0b1};
  let i64_novec = {i64_mv, 0b0111111 /*x111111*/};
  let i64_mv = {alu_st, mv_all};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let mv_all = {0b1, mv_parallel};
  let mv_parallel = {mv0, 0b0 /*x*/, mv1};
  let mv0 = {mv0_base};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
}

class AIE__instr64__st__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<21> alu_st;
  bits<20> st;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {alu_st};
  let alu_st = {0b1, st};
  let st = {st_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr64__st__vecm<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<20> st_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<21> alu_st;
  bits<20> st;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {alu_st};
  let alu_st = {0b1, st};
  let st = {st_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr64__alu__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<19> alu_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<21> alu_st;
  bits<19> alu;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {alu_st};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr64__alu__vecm<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<19> alu_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<21> alu_st;
  bits<19> alu;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {alu_st};
  let alu_st = {0b0, alu, 0b0};
  let alu = {alu_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr64__ldb__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<17> ldb_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv0_ldb, 0b11};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr64__ldb__vecm<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<17> ldb_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<18> mv0_ldb;
  bits<17> ldb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv0_ldb, 0b11};
  let mv0_ldb = {ldb, 0b1};
  let ldb = {ldb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr64__mv0__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<16> mv0_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv0_ldb, 0b11};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr64__mv0__vecm<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<16> mv0_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<18> mv0_ldb;
  bits<16> mv0;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv0_ldb, 0b11};
  let mv0_ldb = {mv0, 0b00};
  let mv0 = {mv0_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr64__lda__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<18> i32_lda;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, i32_lda, 0b01};
  let i32_lda = {lda};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr64__lda__vecm<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<18> lda;
  bits<25> vecm_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<18> i32_lda;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, i32_lda, 0b01};
  let i32_lda = {lda};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr64__mv1ssb__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<7> mv1ssb_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv1, 0b0001011 /*xxx1011*/};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr64__mv1ssb__vecm<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<7> mv1ssb_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv1, 0b0001011 /*xxx1011*/};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

class AIE__instr64__mv1ssb__veca<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<7> mv1ssb_base;
  bits<37> veca_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<13> mv1;
  bits<13> mv1_base;
  bits<7> mv1_ssb;
  bits<37> vec_all;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv1, 0b0001011 /*xxx1011*/};
  let mv1 = {mv1_base};
  let mv1_base = {mv1_ssb, 0b000000 /*x00000*/};
  let mv1_ssb = {mv1ssb_base};
  let vec_all = {veca_base};
}

class AIE__instr64__mv1__vec_shft__vec_strm__vec_short<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<13> mv1_base;
  bits<6> vec_shft;
  bits<5> vec_strm;
  bits<11> vec_short;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  bits<25> vecm_base;
  bits<22> vec_dpd;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv1, 0b0001011 /*xxx1011*/};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
  let vecm_base = {0b000, vec_dpd};
  let vec_dpd = {vec_strm, vec_shft, vec_short};
}

class AIE__instr64__mv1__vecm<dag slot_ins>
  : AIE_instr64_Composite<slot_ins>
{
  // Base slots
  bits<13> mv1_base;
  bits<25> vecm_base;
  // Intermediate fields
  bits<58> i64_vec;
  bits<21> i32_shrt;
  bits<13> mv1;
  bits<37> vec_all;
  bits<37> veca_base;
  bits<25> vec_med;
  // Hierarchical definition
  let instr64 = {i64_vec, 0b00};
  let i64_vec = {i32_shrt, vec_all};
  let i32_shrt = {0b0, mv1, 0b0001011 /*xxx1011*/};
  let mv1 = {mv1_base};
  let vec_all = {veca_base};
  let veca_base = {0b00000000 /*0000xxxx*/, 0b0000 /*xxxx*/, vec_med};
  let vec_med = {vecm_base};
}

// -- Composite Instructions Definitions of AIE --

// instr32 Composite Instructions
// NOTE: These formats can't be instantiated right now as they conflict with
// the instr32's standalone instructions encoding.
// Modifications needed (low-priority as it is just to uniformize the
// MC-Layer):
// 1) Modify the encoding of the standalone instr32 to set at 0 every bit which
//    are not part of a slot.
// 2) Extend the packetization logic for instr32 based instructions
/*
def I32_LNG : AIE__instr32__lng<(ins lng_slot:$lng_base)>;
def I32_VECM : AIE__instr32__vecm<(ins vecm_slot:$vecm_base)>;
def I32_ST : AIE__instr32__st<(ins st_slot:$st_base)>;
def I32_ALU : AIE__instr32__alu<(ins alu_slot:$alu_base)>;
def I32_LDB : AIE__instr32__ldb<(ins ldb_slot:$ldb_base)>;
def I32_MV0 : AIE__instr32__mv0<(ins mv0_slot:$mv0_base)>;
def I32_LDA : AIE__instr32__lda<(ins lda_slot:$lda)>;
def I32_MV1 : AIE__instr32__mv1<(ins mv1_slot:$mv1_base)>;
*/
// Extended instr32 Composite Instructions
/*
def I32_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr32__vec_shft__vec_strm__vec_short<(ins vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I32_MV1SSB : AIE__instr32__mv1ssb<(ins mv1ssb_slot:$mv1ssb_base)>;
*/

// instr64 Composite Instructions
def I64_LDA_LDB_ST : AIE__instr64__lda__ldb__st<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base)>;
def I64_LDA_LDB_ALU : AIE__instr64__lda__ldb__alu<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base)>;
def I64_LDA_LDB_MV0 : AIE__instr64__lda__ldb__mv0<(ins lda_slot:$lda, ldb_slot:$ldb_base, mv0_slot:$mv0_base)>;
def I64_LDA_LDB_MV1 : AIE__instr64__lda__ldb__mv1<(ins lda_slot:$lda, ldb_slot:$ldb_base, mv1_slot:$mv1_base)>;
def I64_LDA_ALU_ST : AIE__instr64__lda__alu__st<(ins lda_slot:$lda, alu_slot:$alu_base, st_slot:$st_base)>;
def I64_LDA_MV0_ST : AIE__instr64__lda__mv0__st<(ins lda_slot:$lda, mv0_slot:$mv0_base, st_slot:$st_base)>;
def I64_LDA_MV0_ALU : AIE__instr64__lda__mv0__alu<(ins lda_slot:$lda, mv0_slot:$mv0_base, alu_slot:$alu_base)>;
def I64_LDA_MV1_ST : AIE__instr64__lda__mv1__st<(ins lda_slot:$lda, mv1_slot:$mv1_base, st_slot:$st_base)>;
def I64_LDA_MV1_ALU : AIE__instr64__lda__mv1__alu<(ins lda_slot:$lda, mv1_slot:$mv1_base, alu_slot:$alu_base)>;
def I64_LDA_MV0_MV1 : AIE__instr64__lda__mv0__mv1<(ins lda_slot:$lda, mv0_slot:$mv0_base, mv1_slot:$mv1_base)>;
def I64_LDA_LNG : AIE__instr64__lda__lng<(ins lda_slot:$lda, lng_slot:$lng_base)>;
def I64_ST_ALU_MV0 : AIE__instr64__st__alu__mv0<(ins st_slot:$st_base, alu_slot:$alu_base, mv0_slot:$mv0_base)>;
def I64_ST_ALU_MV1 : AIE__instr64__st__alu__mv1<(ins st_slot:$st_base, alu_slot:$alu_base, mv1_slot:$mv1_base)>;
def I64_ST_MV0_MV1 : AIE__instr64__st__mv0__mv1<(ins st_slot:$st_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base)>;
def I64_ST_LNG : AIE__instr64__st__lng<(ins st_slot:$st_base, lng_slot:$lng_base)>;
def I64_ALU_MV0_MV1 : AIE__instr64__alu__mv0__mv1<(ins alu_slot:$alu_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base)>;
def I64_ALU_LNG : AIE__instr64__alu__lng<(ins alu_slot:$alu_base, lng_slot:$lng_base)>;
def I64_ST_VECA : AIE__instr64__st__veca<(ins st_slot:$st_base, veca_slot:$veca_base)>;
def I64_ALU_VECA : AIE__instr64__alu__veca<(ins alu_slot:$alu_base, veca_slot:$veca_base)>;
def I64_LDB_VECA : AIE__instr64__ldb__veca<(ins ldb_slot:$ldb_base, veca_slot:$veca_base)>;
def I64_MV0_VECA : AIE__instr64__mv0__veca<(ins mv0_slot:$mv0_base, veca_slot:$veca_base)>;
def I64_LDA_VECA : AIE__instr64__lda__veca<(ins lda_slot:$lda, veca_slot:$veca_base)>;
def I64_MV1_VECA : AIE__instr64__mv1__veca<(ins mv1_slot:$mv1_base, veca_slot:$veca_base)>;
// Extended instr64 Composite Instructions
def I64_LDA_LDB_MV1SSB : AIE__instr64__lda__ldb__mv1ssb<(ins lda_slot:$lda, ldb_slot:$ldb_base, mv1ssb_slot:$mv1ssb_base)>;
def I64_LDA_MV1SSB_ST : AIE__instr64__lda__mv1ssb__st<(ins lda_slot:$lda, mv1ssb_slot:$mv1ssb_base, st_slot:$st_base)>;
def I64_LDA_MV1SSB_ALU : AIE__instr64__lda__mv1ssb__alu<(ins lda_slot:$lda, mv1ssb_slot:$mv1ssb_base, alu_slot:$alu_base)>;
def I64_LDA_MV0_MV1SSB : AIE__instr64__lda__mv0__mv1ssb<(ins lda_slot:$lda, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base)>;
def I64_ST_ALU_MV1SSB : AIE__instr64__st__alu__mv1ssb<(ins st_slot:$st_base, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base)>;
def I64_ST_MV0_MV1SSB : AIE__instr64__st__mv0__mv1ssb<(ins st_slot:$st_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base)>;
def I64_ALU_MV0_MV1SSB : AIE__instr64__alu__mv0__mv1ssb<(ins alu_slot:$alu_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base)>;
def I64_ST_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr64__st__vec_shft__vec_strm__vec_short<(ins st_slot:$st_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I64_ST_VECM : AIE__instr64__st__vecm<(ins st_slot:$st_base, vecm_slot:$vecm_base)>;
def I64_ALU_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr64__alu__vec_shft__vec_strm__vec_short<(ins alu_slot:$alu_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I64_ALU_VECM : AIE__instr64__alu__vecm<(ins alu_slot:$alu_base, vecm_slot:$vecm_base)>;
def I64_LDB_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr64__ldb__vec_shft__vec_strm__vec_short<(ins ldb_slot:$ldb_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I64_LDB_VECM : AIE__instr64__ldb__vecm<(ins ldb_slot:$ldb_base, vecm_slot:$vecm_base)>;
def I64_MV0_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr64__mv0__vec_shft__vec_strm__vec_short<(ins mv0_slot:$mv0_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I64_MV0_VECM : AIE__instr64__mv0__vecm<(ins mv0_slot:$mv0_base, vecm_slot:$vecm_base)>;
def I64_LDA_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr64__lda__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I64_LDA_VECM : AIE__instr64__lda__vecm<(ins lda_slot:$lda, vecm_slot:$vecm_base)>;
def I64_MV1SSB_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr64__mv1ssb__vec_shft__vec_strm__vec_short<(ins mv1ssb_slot:$mv1ssb_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I64_MV1SSB_VECM : AIE__instr64__mv1ssb__vecm<(ins mv1ssb_slot:$mv1ssb_base, vecm_slot:$vecm_base)>;
def I64_MV1SSB_VECA : AIE__instr64__mv1ssb__veca<(ins mv1ssb_slot:$mv1ssb_base, veca_slot:$veca_base)>;
def I64_MV1_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr64__mv1__vec_shft__vec_strm__vec_short<(ins mv1_slot:$mv1_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I64_MV1_VECM : AIE__instr64__mv1__vecm<(ins mv1_slot:$mv1_base, vecm_slot:$vecm_base)>;

// instr96 Composite Instructions
def I96_ST_ALU_VECA : AIE__instr96__st__alu__veca<(ins st_slot:$st_base, alu_slot:$alu_base, veca_slot:$veca_base)>;
def I96_ST_MV0_VECA : AIE__instr96__st__mv0__veca<(ins st_slot:$st_base, mv0_slot:$mv0_base, veca_slot:$veca_base)>;
def I96_LDA_ALU_MV0_VECA : AIE__instr96__lda__alu__mv0__veca<(ins lda_slot:$lda, alu_slot:$alu_base, mv0_slot:$mv0_base, veca_slot:$veca_base)>;
def I96_LDA_ST_MV1_VECA : AIE__instr96__lda__st__mv1__veca<(ins lda_slot:$lda, st_slot:$st_base, mv1_slot:$mv1_base, veca_slot:$veca_base)>;
def I96_LDA_ALU_MV1_VECA : AIE__instr96__lda__alu__mv1__veca<(ins lda_slot:$lda, alu_slot:$alu_base, mv1_slot:$mv1_base, veca_slot:$veca_base)>;
def I96_LDA_LDB_MV0_VECA : AIE__instr96__lda__ldb__mv0__veca<(ins lda_slot:$lda, ldb_slot:$ldb_base, mv0_slot:$mv0_base, veca_slot:$veca_base)>;
def I96_LDA_MV0_MV1_VECA : AIE__instr96__lda__mv0__mv1__veca<(ins lda_slot:$lda, mv0_slot:$mv0_base, mv1_slot:$mv1_base, veca_slot:$veca_base)>;
def I96_LDA_LNG_VECA : AIE__instr96__lda__lng__veca<(ins lda_slot:$lda, lng_slot:$lng_base, veca_slot:$veca_base)>;
def I96_LDA_LDB_ST_ALU_MV0 : AIE__instr96__lda__ldb__st__alu__mv0<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, alu_slot:$alu_base, mv0_slot:$mv0_base)>;
def I96_LDA_LDB_ST_ALU_MV1 : AIE__instr96__lda__ldb__st__alu__mv1<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, alu_slot:$alu_base, mv1_slot:$mv1_base)>;
def I96_LDA_MV0_ST_ALU_MV1 : AIE__instr96__lda__mv0__st__alu__mv1<(ins lda_slot:$lda, mv0_slot:$mv0_base, st_slot:$st_base, alu_slot:$alu_base, mv1_slot:$mv1_base)>;
def I96_LDA_LDB_ST_MV0_MV1 : AIE__instr96__lda__ldb__st__mv0__mv1<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base)>;
def I96_LDA_LDB_ST_LNG : AIE__instr96__lda__ldb__st__lng<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, lng_slot:$lng_base)>;
def I96_LDA_LDB_ALU_MV0_MV1 : AIE__instr96__lda__ldb__alu__mv0__mv1<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base)>;
def I96_LDA_LDB_ALU_LNG : AIE__instr96__lda__ldb__alu__lng<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, lng_slot:$lng_base)>;

// Extended instr96 Composite Instructions
def I96_ST_ALU_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__st__alu__vec_shft__vec_strm__vec_short<(ins st_slot:$st_base, alu_slot:$alu_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_ST_ALU_VECM : AIE__instr96__st__alu__vecm<(ins st_slot:$st_base, alu_slot:$alu_base, vecm_slot:$vecm_base)>;
def I96_ST_MV0_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__st__mv0__vec_shft__vec_strm__vec_short<(ins st_slot:$st_base, mv0_slot:$mv0_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_ST_MV0_VECM : AIE__instr96__st__mv0__vecm<(ins st_slot:$st_base, mv0_slot:$mv0_base, vecm_slot:$vecm_base)>;
def I96_LDA_ALU_MV0_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__lda__alu__mv0__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, alu_slot:$alu_base, mv0_slot:$mv0_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_LDA_ALU_MV0_VECM : AIE__instr96__lda__alu__mv0__vecm<(ins lda_slot:$lda, alu_slot:$alu_base, mv0_slot:$mv0_base, vecm_slot:$vecm_base)>;
def I96_LDA_ST_MV1SSB_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__lda__st__mv1ssb__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, st_slot:$st_base, mv1ssb_slot:$mv1ssb_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_LDA_ST_MV1SSB_VECM : AIE__instr96__lda__st__mv1ssb__vecm<(ins lda_slot:$lda, st_slot:$st_base, mv1ssb_slot:$mv1ssb_base, vecm_slot:$vecm_base)>;
def I96_LDA_ST_MV1SSB_VECA : AIE__instr96__lda__st__mv1ssb__veca<(ins lda_slot:$lda, st_slot:$st_base, mv1ssb_slot:$mv1ssb_base, veca_slot:$veca_base)>;
def I96_LDA_ST_MV1_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__lda__st__mv1__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, st_slot:$st_base, mv1_slot:$mv1_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_LDA_ST_MV1_VECM : AIE__instr96__lda__st__mv1__vecm<(ins lda_slot:$lda, st_slot:$st_base, mv1_slot:$mv1_base, vecm_slot:$vecm_base)>;
def I96_LDA_ALU_MV1SSB_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__lda__alu__mv1ssb__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_LDA_ALU_MV1SSB_VECM : AIE__instr96__lda__alu__mv1ssb__vecm<(ins lda_slot:$lda, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base, vecm_slot:$vecm_base)>;
def I96_LDA_ALU_MV1SSB_VECA : AIE__instr96__lda__alu__mv1ssb__veca<(ins lda_slot:$lda, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base, veca_slot:$veca_base)>;
def I96_LDA_ALU_MV1_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__lda__alu__mv1__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, alu_slot:$alu_base, mv1_slot:$mv1_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_LDA_ALU_MV1_VECM : AIE__instr96__lda__alu__mv1__vecm<(ins lda_slot:$lda, alu_slot:$alu_base, mv1_slot:$mv1_base, vecm_slot:$vecm_base)>;
def I96_LDA_LDB_MV0_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__lda__ldb__mv0__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, mv0_slot:$mv0_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_LDA_LDB_MV0_VECM : AIE__instr96__lda__ldb__mv0__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, mv0_slot:$mv0_base, vecm_slot:$vecm_base)>;
def I96_LDA_MV0_MV1SSB_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__lda__mv0__mv1ssb__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_LDA_MV0_MV1SSB_VECM : AIE__instr96__lda__mv0__mv1ssb__vecm<(ins lda_slot:$lda, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, vecm_slot:$vecm_base)>;
def I96_LDA_MV0_MV1SSB_VECA : AIE__instr96__lda__mv0__mv1ssb__veca<(ins lda_slot:$lda, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, veca_slot:$veca_base)>;
def I96_LDA_MV0_MV1_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__lda__mv0__mv1__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, mv0_slot:$mv0_base, mv1_slot:$mv1_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_LDA_MV0_MV1_VECM : AIE__instr96__lda__mv0__mv1__vecm<(ins lda_slot:$lda, mv0_slot:$mv0_base, mv1_slot:$mv1_base, vecm_slot:$vecm_base)>;
def I96_LDA_LNG_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr96__lda__lng__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, lng_slot:$lng_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I96_LDA_LNG_VECM : AIE__instr96__lda__lng__vecm<(ins lda_slot:$lda, lng_slot:$lng_base, vecm_slot:$vecm_base)>;
def I96_LDA_LDB_ST_ALU_MV1SSB : AIE__instr96__lda__ldb__st__alu__mv1ssb<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base)>;
def I96_LDA_MV0_ST_ALU_MV1SSB : AIE__instr96__lda__mv0__st__alu__mv1ssb<(ins lda_slot:$lda, mv0_slot:$mv0_base, st_slot:$st_base, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base)>;
def I96_LDA_LDB_ST_MV0_MV1SSB : AIE__instr96__lda__ldb__st__mv0__mv1ssb<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base)>;
def I96_LDA_LDB_ALU_MV0_MV1SSB : AIE__instr96__lda__ldb__alu__mv0__mv1ssb<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base)>;

// instr128 Composite Instructions
def I128_LDA_LDB_ST_MV0_MV1_VECA : AIE__instr128__lda__ldb__st__mv0__mv1__veca<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base, veca_slot:$veca_base)>;
def I128_LDA_LDB_ST_LNG_VECA : AIE__instr128__lda__ldb__st__lng__veca<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, lng_slot:$lng_base, veca_slot:$veca_base)>;
def I128_LDA_LDB_ALU_MV0_MV1_VECA : AIE__instr128__lda__ldb__alu__mv0__mv1__veca<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base, veca_slot:$veca_base)>;
def I128_LDA_LDB_ALU_LNG_VECA : AIE__instr128__lda__ldb__alu__lng__veca<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, lng_slot:$lng_base, veca_slot:$veca_base)>;
def I128_LDA_LDB_ST_MV0_MV1SSB_ALU_VECM : AIE__instr128__lda__ldb__st__mv0__mv1ssb__alu__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, alu_slot:$alu_base, vecm_slot:$vecm_base)>;
def I128_LDA_LDB_ST_ALU_MV1_VECA : AIE__instr128__lda__ldb__st__alu__mv1__veca<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, alu_slot:$alu_base, mv1_slot:$mv1_base, veca_slot:$veca_base)>;
def I128_LDA_MV0_ST_ALU_MV1_VECA : AIE__instr128__lda__mv0__st__alu__mv1__veca<(ins lda_slot:$lda, mv0_slot:$mv0_base, st_slot:$st_base, alu_slot:$alu_base, mv1_slot:$mv1_base, veca_slot:$veca_base)>;

// Extended instr128 Composite Instructions
def I128_LDA_LDB_ST_MV0_MV1SSB_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__ldb__st__mv0__mv1ssb__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_LDB_ST_MV0_MV1SSB_VECM : AIE__instr128__lda__ldb__st__mv0__mv1ssb__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, vecm_slot:$vecm_base)>;
def I128_LDA_LDB_ST_MV0_MV1SSB_VECA : AIE__instr128__lda__ldb__st__mv0__mv1ssb__veca<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, veca_slot:$veca_base)>;
def I128_LDA_LDB_ST_MV0_MV1_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__ldb__st__mv0__mv1__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_LDB_ST_MV0_MV1_VECM : AIE__instr128__lda__ldb__st__mv0__mv1__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base, vecm_slot:$vecm_base)>;
def I128_LDA_LDB_ST_LNG_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__ldb__st__lng__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, lng_slot:$lng_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_LDB_ST_LNG_VECM : AIE__instr128__lda__ldb__st__lng__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, lng_slot:$lng_base, vecm_slot:$vecm_base)>;
def I128_LDA_LDB_ALU_MV0_MV1SSB_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__ldb__alu__mv0__mv1ssb__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_LDB_ALU_MV0_MV1SSB_VECM : AIE__instr128__lda__ldb__alu__mv0__mv1ssb__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, vecm_slot:$vecm_base)>;
def I128_LDA_LDB_ALU_MV0_MV1SSB_VECA : AIE__instr128__lda__ldb__alu__mv0__mv1ssb__veca<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, veca_slot:$veca_base)>;
def I128_LDA_LDB_ALU_MV0_MV1_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__ldb__alu__mv0__mv1__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_LDB_ALU_MV0_MV1_VECM : AIE__instr128__lda__ldb__alu__mv0__mv1__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, mv0_slot:$mv0_base, mv1_slot:$mv1_base, vecm_slot:$vecm_base)>;
def I128_LDA_LDB_ALU_LNG_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__ldb__alu__lng__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, lng_slot:$lng_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_LDB_ALU_LNG_VECM : AIE__instr128__lda__ldb__alu__lng__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, alu_slot:$alu_base, lng_slot:$lng_base, vecm_slot:$vecm_base)>;
def I128_LDA_LDB_ST_MV0_MV1SSB_ALU_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__ldb__st__mv0__mv1ssb__alu__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, mv0_slot:$mv0_base, mv1ssb_slot:$mv1ssb_base, alu_slot:$alu_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_LDB_ST_ALU_MV1SSB_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__ldb__st__alu__mv1ssb__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_LDB_ST_ALU_MV1SSB_VECM : AIE__instr128__lda__ldb__st__alu__mv1ssb__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base, vecm_slot:$vecm_base)>;
def I128_LDA_LDB_ST_ALU_MV1SSB_VECA : AIE__instr128__lda__ldb__st__alu__mv1ssb__veca<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base, veca_slot:$veca_base)>;
def I128_LDA_LDB_ST_ALU_MV1_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__ldb__st__alu__mv1__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, alu_slot:$alu_base, mv1_slot:$mv1_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_LDB_ST_ALU_MV1_VECM : AIE__instr128__lda__ldb__st__alu__mv1__vecm<(ins lda_slot:$lda, ldb_slot:$ldb_base, st_slot:$st_base, alu_slot:$alu_base, mv1_slot:$mv1_base, vecm_slot:$vecm_base)>;
def I128_LDA_MV0_ST_ALU_MV1SSB_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__mv0__st__alu__mv1ssb__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, mv0_slot:$mv0_base, st_slot:$st_base, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_MV0_ST_ALU_MV1SSB_VECM : AIE__instr128__lda__mv0__st__alu__mv1ssb__vecm<(ins lda_slot:$lda, mv0_slot:$mv0_base, st_slot:$st_base, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base, vecm_slot:$vecm_base)>;
def I128_LDA_MV0_ST_ALU_MV1SSB_VECA : AIE__instr128__lda__mv0__st__alu__mv1ssb__veca<(ins lda_slot:$lda, mv0_slot:$mv0_base, st_slot:$st_base, alu_slot:$alu_base, mv1ssb_slot:$mv1ssb_base, veca_slot:$veca_base)>;
def I128_LDA_MV0_ST_ALU_MV1_VEC_SHFT_VEC_STRM_VEC_SHORT : AIE__instr128__lda__mv0__st__alu__mv1__vec_shft__vec_strm__vec_short<(ins lda_slot:$lda, mv0_slot:$mv0_base, st_slot:$st_base, alu_slot:$alu_base, mv1_slot:$mv1_base, vecshft_slot:$vec_shft, vecstrm_slot:$vec_strm, vecshrt_slot:$vec_short)>;
def I128_LDA_MV0_ST_ALU_MV1_VECM : AIE__instr128__lda__mv0__st__alu__mv1__vecm<(ins lda_slot:$lda, mv0_slot:$mv0_base, st_slot:$st_base, alu_slot:$alu_base, mv1_slot:$mv1_base, vecm_slot:$vecm_base)>;
