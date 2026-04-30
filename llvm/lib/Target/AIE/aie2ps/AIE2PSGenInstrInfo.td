//===-AIE2PSGenInstrInfo.td -----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

let hasSideEffects = false, mayLoad = false, mayStore = false in {
let Itinerary = II_ABS, Defs = [srCarry] in
def ABS : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "abs", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__abs
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b1000, 0b10, 0b0000};
}

let Itinerary = II_ACQ_COND_mLockId_imm, hasSideEffects = true in
def ACQ_COND_mLockId_imm : AIE2PS_inst_alu_instr32 <(outs), (ins c6u:$id, OP_mRy:$s1, mR23_lock:$r23), "acq.cond", "#$id, $s1, r23">{
// id: me__instr128_or__alu_mv__alumv_or__alu__acquire_cond__mLockId__mLockId_imm__mR23_lock
bits<6> id;
bits<5> s1;

let alu = {id, 0b0, dontcare{1}, 0b11, s1, 0b1, 0b0000};
}

let Itinerary = II_ACQ_COND_mLockId_reg, hasSideEffects = true in
def ACQ_COND_mLockId_reg : AIE2PS_inst_alu_instr32 <(outs), (ins OP_mRx:$s0, OP_mRy:$s1, mR23_lock:$r23), "acq.cond", "$s0, $s1, r23">{
// id: me__instr128_or__alu_mv__alumv_or__alu__acquire_cond__mLockId__mLockId_reg__mR23_lock
bits<5> s0;
bits<5> s1;

let alu = {s0, dontcare{1}, 0b1, dontcare{1}, 0b11, s1, 0b1, 0b0000};
}

let Itinerary = II_ACQ_mLockId_imm, hasSideEffects = true in
def ACQ_mLockId_imm : AIE2PS_inst_alu_instr32 <(outs), (ins c6u:$id, OP_mRy:$s1), "acq", "#$id, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__acquire__mLockId__mLockId_imm
bits<6> id;
bits<5> s1;

let alu = {id, 0b0, dontcare{1}, 0b01, s1, 0b1, 0b0000};
}

let Itinerary = II_ACQ_mLockId_reg, hasSideEffects = true in
def ACQ_mLockId_reg : AIE2PS_inst_alu_instr32 <(outs), (ins OP_mRx:$s0, OP_mRy:$s1), "acq", "$s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__acquire__mLockId__mLockId_reg
bits<5> s0;
bits<5> s1;

let alu = {s0, dontcare{1}, 0b1, dontcare{1}, 0b01, s1, 0b1, 0b0000};
}

let Itinerary = II_ADC, Defs = [srCarry], Uses = [srCarry] in
def ADC : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "adc", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__addx
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0010, 0b1};
}

let Itinerary = II_ADDM_NC_mv_add_ri, ItineraryRegPairs = [
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eDJ_eRe, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_mLRm_eRe, [OperandRegClass<0, mLRm>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eDC_eRe, [OperandRegClass<0, eDC>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eRe_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eVaddSign_eRe, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eP_eRe, [OperandRegClass<0, eP>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eUnpackSign_eRe, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eSRSSign_eRe, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eUPSSign_eRe, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eS_eRe, [OperandRegClass<0, eS>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_mLCm_eRe, [OperandRegClass<0, mLCm>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_ePackSign_eRe, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eDN_eRe, [OperandRegClass<0, eDN>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_mSPm_eRe, [OperandRegClass<0, mSPm>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eM_eRe, [OperandRegClass<0, eM>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eRo_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eDJ_eRo, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_mLRm_eRo, [OperandRegClass<0, mLRm>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eDC_eRo, [OperandRegClass<0, eDC>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eRe_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eVaddSign_eRo, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eP_eRo, [OperandRegClass<0, eP>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eUnpackSign_eRo, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eSRSSign_eRo, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eUPSSign_eRo, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eS_eRo, [OperandRegClass<0, eS>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_mLCm_eRo, [OperandRegClass<0, mLCm>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_ePackSign_eRo, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eDN_eRo, [OperandRegClass<0, eDN>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_mSPm_eRo, [OperandRegClass<0, mSPm>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eM_eRo, [OperandRegClass<0, eM>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_ri_eRo_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>]>] in
def ADDM_NC_mv_add_ri : AIE2PS_inst_mv_instr32 <(outs OP_mMvSclDst:$dst), (ins OP_mRm:$s0, c8s:$imm), "addm.nc", "$dst, $s0, #$imm">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_add_ri
bits<7> dst;
bits<5> s0;
bits<8> imm;

let mv = {dst, s0, imm, 0b00};
}

let Itinerary = II_ADDM_NC_mv_add_rr, ItineraryRegPairs = [
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDJ_eRe_eRe, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mLRm_eRe_eRe, [OperandRegClass<0, mLRm>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDC_eRe_eRe, [OperandRegClass<0, eDC>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eRe_eRe_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eVaddSign_eRe_eRe, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eP_eRe_eRe, [OperandRegClass<0, eP>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eUnpackSign_eRe_eRe, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eSRSSign_eRe_eRe, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eUPSSign_eRe_eRe, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eS_eRe_eRe, [OperandRegClass<0, eS>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mLCm_eRe_eRe, [OperandRegClass<0, mLCm>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_ePackSign_eRe_eRe, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDN_eRe_eRe, [OperandRegClass<0, eDN>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mSPm_eRe_eRe, [OperandRegClass<0, mSPm>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eM_eRe_eRe, [OperandRegClass<0, eM>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eRo_eRe_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDJ_eRo_eRe, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mLRm_eRo_eRe, [OperandRegClass<0, mLRm>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDC_eRo_eRe, [OperandRegClass<0, eDC>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eRe_eRo_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eVaddSign_eRo_eRe, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eP_eRo_eRe, [OperandRegClass<0, eP>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eUnpackSign_eRo_eRe, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eSRSSign_eRo_eRe, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eUPSSign_eRo_eRe, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eS_eRo_eRe, [OperandRegClass<0, eS>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mLCm_eRo_eRe, [OperandRegClass<0, mLCm>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_ePackSign_eRo_eRe, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDN_eRo_eRe, [OperandRegClass<0, eDN>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mSPm_eRo_eRe, [OperandRegClass<0, mSPm>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eM_eRo_eRe, [OperandRegClass<0, eM>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eRo_eRo_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>, OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDJ_eRe_eRo, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mLRm_eRe_eRo, [OperandRegClass<0, mLRm>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDC_eRe_eRo, [OperandRegClass<0, eDC>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eRe_eRe_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eVaddSign_eRe_eRo, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eP_eRe_eRo, [OperandRegClass<0, eP>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eUnpackSign_eRe_eRo, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eSRSSign_eRe_eRo, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eUPSSign_eRe_eRo, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eS_eRe_eRo, [OperandRegClass<0, eS>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mLCm_eRe_eRo, [OperandRegClass<0, mLCm>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_ePackSign_eRe_eRo, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDN_eRe_eRo, [OperandRegClass<0, eDN>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mSPm_eRe_eRo, [OperandRegClass<0, mSPm>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eM_eRe_eRo, [OperandRegClass<0, eM>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eRo_eRe_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDJ_eRo_eRo, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mLRm_eRo_eRo, [OperandRegClass<0, mLRm>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDC_eRo_eRo, [OperandRegClass<0, eDC>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eRe_eRo_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eVaddSign_eRo_eRo, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eP_eRo_eRo, [OperandRegClass<0, eP>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eUnpackSign_eRo_eRo, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eSRSSign_eRo_eRo, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eUPSSign_eRo_eRo, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eS_eRo_eRo, [OperandRegClass<0, eS>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mLCm_eRo_eRo, [OperandRegClass<0, mLCm>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_ePackSign_eRo_eRo, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eDN_eRo_eRo, [OperandRegClass<0, eDN>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_mSPm_eRo_eRo, [OperandRegClass<0, mSPm>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eM_eRo_eRo, [OperandRegClass<0, eM>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ADDM_NC_mv_add_rr_eRo_eRo_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>, OperandRegClass<2, eRo>]>] in
def ADDM_NC_mv_add_rr : AIE2PS_inst_mv_instr32 <(outs OP_mMvSclDst:$dst), (ins OP_mRm:$s0, OP_mRs:$s1), "addm.nc", "$dst, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_add_rr
bits<7> dst;
bits<5> s0;
bits<5> s1;

let mv = {dst, s0, s1, 0b01010};
}

let Itinerary = II_ADDM_NC_mv_cg_or_mv_cg_le_rel, Defs = [le], Uses = [pc] in
def ADDM_NC_mv_cg_or_mv_cg_le_rel : AIE2PS_inst_mv_instr32 <(outs), (ins c11u:$i), "addm.nc", "le, pc, #$i">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cg_or__mv_cg_le_rel__zlpm__rel2abs
bits<11> i;

let mv = {0b1111110, i{6-0}, i{10-7}, 0b0101};
}

let Itinerary = II_ADDM_NC_mv_cg_or_mv_cg_ls_rel, Defs = [ls], Uses = [pc] in
def ADDM_NC_mv_cg_or_mv_cg_ls_rel : AIE2PS_inst_mv_instr32 <(outs), (ins c11u:$i), "addm.nc", "ls, pc, #$i">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cg_or__mv_cg_ls_rel__zlpm__rel2abs
bits<11> i;

let mv = {0b1111111, i{6-0}, i{10-7}, 0b0101};
}

let Itinerary = II_ADDXM_NC_lng_cg_le_rel, Defs = [le], Uses = [pc] in
def ADDXM_NC_lng_cg_le_rel : AIE2PS_inst_lng_instr48 <(outs), (ins cpmaddr:$i), "addxm.nc", "le, pc, #$i">{
// id: me__instr128_or__alu_mv__lng__lng_cg_le_rel__zlpm__rel2abs
bits<20> i;

let lng = {dontcare{5-1}, i, dontcare{12-1}, 0b11, 0b101};
}

let Itinerary = II_ADDXM_NC_lng_cg_ls_rel, Defs = [ls], Uses = [pc] in
def ADDXM_NC_lng_cg_ls_rel : AIE2PS_inst_lng_instr48 <(outs), (ins cpmaddr:$i), "addxm.nc", "ls, pc, #$i">{
// id: me__instr128_or__alu_mv__lng__lng_cg_ls_rel__zlpm__rel2abs
bits<20> i;

let lng = {dontcare{5-1}, i, dontcare{12-1}, 0b10, 0b101};
}

let Itinerary = II_ADD_NC_add_lc_ri in
def ADD_NC_add_lc_ri : AIE2PS_inst_alu_instr32 <(outs mLCxCg:$lc), (ins OP_mRx:$s0, c7s:$imm), "add.nc", "lc, $s0, #$imm">{
// id: me__instr128_or__alu_mv__alumv_or__alu__add_lc_ri__mLCxCg
bits<5> s0;
bits<7> imm;

let alu = {s0, dontcare{2-1}, imm{6-6}, 0b111, imm{5-0}, 0b010};
}

let Itinerary = II_ADD_NC_alu_cg_or_alu_cg_le_rel, Defs = [le], Uses = [pc] in
def ADD_NC_alu_cg_or_alu_cg_le_rel : AIE2PS_inst_alu_instr32 <(outs), (ins c11u:$i), "add.nc", "le, pc, #$i">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_cg_or__alu_cg_le_rel__zlpx__rel2abs
bits<11> i;

let alu = {i{10-6}, dontcare{2-1}, 0b0011, i{5-0}, 0b010};
}

let Itinerary = II_ADD_NC_alu_cg_or_alu_cg_ls_rel, Defs = [ls], Uses = [pc] in
def ADD_NC_alu_cg_or_alu_cg_ls_rel : AIE2PS_inst_alu_instr32 <(outs), (ins c11u:$i), "add.nc", "ls, pc, #$i">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_cg_or__alu_cg_ls_rel__zlpx__rel2abs
bits<11> i;

let alu = {i{10-6}, dontcare{2-1}, 0b1011, i{5-0}, 0b010};
}

let Itinerary = II_ADD_add_r_ri, Defs = [srCarry] in
def ADD_add_r_ri : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, c7s:$imm), "add", "$d0, $s0, #$imm">{
// id: me__instr128_or__alu_mv__alumv_or__alu__add_r_ri
bits<5> s0;
bits<5> d0;
bits<7> imm;

let alu = {s0, d0, imm, 0b110};
}

let Itinerary = II_ADD_alu_r_rr, Defs = [srCarry] in
def ADD_alu_r_rr : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "add", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__add
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0000, 0b1};
}

let Itinerary = II_AND in
def AND : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "and", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__and
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0100, 0b1};
}

let Itinerary = II_ASHL in
def ASHL : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "ashl", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sgn
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1110, 0b1};
}

let Itinerary = II_CLB in
def CLB : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "clb", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__clb
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0000, 0b10, 0b0000};
}

let Itinerary = II_CLZ in
def CLZ : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "clz", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__clbu
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0001, 0b10, 0b0000};
}

let Itinerary = II_DIVS, Constraints = "$d0 = $s0, $r31_out = $r31" in
def DIVS : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0, mR31_divs:$r31_out), (ins mR31_divs:$r31, OP_mRx:$s0, OP_mRy:$s1), "divs", "$d0, r31, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_dstep__mR31_divs
bits<5> s0;
bits<5> s1;

let alu = {s0, s1, dontcare{2-1}, 0b1110, 0b1000};
}

let Itinerary = II_DONE, hasSideEffects = true in
def DONE : AIE2PS_inst_alu_instr32 <(outs), (ins), "done", "">{
// id: me__instr128_or__alu_mv__alumv_or__alu__done

let alu = {dontcare{2-1}, 0b10, 0b0, 0b001, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EQZ in
def EQZ : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "eqz", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__seq0
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0110, 0b10, 0b0000};
}

let Itinerary = II_EQ in
def EQ : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "eq", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__seq
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0111, 0b1};
}

let Itinerary = II_EVENT_ERROR, hasSideEffects = true in
def EVENT_ERROR : AIE2PS_inst_alu_instr32 <(outs), (ins), "event.error", "">{
// id: me__instr128_or__alu_mv__alumv_or__alu__event__event3

let alu = {0b11, dontcare{3-1}, 0b010, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EVENT_WARNING, hasSideEffects = true in
def EVENT_WARNING : AIE2PS_inst_alu_instr32 <(outs), (ins), "event.warning", "">{
// id: me__instr128_or__alu_mv__alumv_or__alu__event__event2

let alu = {0b10, dontcare{3-1}, 0b010, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EVENT_event0, hasSideEffects = true in
def EVENT_event0 : AIE2PS_inst_alu_instr32 <(outs), (ins), "event", "#0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__event__event0

let alu = {0b00, dontcare{3-1}, 0b010, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EVENT_event1, hasSideEffects = true in
def EVENT_event1 : AIE2PS_inst_alu_instr32 <(outs), (ins), "event", "#1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__event__event1

let alu = {0b01, dontcare{3-1}, 0b010, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EXTEND_s16 in
def EXTEND_s16 : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "extend.s16", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__se16
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0011, 0b10, 0b0000};
}

let Itinerary = II_EXTEND_s8 in
def EXTEND_s8 : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "extend.s8", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__se8
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0010, 0b10, 0b0000};
}

let Itinerary = II_EXTEND_u16 in
def EXTEND_u16 : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "extend.u16", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__ze16
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0101, 0b10, 0b0000};
}

let Itinerary = II_EXTEND_u8 in
def EXTEND_u8 : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "extend.u8", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__ze8
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0100, 0b10, 0b0000};
}

let Itinerary = II_GEU in
def GEU : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "geu", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sgeu
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1011, 0b1};
}

let Itinerary = II_GE in
def GE : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "ge", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sge
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1001, 0b1};
}

let Itinerary = II_INVSQRT_nlf_invsqrt, Defs = [srFPNlf], Uses = [crFPNlfMask] in
def INVSQRT_nlf_invsqrt : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRx:$s0), "invsqrt", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b01, 0b0, 0b0, 0b00, 0b1000};
}

let Itinerary = II_INVSQRT_nlf_invsqrt_mOptConvDel_mS3, Defs = [srFPCnvFl2Fx, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPNlfMask] in
def INVSQRT_nlf_invsqrt_mOptConvDel_mS3 : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins mS3:$mcnv_del, OP_mRx:$s0), "invsqrt", " ${d0}.flt2fx, s3, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b01, 0b1, 0b0, 0b00, 0b1000};
}

let Itinerary = II_INVSQRT_nlf_invsqrt_mOptConvDel_mS3_mOptConv, Defs = [srFPCnvFl2Fx, srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPCnvFx2FlMask, crFPNlfMask] in
def INVSQRT_nlf_invsqrt_mOptConvDel_mS3_mOptConv : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins mS3:$mcnv_del, OP_mRx:$s0, mS2:$mcnv), "invsqrt", " ${d0}.flt2fx, s3, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b01, 0b1, 0b1, 0b00, 0b1000};
}

let Itinerary = II_INVSQRT_nlf_invsqrt_mOptConv, Defs = [srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFx2FlMask, crFPNlfMask] in
def INVSQRT_nlf_invsqrt_mOptConv : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRx:$s0, mS2:$mcnv), "invsqrt", " $d0, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b01, 0b0, 0b1, 0b00, 0b1000};
}

let Itinerary = II_INV_nlf_inv, Defs = [srFPNlf], Uses = [crFPNlfMask] in
def INV_nlf_inv : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRx:$s0), "inv", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b10, 0b0, 0b0, 0b00, 0b1000};
}

let Itinerary = II_INV_nlf_inv_mOptConvDel_mS3, Defs = [srFPCnvFl2Fx, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPNlfMask] in
def INV_nlf_inv_mOptConvDel_mS3 : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins mS3:$mcnv_del, OP_mRx:$s0), "inv", " ${d0}.flt2fx, s3, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b10, 0b1, 0b0, 0b00, 0b1000};
}

let Itinerary = II_INV_nlf_inv_mOptConvDel_mS3_mOptConv, Defs = [srFPCnvFl2Fx, srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPCnvFx2FlMask, crFPNlfMask] in
def INV_nlf_inv_mOptConvDel_mS3_mOptConv : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins mS3:$mcnv_del, OP_mRx:$s0, mS2:$mcnv), "inv", " ${d0}.flt2fx, s3, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b10, 0b1, 0b1, 0b00, 0b1000};
}

let Itinerary = II_INV_nlf_inv_mOptConv, Defs = [srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFx2FlMask, crFPNlfMask] in
def INV_nlf_inv_mOptConv : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRx:$s0, mS2:$mcnv), "inv", " $d0, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b10, 0b0, 0b1, 0b00, 0b1000};
}

let Itinerary = II_JL_alumv_or, hasDelaySlot = true, Defs = [lr] in
def JL_alumv_or : AIE2PS_inst_alu_instr32 <(outs), (ins eP:$i), "jl", "$i">{
// id: me__instr128_or__alu_mv__alumv_or__alu__jump_link_ind__mPm__mLRx
bits<3> i;

let alu = {dontcare{2-1}, 0b010, 0b001, dontcare{2-1}, i, 0b000, 0b0000};
}

let Itinerary = II_JL_lng, hasDelaySlot = true, Defs = [lr] in
def JL_lng : AIE2PS_inst_lng_instr48 <(outs), (ins cpmaddr:$i), "jl", "#$i">{
// id: me__instr128_or__alu_mv__lng__jump_link_imm__mLRx
bits<20> i;

let lng = {dontcare{5-1}, i, dontcare{1}, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b100};
}

let Itinerary = II_JNZD, hasDelaySlot = true, Defs = [srCarry] in
def JNZD : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, eP:$i), "jnzd", "$d0, $s0, $i">{
// id: me__instr128_or__alu_mv__alumv_or__alu__jumpc_decr_ind__mPm
bits<5> s0;
bits<5> d0;
bits<3> i;

let alu = {s0, d0, i, 0b100, 0b0000};
}

let Itinerary = II_JNZ, hasDelaySlot = true in
def JNZ : AIE2PS_inst_lng_instr48 <(outs), (ins OP_mRx:$s0, cpmaddr:$i), "jnz", " $s0, #$i">{
// id: me__instr128_or__alu_mv__lng__jumpc_imm__bnez
bits<5> s0;
bits<20> i;

let lng = {s0, i, 0b1, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b110};
}

let Itinerary = II_JZ, hasDelaySlot = true in
def JZ : AIE2PS_inst_lng_instr48 <(outs), (ins OP_mRx:$s0, cpmaddr:$i), "jz", " $s0, #$i">{
// id: me__instr128_or__alu_mv__lng__jumpc_imm__beqz
bits<5> s0;
bits<20> i;

let lng = {s0, i, 0b0, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b110};
}

let Itinerary = II_J_alumv_or, hasDelaySlot = true in
def J_alumv_or : AIE2PS_inst_alu_instr32 <(outs), (ins eP:$i), "j", "$i">{
// id: me__instr128_or__alu_mv__alumv_or__alu__jump_ind__mPm
bits<3> i;

let alu = {dontcare{2-1}, 0b000, 0b001, dontcare{2-1}, i, 0b000, 0b0000};
}

let Itinerary = II_J_lng, hasDelaySlot = true in
def J_lng : AIE2PS_inst_lng_instr48 <(outs), (ins cpmaddr:$i), "j", "#$i">{
// id: me__instr128_or__alu_mv__lng__jump_imm
bits<20> i;

let lng = {dontcare{5-1}, i, dontcare{1}, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b010};
}

let Itinerary = II_LDA_2D_dml_lda2_scalar_EE, DecoderMethod = "DecodeLDA_2D_dml_lda2_scalar_EE", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dml_lda2_scalar_EE : AIE2PS_inst_lda_instr32 <(outs OP_mEEa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b01000101};
}

let Itinerary = II_LDA_2D_dml_lda2_scalar_EG, DecoderMethod = "DecodeLDA_2D_dml_lda2_scalar_EG", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dml_lda2_scalar_EG : AIE2PS_inst_lda_instr32 <(outs OP_mEGa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b0111011};
}

let Itinerary = II_LDA_2D_dml_lda2_scalar_GG, DecoderMethod = "DecodeLDA_2D_dml_lda2_scalar_GG", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dml_lda2_scalar_GG : AIE2PS_inst_lda_instr32 <(outs OP_mGGa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b00100101};
}

let Itinerary = II_LDA_2D_dml_lda_scalar_F, DecoderMethod = "DecodeLDA_2D_dml_lda_scalar_F", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dml_lda_scalar_F : AIE2PS_inst_lda_instr32 <(outs OP_mFa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b0000100};
}

let Itinerary = II_LDA_2D_dml_lda_scalar_L, DecoderMethod = "DecodeLDA_2D_dml_lda_scalar_L", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dml_lda_scalar_L : AIE2PS_inst_lda_instr32 <(outs eL:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_2d__mPa__mDa__mLa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b01100101};
}

let Itinerary = II_LDA_2D_dms_lda_g, DecoderMethod = "DecodeLDA_2D_dms_lda_g", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dms_lda_g : AIE2PS_inst_lda_instr32 <(outs OP_mGa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b0100100};
}

let Itinerary = II_LDA_2D_dms_lda_scalar, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_scalar_eEho, [OperandRegClass<0, eEho>]>], DecoderMethod = "DecodeLDA_2D_dms_lda_scalar", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dms_lda_scalar : AIE2PS_inst_lda_instr32 <(outs OP_mLdaScl:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<7> dst;

let lda = {ptr, mod, 0b100, dst, 0b10011};
}

let Itinerary = II_LDA_2D_dmv_lda_eg2, DecoderMethod = "DecodeLDA_2D_dmv_lda_eg2", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dmv_lda_eg2 : AIE2PS_inst_lda_instr32 <(outs OP_mEG2a:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b00010101};
}

let Itinerary = II_LDA_2D_dmv_lda_f, DecoderMethod = "DecodeLDA_2D_dmv_lda_f", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dmv_lda_f : AIE2PS_inst_lda_instr32 <(outs OP_mFFa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b01010101};
}

let Itinerary = II_LDA_2D_dmv_lda_feg, DecoderMethod = "DecodeLDA_2D_dmv_lda_feg", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dmv_lda_feg : AIE2PS_inst_lda_instr32 <(outs OP_mFEGa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b0010100};
}

let Itinerary = II_LDA_2D_s16, DecoderMethod = "DecodeLDA_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_s16 : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d.s16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_2d__mPa__mDa__lh
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b10, 0b00011};
}

let Itinerary = II_LDA_2D_s8, DecoderMethod = "DecodeLDA_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_s8 : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d.s8", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_2d__mPa__mDa__lb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b00, 0b00011};
}

let Itinerary = II_LDA_2D_u16, DecoderMethod = "DecodeLDA_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_u16 : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d.u16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_2d__mPa__mDa__lhu
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b11, 0b00011};
}

let Itinerary = II_LDA_2D_u8, DecoderMethod = "DecodeLDA_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_u8 : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d.u8", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_2d__mPa__mDa__lbu
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b01, 0b00011};
}

let Itinerary = II_LDA_3D_dml_lda2_scalar_EE, DecoderMethod = "DecodeLDA_3D_dml_lda2_scalar_EE", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dml_lda2_scalar_EE : AIE2PS_inst_lda_instr32 <(outs OP_mEEa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b01000101};
}

let Itinerary = II_LDA_3D_dml_lda2_scalar_EG, DecoderMethod = "DecodeLDA_3D_dml_lda2_scalar_EG", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dml_lda2_scalar_EG : AIE2PS_inst_lda_instr32 <(outs OP_mEGa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b0111011};
}

let Itinerary = II_LDA_3D_dml_lda2_scalar_GG, DecoderMethod = "DecodeLDA_3D_dml_lda2_scalar_GG", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dml_lda2_scalar_GG : AIE2PS_inst_lda_instr32 <(outs OP_mGGa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b00100101};
}

let Itinerary = II_LDA_3D_dml_lda_scalar_F, DecoderMethod = "DecodeLDA_3D_dml_lda_scalar_F", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dml_lda_scalar_F : AIE2PS_inst_lda_instr32 <(outs OP_mFa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b0000100};
}

let Itinerary = II_LDA_3D_dml_lda_scalar_L, DecoderMethod = "DecodeLDA_3D_dml_lda_scalar_L", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dml_lda_scalar_L : AIE2PS_inst_lda_instr32 <(outs eL:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_3d__mPa__mDSa__mLa
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b01100101};
}

let Itinerary = II_LDA_3D_dms_lda_g, DecoderMethod = "DecodeLDA_3D_dms_lda_g", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dms_lda_g : AIE2PS_inst_lda_instr32 <(outs OP_mGa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b0100100};
}

let Itinerary = II_LDA_3D_dms_lda_scalar, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_scalar_eEho, [OperandRegClass<0, eEho>]>], DecoderMethod = "DecodeLDA_3D_dms_lda_scalar", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dms_lda_scalar : AIE2PS_inst_lda_instr32 <(outs OP_mLdaScl:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<7> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b10011};
}

let Itinerary = II_LDA_3D_dmv_lda_eg2, DecoderMethod = "DecodeLDA_3D_dmv_lda_eg2", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dmv_lda_eg2 : AIE2PS_inst_lda_instr32 <(outs OP_mEG2a:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b00010101};
}

let Itinerary = II_LDA_3D_dmv_lda_f, DecoderMethod = "DecodeLDA_3D_dmv_lda_f", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dmv_lda_f : AIE2PS_inst_lda_instr32 <(outs OP_mFFa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b01010101};
}

let Itinerary = II_LDA_3D_dmv_lda_feg, DecoderMethod = "DecodeLDA_3D_dmv_lda_feg", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dmv_lda_feg : AIE2PS_inst_lda_instr32 <(outs OP_mFEGa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b0010100};
}

let Itinerary = II_LDA_3D_s16, DecoderMethod = "DecodeLDA_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_s16 : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d.s16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_3d__mPa__mDSa__lh
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b10, 0b00011};
}

let Itinerary = II_LDA_3D_s8, DecoderMethod = "DecodeLDA_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_s8 : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d.s8", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_3d__mPa__mDSa__lb
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b00, 0b00011};
}

let Itinerary = II_LDA_3D_u16, DecoderMethod = "DecodeLDA_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_u16 : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d.u16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_3d__mPa__mDSa__lhu
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b11, 0b00011};
}

let Itinerary = II_LDA_3D_u8, DecoderMethod = "DecodeLDA_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_u8 : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d.u8", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_3d__mPa__mDSa__lbu
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b01, 0b00011};
}

let Itinerary = II_LDA_TM_2D, DecoderMethod = "DecodeLDA_TM_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_TM_2D : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.tm.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b0000101};
}

let Itinerary = II_LDA_TM_3D, DecoderMethod = "DecodeLDA_TM_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_TM_3D : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.tm.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b0000101};
}

let Itinerary = II_LDA_TM_idx, mayLoad = true in
def LDA_TM_idx : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, eDJ:$dj), "lda.tm", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b0000101};
}

let Itinerary = II_LDA_TM_idx_imm, mayLoad = true in
def LDA_TM_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, c6s_step4:$imm), "lda.tm", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__idx_imm__mPa__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b0000101};
}

let Itinerary = II_LDA_TM_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_TM_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.tm", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b0000101};
}

let Itinerary = II_LDA_TM_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_TM_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, c6s_step4:$imm), "lda.tm", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm_imm__mPa__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b0000101};
}

let Itinerary = II_LDA_dml_lda2_scalar_EE_ld_idx, mayLoad = true in
def LDA_dml_lda2_scalar_EE_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mEEa:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b01000101};
}

let Itinerary = II_LDA_dml_lda2_scalar_EE_ld_idx_imm, mayLoad = true in
def LDA_dml_lda2_scalar_EE_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mEEa:$dst), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b01000101};
}

let Itinerary = II_LDA_dml_lda2_scalar_EE_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda2_scalar_EE_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mEEa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b01000101};
}

let Itinerary = II_LDA_dml_lda2_scalar_EE_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda2_scalar_EE_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mEEa:$dst, eP:$ptr_out), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dml_lda2_scalar_EE__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b01000101};
}

let Itinerary = II_LDA_dml_lda2_scalar_EE_spill, mayLoad = true, Uses = [sp] in
def LDA_dml_lda2_scalar_EE_spill : AIE2PS_inst_lda_instr32 <(outs OP_mEEa:$dst), (ins c13n_step8:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dml_lda2_scalar_EE__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8
bits<9> imm;
bits<4> dst;

let lda = {imm, dst, 0b11000101};
}

let Itinerary = II_LDA_dml_lda2_scalar_EG_ld_idx, mayLoad = true in
def LDA_dml_lda2_scalar_EG_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mEGa:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b0111011};
}

let Itinerary = II_LDA_dml_lda2_scalar_EG_ld_idx_imm, mayLoad = true in
def LDA_dml_lda2_scalar_EG_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mEGa:$dst), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b0111011};
}

let Itinerary = II_LDA_dml_lda2_scalar_EG_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda2_scalar_EG_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mEGa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b0111011};
}

let Itinerary = II_LDA_dml_lda2_scalar_EG_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda2_scalar_EG_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mEGa:$dst, eP:$ptr_out), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dml_lda2_scalar_EG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b0111011};
}

let Itinerary = II_LDA_dml_lda2_scalar_EG_spill, mayLoad = true, Uses = [sp] in
def LDA_dml_lda2_scalar_EG_spill : AIE2PS_inst_lda_instr32 <(outs OP_mEGa:$dst), (ins c13n_step8:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dml_lda2_scalar_EG__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b1111011};
}

let Itinerary = II_LDA_dml_lda2_scalar_GG_ld_idx, mayLoad = true in
def LDA_dml_lda2_scalar_GG_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mGGa:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b00100101};
}

let Itinerary = II_LDA_dml_lda2_scalar_GG_ld_idx_imm, mayLoad = true in
def LDA_dml_lda2_scalar_GG_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mGGa:$dst), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b00100101};
}

let Itinerary = II_LDA_dml_lda2_scalar_GG_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda2_scalar_GG_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mGGa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b00100101};
}

let Itinerary = II_LDA_dml_lda2_scalar_GG_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda2_scalar_GG_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mGGa:$dst, eP:$ptr_out), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dml_lda2_scalar_GG__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b00100101};
}

let Itinerary = II_LDA_dml_lda2_scalar_GG_spill, mayLoad = true, Uses = [sp] in
def LDA_dml_lda2_scalar_GG_spill : AIE2PS_inst_lda_instr32 <(outs OP_mGGa:$dst), (ins c13n_step8:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dml_lda2_scalar_GG__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8
bits<9> imm;
bits<4> dst;

let lda = {imm, dst, 0b10100101};
}

let Itinerary = II_LDA_dml_lda_scalar_F_ld_idx, mayLoad = true in
def LDA_dml_lda_scalar_F_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mFa:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b0000100};
}

let Itinerary = II_LDA_dml_lda_scalar_F_ld_idx_imm, mayLoad = true in
def LDA_dml_lda_scalar_F_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mFa:$dst), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b0000100};
}

let Itinerary = II_LDA_dml_lda_scalar_F_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda_scalar_F_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mFa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b0000100};
}

let Itinerary = II_LDA_dml_lda_scalar_F_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda_scalar_F_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mFa:$dst, eP:$ptr_out), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dml_lda_scalar_F__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b0000100};
}

let Itinerary = II_LDA_dml_lda_scalar_F_spill, mayLoad = true, Uses = [sp] in
def LDA_dml_lda_scalar_F_spill : AIE2PS_inst_lda_instr32 <(outs OP_mFa:$dst), (ins c13n_step8:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dml_lda_scalar_F__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b1000100};
}

let Itinerary = II_LDA_dml_lda_scalar_L_ld_idx, mayLoad = true in
def LDA_dml_lda_scalar_L_ld_idx : AIE2PS_inst_lda_instr32 <(outs eL:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx__mPa__mDJa__mLa
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b01100101};
}

let Itinerary = II_LDA_dml_lda_scalar_L_ld_idx_imm, mayLoad = true in
def LDA_dml_lda_scalar_L_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs eL:$dst), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__idx_imm__mPa__m_c07s_step8__mLa
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b01100101};
}

let Itinerary = II_LDA_dml_lda_scalar_L_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda_scalar_L_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs eL:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm__mPa__mMa__mLa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b01100101};
}

let Itinerary = II_LDA_dml_lda_scalar_L_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dml_lda_scalar_L_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs eL:$dst, eP:$ptr_out), (ins eP:$ptr, c7s_step8:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dml_lda_scalar_L__ld__agua_dml__nrm__agua_dml__normal__agua_dml__pstm_nrm_imm__mPa__m_c07s_step8__mLa
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b01100101};
}

let Itinerary = II_LDA_dml_lda_scalar_L_spill, mayLoad = true, Uses = [sp] in
def LDA_dml_lda_scalar_L_spill : AIE2PS_inst_lda_instr32 <(outs eL:$dst), (ins c13n_step8:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dml_lda_scalar_L__spill__agua_dml__nrm_spill__agua_dml__spill__mSPa__m_c13n_step8__mLa
bits<9> imm;
bits<4> dst;

let lda = {imm, dst, 0b11100101};
}

let Itinerary = II_LDA_dms_lda_g_ld_idx, mayLoad = true in
def LDA_dms_lda_g_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mGa:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b0100100};
}

let Itinerary = II_LDA_dms_lda_g_ld_idx_imm, mayLoad = true in
def LDA_dms_lda_g_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mGa:$dst), (ins eP:$ptr, c6s_step4:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__idx_imm__mPa__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b0100100};
}

let Itinerary = II_LDA_dms_lda_g_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dms_lda_g_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mGa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b0100100};
}

let Itinerary = II_LDA_dms_lda_g_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dms_lda_g_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mGa:$dst, eP:$ptr_out), (ins eP:$ptr, c6s_step4:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dms_lda_g__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm_imm__mPa__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b0100100};
}

let Itinerary = II_LDA_dms_lda_g_spill, mayLoad = true, Uses = [sp] in
def LDA_dms_lda_g_spill : AIE2PS_inst_lda_instr32 <(outs OP_mGa:$dst), (ins c12n_step4:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dms_lda_g__spill__agua_dms__nrm_spill__agua_dms__spill__mSPa__m_c12n_step4
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b1100100};
}

let Itinerary = II_LDA_dms_lda_scalar_ld_idx, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true in
def LDA_dms_lda_scalar_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mLdaScl:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<7> dst;

let lda = {ptr, dj, 0b000, dst, 0b10011};
}

let Itinerary = II_LDA_dms_lda_scalar_ld_idx_imm, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_idx_imm_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true in
def LDA_dms_lda_scalar_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mLdaScl:$dst), (ins eP:$ptr, c6s_step4:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__idx_imm__mPa__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<7> dst;

let lda = {ptr, imm, 0b01, dst, 0b10011};
}

let Itinerary = II_LDA_dms_lda_scalar_ld_pstm_nrm, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dms_lda_scalar_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mLdaScl:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<7> dst;

let lda = {ptr, mod, 0b010, dst, 0b10011};
}

let Itinerary = II_LDA_dms_lda_scalar_ld_pstm_nrm_imm, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_ld_pstm_nrm_imm_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dms_lda_scalar_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mLdaScl:$dst, eP:$ptr_out), (ins eP:$ptr, c6s_step4:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dms_lda_scalar__ld__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm_imm__mPa__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<7> dst;

let lda = {ptr, imm, 0b11, dst, 0b10011};
}

let Itinerary = II_LDA_dms_lda_scalar_spill, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_LDA_dms_lda_scalar_spill_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true, Uses = [sp] in
def LDA_dms_lda_scalar_spill : AIE2PS_inst_lda_instr32 <(outs OP_mLdaScl:$dst), (ins c12n_step4:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dms_lda_scalar__spill__agua_dms__nrm_spill__agua_dms__spill__mSPa__m_c12n_step4
bits<9> imm;
bits<7> dst;

let lda = {imm, dst, 0b01011};
}

let Itinerary = II_LDA_dmv_lda_eg2_ld_idx, mayLoad = true in
def LDA_dmv_lda_eg2_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mEG2a:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b00010101};
}

let Itinerary = II_LDA_dmv_lda_eg2_ld_idx_imm, mayLoad = true in
def LDA_dmv_lda_eg2_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mEG2a:$dst), (ins eP:$ptr, c8s_step16:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b00010101};
}

let Itinerary = II_LDA_dmv_lda_eg2_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dmv_lda_eg2_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mEG2a:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b00010101};
}

let Itinerary = II_LDA_dmv_lda_eg2_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dmv_lda_eg2_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mEG2a:$dst, eP:$ptr_out), (ins eP:$ptr, c8s_step16:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmv_lda_eg2__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b00010101};
}

let Itinerary = II_LDA_dmv_lda_eg2_spill, mayLoad = true, Uses = [sp] in
def LDA_dmv_lda_eg2_spill : AIE2PS_inst_lda_instr32 <(outs OP_mEG2a:$dst), (ins c14n_step16:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_eg2__spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
bits<9> imm;
bits<4> dst;

let lda = {imm, dst, 0b10010101};
}

let Itinerary = II_LDA_dmv_lda_f_ld_idx, mayLoad = true in
def LDA_dmv_lda_f_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mFFa:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b01010101};
}

let Itinerary = II_LDA_dmv_lda_f_ld_idx_imm, mayLoad = true in
def LDA_dmv_lda_f_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mFFa:$dst), (ins eP:$ptr, c8s_step16:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b01010101};
}

let Itinerary = II_LDA_dmv_lda_f_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dmv_lda_f_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mFFa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b01010101};
}

let Itinerary = II_LDA_dmv_lda_f_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dmv_lda_f_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mFFa:$dst, eP:$ptr_out), (ins eP:$ptr, c8s_step16:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmv_lda_f__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b01010101};
}

let Itinerary = II_LDA_dmv_lda_f_spill, mayLoad = true, Uses = [sp] in
def LDA_dmv_lda_f_spill : AIE2PS_inst_lda_instr32 <(outs OP_mFFa:$dst), (ins c14n_step16:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_f__spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
bits<9> imm;
bits<4> dst;

let lda = {imm, dst, 0b11010101};
}

let Itinerary = II_LDA_dmv_lda_feg_ld_idx, mayLoad = true in
def LDA_dmv_lda_feg_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mFEGa:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b0010100};
}

let Itinerary = II_LDA_dmv_lda_feg_ld_idx_imm, mayLoad = true in
def LDA_dmv_lda_feg_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mFEGa:$dst), (ins eP:$ptr, c8s_step16:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b0010100};
}

let Itinerary = II_LDA_dmv_lda_feg_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dmv_lda_feg_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mFEGa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b0010100};
}

let Itinerary = II_LDA_dmv_lda_feg_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dmv_lda_feg_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mFEGa:$dst, eP:$ptr_out), (ins eP:$ptr, c8s_step16:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmv_lda_feg__ld__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b0010100};
}

let Itinerary = II_LDA_dmv_lda_feg_spill, mayLoad = true, Uses = [sp] in
def LDA_dmv_lda_feg_spill : AIE2PS_inst_lda_instr32 <(outs OP_mFEGa:$dst), (ins c14n_step16:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_feg__spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b1010100};
}

let Itinerary = II_LDA_s16_idx, mayLoad = true in
def LDA_s16_idx : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, eDJ:$dj), "lda.s16", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx__mPa__mDJa__lh
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b10, 0b00011};
}

let Itinerary = II_LDA_s16_idx_imm, mayLoad = true in
def LDA_s16_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, c5s_step2:$imm), "lda.s16", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx_imm__mPa__m_c05s_step2__lh
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b10, 0b00011};
}

let Itinerary = II_LDA_s16_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_s16_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.s16", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm__mPa__mMa__lh
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b10, 0b00011};
}

let Itinerary = II_LDA_s16_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_s16_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, c5s_step2:$imm), "lda.s16", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm_imm__mPa__m_c05s_step2__lh
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b10, 0b00011};
}

let Itinerary = II_LDA_s8_idx, mayLoad = true in
def LDA_s8_idx : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, eDJ:$dj), "lda.s8", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx__mPa__mDJa__lb
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b00, 0b00011};
}

let Itinerary = II_LDA_s8_idx_imm, mayLoad = true in
def LDA_s8_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, c4s:$imm), "lda.s8", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx_imm__mPa__m_c04s__lb
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b00, 0b00011};
}

let Itinerary = II_LDA_s8_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_s8_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.s8", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm__mPa__mMa__lb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b00, 0b00011};
}

let Itinerary = II_LDA_s8_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_s8_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, c4s:$imm), "lda.s8", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm_imm__mPa__m_c04s__lb
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b00, 0b00011};
}

let Itinerary = II_LDA_u16_idx, mayLoad = true in
def LDA_u16_idx : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, eDJ:$dj), "lda.u16", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx__mPa__mDJa__lhu
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b11, 0b00011};
}

let Itinerary = II_LDA_u16_idx_imm, mayLoad = true in
def LDA_u16_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, c5s_step2:$imm), "lda.u16", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx_imm__mPa__m_c05s_step2__lhu
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b11, 0b00011};
}

let Itinerary = II_LDA_u16_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_u16_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.u16", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm__mPa__mMa__lhu
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b11, 0b00011};
}

let Itinerary = II_LDA_u16_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_u16_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, c5s_step2:$imm), "lda.u16", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm_imm__mPa__m_c05s_step2__lhu
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b11, 0b00011};
}

let Itinerary = II_LDA_u8_idx, mayLoad = true in
def LDA_u8_idx : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, eDJ:$dj), "lda.u8", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx__mPa__mDJa__lbu
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b01, 0b00011};
}

let Itinerary = II_LDA_u8_idx_imm, mayLoad = true in
def LDA_u8_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins eP:$ptr, c4s:$imm), "lda.u8", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx_imm__mPa__m_c04s__lbu
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b01, 0b00011};
}

let Itinerary = II_LDA_u8_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_u8_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.u8", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm__mPa__mMa__lbu
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b01, 0b00011};
}

let Itinerary = II_LDA_u8_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_u8_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst, eP:$ptr_out), (ins eP:$ptr, c4s:$imm), "lda.u8", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm_imm__mPa__m_c04s__lbu
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b01, 0b00011};
}

let Itinerary = II_LSHL in
def LSHL : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "lshl", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__uns
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1101, 0b1};
}

let Itinerary = II_LTU in
def LTU : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "ltu", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sltu
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1100, 0b1};
}

let Itinerary = II_LT in
def LT : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "lt", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__slt
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1010, 0b1};
}

let Itinerary = II_MAC, Constraints = "$a0 = $d0" in
def MAC : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRz:$a0, OP_mRx:$s0, OP_mRy:$s1), "mac", "$d0, $a0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mac_r_rrr
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0, 0b1100};
}

let Itinerary = II_MAXU in
def MAXU : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy8:$s1), "maxu", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr_ext__maxu
bits<5> s0;
bits<5> d0;
bits<3> s1;

let alu = {s0, d0, s1, 0b01, 0b1, 0b1000};
}

let Itinerary = II_MAX in
def MAX : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy8:$s1), "max", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr_ext__max
bits<5> s0;
bits<5> d0;
bits<3> s1;

let alu = {s0, d0, s1, 0b00, 0b1, 0b1000};
}

let Itinerary = II_MINU in
def MINU : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy8:$s1), "minu", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr_ext__minu
bits<5> s0;
bits<5> d0;
bits<3> s1;

let alu = {s0, d0, s1, 0b11, 0b1, 0b1000};
}

let Itinerary = II_MIN in
def MIN : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy8:$s1), "min", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr_ext__min
bits<5> s0;
bits<5> d0;
bits<3> s1;

let alu = {s0, d0, s1, 0b10, 0b1, 0b1000};
}

let Itinerary = II_MOVA, ItineraryRegPairs = [
ItinRegClassPair<II_MOVA_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOVA_mLCaCg, [OperandRegClass<0, mLCaCg>]>,
ItinRegClassPair<II_MOVA_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOVA_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_MOVA_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOVA_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOVA_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOVA_eRo, [OperandRegClass<0, eRo>]>], isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOVA : AIE2PS_inst_lda_instr32 <(outs OP_mLdaCg:$dst), (ins c11s:$i), "mova", "$dst, #$i">{
// id: me__instr128_or__lda__lda_cg
bits<11> i;
bits<7> dst;

let lda = {i{10-2}, dst, i{1-0}, 0b000};
}

let Itinerary = II_MOVS, ItineraryRegPairs = [
ItinRegClassPair<II_MOVS_eDJ_eDJ, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eDC_eDJ, [OperandRegClass<0, eDC>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eDN_eDJ, [OperandRegClass<0, eDN>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eP_eDJ, [OperandRegClass<0, eP>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eM_eDJ, [OperandRegClass<0, eM>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eDJ_eDC, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eDJ_eDN, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eDJ_eRe, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVS_eDJ_eP, [OperandRegClass<0, eDJ>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eDJ_eM, [OperandRegClass<0, eDJ>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOVS_eDJ_eRo, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVS_eDC_eDC, [OperandRegClass<0, eDC>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eDC_eDN, [OperandRegClass<0, eDC>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eDC_eRe, [OperandRegClass<0, eDC>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVS_eDC_eP, [OperandRegClass<0, eDC>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eDC_eM, [OperandRegClass<0, eDC>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOVS_eDC_eRo, [OperandRegClass<0, eDC>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVS_eDN_eDC, [OperandRegClass<0, eDN>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eDN_eDN, [OperandRegClass<0, eDN>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eDN_eRe, [OperandRegClass<0, eDN>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVS_eDN_eP, [OperandRegClass<0, eDN>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eDN_eM, [OperandRegClass<0, eDN>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOVS_eDN_eRo, [OperandRegClass<0, eDN>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVS_eP_eDC, [OperandRegClass<0, eP>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eP_eDN, [OperandRegClass<0, eP>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eP_eRe, [OperandRegClass<0, eP>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVS_eP_eP, [OperandRegClass<0, eP>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eP_eM, [OperandRegClass<0, eP>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOVS_eP_eRo, [OperandRegClass<0, eP>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVS_eM_eDC, [OperandRegClass<0, eM>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eM_eDN, [OperandRegClass<0, eM>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eM_eRe, [OperandRegClass<0, eM>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVS_eM_eP, [OperandRegClass<0, eM>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eM_eM, [OperandRegClass<0, eM>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOVS_eM_eRo, [OperandRegClass<0, eM>, OperandRegClass<1, eRo>]>] in
def MOVS : AIE2PS_inst_st_instr32 <(outs OP_mAguDst:$dst), (ins OP_mAguSrc:$src), "movs", "$dst, $src">{
// id: me__instr128_or__st__mvs_agu__agus__mv
bits<6> dst;
bits<7> src;

let st = {dst, dontcare{3-1}, src, 0b00110};
}

let Itinerary = II_MOVXM_lng_cg, ItineraryRegPairs = [
ItinRegClassPair<II_MOVXM_lng_cg_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOVXM_lng_cg_mLRm, [OperandRegClass<0, mLRm>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_MOVXM_lng_cg_mLCmCg, [OperandRegClass<0, mLCmCg>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eVaddSign, [OperandRegClass<0, eVaddSign>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eUnpackSign, [OperandRegClass<0, eUnpackSign>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eSRSSign, [OperandRegClass<0, eSRSSign>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eUPSSign, [OperandRegClass<0, eUPSSign>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eS, [OperandRegClass<0, eS>]>,
ItinRegClassPair<II_MOVXM_lng_cg_ePackSign, [OperandRegClass<0, ePackSign>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOVXM_lng_cg_mSPm, [OperandRegClass<0, mSPm>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOVXM_lng_cg_eRo, [OperandRegClass<0, eRo>]>], isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOVXM_lng_cg : AIE2PS_inst_lng_instr48 <(outs OP_mMvSclDstCg:$dst), (ins c32s:$i), "movxm", "$dst, #$i">{
// id: me__instr128_or__alu_mv__lng__lng_cg
bits<32> i;
bits<7> dst;

let lng = {i{31-12}, dst, i{11-0}, 0b001};
}

let Itinerary = II_MOVXM_lng_cg_le_abs, isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true, Defs = [le] in
def MOVXM_lng_cg_le_abs : AIE2PS_inst_lng_instr48 <(outs), (ins cpmaddr:$i), "movxm", "le, #$i">{
// id: me__instr128_or__alu_mv__lng__lng_cg_le_abs
bits<20> i;

let lng = {dontcare{5-1}, i, dontcare{12-1}, 0b01, 0b101};
}

let Itinerary = II_MOVXM_lng_cg_ls_abs, isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true, Defs = [ls] in
def MOVXM_lng_cg_ls_abs : AIE2PS_inst_lng_instr48 <(outs), (ins cpmaddr:$i), "movxm", "ls, #$i">{
// id: me__instr128_or__alu_mv__lng__lng_cg_ls_abs
bits<20> i;

let lng = {dontcare{5-1}, i, dontcare{12-1}, 0b00, 0b101};
}

let Itinerary = II_MOVX_alu_cg_or, isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOVX_alu_cg_or : AIE2PS_inst_alu_instr32 <(outs OP_mAluCg:$dst), (ins c11s:$i), "movx", "$dst, #$i">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_cg_or__alu_cg
bits<11> i;
bits<6> dst;

let alu = {i{10-6}, dst, i{5-0}, 0b010};
}

let Itinerary = II_MOVX_mvx_cr_imm, ItineraryRegPairs = [
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRFPNlfMask, [OperandRegClass<0, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRSCDEn, [OperandRegClass<0, mCRSCDEn>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRPackSize, [OperandRegClass<0, mCRPackSize>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRRnd, [OperandRegClass<0, mCRRnd>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRSat, [OperandRegClass<0, mCRSat>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRFPMask, [OperandRegClass<0, mCRFPMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRF2IMask, [OperandRegClass<0, mCRF2IMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRF2FMask, [OperandRegClass<0, mCRF2FMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_eVaddSign, [OperandRegClass<0, eVaddSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRF2BMask, [OperandRegClass<0, mCRF2BMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_eSRSSign, [OperandRegClass<0, eSRSSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_eUnpackSign, [OperandRegClass<0, eUnpackSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRFPConvSat, [OperandRegClass<0, mCRFPConvSat>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRUPSMode, [OperandRegClass<0, mCRUPSMode>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRBF8conf, [OperandRegClass<0, mCRBF8conf>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_eUPSSign, [OperandRegClass<0, eUPSSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRUnpackSize, [OperandRegClass<0, mCRUnpackSize>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_ePackSign, [OperandRegClass<0, ePackSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRMCDEn, [OperandRegClass<0, mCRMCDEn>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRFP8conf, [OperandRegClass<0, mCRFP8conf>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRSRSMode, [OperandRegClass<0, mCRSRSMode>]>], isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOVX_mvx_cr_imm : AIE2PS_inst_alu_instr32 <(outs OP_mCRx:$dst), (ins c5u:$src), "movx", "$dst, #$src">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mvx_cr_imm
bits<5> src;
bits<6> dst;

let alu = {src, 0b10, dst, 0b000, 0b0000};
}

let Itinerary = II_MOVX_mvx_cr_r, ItineraryRegPairs = [
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPNlfMask_eRe, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRSCDEn_eRe, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRPackSize_eRe, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRRnd_eRe, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRSat_eRe, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPMask_eRe, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRF2IMask_eRe, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRF2FMask_eRe, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eVaddSign_eRe, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRF2BMask_eRe, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPCnvFl2FxMask_eRe, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eSRSSign_eRe, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eUnpackSign_eRe, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPConvSat_eRe, [OperandRegClass<0, mCRFPConvSat>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRUPSMode_eRe, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRBF8conf_eRe, [OperandRegClass<0, mCRBF8conf>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eUPSSign_eRe, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRUnpackSize_eRe, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_ePackSign_eRe, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRMCDEn_eRe, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPCnvFx2FlMask_eRe, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFP8conf_eRe, [OperandRegClass<0, mCRFP8conf>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRSRSMode_eRe, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPNlfMask_eRo, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRSCDEn_eRo, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRPackSize_eRo, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRRnd_eRo, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRSat_eRo, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPMask_eRo, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRF2IMask_eRo, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRF2FMask_eRo, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eVaddSign_eRo, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRF2BMask_eRo, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPCnvFl2FxMask_eRo, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eSRSSign_eRo, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eUnpackSign_eRo, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPConvSat_eRo, [OperandRegClass<0, mCRFPConvSat>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRUPSMode_eRo, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRBF8conf_eRo, [OperandRegClass<0, mCRBF8conf>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eUPSSign_eRo, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRUnpackSize_eRo, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_ePackSign_eRo, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRMCDEn_eRo, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPCnvFx2FlMask_eRo, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFP8conf_eRo, [OperandRegClass<0, mCRFP8conf>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRSRSMode_eRo, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eRo>]>] in
def MOVX_mvx_cr_r : AIE2PS_inst_alu_instr32 <(outs OP_mCRx:$dst), (ins OP_mRx:$src), "movx", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mvx_cr_r
bits<5> src;
bits<6> dst;

let alu = {src, 0b11, dst, 0b000, 0b0000};
}

let Itinerary = II_MOV_CPH_mMStream_tlast_imm, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_mMStream_tlast_imm : AIE2PS_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, OP_mRs:$id), "mov.cph", " ms, $addr, #$nw, #$op, $id">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__noTlast__mMs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b0, 0b0, 0b0, 0b0, op, addr, id, nw, 0b10110};
}

let Itinerary = II_MOV_CPH_mMStream_tlast_reg, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_mMStream_tlast_reg : AIE2PS_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, OP_mRs:$id, mR28_tlast:$tlast), "mov.cph", " ms, $addr, #$nw, #$op, $id, r28">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_reg__mR28_tlast__mMs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b0, 0b1, dontcare{1}, 0b0, op, addr, id, nw, 0b10110};
}

let Itinerary = II_MOV_CPH_nb_mMStream_tlast_imm, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_nb_mMStream_tlast_imm : AIE2PS_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, OP_mRs:$id), "mov.cph.nb", " ms, $addr, #$nw, #$op, $id">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__noTlast__mMs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b1, 0b0, 0b0, 0b0, op, addr, id, nw, 0b10110};
}

let Itinerary = II_MOV_CPH_nb_mMStream_tlast_reg, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_nb_mMStream_tlast_reg : AIE2PS_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, OP_mRs:$id, mR28_tlast:$tlast), "mov.cph.nb", " ms, $addr, #$nw, #$op, $id, r28">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_reg__mR28_tlast__mMs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b1, 0b1, dontcare{1}, 0b0, op, addr, id, nw, 0b10110};
}

let Itinerary = II_MOV_CPH_nb_tlast, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_nb_tlast : AIE2PS_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, OP_mRs:$id), "mov.cph.nb.tlast", " ms, $addr, #$nw, #$op, $id">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__doTlast__mMs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b1, 0b0, 0b1, 0b0, op, addr, id, nw, 0b10110};
}

let Itinerary = II_MOV_CPH_tlast, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_tlast : AIE2PS_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, OP_mRs:$id), "mov.cph.tlast", " ms, $addr, #$nw, #$op, $id">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__doTlast__mMs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b0, 0b0, 0b1, 0b0, op, addr, id, nw, 0b10110};
}

let Itinerary = II_MOV_PH_mMStream_tlast_imm, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_mMStream_tlast_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mRs:$id, c3u:$pcktType), "mov.ph", " ms, $id, #$pcktType">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__noTlast
bits<3> pcktType;
bits<5> id;

let st = {0b0, 0b0, 0b0, 0b100, pcktType, id, dontcare{2-1}, 0b10110};
}

let Itinerary = II_MOV_PH_mMStream_tlast_reg, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_mMStream_tlast_reg : AIE2PS_inst_st_instr32 <(outs), (ins OP_mRs:$id, c3u:$pcktType, mR28_tlast:$tlast), "mov.ph", " ms, $id, #$pcktType, r28">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
bits<3> pcktType;
bits<5> id;

let st = {0b0, 0b1, dontcare{1}, 0b100, pcktType, id, dontcare{2-1}, 0b10110};
}

let Itinerary = II_MOV_PH_nb_mMStream_tlast_imm, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_nb_mMStream_tlast_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mRs:$id, c3u:$pcktType), "mov.ph.nb", " ms, $id, #$pcktType">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__noTlast
bits<3> pcktType;
bits<5> id;

let st = {0b1, 0b0, 0b0, 0b100, pcktType, id, dontcare{2-1}, 0b10110};
}

let Itinerary = II_MOV_PH_nb_mMStream_tlast_reg, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_nb_mMStream_tlast_reg : AIE2PS_inst_st_instr32 <(outs), (ins OP_mRs:$id, c3u:$pcktType, mR28_tlast:$tlast), "mov.ph.nb", " ms, $id, #$pcktType, r28">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
bits<3> pcktType;
bits<5> id;

let st = {0b1, 0b1, dontcare{1}, 0b100, pcktType, id, dontcare{2-1}, 0b10110};
}

let Itinerary = II_MOV_PH_nb_tlast, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_nb_tlast : AIE2PS_inst_st_instr32 <(outs), (ins OP_mRs:$id, c3u:$pcktType), "mov.ph.nb.tlast", " ms, $id, #$pcktType">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__doTlast
bits<3> pcktType;
bits<5> id;

let st = {0b1, 0b0, 0b1, 0b100, pcktType, id, dontcare{2-1}, 0b10110};
}

let Itinerary = II_MOV_PH_tlast, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_tlast : AIE2PS_inst_st_instr32 <(outs), (ins OP_mRs:$id, c3u:$pcktType), "mov.ph.tlast", " ms, $id, #$pcktType">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__doTlast
bits<3> pcktType;
bits<5> id;

let st = {0b0, 0b0, 0b1, 0b100, pcktType, id, dontcare{2-1}, 0b10110};
}

let Itinerary = II_MOV_alu_mv_alu_flt2fx, Defs = [srFPCnvFl2Fx], Uses = [crFPCnvFl2FxMask] in
def MOV_alu_mv_alu_flt2fx : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRx:$s0, mS3:$s3), "mov", "$d0, ${s0}.flt2fx, s3">{
// id: me__instr128_or__alu_mv__alumv_or__alu__flt2fx__mFlt2fx__mS3
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, dontcare{3-1}, 0b010, 0b1000};
}

let Itinerary = II_MOV_alu_mv_alu_fx2flt, Defs = [srFPCnvFx2Fl], Uses = [crFPCnvFx2FlMask] in
def MOV_alu_mv_alu_fx2flt : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRx:$s0, mS2:$s2), "mov", "$d0, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__fx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, dontcare{2-1}, 0b0, 0b110, 0b1000};
}

let Itinerary = II_MOV_alu_mv_mv_mv_cg_or, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_mLRm, [OperandRegClass<0, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_mLCmCg, [OperandRegClass<0, mLCmCg>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eVaddSign, [OperandRegClass<0, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eUnpackSign, [OperandRegClass<0, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eSRSSign, [OperandRegClass<0, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eUPSSign, [OperandRegClass<0, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eS, [OperandRegClass<0, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_ePackSign, [OperandRegClass<0, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_mSPm, [OperandRegClass<0, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_or_eRo, [OperandRegClass<0, eRo>]>], isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOV_alu_mv_mv_mv_cg_or : AIE2PS_inst_mv_instr32 <(outs OP_mMvSclDstCg:$dst), (ins c11s:$i), "mov", "$dst, #$i">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cg_or__mv_cg
bits<7> dst;
bits<11> i;

let mv = {dst, i{6-0}, i{10-7}, 0b0101};
}

let Itinerary = II_MOV_alu_mv_mv_mv_cntr2l, hasSideEffects = true in
def MOV_alu_mv_mv_mv_cntr2l : AIE2PS_inst_mv_instr32 <(outs eL:$dst), (ins), "mov", "$dst, cntr">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cntr2l__mLm
bits<4> dst;

let mv = {dst, dontcare{3-1}, 0b001100001101111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_eh_to_eh in
def MOV_alu_mv_mv_mv_e_mv_eh_to_eh : AIE2PS_inst_mv_instr32 <(outs OP_mEhm:$dst), (ins OP_mEhm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_eh_to_eh
bits<4> dst;
bits<4> src;

let mv = {dst, 0b10, src, 0b10, 0b0011001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_eh_to_el in
def MOV_alu_mv_mv_mv_e_mv_eh_to_el : AIE2PS_inst_mv_instr32 <(outs OP_mElm:$dst), (ins OP_mEhm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_eh_to_el
bits<4> dst;
bits<4> src;

let mv = {dst, 0b00, src, 0b10, 0b0011001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_eh_to_r in
def MOV_alu_mv_mv_mv_e_mv_eh_to_r : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mEhm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_eh_to_r
bits<5> dst;
bits<4> src;

let mv = {dst, 0b1, src, 0b10, 0b0011001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_el_to_eh in
def MOV_alu_mv_mv_mv_e_mv_el_to_eh : AIE2PS_inst_mv_instr32 <(outs OP_mEhm:$dst), (ins OP_mElm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_el_to_eh
bits<4> dst;
bits<4> src;

let mv = {dst, 0b10, src, 0b00, 0b0011001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_el_to_el in
def MOV_alu_mv_mv_mv_e_mv_el_to_el : AIE2PS_inst_mv_instr32 <(outs OP_mElm:$dst), (ins OP_mElm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_el_to_el
bits<4> dst;
bits<4> src;

let mv = {dst, 0b00, src, 0b00, 0b0011001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_el_to_r in
def MOV_alu_mv_mv_mv_e_mv_el_to_r : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mElm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_el_to_r
bits<5> dst;
bits<4> src;

let mv = {dst, 0b1, src, 0b00, 0b0011001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_r_to_eh in
def MOV_alu_mv_mv_mv_e_mv_r_to_eh : AIE2PS_inst_mv_instr32 <(outs OP_mEhm:$dst), (ins OP_mRm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_r_to_eh
bits<4> dst;
bits<5> src;

let mv = {dst, 0b10, src, 0b1, 0b0011001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_r_to_el in
def MOV_alu_mv_mv_mv_e_mv_r_to_el : AIE2PS_inst_mv_instr32 <(outs OP_mElm:$dst), (ins OP_mRm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_r_to_el
bits<4> dst;
bits<5> src;

let mv = {dst, 0b00, src, 0b1, 0b0011001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_g_mv_gh_to_gh in
def MOV_alu_mv_mv_mv_g_mv_gh_to_gh : AIE2PS_inst_mv_instr32 <(outs OP_mGhm:$dst), (ins OP_mGhm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gh_to_gh
bits<4> dst;
bits<4> src;

let mv = {dst, 0b10, src, 0b10, 0b0111001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_g_mv_gh_to_gl in
def MOV_alu_mv_mv_mv_g_mv_gh_to_gl : AIE2PS_inst_mv_instr32 <(outs OP_mGlm:$dst), (ins OP_mGhm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gh_to_gl
bits<4> dst;
bits<4> src;

let mv = {dst, 0b00, src, 0b10, 0b0111001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_g_mv_gh_to_r in
def MOV_alu_mv_mv_mv_g_mv_gh_to_r : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mGhm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gh_to_r
bits<5> dst;
bits<4> src;

let mv = {dst, 0b1, src, 0b10, 0b0111001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_g_mv_gl_to_gh in
def MOV_alu_mv_mv_mv_g_mv_gl_to_gh : AIE2PS_inst_mv_instr32 <(outs OP_mGhm:$dst), (ins OP_mGlm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gl_to_gh
bits<4> dst;
bits<4> src;

let mv = {dst, 0b10, src, 0b00, 0b0111001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_g_mv_gl_to_gl in
def MOV_alu_mv_mv_mv_g_mv_gl_to_gl : AIE2PS_inst_mv_instr32 <(outs OP_mGlm:$dst), (ins OP_mGlm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gl_to_gl
bits<4> dst;
bits<4> src;

let mv = {dst, 0b00, src, 0b00, 0b0111001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_g_mv_gl_to_r in
def MOV_alu_mv_mv_mv_g_mv_gl_to_r : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mGlm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_gl_to_r
bits<5> dst;
bits<4> src;

let mv = {dst, 0b1, src, 0b00, 0b0111001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_g_mv_r_to_gh in
def MOV_alu_mv_mv_mv_g_mv_r_to_gh : AIE2PS_inst_mv_instr32 <(outs OP_mGhm:$dst), (ins OP_mRm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_r_to_gh
bits<4> dst;
bits<5> src;

let mv = {dst, 0b10, src, 0b1, 0b0111001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_g_mv_r_to_gl in
def MOV_alu_mv_mv_mv_g_mv_r_to_gl : AIE2PS_inst_mv_instr32 <(outs OP_mGlm:$dst), (ins OP_mRm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_g__mv_r_to_gl
bits<4> dst;
bits<5> src;

let mv = {dst, 0b00, src, 0b1, 0b0111001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_r_sc in
def MOV_alu_mv_mv_mv_r_sc : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mSCm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_r_sc
bits<5> dst;
bits<6> src;

let mv = {dst, src, 0b01000101111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_sc_cg, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRF2IFlags, [OperandRegClass<0, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPNlfMask, [OperandRegClass<0, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRFifo_of, [OperandRegClass<0, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRPackSize, [OperandRegClass<0, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRRnd, [OperandRegClass<0, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRCarry, [OperandRegClass<0, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRF2IMask, [OperandRegClass<0, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRF2FMask, [OperandRegClass<0, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRF2BMask, [OperandRegClass<0, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRF2FFlags, [OperandRegClass<0, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPConvSat, [OperandRegClass<0, mCRFPConvSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRBF8conf, [OperandRegClass<0, mCRBF8conf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRFifo_uf, [OperandRegClass<0, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRMCDEn, [OperandRegClass<0, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRMS0, [OperandRegClass<0, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRUPS_of, [OperandRegClass<0, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRSparse_of, [OperandRegClass<0, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRSRSMode, [OperandRegClass<0, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRSCDEn, [OperandRegClass<0, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRSat, [OperandRegClass<0, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPMask, [OperandRegClass<0, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRSRS_of, [OperandRegClass<0, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRFPFlags, [OperandRegClass<0, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRF2BFlags, [OperandRegClass<0, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRUPSMode, [OperandRegClass<0, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRFPNlf, [OperandRegClass<0, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRUnpackSize, [OperandRegClass<0, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mSRSS0, [OperandRegClass<0, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_cg_mCRFP8conf, [OperandRegClass<0, mCRFP8conf>]>], isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOV_alu_mv_mv_mv_sc_cg : AIE2PS_inst_mv_instr32 <(outs OP_mSCm:$dst), (ins c8s:$i), "mov", "$dst, #$i">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_sc_cg
bits<6> dst;
bits<8> i;

let mv = {dst, i, 0b00010111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_sc_r, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRF2IFlags_eRe, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPNlfMask_eRe, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFl2Fx_eRe, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_of_eRe, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRPackSize_eRe, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRRnd_eRe, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRCarry_eRe, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRF2IMask_eRe, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRF2FMask_eRe, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRF2BMask_eRe, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRF2FFlags_eRe, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFx2Fl_eRe, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPConvSat_eRe, [OperandRegClass<0, mCRFPConvSat>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRBF8conf_eRe, [OperandRegClass<0, mCRBF8conf>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_uf_eRe, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRMCDEn_eRe, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFx2FlMask_eRe, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRMS0_eRe, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRUPS_of_eRe, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRSparse_of_eRe, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRSRSMode_eRe, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRSCDEn_eRe, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRSat_eRe, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPMask_eRe, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFl2FxMask_eRe, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRSRS_of_eRe, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFPFlags_eRe, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRF2BFlags_eRe, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRUPSMode_eRe, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFPNlf_eRe, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRUnpackSize_eRe, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRSS0_eRe, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFP8conf_eRe, [OperandRegClass<0, mCRFP8conf>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRF2IFlags_eRo, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPNlfMask_eRo, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFl2Fx_eRo, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_of_eRo, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRPackSize_eRo, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRRnd_eRo, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRCarry_eRo, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRF2IMask_eRo, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRF2FMask_eRo, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRF2BMask_eRo, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRF2FFlags_eRo, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFPCnvFx2Fl_eRo, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPConvSat_eRo, [OperandRegClass<0, mCRFPConvSat>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRBF8conf_eRo, [OperandRegClass<0, mCRBF8conf>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFifo_uf_eRo, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRMCDEn_eRo, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFx2FlMask_eRo, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRMS0_eRo, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRUPS_of_eRo, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRSparse_of_eRo, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRSRSMode_eRo, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRSCDEn_eRo, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRSat_eRo, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPMask_eRo, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFPCnvFl2FxMask_eRo, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRSRS_of_eRo, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFPFlags_eRo, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRF2BFlags_eRo, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRUPSMode_eRo, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRFPNlf_eRo, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRUnpackSize_eRo, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mSRSS0_eRo, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_sc_r_mCRFP8conf_eRo, [OperandRegClass<0, mCRFP8conf>, OperandRegClass<1, eRo>]>] in
def MOV_alu_mv_mv_mv_sc_r : AIE2PS_inst_mv_instr32 <(outs OP_mSCm:$dst), (ins OP_mRm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_sc_r
bits<6> dst;
bits<5> src;

let mv = {dst, dontcare{1}, src, 0b1011001111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_scl, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mLEm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mLEm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mLEm, [OperandRegClass<0, eDC>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_mLEm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLEm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mLEm, [OperandRegClass<0, eP>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLEm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLEm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLEm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mLEm, [OperandRegClass<0, eS>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mLEm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLEm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mLEm, [OperandRegClass<0, eDN>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mLEm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mLEm, [OperandRegClass<0, eM>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_mLEm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eDJ, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mLRm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eDC, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eRe, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mLSm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eVaddSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eP, [OperandRegClass<0, eDJ>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eUnpackSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eSRSSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCoreID, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eUPSSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eS, [OperandRegClass<0, eDJ>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mLCm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_ePackSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eDN, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSPm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eM, [OperandRegClass<0, eDJ>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eRo, [OperandRegClass<0, eDJ>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eDJ, [OperandRegClass<0, mLRm>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mLRm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eDC, [OperandRegClass<0, mLRm>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eRe, [OperandRegClass<0, mLRm>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mLSm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eVaddSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eP, [OperandRegClass<0, mLRm>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eUnpackSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eSRSSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCoreID, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eUPSSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eS, [OperandRegClass<0, mLRm>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mLCm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_ePackSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eDN, [OperandRegClass<0, mLRm>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSPm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eM, [OperandRegClass<0, mLRm>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eRo, [OperandRegClass<0, mLRm>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eDJ, [OperandRegClass<0, eDC>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mLRm, [OperandRegClass<0, eDC>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eDC, [OperandRegClass<0, eDC>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eRe, [OperandRegClass<0, eDC>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mLSm, [OperandRegClass<0, eDC>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eVaddSign, [OperandRegClass<0, eDC>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eP, [OperandRegClass<0, eDC>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eUnpackSign, [OperandRegClass<0, eDC>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eSRSSign, [OperandRegClass<0, eDC>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCoreID, [OperandRegClass<0, eDC>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eUPSSign, [OperandRegClass<0, eDC>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eS, [OperandRegClass<0, eDC>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mLCm, [OperandRegClass<0, eDC>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_ePackSign, [OperandRegClass<0, eDC>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eDN, [OperandRegClass<0, eDC>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSPm, [OperandRegClass<0, eDC>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eM, [OperandRegClass<0, eDC>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eRo, [OperandRegClass<0, eDC>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eDJ, [OperandRegClass<0, eRe>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_mLRm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eDC, [OperandRegClass<0, eRe>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_mLSm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eVaddSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eP, [OperandRegClass<0, eRe>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eUnpackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eSRSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_mCoreID, [OperandRegClass<0, eRe>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eUPSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eS, [OperandRegClass<0, eRe>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_mLCm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_ePackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eDN, [OperandRegClass<0, eRe>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_mSPm, [OperandRegClass<0, eRe>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eM, [OperandRegClass<0, eRe>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRe_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDJ, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLRm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDC, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eRe, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLSm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eVaddSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eP, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eUnpackSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eSRSSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCoreID, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eUPSSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eS, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLCm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_ePackSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDN, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSPm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eM, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eRo, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eDJ, [OperandRegClass<0, eP>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mLRm, [OperandRegClass<0, eP>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eDC, [OperandRegClass<0, eP>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eRe, [OperandRegClass<0, eP>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mLSm, [OperandRegClass<0, eP>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eVaddSign, [OperandRegClass<0, eP>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eP, [OperandRegClass<0, eP>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eUnpackSign, [OperandRegClass<0, eP>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eSRSSign, [OperandRegClass<0, eP>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCoreID, [OperandRegClass<0, eP>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eUPSSign, [OperandRegClass<0, eP>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eS, [OperandRegClass<0, eP>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mLCm, [OperandRegClass<0, eP>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_ePackSign, [OperandRegClass<0, eP>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eDN, [OperandRegClass<0, eP>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSPm, [OperandRegClass<0, eP>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eM, [OperandRegClass<0, eP>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eRo, [OperandRegClass<0, eP>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDJ, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLRm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDC, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eRe, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLSm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eVaddSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eP, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eUnpackSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eSRSSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCoreID, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eUPSSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eS, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLCm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_ePackSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDN, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSPm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eM, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eRo, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDJ, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLRm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDC, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eRe, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLSm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eVaddSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eP, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eUnpackSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eSRSSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCoreID, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eUPSSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eS, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLCm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_ePackSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDN, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSPm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eM, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eRo, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDJ, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLRm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDC, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eRe, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLSm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eVaddSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eP, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eUnpackSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eSRSSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCoreID, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eUPSSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eS, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLCm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_ePackSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDN, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSPm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eM, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eRo, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eDJ, [OperandRegClass<0, eS>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mLRm, [OperandRegClass<0, eS>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eDC, [OperandRegClass<0, eS>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eRe, [OperandRegClass<0, eS>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mLSm, [OperandRegClass<0, eS>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eVaddSign, [OperandRegClass<0, eS>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eP, [OperandRegClass<0, eS>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eUnpackSign, [OperandRegClass<0, eS>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eSRSSign, [OperandRegClass<0, eS>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCoreID, [OperandRegClass<0, eS>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eUPSSign, [OperandRegClass<0, eS>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eS, [OperandRegClass<0, eS>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mLCm, [OperandRegClass<0, eS>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_ePackSign, [OperandRegClass<0, eS>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eDN, [OperandRegClass<0, eS>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSPm, [OperandRegClass<0, eS>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eM, [OperandRegClass<0, eS>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eRo, [OperandRegClass<0, eS>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eDJ, [OperandRegClass<0, mLCm>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mLRm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eDC, [OperandRegClass<0, mLCm>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eRe, [OperandRegClass<0, mLCm>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mLSm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eVaddSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eP, [OperandRegClass<0, mLCm>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eUnpackSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eSRSSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCoreID, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eUPSSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eS, [OperandRegClass<0, mLCm>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mLCm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_ePackSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eDN, [OperandRegClass<0, mLCm>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSPm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eM, [OperandRegClass<0, mLCm>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eRo, [OperandRegClass<0, mLCm>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eDJ, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLRm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eDC, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eRe, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLSm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eVaddSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eP, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eUnpackSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eSRSSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCoreID, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eUPSSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eS, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLCm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_ePackSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eDN, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSPm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eM, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eRo, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eDJ, [OperandRegClass<0, eDN>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mLRm, [OperandRegClass<0, eDN>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eDC, [OperandRegClass<0, eDN>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eRe, [OperandRegClass<0, eDN>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mLSm, [OperandRegClass<0, eDN>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eVaddSign, [OperandRegClass<0, eDN>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eP, [OperandRegClass<0, eDN>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eUnpackSign, [OperandRegClass<0, eDN>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eSRSSign, [OperandRegClass<0, eDN>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCoreID, [OperandRegClass<0, eDN>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eUPSSign, [OperandRegClass<0, eDN>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eS, [OperandRegClass<0, eDN>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mLCm, [OperandRegClass<0, eDN>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_ePackSign, [OperandRegClass<0, eDN>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eDN, [OperandRegClass<0, eDN>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSPm, [OperandRegClass<0, eDN>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eM, [OperandRegClass<0, eDN>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eRo, [OperandRegClass<0, eDN>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eDJ, [OperandRegClass<0, mSPm>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mLRm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eDC, [OperandRegClass<0, mSPm>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eRe, [OperandRegClass<0, mSPm>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mLSm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eVaddSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eP, [OperandRegClass<0, mSPm>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eUnpackSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eSRSSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCoreID, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eUPSSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eS, [OperandRegClass<0, mSPm>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mLCm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_ePackSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eDN, [OperandRegClass<0, mSPm>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSPm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eM, [OperandRegClass<0, mSPm>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eRo, [OperandRegClass<0, mSPm>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eDJ, [OperandRegClass<0, eM>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mLRm, [OperandRegClass<0, eM>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eDC, [OperandRegClass<0, eM>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eRe, [OperandRegClass<0, eM>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mLSm, [OperandRegClass<0, eM>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eVaddSign, [OperandRegClass<0, eM>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eP, [OperandRegClass<0, eM>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eUnpackSign, [OperandRegClass<0, eM>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eSRSSign, [OperandRegClass<0, eM>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCoreID, [OperandRegClass<0, eM>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eUPSSign, [OperandRegClass<0, eM>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eS, [OperandRegClass<0, eM>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mLCm, [OperandRegClass<0, eM>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_ePackSign, [OperandRegClass<0, eM>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eDN, [OperandRegClass<0, eM>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSPm, [OperandRegClass<0, eM>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eM, [OperandRegClass<0, eM>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eRo, [OperandRegClass<0, eM>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eDJ, [OperandRegClass<0, eRo>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_mLRm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eDC, [OperandRegClass<0, eRo>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_mLSm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eVaddSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eP, [OperandRegClass<0, eRo>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eUnpackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eSRSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_mCoreID, [OperandRegClass<0, eRo>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eUPSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eS, [OperandRegClass<0, eRo>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_mLCm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_ePackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eDN, [OperandRegClass<0, eRo>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_mSPm, [OperandRegClass<0, eRo>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eM, [OperandRegClass<0, eRo>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eRo_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>]>] in
def MOV_alu_mv_mv_mv_scl : AIE2PS_inst_mv_instr32 <(outs OP_mMvSclDst:$dst), (ins OP_mMvSclSrc:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_scl
bits<7> dst;
bits<7> src;

let mv = {dst, src, 0b10010111};
}

let Itinerary = II_MOV_lda, hasSideEffects = true, Defs = [srSS0] in
def MOV_lda : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins), "mov", " $dst, ss">{
// id: me__instr128_or__lda__mv_ss2scl__mSStream__mSStream_b
bits<5> dst;

let lda = {0b0, dontcare{8-1}, dst, 0b1111100};
}

let Itinerary = II_MOV_st_mMStream_tlast_imm, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eRo, [OperandRegClass<0, eRo>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_st_mMStream_tlast_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mSclMS:$src), "mov", " ms, $src">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__noTlast
bits<7> src;

let st = {0b0, 0b0, 0b0, 0b101, dontcare{3-1}, src, 0b10110};
}

let Itinerary = II_MOV_st_mMStream_tlast_reg, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eRo, [OperandRegClass<0, eRo>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_st_mMStream_tlast_reg : AIE2PS_inst_st_instr32 <(outs), (ins OP_mSclMS:$src, mR28_tlast:$tlast), "mov", " ms, $src, r28">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
bits<7> src;

let st = {0b0, 0b1, dontcare{1}, 0b101, dontcare{3-1}, src, 0b10110};
}

let Itinerary = II_MOV_d1, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d1_eRe_mLEm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d1_eRo_mLEm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d1_eRe_eDJ, [OperandRegClass<0, eRe>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d1_eRe_mLRm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d1_eRe_eDC, [OperandRegClass<0, eRe>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d1_eRe_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d1_eRe_mLSm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d1_eRe_eVaddSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d1_eRe_eP, [OperandRegClass<0, eRe>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d1_eRe_eUnpackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d1_eRe_eSRSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d1_eRe_mCoreID, [OperandRegClass<0, eRe>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d1_eRe_eUPSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d1_eRe_eS, [OperandRegClass<0, eRe>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d1_eRe_mLCm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d1_eRe_ePackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d1_eRe_eDN, [OperandRegClass<0, eRe>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d1_eRe_mSPm, [OperandRegClass<0, eRe>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d1_eRe_eM, [OperandRegClass<0, eRe>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d1_eRe_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_d1_eRo_eDJ, [OperandRegClass<0, eRo>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d1_eRo_mLRm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d1_eRo_eDC, [OperandRegClass<0, eRo>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d1_eRo_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d1_eRo_mLSm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d1_eRo_eVaddSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d1_eRo_eP, [OperandRegClass<0, eRo>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d1_eRo_eUnpackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d1_eRo_eSRSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d1_eRo_mCoreID, [OperandRegClass<0, eRo>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d1_eRo_eUPSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d1_eRo_eS, [OperandRegClass<0, eRo>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d1_eRo_mLCm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d1_eRo_ePackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d1_eRo_eDN, [OperandRegClass<0, eRo>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d1_eRo_mSPm, [OperandRegClass<0, eRo>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d1_eRo_eM, [OperandRegClass<0, eRo>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d1_eRo_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>]>] in
def MOV_d1 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mMvSclSrc:$src), "mov.d1", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay1
bits<5> dst;
bits<7> src;

let mv = {dst, 0b00, src, 0b0, 0b0111011};
}

let Itinerary = II_MOV_d2, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d2_eRe_mLEm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d2_eRo_mLEm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d2_eRe_eDJ, [OperandRegClass<0, eRe>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d2_eRe_mLRm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d2_eRe_eDC, [OperandRegClass<0, eRe>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d2_eRe_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d2_eRe_mLSm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d2_eRe_eVaddSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d2_eRe_eP, [OperandRegClass<0, eRe>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d2_eRe_eUnpackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d2_eRe_eSRSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d2_eRe_mCoreID, [OperandRegClass<0, eRe>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d2_eRe_eUPSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d2_eRe_eS, [OperandRegClass<0, eRe>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d2_eRe_mLCm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d2_eRe_ePackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d2_eRe_eDN, [OperandRegClass<0, eRe>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d2_eRe_mSPm, [OperandRegClass<0, eRe>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d2_eRe_eM, [OperandRegClass<0, eRe>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d2_eRe_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_d2_eRo_eDJ, [OperandRegClass<0, eRo>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d2_eRo_mLRm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d2_eRo_eDC, [OperandRegClass<0, eRo>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d2_eRo_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d2_eRo_mLSm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d2_eRo_eVaddSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d2_eRo_eP, [OperandRegClass<0, eRo>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d2_eRo_eUnpackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d2_eRo_eSRSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d2_eRo_mCoreID, [OperandRegClass<0, eRo>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d2_eRo_eUPSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d2_eRo_eS, [OperandRegClass<0, eRo>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d2_eRo_mLCm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d2_eRo_ePackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d2_eRo_eDN, [OperandRegClass<0, eRo>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d2_eRo_mSPm, [OperandRegClass<0, eRo>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d2_eRo_eM, [OperandRegClass<0, eRo>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d2_eRo_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>]>] in
def MOV_d2 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mMvSclSrc:$src), "mov.d2", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay2
bits<5> dst;
bits<7> src;

let mv = {dst, 0b00, src, 0b1, 0b0111011};
}

let Itinerary = II_MOV_d3, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d3_eRe_mLEm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d3_eRo_mLEm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d3_eRe_eDJ, [OperandRegClass<0, eRe>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d3_eRe_mLRm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d3_eRe_eDC, [OperandRegClass<0, eRe>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d3_eRe_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d3_eRe_mLSm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d3_eRe_eVaddSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d3_eRe_eP, [OperandRegClass<0, eRe>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d3_eRe_eUnpackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d3_eRe_eSRSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d3_eRe_mCoreID, [OperandRegClass<0, eRe>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d3_eRe_eUPSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d3_eRe_eS, [OperandRegClass<0, eRe>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d3_eRe_mLCm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d3_eRe_ePackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d3_eRe_eDN, [OperandRegClass<0, eRe>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d3_eRe_mSPm, [OperandRegClass<0, eRe>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d3_eRe_eM, [OperandRegClass<0, eRe>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d3_eRe_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_d3_eRo_eDJ, [OperandRegClass<0, eRo>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d3_eRo_mLRm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d3_eRo_eDC, [OperandRegClass<0, eRo>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d3_eRo_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d3_eRo_mLSm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d3_eRo_eVaddSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d3_eRo_eP, [OperandRegClass<0, eRo>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d3_eRo_eUnpackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d3_eRo_eSRSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d3_eRo_mCoreID, [OperandRegClass<0, eRo>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d3_eRo_eUPSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d3_eRo_eS, [OperandRegClass<0, eRo>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d3_eRo_mLCm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d3_eRo_ePackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d3_eRo_eDN, [OperandRegClass<0, eRo>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d3_eRo_mSPm, [OperandRegClass<0, eRo>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d3_eRo_eM, [OperandRegClass<0, eRo>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d3_eRo_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>]>] in
def MOV_d3 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mMvSclSrc:$src), "mov.d3", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay3
bits<5> dst;
bits<7> src;

let mv = {dst, 0b01, src, 0b0, 0b0111011};
}

let Itinerary = II_MOV_d4, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d4_eRe_mLEm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d4_eRo_mLEm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d4_eRe_eDJ, [OperandRegClass<0, eRe>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d4_eRe_mLRm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d4_eRe_eDC, [OperandRegClass<0, eRe>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d4_eRe_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d4_eRe_mLSm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d4_eRe_eVaddSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d4_eRe_eP, [OperandRegClass<0, eRe>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d4_eRe_eUnpackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d4_eRe_eSRSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d4_eRe_mCoreID, [OperandRegClass<0, eRe>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d4_eRe_eUPSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d4_eRe_eS, [OperandRegClass<0, eRe>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d4_eRe_mLCm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d4_eRe_ePackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d4_eRe_eDN, [OperandRegClass<0, eRe>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d4_eRe_mSPm, [OperandRegClass<0, eRe>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d4_eRe_eM, [OperandRegClass<0, eRe>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d4_eRe_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_d4_eRo_eDJ, [OperandRegClass<0, eRo>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d4_eRo_mLRm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d4_eRo_eDC, [OperandRegClass<0, eRo>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d4_eRo_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d4_eRo_mLSm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d4_eRo_eVaddSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d4_eRo_eP, [OperandRegClass<0, eRo>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d4_eRo_eUnpackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d4_eRo_eSRSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d4_eRo_mCoreID, [OperandRegClass<0, eRo>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d4_eRo_eUPSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d4_eRo_eS, [OperandRegClass<0, eRo>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d4_eRo_mLCm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d4_eRo_ePackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d4_eRo_eDN, [OperandRegClass<0, eRo>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d4_eRo_mSPm, [OperandRegClass<0, eRo>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d4_eRo_eM, [OperandRegClass<0, eRo>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d4_eRo_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>]>] in
def MOV_d4 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mMvSclSrc:$src), "mov.d4", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay4
bits<5> dst;
bits<7> src;

let mv = {dst, 0b01, src, 0b1, 0b0111011};
}

let Itinerary = II_MOV_d5, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d5_eRe_mLEm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d5_eRo_mLEm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d5_eRe_eDJ, [OperandRegClass<0, eRe>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d5_eRe_mLRm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d5_eRe_eDC, [OperandRegClass<0, eRe>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d5_eRe_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d5_eRe_mLSm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d5_eRe_eVaddSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d5_eRe_eP, [OperandRegClass<0, eRe>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d5_eRe_eUnpackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d5_eRe_eSRSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d5_eRe_mCoreID, [OperandRegClass<0, eRe>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d5_eRe_eUPSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d5_eRe_eS, [OperandRegClass<0, eRe>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d5_eRe_mLCm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d5_eRe_ePackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d5_eRe_eDN, [OperandRegClass<0, eRe>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d5_eRe_mSPm, [OperandRegClass<0, eRe>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d5_eRe_eM, [OperandRegClass<0, eRe>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d5_eRe_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_d5_eRo_eDJ, [OperandRegClass<0, eRo>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d5_eRo_mLRm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d5_eRo_eDC, [OperandRegClass<0, eRo>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d5_eRo_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d5_eRo_mLSm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d5_eRo_eVaddSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d5_eRo_eP, [OperandRegClass<0, eRo>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d5_eRo_eUnpackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d5_eRo_eSRSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d5_eRo_mCoreID, [OperandRegClass<0, eRo>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d5_eRo_eUPSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d5_eRo_eS, [OperandRegClass<0, eRo>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d5_eRo_mLCm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d5_eRo_ePackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d5_eRo_eDN, [OperandRegClass<0, eRo>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d5_eRo_mSPm, [OperandRegClass<0, eRo>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d5_eRo_eM, [OperandRegClass<0, eRo>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d5_eRo_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>]>] in
def MOV_d5 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mMvSclSrc:$src), "mov.d5", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay5
bits<5> dst;
bits<7> src;

let mv = {dst, 0b10, src, 0b0, 0b0111011};
}

let Itinerary = II_MOV_d6, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d6_eRe_mLEm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d6_eRo_mLEm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d6_eRe_eDJ, [OperandRegClass<0, eRe>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d6_eRe_mLRm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d6_eRe_eDC, [OperandRegClass<0, eRe>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d6_eRe_eRe, [OperandRegClass<0, eRe>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d6_eRe_mLSm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d6_eRe_eVaddSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d6_eRe_eP, [OperandRegClass<0, eRe>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d6_eRe_eUnpackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d6_eRe_eSRSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d6_eRe_mCoreID, [OperandRegClass<0, eRe>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d6_eRe_eUPSSign, [OperandRegClass<0, eRe>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d6_eRe_eS, [OperandRegClass<0, eRe>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d6_eRe_mLCm, [OperandRegClass<0, eRe>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d6_eRe_ePackSign, [OperandRegClass<0, eRe>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d6_eRe_eDN, [OperandRegClass<0, eRe>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d6_eRe_mSPm, [OperandRegClass<0, eRe>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d6_eRe_eM, [OperandRegClass<0, eRe>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d6_eRe_eRo, [OperandRegClass<0, eRe>, OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_MOV_d6_eRo_eDJ, [OperandRegClass<0, eRo>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d6_eRo_mLRm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d6_eRo_eDC, [OperandRegClass<0, eRo>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d6_eRo_eRe, [OperandRegClass<0, eRo>, OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_MOV_d6_eRo_mLSm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d6_eRo_eVaddSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d6_eRo_eP, [OperandRegClass<0, eRo>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d6_eRo_eUnpackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d6_eRo_eSRSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_d6_eRo_mCoreID, [OperandRegClass<0, eRo>, OperandRegClass<1, mCoreID>]>,
ItinRegClassPair<II_MOV_d6_eRo_eUPSSign, [OperandRegClass<0, eRo>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d6_eRo_eS, [OperandRegClass<0, eRo>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d6_eRo_mLCm, [OperandRegClass<0, eRo>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d6_eRo_ePackSign, [OperandRegClass<0, eRo>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d6_eRo_eDN, [OperandRegClass<0, eRo>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d6_eRo_mSPm, [OperandRegClass<0, eRo>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d6_eRo_eM, [OperandRegClass<0, eRo>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d6_eRo_eRo, [OperandRegClass<0, eRo>, OperandRegClass<1, eRo>]>] in
def MOV_d6 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mMvSclSrc:$src), "mov.d6", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay6
bits<5> dst;
bits<7> src;

let mv = {dst, 0b10, src, 0b1, 0b0111011};
}

let Itinerary = II_MOV_nb_lda, hasSideEffects = true, Defs = [srSS0] in
def MOV_nb_lda : AIE2PS_inst_lda_instr32 <(outs OP_mRa:$dst), (ins), "mov.nb", " $dst, ss">{
// id: me__instr128_or__lda__mv_ss2scl__mSStream__mSStream_nb
bits<5> dst;

let lda = {0b1, dontcare{8-1}, dst, 0b1111100};
}

let Itinerary = II_MOV_nb_st_mMStream_tlast_imm, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eRo, [OperandRegClass<0, eRo>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_nb_st_mMStream_tlast_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mSclMS:$src), "mov.nb", " ms, $src">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__noTlast
bits<7> src;

let st = {0b1, 0b0, 0b0, 0b101, dontcare{3-1}, src, 0b10110};
}

let Itinerary = II_MOV_nb_st_mMStream_tlast_reg, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eRo, [OperandRegClass<0, eRo>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_nb_st_mMStream_tlast_reg : AIE2PS_inst_st_instr32 <(outs), (ins OP_mSclMS:$src, mR28_tlast:$tlast), "mov.nb", " ms, $src, r28">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
bits<7> src;

let st = {0b1, 0b1, dontcare{1}, 0b101, dontcare{3-1}, src, 0b10110};
}

let Itinerary = II_MOV_nb_tlast, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_nb_tlast_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_nb_tlast_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_nb_tlast_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_MOV_nb_tlast_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_nb_tlast_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_nb_tlast_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOV_nb_tlast_eRo, [OperandRegClass<0, eRo>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_nb_tlast : AIE2PS_inst_st_instr32 <(outs), (ins OP_mSclMS:$src), "mov.nb.tlast", " ms, $src">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__doTlast
bits<7> src;

let st = {0b1, 0b0, 0b1, 0b101, dontcare{3-1}, src, 0b10110};
}

let Itinerary = II_MOV_tlast, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_tlast_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_tlast_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_tlast_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_MOV_tlast_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_tlast_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_tlast_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOV_tlast_eRo, [OperandRegClass<0, eRo>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_tlast : AIE2PS_inst_st_instr32 <(outs), (ins OP_mSclMS:$src), "mov.tlast", " ms, $src">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__doTlast
bits<7> src;

let st = {0b0, 0b0, 0b1, 0b101, dontcare{3-1}, src, 0b10110};
}

let Itinerary = II_MSC, Constraints = "$a0 = $d0" in
def MSC : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRz:$a0, OP_mRx:$s0, OP_mRy:$s1), "msc", "$d0, $a0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mac_r_rrr
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1, 0b1100};
}

let Itinerary = II_MUL in
def MUL : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "mul", "$d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mul_r_rr
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1, 0b1111};
}

let Itinerary = II_NEZ in
def NEZ : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "nez", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__sne0
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0111, 0b10, 0b0000};
}

let Itinerary = II_NE in
def NE : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "ne", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sne
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1000, 0b1};
}

let Itinerary = II_NOPA, isSlotNOP = true in
def NOPA : AIE2PS_inst_lda_instr32 <(outs), (ins), "nopa", "">{
// id: me__instr128_or__lda__nop_lda

let lda = {dontcare{12-1}, 0b110001101};
}

let Itinerary = II_NOPB, isSlotNOP = true in
def NOPB : AIE2PS_inst_ldb_instr32 <(outs), (ins), "nopb", "">{
// id: me__instr128_or__ldb__nop_ldb

let ldb = {dontcare{12-1}, 0b10110};
}

let Itinerary = II_NOPM, isSlotNOP = true in
def NOPM : AIE2PS_inst_mv_instr32 <(outs), (ins), "nopm", "">{
// id: me__instr128_or__alu_mv__alumv_or__mv__nop_mv

let mv = {dontcare{7-1}, 0b101100001101111};
}

let Itinerary = II_NOPS, isSlotNOP = true in
def NOPS : AIE2PS_inst_st_instr32 <(outs), (ins), "nops", "">{
// id: me__instr128_or__st__nop_sts

let st = {dontcare{12-1}, 0b101101010};
}

let Itinerary = II_NOPV, isSlotNOP = true in
def NOPV : AIE2PS_inst_vec_instr32 <(outs), (ins), "nopv", "">{
// id: me__instr128_or__vec__nop_vec

let vec = {dontcare{3-1}, dontcare{3-1}, dontcare{3-1}, dontcare{3-1}, 0b11111, dontcare{1}, dontcare{3-1}, 0b101, 0b0};
}

let Itinerary = II_NOPXM, isSlotNOP = true in
def NOPXM : AIE2PS_inst_lng_instr48 <(outs), (ins), "nopxm", "">{
// id: me__instr128_or__alu_mv__lng__nop_lng

let lng = {dontcare{5-1}, dontcare{5-1}, dontcare{4-1}, dontcare{4-1}, dontcare{4-1}, dontcare{4-1}, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b000};
}

let Itinerary = II_NOPX, isSlotNOP = true in
def NOPX : AIE2PS_inst_alu_instr32 <(outs), (ins), "nopx", "">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nop_alu

let alu = {dontcare{5-1}, 0b000, dontcare{4-1}, 0b0, 0b000, 0b0000};
}

let Itinerary = II_OR in
def OR : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "or", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__or
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0101, 0b1};
}

let Itinerary = II_PADDA_2D, DecoderMethod = "DecodePADD_2D", Constraints = "$ptr_out = $ptr" in
def PADDA_2D : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "padda.2d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;

let lda = {ptr, mod, 0b100, dontcare{3-1}, 0b010001101};
}

let Itinerary = II_PADDA_3D, DecoderMethod = "DecodePADD_3D", Constraints = "$ptr_out = $ptr" in
def PADDA_3D : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "padda.3d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;

let lda = {ptr, dontcare{1}, mod, 0b110, dontcare{3-1}, 0b010001101};
}

let Itinerary = II_PADDA_pstm_nrm, Constraints = "$ptr_out = $ptr" in
def PADDA_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, eM:$mod), "padda", " [$ptr], $mod">{
// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;

let lda = {ptr, mod, 0b010, dontcare{3-1}, 0b010001101};
}

let Itinerary = II_PADDA_pstm_nrm_imm, Constraints = "$ptr_out = $ptr" in
def PADDA_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "padda", " [$ptr], #$imm">{
// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_nrm_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;

let lda = {ptr, imm, 0b11, dontcare{3-1}, 0b010001101};
}

let Itinerary = II_PADDB_2D, DecoderMethod = "DecodePADDB_2D", Constraints = "$ptr_out = $ptr" in
def PADDB_2D : AIE2PS_inst_ldb_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "paddb.2d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_2d__mPb__mDb
bits<3> ptr;
bits<3> mod;

let ldb = {ptr, mod, 0b100, dontcare{3-1}, 0b00110};
}

let Itinerary = II_PADDB_3D, DecoderMethod = "DecodePADDB_3D", Constraints = "$ptr_out = $ptr" in
def PADDB_3D : AIE2PS_inst_ldb_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "paddb.3d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_3d__mPb__mDSb
bits<3> ptr;
bits<2> mod;

let ldb = {ptr, dontcare{1}, mod, 0b110, dontcare{3-1}, 0b00110};
}

let Itinerary = II_PADDB_pstm_nrm, Constraints = "$ptr_out = $ptr" in
def PADDB_pstm_nrm : AIE2PS_inst_ldb_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, eM:$mod), "paddb", " [$ptr], $mod">{
// id: me__instr128_or__ldb__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_nrm__mPb__mMb
bits<3> ptr;
bits<3> mod;

let ldb = {ptr, mod, 0b010, dontcare{3-1}, 0b00110};
}

let Itinerary = II_PADDB_pstm_nrm_imm, Constraints = "$ptr_out = $ptr" in
def PADDB_pstm_nrm_imm : AIE2PS_inst_ldb_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "paddb", " [$ptr], #$imm">{
// id: me__instr128_or__ldb__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_nrm_imm__mPb__m_c10s_step64
bits<3> ptr;
bits<4> imm;

let ldb = {ptr, imm, 0b11, dontcare{3-1}, 0b00110};
}

let Itinerary = II_PADDS_2D, DecoderMethod = "DecodePADD_2D", Constraints = "$ptr_out = $ptr" in
def PADDS_2D : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "padds.2d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;

let st = {ptr, mod, 0b100, dontcare{3-1}, 0b001101010};
}

let Itinerary = II_PADDS_3D, DecoderMethod = "DecodePADD_3D", Constraints = "$ptr_out = $ptr" in
def PADDS_3D : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "padds.3d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;

let st = {ptr, dontcare{1}, mod, 0b110, dontcare{3-1}, 0b001101010};
}

let Itinerary = II_PADDS_pstm_nrm, Constraints = "$ptr_out = $ptr" in
def PADDS_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, eM:$mod), "padds", " [$ptr], $mod">{
// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;

let st = {ptr, mod, 0b010, dontcare{3-1}, 0b001101010};
}

let Itinerary = II_PADDS_pstm_nrm_imm, Constraints = "$ptr_out = $ptr" in
def PADDS_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "padds", " [$ptr], #$imm">{
// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_nrm_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;

let st = {ptr, imm, 0b11, dontcare{3-1}, 0b001101010};
}

let Itinerary = II_PADDXM_pstm_sp, Defs = [sp], Uses = [sp] in
def PADDXM_pstm_sp : AIE2PS_inst_lda_instr32 <(outs), (ins eM:$mod), "paddxm", " [sp], $mod">{
// id: me__instr128_or__lda__lda_ptr_inc_long__agua_sa_lng__standalone_long__agua_sa_lng__standalone_lng__agua_sa_lng__pstm_sp__mSPa__mMa
bits<3> mod;

let lda = {dontcare{3-1}, mod, dontcare{7-1}, 0b1, 0b0111100};
}

let Itinerary = II_PADDXM_pstm_sp_imm, Defs = [sp], Uses = [sp] in
def PADDXM_pstm_sp_imm : AIE2PS_inst_lda_instr32 <(outs), (ins c19s_step64:$imm), "paddxm", " [sp], #$imm">{
// id: me__instr128_or__lda__lda_ptr_inc_long__agua_sa_lng__standalone_long__agua_sa_lng__standalone_lng__agua_sa_lng__pstm_sp_imm__mSPa__m_c19s_step64
bits<13> imm;

let lda = {imm, 0b0, 0b0111100};
}

let Itinerary = II_POPCOUNT in
def POPCOUNT : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0), "popcount", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__popcnt
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b1001, 0b10, 0b0000};
}

let Itinerary = II_REL_COND_mLockId_imm, hasSideEffects = true in
def REL_COND_mLockId_imm : AIE2PS_inst_alu_instr32 <(outs), (ins c6u:$id, OP_mRy:$s1, mR23_lock:$r23), "rel.cond", "#$id, $s1, r23">{
// id: me__instr128_or__alu_mv__alumv_or__alu__release_cond__mLockId__mLockId_imm__mR23_lock
bits<6> id;
bits<5> s1;

let alu = {id, 0b0, dontcare{1}, 0b10, s1, 0b1, 0b0000};
}

let Itinerary = II_REL_COND_mLockId_reg, hasSideEffects = true in
def REL_COND_mLockId_reg : AIE2PS_inst_alu_instr32 <(outs), (ins OP_mRx:$s0, OP_mRy:$s1, mR23_lock:$r23), "rel.cond", "$s0, $s1, r23">{
// id: me__instr128_or__alu_mv__alumv_or__alu__release_cond__mLockId__mLockId_reg__mR23_lock
bits<5> s0;
bits<5> s1;

let alu = {s0, dontcare{1}, 0b1, dontcare{1}, 0b10, s1, 0b1, 0b0000};
}

let Itinerary = II_REL_mLockId_imm, hasSideEffects = true in
def REL_mLockId_imm : AIE2PS_inst_alu_instr32 <(outs), (ins c6u:$id, OP_mRy:$s1), "rel", "#$id, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__release__mLockId__mLockId_imm
bits<6> id;
bits<5> s1;

let alu = {id, 0b0, dontcare{1}, 0b00, s1, 0b1, 0b0000};
}

let Itinerary = II_REL_mLockId_reg, hasSideEffects = true in
def REL_mLockId_reg : AIE2PS_inst_alu_instr32 <(outs), (ins OP_mRx:$s0, OP_mRy:$s1), "rel", "$s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__release__mLockId__mLockId_reg
bits<5> s0;
bits<5> s1;

let alu = {s0, dontcare{1}, 0b1, dontcare{1}, 0b00, s1, 0b1, 0b0000};
}

let Itinerary = II_RET, hasDelaySlot = true, Uses = [lr] in
def RET : AIE2PS_inst_alu_instr32 <(outs), (ins), "ret", "lr">{
// id: me__instr128_or__alu_mv__alumv_or__alu__jump_return__mLRx

let alu = {dontcare{2-1}, 0b110, 0b001, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_SBC, Defs = [srCarry], Uses = [srCarry] in
def SBC : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "sbc", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__subx
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0011, 0b1};
}

let Itinerary = II_SEL_EQZ in
def SEL_EQZ : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1, mR27_select:$r27), "sel.eqz", " $d0, $s0, $s1, r27">{
// id: me__instr128_or__alu_mv__alumv_or__alu__select_r_rr__IF_FALSE__mR27_select
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0, 0b0100};
}

let Itinerary = II_SEL_NEZ in
def SEL_NEZ : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1, mR27_select:$r27), "sel.nez", " $d0, $s0, $s1, r27">{
// id: me__instr128_or__alu_mv__alumv_or__alu__select_r_rr__IF_TRUE__mR27_select
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1, 0b0100};
}

let Itinerary = II_SQRT_nlf_sqrt, Defs = [srFPNlf], Uses = [crFPNlfMask] in
def SQRT_nlf_sqrt : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRx:$s0), "sqrt", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b00, 0b0, 0b0, 0b00, 0b1000};
}

let Itinerary = II_SQRT_nlf_sqrt_mOptConvDel_mS3, Defs = [srFPCnvFl2Fx, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPNlfMask] in
def SQRT_nlf_sqrt_mOptConvDel_mS3 : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins mS3:$mcnv_del, OP_mRx:$s0), "sqrt", " ${d0}.flt2fx, s3, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b00, 0b1, 0b0, 0b00, 0b1000};
}

let Itinerary = II_SQRT_nlf_sqrt_mOptConvDel_mS3_mOptConv, Defs = [srFPCnvFl2Fx, srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPCnvFx2FlMask, crFPNlfMask] in
def SQRT_nlf_sqrt_mOptConvDel_mS3_mOptConv : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins mS3:$mcnv_del, OP_mRx:$s0, mS2:$mcnv), "sqrt", " ${d0}.flt2fx, s3, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b00, 0b1, 0b1, 0b00, 0b1000};
}

let Itinerary = II_SQRT_nlf_sqrt_mOptConv, Defs = [srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFx2FlMask, crFPNlfMask] in
def SQRT_nlf_sqrt_mOptConv : AIE2PS_inst_alu_instr32 <(outs OP_mRa:$d0), (ins OP_mRx:$s0, mS2:$mcnv), "sqrt", " $d0, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b00, 0b0, 0b1, 0b00, 0b1000};
}

let Itinerary = II_ST_2D_dml_sts_scalar, ItineraryRegPairs = [
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eFlo, [OperandRegClass<2, eFlo>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eEGhe_s, [OperandRegClass<2, eEGhe_s>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eL, [OperandRegClass<2, eL>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eEEo, [OperandRegClass<2, eEEo>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eFle, [OperandRegClass<2, eFle>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eGGo, [OperandRegClass<2, eGGo>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eFhe, [OperandRegClass<2, eFhe>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eEGho_s, [OperandRegClass<2, eEGho_s>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eFho, [OperandRegClass<2, eFho>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eEGlo_s, [OperandRegClass<2, eEGlo_s>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eEGle_s, [OperandRegClass<2, eEGle_s>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eGGe, [OperandRegClass<2, eGGe>]>,
ItinRegClassPair<II_ST_2D_dml_sts_scalar_eEEe, [OperandRegClass<2, eEEe>]>], DecoderMethod = "DecodeST_2D_dml_sts_scalar", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_2D_dml_sts_scalar : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mlSts:$src, eP:$ptr, eD:$mod), "st.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<7> src;

let st = {ptr, mod, 0b100, src, 0b10011};
}

let Itinerary = II_ST_2D_dms_sts_g, DecoderMethod = "DecodeST_2D_dms_sts_g", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_2D_dms_sts_g : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mGs:$src, eP:$ptr, eD:$mod), "st.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b1110111};
}

let Itinerary = II_ST_2D_dms_sts_scalar, ItineraryRegPairs = [
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eEle, [OperandRegClass<2, eEle>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eElo, [OperandRegClass<2, eElo>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eDJ, [OperandRegClass<2, eDJ>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_mLRs, [OperandRegClass<2, mLRs>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eDC, [OperandRegClass<2, eDC>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eRe, [OperandRegClass<2, eRe>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eDN, [OperandRegClass<2, eDN>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eEhe, [OperandRegClass<2, eEhe>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eP, [OperandRegClass<2, eP>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eM, [OperandRegClass<2, eM>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eRo, [OperandRegClass<2, eRo>]>,
ItinRegClassPair<II_ST_2D_dms_sts_scalar_eEho, [OperandRegClass<2, eEho>]>], DecoderMethod = "DecodeST_2D_dms_sts_scalar", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_2D_dms_sts_scalar : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mSclSt:$src, eP:$ptr, eD:$mod), "st.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<7> src;

let st = {ptr, mod, 0b100, src, 0b10100};
}

let Itinerary = II_ST_2D_dmv_sts_f, DecoderMethod = "DecodeST_2D_dmv_sts_f", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_2D_dmv_sts_f : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mFFs:$src, eP:$ptr, eD:$mod), "st.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b11011001};
}

let Itinerary = II_ST_2D_s16, DecoderMethod = "DecodeST_2D", mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_2D_s16 : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mRs:$src, eP:$ptr, eD:$mod), "st.2d.s16", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> src;

let lda = {ptr, mod, 0b100, src, 0b1, 0b011011};
}

let Itinerary = II_ST_2D_s8, DecoderMethod = "DecodeST_2D", mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_2D_s8 : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mRs:$src, eP:$ptr, eD:$mod), "st.2d.s8", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> src;

let lda = {ptr, mod, 0b100, src, 0b0, 0b011011};
}

let Itinerary = II_ST_3D_dml_sts_scalar, ItineraryRegPairs = [
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eFlo, [OperandRegClass<3, eFlo>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eEGhe_s, [OperandRegClass<3, eEGhe_s>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eL, [OperandRegClass<3, eL>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eEEo, [OperandRegClass<3, eEEo>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eFle, [OperandRegClass<3, eFle>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eGGo, [OperandRegClass<3, eGGo>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eFhe, [OperandRegClass<3, eFhe>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eEGho_s, [OperandRegClass<3, eEGho_s>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eFho, [OperandRegClass<3, eFho>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eEGlo_s, [OperandRegClass<3, eEGlo_s>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eEGle_s, [OperandRegClass<3, eEGle_s>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eGGe, [OperandRegClass<3, eGGe>]>,
ItinRegClassPair<II_ST_3D_dml_sts_scalar_eEEe, [OperandRegClass<3, eEEe>]>], DecoderMethod = "DecodeST_3D_dml_sts_scalar", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_3D_dml_sts_scalar : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mlSts:$src, eP:$ptr, eDS:$mod), "st.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<7> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b10011};
}

let Itinerary = II_ST_3D_dms_sts_g, DecoderMethod = "DecodeST_3D_dms_sts_g", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_3D_dms_sts_g : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mGs:$src, eP:$ptr, eDS:$mod), "st.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1110111};
}

let Itinerary = II_ST_3D_dms_sts_scalar, ItineraryRegPairs = [
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eEle, [OperandRegClass<3, eEle>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eElo, [OperandRegClass<3, eElo>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eDJ, [OperandRegClass<3, eDJ>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_mLRs, [OperandRegClass<3, mLRs>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eDC, [OperandRegClass<3, eDC>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eRe, [OperandRegClass<3, eRe>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eDN, [OperandRegClass<3, eDN>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eEhe, [OperandRegClass<3, eEhe>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eP, [OperandRegClass<3, eP>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eM, [OperandRegClass<3, eM>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eRo, [OperandRegClass<3, eRo>]>,
ItinRegClassPair<II_ST_3D_dms_sts_scalar_eEho, [OperandRegClass<3, eEho>]>], DecoderMethod = "DecodeST_3D_dms_sts_scalar", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_3D_dms_sts_scalar : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mSclSt:$src, eP:$ptr, eDS:$mod), "st.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<7> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b10100};
}

let Itinerary = II_ST_3D_dmv_sts_f, DecoderMethod = "DecodeST_3D_dmv_sts_f", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_3D_dmv_sts_f : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mFFs:$src, eP:$ptr, eDS:$mod), "st.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b11011001};
}

let Itinerary = II_ST_3D_s16, DecoderMethod = "DecodeST_3D", mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_3D_s16 : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mRs:$src, eP:$ptr, eDS:$mod), "st.3d.s16", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> src;

let lda = {ptr, dontcare{1}, mod, 0b110, src, 0b1, 0b011011};
}

let Itinerary = II_ST_3D_s8, DecoderMethod = "DecodeST_3D", mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_3D_s8 : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mRs:$src, eP:$ptr, eDS:$mod), "st.3d.s8", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> src;

let lda = {ptr, dontcare{1}, mod, 0b110, src, 0b0, 0b011011};
}

let Itinerary = II_ST_TM_2D, DecoderMethod = "DecodeST_TM_2D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_TM_2D : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mRs:$src, eP:$ptr, eD:$mod), "st.tm.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b1101001};
}

let Itinerary = II_ST_TM_3D, DecoderMethod = "DecodeST_TM_3D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_TM_3D : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mRs:$src, eP:$ptr, eDS:$mod), "st.tm.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1101001};
}

let Itinerary = II_ST_TM_idx, mayStore = true in
def ST_TM_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mRs:$src, eP:$ptr, eDJ:$dj), "st.tm", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b1101001};
}

let Itinerary = II_ST_TM_idx_imm, mayStore = true in
def ST_TM_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mRs:$src, eP:$ptr, c6s_step4:$imm), "st.tm", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__idx_imm__mPs__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b1101001};
}

let Itinerary = II_ST_TM_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_TM_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mRs:$src, eP:$ptr, eM:$mod), "st.tm", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b1101001};
}

let Itinerary = II_ST_TM_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_TM_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mRs:$src, eP:$ptr, c6s_step4:$imm), "st.tm", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm_imm__mPs__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b1101001};
}

let Itinerary = II_ST_dml_sts_scalar_spill, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eFlo, [OperandRegClass<0, eFlo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eEGhe_s, [OperandRegClass<0, eEGhe_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eL, [OperandRegClass<0, eL>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eEEo, [OperandRegClass<0, eEEo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eFle, [OperandRegClass<0, eFle>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eGGo, [OperandRegClass<0, eGGo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eFhe, [OperandRegClass<0, eFhe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eEGho_s, [OperandRegClass<0, eEGho_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eFho, [OperandRegClass<0, eFho>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eEGlo_s, [OperandRegClass<0, eEGlo_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eEGle_s, [OperandRegClass<0, eEGle_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eGGe, [OperandRegClass<0, eGGe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_spill_eEEe, [OperandRegClass<0, eEEe>]>], mayStore = true, Uses = [sp] in
def ST_dml_sts_scalar_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mlSts:$src, c13n_step8:$imm), "st", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dml_sts_scalar__spill__agus_dml__nrm_spill__agus_dml__spill__mSPs__m_c13n_step8
bits<9> imm;
bits<7> src;

let st = {imm, src, 0b00011};
}

let Itinerary = II_ST_dml_sts_scalar_st_idx, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eFlo, [OperandRegClass<0, eFlo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eEGhe_s, [OperandRegClass<0, eEGhe_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eL, [OperandRegClass<0, eL>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eEEo, [OperandRegClass<0, eEEo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eFle, [OperandRegClass<0, eFle>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eGGo, [OperandRegClass<0, eGGo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eFhe, [OperandRegClass<0, eFhe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eEGho_s, [OperandRegClass<0, eEGho_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eFho, [OperandRegClass<0, eFho>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eEGlo_s, [OperandRegClass<0, eEGlo_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eEGle_s, [OperandRegClass<0, eEGle_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eGGe, [OperandRegClass<0, eGGe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_eEEe, [OperandRegClass<0, eEEe>]>], mayStore = true in
def ST_dml_sts_scalar_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mlSts:$src, eP:$ptr, eDJ:$dj), "st", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<7> src;

let st = {ptr, dj, 0b000, src, 0b10011};
}

let Itinerary = II_ST_dml_sts_scalar_st_idx_imm, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eFlo, [OperandRegClass<0, eFlo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eEGhe_s, [OperandRegClass<0, eEGhe_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eL, [OperandRegClass<0, eL>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eEEo, [OperandRegClass<0, eEEo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eFle, [OperandRegClass<0, eFle>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eGGo, [OperandRegClass<0, eGGo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eFhe, [OperandRegClass<0, eFhe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eEGho_s, [OperandRegClass<0, eEGho_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eFho, [OperandRegClass<0, eFho>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eEGlo_s, [OperandRegClass<0, eEGlo_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eEGle_s, [OperandRegClass<0, eEGle_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eGGe, [OperandRegClass<0, eGGe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_idx_imm_eEEe, [OperandRegClass<0, eEEe>]>], mayStore = true in
def ST_dml_sts_scalar_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mlSts:$src, eP:$ptr, c7s_step8:$imm), "st", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__idx_imm__mPs__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<7> src;

let st = {ptr, imm, 0b01, src, 0b10011};
}

let Itinerary = II_ST_dml_sts_scalar_st_pstm_nrm, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eFlo, [OperandRegClass<1, eFlo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eEGhe_s, [OperandRegClass<1, eEGhe_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eL, [OperandRegClass<1, eL>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eEEo, [OperandRegClass<1, eEEo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eFle, [OperandRegClass<1, eFle>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eGGo, [OperandRegClass<1, eGGo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eFhe, [OperandRegClass<1, eFhe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eEGho_s, [OperandRegClass<1, eEGho_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eFho, [OperandRegClass<1, eFho>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eEGlo_s, [OperandRegClass<1, eEGlo_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eEGle_s, [OperandRegClass<1, eEGle_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eGGe, [OperandRegClass<1, eGGe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_eEEe, [OperandRegClass<1, eEEe>]>], mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dml_sts_scalar_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mlSts:$src, eP:$ptr, eM:$mod), "st", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<7> src;

let st = {ptr, mod, 0b010, src, 0b10011};
}

let Itinerary = II_ST_dml_sts_scalar_st_pstm_nrm_imm, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFlo, [OperandRegClass<1, eFlo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGhe_s, [OperandRegClass<1, eEGhe_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eL, [OperandRegClass<1, eL>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEEo, [OperandRegClass<1, eEEo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFle, [OperandRegClass<1, eFle>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eGGo, [OperandRegClass<1, eGGo>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFhe, [OperandRegClass<1, eFhe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGho_s, [OperandRegClass<1, eEGho_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eFho, [OperandRegClass<1, eFho>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGlo_s, [OperandRegClass<1, eEGlo_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEGle_s, [OperandRegClass<1, eEGle_s>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eGGe, [OperandRegClass<1, eGGe>]>,
ItinRegClassPair<II_ST_dml_sts_scalar_st_pstm_nrm_imm_eEEe, [OperandRegClass<1, eEEe>]>], mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dml_sts_scalar_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mlSts:$src, eP:$ptr, c7s_step8:$imm), "st", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dml_sts_scalar__st__agus_dml__nrm__agus_dml__normal__agus_dml__pstm_nrm_imm__mPs__m_c07s_step8
bits<3> ptr;
bits<4> imm;
bits<7> src;

let st = {ptr, imm, 0b11, src, 0b10011};
}

let Itinerary = II_ST_dms_sts_g_spill, mayStore = true, Uses = [sp] in
def ST_dms_sts_g_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mGs:$src, c12n_step4:$imm), "st", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dms_sts_g__spill__agus_dms__nrm_spill__agus_dms__spill__mSPs__m_c12n_step4
bits<9> imm;
bits<5> src;

let st = {imm, src, 0b0110111};
}

let Itinerary = II_ST_dms_sts_g_st_idx, mayStore = true in
def ST_dms_sts_g_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mGs:$src, eP:$ptr, eDJ:$dj), "st", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b1110111};
}

let Itinerary = II_ST_dms_sts_g_st_idx_imm, mayStore = true in
def ST_dms_sts_g_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mGs:$src, eP:$ptr, c6s_step4:$imm), "st", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__idx_imm__mPs__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b1110111};
}

let Itinerary = II_ST_dms_sts_g_st_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dms_sts_g_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mGs:$src, eP:$ptr, eM:$mod), "st", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b1110111};
}

let Itinerary = II_ST_dms_sts_g_st_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dms_sts_g_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mGs:$src, eP:$ptr, c6s_step4:$imm), "st", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dms_sts_g__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm_imm__mPs__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b1110111};
}

let Itinerary = II_ST_dms_sts_scalar_spill, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_mLRs, [OperandRegClass<0, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_spill_eEho, [OperandRegClass<0, eEho>]>], mayStore = true, Uses = [sp] in
def ST_dms_sts_scalar_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mSclSt:$src, c12n_step4:$imm), "st", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dms_sts_scalar__spill__agus_dms__nrm_spill__agus_dms__spill__mSPs__m_c12n_step4
bits<9> imm;
bits<7> src;

let st = {imm, src, 0b00100};
}

let Itinerary = II_ST_dms_sts_scalar_st_idx, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_mLRs, [OperandRegClass<0, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_eEho, [OperandRegClass<0, eEho>]>], mayStore = true in
def ST_dms_sts_scalar_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mSclSt:$src, eP:$ptr, eDJ:$dj), "st", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<7> src;

let st = {ptr, dj, 0b000, src, 0b10100};
}

let Itinerary = II_ST_dms_sts_scalar_st_idx_imm, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_mLRs, [OperandRegClass<0, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eRe, [OperandRegClass<0, eRe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eRo, [OperandRegClass<0, eRo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_idx_imm_eEho, [OperandRegClass<0, eEho>]>], mayStore = true in
def ST_dms_sts_scalar_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mSclSt:$src, eP:$ptr, c6s_step4:$imm), "st", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__idx_imm__mPs__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<7> src;

let st = {ptr, imm, 0b01, src, 0b10100};
}

let Itinerary = II_ST_dms_sts_scalar_st_pstm_nrm, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eEle, [OperandRegClass<1, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eElo, [OperandRegClass<1, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_mLRs, [OperandRegClass<1, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eRe, [OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eEhe, [OperandRegClass<1, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eRo, [OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_eEho, [OperandRegClass<1, eEho>]>], mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dms_sts_scalar_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mSclSt:$src, eP:$ptr, eM:$mod), "st", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<7> src;

let st = {ptr, mod, 0b010, src, 0b10100};
}

let Itinerary = II_ST_dms_sts_scalar_st_pstm_nrm_imm, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eEle, [OperandRegClass<1, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eElo, [OperandRegClass<1, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_mLRs, [OperandRegClass<1, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eRe, [OperandRegClass<1, eRe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eEhe, [OperandRegClass<1, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eRo, [OperandRegClass<1, eRo>]>,
ItinRegClassPair<II_ST_dms_sts_scalar_st_pstm_nrm_imm_eEho, [OperandRegClass<1, eEho>]>], mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dms_sts_scalar_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mSclSt:$src, eP:$ptr, c6s_step4:$imm), "st", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dms_sts_scalar__st__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm_imm__mPs__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<7> src;

let st = {ptr, imm, 0b11, src, 0b10100};
}

let Itinerary = II_ST_dmv_sts_f_spill, mayStore = true, Uses = [sp] in
def ST_dmv_sts_f_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mFFs:$src, c14n_step16:$imm), "st", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmv_sts_f__spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
bits<9> imm;
bits<4> src;

let st = {imm, src, 0b01011001};
}

let Itinerary = II_ST_dmv_sts_f_st_idx, mayStore = true in
def ST_dmv_sts_f_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mFFs:$src, eP:$ptr, eDJ:$dj), "st", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b11011001};
}

let Itinerary = II_ST_dmv_sts_f_st_idx_imm, mayStore = true in
def ST_dmv_sts_f_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mFFs:$src, eP:$ptr, c8s_step16:$imm), "st", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b11011001};
}

let Itinerary = II_ST_dmv_sts_f_st_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dmv_sts_f_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mFFs:$src, eP:$ptr, eM:$mod), "st", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b11011001};
}

let Itinerary = II_ST_dmv_sts_f_st_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dmv_sts_f_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mFFs:$src, eP:$ptr, c8s_step16:$imm), "st", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmv_sts_f__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b11011001};
}

let Itinerary = II_ST_s16_idx, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads] in
def ST_s16_idx : AIE2PS_inst_lda_instr32 <(outs), (ins OP_mRs:$src, eP:$ptr, eDJ:$dj), "st.s16", "$src, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> src;

let lda = {ptr, dj, 0b000, src, 0b1, 0b011011};
}

let Itinerary = II_ST_s16_idx_imm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads] in
def ST_s16_idx_imm : AIE2PS_inst_lda_instr32 <(outs), (ins OP_mRs:$src, eP:$ptr, c5s_step2:$imm), "st.s16", "$src, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__idx_imm__mPa__m_c05s_step2
bits<3> ptr;
bits<4> imm;
bits<5> src;

let lda = {ptr, imm, 0b01, src, 0b1, 0b011011};
}

let Itinerary = II_ST_s16_pstm_nrm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_s16_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out), (ins OP_mRs:$src, eP:$ptr, eM:$mod), "st.s16", "$src, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> src;

let lda = {ptr, mod, 0b010, src, 0b1, 0b011011};
}

let Itinerary = II_ST_s16_pstm_nrm_imm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_s16_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out), (ins OP_mRs:$src, eP:$ptr, c5s_step2:$imm), "st.s16", "$src, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_nrm_imm__mPa__m_c05s_step2
bits<3> ptr;
bits<4> imm;
bits<5> src;

let lda = {ptr, imm, 0b11, src, 0b1, 0b011011};
}

let Itinerary = II_ST_s8_idx, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads] in
def ST_s8_idx : AIE2PS_inst_lda_instr32 <(outs), (ins OP_mRs:$src, eP:$ptr, eDJ:$dj), "st.s8", "$src, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> src;

let lda = {ptr, dj, 0b000, src, 0b0, 0b011011};
}

let Itinerary = II_ST_s8_idx_imm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads] in
def ST_s8_idx_imm : AIE2PS_inst_lda_instr32 <(outs), (ins OP_mRs:$src, eP:$ptr, c4s:$imm), "st.s8", "$src, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__idx_imm__mPa__m_c04s
bits<3> ptr;
bits<4> imm;
bits<5> src;

let lda = {ptr, imm, 0b01, src, 0b0, 0b011011};
}

let Itinerary = II_ST_s8_pstm_nrm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_s8_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out), (ins OP_mRs:$src, eP:$ptr, eM:$mod), "st.s8", "$src, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> src;

let lda = {ptr, mod, 0b010, src, 0b0, 0b011011};
}

let Itinerary = II_ST_s8_pstm_nrm_imm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_s8_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs eP:$ptr_out), (ins OP_mRs:$src, eP:$ptr, c4s:$imm), "st.s8", "$src, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_nrm_imm__mPa__m_c04s
bits<3> ptr;
bits<4> imm;
bits<5> src;

let lda = {ptr, imm, 0b11, src, 0b0, 0b011011};
}

let Itinerary = II_SUB, Defs = [srCarry] in
def SUB : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "sub", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sub
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0001, 0b1};
}

let Itinerary = II_VABS_GTZ16_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VABS_GTZ16_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXn:$s2), "vabs_gtz16", " $d, r16, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b01, 0b0, 0b11100101111};
}

let Itinerary = II_VABS_GTZ16_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VABS_GTZ16_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXn:$s2), "vabs_gtz16", " $d, r16, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b01, 0b1, 0b11100101111};
}

let Itinerary = II_VABS_GTZ32_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VABS_GTZ32_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXn:$s2), "vabs_gtz32", " $d, r16, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b00, 0b0, 0b11100101111};
}

let Itinerary = II_VABS_GTZ32_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VABS_GTZ32_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXn:$s2), "vabs_gtz32", " $d, r16, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b00, 0b1, 0b11100101111};
}

let Itinerary = II_VABS_GTZ8_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VABS_GTZ8_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXn:$s2), "vabs_gtz8", " $d, r17:r16, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b10, 0b0, 0b11100101111};
}

let Itinerary = II_VABS_GTZ8_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VABS_GTZ8_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXn:$s2), "vabs_gtz8", " $d, r17:r16, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b10, 0b1, 0b11100101111};
}

let Itinerary = II_VADDMAC_vaddmac_vmul_cm_core_X_QX, Defs = [srSparse_of], Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0110, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_vmul_cm_core_X_QY, Defs = [srSparse_of], Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_vmul_cm_core_X_R, Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1000, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_vmul_cm_core_X_X, Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0000, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_vmul_cm_core_X_Y, Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_vmul_cm_core_Y_QX, Defs = [srSparse_of], Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_vmul_cm_core_Y_R, Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_vmul_cm_core_Y_X, Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_vmul_cm_core_Y_Y, Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_half_vmul_cm_half_core_X_QX, Defs = [srSparse_of], Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1010, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_half_vmul_cm_half_core_X_R, Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0111, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_half_vmul_cm_half_core_X_X, Constraints = "$acc1 = $dst" in
def VADDMAC_vaddmac_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0011, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0110, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_R, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1000, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0000, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_R, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0110, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_R, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1000, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_X, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0000, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_Y, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_R, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_X, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_Y, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1010, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_R, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0111, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0011, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1010, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_R, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0111, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_X, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMAC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0011, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmul_bf_core_X_QY, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_X_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bf_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmul_bf_core_X_X, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bf_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0100, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_R, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_Y_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_vmul_bf_core_Y_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bf_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1100, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bf_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_core_Y_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bf_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_half_vmul_bf_half_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bf_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1011, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY", hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0100, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY", hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0100, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1100, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY", hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1011, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1100, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY", hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1011, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp13, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bfp13", Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bfp13 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_reg__vmac_acc2_comp0__add_reg__mDMm__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp13_scd_add_scd_incr, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bfp13_scd_add_scd_incr", hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bfp13_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp13_scd_add_scd_noincr, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bfp13_scd_add_scd_noincr", hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bfp13_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_noincr__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp16, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bfp16", Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$acc1 = $dst" in
def VADDMAC_f_vaddmac_bfp16 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp16__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;
bits<3> acc2;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, acc2, 0b010, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp16_scd_add_scd_incr, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bfp16_scd_add_scd_incr", hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMAC_f_vaddmac_bfp16_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp16_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;
bits<3> acc1;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, acc1, 0b100, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp16_scd_add_scd_noincr, DecoderMethod = "DecodeVADDMAC_f_vaddmac_bfp16_scd_add_scd_noincr", hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VADDMAC_f_vaddmac_bfp16_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp16_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;
bits<3> acc1;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, acc1, 0b011, 0b0};
}

let Itinerary = II_VADDMSC_vaddmac_vmul_cm_core_X_QX, Defs = [srSparse_of], Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0110, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_vmul_cm_core_X_QY, Defs = [srSparse_of], Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_vmul_cm_core_X_R, Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1000, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_vmul_cm_core_X_X, Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0000, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_vmul_cm_core_X_Y, Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_vmul_cm_core_Y_QX, Defs = [srSparse_of], Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_vmul_cm_core_Y_R, Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_vmul_cm_core_Y_X, Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_vmul_cm_core_Y_Y, Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_half_vmul_cm_half_core_X_QX, Defs = [srSparse_of], Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1010, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_half_vmul_cm_half_core_X_R, Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0111, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_half_vmul_cm_half_core_X_X, Constraints = "$acc1 = $dst" in
def VADDMSC_vaddmac_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0011, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0110, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_R, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1000, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0000, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_R, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_add_scd_incr_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0110, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_R, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1000, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_X, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0000, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_Y, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_R, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_X, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_Y, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_add_scd_noincr_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1010, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_R, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0111, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_vaddmac_scd_half_add_scd_incr_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0011, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1010, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_R, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0111, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_X, hasSideEffects = true, Uses = [crSCDEn] in
def VADDMSC_vaddmac_scd_half_add_scd_noincr_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0011, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmul_bf_core_X_QY, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_X_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bf_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmul_bf_core_X_X, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bf_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0100, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_R, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_Y_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_vmul_bf_core_Y_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bf_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1100, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bf_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_core_Y_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bf_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_half_vmul_bf_half_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bf_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1011, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY", hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0100, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bf_scd_add_scd_incr_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY", hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0100, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bf_scd_add_scd_noincr_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, eY:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1100, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY", hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bf_scd_half_add_scd_incr_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1011, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1100, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY", hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X", hasSideEffects = true, Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bf_scd_half_add_scd_noincr_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bf_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1011, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp13, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bfp13", Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bfp13 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_reg__vmac_acc2_comp0__add_reg__mDMm__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc2;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp13_scd_add_scd_incr, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bfp13_scd_add_scd_incr", hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bfp13_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp13_scd_add_scd_noincr, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bfp13_scd_add_scd_noincr", hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bfp13_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_noincr__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp16, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bfp16", Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$acc1 = $dst" in
def VADDMSC_f_vaddmac_bfp16 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp16__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;
bits<3> acc2;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, acc2, 0b010, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp16_scd_add_scd_incr, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bfp16_scd_add_scd_incr", hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADDMSC_f_vaddmac_bfp16_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp16_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;
bits<3> acc1;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, acc1, 0b100, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp16_scd_add_scd_noincr, DecoderMethod = "DecodeVADDMSC_f_vaddmac_bfp16_scd_add_scd_noincr", hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VADDMSC_f_vaddmac_bfp16_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vaddmac_bfp16_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;
bits<3> acc1;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, acc1, 0b011, 0b1};
}

let Itinerary = II_VADDSUB_16, Uses = [crBF8conf, crFP8conf] in
def VADDSUB_16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, OP_mRS16m:$sel), "vaddsub.16", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in32__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b01, 0b1, 0b001};
}

let Itinerary = II_VADDSUB_32, Uses = [crBF8conf, crFP8conf] in
def VADDSUB_32 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, OP_mRS16m:$sel), "vaddsub.32", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in16__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b00, 0b1, 0b001};
}

let Itinerary = II_VADDSUB_8, Uses = [crBF8conf, crFP8conf] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VADDSUB_8_mLBypass, [OperandRegClass<3, eL>]>] in
// END MANUAL ADDITION
def VADDSUB_8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, eL:$sel), "vaddsub.8", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in64__mLm__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b10, 0b1, 0b001};
}

let Itinerary = II_VADD_16, Uses = [crBF8conf, crFP8conf] in
def VADD_16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vadd.16", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vadd__vec_size__vec_size32__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b0, 0b01, 0b1111011};
}

let Itinerary = II_VADD_32, Uses = [crBF8conf, crFP8conf] in
def VADD_32 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vadd.32", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vadd__vec_size__vec_size16__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b0, 0b10, 0b1111011};
}

let Itinerary = II_VADD_8, Uses = [crBF8conf, crFP8conf] in
def VADD_8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vadd.8", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vadd__vec_size__vec_size64__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b0, 0b00, 0b1111011};
}

let Itinerary = II_VADD_vacc in
def VADD_vacc : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mRv8:$acc), "vadd", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b000, 0b000, acc2, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_vacc_half in
def VADD_vacc_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mRv8:$acc), "vadd", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b001, 0b000, acc2, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_vacc_scd_add_scd_incr, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADD_vacc_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b000, 0b100, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_vacc_scd_add_scd_noincr, hasSideEffects = true, Uses = [crSCDEn] in
def VADD_vacc_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b000, 0b011, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_vacc_scd_half_add_scd_incr_half, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADD_vacc_scd_half_add_scd_incr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b001, 0b100, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_vacc_scd_half_add_scd_noincr_half, hasSideEffects = true, Uses = [crSCDEn] in
def VADD_vacc_scd_half_add_scd_noincr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b001, 0b011, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_f_vacc_fp, Defs = [srFPFlags], Uses = [crFPMask] in
def VADD_f_vacc_fp : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mRv8:$acc), "vadd.f", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_fp__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b010, 0b000, acc2, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_f_vacc_fp_scd_add_scd_incr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADD_f_vacc_fp_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd.f", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_fp_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b010, 0b100, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_f_vacc_fp_scd_add_scd_noincr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VADD_f_vacc_fp_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd.f", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_fp_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b010, 0b011, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_f_vacc_half_fp, Defs = [srFPFlags], Uses = [crFPMask] in
def VADD_f_vacc_half_fp : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mRv8:$acc), "vadd.f", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_half_fp__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b011, 0b000, acc2, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_f_vacc_half_fp_scd_add_scd_incr_half, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADD_f_vacc_half_fp_scd_add_scd_incr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd.f", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_half_fp_scd__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b011, 0b100, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_f_vacc_half_fp_scd_add_scd_noincr_half, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VADD_f_vacc_half_fp_scd_add_scd_noincr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd.f", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_half_fp_scd__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b011, 0b011, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_mx6_vacc_fp_bfp13, Defs = [srFPFlags], Uses = [crFPMask] in
def VADD_mx6_vacc_fp_bfp13 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mRv8:$acc), "vadd.mx6", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_fp_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_reg__vmac_acc2_comp0__add_reg__mDMm__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b100, 0b000, acc2, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_mx6_vacc_fp_bfp13_scd_add_scd_incr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADD_mx6_vacc_fp_bfp13_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd.mx6", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_fp_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b100, 0b100, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_mx6_vacc_fp_bfp13_scd_add_scd_noincr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VADD_mx6_vacc_fp_bfp13_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd.mx6", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_fp_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_noincr__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b100, 0b011, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_mx6_vacc_half_fp_bfp13, Defs = [srFPFlags], Uses = [crFPMask] in
def VADD_mx6_vacc_half_fp_bfp13 : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mRv8:$acc), "vadd.mx6", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_half_fp_bfp13__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_reg_half__vmac_acc2_comp0__add_reg_half__mCMLm__vacc_half_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b101, 0b000, acc2, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_mx6_vacc_half_fp_bfp13_scd_add_scd_incr_half, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VADD_mx6_vacc_half_fp_bfp13_scd_add_scd_incr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd.mx6", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_half_fp_bfp13_scd__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_scd_half__vmac_acc2_comp0__add_scd_half__vmac_acc2_comp0__add_scd_incr_half__vmac_scd_cml_dyn__vmac_scd_dm_comp0__vmac_scd_cml_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vacc_half_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b101, 0b100, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VADD_mx6_vacc_half_fp_bfp13_scd_add_scd_noincr_half, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VADD_mx6_vacc_half_fp_bfp13_scd_add_scd_noincr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vadd.mx6", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_half_fp_bfp13_scd__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_scd_half__vmac_acc2_comp0__add_scd_half__vmac_acc2_comp0__add_scd_noincr_half__vmac_scd_cml_reg__vmac_scd_dm_comp0__vmac_scd_cml_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vacc_half_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b101, 0b011, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VBAND, Uses = [crBF8conf, crFP8conf] in
def VBAND : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vband", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_log__vband__vec_add_mX__vec_add_nX__vec_size16
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b0, 0b10, 0b1000111};
}

let Itinerary = II_VBCSTSHFL_16_vec_broadcast_shuffle_bm in
def VBCSTSHFL_16_vec_broadcast_shuffle_bm : AIE2PS_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins OP_mRm:$src, mR29_insert:$r29), "vbcstshfl.16", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_16__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b01, 0b0, 0b0100101111};
}

let Itinerary = II_VBCSTSHFL_16_vec_broadcast_shuffle_x in
def VBCSTSHFL_16_vec_broadcast_shuffle_x : AIE2PS_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mRm:$src, mR29_insert:$r29), "vbcstshfl.16", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_16__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b01, 0b1, 0b0100101111};
}

let Itinerary = II_VBCSTSHFL_32_vec_broadcast_shuffle_bm in
def VBCSTSHFL_32_vec_broadcast_shuffle_bm : AIE2PS_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins OP_mRm:$src, mR29_insert:$r29), "vbcstshfl.32", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_32__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b10, 0b0, 0b0100101111};
}

let Itinerary = II_VBCSTSHFL_32_vec_broadcast_shuffle_x in
def VBCSTSHFL_32_vec_broadcast_shuffle_x : AIE2PS_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mRm:$src, mR29_insert:$r29), "vbcstshfl.32", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_32__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b10, 0b1, 0b0100101111};
}

let Itinerary = II_VBCSTSHFL_64_vec_broadcast_shuffle_bm in
def VBCSTSHFL_64_vec_broadcast_shuffle_bm : AIE2PS_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins eL:$src, mR29_insert:$r29), "vbcstshfl.64", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mR29_insert
bits<4> dst;
bits<4> src;

let mv = {dst, src, dontcare{1}, 0b11, 0b0, 0b0100101111};
}

let Itinerary = II_VBCSTSHFL_64_vec_broadcast_shuffle_x in
def VBCSTSHFL_64_vec_broadcast_shuffle_x : AIE2PS_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins eL:$src, mR29_insert:$r29), "vbcstshfl.64", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mR29_insert
bits<4> dst;
bits<4> src;

let mv = {dst, src, dontcare{1}, 0b11, 0b1, 0b0100101111};
}

let Itinerary = II_VBCSTSHFL_8_vec_broadcast_shuffle_bm in
def VBCSTSHFL_8_vec_broadcast_shuffle_bm : AIE2PS_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins OP_mRm:$src, mR29_insert:$r29), "vbcstshfl.8", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_8__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b00, 0b0, 0b0100101111};
}

let Itinerary = II_VBCSTSHFL_8_vec_broadcast_shuffle_x in
def VBCSTSHFL_8_vec_broadcast_shuffle_x : AIE2PS_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mRm:$src, mR29_insert:$r29), "vbcstshfl.8", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_8__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b00, 0b1, 0b0100101111};
}

let Itinerary = II_VBCST_16 in
def VBCST_16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mRm:$src), "vbcst.16", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_R__s2v_wide_16__mMvXDst
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b01, 0b00010101111};
}

let Itinerary = II_VBCST_32 in
def VBCST_32 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mRm:$src), "vbcst.32", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_R__s2v_wide_32__mMvXDst
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b10, 0b00010101111};
}

let Itinerary = II_VBCST_64 in
def VBCST_64 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins eL:$src), "vbcst.64", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mMvXDst
bits<4> dst;
bits<4> src;

let mv = {dst, src, dontcare{1}, 0b11, 0b00010101111};
}

let Itinerary = II_VBCST_8 in
def VBCST_8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mRm:$src), "vbcst.8", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_R__s2v_wide_8__mMvXDst
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b00, 0b00010101111};
}

let Itinerary = II_VBNEG_LTZ_s16, Uses = [crBF8conf, crFP8conf] in
def VBNEG_LTZ_s16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXn:$s2), "vbneg_ltz.s16", " $d, r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vbneg_ltz__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b01, 0b0, 0b00110101111};
}

let Itinerary = II_VBNEG_LTZ_s32, Uses = [crBF8conf, crFP8conf] in
def VBNEG_LTZ_s32 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXn:$s2), "vbneg_ltz.s32", " $d, r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vbneg_ltz__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b00, 0b0, 0b00110101111};
}

let Itinerary = II_VBNEG_LTZ_s8, Uses = [crBF8conf, crFP8conf] in
def VBNEG_LTZ_s8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXn:$s2), "vbneg_ltz.s8", " $d, r17:r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vbneg_ltz__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b10, 0b0, 0b00110101111};
}

let Itinerary = II_VBOR, Uses = [crBF8conf, crFP8conf] in
def VBOR : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vbor", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_log__vbor__vec_add_mX__vec_add_nX__vec_size16
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b1, 0b10, 0b1000111};
}

let Itinerary = II_VCLR_vclr in
def VCLR_vclr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins), "vclr", "$dst">{
// id: me__instr128_or__vec__vclr__mDMv
bits<3> dst;

let vec = {dontcare{3-1}, dst, dontcare{3-1}, dontcare{1}, dontcare{1}, 0b0101, dontcare{3-1}, dontcare{3-1}, 0b101, dontcare{1}};
}

let Itinerary = II_VCLR_vclr_half in
def VCLR_vclr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins), "vclr", "$dst">{
// id: me__instr128_or__vec__vclr_half__mCMLv
bits<3> dst;

let vec = {dontcare{3-1}, dst, dontcare{3-1}, dontcare{1}, dontcare{1}, 0b1101, dontcare{3-1}, dontcare{3-1}, 0b101, dontcare{1}};
}

let Itinerary = II_VCLR_f_vclr_fp in
def VCLR_f_vclr_fp : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins), "vclr.f", "$dst">{
// id: me__instr128_or__vec__vclr_fp__mDMv
bits<3> dst;

let vec = {dontcare{3-1}, dst, dontcare{3-1}, dontcare{1}, dontcare{1}, 0b111, 0b1111, 0b111, 0b111, 0b1};
}

let Itinerary = II_VCLR_f_vclr_half_fp in
def VCLR_f_vclr_half_fp : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins), "vclr.f", "$dst">{
// id: me__instr128_or__vec__vclr_half_fp__mCMLv
bits<3> dst;

let vec = {dontcare{3-1}, dst, dontcare{3-1}, dontcare{1}, dontcare{1}, 0b101, 0b1111, 0b111, 0b111, 0b1};
}

let Itinerary = II_VCONV_bf16_fp32_mv_conv_mv_w_srs_bf16, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd] in
def VCONV_bf16_fp32_mv_conv_mv_w_srs_bf16 : AIE2PS_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mBMs:$src), "vconv.bf16.fp32", " $dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_bf16__srs_f__mW
bits<5> dst;
bits<5> src;

let st = {dst, 0b0, 0b101, src, 0b1, 0b010111};
}

let Itinerary = II_VCONV_bf16_fp32_mv_conv_mv_x_srs_bf16, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd] in
def VCONV_bf16_fp32_mv_conv_mv_x_srs_bf16 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mCMs:$src), "vconv.bf16.fp32", " $dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_bf16__srs_f__mX
bits<4> dst;
bits<4> src;

let st = {dst, dontcare{1}, 0b0, 0b100, src, 0b0, 0b1, 0b010111};
}

let Itinerary = II_VCONV_bf16_fp32_mv_mcd_bf16_cm, hasSideEffects = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crMCDEn, crRnd] in
def VCONV_bf16_fp32_mv_mcd_bf16_cm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src), "vconv.bf16.fp32", " mcd, $src">{
// id: me__instr128_or__st__mv_mcd_bf16_cm__srs_f__mX__mMCD_E3
bits<4> src;

let st = {dontcare{9-1}, src, 0b11001010};
}

let Itinerary = II_VCONV_bf8_fp32, Defs = [srF2FFlags], Uses = [crBF8conf, crF2FMask, crFPConvSat, crRnd] in
def VCONV_bf8_fp32 : AIE2PS_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mCMs:$src, eS:$i), "vconv.bf8.fp32", " $dst, $src, $i">{
// id: me__instr128_or__st__mv_w_srs_bf8__srs_f__mXl__mSs
bits<5> dst;
bits<4> src;
bits<2> i;

let st = {dst, dontcare{4-1}, src, dontcare{1}, i, 0b00101};
}

let Itinerary = II_VCONV_fp16_fp32_mv_mcd_fp16_cm, hasSideEffects = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crMCDEn, crRnd] in
def VCONV_fp16_fp32_mv_mcd_fp16_cm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src), "vconv.fp16.fp32", " mcd, $src">{
// id: me__instr128_or__st__mv_mcd_fp16_cm__srs_f__mX__mMCD_E3
bits<4> src;

let st = {dontcare{9-1}, src, 0b00101010};
}

let Itinerary = II_VCONV_fp16_fp32_mv_x_srs_fp16, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd] in
def VCONV_fp16_fp32_mv_x_srs_fp16 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mCMs:$src), "vconv.fp16.fp32", " $dst, $src">{
// id: me__instr128_or__st__mv_x_srs_fp16__srs_f__mX
bits<4> dst;
bits<4> src;

let st = {dst, dontcare{5-1}, src, 0b10101010};
}

let Itinerary = II_VCONV_fp32_bf16_alu, hasSideEffects = true, Uses = [crSCDEn] in
def VCONV_fp32_bf16_alu : AIE2PS_inst_alu_instr32 <(outs OP_mCMa:$dst), (ins), "vconv.fp32.bf16", " $dst, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_bf_cm_slotX__mv_scd_bf_cm__ups_f__double__fp32_from_mode_bf16__mv_scd_E2__short__mSCD_E2__mSCD_comp_E2__mSCD
bits<4> dst;

let alu = {0b0101, 0b1, 0b001, dst, 0b0, 0b000, 0b0000};
}

let Itinerary = II_VCONV_fp32_bf16_mv_mv_w_ups_bf16 in
def VCONV_fp32_bf16_mv_mv_w_ups_bf16 : AIE2PS_inst_mv_instr32 <(outs OP_mBMa:$dst), (ins OP_mWm:$src), "vconv.fp32.bf16", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_w_ups_bf16
bits<5> dst;
bits<5> src;

let mv = {dst, dontcare{1}, src, 0b11000101111};
}

let Itinerary = II_VCONV_fp32_bf16_mv_mv_x_ups_f16 in
def VCONV_fp32_bf16_mv_mv_x_ups_f16 : AIE2PS_inst_mv_instr32 <(outs OP_mCMa:$dst), (ins OP_mXm:$src), "vconv.fp32.bf16", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_x_ups_f16__ups_f__double__fp32_from_mode_bf16
bits<4> dst;
bits<4> src;

let mv = {dst, dontcare{2-1}, src, 0b0, 0b01100101111};
}

let Itinerary = II_VCONV_fp32_fp16_alu, hasSideEffects = true, Uses = [crSCDEn] in
def VCONV_fp32_fp16_alu : AIE2PS_inst_alu_instr32 <(outs OP_mCMa:$dst), (ins), "vconv.fp32.fp16", " $dst, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_bf_cm_slotX__mv_scd_bf_cm__ups_f__double__fp32_from_mode_fp16__mv_scd_E2__short__mSCD_E2__mSCD_comp_E2__mSCD
bits<4> dst;

let alu = {0b0101, 0b1, 0b001, dst, 0b1, 0b000, 0b0000};
}

let Itinerary = II_VCONV_fp32_fp16_mv in
def VCONV_fp32_fp16_mv : AIE2PS_inst_mv_instr32 <(outs OP_mCMa:$dst), (ins OP_mXm:$src), "vconv.fp32.fp16", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_x_ups_f16__ups_f__double__fp32_from_mode_fp16
bits<4> dst;
bits<4> src;

let mv = {dst, dontcare{2-1}, src, 0b1, 0b01100101111};
}

let Itinerary = II_VCONV_fp8_fp32, Defs = [srF2FFlags], Uses = [crF2FMask, crFP8conf, crFPConvSat, crRnd] in
def VCONV_fp8_fp32 : AIE2PS_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mCMs:$src, eS:$i), "vconv.fp8.fp32", " $dst, $src, $i">{
// id: me__instr128_or__st__mv_w_srs_fp8__srs_f__mXl__mSs
bits<5> dst;
bits<4> src;
bits<2> i;

let st = {dst, dontcare{4-1}, src, dontcare{1}, i, 0b10101};
}

let Itinerary = II_VCONV_mx4_fp32, Defs = [srF2BFlags], Uses = [crF2BMask, crRnd] in
def VCONV_mx4_fp32 : AIE2PS_inst_st_instr32 <(outs OP_mFEWa:$dst), (ins eDM:$src), "vconv.mx4.fp32", "$dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_fp32_to_bfp11__bfp__mFp2Bfp11__bfp__basis__mDMs
bits<5> dst;
bits<3> src;

let st = {dst, 0b0, 0b0, dontcare{2-1}, 0b1, src, dontcare{1}, 0b1, 0b010111};
}

let Itinerary = II_VCONV_mx6_fp32, Defs = [srF2BFlags], Uses = [crF2BMask, crRnd] in
def VCONV_mx6_fp32 : AIE2PS_inst_st_instr32 <(outs OP_mFEWa:$dst), (ins eDM:$src), "vconv.mx6.fp32", "$dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_fp32_to_bfp13__bfp__mFp2Bfp13__bfp__basis__mDMs
bits<5> dst;
bits<3> src;

let st = {dst, 0b0, 0b0, dontcare{2-1}, 0b0, src, dontcare{1}, 0b1, 0b010111};
}

let Itinerary = II_VCONV_mx9_fp32, Defs = [srF2BFlags], Uses = [crF2BMask, crRnd] in
def VCONV_mx9_fp32 : AIE2PS_inst_st_instr32 <(outs OP_mEXa:$dst), (ins eDM:$src), "vconv.mx9.fp32", "$dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_fp32_to_bfp16__bfp__mFp2Bfp16__bfp__basis__mDMs
bits<4> dst;
bits<3> src;

let st = {dst, 0b1, 0b0, 0b111, dontcare{1}, src, dontcare{1}, 0b1, 0b010111};
}

let Itinerary = II_VCONV_s32_bf16_fp32, Defs = [srF2FFlags, srF2IFlags], Uses = [crF2FMask, crF2IMask, crFPConvSat, crRnd] in
def VCONV_s32_bf16_fp32 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mBMs:$src, eS:$shft), "vconv.s32.bf16.fp32", "$dst, $src, $shft">{
// id: me__instr128_or__st__mv_conv__mv_fp32_to_int_bm__bf16_to_int_E2__mv__srs_f__mW__mSs
bits<4> dst;
bits<2> shft;
bits<5> src;

let st = {dst, 0b1, 0b1, shft, dontcare{1}, src, 0b1, 0b010111};
}

let Itinerary = II_VEQZ_16, Uses = [crBF8conf, crFP8conf] in
def VEQZ_16 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXn:$s2), "veqz.16", " $cmp, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out32__vec_add_nX
bits<4> cmp;
bits<4> s2;

let mv = {cmp, s2, 0b01, 0b111010101111};
}

let Itinerary = II_VEQZ_32, Uses = [crBF8conf, crFP8conf] in
def VEQZ_32 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXn:$s2), "veqz.32", " $cmp, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out16__vec_add_nX
bits<4> cmp;
bits<4> s2;

let mv = {cmp, s2, 0b10, 0b111010101111};
}

let Itinerary = II_VEQZ_8, Uses = [crBF8conf, crFP8conf] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VEQZ_8_mLBypass, [OperandRegClass<0, eL>]>] in
// END MANUAL ADDITION
def VEQZ_8 : AIE2PS_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXn:$s2), "veqz.8", " $cmp, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out64__mLm__vec_add_nX
bits<4> cmp;
bits<4> s2;

let mv = {cmp, s2, 0b00, 0b111010101111};
}

let Itinerary = II_VEXP2_bf16 in
def VEXP2_bf16 : AIE2PS_inst_mv_instr32 <(outs OP_mWm:$dst), (ins OP_mBMm:$src), "vexp2.bf16", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf_16
bits<5> dst;
bits<5> src;

let mv = {dst, src, 0b001110101111};
}

let Itinerary = II_VEXP2_fp32 in
def VEXP2_fp32 : AIE2PS_inst_mv_instr32 <(outs OP_mBMm:$dst), (ins OP_mBMm:$src), "vexp2.fp32", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf_32
bits<5> dst;
bits<5> src;

let mv = {dst, src, 0b101110101111};
}

let Itinerary = II_VEXTBCSTSHFL_128 in
def VEXTBCSTSHFL_128 : AIE2PS_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, OP_mRm:$idx, mR29_insert:$r29), "vextbcstshfl.128", " $dst, $s1, $idx, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, idx, 0b00, 0b0000111};
}

let Itinerary = II_VEXTBCSTSHFL_16 in
def VEXTBCSTSHFL_16 : AIE2PS_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, OP_mRm:$idx, mR29_insert:$r29), "vextbcstshfl.16", " $dst, $s1, $idx, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__s2v_wide2_16__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, idx, 0b01, 0b0000111};
}

let Itinerary = II_VEXTBCSTSHFL_32 in
def VEXTBCSTSHFL_32 : AIE2PS_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, OP_mRm:$idx, mR29_insert:$r29), "vextbcstshfl.32", " $dst, $s1, $idx, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__s2v_wide2_32__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, idx, 0b10, 0b0000111};
}

let Itinerary = II_VEXTBCSTSHFL_64 in
def VEXTBCSTSHFL_64 : AIE2PS_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, OP_mRm:$idx, mR29_insert:$r29), "vextbcstshfl.64", " $dst, $s1, $idx, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__s2v_wide2_64__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, idx, 0b11, 0b0000111};
}

let Itinerary = II_VEXTBCST_128_vec_extract_broadcast_imm in
def VEXTBCST_128_vec_extract_broadcast_imm : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.128", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_128__mMvXDst
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b100, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_128_vec_extract_broadcast_r in
def VEXTBCST_128_vec_extract_broadcast_r : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextbcst.128", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_128__mMvXDst
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b100, idx, 0b101011};
}

let Itinerary = II_VEXTBCST_16_vec_extract_broadcast_imm in
def VEXTBCST_16_vec_extract_broadcast_imm : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.16", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_16__mMvXDst
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b001, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_16_vec_extract_broadcast_r in
def VEXTBCST_16_vec_extract_broadcast_r : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextbcst.16", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_16__mMvXDst
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b001, idx, 0b101011};
}

let Itinerary = II_VEXTBCST_32_vec_extract_broadcast_imm in
def VEXTBCST_32_vec_extract_broadcast_imm : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.32", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_32__mMvXDst
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b010, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_32_vec_extract_broadcast_r in
def VEXTBCST_32_vec_extract_broadcast_r : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextbcst.32", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_32__mMvXDst
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b010, idx, 0b101011};
}

let Itinerary = II_VEXTBCST_64_vec_extract_broadcast_imm in
def VEXTBCST_64_vec_extract_broadcast_imm : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.64", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_64__mMvXDst
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b011, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_64_vec_extract_broadcast_r in
def VEXTBCST_64_vec_extract_broadcast_r : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextbcst.64", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_64__mMvXDst
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b011, idx, 0b101011};
}

let Itinerary = II_VEXTBCST_8_vec_extract_broadcast_imm in
def VEXTBCST_8_vec_extract_broadcast_imm : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.8", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_8__mMvXDst
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b000, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_8_vec_extract_broadcast_r in
def VEXTBCST_8_vec_extract_broadcast_r : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextbcst.8", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_8__mMvXDst
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b000, idx, 0b101011};
}

let Itinerary = II_VEXTRACT_16_vec_extract_imm_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_16_vec_extract_imm_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.16", " $dst, $s1, #$idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__vaddSign0__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b01, 0b0, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_16_vec_extract_imm_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_16_vec_extract_imm_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.16", " $dst, $s1, #$idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__vaddSign1__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b01, 0b1, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_16_vec_extract_r_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_16_vec_extract_r_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextract.16", " $dst, $s1, $idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__vaddSign0
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b01, 0b0, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_16_vec_extract_r_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_16_vec_extract_r_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextract.16", " $dst, $s1, $idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__vaddSign1
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b01, 0b1, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_32_vec_extract_imm_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_32_vec_extract_imm_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.32", " $dst, $s1, #$idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__vaddSign0__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b10, 0b0, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_32_vec_extract_imm_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_32_vec_extract_imm_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.32", " $dst, $s1, #$idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__vaddSign1__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b10, 0b1, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_32_vec_extract_r_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_32_vec_extract_r_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextract.32", " $dst, $s1, $idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__vaddSign0
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b10, 0b0, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_32_vec_extract_r_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_32_vec_extract_r_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextract.32", " $dst, $s1, $idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__vaddSign1
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b10, 0b1, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_64_vec_extract_imm_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_64_vec_extract_imm_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs eL:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.64", " $dst, $s1, #$idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign0__mIdxImm
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, dontcare{1}, s1, 0b11, 0b0, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_64_vec_extract_imm_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_64_vec_extract_imm_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs eL:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.64", " $dst, $s1, #$idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign1__mIdxImm
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, dontcare{1}, s1, 0b11, 0b1, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_64_vec_extract_r_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_64_vec_extract_r_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs eL:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextract.64", " $dst, $s1, $idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign0
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, dontcare{1}, s1, 0b11, 0b0, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_64_vec_extract_r_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_64_vec_extract_r_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs eL:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextract.64", " $dst, $s1, $idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign1
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, dontcare{1}, s1, 0b11, 0b1, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_8_vec_extract_imm_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_8_vec_extract_imm_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.8", " $dst, $s1, #$idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__vaddSign0__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b00, 0b0, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_8_vec_extract_imm_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_8_vec_extract_imm_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.8", " $dst, $s1, #$idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__vaddSign1__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b00, 0b1, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_8_vec_extract_r_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_8_vec_extract_r_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextract.8", " $dst, $s1, $idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__vaddSign0
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b00, 0b0, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_8_vec_extract_r_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_8_vec_extract_r_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRm:$dst), (ins OP_mXm:$s1, OP_mRm:$idx), "vextract.8", " $dst, $s1, $idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__vaddSign1
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b00, 0b1, idx, 0b00110};
}

let Itinerary = II_VFLOOR_s32_bf16, Defs = [srF2IFlags], Uses = [crF2IMask] in
def VFLOOR_s32_bf16 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mFl2FxSrc_W:$src, eS:$shft), "vfloor.s32.bf16", "$dst, $src, $shft">{
// id: me__instr128_or__st__mv_conv__mv_bf16_to_int_w__bf16_to_int_E1__mv__mSs
bits<4> dst;
bits<2> shft;
bits<5> src;

let st = {dst, 0b0, 0b1, shft, dontcare{1}, src, 0b1, 0b010111};
}

let Itinerary = II_VGE_16_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VGE_16_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.16", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out32__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b0, 0b01, 0b001011};
}

let Itinerary = II_VGE_16_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VGE_16_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.16", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out32__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b1, 0b01, 0b001011};
}

let Itinerary = II_VGE_32_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VGE_32_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.32", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out16__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b0, 0b10, 0b001011};
}

let Itinerary = II_VGE_32_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VGE_32_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.32", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out16__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b1, 0b10, 0b001011};
}

let Itinerary = II_VGE_8_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VGE_8_vaddSign0_mLBypass, [OperandRegClass<0, eL>]>] in
// END MANUAL ADDITION
def VGE_8_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.8", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b0, 0b00, 0b001011};
}

let Itinerary = II_VGE_8_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VGE_8_vaddSign1_mLBypass, [OperandRegClass<0, eL>]>] in
// END MANUAL ADDITION
def VGE_8_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.8", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b1, 0b00, 0b001011};
}

let Itinerary = II_VGE_bf16, Uses = [crBF8conf, crFP8conf] in
def VGE_bf16 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.bf16", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_compare__vge_bf16__vec_cmp_out32__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b01, 0b01110111};
}

let Itinerary = II_VGE_bf8, Uses = [crBF8conf, crFP8conf] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VGE_bf8_mLBypass, [OperandRegClass<0, eL>]>] in
// END MANUAL ADDITION
def VGE_bf8 : AIE2PS_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.bf8", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vge_bf8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b01, 0b00001111};
}

let Itinerary = II_VGE_fp16, Uses = [crBF8conf, crFP8conf] in
def VGE_fp16 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.fp16", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_compare__vge_fp16__vec_cmp_out32__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b11, 0b01110111};
}

let Itinerary = II_VGE_fp8, Uses = [crBF8conf, crFP8conf] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VGE_fp8_mLBypass, [OperandRegClass<0, eL>]>] in
// END MANUAL ADDITION
def VGE_fp8 : AIE2PS_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.fp8", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vge_fp8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b11, 0b00001111};
}

let Itinerary = II_VINSERT_16_mIdxImm0 in
def VINSERT_16_mIdxImm0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, OP_mRm:$src), "vinsert.16", " $dst, $s1, #0, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_16__mInsertIdxRed__mIdxImm0
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b0, src, 0b01, 0b011011};
}

let Itinerary = II_VINSERT_16_mR29_insert in
def VINSERT_16_mR29_insert : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, mR29_insert:$r29, OP_mRm:$src), "vinsert.16", " $dst, $s1, r29, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_16__mInsertIdxRed__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b1, src, 0b01, 0b011011};
}

let Itinerary = II_VINSERT_32_mIdxImm0 in
def VINSERT_32_mIdxImm0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, OP_mRm:$src), "vinsert.32", " $dst, $s1, #0, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_32__mInsertIdxRed__mIdxImm0
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b0, src, 0b10, 0b011011};
}

let Itinerary = II_VINSERT_32_mR29_insert in
def VINSERT_32_mR29_insert : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, mR29_insert:$r29, OP_mRm:$src), "vinsert.32", " $dst, $s1, r29, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_32__mInsertIdxRed__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b1, src, 0b10, 0b011011};
}

let Itinerary = II_VINSERT_64_mIdxImm0 in
def VINSERT_64_mIdxImm0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eL:$src), "vinsert.64", " $dst, $s1, #0, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mInsertIdxRed__mIdxImm0
bits<4> dst;
bits<4> s1;
bits<4> src;

let mv = {dst, s1, 0b0, src, dontcare{1}, 0b11, 0b011011};
}

let Itinerary = II_VINSERT_64_mR29_insert in
def VINSERT_64_mR29_insert : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, mR29_insert:$r29, eL:$src), "vinsert.64", " $dst, $s1, r29, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mInsertIdxRed__mR29_insert
bits<4> dst;
bits<4> s1;
bits<4> src;

let mv = {dst, s1, 0b1, src, dontcare{1}, 0b11, 0b011011};
}

let Itinerary = II_VINSERT_8_mIdxImm0 in
def VINSERT_8_mIdxImm0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, OP_mRm:$src), "vinsert.8", " $dst, $s1, #0, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_8__mInsertIdxRed__mIdxImm0
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b0, src, 0b00, 0b011011};
}

let Itinerary = II_VINSERT_8_mR29_insert in
def VINSERT_8_mR29_insert : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, mR29_insert:$r29, OP_mRm:$src), "vinsert.8", " $dst, $s1, r29, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_8__mInsertIdxRed__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b1, src, 0b00, 0b011011};
}

let Itinerary = II_VLDA_128_dmv_lda_w_idx, mayLoad = true in
def VLDA_128_dmv_lda_w_idx : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst), (ins eP:$ptr, eDJ:$dj), "vlda.128", "$dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b0110100};
}

let Itinerary = II_VLDA_128_dmv_lda_w_idx_imm, mayLoad = true in
def VLDA_128_dmv_lda_w_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst), (ins eP:$ptr, c8s_step16:$imm), "vlda.128", "$dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b0110100};
}

let Itinerary = II_VLDA_128_dmv_lda_w_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_128_dmv_lda_w_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda.128", "$dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b0110100};
}

let Itinerary = II_VLDA_128_dmv_lda_w_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_128_dmv_lda_w_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out), (ins eP:$ptr, c8s_step16:$imm), "vlda.128", "$dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b0110100};
}

let Itinerary = II_VLDA_128_dmv_lda_w_spill, mayLoad = true, Uses = [sp] in
def VLDA_128_dmv_lda_w_spill : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst), (ins c14n_step16:$imm), "vlda.128", "$dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_w_spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b1110100};
}

let Itinerary = II_VLDA_2D_128, DecoderMethod = "DecodeVLDA_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_128 : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d.128", "$dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b0110100};
}

let Itinerary = II_VLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf16, DecoderMethod = "DecodeVLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf16", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf16 : AIE2PS_inst_lda_instr32 <(outs OP_mBMa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d.conv.fp32.bf16", "$dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_bf16_core
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b0001100};
}

let Itinerary = II_VLDA_2D_CONV_fp32_bf16_dmx_lda_ups_f16, DecoderMethod = "DecodeVLDA_2D_CONV_fp32", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_CONV_fp32_bf16_dmx_lda_ups_f16 : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d.conv.fp32.bf16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b0, 0b1011100};
}

let Itinerary = II_VLDA_2D_CONV_fp32_fp16, DecoderMethod = "DecodeVLDA_2D_CONV_fp32", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_CONV_fp32_fp16 : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d.conv.fp32.fp16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b1, 0b1011100};
}

let Itinerary = II_VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0, DecoderMethod = "DecodeVLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$base, eP:$ptr, eD:$mod), "vlda.2d.ups.2x", "$dst, $base, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<5> dst;
bits<2> base;

let lda = {ptr, mod, 0b100, dst, base, 0b0, 0b0001};
}

let Itinerary = II_VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1, DecoderMethod = "DecodeVLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$base, eP:$ptr, eD:$mod), "vlda.2d.ups.2x", "$dst, $base, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<5> dst;
bits<2> base;

let lda = {ptr, mod, 0b100, dst, base, 0b1, 0b0001};
}

let Itinerary = II_VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0, DecoderMethod = "DecodeVLDA_2D_UPS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$base, eP:$ptr, eD:$mod), "vlda.2d.ups.2x", "$dst, $base, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, mod, 0b100, dst, dontcare{1}, base, 0b0, 0b0010};
}

let Itinerary = II_VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1, DecoderMethod = "DecodeVLDA_2D_UPS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$base, eP:$ptr, eD:$mod), "vlda.2d.ups.2x", "$dst, $base, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, mod, 0b100, dst, dontcare{1}, base, 0b1, 0b0010};
}

let Itinerary = II_VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0, DecoderMethod = "DecodeVLDA_2D_UPS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$base, eP:$ptr, eD:$mod), "vlda.2d.ups.4x", "$dst, $base, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, mod, 0b100, dst, dontcare{1}, base, 0b0, 0b1001};
}

let Itinerary = II_VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1, DecoderMethod = "DecodeVLDA_2D_UPS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$base, eP:$ptr, eD:$mod), "vlda.2d.ups.4x", "$dst, $base, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, mod, 0b100, dst, dontcare{1}, base, 0b1, 0b1001};
}

let Itinerary = II_VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign0, DecoderMethod = "DecodeVLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign0 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$base, eP:$ptr, eD:$mod), "vlda.2d.ups.4x", "$dst, $base, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<3> dst;
bits<2> base;

let lda = {ptr, mod, 0b100, dst, dontcare{2-1}, base, 0b0, 0b1010};
}

let Itinerary = II_VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign1, DecoderMethod = "DecodeVLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign1 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$base, eP:$ptr, eD:$mod), "vlda.2d.ups.4x", "$dst, $base, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<3> dst;
bits<2> base;

let lda = {ptr, mod, 0b100, dst, dontcare{2-1}, base, 0b1, 0b1010};
}

let Itinerary = II_VLDA_2D_dmw_lda_feg2, DecoderMethod = "DecodeVLDA_2D_dmw_lda_feg2", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_dmw_lda_feg2 : AIE2PS_inst_lda_instr32 <(outs OP_mFEG2a:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b00110101};
}

let Itinerary = II_VLDA_2D_dmw_lda_w, DecoderMethod = "DecodeVLDA_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_dmw_lda_w : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b1001100};
}

let Itinerary = II_VLDA_2D_dmx_lda_bm, DecoderMethod = "DecodeVLDA_2D_dmx_lda_bm", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_dmx_lda_bm : AIE2PS_inst_lda_instr32 <(outs OP_mBM_ld:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b1101100};
}

let Itinerary = II_VLDA_2D_dmx_lda_x, DecoderMethod = "DecodeVLDA_2D_dmx_lda_x", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_dmx_lda_x : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b11110101};
}

let Itinerary = II_VLDA_3D_128, DecoderMethod = "DecodeVLDA_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_128 : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d.128", "$dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b0110100};
}

let Itinerary = II_VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf16, DecoderMethod = "DecodeVLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf16", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf16 : AIE2PS_inst_lda_instr32 <(outs OP_mBMa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d.conv.fp32.bf16", "$dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_bf16_core
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b0001100};
}

let Itinerary = II_VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_f16, DecoderMethod = "DecodeVLDA_3D_CONV_fp32", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_f16 : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d.conv.fp32.bf16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b0, 0b1011100};
}

let Itinerary = II_VLDA_3D_CONV_fp32_fp16, DecoderMethod = "DecodeVLDA_3D_CONV_fp32", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_CONV_fp32_fp16 : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d.conv.fp32.fp16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b1, 0b1011100};
}

let Itinerary = II_VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0, DecoderMethod = "DecodeVLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$base, eP:$ptr, eDS:$mod), "vlda.3d.ups.2x", "$dst, $base, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<2> mod;
bits<5> dst;
bits<2> base;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, base, 0b0, 0b0001};
}

let Itinerary = II_VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1, DecoderMethod = "DecodeVLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$base, eP:$ptr, eDS:$mod), "vlda.3d.ups.2x", "$dst, $base, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<2> mod;
bits<5> dst;
bits<2> base;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, base, 0b1, 0b0001};
}

let Itinerary = II_VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0, DecoderMethod = "DecodeVLDA_3D_UPS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$base, eP:$ptr, eDS:$mod), "vlda.3d.ups.2x", "$dst, $base, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<2> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{1}, base, 0b0, 0b0010};
}

let Itinerary = II_VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1, DecoderMethod = "DecodeVLDA_3D_UPS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$base, eP:$ptr, eDS:$mod), "vlda.3d.ups.2x", "$dst, $base, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<2> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{1}, base, 0b1, 0b0010};
}

let Itinerary = II_VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0, DecoderMethod = "DecodeVLDA_3D_UPS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$base, eP:$ptr, eDS:$mod), "vlda.3d.ups.4x", "$dst, $base, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<2> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{1}, base, 0b0, 0b1001};
}

let Itinerary = II_VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1, DecoderMethod = "DecodeVLDA_3D_UPS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$base, eP:$ptr, eDS:$mod), "vlda.3d.ups.4x", "$dst, $base, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<2> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{1}, base, 0b1, 0b1001};
}

let Itinerary = II_VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0, DecoderMethod = "DecodeVLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$base, eP:$ptr, eDS:$mod), "vlda.3d.ups.4x", "$dst, $base, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<2> mod;
bits<3> dst;
bits<2> base;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{2-1}, base, 0b0, 0b1010};
}

let Itinerary = II_VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1, DecoderMethod = "DecodeVLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsS", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$base, eP:$ptr, eDS:$mod), "vlda.3d.ups.4x", "$dst, $base, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<2> mod;
bits<3> dst;
bits<2> base;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{2-1}, base, 0b1, 0b1010};
}

let Itinerary = II_VLDA_3D_dmw_lda_feg2, DecoderMethod = "DecodeVLDA_3D_dmw_lda_feg2", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_dmw_lda_feg2 : AIE2PS_inst_lda_instr32 <(outs OP_mFEG2a:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b00110101};
}

let Itinerary = II_VLDA_3D_dmw_lda_w, DecoderMethod = "DecodeVLDA_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_dmw_lda_w : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b1001100};
}

let Itinerary = II_VLDA_3D_dmx_lda_bm, DecoderMethod = "DecodeVLDA_3D_dmx_lda_bm", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_dmx_lda_bm : AIE2PS_inst_lda_instr32 <(outs OP_mBM_ld:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b1101100};
}

let Itinerary = II_VLDA_3D_dmx_lda_x, DecoderMethod = "DecodeVLDA_3D_dmx_lda_x", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_dmx_lda_x : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b11110101};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_idx, mayLoad = true in
def VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_idx : AIE2PS_inst_lda_instr32 <(outs OP_mBMa:$dst), (ins eP:$ptr, eDJ:$dj), "vlda.conv.fp32.bf16", "$dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_bf16_core
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b0001100};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_idx_imm, mayLoad = true in
def VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mBMa:$dst), (ins eP:$ptr, c9s_step32:$imm), "vlda.conv.fp32.bf16", "$dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_bf16_core
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b0001100};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mBMa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda.conv.fp32.bf16", "$dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_bf16_core
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b0001100};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_bf16_dmw_lda_ups_bf16_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mBMa:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vlda.conv.fp32.bf16", "$dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_bf16__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_bf16_core
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b0001100};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_idx, mayLoad = true in
def VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_idx : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst), (ins eP:$ptr, eDJ:$dj), "vlda.conv.fp32.bf16", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b0, 0b1011100};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_idx_imm, mayLoad = true in
def VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst), (ins eP:$ptr, c10s_step64:$imm), "vlda.conv.fp32.bf16", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b0, 0b1011100};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda.conv.fp32.bf16", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b0, 0b1011100};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_bf16_dmx_lda_ups_f16_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vlda.conv.fp32.bf16", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_bf16
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b0, 0b1011100};
}

let Itinerary = II_VLDA_CONV_fp32_fp16_idx, mayLoad = true in
def VLDA_CONV_fp32_fp16_idx : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst), (ins eP:$ptr, eDJ:$dj), "vlda.conv.fp32.fp16", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b1, 0b1011100};
}

let Itinerary = II_VLDA_CONV_fp32_fp16_idx_imm, mayLoad = true in
def VLDA_CONV_fp32_fp16_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst), (ins eP:$ptr, c10s_step64:$imm), "vlda.conv.fp32.fp16", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b1, 0b1011100};
}

let Itinerary = II_VLDA_CONV_fp32_fp16_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_fp16_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda.conv.fp32.fp16", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b1, 0b1011100};
}

let Itinerary = II_VLDA_CONV_fp32_fp16_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_fp16_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mCMa:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vlda.conv.fp32.fp16", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_f16__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_f16_core__ups_f__double__fp32_from_mode_fp16
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b1, 0b1011100};
}

let Itinerary = II_VLDA_FILL, DecoderMethod = "DecodeVLDA_FILL", mayLoad = true in
def VLDA_FILL : AIE2PS_inst_lda_instr32 <(outs ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos), "vlda.fill", " [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__normal_fill__dmx_lda_fifo_x__core_fill__dmx_lda_fifo_x__normal__mPfa__mLdFifo_a
bits<1> ptr;

let lda = {0b00, ptr, dontcare{3-1}, 0b111, dontcare{4-1}, 0b01110101};
}

let Itinerary = II_VLDA_POP_2D_dmx_lda_fifo_ex_bfp16, DecoderMethod = "DecodeVLDA_POP_2D_1", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_2D_dmx_lda_fifo_ex_bfp16 : AIE2PS_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eD:$mod), "vlda.pop.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_bfp16__fifo_2d_pop__dmx_lda_fifo_ex_bfp16__core_pop__dmx_lda_fifo_ex_bfp16__fifo_2d__mPfa__mLdFifo_a__mDa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b01, ptr, mod, 0b100, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_2D_dmx_lda_fifo_fex_bfp13, DecoderMethod = "DecodeVLDA_POP_2D", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_2D_dmx_lda_fifo_fex_bfp13 : AIE2PS_inst_lda_instr32 <(outs OP_mFEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eD:$mod), "vlda.pop.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp13__fifo_2d_pop__dmx_lda_fifo_fex_bfp13__core_pop__dmx_lda_fifo_fex_bfp13__fifo_2d__mPfa__mLdFifo_a__mDa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b01, ptr, mod, 0b101, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_2D_dmx_lda_fifo_x, DecoderMethod = "DecodeVLDA_POP_2D_dmx_lda_fifo_x", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_2D_dmx_lda_fifo_x : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eD:$mod), "vlda.pop.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__fifo_2d_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__fifo_2d__mPfa__mLdFifo_a__mDa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b01, ptr, mod, 0b000, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_3D_dmx_lda_fifo_ex_bfp16, DecoderMethod = "DecodeVLDA_POP_3D_1", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_3D_dmx_lda_fifo_ex_bfp16 : AIE2PS_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eDS:$mod), "vlda.pop.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_bfp16__fifo_3d_pop__dmx_lda_fifo_ex_bfp16__core_pop__dmx_lda_fifo_ex_bfp16__fifo_3d__mPfa__mLdFifo_a__mDSa
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let lda = {0b10, ptr, dontcare{1}, mod, 0b100, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_3D_dmx_lda_fifo_fex_bfp13, DecoderMethod = "DecodeVLDA_POP_3D", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_3D_dmx_lda_fifo_fex_bfp13 : AIE2PS_inst_lda_instr32 <(outs OP_mFEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eDS:$mod), "vlda.pop.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp13__fifo_3d_pop__dmx_lda_fifo_fex_bfp13__core_pop__dmx_lda_fifo_fex_bfp13__fifo_3d__mPfa__mLdFifo_a__mDSa
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let lda = {0b10, ptr, dontcare{1}, mod, 0b101, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_3D_dmx_lda_fifo_x, DecoderMethod = "DecodeVLDA_POP_3D_dmx_lda_fifo_x", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_3D_dmx_lda_fifo_x : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eDS:$mod), "vlda.pop.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__fifo_3d_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__fifo_3d__mPfa__mLdFifo_a__mDSa
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let lda = {0b10, ptr, dontcare{1}, mod, 0b000, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_dmx_lda_fifo_ex_bfp16_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_dmx_lda_fifo_ex_bfp16_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_dmx_lda_fifo_ex_bfp16_fifo_1d_pop : AIE2PS_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eM:$mod), "vlda.pop", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_bfp16__fifo_1d_pop__dmx_lda_fifo_ex_bfp16__core_pop__dmx_lda_fifo_ex_bfp16__fifo_1d__mPfa__mLdFifo_a__mMa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b11, ptr, mod, 0b100, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_dmx_lda_fifo_ex_bfp16_normal_pop, DecoderMethod = "DecodeVLDA_POP_dmx_lda_fifo_ex_bfp16_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_dmx_lda_fifo_ex_bfp16_normal_pop : AIE2PS_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos), "vlda.pop", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_bfp16__normal_pop__dmx_lda_fifo_ex_bfp16__core_pop__dmx_lda_fifo_ex_bfp16__normal__mPfa__mLdFifo_a
bits<1> ptr;
bits<4> dst;

let lda = {0b00, ptr, dontcare{3-1}, 0b100, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_dmx_lda_fifo_fex_bfp13_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_dmx_lda_fifo_fex_bfp13_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_dmx_lda_fifo_fex_bfp13_fifo_1d_pop : AIE2PS_inst_lda_instr32 <(outs OP_mFEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eM:$mod), "vlda.pop", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp13__fifo_1d_pop__dmx_lda_fifo_fex_bfp13__core_pop__dmx_lda_fifo_fex_bfp13__fifo_1d__mPfa__mLdFifo_a__mMa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b11, ptr, mod, 0b101, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_dmx_lda_fifo_fex_bfp13_normal_pop, DecoderMethod = "DecodeVLDA_POP_dmx_lda_fifo_fex_bfp13_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_dmx_lda_fifo_fex_bfp13_normal_pop : AIE2PS_inst_lda_instr32 <(outs OP_mFEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos), "vlda.pop", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp13__normal_pop__dmx_lda_fifo_fex_bfp13__core_pop__dmx_lda_fifo_fex_bfp13__normal__mPfa__mLdFifo_a
bits<1> ptr;
bits<4> dst;

let lda = {0b00, ptr, dontcare{3-1}, 0b101, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_dmx_lda_fifo_x_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_dmx_lda_fifo_x_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_dmx_lda_fifo_x_fifo_1d_pop : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eM:$mod), "vlda.pop", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__fifo_1d_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__fifo_1d__mPfa__mLdFifo_a__mMa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b11, ptr, mod, 0b000, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_dmx_lda_fifo_x_normal_pop, DecoderMethod = "DecodeVLDA_POP_dmx_lda_fifo_x_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_dmx_lda_fifo_x_normal_pop : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos), "vlda.pop", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__normal_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__normal__mPfa__mLdFifo_a
bits<1> ptr;
bits<4> dst;

let lda = {0b00, ptr, dontcare{3-1}, 0b000, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_mx4_2D, DecoderMethod = "DecodeVLDA_POP_2D", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_mx4_2D : AIE2PS_inst_lda_instr32 <(outs OP_mFEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eD:$mod), "vlda.pop.mx4.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp11__fifo_2d_pop__dmx_lda_fifo_fex_bfp11__core_pop__dmx_lda_fifo_fex_bfp11__fifo_2d__mPfa__mLdFifo_a__mDa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b01, ptr, mod, 0b011, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_mx4_3D, DecoderMethod = "DecodeVLDA_POP_3D", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_mx4_3D : AIE2PS_inst_lda_instr32 <(outs OP_mFEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eDS:$mod), "vlda.pop.mx4.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp11__fifo_3d_pop__dmx_lda_fifo_fex_bfp11__core_pop__dmx_lda_fifo_fex_bfp11__fifo_3d__mPfa__mLdFifo_a__mDSa
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let lda = {0b10, ptr, dontcare{1}, mod, 0b011, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_mx4_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_mx4_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_mx4_fifo_1d_pop : AIE2PS_inst_lda_instr32 <(outs OP_mFEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eM:$mod), "vlda.pop.mx4", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp11__fifo_1d_pop__dmx_lda_fifo_fex_bfp11__core_pop__dmx_lda_fifo_fex_bfp11__fifo_1d__mPfa__mLdFifo_a__mMa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b11, ptr, mod, 0b011, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_mx4_normal_pop, DecoderMethod = "DecodeVLDA_POP_mx4_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_mx4_normal_pop : AIE2PS_inst_lda_instr32 <(outs OP_mFEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos), "vlda.pop.mx4", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_fex_bfp11__normal_pop__dmx_lda_fifo_fex_bfp11__core_pop__dmx_lda_fifo_fex_bfp11__normal__mPfa__mLdFifo_a
bits<1> ptr;
bits<4> dst;

let lda = {0b00, ptr, dontcare{3-1}, 0b011, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_s_2D, DecoderMethod = "DecodeVLDA_POP_2D_1", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_s_2D : AIE2PS_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eD:$mod), "vlda.pop.s.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__fifo_2d_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__fifo_2d__mPfa__mLdFifo_a__mDa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b01, ptr, mod, 0b001, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_s_3D, DecoderMethod = "DecodeVLDA_POP_3D_1", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_s_3D : AIE2PS_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eDS:$mod), "vlda.pop.s.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__fifo_3d_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__fifo_3d__mPfa__mLdFifo_a__mDSa
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let lda = {0b10, ptr, dontcare{1}, mod, 0b001, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_s_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_s_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_s_fifo_1d_pop : AIE2PS_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos, eM:$mod), "vlda.pop.s", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__fifo_1d_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__fifo_1d__mPfa__mLdFifo_a__mMa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b11, ptr, mod, 0b001, dst, 0b01110101};
}

let Itinerary = II_VLDA_POP_s_normal_pop, DecoderMethod = "DecodeVLDA_POP_s_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_s_normal_pop : AIE2PS_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2a:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2a:$pos), "vlda.pop.s", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__normal_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__normal__mPfa__mLdFifo_a
bits<1> ptr;
bits<4> dst;

let lda = {0b00, ptr, dontcare{3-1}, 0b001, dst, 0b01110101};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eS:$base, eP:$ptr, eDJ:$dj), "vlda.ups.2x", "$dst, $base, upssign0, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> dj;
bits<5> dst;
bits<2> base;

let lda = {ptr, dj, 0b000, dst, base, 0b0, 0b0001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eS:$base, eP:$ptr, eDJ:$dj), "vlda.ups.2x", "$dst, $base, upssign1, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> dj;
bits<5> dst;
bits<2> base;

let lda = {ptr, dj, 0b000, dst, base, 0b1, 0b0001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eS:$base, eP:$ptr, c9s_step32:$imm), "vlda.ups.2x", "$dst, $base, upssign0, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<5> dst;
bits<2> base;

let lda = {ptr, imm, 0b01, dst, base, 0b0, 0b0001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eS:$base, eP:$ptr, c9s_step32:$imm), "vlda.ups.2x", "$dst, $base, upssign1, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<5> dst;
bits<2> base;

let lda = {ptr, imm, 0b01, dst, base, 0b1, 0b0001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, eM:$mod), "vlda.ups.2x", "$dst, $base, upssign0, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<5> dst;
bits<2> base;

let lda = {ptr, mod, 0b010, dst, base, 0b0, 0b0001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, eM:$mod), "vlda.ups.2x", "$dst, $base, upssign1, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<5> dst;
bits<2> base;

let lda = {ptr, mod, 0b010, dst, base, 0b1, 0b0001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, c9s_step32:$imm), "vlda.ups.2x", "$dst, $base, upssign0, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<5> dst;
bits<2> base;

let lda = {ptr, imm, 0b11, dst, base, 0b0, 0b0001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, c9s_step32:$imm), "vlda.ups.2x", "$dst, $base, upssign1, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<5> dst;
bits<2> base;

let lda = {ptr, imm, 0b11, dst, base, 0b1, 0b0001};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$base, eP:$ptr, eDJ:$dj), "vlda.ups.2x", "$dst, $base, upssign0, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> dj;
bits<4> dst;
bits<2> base;

let lda = {ptr, dj, 0b000, dst, dontcare{1}, base, 0b0, 0b0010};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$base, eP:$ptr, eDJ:$dj), "vlda.ups.2x", "$dst, $base, upssign1, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> dj;
bits<4> dst;
bits<2> base;

let lda = {ptr, dj, 0b000, dst, dontcare{1}, base, 0b1, 0b0010};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$base, eP:$ptr, c10s_step64:$imm), "vlda.ups.2x", "$dst, $base, upssign0, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> base;

let lda = {ptr, imm, 0b01, dst, dontcare{1}, base, 0b0, 0b0010};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$base, eP:$ptr, c10s_step64:$imm), "vlda.ups.2x", "$dst, $base, upssign1, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> base;

let lda = {ptr, imm, 0b01, dst, dontcare{1}, base, 0b1, 0b0010};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, eM:$mod), "vlda.ups.2x", "$dst, $base, upssign0, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, mod, 0b010, dst, dontcare{1}, base, 0b0, 0b0010};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, eM:$mod), "vlda.ups.2x", "$dst, $base, upssign1, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, mod, 0b010, dst, dontcare{1}, base, 0b1, 0b0010};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, c10s_step64:$imm), "vlda.ups.2x", "$dst, $base, upssign0, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> base;

let lda = {ptr, imm, 0b11, dst, dontcare{1}, base, 0b0, 0b0010};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, c10s_step64:$imm), "vlda.ups.2x", "$dst, $base, upssign1, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> base;

let lda = {ptr, imm, 0b11, dst, dontcare{1}, base, 0b1, 0b0010};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$base, eP:$ptr, eDJ:$dj), "vlda.ups.4x", "$dst, $base, upssign0, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> dj;
bits<4> dst;
bits<2> base;

let lda = {ptr, dj, 0b000, dst, dontcare{1}, base, 0b0, 0b1001};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$base, eP:$ptr, eDJ:$dj), "vlda.ups.4x", "$dst, $base, upssign1, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> dj;
bits<4> dst;
bits<2> base;

let lda = {ptr, dj, 0b000, dst, dontcare{1}, base, 0b1, 0b1001};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$base, eP:$ptr, c9s_step32:$imm), "vlda.ups.4x", "$dst, $base, upssign0, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> base;

let lda = {ptr, imm, 0b01, dst, dontcare{1}, base, 0b0, 0b1001};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$base, eP:$ptr, c9s_step32:$imm), "vlda.ups.4x", "$dst, $base, upssign1, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> base;

let lda = {ptr, imm, 0b01, dst, dontcare{1}, base, 0b1, 0b1001};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, eM:$mod), "vlda.ups.4x", "$dst, $base, upssign0, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, mod, 0b010, dst, dontcare{1}, base, 0b0, 0b1001};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, eM:$mod), "vlda.ups.4x", "$dst, $base, upssign1, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> base;

let lda = {ptr, mod, 0b010, dst, dontcare{1}, base, 0b1, 0b1001};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, c9s_step32:$imm), "vlda.ups.4x", "$dst, $base, upssign0, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> base;

let lda = {ptr, imm, 0b11, dst, dontcare{1}, base, 0b0, 0b1001};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, c9s_step32:$imm), "vlda.ups.4x", "$dst, $base, upssign1, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> base;

let lda = {ptr, imm, 0b11, dst, dontcare{1}, base, 0b1, 0b1001};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign0 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst), (ins eS:$base, eP:$ptr, eDJ:$dj), "vlda.ups.4x", "$dst, $base, upssign0, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> dj;
bits<3> dst;
bits<2> base;

let lda = {ptr, dj, 0b000, dst, dontcare{2-1}, base, 0b0, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign1 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst), (ins eS:$base, eP:$ptr, eDJ:$dj), "vlda.ups.4x", "$dst, $base, upssign1, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> dj;
bits<3> dst;
bits<2> base;

let lda = {ptr, dj, 0b000, dst, dontcare{2-1}, base, 0b1, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst), (ins eS:$base, eP:$ptr, c10s_step64:$imm), "vlda.ups.4x", "$dst, $base, upssign0, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<3> dst;
bits<2> base;

let lda = {ptr, imm, 0b01, dst, dontcare{2-1}, base, 0b0, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst), (ins eS:$base, eP:$ptr, c10s_step64:$imm), "vlda.ups.4x", "$dst, $base, upssign1, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<3> dst;
bits<2> base;

let lda = {ptr, imm, 0b01, dst, dontcare{2-1}, base, 0b1, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, eM:$mod), "vlda.ups.4x", "$dst, $base, upssign0, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<3> dst;
bits<2> base;

let lda = {ptr, mod, 0b010, dst, dontcare{2-1}, base, 0b0, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, eM:$mod), "vlda.ups.4x", "$dst, $base, upssign1, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<3> dst;
bits<2> base;

let lda = {ptr, mod, 0b010, dst, dontcare{2-1}, base, 0b1, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign0 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, c10s_step64:$imm), "vlda.ups.4x", "$dst, $base, upssign0, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<3> dst;
bits<2> base;

let lda = {ptr, imm, 0b11, dst, dontcare{2-1}, base, 0b0, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign1 : AIE2PS_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out), (ins eS:$base, eP:$ptr, c10s_step64:$imm), "vlda.ups.4x", "$dst, $base, upssign1, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<3> dst;
bits<2> base;

let lda = {ptr, imm, 0b11, dst, dontcare{2-1}, base, 0b1, 0b1010};
}

let Itinerary = II_VLDA_dmw_lda_feg2_ld_idx, mayLoad = true in
def VLDA_dmw_lda_feg2_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mFEG2a:$dst), (ins eP:$ptr, eDJ:$dj), "vlda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b00110101};
}

let Itinerary = II_VLDA_dmw_lda_feg2_ld_idx_imm, mayLoad = true in
def VLDA_dmw_lda_feg2_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mFEG2a:$dst), (ins eP:$ptr, c9s_step32:$imm), "vlda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b00110101};
}

let Itinerary = II_VLDA_dmw_lda_feg2_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmw_lda_feg2_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mFEG2a:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b00110101};
}

let Itinerary = II_VLDA_dmw_lda_feg2_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmw_lda_feg2_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mFEG2a:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vlda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_feg2__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b00110101};
}

let Itinerary = II_VLDA_dmw_lda_feg2_spill, mayLoad = true, Uses = [sp] in
def VLDA_dmw_lda_feg2_spill : AIE2PS_inst_lda_instr32 <(outs OP_mFEG2a:$dst), (ins c15n_step32:$imm), "vlda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_feg2__spill__agua_dmw__nrm_spill__agua_dmw__spill__mSPa__m_c15n_step32
bits<9> imm;
bits<4> dst;

let lda = {imm, dst, 0b10110101};
}

let Itinerary = II_VLDA_dmw_lda_w_ld_idx, mayLoad = true in
def VLDA_dmw_lda_w_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst), (ins eP:$ptr, eDJ:$dj), "vlda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b1001100};
}

let Itinerary = II_VLDA_dmw_lda_w_ld_idx_imm, mayLoad = true in
def VLDA_dmw_lda_w_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst), (ins eP:$ptr, c9s_step32:$imm), "vlda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b1001100};
}

let Itinerary = II_VLDA_dmw_lda_w_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmw_lda_w_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b1001100};
}

let Itinerary = II_VLDA_dmw_lda_w_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmw_lda_w_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vlda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_w__ld__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b1001100};
}

let Itinerary = II_VLDA_dmw_lda_w_spill, mayLoad = true, Uses = [sp] in
def VLDA_dmw_lda_w_spill : AIE2PS_inst_lda_instr32 <(outs OP_mWa:$dst), (ins c15n_step32:$imm), "vlda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_w__spill__agua_dmw__nrm_spill__agua_dmw__spill__mSPa__m_c15n_step32
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b0101100};
}

let Itinerary = II_VLDA_dmx_lda_bm_ld_idx, mayLoad = true in
def VLDA_dmx_lda_bm_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mBM_ld:$dst), (ins eP:$ptr, eDJ:$dj), "vlda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b1101100};
}

let Itinerary = II_VLDA_dmx_lda_bm_ld_idx_imm, mayLoad = true in
def VLDA_dmx_lda_bm_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mBM_ld:$dst), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b1101100};
}

let Itinerary = II_VLDA_dmx_lda_bm_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_bm_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mBM_ld:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b1101100};
}

let Itinerary = II_VLDA_dmx_lda_bm_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_bm_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mBM_ld:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_bm__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b1101100};
}

let Itinerary = II_VLDA_dmx_lda_bm_spill, mayLoad = true, Uses = [sp] in
def VLDA_dmx_lda_bm_spill : AIE2PS_inst_lda_instr32 <(outs OP_mBM_ld:$dst), (ins c16n_step64:$imm), "vlda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_bm__spill__agua_dmx__nrm_spill__agua_dmx__spill__mSPa__m_c16n_step64
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b0011100};
}

let Itinerary = II_VLDA_dmx_lda_x_ld_idx, mayLoad = true in
def VLDA_dmx_lda_x_ld_idx : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst), (ins eP:$ptr, eDJ:$dj), "vlda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b11110101};
}

let Itinerary = II_VLDA_dmx_lda_x_ld_idx_imm, mayLoad = true in
def VLDA_dmx_lda_x_ld_idx_imm : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b11110101};
}

let Itinerary = II_VLDA_dmx_lda_x_ld_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_x_ld_pstm_nrm : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b11110101};
}

let Itinerary = II_VLDA_dmx_lda_x_ld_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_x_ld_pstm_nrm_imm : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_x__ld__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b11110101};
}

let Itinerary = II_VLDA_dmx_lda_x_spill, mayLoad = true, Uses = [sp] in
def VLDA_dmx_lda_x_spill : AIE2PS_inst_lda_instr32 <(outs OP_mXa:$dst), (ins c16n_step64:$imm), "vlda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_x__spill__agua_dmx__nrm_spill__agua_dmx__spill__mSPa__m_c16n_step64
bits<9> imm;
bits<4> dst;

let lda = {imm, dst, 0b00001101};
}

let Itinerary = II_VLDB_128_idx, mayLoad = true in
def VLDB_128_idx : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.128", "$dst, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__idx__mPb__mDJb
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let ldb = {ptr, dj, 0b000, dst, 0b000};
}

let Itinerary = II_VLDB_128_idx_imm, mayLoad = true in
def VLDB_128_idx_imm : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins eP:$ptr, c8s_step16:$imm), "vldb.128", "$dst, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__idx_imm__mPb__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let ldb = {ptr, imm, 0b01, dst, 0b000};
}

let Itinerary = II_VLDB_128_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_128_pstm_nrm : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.128", "$dst, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_nrm__mPb__mMb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let ldb = {ptr, mod, 0b010, dst, 0b000};
}

let Itinerary = II_VLDB_128_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_128_pstm_nrm_imm : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out), (ins eP:$ptr, c8s_step16:$imm), "vldb.128", "$dst, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_nrm_imm__mPb__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let ldb = {ptr, imm, 0b11, dst, 0b000};
}

let Itinerary = II_VLDB_2D_128, DecoderMethod = "DecodeVLDB_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_2D_128 : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.128", "$dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_2d__mPb__mDb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let ldb = {ptr, mod, 0b100, dst, 0b000};
}

let Itinerary = II_VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign0, DecoderMethod = "DecodeVLDB_2D_UNPACK_dmw_ldb_unpack_unpackS", mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.unpack", "$dst, unpacksign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_2d__mPb__mDb__unpackSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b100, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign1, DecoderMethod = "DecodeVLDB_2D_UNPACK_dmw_ldb_unpack_unpackS", mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.unpack", "$dst, unpacksign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_2d__mPb__mDb__unpackSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b100, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign0, DecoderMethod = "DecodeVLDB_2D_UNPACK_dmx_ldb_unpack_unpackS", mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.unpack", "$dst, unpacksign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_2d__mPb__mDb__mYb__unpackSign0
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let ldb = {ptr, mod, 0b100, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign1, DecoderMethod = "DecodeVLDB_2D_UNPACK_dmx_ldb_unpack_unpackS", mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.unpack", "$dst, unpacksign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_2d__mPb__mDb__mYb__unpackSign1
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let ldb = {ptr, mod, 0b100, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_2D_dmw_ldb, DecoderMethod = "DecodeVLDB_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_2D_dmw_ldb : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_2d__mPb__mDb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let ldb = {ptr, mod, 0b100, dst, 0b001};
}

let Itinerary = II_VLDB_2D_dmx_ldb_x, DecoderMethod = "DecodeVLDB_2D_dmx_ldb_x", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_2D_dmx_ldb_x : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_2d__mPb__mDb
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b100, dst, 0b1101};
}

let Itinerary = II_VLDB_3D_128, DecoderMethod = "DecodeVLDB_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_3D_128 : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.128", "$dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_3d__mPb__mDSb
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b000};
}

let Itinerary = II_VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0, DecoderMethod = "DecodeVLDB_3D_UNPACK_dmw_ldb_unpack_unpackS", mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.unpack", "$dst, unpacksign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_3d__mPb__mDSb__unpackSign0
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1, DecoderMethod = "DecodeVLDB_3D_UNPACK_dmw_ldb_unpack_unpackS", mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.unpack", "$dst, unpacksign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_3d__mPb__mDSb__unpackSign1
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0, DecoderMethod = "DecodeVLDB_3D_UNPACK_dmx_ldb_unpack_unpackS", mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.unpack", "$dst, unpacksign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_3d__mPb__mDSb__mYb__unpackSign0
bits<3> ptr;
bits<2> mod;
bits<3> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1, DecoderMethod = "DecodeVLDB_3D_UNPACK_dmx_ldb_unpack_unpackS", mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.unpack", "$dst, unpacksign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_3d__mPb__mDSb__mYb__unpackSign1
bits<3> ptr;
bits<2> mod;
bits<3> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_3D_dmw_ldb, DecoderMethod = "DecodeVLDB_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_3D_dmw_ldb : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_3d__mPb__mDSb
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b001};
}

let Itinerary = II_VLDB_3D_dmx_ldb_x, DecoderMethod = "DecodeVLDB_3D_dmx_ldb_x", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_3D_dmx_ldb_x : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_3d__mPb__mDSb
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b1101};
}

let Itinerary = II_VLDB_4x16_hi, mayLoad = true in
def VLDB_4x16_hi : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x16.hi", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x16__load_4x_pointer_hi
bits<5> src;
bits<5> dst;

let ldb = {src, 0b1, 0b00, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x16_lo, mayLoad = true in
def VLDB_4x16_lo : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x16.lo", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x16__load_4x_pointer_lo
bits<5> src;
bits<5> dst;

let ldb = {src, 0b0, 0b00, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x32_hi, mayLoad = true in
def VLDB_4x32_hi : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x32.hi", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x32__load_4x_pointer_hi
bits<5> src;
bits<5> dst;

let ldb = {src, 0b1, 0b01, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x32_lo, mayLoad = true in
def VLDB_4x32_lo : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x32.lo", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x32__load_4x_pointer_lo
bits<5> src;
bits<5> dst;

let ldb = {src, 0b0, 0b01, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x64_hi, mayLoad = true in
def VLDB_4x64_hi : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x64.hi", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x64__load_4x_pointer_hi
bits<5> src;
bits<5> dst;

let ldb = {src, 0b1, 0b10, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x64_lo, mayLoad = true in
def VLDB_4x64_lo : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x64.lo", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x64__load_4x_pointer_lo
bits<5> src;
bits<5> dst;

let ldb = {src, 0b0, 0b10, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_FILLX, DecoderMethod = "DecodeVLDB_FILLX", mayLoad = true, Defs = [lfe], Uses = [lfe] in
def VLDB_FILLX : AIE2PS_inst_ldb_instr32 <(outs ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins mR30_fifo_step_e1:$conf_e1, mR30_fifo_step_e7:$conf_e7, ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos), "vldb.fillx", " [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__normal_fillx__dmx_ldb_fifo_x__core_fillx__mR30_fifo_step_e1__mR30_fifo_step_e7__dmx_ldb_fifo_x__normalx__mPfb__mLdFifo_b
bits<1> ptr;

let ldb = {0b10, ptr, dontcare{3-1}, 0b111, dontcare{4-1}, 0b0101};
}

let Itinerary = II_VLDB_FILL, DecoderMethod = "DecodeVLDB_FILL", mayLoad = true in
def VLDB_FILL : AIE2PS_inst_ldb_instr32 <(outs ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos), "vldb.fill", " [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__normal_fill__dmx_ldb_fifo_x__core_fill__dmx_ldb_fifo_x__normal__mPfb__mLdFifo_b
bits<1> ptr;

let ldb = {0b00, ptr, dontcare{3-1}, 0b111, dontcare{4-1}, 0b0101};
}

let Itinerary = II_VLDB_POPX, DecoderMethod = "DecodeVLDB_POPX", mayLoad = true, Defs = [lfe, srFifo_uf], Uses = [lfe] in
def VLDB_POPX : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins mR30_fifo_step_e1:$conf_e1, mR30_fifo_step_e7:$conf_e7, ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos), "vldb.popx", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__normal_popx__dmx_ldb_fifo_x__core_popx__mR30_fifo_step_e1__mR30_fifo_step_e7__dmx_ldb_fifo_x__normalx__mPfb__mLdFifo_b
bits<1> ptr;
bits<4> dst;

let ldb = {0b01, ptr, dontcare{3-1}, 0b111, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_2D_dmx_ldb_fifo_ex_bfp16, DecoderMethod = "DecodeVLDB_POP_2D", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_2D_dmx_ldb_fifo_ex_bfp16 : AIE2PS_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eD:$mod), "vldb.pop.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_ex_bfp16__fifo_2d_pop__dmx_ldb_fifo_ex_bfp16__core_pop__dmx_ldb_fifo_ex_bfp16__fifo_2d__mPfb__mLdFifo_b__mDb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b01, ptr, mod, 0b100, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_2D_dmx_ldb_fifo_fex_bfp13, DecoderMethod = "DecodeVLDB_POP_2D_1", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_2D_dmx_ldb_fifo_fex_bfp13 : AIE2PS_inst_ldb_instr32 <(outs OP_mFEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eD:$mod), "vldb.pop.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp13__fifo_2d_pop__dmx_ldb_fifo_fex_bfp13__core_pop__dmx_ldb_fifo_fex_bfp13__fifo_2d__mPfb__mLdFifo_b__mDb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b01, ptr, mod, 0b101, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_2D_dmx_ldb_fifo_x, DecoderMethod = "DecodeVLDB_POP_2D_dmx_ldb_fifo_x", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_2D_dmx_ldb_fifo_x : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eD:$mod), "vldb.pop.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__fifo_2d_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__fifo_2d__mPfb__mLdFifo_b__mDb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b01, ptr, mod, 0b000, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_3D_dmx_ldb_fifo_ex_bfp16, DecoderMethod = "DecodeVLDB_POP_3D", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_3D_dmx_ldb_fifo_ex_bfp16 : AIE2PS_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eDS:$mod), "vldb.pop.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_ex_bfp16__fifo_3d_pop__dmx_ldb_fifo_ex_bfp16__core_pop__dmx_ldb_fifo_ex_bfp16__fifo_3d__mPfb__mLdFifo_b__mDSb
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {0b10, ptr, dontcare{1}, mod, 0b100, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_3D_dmx_ldb_fifo_fex_bfp13, DecoderMethod = "DecodeVLDB_POP_3D_1", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_3D_dmx_ldb_fifo_fex_bfp13 : AIE2PS_inst_ldb_instr32 <(outs OP_mFEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eDS:$mod), "vldb.pop.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp13__fifo_3d_pop__dmx_ldb_fifo_fex_bfp13__core_pop__dmx_ldb_fifo_fex_bfp13__fifo_3d__mPfb__mLdFifo_b__mDSb
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {0b10, ptr, dontcare{1}, mod, 0b101, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_3D_dmx_ldb_fifo_x, DecoderMethod = "DecodeVLDB_POP_3D_dmx_ldb_fifo_x", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_3D_dmx_ldb_fifo_x : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eDS:$mod), "vldb.pop.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__fifo_3d_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__fifo_3d__mPfb__mLdFifo_b__mDSb
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {0b10, ptr, dontcare{1}, mod, 0b000, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_dmx_ldb_fifo_ex_bfp16_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_dmx_ldb_fifo_ex_bfp16_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_dmx_ldb_fifo_ex_bfp16_fifo_1d_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eM:$mod), "vldb.pop", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_ex_bfp16__fifo_1d_pop__dmx_ldb_fifo_ex_bfp16__core_pop__dmx_ldb_fifo_ex_bfp16__fifo_1d__mPfb__mLdFifo_b__mMb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b11, ptr, mod, 0b100, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_dmx_ldb_fifo_ex_bfp16_normal_pop, DecoderMethod = "DecodeVLDB_POP_dmx_ldb_fifo_ex_bfp16_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_dmx_ldb_fifo_ex_bfp16_normal_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos), "vldb.pop", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_ex_bfp16__normal_pop__dmx_ldb_fifo_ex_bfp16__core_pop__dmx_ldb_fifo_ex_bfp16__normal__mPfb__mLdFifo_b
bits<1> ptr;
bits<4> dst;

let ldb = {0b00, ptr, dontcare{3-1}, 0b100, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_dmx_ldb_fifo_fex_bfp13_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_dmx_ldb_fifo_fex_bfp13_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_dmx_ldb_fifo_fex_bfp13_fifo_1d_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mFEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eM:$mod), "vldb.pop", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp13__fifo_1d_pop__dmx_ldb_fifo_fex_bfp13__core_pop__dmx_ldb_fifo_fex_bfp13__fifo_1d__mPfb__mLdFifo_b__mMb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b11, ptr, mod, 0b101, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_dmx_ldb_fifo_fex_bfp13_normal_pop, DecoderMethod = "DecodeVLDB_POP_dmx_ldb_fifo_fex_bfp13_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_dmx_ldb_fifo_fex_bfp13_normal_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mFEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos), "vldb.pop", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp13__normal_pop__dmx_ldb_fifo_fex_bfp13__core_pop__dmx_ldb_fifo_fex_bfp13__normal__mPfb__mLdFifo_b
bits<1> ptr;
bits<4> dst;

let ldb = {0b00, ptr, dontcare{3-1}, 0b101, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_dmx_ldb_fifo_x_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_dmx_ldb_fifo_x_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_dmx_ldb_fifo_x_fifo_1d_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eM:$mod), "vldb.pop", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__fifo_1d_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__fifo_1d__mPfb__mLdFifo_b__mMb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b11, ptr, mod, 0b000, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_dmx_ldb_fifo_x_normal_pop, DecoderMethod = "DecodeVLDB_POP_dmx_ldb_fifo_x_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_dmx_ldb_fifo_x_normal_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos), "vldb.pop", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_x__normal_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__normal__mPfb__mLdFifo_b
bits<1> ptr;
bits<4> dst;

let ldb = {0b00, ptr, dontcare{3-1}, 0b000, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_mx4_2D, DecoderMethod = "DecodeVLDB_POP_2D_1", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_mx4_2D : AIE2PS_inst_ldb_instr32 <(outs OP_mFEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eD:$mod), "vldb.pop.mx4.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp11__fifo_2d_pop__dmx_ldb_fifo_fex_bfp11__core_pop__dmx_ldb_fifo_fex_bfp11__fifo_2d__mPfb__mLdFifo_b__mDb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b01, ptr, mod, 0b011, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_mx4_3D, DecoderMethod = "DecodeVLDB_POP_3D_1", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_mx4_3D : AIE2PS_inst_ldb_instr32 <(outs OP_mFEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eDS:$mod), "vldb.pop.mx4.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp11__fifo_3d_pop__dmx_ldb_fifo_fex_bfp11__core_pop__dmx_ldb_fifo_fex_bfp11__fifo_3d__mPfb__mLdFifo_b__mDSb
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {0b10, ptr, dontcare{1}, mod, 0b011, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_mx4_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_mx4_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_mx4_fifo_1d_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mFEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eM:$mod), "vldb.pop.mx4", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp11__fifo_1d_pop__dmx_ldb_fifo_fex_bfp11__core_pop__dmx_ldb_fifo_fex_bfp11__fifo_1d__mPfb__mLdFifo_b__mMb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b11, ptr, mod, 0b011, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_mx4_normal_pop, DecoderMethod = "DecodeVLDB_POP_mx4_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_mx4_normal_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mFEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos), "vldb.pop.mx4", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_fex_bfp11__normal_pop__dmx_ldb_fifo_fex_bfp11__core_pop__dmx_ldb_fifo_fex_bfp11__normal__mPfb__mLdFifo_b
bits<1> ptr;
bits<4> dst;

let ldb = {0b00, ptr, dontcare{3-1}, 0b011, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_s_2D, DecoderMethod = "DecodeVLDB_POP_2D", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_s_2D : AIE2PS_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eD:$mod), "vldb.pop.s.2d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_qx__fifo_2d_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__fifo_2d__mPfb__mLdFifo_b__mDb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b01, ptr, mod, 0b001, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_s_3D, DecoderMethod = "DecodeVLDB_POP_3D", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_s_3D : AIE2PS_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eDS:$mod), "vldb.pop.s.3d", "$dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_qx__fifo_3d_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__fifo_3d__mPfb__mLdFifo_b__mDSb
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {0b10, ptr, dontcare{1}, mod, 0b001, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_s_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_s_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_s_fifo_1d_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos, eM:$mod), "vldb.pop.s", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_qx__fifo_1d_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__fifo_1d__mPfb__mLdFifo_b__mMb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b11, ptr, mod, 0b001, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_s_normal_pop, DecoderMethod = "DecodeVLDB_POP_s_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_s_normal_pop : AIE2PS_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, OP_mRF2b:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, OP_mRF2b:$pos), "vldb.pop.s", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo_x__dmx_ldb_fifo_qx__normal_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__normal__mPfb__mLdFifo_b
bits<1> ptr;
bits<4> dst;

let ldb = {0b00, ptr, dontcare{3-1}, 0b001, dst, 0b0101};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0] in
def VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.unpack", "$dst, unpacksign0, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx__mPb__mDJb__unpackSign0
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let ldb = {ptr, dj, 0b000, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1] in
def VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.unpack", "$dst, unpacksign1, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx__mPb__mDJb__unpackSign1
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let ldb = {ptr, dj, 0b000, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0] in
def VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, c9s_step32:$imm), "vldb.unpack", "$dst, unpacksign0, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx_imm__mPb__m_c09s_step32__unpackSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b01, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1] in
def VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, c9s_step32:$imm), "vldb.unpack", "$dst, unpacksign1, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx_imm__mPb__m_c09s_step32__unpackSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b01, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.unpack", "$dst, unpacksign0, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm__mPb__mMb__unpackSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b010, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.unpack", "$dst, unpacksign1, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm__mPb__mMb__unpackSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b010, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vldb.unpack", "$dst, unpacksign0, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm_imm__mPb__m_c09s_step32__unpackSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b11, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vldb.unpack", "$dst, unpacksign1, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm_imm__mPb__m_c09s_step32__unpackSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b11, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0] in
def VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.unpack", "$dst, unpacksign0, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx__mPb__mDJb__mYb__unpackSign0
bits<3> ptr;
bits<3> dj;
bits<3> dst;

let ldb = {ptr, dj, 0b000, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1] in
def VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.unpack", "$dst, unpacksign1, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx__mPb__mDJb__mYb__unpackSign1
bits<3> ptr;
bits<3> dj;
bits<3> dst;

let ldb = {ptr, dj, 0b000, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0] in
def VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst), (ins eP:$ptr, c10s_step64:$imm), "vldb.unpack", "$dst, unpacksign0, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx_imm__mPb__m_c10s_step64__mYb__unpackSign0
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let ldb = {ptr, imm, 0b01, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1] in
def VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst), (ins eP:$ptr, c10s_step64:$imm), "vldb.unpack", "$dst, unpacksign1, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx_imm__mPb__m_c10s_step64__mYb__unpackSign1
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let ldb = {ptr, imm, 0b01, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.unpack", "$dst, unpacksign0, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm__mPb__mMb__mYb__unpackSign0
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let ldb = {ptr, mod, 0b010, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.unpack", "$dst, unpacksign1, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm__mPb__mMb__mYb__unpackSign1
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let ldb = {ptr, mod, 0b010, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vldb.unpack", "$dst, unpacksign0, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm_imm__mPb__m_c10s_step64__mYb__unpackSign0
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let ldb = {ptr, imm, 0b11, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vldb.unpack", "$dst, unpacksign1, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm_imm__mPb__m_c10s_step64__mYb__unpackSign1
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let ldb = {ptr, imm, 0b11, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_dmw_ldb_idx, mayLoad = true in
def VLDB_dmw_ldb_idx : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx__mPb__mDJb
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let ldb = {ptr, dj, 0b000, dst, 0b001};
}

let Itinerary = II_VLDB_dmw_ldb_idx_imm, mayLoad = true in
def VLDB_dmw_ldb_idx_imm : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins eP:$ptr, c9s_step32:$imm), "vldb", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx_imm__mPb__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let ldb = {ptr, imm, 0b01, dst, 0b001};
}

let Itinerary = II_VLDB_dmw_ldb_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_dmw_ldb_pstm_nrm : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb", " $dst, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm__mPb__mMb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let ldb = {ptr, mod, 0b010, dst, 0b001};
}

let Itinerary = II_VLDB_dmw_ldb_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_dmw_ldb_pstm_nrm_imm : AIE2PS_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vldb", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm_imm__mPb__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let ldb = {ptr, imm, 0b11, dst, 0b001};
}

let Itinerary = II_VLDB_dmx_ldb_x_idx, mayLoad = true in
def VLDB_dmx_ldb_x_idx : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx__mPb__mDJb
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let ldb = {ptr, dj, 0b000, dst, 0b1101};
}

let Itinerary = II_VLDB_dmx_ldb_x_idx_imm, mayLoad = true in
def VLDB_dmx_ldb_x_idx_imm : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, c10s_step64:$imm), "vldb", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx_imm__mPb__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b01, dst, 0b1101};
}

let Itinerary = II_VLDB_dmx_ldb_x_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_dmx_ldb_x_pstm_nrm : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb", " $dst, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm__mPb__mMb
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b010, dst, 0b1101};
}

let Itinerary = II_VLDB_dmx_ldb_x_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_dmx_ldb_x_pstm_nrm_imm : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vldb", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm_imm__mPb__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b11, dst, 0b1101};
}

let Itinerary = II_VLT_16_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VLT_16_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.16", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out32__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b0, 0b01, 0b001011};
}

let Itinerary = II_VLT_16_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VLT_16_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.16", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out32__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b1, 0b01, 0b001011};
}

let Itinerary = II_VLT_32_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VLT_32_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.32", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out16__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b0, 0b10, 0b001011};
}

let Itinerary = II_VLT_32_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VLT_32_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.32", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out16__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b1, 0b10, 0b001011};
}

let Itinerary = II_VLT_8_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VLT_8_vaddSign0_mLBypass, [OperandRegClass<0, eL>]>] in
// END MANUAL ADDITION
def VLT_8_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.8", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b0, 0b00, 0b001011};
}

let Itinerary = II_VLT_8_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VLT_8_vaddSign1_mLBypass, [OperandRegClass<0, eL>]>] in
// END MANUAL ADDITION
def VLT_8_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.8", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b1, 0b00, 0b001011};
}

let Itinerary = II_VLT_bf16, Uses = [crBF8conf, crFP8conf] in
def VLT_bf16 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.bf16", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_compare__vlt_bf16__vec_cmp_out32__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b00, 0b01110111};
}

let Itinerary = II_VLT_bf8, Uses = [crBF8conf, crFP8conf] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VLT_bf8_mLBypass, [OperandRegClass<0, eL>]>] in
// END MANUAL ADDITION
def VLT_bf8 : AIE2PS_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.bf8", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vlt_bf8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b00, 0b00001111};
}

let Itinerary = II_VLT_fp16, Uses = [crBF8conf, crFP8conf] in
def VLT_fp16 : AIE2PS_inst_mv_instr32 <(outs OP_mRS16m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.fp16", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_compare__vlt_fp16__vec_cmp_out32__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b10, 0b01110111};
}

let Itinerary = II_VLT_fp8, Uses = [crBF8conf, crFP8conf] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VLT_fp8_mLBypass, [OperandRegClass<0, eL>]>] in
// END MANUAL ADDITION
def VLT_fp8 : AIE2PS_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.fp8", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vlt_fp8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b10, 0b00001111};
}

let Itinerary = II_VMAC_vmac_vmul_cm_core_X_QX, Defs = [srSparse_of] in
def VMAC_vmac_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0110, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_vmul_cm_core_X_QY, Defs = [srSparse_of] in
def VMAC_vmac_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_vmul_cm_core_X_R in
def VMAC_vmac_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1000, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_vmul_cm_core_X_X in
def VMAC_vmac_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0000, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_vmul_cm_core_X_Y in
def VMAC_vmac_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_vmul_cm_core_Y_QX, Defs = [srSparse_of] in
def VMAC_vmac_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_vmul_cm_core_Y_R in
def VMAC_vmac_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_vmul_cm_core_Y_X in
def VMAC_vmac_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_vmul_cm_core_Y_Y in
def VMAC_vmac_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eY:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_half_vmul_cm_half_core_X_QX, Defs = [srSparse_of] in
def VMAC_vmac_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1010, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_half_vmul_cm_half_core_X_R in
def VMAC_vmac_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0111, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_vmac_half_vmul_cm_half_core_X_X in
def VMAC_vmac_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0011, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bf_vmul_bf_core_X_QY, DecoderMethod = "DecodeVMAC_f_vmac_bf_vmul_bf_core_X_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMAC_f_vmac_bf_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bf_vmul_bf_core_X_X, DecoderMethod = "DecodeVMAC_f_vmac_bf_vmul_bf_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMAC_f_vmac_bf_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0100, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bf_vmul_bf_core_Y_R, DecoderMethod = "DecodeVMAC_f_vmac_bf_vmul_bf_core_Y_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMAC_f_vmac_bf_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bf_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVMAC_f_vmac_bf_vmul_bf_core_Y_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMAC_f_vmac_bf_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eY:$s2, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bf_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVMAC_f_vmac_bf_half_vmul_bf_core_X_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMAC_f_vmac_bf_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1100, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bf_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVMAC_f_vmac_bf_half_vmul_bf_core_X_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMAC_f_vmac_bf_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bf_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVMAC_f_vmac_bf_half_vmul_bf_core_Y_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMAC_f_vmac_bf_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bf_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVMAC_f_vmac_bf_half_vmul_bf_half_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMAC_f_vmac_bf_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1011, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bfp13, DecoderMethod = "DecodeVMAC_f_vmac_bfp13", Defs = [srFPFlags], Uses = [crFPMask] in
def VMAC_f_vmac_bfp13 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bfp16, DecoderMethod = "DecodeVMAC_f_vmac_bfp16", Defs = [srFPFlags], Uses = [crFPMask] in
def VMAC_f_vmac_bfp16 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vmac.f", "$dst, $acc1, $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bfp16__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;
bits<3> acc1;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, acc1, 0b001, 0b0};
}

let Itinerary = II_VMAXDIFF_LT_16_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VMAXDIFF_LT_16_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b000, 0b0, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_16_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VMAXDIFF_LT_16_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b000, 0b1, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_32_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VMAXDIFF_LT_32_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b000, 0b0, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_32_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VMAXDIFF_LT_32_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b000, 0b1, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_8_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VMAXDIFF_LT_8_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b000, 0b0, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_8_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VMAXDIFF_LT_8_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b000, 0b1, 0b0010};
}

let Itinerary = II_VMAX_LT_16_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VMAX_LT_16_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b100, 0b0, 0b0010};
}

let Itinerary = II_VMAX_LT_16_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VMAX_LT_16_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b100, 0b1, 0b0010};
}

let Itinerary = II_VMAX_LT_32_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VMAX_LT_32_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b100, 0b0, 0b0010};
}

let Itinerary = II_VMAX_LT_32_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VMAX_LT_32_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b100, 0b1, 0b0010};
}

let Itinerary = II_VMAX_LT_8_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VMAX_LT_8_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b100, 0b0, 0b0010};
}

let Itinerary = II_VMAX_LT_8_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VMAX_LT_8_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b100, 0b1, 0b0010};
}

let Itinerary = II_VMAX_LT_bf16, Uses = [crBF8conf, crFP8conf] in
def VMAX_LT_bf16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.bf16", " $d, r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_min_max__vmax_lt_bf16__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b11110111};
}

let Itinerary = II_VMAX_LT_bf8, Uses = [crBF8conf, crFP8conf] in
def VMAX_LT_bf8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.bf8", " $d, r17:r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_min_max__vmax_lt_bf8__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b10001111};
}

let Itinerary = II_VMAX_LT_fp16, Uses = [crBF8conf, crFP8conf] in
def VMAX_LT_fp16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.fp16", " $d, r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_min_max__vmax_lt_fp16__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b11, 0b11110111};
}

let Itinerary = II_VMAX_LT_fp8, Uses = [crBF8conf, crFP8conf] in
def VMAX_LT_fp8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.fp8", " $d, r17:r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_min_max__vmax_lt_fp8__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b11, 0b10001111};
}

let Itinerary = II_VMIN_GE_16_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VMIN_GE_16_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b011, 0b0, 0b0010};
}

let Itinerary = II_VMIN_GE_16_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VMIN_GE_16_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b011, 0b1, 0b0010};
}

let Itinerary = II_VMIN_GE_32_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VMIN_GE_32_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b011, 0b0, 0b0010};
}

let Itinerary = II_VMIN_GE_32_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VMIN_GE_32_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b011, 0b1, 0b0010};
}

let Itinerary = II_VMIN_GE_8_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VMIN_GE_8_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b011, 0b0, 0b0010};
}

let Itinerary = II_VMIN_GE_8_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VMIN_GE_8_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b011, 0b1, 0b0010};
}

let Itinerary = II_VMIN_GE_bf16, Uses = [crBF8conf, crFP8conf] in
def VMIN_GE_bf16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.bf16", " $d, r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_min_max__vmin_ge_bf16__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b11110111};
}

let Itinerary = II_VMIN_GE_bf8, Uses = [crBF8conf, crFP8conf] in
def VMIN_GE_bf8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.bf8", " $d, r17:r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_min_max__vmin_ge_bf8__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b10001111};
}

let Itinerary = II_VMIN_GE_fp16, Uses = [crBF8conf, crFP8conf] in
def VMIN_GE_fp16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.fp16", " $d, r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f16_min_max__vmin_ge_fp16__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b11110111};
}

let Itinerary = II_VMIN_GE_fp8, Uses = [crBF8conf, crFP8conf] in
def VMIN_GE_fp8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.fp8", " $d, r17:r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_min_max__vmin_ge_fp8__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b10001111};
}

let Itinerary = II_VMOV_0_mv_scd_cm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_0_mv_scd_cm : AIE2PS_inst_alu_instr32 <(outs OP_mCMm:$dst), (ins), "vmov.0", " $dst, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cm__mv_scd_E2__long_static__scd_cm_0__mSCD_E2__mSCD_comp_E2__mSCD
bits<4> dst;

let alu = {0b0100, 0b1, 0b001, dst, 0b0, 0b000, 0b0000};
}

let Itinerary = II_VMOV_0_mv_scd_dm_imm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_0_mv_scd_dm_imm : AIE2PS_inst_alu_instr32 <(outs eDM:$scd_core), (ins), "vmov.0", " $scd_core, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_imm__scd_dm_0__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD
bits<3> scd_core;

let alu = {0b1100, 0b1, 0b001, scd_core, 0b00, 0b000, 0b0000};
}

let Itinerary = II_VMOV_1_mv_scd_cm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_1_mv_scd_cm : AIE2PS_inst_alu_instr32 <(outs OP_mCMm:$dst), (ins), "vmov.1", " $dst, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cm__mv_scd_E2__long_static__scd_cm_1__mSCD_E2__mSCD_comp_E2__mSCD
bits<4> dst;

let alu = {0b0100, 0b1, 0b001, dst, 0b1, 0b000, 0b0000};
}

let Itinerary = II_VMOV_1_mv_scd_dm_imm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_1_mv_scd_dm_imm : AIE2PS_inst_alu_instr32 <(outs eDM:$scd_core), (ins), "vmov.1", " $scd_core, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_imm__scd_dm_1__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD
bits<3> scd_core;

let alu = {0b1100, 0b1, 0b001, scd_core, 0b01, 0b000, 0b0000};
}

let Itinerary = II_VMOV_2, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_2 : AIE2PS_inst_alu_instr32 <(outs eDM:$scd_core), (ins), "vmov.2", " $scd_core, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_imm__scd_dm_2__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD
bits<3> scd_core;

let alu = {0b1100, 0b1, 0b001, scd_core, 0b10, 0b000, 0b0000};
}

let Itinerary = II_VMOV_3, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_3 : AIE2PS_inst_alu_instr32 <(outs eDM:$scd_core), (ins), "vmov.3", " $scd_core, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_imm__scd_dm_3__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD
bits<3> scd_core;

let alu = {0b1100, 0b1, 0b001, scd_core, 0b11, 0b000, 0b0000};
}

let Itinerary = II_VMOV_D_vmov in
def VMOV_D_vmov : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1), "vmov.d", "$dst, $acc1">{
// id: me__instr128_or__vec__vmov__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv
bits<3> dst;
bits<3> acc1;

let vec = {dontcare{3-1}, dst, dontcare{3-1}, dontcare{1}, dontcare{1}, 0b0, 0b001, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VMOV_D_vmov_half in
def VMOV_D_vmov_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1), "vmov.d", "$dst, $acc1">{
// id: me__instr128_or__vec__vmov_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv
bits<3> dst;
bits<3> acc1;

let vec = {dontcare{3-1}, dst, dontcare{3-1}, dontcare{1}, dontcare{1}, 0b1, 0b001, dontcare{3-1}, acc1, 0b101, 0b0};
}

let Itinerary = II_VMOV_alu_mv_alu_mv_scd_bm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_alu_mv_alu_mv_scd_bm : AIE2PS_inst_alu_instr32 <(outs OP_mBMm:$dst), (ins), "vmov", "$dst, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_bm__mv_scd_E2__short__mSCD_E2__mSCD_comp_E2__mSCD
bits<5> dst;

let alu = {0b0000, 0b1, 0b001, dst, 0b000, 0b0000};
}

let Itinerary = II_VMOV_alu_mv_alu_mv_scd_cmh_dyn, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$r31_out = $r31" in
def VMOV_alu_mv_alu_mv_scd_cmh_dyn : AIE2PS_inst_alu_instr32 <(outs eCMH:$scd_core, mR31_scd:$r31_out), (ins mR31_scd:$r31), "vmov", "$scd_core, scd, r31++">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cmh_dyn__mv_scd_cm_cmh__mv_scd_cm_dyn__mv_scd_cm_cmh__mv_scd_cm_core__mCMHm__mv_scd_E2__long_dynam_cm__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r_incr__mR31_scd
bits<3> scd_core;

let alu = {0b1001, 0b1, 0b001, scd_core, dontcare{2-1}, 0b000, 0b0000};
}

let Itinerary = II_VMOV_alu_mv_alu_mv_scd_cmh_reg, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_alu_mv_alu_mv_scd_cmh_reg : AIE2PS_inst_alu_instr32 <(outs eCMH:$scd_core), (ins mR31_scd:$r31), "vmov", "$scd_core, scd, r31">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cmh_reg__mv_scd_cm_cmh__mv_scd_cm_reg__mv_scd_cm_cmh__mv_scd_cm_core__mCMHm__mv_scd_E2__long_dynam_cm__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r__mR31_scd
bits<3> scd_core;

let alu = {0b0001, 0b1, 0b001, scd_core, dontcare{2-1}, 0b000, 0b0000};
}

let Itinerary = II_VMOV_alu_mv_alu_mv_scd_cml_dyn, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$r31_out = $r31" in
def VMOV_alu_mv_alu_mv_scd_cml_dyn : AIE2PS_inst_alu_instr32 <(outs eCML:$scd_core, mR31_scd:$r31_out), (ins mR31_scd:$r31), "vmov", "$scd_core, scd, r31++">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cml_dyn__mv_scd_cm_cml__mv_scd_cm_dyn__mv_scd_cm_cml__mv_scd_cm_core__mCMLm__mv_scd_E2__long_dynam_cm__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r_incr__mR31_scd
bits<3> scd_core;

let alu = {0b1110, 0b1, 0b001, scd_core, dontcare{2-1}, 0b000, 0b0000};
}

let Itinerary = II_VMOV_alu_mv_alu_mv_scd_cml_reg, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_alu_mv_alu_mv_scd_cml_reg : AIE2PS_inst_alu_instr32 <(outs eCML:$scd_core), (ins mR31_scd:$r31), "vmov", "$scd_core, scd, r31">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_cml_reg__mv_scd_cm_cml__mv_scd_cm_reg__mv_scd_cm_cml__mv_scd_cm_core__mCMLm__mv_scd_E2__long_dynam_cm__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r__mR31_scd
bits<3> scd_core;

let alu = {0b0110, 0b1, 0b001, scd_core, dontcare{2-1}, 0b000, 0b0000};
}

let Itinerary = II_VMOV_alu_mv_alu_mv_scd_dm_dyn, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$r31_out = $r31" in
def VMOV_alu_mv_alu_mv_scd_dm_dyn : AIE2PS_inst_alu_instr32 <(outs eDM:$scd_core, mR31_scd:$r31_out), (ins mR31_scd:$r31), "vmov", "$scd_core, scd, r31++">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_dyn__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r_incr__mR31_scd
bits<3> scd_core;

let alu = {0b1010, 0b1, 0b001, scd_core, dontcare{2-1}, 0b000, 0b0000};
}

let Itinerary = II_VMOV_alu_mv_alu_mv_scd_dm_reg, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_alu_mv_alu_mv_scd_dm_reg : AIE2PS_inst_alu_instr32 <(outs eDM:$scd_core), (ins mR31_scd:$r31), "vmov", "$scd_core, scd, r31">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_dm_reg__mv_scd_dm_core__mDMm__mv_scd_E2__long_dynam__mSCD_E2__mSCD_comp_E2__mSCD__mSCD_r__mR31_scd
bits<3> scd_core;

let alu = {0b0010, 0b1, 0b001, scd_core, dontcare{2-1}, 0b000, 0b0000};
}

let Itinerary = II_VMOV_alu_mv_alu_mv_scd_x, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_alu_mv_alu_mv_scd_x : AIE2PS_inst_alu_instr32 <(outs OP_mXa:$dst), (ins), "vmov", "$dst, scd">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mv_scd_slotX__mv_scd_x__mv_scd_E2__short__mSCD_E2__mSCD_comp_E2__mSCD
bits<4> dst;

let alu = {0b1000, 0b1, 0b001, dst, dontcare{1}, 0b000, 0b0000};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_cm in
def VMOV_alu_mv_mv_mv_cm : AIE2PS_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mCMm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cm
bits<4> dst;
bits<4> src;

let mv = {dst, dontcare{2-1}, src, 0b010110101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_ee in
def VMOV_alu_mv_mv_mv_ee : AIE2PS_inst_mv_instr32 <(outs OP_mEEm:$dst), (ins OP_mEEm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ee
bits<4> dst;
bits<4> src;

let mv = {dst, src, 0b00000001101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_eg in
def VMOV_alu_mv_mv_mv_eg : AIE2PS_inst_mv_instr32 <(outs OP_mEGm:$dst), (ins OP_mEGm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_eg
bits<5> dst;
bits<5> src;

let mv = {dst, src, 0b110110101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_eg2 in
def VMOV_alu_mv_mv_mv_eg2 : AIE2PS_inst_mv_instr32 <(outs OP_mEG2m:$dst), (ins OP_mEG2m:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_eg2
bits<4> dst;
bits<4> src;

let mv = {dst, src, 0b10000001101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_eg2_to_w in
def VMOV_alu_mv_mv_mv_eg2_to_w : AIE2PS_inst_mv_instr32 <(outs OP_mWm:$dst), (ins OP_mEG2m:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_eg2_to_w
bits<5> dst;
bits<4> src;

let mv = {dst, src, 0b0011110101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_egw in
def VMOV_alu_mv_mv_mv_egw : AIE2PS_inst_mv_instr32 <(outs OP_mEWm:$dst), (ins OP_mEWm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_egw
bits<5> dst;
bits<5> src;

let mv = {dst, src, 0b001010101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_egx in
def VMOV_alu_mv_mv_mv_egx : AIE2PS_inst_mv_instr32 <(outs OP_mEXm:$dst), (ins OP_mEXm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_egx
bits<4> dst;
bits<4> src;

let mv = {dst, src, 0b01000001101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_f in
def VMOV_alu_mv_mv_mv_f : AIE2PS_inst_mv_instr32 <(outs OP_mFm:$dst), (ins OP_mFm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_f
bits<5> dst;
bits<5> src;

let mv = {dst, src, 0b101010101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_f_to_l in
def VMOV_alu_mv_mv_mv_f_to_l : AIE2PS_inst_mv_instr32 <(outs eL:$dst), (ins OP_mFm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_f_to_l__mLm
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b1011110101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_few in
def VMOV_alu_mv_mv_mv_few : AIE2PS_inst_mv_instr32 <(outs OP_mFEWm:$dst), (ins OP_mFEWm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_few
bits<5> dst;
bits<5> src;

let mv = {dst, src, 0b011010101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_fex in
def VMOV_alu_mv_mv_mv_fex : AIE2PS_inst_mv_instr32 <(outs OP_mFEXm:$dst), (ins OP_mFEXm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_fex
bits<4> dst;
bits<4> src;

let mv = {dst, src, 0b11000001101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_ff in
def VMOV_alu_mv_mv_mv_ff : AIE2PS_inst_mv_instr32 <(outs OP_mFFm:$dst), (ins OP_mFFm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ff
bits<4> dst;
bits<4> src;

let mv = {dst, src, 0b00100001101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_gg in
def VMOV_alu_mv_mv_mv_gg : AIE2PS_inst_mv_instr32 <(outs OP_mGGm:$dst), (ins OP_mGGm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_gg
bits<4> dst;
bits<4> src;

let mv = {dst, src, 0b10100001101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_l_to_f in
def VMOV_alu_mv_mv_mv_l_to_f : AIE2PS_inst_mv_instr32 <(outs OP_mFm:$dst), (ins eL:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_l_to_f__mLm
bits<5> dst;
bits<4> src;

let mv = {dst, src, 0b0111110101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_w in
def VMOV_alu_mv_mv_mv_w : AIE2PS_inst_mv_instr32 <(outs OP_mWm:$dst), (ins OP_mWm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_w
bits<5> dst;
bits<5> src;

let mv = {dst, 0b0, src, 0b0, 0b1111001111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_w_to_eg2 in
def VMOV_alu_mv_mv_mv_w_to_eg2 : AIE2PS_inst_mv_instr32 <(outs OP_mEG2m:$dst), (ins OP_mWm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_w_to_eg2
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b1111110101111};
}

let Itinerary = II_VMOV_alu_mv_mv_mv_x, ItineraryRegPairs = [
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_eXe, [OperandRegClass<0, eXe>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_eXe, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eXe, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_eXe, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_eXe, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eXe, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eXe, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_eXe, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_eXe, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eXe, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_eXe, [OperandRegClass<0, eXo>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_mStFifol, [OperandRegClass<0, eXe>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_eLdFifoLReg, [OperandRegClass<0, eXe>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_eBMHH, [OperandRegClass<0, eXe>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_eBMHL, [OperandRegClass<0, eXe>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_eLdFifoHReg, [OperandRegClass<0, eXe>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_mFifoExtra, [OperandRegClass<0, eXe>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_eBMLL, [OperandRegClass<0, eXe>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_eBMLH, [OperandRegClass<0, eXe>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_mStFifoh, [OperandRegClass<0, eXe>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXe_eXo, [OperandRegClass<0, eXe>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_mStFifol, [OperandRegClass<0, mStFifol>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_eLdFifoLReg, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMHH, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMHL, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_eLdFifoHReg, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_mFifoExtra, [OperandRegClass<0, mStFifol>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMLL, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_eBMLH, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_mStFifoh, [OperandRegClass<0, mStFifol>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifol_eXo, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_mStFifol, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMHH, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMHL, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eLdFifoHReg, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_mFifoExtra, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMLL, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eBMLH, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_mStFifoh, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoLReg_eXo, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_mStFifol, [OperandRegClass<0, eBMHH>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_eLdFifoLReg, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMHH, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMHL, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_eLdFifoHReg, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_mFifoExtra, [OperandRegClass<0, eBMHH>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMLL, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_eBMLH, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_mStFifoh, [OperandRegClass<0, eBMHH>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHH_eXo, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_mStFifol, [OperandRegClass<0, eBMHL>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_eLdFifoLReg, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMHH, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMHL, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_eLdFifoHReg, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_mFifoExtra, [OperandRegClass<0, eBMHL>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMLL, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_eBMLH, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_mStFifoh, [OperandRegClass<0, eBMHL>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMHL_eXo, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_mStFifol, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eLdFifoLReg, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMHH, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMHL, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_mFifoExtra, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMLL, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eBMLH, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_mStFifoh, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eLdFifoHReg_eXo, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_mStFifol, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eLdFifoLReg, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMHH, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMHL, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eLdFifoHReg, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_mFifoExtra, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMLL, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eBMLH, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_mStFifoh, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mFifoExtra_eXo, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_mStFifol, [OperandRegClass<0, eBMLL>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_eLdFifoLReg, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMHH, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMHL, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_eLdFifoHReg, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_mFifoExtra, [OperandRegClass<0, eBMLL>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMLL, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_eBMLH, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_mStFifoh, [OperandRegClass<0, eBMLL>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLL_eXo, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_mStFifol, [OperandRegClass<0, eBMLH>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_eLdFifoLReg, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMHH, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMHL, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_eLdFifoHReg, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_mFifoExtra, [OperandRegClass<0, eBMLH>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMLL, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_eBMLH, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_mStFifoh, [OperandRegClass<0, eBMLH>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eBMLH_eXo, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_mStFifol, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eLdFifoLReg, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMHH, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMHL, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eLdFifoHReg, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_mFifoExtra, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMLL, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eBMLH, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_mStFifoh, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_mStFifoh_eXo, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_mStFifol, [OperandRegClass<0, eXo>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_eLdFifoLReg, [OperandRegClass<0, eXo>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_eBMHH, [OperandRegClass<0, eXo>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_eBMHL, [OperandRegClass<0, eXo>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_eLdFifoHReg, [OperandRegClass<0, eXo>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_mFifoExtra, [OperandRegClass<0, eXo>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_eBMLL, [OperandRegClass<0, eXo>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_eBMLH, [OperandRegClass<0, eXo>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_mStFifoh, [OperandRegClass<0, eXo>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_mv_x_eXo_eXo, [OperandRegClass<0, eXo>, OperandRegClass<1, eXo>]>] in
def VMOV_alu_mv_mv_mv_x : AIE2PS_inst_mv_instr32 <(outs OP_mMvBMXDst:$dst), (ins OP_mMvBMXSrc:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_x
bits<6> dst;
bits<6> src;

let mv = {dst, src, 0b0000101111};
}

let Itinerary = II_VMOV_st_mv_mcd_bm, hasSideEffects = true, Uses = [crMCDEn] in
def VMOV_st_mv_mcd_bm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs:$src), "vmov", "mcd, $src">{
// id: me__instr128_or__st__st_streams__mv_mcd_bm__mMCD__mMcdBMSrc
bits<5> src;

let st = {dontcare{3-1}, 0b110, dontcare{3-1}, src, dontcare{2-1}, 0b10110};
}

let Itinerary = II_VMOV_st_mv_mcd_x, hasSideEffects = true, Uses = [crMCDEn] in
def VMOV_st_mv_mcd_x : AIE2PS_inst_st_instr32 <(outs), (ins OP_mXs:$src), "vmov", "mcd, $src">{
// id: me__instr128_or__st__st_streams__mv_mcd_x__mMCD__mMcdXSrc
bits<4> src;

let st = {dontcare{3-1}, 0b111, dontcare{3-1}, src, dontcare{3-1}, 0b10110};
}

let Itinerary = II_VMSC_vmac_vmul_cm_core_X_QX, Defs = [srSparse_of] in
def VMSC_vmac_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0110, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_vmul_cm_core_X_QY, Defs = [srSparse_of] in
def VMSC_vmac_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_vmul_cm_core_X_R in
def VMSC_vmac_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1000, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_vmul_cm_core_X_X in
def VMSC_vmac_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0000, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_vmul_cm_core_X_Y in
def VMSC_vmac_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_vmul_cm_core_Y_QX, Defs = [srSparse_of] in
def VMSC_vmac_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_vmul_cm_core_Y_R in
def VMSC_vmac_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_vmul_cm_core_Y_X in
def VMSC_vmac_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_vmul_cm_core_Y_Y in
def VMSC_vmac_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eY:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_half_vmul_cm_half_core_X_QX, Defs = [srSparse_of] in
def VMSC_vmac_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1010, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_half_vmul_cm_half_core_X_R in
def VMSC_vmac_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0111, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_vmac_half_vmul_cm_half_core_X_X in
def VMSC_vmac_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0011, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bf_vmul_bf_core_X_QY, DecoderMethod = "DecodeVMSC_f_vmac_bf_vmul_bf_core_X_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMSC_f_vmac_bf_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bf_vmul_bf_core_X_X, DecoderMethod = "DecodeVMSC_f_vmac_bf_vmul_bf_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMSC_f_vmac_bf_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0100, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bf_vmul_bf_core_Y_R, DecoderMethod = "DecodeVMSC_f_vmac_bf_vmul_bf_core_Y_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMSC_f_vmac_bf_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bf_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVMSC_f_vmac_bf_vmul_bf_core_Y_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMSC_f_vmac_bf_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eY:$s2, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bf_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVMSC_f_vmac_bf_half_vmul_bf_core_X_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMSC_f_vmac_bf_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1100, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bf_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVMSC_f_vmac_bf_half_vmul_bf_core_X_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMSC_f_vmac_bf_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bf_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVMSC_f_vmac_bf_half_vmul_bf_core_Y_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMSC_f_vmac_bf_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bf_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVMSC_f_vmac_bf_half_vmul_bf_half_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMSC_f_vmac_bf_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bf_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1011, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bfp13, DecoderMethod = "DecodeVMSC_f_vmac_bfp13", Defs = [srFPFlags], Uses = [crFPMask] in
def VMSC_f_vmac_bfp13 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;
bits<3> acc1;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, acc1, 0b001, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bfp16, DecoderMethod = "DecodeVMSC_f_vmac_bfp16", Defs = [srFPFlags], Uses = [crFPMask] in
def VMSC_f_vmac_bfp16 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vmsc.f", "$dst, $acc1, $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmac_bfp16__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;
bits<3> acc1;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, acc1, 0b001, 0b1};
}

let Itinerary = II_VMUL_vmul_vmul_cm_core_X_QX, Defs = [srSparse_of] in
def VMUL_vmul_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0110, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_vmul_cm_core_X_QY, Defs = [srSparse_of] in
def VMUL_vmul_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_vmul_cm_core_X_R in
def VMUL_vmul_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b1000, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_vmul_cm_core_X_X in
def VMUL_vmul_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0000, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_vmul_cm_core_X_Y in
def VMUL_vmul_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_vmul_cm_core_Y_QX, Defs = [srSparse_of] in
def VMUL_vmul_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_vmul_cm_core_Y_R in
def VMUL_vmul_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_vmul_cm_core_Y_X in
def VMUL_vmul_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_vmul_cm_core_Y_Y in
def VMUL_vmul_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eY:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_half_vmul_cm_half_core_X_QX, Defs = [srSparse_of] in
def VMUL_vmul_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b1010, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_half_vmul_cm_half_core_X_R in
def VMUL_vmul_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0111, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_vmul_half_vmul_cm_half_core_X_X in
def VMUL_vmul_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0011, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bf_vmul_bf_core_X_QY, DecoderMethod = "DecodeVMUL_f_vmul_bf_vmul_bf_core_X_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMUL_f_vmul_bf_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bf_vmul_bf_core_X_X, DecoderMethod = "DecodeVMUL_f_vmul_bf_vmul_bf_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMUL_f_vmul_bf_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0100, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bf_vmul_bf_core_Y_R, DecoderMethod = "DecodeVMUL_f_vmul_bf_vmul_bf_core_Y_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMUL_f_vmul_bf_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bf_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVMUL_f_vmul_bf_vmul_bf_core_Y_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMUL_f_vmul_bf_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eY:$s2, OP_mRv8:$acc), "vmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bf_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVMUL_f_vmul_bf_half_vmul_bf_core_X_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMUL_f_vmul_bf_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b1100, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bf_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVMUL_f_vmul_bf_half_vmul_bf_core_X_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMUL_f_vmul_bf_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bf_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVMUL_f_vmul_bf_half_vmul_bf_core_Y_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMUL_f_vmul_bf_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bf_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVMUL_f_vmul_bf_half_vmul_bf_half_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VMUL_f_vmul_bf_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b1011, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bfp13, DecoderMethod = "DecodeVMUL_f_vmul_bfp13", Defs = [srFPFlags], Uses = [crFPMask] in
def VMUL_f_vmul_bfp13 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bfp13__mDMv__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bfp16, DecoderMethod = "DecodeVMUL_f_vmul_bfp16", Defs = [srFPFlags], Uses = [crFPMask] in
def VMUL_f_vmul_bfp16 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vmul.f", "$dst, $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bfp16__mDMv__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, dontcare{3-1}, 0b000, 0b0};
}

let Itinerary = II_VNEGMUL_vmul_vmul_cm_core_X_QX, Defs = [srSparse_of] in
def VNEGMUL_vmul_vmul_cm_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0110, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_vmul_cm_core_X_QY, Defs = [srSparse_of] in
def VNEGMUL_vmul_vmul_cm_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_QY__mEYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b0, 0b0001, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_vmul_cm_core_X_R in
def VNEGMUL_vmul_vmul_cm_core_X_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_R__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b1000, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_vmul_cm_core_X_X in
def VNEGMUL_vmul_vmul_cm_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0000, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_vmul_cm_core_X_Y in
def VNEGMUL_vmul_vmul_cm_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b0, 0b1111, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_vmul_cm_core_Y_QX, Defs = [srSparse_of] in
def VNEGMUL_vmul_vmul_cm_core_Y_QX : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, s1, 0b1, s2, 0b0101, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_vmul_cm_core_Y_R in
def VNEGMUL_vmul_vmul_cm_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_R__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, s1, 0b0, s2, 0b0101, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_vmul_cm_core_Y_X in
def VNEGMUL_vmul_vmul_cm_core_Y_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, s1, 0b1, s2, 0b1101, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_vmul_cm_core_Y_Y in
def VNEGMUL_vmul_vmul_cm_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eY:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul__mDMv__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, s1, 0b0, s2, 0b0, 0b1001, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_half_vmul_cm_half_core_X_QX, Defs = [srSparse_of] in
def VNEGMUL_vmul_half_vmul_cm_half_core_X_QX : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mEXw:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_QX__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b1010, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_half_vmul_cm_half_core_X_R in
def VNEGMUL_vmul_half_vmul_cm_half_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_R__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0111, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_half_vmul_cm_half_core_X_X in
def VNEGMUL_vmul_half_vmul_cm_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul_half__mCMLv__vmul_cm_half_core__vmul_cm_half_core_X_X__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0011, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_vmul_bf_core_X_QY, DecoderMethod = "DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_X_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VNEGMUL_f_vmul_bf_vmul_bf_core_X_QY : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eEY_s:$s2, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_X_QY__mEYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b1, 0b0001, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_vmul_bf_core_X_X, DecoderMethod = "DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VNEGMUL_f_vmul_bf_vmul_bf_core_X_X : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b0100, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_vmul_bf_core_Y_R, DecoderMethod = "DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_Y_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VNEGMUL_f_vmul_bf_vmul_bf_core_Y_R : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_Y_R__mYv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, s1, 0b1, s2, 0b1001, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y, DecoderMethod = "DecodeVNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eY:$s2, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf__mDMv__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, s1, 0b0, s2, 0b1, 0b1001, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_R, DecoderMethod = "DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_core_X_R", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_R : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mRV16s:$s2, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_R__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b1100, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_Y, DecoderMethod = "DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_core_X_Y", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VNEGMUL_f_vmul_bf_half_vmul_bf_core_X_Y : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, eY:$s2, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_X_Y__mYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b0, 0b1110, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_half_vmul_bf_core_Y_QY, DecoderMethod = "DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_core_Y_QY", Defs = [srFPFlags, srSparse_of], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VNEGMUL_f_vmul_bf_half_vmul_bf_core_Y_QY : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eY:$s1, eEY_s:$s2, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_core_Y_QY__mYv__mEYw__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, s1, 0b0, s2, dontcare{1}, 0b1101, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_half_vmul_bf_half_core_X_X, DecoderMethod = "DecodeVNEGMUL_f_vmul_bf_half_vmul_bf_half_core_X_X", Defs = [srFPFlags], Uses = [crBF8conf, crFP8conf, crFPMask] in
def VNEGMUL_f_vmul_bf_half_vmul_bf_half_core_X_X : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bf_half__mCMLv__vmul_bf_half_core__vmul_bf_half_core_X_X__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, s1, s2, 0b1011, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bfp13, DecoderMethod = "DecodeVNEGMUL_f_vmul_bfp13", Defs = [srFPFlags], Uses = [crFPMask] in
def VNEGMUL_f_vmul_bfp13 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mFEXv:$s1, eFEY:$s2, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bfp13__mDMv__vmul_bfp13_core__vmul_bfp13_core_FEX_FEY__mFEYw__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, s1, s2, 0b1, 0b1110, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bfp16, DecoderMethod = "DecodeVNEGMUL_f_vmul_bfp16", Defs = [srFPFlags], Uses = [crFPMask] in
def VNEGMUL_f_vmul_bfp16 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mEWlv_bis:$s1l, eEYs:$s2l, eEYs:$s2h, OP_mEWhv:$s1h, OP_mRv8:$acc), "vnegmul.f", "$dst, $s1l, $s2l, $s2h, $acc">, AIE_HasTiedSubregister{
// id: me__instr128_or__vec__vmul_bfp16__mDMv__vmul_bfp16_core__vmul_bfp16_core_EX_EZ__mEYsw__vacc_bfp16_core
bits<3> acc;
bits<3> dst;
bits<4> s1l;
bits<2> s2l;
bits<2> s2h;

let vec = {acc, dst, s1l, s2l, s2h, 0b0010, dontcare{3-1}, 0b000, 0b1};
}

let Itinerary = II_VNEG_GTZ_16, Uses = [crBF8conf, crFP8conf] in
def VNEG_GTZ_16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXn:$s2), "vneg_gtz.16", " $d, r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vneg_gtz__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b01, 0b1, 0b00110101111};
}

let Itinerary = II_VNEG_GTZ_32, Uses = [crBF8conf, crFP8conf] in
def VNEG_GTZ_32 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXn:$s2), "vneg_gtz.32", " $d, r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vneg_gtz__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b00, 0b1, 0b00110101111};
}

let Itinerary = II_VNEG_GTZ_8, Uses = [crBF8conf, crFP8conf] in
def VNEG_GTZ_8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXn:$s2), "vneg_gtz.8", " $d, r17:r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vneg_gtz__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b10, 0b1, 0b00110101111};
}

let Itinerary = II_VNEG_vneg in
def VNEG_vneg : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mRv8:$acc), "vneg", "$dst, $acc1, $acc">{
// id: me__instr128_or__vec__vneg__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, dontcare{1}, 0b000, 0b10, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VNEG_vneg_half in
def VNEG_vneg_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mRv8:$acc), "vneg", "$dst, $acc1, $acc">{
// id: me__instr128_or__vec__vneg_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, dontcare{1}, 0b001, 0b10, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VNEG_f_vneg_fp, Defs = [srFPFlags], Uses = [crFPMask] in
def VNEG_f_vneg_fp : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mRv8:$acc), "vneg.f", "$dst, $acc1, $acc">{
// id: me__instr128_or__vec__vneg_fp__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, dontcare{1}, 0b010, 0b10, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VNEG_f_vneg_half_fp, Defs = [srFPFlags], Uses = [crFPMask] in
def VNEG_f_vneg_half_fp : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mRv8:$acc), "vneg.f", "$dst, $acc1, $acc">{
// id: me__instr128_or__vec__vneg_half_fp__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, dontcare{1}, 0b011, 0b10, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VNEG_mx6_vneg_fp_bfp13, Defs = [srFPFlags], Uses = [crFPMask] in
def VNEG_mx6_vneg_fp_bfp13 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mRv8:$acc), "vneg.mx6", "$dst, $acc1, $acc">{
// id: me__instr128_or__vec__vneg_fp_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, dontcare{1}, 0b100, 0b10, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VNEG_mx6_vneg_half_fp_bfp13, Defs = [srFPFlags], Uses = [crFPMask] in
def VNEG_mx6_vneg_half_fp_bfp13 : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, OP_mRv8:$acc), "vneg.mx6", "$dst, $acc1, $acc">{
// id: me__instr128_or__vec__vneg_half_fp_bfp13__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vacc_half_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, dontcare{1}, 0b101, 0b10, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VPACK_mv_pack_w_packSign0, Uses = [crPackSize, crSat, packSign0] in
def VPACK_mv_pack_w_packSign0 : AIE2PS_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mXs:$src), "vpack", "$dst, $src, packsign0">{
// id: me__instr128_or__st__mv_conv__mv_pack_w__packSign0
bits<5> dst;
bits<4> src;

let st = {dst, 0b0, 0b110, src, 0b0, 0b1, 0b010111};
}

let Itinerary = II_VPACK_mv_pack_w_packSign1, Uses = [crPackSize, crSat, packSign1] in
def VPACK_mv_pack_w_packSign1 : AIE2PS_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mXs:$src), "vpack", "$dst, $src, packsign1">{
// id: me__instr128_or__st__mv_conv__mv_pack_w__packSign1
bits<5> dst;
bits<4> src;

let st = {dst, 0b0, 0b110, src, 0b1, 0b1, 0b010111};
}

let Itinerary = II_VPACK_mv_pack_x_packSign0, Uses = [crPackSize, crSat, packSign0] in
def VPACK_mv_pack_x_packSign0 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins eY:$src), "vpack", "$dst, $src, packsign0">{
// id: me__instr128_or__st__mv_conv__mv_pack_x__mYs__packSign0
bits<4> dst;
bits<3> src;

let st = {dst, 0b0, 0b0, 0b111, src, dontcare{1}, 0b0, 0b1, 0b010111};
}

let Itinerary = II_VPACK_mv_pack_x_packSign1, Uses = [crPackSize, crSat, packSign1] in
def VPACK_mv_pack_x_packSign1 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins eY:$src), "vpack", "$dst, $src, packsign1">{
// id: me__instr128_or__st__mv_conv__mv_pack_x__mYs__packSign1
bits<4> dst;
bits<3> src;

let st = {dst, 0b0, 0b0, 0b111, src, dontcare{1}, 0b1, 0b1, 0b010111};
}

let Itinerary = II_VPUSH_hi_16 in
def VPUSH_hi_16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mRm:$s2), "vpush.hi.16", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_R__s2v_wide_16
bits<4> d;
bits<4> s1;
bits<5> s2;

let mv = {d, s1, s2, 0b01, 0b0100111};
}

let Itinerary = II_VPUSH_hi_32 in
def VPUSH_hi_32 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mRm:$s2), "vpush.hi.32", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_R__s2v_wide_32
bits<4> d;
bits<4> s1;
bits<5> s2;

let mv = {d, s1, s2, 0b10, 0b0100111};
}

let Itinerary = II_VPUSH_hi_64 in
def VPUSH_hi_64 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, eL:$s2), "vpush.hi.64", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_L__s2v_wide_64__mLm
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, dontcare{1}, 0b11, 0b0100111};
}

let Itinerary = II_VPUSH_hi_8 in
def VPUSH_hi_8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mRm:$s2), "vpush.hi.8", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_R__s2v_wide_8
bits<4> d;
bits<4> s1;
bits<5> s2;

let mv = {d, s1, s2, 0b00, 0b0100111};
}

let Itinerary = II_VPUSH_lo_16 in
def VPUSH_lo_16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mRm:$s1, OP_mXn:$s2), "vpush.lo.16", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_R__s2v_wide_16
bits<4> d;
bits<5> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b1100111};
}

let Itinerary = II_VPUSH_lo_32 in
def VPUSH_lo_32 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mRm:$s1, OP_mXn:$s2), "vpush.lo.32", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_R__s2v_wide_32
bits<4> d;
bits<5> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b1100111};
}

let Itinerary = II_VPUSH_lo_64 in
def VPUSH_lo_64 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins eL:$s1, OP_mXn:$s2), "vpush.lo.64", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_L__s2v_wide_64__mLm
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, dontcare{1}, s2, 0b11, 0b1100111};
}

let Itinerary = II_VPUSH_lo_8 in
def VPUSH_lo_8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mRm:$s1, OP_mXn:$s2), "vpush.lo.8", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_R__s2v_wide_8
bits<4> d;
bits<5> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b1100111};
}

let Itinerary = II_VSEL_16, Uses = [crBF8conf, crFP8conf] in
def VSEL_16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, OP_mRS16m:$sel), "vsel.16", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in32__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b01, 0b0, 0b001};
}

let Itinerary = II_VSEL_32, Uses = [crBF8conf, crFP8conf] in
def VSEL_32 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, OP_mRS16m:$sel), "vsel.32", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in16__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b00, 0b0, 0b001};
}

let Itinerary = II_VSEL_8, Uses = [crBF8conf, crFP8conf] in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VSEL_8_mLBypass, [OperandRegClass<3, eL>]>] in
// END MANUAL ADDITION
def VSEL_8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, eL:$sel), "vsel.8", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in64__mLm__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b10, 0b0, 0b001};
}

let Itinerary = II_VSHIFT_ALIGN in
def VSHIFT_ALIGN : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, eS:$pre, OP_mXn:$s2, mR30_shiftx:$shift), "vshift.align", "$d, $s1, $pre, $s2, r30">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shift_align__mSm__mR30_shiftx
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<2> pre;

let mv = {d, s1, s2, pre, 0b01001111};
}

let Itinerary = II_VSHIFT in
def VSHIFT : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, OP_mRm:$shift), "vshift", "$d, $s1, $s2, $shift">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shift
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<5> shift;

let mv = {d, s1, s2, shift, 0b10110};
}

let Itinerary = II_VSHUFFLE_vec_shuffle_bm in
def VSHUFFLE_vec_shuffle_bm : AIE2PS_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins OP_mXm:$s1, OP_mXn:$s2, OP_mRm:$mod), "vshuffle", "$dst, $s1, $s2, $mod">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_bm
bits<4> dst;
bits<4> s1;
bits<4> s2;
bits<5> mod;

let mv = {dst, s1, s2, mod, 0b01110};
}

let Itinerary = II_VSHUFFLE_vec_shuffle_egx in
def VSHUFFLE_vec_shuffle_egx : AIE2PS_inst_mv_instr32 <(outs OP_mEXm:$dst), (ins OP_mEXm:$s1, OP_mEXn:$s2, OP_mRm:$mod), "vshuffle", "$dst, $s1, $s2, $mod">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_egx
bits<4> dst;
bits<4> s1;
bits<4> s2;
bits<5> mod;

let mv = {dst, s1, s2, mod, 0b11110};
}

let Itinerary = II_VSHUFFLE_vec_shuffle_fex in
def VSHUFFLE_vec_shuffle_fex : AIE2PS_inst_mv_instr32 <(outs OP_mFEXm:$dst), (ins OP_mFEXm:$s1, OP_mFEXn:$s2, OP_mRm:$mod), "vshuffle", "$dst, $s1, $s2, $mod">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_fex
bits<4> dst;
bits<4> s1;
bits<4> s2;
bits<5> mod;

let mv = {dst, s1, s2, mod, 0b00011};
}

let Itinerary = II_VSHUFFLE_vec_shuffle_x in
def VSHUFFLE_vec_shuffle_x : AIE2PS_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, OP_mXn:$s2, OP_mRm:$mod), "vshuffle", "$dst, $s1, $s2, $mod">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_x
bits<4> dst;
bits<4> s1;
bits<4> s2;
bits<5> mod;

let mv = {dst, s1, s2, mod, 0b10011};
}

let Itinerary = II_VSRS_2x_mv_w_srs_bm_srsSign0, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VSRS_2x_mv_w_srs_bm_srsSign0 : AIE2PS_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mBMs:$src, eS:$base), "vsrs.2x", "$dst, $src, $base, srssign0">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_bm__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<5> dst;
bits<2> base;
bits<5> src;

let st = {dst, 0b0, base, 0b0, src, 0b0, 0b010111};
}

let Itinerary = II_VSRS_2x_mv_w_srs_bm_srsSign1, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VSRS_2x_mv_w_srs_bm_srsSign1 : AIE2PS_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mBMs:$src, eS:$base), "vsrs.2x", "$dst, $src, $base, srssign1">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_bm__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<5> dst;
bits<2> base;
bits<5> src;

let st = {dst, 0b0, base, 0b1, src, 0b0, 0b010111};
}

let Itinerary = II_VSRS_2x_mv_x_srs_cm_srsSign0, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VSRS_2x_mv_x_srs_cm_srsSign0 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mCMs:$src, eS:$base), "vsrs.2x", "$dst, $src, $base, srssign0">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_cm__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<4> dst;
bits<2> base;
bits<4> src;

let st = {dst, 0b0, 0b1, base, 0b0, src, 0b1, 0b0, 0b010111};
}

let Itinerary = II_VSRS_2x_mv_x_srs_cm_srsSign1, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VSRS_2x_mv_x_srs_cm_srsSign1 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mCMs:$src, eS:$base), "vsrs.2x", "$dst, $src, $base, srssign1">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_cm__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<4> dst;
bits<2> base;
bits<4> src;

let st = {dst, 0b0, 0b1, base, 0b1, src, 0b1, 0b0, 0b010111};
}

let Itinerary = II_VSRS_4x_mv_w_srs_cm_srsSign0, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VSRS_4x_mv_w_srs_cm_srsSign0 : AIE2PS_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mCMs:$src, eS:$base), "vsrs.4x", "$dst, $src, $base, srssign0">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_cm__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<5> dst;
bits<2> base;
bits<4> src;

let st = {dst, 0b1, base, 0b0, src, 0b0, 0b0, 0b010111};
}

let Itinerary = II_VSRS_4x_mv_w_srs_cm_srsSign1, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VSRS_4x_mv_w_srs_cm_srsSign1 : AIE2PS_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mCMs:$src, eS:$base), "vsrs.4x", "$dst, $src, $base, srssign1">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_cm__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<5> dst;
bits<2> base;
bits<4> src;

let st = {dst, 0b1, base, 0b1, src, 0b0, 0b0, 0b010111};
}

let Itinerary = II_VSRS_4x_mv_x_srs_dm_srsSign0, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VSRS_4x_mv_x_srs_dm_srsSign0 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins eDM:$src, eS:$base), "vsrs.4x", "$dst, $src, $base, srssign0">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_dm__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<4> dst;
bits<2> base;
bits<3> src;

let st = {dst, 0b1, 0b1, base, 0b0, src, dontcare{1}, 0b1, 0b0, 0b010111};
}

let Itinerary = II_VSRS_4x_mv_x_srs_dm_srsSign1, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VSRS_4x_mv_x_srs_dm_srsSign1 : AIE2PS_inst_st_instr32 <(outs OP_mXa:$dst), (ins eDM:$src, eS:$base), "vsrs.4x", "$dst, $src, $base, srssign1">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_dm__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<4> dst;
bits<2> base;
bits<3> src;

let st = {dst, 0b1, 0b1, base, 0b1, src, dontcare{1}, 0b1, 0b0, 0b010111};
}

let Itinerary = II_VST_128_dmv_sts_eg_spill, mayStore = true, Uses = [sp] in
def VST_128_dmv_sts_eg_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mEG2s:$src, c14n_step16:$imm), "vst.128", "$src, [sp, #$imm]">{
// id: me__instr128_or__st__dmv_sts_eg__spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
bits<9> imm;
bits<4> src;

let st = {imm, src, 0b00011001};
}

let Itinerary = II_VST_128_dmv_sts_eg_st_idx, mayStore = true in
def VST_128_dmv_sts_eg_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mEG2s:$src, eP:$ptr, eDJ:$dj), "vst.128", "$src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b10011001};
}

let Itinerary = II_VST_128_dmv_sts_eg_st_idx_imm, mayStore = true in
def VST_128_dmv_sts_eg_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mEG2s:$src, eP:$ptr, c8s_step16:$imm), "vst.128", "$src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b10011001};
}

let Itinerary = II_VST_128_dmv_sts_eg_st_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_128_dmv_sts_eg_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mEG2s:$src, eP:$ptr, eM:$mod), "vst.128", "$src, [$ptr], $mod">{
// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b10011001};
}

let Itinerary = II_VST_128_dmv_sts_eg_st_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_128_dmv_sts_eg_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mEG2s:$src, eP:$ptr, c8s_step16:$imm), "vst.128", "$src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b10011001};
}

let Itinerary = II_VST_128_dmv_sts_feg_spill, mayStore = true, Uses = [sp] in
def VST_128_dmv_sts_feg_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mFEGs:$src, c14n_step16:$imm), "vst.128", "$src, [sp, #$imm]">{
// id: me__instr128_or__st__dmv_sts_feg__spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
bits<9> imm;
bits<5> src;

let st = {imm, src, 0b0001000};
}

let Itinerary = II_VST_128_dmv_sts_feg_st_idx, mayStore = true in
def VST_128_dmv_sts_feg_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mFEGs:$src, eP:$ptr, eDJ:$dj), "vst.128", "$src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b1001000};
}

let Itinerary = II_VST_128_dmv_sts_feg_st_idx_imm, mayStore = true in
def VST_128_dmv_sts_feg_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mFEGs:$src, eP:$ptr, c8s_step16:$imm), "vst.128", "$src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b1001000};
}

let Itinerary = II_VST_128_dmv_sts_feg_st_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_128_dmv_sts_feg_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mFEGs:$src, eP:$ptr, eM:$mod), "vst.128", "$src, [$ptr], $mod">{
// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b1001000};
}

let Itinerary = II_VST_128_dmv_sts_feg_st_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_128_dmv_sts_feg_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mFEGs:$src, eP:$ptr, c8s_step16:$imm), "vst.128", "$src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b1001000};
}

let Itinerary = II_VST_128_dmv_sts_w_spill, mayStore = true, Uses = [sp] in
def VST_128_dmv_sts_w_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mWs_128b:$src, c14n_step16:$imm), "vst.128", "$src, [sp, #$imm]">{
// id: me__instr128_or__st__dmv_sts_w__spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
bits<9> imm;
bits<5> src;

let st = {imm, src, 0b0101000};
}

let Itinerary = II_VST_128_dmv_sts_w_st_idx, mayStore = true in
def VST_128_dmv_sts_w_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mWs_128b:$src, eP:$ptr, eDJ:$dj), "vst.128", "$src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b1101000};
}

let Itinerary = II_VST_128_dmv_sts_w_st_idx_imm, mayStore = true in
def VST_128_dmv_sts_w_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mWs_128b:$src, eP:$ptr, c8s_step16:$imm), "vst.128", "$src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b1101000};
}

let Itinerary = II_VST_128_dmv_sts_w_st_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_128_dmv_sts_w_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mWs_128b:$src, eP:$ptr, eM:$mod), "vst.128", "$src, [$ptr], $mod">{
// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b1101000};
}

let Itinerary = II_VST_128_dmv_sts_w_st_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_128_dmv_sts_w_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mWs_128b:$src, eP:$ptr, c8s_step16:$imm), "vst.128", "$src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b1101000};
}

let Itinerary = II_VST_2D_128_dmv_sts_eg, DecoderMethod = "DecodeVST_2D_128_dmv_sts_eg", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_128_dmv_sts_eg : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mEG2s:$src, eP:$ptr, eD:$mod), "vst.2d.128", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b10011001};
}

let Itinerary = II_VST_2D_128_dmv_sts_feg, DecoderMethod = "DecodeVST_2D_128_dmv_sts_feg", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_128_dmv_sts_feg : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mFEGs:$src, eP:$ptr, eD:$mod), "vst.2d.128", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b1001000};
}

let Itinerary = II_VST_2D_128_dmv_sts_w, DecoderMethod = "DecodeVST_2D_128_dmv_sts_w", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_128_dmv_sts_w : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mWs_128b:$src, eP:$ptr, eD:$mod), "vst.2d.128", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b1101000};
}

let Itinerary = II_VST_2D_CONV_bf16_fp32_dmw_sts_srs_bf16, DecoderMethod = "DecodeVST_2D_CONV_bf16_fp32_dmw_sts_srs_bf16", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_2D_CONV_bf16_fp32_dmw_sts_srs_bf16 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mBMs:$src, eP:$ptr, eD:$mod), "vst.2d.conv.bf16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs_f__mW
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b1011000};
}

let Itinerary = II_VST_2D_CONV_bf16_fp32_dmx_sts_srs_bf16, DecoderMethod = "DecodeVST_2D_CONV", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_2D_CONV_bf16_fp32_dmx_sts_srs_bf16 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eP:$ptr, eD:$mod), "vst.2d.conv.bf16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs_f__mX
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b11111001};
}

let Itinerary = II_VST_2D_CONV_bf8_fp32, DecoderMethod = "DecodeVST_2D_CONV_1", mayStore = true, Defs = [srF2FFlags], Uses = [crBF8conf, crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_2D_CONV_bf8_fp32 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$i, eP:$ptr, eD:$mod), "vst.2d.conv.bf8.fp32", " $src, $i, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs_f__mXl__mSs
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> i;

let st = {ptr, mod, 0b100, src, i, 0b000111};
}

let Itinerary = II_VST_2D_CONV_fp16_fp32, DecoderMethod = "DecodeVST_2D_CONV", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_2D_CONV_fp16_fp32 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eP:$ptr, eD:$mod), "vst.2d.conv.fp16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs_f__mX
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b00001010};
}

let Itinerary = II_VST_2D_CONV_fp8_fp32, DecoderMethod = "DecodeVST_2D_CONV_1", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFP8conf, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_2D_CONV_fp8_fp32 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$i, eP:$ptr, eD:$mod), "vst.2d.conv.fp8.fp32", " $src, $i, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs_f__mXl__mSs
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> i;

let st = {ptr, mod, 0b100, src, i, 0b100111};
}

let Itinerary = II_VST_2D_PACK_dmw_sts_pack_packSign0, DecoderMethod = "DecodeVST_2D_PACK_dmw_sts_pack_packS", mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_PACK_dmw_sts_pack_packSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mXs:$src, eP:$ptr, eD:$mod), "vst.2d.pack", "$src, packsign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b0, 0b0011000};
}

let Itinerary = II_VST_2D_PACK_dmw_sts_pack_packSign1, DecoderMethod = "DecodeVST_2D_PACK_dmw_sts_pack_packS", mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_PACK_dmw_sts_pack_packSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mXs:$src, eP:$ptr, eD:$mod), "vst.2d.pack", "$src, packsign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b1, 0b0011000};
}

let Itinerary = II_VST_2D_PACK_dmx_sts_pack_packSign0, DecoderMethod = "DecodeVST_2D_PACK_dmx_sts_pack_packS", mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_PACK_dmx_sts_pack_packSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eY:$src, eP:$ptr, eD:$mod), "vst.2d.pack", "$src, packsign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b100, src, dontcare{1}, 0b0, 0b0101001};
}

let Itinerary = II_VST_2D_PACK_dmx_sts_pack_packSign1, DecoderMethod = "DecodeVST_2D_PACK_dmx_sts_pack_packS", mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_PACK_dmx_sts_pack_packSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eY:$src, eP:$ptr, eD:$mod), "vst.2d.pack", "$src, packsign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b100, src, dontcare{1}, 0b1, 0b0101001};
}

let Itinerary = II_VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0, DecoderMethod = "DecodeVST_2D_SRS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eD:$mod), "vst.2d.srs.2x", "$src, $base, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> base;

let st = {ptr, mod, 0b100, src, 0b1, base, 0b0, 0b0000};
}

let Itinerary = II_VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1, DecoderMethod = "DecodeVST_2D_SRS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eD:$mod), "vst.2d.srs.2x", "$src, $base, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> base;

let st = {ptr, mod, 0b100, src, 0b1, base, 0b1, 0b0000};
}

let Itinerary = II_VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0, DecoderMethod = "DecodeVST_2D_SRS_2x_dmw_sts_srs_bm_srsS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mBMs:$src, eS:$base, eP:$ptr, eD:$mod), "vst.2d.srs.2x", "$src, $base, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<5> src;
bits<2> base;

let st = {ptr, mod, 0b100, src, base, 0b0, 0b0001};
}

let Itinerary = II_VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1, DecoderMethod = "DecodeVST_2D_SRS_2x_dmw_sts_srs_bm_srsS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mBMs:$src, eS:$base, eP:$ptr, eD:$mod), "vst.2d.srs.2x", "$src, $base, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<5> src;
bits<2> base;

let st = {ptr, mod, 0b100, src, base, 0b1, 0b0001};
}

let Itinerary = II_VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0, DecoderMethod = "DecodeVST_2D_SRS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eD:$mod), "vst.2d.srs.4x", "$src, $base, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> base;

let st = {ptr, mod, 0b100, src, 0b0, base, 0b0, 0b0000};
}

let Itinerary = II_VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1, DecoderMethod = "DecodeVST_2D_SRS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eD:$mod), "vst.2d.srs.4x", "$src, $base, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> base;

let st = {ptr, mod, 0b100, src, 0b0, base, 0b1, 0b0000};
}

let Itinerary = II_VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0, DecoderMethod = "DecodeVST_2D_SRS_4x_dmx_sts_srs_dm_srsS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eDM:$src, eS:$base, eP:$ptr, eD:$mod), "vst.2d.srs.4x", "$src, $base, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<3> src;
bits<2> base;

let st = {ptr, mod, 0b100, src, dontcare{2-1}, base, 0b0, 0b0010};
}

let Itinerary = II_VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1, DecoderMethod = "DecodeVST_2D_SRS_4x_dmx_sts_srs_dm_srsS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eDM:$src, eS:$base, eP:$ptr, eD:$mod), "vst.2d.srs.4x", "$src, $base, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<3> src;
bits<2> base;

let st = {ptr, mod, 0b100, src, dontcare{2-1}, base, 0b1, 0b0010};
}

let Itinerary = II_VST_2D_dmw_sts_feg2, DecoderMethod = "DecodeVST_2D_dmw_sts_feg2", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_dmw_sts_feg2 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mFEG2s:$src, eP:$ptr, eD:$mod), "vst.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b10111001};
}

let Itinerary = II_VST_2D_dmw_sts_w, DecoderMethod = "DecodeVST_2D_dmw_sts_w", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_dmw_sts_w : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mWs:$src, eP:$ptr, eD:$mod), "vst.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b1111000};
}

let Itinerary = II_VST_2D_dmx_sts_bm, DecoderMethod = "DecodeVST_2D_dmx_sts_bm", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_dmx_sts_bm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mBMs_v16w32:$src, eP:$ptr, eD:$mod), "vst.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b1001001};
}

let Itinerary = II_VST_2D_dmx_sts_x, DecoderMethod = "DecodeVST_2D_dmx_sts_x", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_dmx_sts_x : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mXs_split:$src, eP:$ptr, eD:$mod), "vst.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b01001010};
}

let Itinerary = II_VST_3D_128_dmv_sts_eg, DecoderMethod = "DecodeVST_3D_128_dmv_sts_eg", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_128_dmv_sts_eg : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mEG2s:$src, eP:$ptr, eDS:$mod), "vst.3d.128", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_eg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b10011001};
}

let Itinerary = II_VST_3D_128_dmv_sts_feg, DecoderMethod = "DecodeVST_3D_128_dmv_sts_feg", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_128_dmv_sts_feg : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mFEGs:$src, eP:$ptr, eDS:$mod), "vst.3d.128", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_feg__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1001000};
}

let Itinerary = II_VST_3D_128_dmv_sts_w, DecoderMethod = "DecodeVST_3D_128_dmv_sts_w", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_128_dmv_sts_w : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mWs_128b:$src, eP:$ptr, eDS:$mod), "vst.3d.128", "$src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_w__st__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1101000};
}

let Itinerary = II_VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf16, DecoderMethod = "DecodeVST_3D_CONV_bf16_fp32_dmw_sts_srs_bf16", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf16 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mBMs:$src, eP:$ptr, eDS:$mod), "vst.3d.conv.bf16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs_f__mW
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1011000};
}

let Itinerary = II_VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf16, DecoderMethod = "DecodeVST_3D_CONV", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf16 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eP:$ptr, eDS:$mod), "vst.3d.conv.bf16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs_f__mX
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b11111001};
}

let Itinerary = II_VST_3D_CONV_bf8_fp32, DecoderMethod = "DecodeVST_3D_CONV_1", mayStore = true, Defs = [srF2FFlags], Uses = [crBF8conf, crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_3D_CONV_bf8_fp32 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$i, eP:$ptr, eDS:$mod), "vst.3d.conv.bf8.fp32", " $src, $i, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs_f__mXl__mSs
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> i;

let st = {ptr, dontcare{1}, mod, 0b110, src, i, 0b000111};
}

let Itinerary = II_VST_3D_CONV_fp16_fp32, DecoderMethod = "DecodeVST_3D_CONV", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_3D_CONV_fp16_fp32 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eP:$ptr, eDS:$mod), "vst.3d.conv.fp16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs_f__mX
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b00001010};
}

let Itinerary = II_VST_3D_CONV_fp8_fp32, DecoderMethod = "DecodeVST_3D_CONV_1", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFP8conf, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_3D_CONV_fp8_fp32 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$i, eP:$ptr, eDS:$mod), "vst.3d.conv.fp8.fp32", " $src, $i, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs_f__mXl__mSs
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> i;

let st = {ptr, dontcare{1}, mod, 0b110, src, i, 0b100111};
}

let Itinerary = II_VST_3D_PACK_dmw_sts_pack_packSign0, DecoderMethod = "DecodeVST_3D_PACK_dmw_sts_pack_packS", mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_PACK_dmw_sts_pack_packSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mXs:$src, eP:$ptr, eDS:$mod), "vst.3d.pack", "$src, packsign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b0, 0b0011000};
}

let Itinerary = II_VST_3D_PACK_dmw_sts_pack_packSign1, DecoderMethod = "DecodeVST_3D_PACK_dmw_sts_pack_packS", mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_PACK_dmw_sts_pack_packSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mXs:$src, eP:$ptr, eDS:$mod), "vst.3d.pack", "$src, packsign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1, 0b0011000};
}

let Itinerary = II_VST_3D_PACK_dmx_sts_pack_packSign0, DecoderMethod = "DecodeVST_3D_PACK_dmx_sts_pack_packS", mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_PACK_dmx_sts_pack_packSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eY:$src, eP:$ptr, eDS:$mod), "vst.3d.pack", "$src, packsign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<2> mod;
bits<3> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, dontcare{1}, 0b0, 0b0101001};
}

let Itinerary = II_VST_3D_PACK_dmx_sts_pack_packSign1, DecoderMethod = "DecodeVST_3D_PACK_dmx_sts_pack_packS", mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_PACK_dmx_sts_pack_packSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eY:$src, eP:$ptr, eDS:$mod), "vst.3d.pack", "$src, packsign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<2> mod;
bits<3> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, dontcare{1}, 0b1, 0b0101001};
}

let Itinerary = II_VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0, DecoderMethod = "DecodeVST_3D_SRS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eDS:$mod), "vst.3d.srs.2x", "$src, $base, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> base;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1, base, 0b0, 0b0000};
}

let Itinerary = II_VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1, DecoderMethod = "DecodeVST_3D_SRS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eDS:$mod), "vst.3d.srs.2x", "$src, $base, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> base;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1, base, 0b1, 0b0000};
}

let Itinerary = II_VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0, DecoderMethod = "DecodeVST_3D_SRS_2x_dmw_sts_srs_bm_srsS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mBMs:$src, eS:$base, eP:$ptr, eDS:$mod), "vst.3d.srs.2x", "$src, $base, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<2> mod;
bits<5> src;
bits<2> base;

let st = {ptr, dontcare{1}, mod, 0b110, src, base, 0b0, 0b0001};
}

let Itinerary = II_VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1, DecoderMethod = "DecodeVST_3D_SRS_2x_dmw_sts_srs_bm_srsS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mBMs:$src, eS:$base, eP:$ptr, eDS:$mod), "vst.3d.srs.2x", "$src, $base, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<2> mod;
bits<5> src;
bits<2> base;

let st = {ptr, dontcare{1}, mod, 0b110, src, base, 0b1, 0b0001};
}

let Itinerary = II_VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0, DecoderMethod = "DecodeVST_3D_SRS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eDS:$mod), "vst.3d.srs.4x", "$src, $base, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> base;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b0, base, 0b0, 0b0000};
}

let Itinerary = II_VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1, DecoderMethod = "DecodeVST_3D_SRS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eDS:$mod), "vst.3d.srs.4x", "$src, $base, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> base;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b0, base, 0b1, 0b0000};
}

let Itinerary = II_VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0, DecoderMethod = "DecodeVST_3D_SRS_4x_dmx_sts_srs_dm_srsS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eDM:$src, eS:$base, eP:$ptr, eDS:$mod), "vst.3d.srs.4x", "$src, $base, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<2> mod;
bits<3> src;
bits<2> base;

let st = {ptr, dontcare{1}, mod, 0b110, src, dontcare{2-1}, base, 0b0, 0b0010};
}

let Itinerary = II_VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1, DecoderMethod = "DecodeVST_3D_SRS_4x_dmx_sts_srs_dm_srsS", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eDM:$src, eS:$base, eP:$ptr, eDS:$mod), "vst.3d.srs.4x", "$src, $base, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<2> mod;
bits<3> src;
bits<2> base;

let st = {ptr, dontcare{1}, mod, 0b110, src, dontcare{2-1}, base, 0b1, 0b0010};
}

let Itinerary = II_VST_3D_dmw_sts_feg2, DecoderMethod = "DecodeVST_3D_dmw_sts_feg2", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_dmw_sts_feg2 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mFEG2s:$src, eP:$ptr, eDS:$mod), "vst.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b10111001};
}

let Itinerary = II_VST_3D_dmw_sts_w, DecoderMethod = "DecodeVST_3D_dmw_sts_w", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_dmw_sts_w : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mWs:$src, eP:$ptr, eDS:$mod), "vst.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1111000};
}

let Itinerary = II_VST_3D_dmx_sts_bm, DecoderMethod = "DecodeVST_3D_dmx_sts_bm", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_dmx_sts_bm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mBMs_v16w32:$src, eP:$ptr, eDS:$mod), "vst.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1001001};
}

let Itinerary = II_VST_3D_dmx_sts_x, DecoderMethod = "DecodeVST_3D_dmx_sts_x", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_dmx_sts_x : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mXs_split:$src, eP:$ptr, eDS:$mod), "vst.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b01001010};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_idx, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd] in
def VST_CONV_bf16_fp32_dmw_sts_srs_bf16_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eP:$ptr, eDJ:$dj), "vst.conv.bf16.fp32", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs_f__mW
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b1011000};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_idx_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd] in
def VST_CONV_bf16_fp32_dmw_sts_srs_bf16_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eP:$ptr, c9s_step32:$imm), "vst.conv.bf16.fp32", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs_f__mW
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b1011000};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_pstm_nrm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf16_fp32_dmw_sts_srs_bf16_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eP:$ptr, eM:$mod), "vst.conv.bf16.fp32", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs_f__mW
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b1011000};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmw_sts_srs_bf16_pstm_nrm_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf16_fp32_dmw_sts_srs_bf16_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eP:$ptr, c9s_step32:$imm), "vst.conv.bf16.fp32", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_srs_bf16__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs_f__mW
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b1011000};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_idx, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd] in
def VST_CONV_bf16_fp32_dmx_sts_srs_bf16_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eP:$ptr, eDJ:$dj), "vst.conv.bf16.fp32", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs_f__mX
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b11111001};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_idx_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd] in
def VST_CONV_bf16_fp32_dmx_sts_srs_bf16_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eP:$ptr, c10s_step64:$imm), "vst.conv.bf16.fp32", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs_f__mX
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b11111001};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_pstm_nrm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf16_fp32_dmx_sts_srs_bf16_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eP:$ptr, eM:$mod), "vst.conv.bf16.fp32", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs_f__mX
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b11111001};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmx_sts_srs_bf16_pstm_nrm_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf16_fp32_dmx_sts_srs_bf16_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eP:$ptr, c10s_step64:$imm), "vst.conv.bf16.fp32", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_srs_bf16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs_f__mX
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b11111001};
}

let Itinerary = II_VST_CONV_bf8_fp32_idx, mayStore = true, Defs = [srF2FFlags], Uses = [crBF8conf, crF2FMask, crFPConvSat, crRnd] in
def VST_CONV_bf8_fp32_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$i, eP:$ptr, eDJ:$dj), "vst.conv.bf8.fp32", " $src, $i, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs_f__mXl__mSs
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> i;

let st = {ptr, dj, 0b000, src, i, 0b000111};
}

let Itinerary = II_VST_CONV_bf8_fp32_idx_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crBF8conf, crF2FMask, crFPConvSat, crRnd] in
def VST_CONV_bf8_fp32_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$i, eP:$ptr, c9s_step32:$imm), "vst.conv.bf8.fp32", " $src, $i, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs_f__mXl__mSs
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> i;

let st = {ptr, imm, 0b01, src, i, 0b000111};
}

let Itinerary = II_VST_CONV_bf8_fp32_pstm_nrm, mayStore = true, Defs = [srF2FFlags], Uses = [crBF8conf, crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf8_fp32_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$i, eP:$ptr, eM:$mod), "vst.conv.bf8.fp32", " $src, $i, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs_f__mXl__mSs
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> i;

let st = {ptr, mod, 0b010, src, i, 0b000111};
}

let Itinerary = II_VST_CONV_bf8_fp32_pstm_nrm_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crBF8conf, crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf8_fp32_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$i, eP:$ptr, c9s_step32:$imm), "vst.conv.bf8.fp32", " $src, $i, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_srs_bf8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs_f__mXl__mSs
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> i;

let st = {ptr, imm, 0b11, src, i, 0b000111};
}

let Itinerary = II_VST_CONV_fp16_fp32_idx, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd] in
def VST_CONV_fp16_fp32_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eP:$ptr, eDJ:$dj), "vst.conv.fp16.fp32", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs_f__mX
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b00001010};
}

let Itinerary = II_VST_CONV_fp16_fp32_idx_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd] in
def VST_CONV_fp16_fp32_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eP:$ptr, c10s_step64:$imm), "vst.conv.fp16.fp32", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs_f__mX
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b00001010};
}

let Itinerary = II_VST_CONV_fp16_fp32_pstm_nrm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_fp16_fp32_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eP:$ptr, eM:$mod), "vst.conv.fp16.fp32", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs_f__mX
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b00001010};
}

let Itinerary = II_VST_CONV_fp16_fp32_pstm_nrm_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_fp16_fp32_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eP:$ptr, c10s_step64:$imm), "vst.conv.fp16.fp32", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_srs_fp16__agus_dmx__fifo__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs_f__mX
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b00001010};
}

let Itinerary = II_VST_CONV_fp8_fp32_idx, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFP8conf, crFPConvSat, crRnd] in
def VST_CONV_fp8_fp32_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$i, eP:$ptr, eDJ:$dj), "vst.conv.fp8.fp32", " $src, $i, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs_f__mXl__mSs
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> i;

let st = {ptr, dj, 0b000, src, i, 0b100111};
}

let Itinerary = II_VST_CONV_fp8_fp32_idx_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFP8conf, crFPConvSat, crRnd] in
def VST_CONV_fp8_fp32_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$i, eP:$ptr, c9s_step32:$imm), "vst.conv.fp8.fp32", " $src, $i, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs_f__mXl__mSs
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> i;

let st = {ptr, imm, 0b01, src, i, 0b100111};
}

let Itinerary = II_VST_CONV_fp8_fp32_pstm_nrm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFP8conf, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_fp8_fp32_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$i, eP:$ptr, eM:$mod), "vst.conv.fp8.fp32", " $src, $i, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs_f__mXl__mSs
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> i;

let st = {ptr, mod, 0b010, src, i, 0b100111};
}

let Itinerary = II_VST_CONV_fp8_fp32_pstm_nrm_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crFP8conf, crFPConvSat, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_fp8_fp32_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$i, eP:$ptr, c9s_step32:$imm), "vst.conv.fp8.fp32", " $src, $i, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_srs_fp8__agus_dmw__fifo__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs_f__mXl__mSs
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> i;

let st = {ptr, imm, 0b11, src, i, 0b100111};
}

let Itinerary = II_VST_FLUSH_2D, DecoderMethod = "DecodeVST_FLUSH_2D", mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_2D : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out, eDC:$dc), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eD:$mod), "vst.flush.2d", "[p2, sf, r26, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__fifo_2d_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__fifo_2d__mPfs__mR26_fifo_st__mDs
bits<3> mod;

let st = {0b011, mod, 0b01, dontcare{1}, dontcare{3-1}, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_FLUSH_3D, DecoderMethod = "DecodeVST_FLUSH_3D", mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_3D : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out, eDCL:$dcl, eDCH:$dch), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eDS:$mod), "vst.flush.3d", "[p2, sf, r26, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__fifo_3d_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__fifo_3d__mPfs__mR26_fifo_st__mDSs
bits<2> mod;

let st = {0b011, dontcare{1}, mod, 0b10, dontcare{1}, dontcare{3-1}, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_FLUSH_fifo_1d_flush, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_fifo_1d_flush : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eM:$mod), "vst.flush", " [p2, sf, r26, $mod]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__fifo_1d_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__fifo_1d__mPfs__mR26_fifo_st__mMs
bits<3> mod;

let st = {0b011, mod, 0b11, dontcare{1}, dontcare{3-1}, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_FLUSH_normal_flush, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_normal_flush : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail), "vst.flush", " [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__normal_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__normal__mPfs__mR26_fifo_st

let st = {0b011, dontcare{1}, dontcare{2-1}, 0b00, dontcare{1}, dontcare{3-1}, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_FLUSH_d_2D, DecoderMethod = "DecodeVST_FLUSH_2D", mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_d_2D : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out, eDC:$dc), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eD:$mod), "vst.flush.d.2d", "[p2, sf, r26, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__fifo_2d_flush__dmx_sts_fifo_conv_ex_ebs16__core_flush__mStFifo__dmx_sts_fifo_conv_ex_ebs16__fifo_2d__mPfs__mR26_fifo_st__mDs
bits<3> mod;

let st = {0b111, mod, 0b01, dontcare{1}, dontcare{3-1}, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_FLUSH_d_3D, DecoderMethod = "DecodeVST_FLUSH_3D", mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_d_3D : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out, eDCL:$dcl, eDCH:$dch), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eDS:$mod), "vst.flush.d.3d", "[p2, sf, r26, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__fifo_3d_flush__dmx_sts_fifo_conv_ex_ebs16__core_flush__mStFifo__dmx_sts_fifo_conv_ex_ebs16__fifo_3d__mPfs__mR26_fifo_st__mDSs
bits<2> mod;

let st = {0b111, dontcare{1}, mod, 0b10, dontcare{1}, dontcare{3-1}, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_FLUSH_d_fifo_1d_flush, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_d_fifo_1d_flush : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eM:$mod), "vst.flush.d", " [p2, sf, r26, $mod]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__fifo_1d_flush__dmx_sts_fifo_conv_ex_ebs16__core_flush__mStFifo__dmx_sts_fifo_conv_ex_ebs16__fifo_1d__mPfs__mR26_fifo_st__mMs
bits<3> mod;

let st = {0b111, mod, 0b11, dontcare{1}, dontcare{3-1}, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_FLUSH_d_normal_flush, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_d_normal_flush : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail), "vst.flush.d", " [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__normal_flush__dmx_sts_fifo_conv_ex_ebs16__core_flush__mStFifo__dmx_sts_fifo_conv_ex_ebs16__normal__mPfs__mR26_fifo_st

let st = {0b111, dontcare{1}, dontcare{2-1}, 0b00, dontcare{1}, dontcare{3-1}, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_idx_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0] in
def VST_PACK_dmw_sts_pack_idx_packSign0 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, eDJ:$dj), "vst.pack", "$src, packsign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b0, 0b0011000};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_idx_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1] in
def VST_PACK_dmw_sts_pack_idx_packSign1 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, eDJ:$dj), "vst.pack", "$src, packsign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b1, 0b0011000};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_idx_imm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0] in
def VST_PACK_dmw_sts_pack_idx_imm_packSign0 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, c9s_step32:$imm), "vst.pack", "$src, packsign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b0, 0b0011000};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_idx_imm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1] in
def VST_PACK_dmw_sts_pack_idx_imm_packSign1 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, c9s_step32:$imm), "vst.pack", "$src, packsign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b1, 0b0011000};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_pstm_nrm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmw_sts_pack_pstm_nrm_packSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, eM:$mod), "vst.pack", "$src, packsign0, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b0, 0b0011000};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_pstm_nrm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmw_sts_pack_pstm_nrm_packSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, eM:$mod), "vst.pack", "$src, packsign1, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b1, 0b0011000};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, c9s_step32:$imm), "vst.pack", "$src, packsign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b0, 0b0011000};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, c9s_step32:$imm), "vst.pack", "$src, packsign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b1, 0b0011000};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_idx_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0] in
def VST_PACK_dmx_sts_pack_idx_packSign0 : AIE2PS_inst_st_instr32 <(outs), (ins eY:$src, eP:$ptr, eDJ:$dj), "vst.pack", "$src, packsign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<3> dj;
bits<3> src;

let st = {ptr, dj, 0b000, src, dontcare{1}, 0b0, 0b0101001};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_idx_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1] in
def VST_PACK_dmx_sts_pack_idx_packSign1 : AIE2PS_inst_st_instr32 <(outs), (ins eY:$src, eP:$ptr, eDJ:$dj), "vst.pack", "$src, packsign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<3> dj;
bits<3> src;

let st = {ptr, dj, 0b000, src, dontcare{1}, 0b1, 0b0101001};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_idx_imm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0] in
def VST_PACK_dmx_sts_pack_idx_imm_packSign0 : AIE2PS_inst_st_instr32 <(outs), (ins eY:$src, eP:$ptr, c10s_step64:$imm), "vst.pack", "$src, packsign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b01, src, dontcare{1}, 0b0, 0b0101001};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_idx_imm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1] in
def VST_PACK_dmx_sts_pack_idx_imm_packSign1 : AIE2PS_inst_st_instr32 <(outs), (ins eY:$src, eP:$ptr, c10s_step64:$imm), "vst.pack", "$src, packsign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b01, src, dontcare{1}, 0b1, 0b0101001};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_pstm_nrm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmx_sts_pack_pstm_nrm_packSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eY:$src, eP:$ptr, eM:$mod), "vst.pack", "$src, packsign0, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b010, src, dontcare{1}, 0b0, 0b0101001};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_pstm_nrm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmx_sts_pack_pstm_nrm_packSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eY:$src, eP:$ptr, eM:$mod), "vst.pack", "$src, packsign1, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b010, src, dontcare{1}, 0b1, 0b0101001};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eY:$src, eP:$ptr, c10s_step64:$imm), "vst.pack", "$src, packsign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b11, src, dontcare{1}, 0b0, 0b0101001};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eY:$src, eP:$ptr, c10s_step64:$imm), "vst.pack", "$src, packsign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b11, src, dontcare{1}, 0b1, 0b0101001};
}

let Itinerary = II_VST_PUSH_256_CONV_mx4_fp32, mayStore = true, Defs = [srF2BFlags, srFifo_of], Uses = [crF2BMask, crRnd], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_256_CONV_mx4_fp32 : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, eDM:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.256.conv.mx4.fp32", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_few_bfp11__normal_push__dmx_sts_fifo_conv_few_bfp11__core_push__bfp__mFp2Bfp11__bfp__basis__mDMs__mStFifo__dmx_sts_fifo_conv_few_bfp11__normal__mPfs__mR26_fifo_st
bits<3> src;

let st = {0b101, dontcare{1}, 0b10, dontcare{2-1}, dontcare{1}, src, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_PUSH_256, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_256 : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mFEWs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.256", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_few_bfp11__normal_push__dmx_sts_fifo_bare_few_bfp11__core_push__mStFifo__dmx_sts_fifo_bare_few_bfp11__normal__mPfs__mR26_fifo_st
bits<5> src;

let st = {0b101, dontcare{1}, 0b11, dontcare{2-1}, src, 0b01111001};
}

let Itinerary = II_VST_PUSH_384_CONV_mx6_fp32, mayStore = true, Defs = [srF2BFlags, srFifo_of], Uses = [crF2BMask, crRnd], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_384_CONV_mx6_fp32 : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, eDM:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.384.conv.mx6.fp32", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_few__normal_push__dmx_sts_fifo_conv_few__core_push__bfp__mFp2Bfp13__bfp__basis__mDMs__mStFifo__dmx_sts_fifo_conv_few__normal__mPfs__mR26_fifo_st
bits<3> src;

let st = {0b101, dontcare{1}, 0b00, dontcare{2-1}, dontcare{1}, src, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_PUSH_384, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_384 : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mFEWs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.384", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_few__normal_push__dmx_sts_fifo_bare_few__core_push__mStFifo__dmx_sts_fifo_bare_few__normal__mPfs__mR26_fifo_st
bits<5> src;

let st = {0b101, dontcare{1}, 0b01, dontcare{2-1}, src, 0b01111001};
}

let Itinerary = II_VST_PUSH_512_dmx_sts_fifo_bare_fex_bfp11, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_512_dmx_sts_fifo_bare_fex_bfp11 : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mFEXs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.512", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_fex_bfp11__normal_push__dmx_sts_fifo_bare_fex_bfp11__core_push__mStFifo__dmx_sts_fifo_bare_fex_bfp11__normal__mPfs__mR26_fifo_st
bits<4> src;

let st = {0b110, dontcare{1}, dontcare{2-1}, dontcare{2-1}, dontcare{1}, src, 0b01111001};
}

let Itinerary = II_VST_PUSH_512_dmx_sts_fifo_bare_x, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_512_dmx_sts_fifo_bare_x : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mXs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.512", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_x__normal_push__dmx_sts_fifo_bare_x__core_push__mStFifo__dmx_sts_fifo_bare_x__normal__mPfs__mR26_fifo_st
bits<4> src;

let st = {0b000, dontcare{1}, dontcare{2-1}, dontcare{2-1}, dontcare{1}, src, 0b01111001};
}

let Itinerary = II_VST_PUSH_576_CONV_mx9_fp32, mayStore = true, Defs = [srF2BFlags, srFifo_of], Uses = [crF2BMask, crRnd], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_576_CONV_mx9_fp32 : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, eDM:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.576.conv.mx9.fp32", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_conv_ex_ebs16__normal_push__dmx_sts_fifo_conv_ex_ebs16__core_push__bfp__mFp2Bfp16__bfp__basis__mDMs__mStFifo__dmx_sts_fifo_conv_ex_ebs16__normal__mPfs__mR26_fifo_st
bits<3> src;

let st = {0b001, dontcare{1}, dontcare{2-1}, dontcare{2-1}, dontcare{1}, src, dontcare{1}, 0b01111001};
}

let Itinerary = II_VST_PUSH_576, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_576 : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mEXs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.576", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_ex__normal_push__dmx_sts_fifo_bare_ex__core_push__mStFifo__dmx_sts_fifo_bare_ex__normal__mPfs__mR26_fifo_st
bits<4> src;

let st = {0b100, dontcare{1}, dontcare{2-1}, dontcare{2-1}, dontcare{1}, src, 0b01111001};
}

let Itinerary = II_VST_PUSH_768, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_768 : AIE2PS_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mFEXs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.768", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo___dmx_sts_fifo_bare_fex__normal_push__dmx_sts_fifo_bare_fex__core_push__mStFifo__dmx_sts_fifo_bare_fex__normal__mPfs__mR26_fifo_st
bits<4> src;

let st = {0b010, dontcare{1}, dontcare{2-1}, dontcare{2-1}, dontcare{1}, src, 0b01111001};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eDJ:$dj), "vst.srs.2x", "$src, $base, srssign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> base;

let st = {ptr, dj, 0b000, src, 0b1, base, 0b0, 0b0000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_idx_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_2x_dm_sts_srs_cm_idx_srsSign1 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eDJ:$dj), "vst.srs.2x", "$src, $base, srssign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> base;

let st = {ptr, dj, 0b000, src, 0b1, base, 0b1, 0b0000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign0 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$base, eP:$ptr, c10s_step64:$imm), "vst.srs.2x", "$src, $base, srssign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> base;

let st = {ptr, imm, 0b01, src, 0b1, base, 0b0, 0b0000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign1 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$base, eP:$ptr, c10s_step64:$imm), "vst.srs.2x", "$src, $base, srssign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> base;

let st = {ptr, imm, 0b01, src, 0b1, base, 0b1, 0b0000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eM:$mod), "vst.srs.2x", "$src, $base, srssign0, [$ptr], $mod">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> base;

let st = {ptr, mod, 0b010, src, 0b1, base, 0b0, 0b0000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eM:$mod), "vst.srs.2x", "$src, $base, srssign1, [$ptr], $mod">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> base;

let st = {ptr, mod, 0b010, src, 0b1, base, 0b1, 0b0000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$base, eP:$ptr, c10s_step64:$imm), "vst.srs.2x", "$src, $base, srssign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> base;

let st = {ptr, imm, 0b11, src, 0b1, base, 0b0, 0b0000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$base, eP:$ptr, c10s_step64:$imm), "vst.srs.2x", "$src, $base, srssign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> base;

let st = {ptr, imm, 0b11, src, 0b1, base, 0b1, 0b0000};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign0 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eS:$base, eP:$ptr, eDJ:$dj), "vst.srs.2x", "$src, $base, srssign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> dj;
bits<5> src;
bits<2> base;

let st = {ptr, dj, 0b000, src, base, 0b0, 0b0001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign1 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eS:$base, eP:$ptr, eDJ:$dj), "vst.srs.2x", "$src, $base, srssign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> dj;
bits<5> src;
bits<2> base;

let st = {ptr, dj, 0b000, src, base, 0b1, 0b0001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign0 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eS:$base, eP:$ptr, c9s_step32:$imm), "vst.srs.2x", "$src, $base, srssign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<5> src;
bits<2> base;

let st = {ptr, imm, 0b01, src, base, 0b0, 0b0001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign1 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eS:$base, eP:$ptr, c9s_step32:$imm), "vst.srs.2x", "$src, $base, srssign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<5> src;
bits<2> base;

let st = {ptr, imm, 0b01, src, base, 0b1, 0b0001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eS:$base, eP:$ptr, eM:$mod), "vst.srs.2x", "$src, $base, srssign0, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<5> src;
bits<2> base;

let st = {ptr, mod, 0b010, src, base, 0b0, 0b0001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eS:$base, eP:$ptr, eM:$mod), "vst.srs.2x", "$src, $base, srssign1, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<5> src;
bits<2> base;

let st = {ptr, mod, 0b010, src, base, 0b1, 0b0001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eS:$base, eP:$ptr, c9s_step32:$imm), "vst.srs.2x", "$src, $base, srssign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<5> src;
bits<2> base;

let st = {ptr, imm, 0b11, src, base, 0b0, 0b0001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eS:$base, eP:$ptr, c9s_step32:$imm), "vst.srs.2x", "$src, $base, srssign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<5> src;
bits<2> base;

let st = {ptr, imm, 0b11, src, base, 0b1, 0b0001};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_idx_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_4x_dm_sts_srs_cm_idx_srsSign0 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eDJ:$dj), "vst.srs.4x", "$src, $base, srssign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> base;

let st = {ptr, dj, 0b000, src, 0b0, base, 0b0, 0b0000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_idx_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_4x_dm_sts_srs_cm_idx_srsSign1 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eDJ:$dj), "vst.srs.4x", "$src, $base, srssign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> base;

let st = {ptr, dj, 0b000, src, 0b0, base, 0b1, 0b0000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign0 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$base, eP:$ptr, c9s_step32:$imm), "vst.srs.4x", "$src, $base, srssign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> base;

let st = {ptr, imm, 0b01, src, 0b0, base, 0b0, 0b0000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign1 : AIE2PS_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$base, eP:$ptr, c9s_step32:$imm), "vst.srs.4x", "$src, $base, srssign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> base;

let st = {ptr, imm, 0b01, src, 0b0, base, 0b1, 0b0000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eM:$mod), "vst.srs.4x", "$src, $base, srssign0, [$ptr], $mod">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> base;

let st = {ptr, mod, 0b010, src, 0b0, base, 0b0, 0b0000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$base, eP:$ptr, eM:$mod), "vst.srs.4x", "$src, $base, srssign1, [$ptr], $mod">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> base;

let st = {ptr, mod, 0b010, src, 0b0, base, 0b1, 0b0000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$base, eP:$ptr, c9s_step32:$imm), "vst.srs.4x", "$src, $base, srssign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> base;

let st = {ptr, imm, 0b11, src, 0b0, base, 0b0, 0b0000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$base, eP:$ptr, c9s_step32:$imm), "vst.srs.4x", "$src, $base, srssign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> base;

let st = {ptr, imm, 0b11, src, 0b0, base, 0b1, 0b0000};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign0 : AIE2PS_inst_st_instr32 <(outs), (ins eDM:$src, eS:$base, eP:$ptr, eDJ:$dj), "vst.srs.4x", "$src, $base, srssign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> dj;
bits<3> src;
bits<2> base;

let st = {ptr, dj, 0b000, src, dontcare{2-1}, base, 0b0, 0b0010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign1 : AIE2PS_inst_st_instr32 <(outs), (ins eDM:$src, eS:$base, eP:$ptr, eDJ:$dj), "vst.srs.4x", "$src, $base, srssign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> dj;
bits<3> src;
bits<2> base;

let st = {ptr, dj, 0b000, src, dontcare{2-1}, base, 0b1, 0b0010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign0 : AIE2PS_inst_st_instr32 <(outs), (ins eDM:$src, eS:$base, eP:$ptr, c10s_step64:$imm), "vst.srs.4x", "$src, $base, srssign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<3> src;
bits<2> base;

let st = {ptr, imm, 0b01, src, dontcare{2-1}, base, 0b0, 0b0010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign1 : AIE2PS_inst_st_instr32 <(outs), (ins eDM:$src, eS:$base, eP:$ptr, c10s_step64:$imm), "vst.srs.4x", "$src, $base, srssign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<3> src;
bits<2> base;

let st = {ptr, imm, 0b01, src, dontcare{2-1}, base, 0b1, 0b0010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eDM:$src, eS:$base, eP:$ptr, eM:$mod), "vst.srs.4x", "$src, $base, srssign0, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<3> src;
bits<2> base;

let st = {ptr, mod, 0b010, src, dontcare{2-1}, base, 0b0, 0b0010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eDM:$src, eS:$base, eP:$ptr, eM:$mod), "vst.srs.4x", "$src, $base, srssign1, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<3> src;
bits<2> base;

let st = {ptr, mod, 0b010, src, dontcare{2-1}, base, 0b1, 0b0010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign0 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eDM:$src, eS:$base, eP:$ptr, c10s_step64:$imm), "vst.srs.4x", "$src, $base, srssign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<3> src;
bits<2> base;

let st = {ptr, imm, 0b11, src, dontcare{2-1}, base, 0b0, 0b0010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign1 : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins eDM:$src, eS:$base, eP:$ptr, c10s_step64:$imm), "vst.srs.4x", "$src, $base, srssign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<3> src;
bits<2> base;

let st = {ptr, imm, 0b11, src, dontcare{2-1}, base, 0b1, 0b0010};
}

let Itinerary = II_VST_dmw_sts_feg2_spill, mayStore = true, Uses = [sp] in
def VST_dmw_sts_feg2_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mFEG2s:$src, c15n_step32:$imm), "vst", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmw_sts_feg2__spill__agus_dmw__nrm_spill__agus_dmw__spill__mSPs__m_c15n_step32
bits<9> imm;
bits<4> src;

let st = {imm, src, 0b00111001};
}

let Itinerary = II_VST_dmw_sts_feg2_st_idx, mayStore = true in
def VST_dmw_sts_feg2_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mFEG2s:$src, eP:$ptr, eDJ:$dj), "vst", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b10111001};
}

let Itinerary = II_VST_dmw_sts_feg2_st_idx_imm, mayStore = true in
def VST_dmw_sts_feg2_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mFEG2s:$src, eP:$ptr, c9s_step32:$imm), "vst", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b10111001};
}

let Itinerary = II_VST_dmw_sts_feg2_st_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmw_sts_feg2_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mFEG2s:$src, eP:$ptr, eM:$mod), "vst", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b10111001};
}

let Itinerary = II_VST_dmw_sts_feg2_st_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmw_sts_feg2_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mFEG2s:$src, eP:$ptr, c9s_step32:$imm), "vst", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_feg2__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b10111001};
}

let Itinerary = II_VST_dmw_sts_w_spill, mayStore = true, Uses = [sp] in
def VST_dmw_sts_w_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mWs:$src, c15n_step32:$imm), "vst", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmw_sts_w__spill__agus_dmw__nrm_spill__agus_dmw__spill__mSPs__m_c15n_step32
bits<9> imm;
bits<5> src;

let st = {imm, src, 0b0111000};
}

let Itinerary = II_VST_dmw_sts_w_st_idx, mayStore = true in
def VST_dmw_sts_w_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mWs:$src, eP:$ptr, eDJ:$dj), "vst", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b1111000};
}

let Itinerary = II_VST_dmw_sts_w_st_idx_imm, mayStore = true in
def VST_dmw_sts_w_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mWs:$src, eP:$ptr, c9s_step32:$imm), "vst", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b1111000};
}

let Itinerary = II_VST_dmw_sts_w_st_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmw_sts_w_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mWs:$src, eP:$ptr, eM:$mod), "vst", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b1111000};
}

let Itinerary = II_VST_dmw_sts_w_st_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmw_sts_w_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mWs:$src, eP:$ptr, c9s_step32:$imm), "vst", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_w__st__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b1111000};
}

let Itinerary = II_VST_dmx_sts_bm_spill, mayStore = true, Uses = [sp] in
def VST_dmx_sts_bm_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs_v16w32:$src, c16n_step64:$imm), "vst", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmx_sts_bm__spill__agus_dmx__nrm_spill__agus_dmx__spill__mSPs__m_c16n_step64
bits<9> imm;
bits<5> src;

let st = {imm, src, 0b0001001};
}

let Itinerary = II_VST_dmx_sts_bm_st_idx, mayStore = true in
def VST_dmx_sts_bm_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs_v16w32:$src, eP:$ptr, eDJ:$dj), "vst", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b1001001};
}

let Itinerary = II_VST_dmx_sts_bm_st_idx_imm, mayStore = true in
def VST_dmx_sts_bm_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mBMs_v16w32:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b1001001};
}

let Itinerary = II_VST_dmx_sts_bm_st_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_bm_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs_v16w32:$src, eP:$ptr, eM:$mod), "vst", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b1001001};
}

let Itinerary = II_VST_dmx_sts_bm_st_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_bm_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs_v16w32:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_bm__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b1001001};
}

let Itinerary = II_VST_dmx_sts_x_spill, mayStore = true, Uses = [sp] in
def VST_dmx_sts_x_spill : AIE2PS_inst_st_instr32 <(outs), (ins OP_mXs_split:$src, c16n_step64:$imm), "vst", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmx_sts_x__spill__agus_dmx__nrm_spill__agus_dmx__spill__mSPs__m_c16n_step64
bits<9> imm;
bits<4> src;

let st = {imm, src, 0b10001010};
}

let Itinerary = II_VST_dmx_sts_x_st_idx, mayStore = true in
def VST_dmx_sts_x_st_idx : AIE2PS_inst_st_instr32 <(outs), (ins OP_mXs_split:$src, eP:$ptr, eDJ:$dj), "vst", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b01001010};
}

let Itinerary = II_VST_dmx_sts_x_st_idx_imm, mayStore = true in
def VST_dmx_sts_x_st_idx_imm : AIE2PS_inst_st_instr32 <(outs), (ins OP_mXs_split:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b01001010};
}

let Itinerary = II_VST_dmx_sts_x_st_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_x_st_pstm_nrm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs_split:$src, eP:$ptr, eM:$mod), "vst", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b01001010};
}

let Itinerary = II_VST_dmx_sts_x_st_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_x_st_pstm_nrm_imm : AIE2PS_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs_split:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_x__st__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b01001010};
}

let Itinerary = II_VSUB_16, Uses = [crBF8conf, crFP8conf] in
def VSUB_16 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub.16", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vsub__vec_size__vec_size32__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b1, 0b01, 0b1111011};
}

let Itinerary = II_VSUB_32, Uses = [crBF8conf, crFP8conf] in
def VSUB_32 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub.32", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vsub__vec_size__vec_size16__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b1, 0b10, 0b1111011};
}

let Itinerary = II_VSUB_8, Uses = [crBF8conf, crFP8conf] in
def VSUB_8 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub.8", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vsub__vec_size__vec_size64__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b1, 0b00, 0b1111011};
}

let Itinerary = II_VSUB_GE_16_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VSUB_GE_16_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b010, 0b0, 0b0010};
}

let Itinerary = II_VSUB_GE_16_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VSUB_GE_16_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b010, 0b1, 0b0010};
}

let Itinerary = II_VSUB_GE_32_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VSUB_GE_32_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b010, 0b0, 0b0010};
}

let Itinerary = II_VSUB_GE_32_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VSUB_GE_32_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b010, 0b1, 0b0010};
}

let Itinerary = II_VSUB_GE_8_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VSUB_GE_8_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b010, 0b0, 0b0010};
}

let Itinerary = II_VSUB_GE_8_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VSUB_GE_8_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b010, 0b1, 0b0010};
}

let Itinerary = II_VSUB_LT_16_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VSUB_LT_16_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b001, 0b0, 0b0010};
}

let Itinerary = II_VSUB_LT_16_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VSUB_LT_16_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b001, 0b1, 0b0010};
}

let Itinerary = II_VSUB_LT_32_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VSUB_LT_32_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b001, 0b0, 0b0010};
}

let Itinerary = II_VSUB_LT_32_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VSUB_LT_32_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mR16m:$r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b001, 0b1, 0b0010};
}

let Itinerary = II_VSUB_LT_8_vaddSign0, Uses = [crBF8conf, crFP8conf, vaddSign0] in
def VSUB_LT_8_vaddSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b001, 0b0, 0b0010};
}

let Itinerary = II_VSUB_LT_8_vaddSign1, Uses = [crBF8conf, crFP8conf, vaddSign1] in
def VSUB_LT_8_vaddSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$r17_r16), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b001, 0b1, 0b0010};
}

let Itinerary = II_VSUB_vacc in
def VSUB_vacc : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mRv8:$acc), "vsub", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b000, 0b000, acc2, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_vacc_half in
def VSUB_vacc_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mRv8:$acc), "vsub", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b001, 0b000, acc2, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_vacc_scd_add_scd_incr, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VSUB_vacc_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b000, 0b100, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_vacc_scd_add_scd_noincr, hasSideEffects = true, Uses = [crSCDEn] in
def VSUB_vacc_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_cm_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b000, 0b011, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_vacc_scd_half_add_scd_incr_half, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$acc2_out = $acc2" in
def VSUB_vacc_scd_half_add_scd_incr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b001, 0b100, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_vacc_scd_half_add_scd_noincr_half, hasSideEffects = true, Uses = [crSCDEn] in
def VSUB_vacc_scd_half_add_scd_noincr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_scd_half__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_cm_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b001, 0b011, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_f_vacc_fp, Defs = [srFPFlags], Uses = [crFPMask] in
def VSUB_f_vacc_fp : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mRv8:$acc), "vsub.f", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_fp__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_reg_d__vmac_acc2_comp1__add_reg__mDMm__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b010, 0b000, acc2, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_f_vacc_fp_scd_add_scd_incr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VSUB_f_vacc_fp_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub.f", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_fp_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_incr__vmac_scd_dm_dynd__vmac_scd_dm_comp1__vmac_scd_dm_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b010, 0b100, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_f_vacc_fp_scd_add_scd_noincr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VSUB_f_vacc_fp_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub.f", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_fp_scd__mDMv__vmac_acc1_add_d__vmac_acc1_comp1__add__mDMv__vmac_acc2_add_scd_d__vmac_acc2_comp1__add_scd__vmac_acc2_comp1__add_scd_noincr__vmac_scd_dm_regd__vmac_scd_dm_comp1__vmac_scd_dm_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_bf_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b010, 0b011, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_f_vacc_half_fp, Defs = [srFPFlags], Uses = [crFPMask] in
def VSUB_f_vacc_half_fp : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mRv8:$acc), "vsub.f", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_half_fp__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_reg_half_d__vmac_acc2_comp1__add_reg_half__mCMLm__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b011, 0b000, acc2, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_f_vacc_half_fp_scd_add_scd_incr_half, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VSUB_f_vacc_half_fp_scd_add_scd_incr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub.f", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_half_fp_scd__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_incr_half__vmac_scd_cml_dynd__vmac_scd_dm_comp1__vmac_scd_cml_dyn__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r_incr__mR31_scd__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b011, 0b100, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_f_vacc_half_fp_scd_add_scd_noincr_half, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VSUB_f_vacc_half_fp_scd_add_scd_noincr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub.f", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_half_fp_scd__mCMLv__vmac_acc1_add_half_d__vmac_acc1_comp1__add_half__mCMLv__vmac_acc2_add_scd_half_d__vmac_acc2_comp1__add_scd_half__vmac_acc2_comp1__add_scd_noincr_half__vmac_scd_cml_regd__vmac_scd_dm_comp1__vmac_scd_cml_reg__vmac_scd_dm_comp1__vmac_scd_dm_core__mSCD_E4__mSCD_comp_E4__mSCD__mSCD_r__mR31_scd__vacc_bf_half_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b011, 0b011, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_mx6_vacc_fp_bfp13, Defs = [srFPFlags], Uses = [crFPMask] in
def VSUB_mx6_vacc_fp_bfp13 : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mRv8:$acc), "vsub.mx6", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_fp_bfp13__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_reg__vmac_acc2_comp0__add_reg__mDMm__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b100, 0b000, acc2, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_mx6_vacc_fp_bfp13_scd_add_scd_incr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VSUB_mx6_vacc_fp_bfp13_scd_add_scd_incr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$acc2_out), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub.mx6", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_fp_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b100, 0b100, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_mx6_vacc_fp_bfp13_scd_add_scd_noincr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VSUB_mx6_vacc_fp_bfp13_scd_add_scd_noincr : AIE2PS_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub.mx6", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_fp_bfp13_scd__mDMv__vmac_acc1_add__vmac_acc1_comp0__add__mDMv__vmac_acc2_add_scd__vmac_acc2_comp0__add_scd__vmac_acc2_comp0__add_scd_noincr__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vacc_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b100, 0b011, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_mx6_vacc_half_fp_bfp13, Defs = [srFPFlags], Uses = [crFPMask] in
def VSUB_mx6_vacc_half_fp_bfp13 : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, eCML:$acc2, OP_mRv8:$acc), "vsub.mx6", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_half_fp_bfp13__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_reg_half__vmac_acc2_comp0__add_reg_half__mCMLm__vacc_half_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc2;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b101, 0b000, acc2, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_mx6_vacc_half_fp_bfp13_scd_add_scd_incr_half, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$acc2_out = $acc2" in
def VSUB_mx6_vacc_half_fp_bfp13_scd_add_scd_incr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst, mR31_scd:$acc2_out), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub.mx6", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_half_fp_bfp13_scd__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_scd_half__vmac_acc2_comp0__add_scd_half__vmac_acc2_comp0__add_scd_incr_half__vmac_scd_cml_dyn__vmac_scd_dm_comp0__vmac_scd_cml_dyn__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r_incr__mR31_scd__vacc_half_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b101, 0b100, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VSUB_mx6_vacc_half_fp_bfp13_scd_add_scd_noincr_half, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VSUB_mx6_vacc_half_fp_bfp13_scd_add_scd_noincr_half : AIE2PS_inst_vec_instr32 <(outs eCML:$dst), (ins eCML:$acc1, mR31_scd:$acc2, OP_mRv8:$acc), "vsub.mx6", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_half_fp_bfp13_scd__mCMLv__vmac_acc1_add_half__vmac_acc1_comp0__add_half__mCMLv__vmac_acc2_add_scd_half__vmac_acc2_comp0__add_scd_half__vmac_acc2_comp0__add_scd_noincr_half__vmac_scd_cml_reg__vmac_scd_dm_comp0__vmac_scd_cml_reg__vmac_scd_dm_comp0__vmac_scd_dm_core__mSCD_E3__mSCD_comp_E3__mSCD__mSCD_r__mR31_scd__vacc_half_bfp13_core
bits<3> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, dontcare{3-1}, 0b101, 0b011, dontcare{3-1}, acc1, 0b101, 0b1};
}

let Itinerary = II_VTANH in
def VTANH : AIE2PS_inst_mv_instr32 <(outs OP_mWm:$dst), (ins OP_mBMm:$src), "vtanh", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf__vtanh
bits<5> dst;
bits<5> src;

let mv = {dst, src, 0b1, 0b10010101111};
}

let Itinerary = II_VUNPACK_mv_unpack_w_unpackSign0, Uses = [crUnpackSize, unpackSign0] in
def VUNPACK_mv_unpack_w_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins OP_mWs:$src), "vunpack", "$dst, $src, unpacksign0">{
// id: me__instr128_or__ldb__ldb_or1__mv_unpack_w__unpackSign0
bits<5> src;
bits<4> dst;

let ldb = {src, dontcare{1}, 0b01, 0b0, dst, 0b0, 0b100};
}

let Itinerary = II_VUNPACK_mv_unpack_w_unpackSign1, Uses = [crUnpackSize, unpackSign1] in
def VUNPACK_mv_unpack_w_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins OP_mWs:$src), "vunpack", "$dst, $src, unpacksign1">{
// id: me__instr128_or__ldb__ldb_or1__mv_unpack_w__unpackSign1
bits<5> src;
bits<4> dst;

let ldb = {src, dontcare{1}, 0b01, 0b0, dst, 0b1, 0b100};
}

let Itinerary = II_VUNPACK_mv_unpack_x_unpackSign0, Uses = [crUnpackSize, unpackSign0] in
def VUNPACK_mv_unpack_x_unpackSign0 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst), (ins OP_mXs:$src), "vunpack", "$dst, $src, unpacksign0">{
// id: me__instr128_or__ldb__ldb_or1__mv_unpack_x__mYb__unpackSign0
bits<4> src;
bits<3> dst;

let ldb = {src, dontcare{1}, dontcare{1}, 0b11, 0b0, dst, dontcare{1}, 0b0, 0b100};
}

let Itinerary = II_VUNPACK_mv_unpack_x_unpackSign1, Uses = [crUnpackSize, unpackSign1] in
def VUNPACK_mv_unpack_x_unpackSign1 : AIE2PS_inst_ldb_instr32 <(outs eY:$dst), (ins OP_mXs:$src), "vunpack", "$dst, $src, unpacksign1">{
// id: me__instr128_or__ldb__ldb_or1__mv_unpack_x__mYb__unpackSign1
bits<4> src;
bits<3> dst;

let ldb = {src, dontcare{1}, dontcare{1}, 0b11, 0b0, dst, dontcare{1}, 0b1, 0b100};
}

let Itinerary = II_VUPS_2x_mv_ups_w2b_upsSign0, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VUPS_2x_mv_ups_w2b_upsSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mBMm:$dst), (ins OP_mWm:$src, eS:$base), "vups.2x", "$dst, $src, $base, upssign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2b__ups_mov__w2b__ups_mov__base__mSm__upsSign0
bits<5> dst;
bits<5> src;
bits<2> base;

let mv = {dst, dontcare{1}, src, base, 0b0, 0b01010111};
}

let Itinerary = II_VUPS_2x_mv_ups_w2b_upsSign1, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VUPS_2x_mv_ups_w2b_upsSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mBMm:$dst), (ins OP_mWm:$src, eS:$base), "vups.2x", "$dst, $src, $base, upssign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2b__ups_mov__w2b__ups_mov__base__mSm__upsSign1
bits<5> dst;
bits<5> src;
bits<2> base;

let mv = {dst, dontcare{1}, src, base, 0b1, 0b01010111};
}

let Itinerary = II_VUPS_2x_mv_ups_x2c_upsSign0, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VUPS_2x_mv_ups_x2c_upsSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mXm:$src, eS:$base), "vups.2x", "$dst, $src, $base, upssign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2c__ups_mov__x2c__ups_mov__base__mSm__upsSign0
bits<4> dst;
bits<4> src;
bits<2> base;

let mv = {dst, dontcare{2-1}, src, dontcare{1}, base, 0b0, 0b00110111};
}

let Itinerary = II_VUPS_2x_mv_ups_x2c_upsSign1, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VUPS_2x_mv_ups_x2c_upsSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mXm:$src, eS:$base), "vups.2x", "$dst, $src, $base, upssign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2c__ups_mov__x2c__ups_mov__base__mSm__upsSign1
bits<4> dst;
bits<4> src;
bits<2> base;

let mv = {dst, dontcare{2-1}, src, dontcare{1}, base, 0b1, 0b00110111};
}

let Itinerary = II_VUPS_4x_mv_ups_w2c_upsSign0, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VUPS_4x_mv_ups_w2c_upsSign0 : AIE2PS_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mWm:$src, eS:$base), "vups.4x", "$dst, $src, $base, upssign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2c__ups_mov__w2c__ups_mov__base__mSm__upsSign0
bits<4> dst;
bits<5> src;
bits<2> base;

let mv = {dst, dontcare{2-1}, src, base, 0b0, 0b11010111};
}

let Itinerary = II_VUPS_4x_mv_ups_w2c_upsSign1, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VUPS_4x_mv_ups_w2c_upsSign1 : AIE2PS_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mWm:$src, eS:$base), "vups.4x", "$dst, $src, $base, upssign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2c__ups_mov__w2c__ups_mov__base__mSm__upsSign1
bits<4> dst;
bits<5> src;
bits<2> base;

let mv = {dst, dontcare{2-1}, src, base, 0b1, 0b11010111};
}

let Itinerary = II_VUPS_4x_mv_ups_x2d_upsSign0, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VUPS_4x_mv_ups_x2d_upsSign0 : AIE2PS_inst_mv_instr32 <(outs eDM:$dst), (ins OP_mXm:$src, eS:$base), "vups.4x", "$dst, $src, $base, upssign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2d__ups_mov__x2d__mDMm__ups_mov__base__mSm__upsSign0
bits<3> dst;
bits<4> src;
bits<2> base;

let mv = {dst, dontcare{3-1}, src, dontcare{1}, base, 0b0, 0b10110111};
}

let Itinerary = II_VUPS_4x_mv_ups_x2d_upsSign1, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VUPS_4x_mv_ups_x2d_upsSign1 : AIE2PS_inst_mv_instr32 <(outs eDM:$dst), (ins OP_mXm:$src, eS:$base), "vups.4x", "$dst, $src, $base, upssign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2d__ups_mov__x2d__mDMm__ups_mov__base__mSm__upsSign1
bits<3> dst;
bits<4> src;
bits<2> base;

let mv = {dst, dontcare{3-1}, src, dontcare{1}, base, 0b1, 0b10110111};
}

let Itinerary = II_XOR in
def XOR : AIE2PS_inst_alu_instr32 <(outs OP_mRx:$d0), (ins OP_mRx:$s0, OP_mRy:$s1), "xor", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__xor
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0110, 0b1};
}

}
