//===-AIE2PGenInstrInfo.td -----------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//* Automatically generated file, do not edit! *

let hasSideEffects = false, mayLoad = false, mayStore = false in {
let Itinerary = II_ABS, Defs = [srCarry] in
def ABS : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "abs", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__abs__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b1000, 0b10, 0b0000};
}

let Itinerary = II_ACQ_COND_mLockId_imm, hasSideEffects = true in
def ACQ_COND_mLockId_imm : AIE2P_inst_alu_instr32 <(outs), (ins c6u:$id, eR:$s1, mR26_lock:$s2), "acq.cond", "#$id, $s1, r26">{
// id: me__instr128_or__alu_mv__alumv_or__alu__acquire_cond__mLockId__mLockId_imm__mRy__mR26_lock
bits<6> id;
bits<5> s1;

let alu = {id, 0b0, dontcare{1}, 0b11, s1, 0b1, 0b0000};
}

let Itinerary = II_ACQ_COND_mLockId_reg, hasSideEffects = true in
def ACQ_COND_mLockId_reg : AIE2P_inst_alu_instr32 <(outs), (ins eR:$id, eR:$s1, mR26_lock:$s2), "acq.cond", "$id, $s1, r26">{
// id: me__instr128_or__alu_mv__alumv_or__alu__acquire_cond__mLockId__mLockId_reg__mRx__mRy__mR26_lock
bits<5> id;
bits<5> s1;

let alu = {id, dontcare{1}, 0b1, dontcare{1}, 0b11, s1, 0b1, 0b0000};
}

let Itinerary = II_ACQ_mLockId_imm, hasSideEffects = true in
def ACQ_mLockId_imm : AIE2P_inst_alu_instr32 <(outs), (ins c6u:$id, eR:$s1), "acq", "#$id, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__acquire__mLockId__mLockId_imm__mRy
bits<6> id;
bits<5> s1;

let alu = {id, 0b0, dontcare{1}, 0b01, s1, 0b1, 0b0000};
}

let Itinerary = II_ACQ_mLockId_reg, hasSideEffects = true in
def ACQ_mLockId_reg : AIE2P_inst_alu_instr32 <(outs), (ins eR:$id, eR:$s1), "acq", "$id, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__acquire__mLockId__mLockId_reg__mRx__mRy
bits<5> id;
bits<5> s1;

let alu = {id, dontcare{1}, 0b1, dontcare{1}, 0b01, s1, 0b1, 0b0000};
}

let Itinerary = II_ADC, Defs = [srCarry], Uses = [srCarry] in
def ADC : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "adc", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__addx__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0010, 0b1};
}

let Itinerary = II_ADD_NC_mv_add_ri, ItineraryRegPairs = [
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRF2IFlags, [OperandRegClass<0, mSRF2IFlags>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRFPNlfMask, [OperandRegClass<0, mCRFPNlfMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mLEm, [OperandRegClass<0, mLEm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRCarry, [OperandRegClass<0, mSRCarry>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mLRm, [OperandRegClass<0, mLRm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRPackSize, [OperandRegClass<0, mCRPackSize>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRRnd, [OperandRegClass<0, mCRRnd>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRFifo_of, [OperandRegClass<0, mSRFifo_of>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRF2FMask, [OperandRegClass<0, mCRF2FMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRF2FFlags, [OperandRegClass<0, mSRF2FFlags>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRF2IMask, [OperandRegClass<0, mCRF2IMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRF2BMask, [OperandRegClass<0, mCRF2BMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eVaddSign, [OperandRegClass<0, eVaddSign>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eS, [OperandRegClass<0, eS>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRFifo_uf, [OperandRegClass<0, mSRFifo_uf>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRMCDEn, [OperandRegClass<0, mCRMCDEn>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRMS0, [OperandRegClass<0, mSRMS0>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRUPS_of, [OperandRegClass<0, mSRUPS_of>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRSparse_of, [OperandRegClass<0, mSRSparse_of>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRSRSMode, [OperandRegClass<0, mCRSRSMode>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mLCm, [OperandRegClass<0, mLCm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRSCDEn, [OperandRegClass<0, mCRSCDEn>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mLSm, [OperandRegClass<0, mLSm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRSat, [OperandRegClass<0, mCRSat>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRFPMask, [OperandRegClass<0, mCRFPMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eUnpackSign, [OperandRegClass<0, eUnpackSign>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRSRS_of, [OperandRegClass<0, mSRSRS_of>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRFPFlags, [OperandRegClass<0, mSRFPFlags>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRF2BFlags, [OperandRegClass<0, mSRF2BFlags>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRUPSMode, [OperandRegClass<0, mCRUPSMode>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRFPNlf, [OperandRegClass<0, mSRFPNlf>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mCRUnpackSize, [OperandRegClass<0, mCRUnpackSize>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eUPSSign, [OperandRegClass<0, eUPSSign>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_ePackSign, [OperandRegClass<0, ePackSign>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSPm, [OperandRegClass<0, mSPm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_mSRSS0, [OperandRegClass<0, mSRSS0>]>,
ItinRegClassPair<II_ADD_NC_mv_add_ri_eSRSSign, [OperandRegClass<0, eSRSSign>]>] in
def ADD_NC_mv_add_ri : AIE2P_inst_mv_instr32 <(outs OP_mMvSclDst:$dst), (ins eR:$s0, c8s:$imm), "add.nc", "$dst, $s0, #$imm">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_add_ri__mRm
bits<7> dst;
bits<5> s0;
bits<8> imm;

let mv = {dst, s0, imm, 0b00};
}

let Itinerary = II_ADD_NC_mv_add_rr, ItineraryRegPairs = [
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRF2IFlags, [OperandRegClass<0, mSRF2IFlags>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRFPNlfMask, [OperandRegClass<0, mCRFPNlfMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mLEm, [OperandRegClass<0, mLEm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRCarry, [OperandRegClass<0, mSRCarry>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mLRm, [OperandRegClass<0, mLRm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRPackSize, [OperandRegClass<0, mCRPackSize>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRRnd, [OperandRegClass<0, mCRRnd>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRFifo_of, [OperandRegClass<0, mSRFifo_of>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRF2FMask, [OperandRegClass<0, mCRF2FMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRF2FFlags, [OperandRegClass<0, mSRF2FFlags>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRF2IMask, [OperandRegClass<0, mCRF2IMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRF2BMask, [OperandRegClass<0, mCRF2BMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eVaddSign, [OperandRegClass<0, eVaddSign>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eS, [OperandRegClass<0, eS>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRFifo_uf, [OperandRegClass<0, mSRFifo_uf>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRMCDEn, [OperandRegClass<0, mCRMCDEn>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRMS0, [OperandRegClass<0, mSRMS0>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRUPS_of, [OperandRegClass<0, mSRUPS_of>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRSparse_of, [OperandRegClass<0, mSRSparse_of>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRSRSMode, [OperandRegClass<0, mCRSRSMode>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mLCm, [OperandRegClass<0, mLCm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRSCDEn, [OperandRegClass<0, mCRSCDEn>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mLSm, [OperandRegClass<0, mLSm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRSat, [OperandRegClass<0, mCRSat>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRFPMask, [OperandRegClass<0, mCRFPMask>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eUnpackSign, [OperandRegClass<0, eUnpackSign>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRSRS_of, [OperandRegClass<0, mSRSRS_of>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRFPFlags, [OperandRegClass<0, mSRFPFlags>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRF2BFlags, [OperandRegClass<0, mSRF2BFlags>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRUPSMode, [OperandRegClass<0, mCRUPSMode>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRFPNlf, [OperandRegClass<0, mSRFPNlf>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mCRUnpackSize, [OperandRegClass<0, mCRUnpackSize>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eUPSSign, [OperandRegClass<0, eUPSSign>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_ePackSign, [OperandRegClass<0, ePackSign>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSPm, [OperandRegClass<0, mSPm>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_mSRSS0, [OperandRegClass<0, mSRSS0>]>,
ItinRegClassPair<II_ADD_NC_mv_add_rr_eSRSSign, [OperandRegClass<0, eSRSSign>]>] in
def ADD_NC_mv_add_rr : AIE2P_inst_mv_instr32 <(outs OP_mMvSclDst:$dst), (ins eR:$s0, eR:$s1), "add.nc", "$dst, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_add_rr__mRm__mRs
bits<7> dst;
bits<5> s0;
bits<5> s1;

let mv = {dst, s0, s1, 0b01010};
}

let Itinerary = II_ADD_add_r_ri, Defs = [srCarry] in
def ADD_add_r_ri : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, c7s:$imm), "add", "$d0, $s0, #$imm">{
// id: me__instr128_or__alu_mv__alumv_or__alu__add_r_ri__mRx
bits<5> s0;
bits<5> d0;
bits<7> imm;

let alu = {s0, d0, imm, 0b110};
}

let Itinerary = II_ADD_alu_r_rr, Defs = [srCarry] in
def ADD_alu_r_rr : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "add", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__add__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0000, 0b1};
}

let Itinerary = II_AND in
def AND : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "and", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__and__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0100, 0b1};
}

let Itinerary = II_ASHL in
def ASHL : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "ashl", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sgn__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1110, 0b1};
}

let Itinerary = II_CLB in
def CLB : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "clb", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__clb__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0000, 0b10, 0b0000};
}

let Itinerary = II_CLZ in
def CLZ : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "clz", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__clbu__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0001, 0b10, 0b0000};
}

let Itinerary = II_DIVS, Constraints = "$sd_out = $sd" in
def DIVS : AIE2P_inst_alu_instr32 <(outs eR:$d0, mR31_divs:$sd_out), (ins mR31_divs:$sd, eR:$s0, eR:$s1), "divs", "$d0, r31, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_dstep__mRx__mR31_divs__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1, 0b1000};
}

let Itinerary = II_DONE, hasSideEffects = true in
def DONE : AIE2P_inst_alu_instr32 <(outs), (ins), "done", "">{
// id: me__instr128_or__alu_mv__alumv_or__alu__done

let alu = {dontcare{5-1}, 0b001, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EQZ in
def EQZ : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "eqz", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__seq0__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0110, 0b10, 0b0000};
}

let Itinerary = II_EQ in
def EQ : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "eq", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__seq__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0111, 0b1};
}

let Itinerary = II_EVENT_ERROR, hasSideEffects = 1 in
def EVENT_ERROR : AIE2P_inst_alu_instr32 <(outs), (ins), "event.error", "">{
// id: me__instr128_or__alu_mv__alumv_or__alu__event__event3

let alu = {0b11, dontcare{3-1}, 0b010, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EVENT_WARNING, hasSideEffects = 1 in
def EVENT_WARNING : AIE2P_inst_alu_instr32 <(outs), (ins), "event.warning", "">{
// id: me__instr128_or__alu_mv__alumv_or__alu__event__event2

let alu = {0b10, dontcare{3-1}, 0b010, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EVENT_event0, hasSideEffects = 1 in
def EVENT_event0 : AIE2P_inst_alu_instr32 <(outs), (ins), "event", "#0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__event__event0

let alu = {0b00, dontcare{3-1}, 0b010, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EVENT_event1, hasSideEffects = 1 in
def EVENT_event1 : AIE2P_inst_alu_instr32 <(outs), (ins), "event", "#1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__event__event1

let alu = {0b01, dontcare{3-1}, 0b010, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_EXTEND_s16 in
def EXTEND_s16 : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "extend.s16", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__se16__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0011, 0b10, 0b0000};
}

let Itinerary = II_EXTEND_s8 in
def EXTEND_s8 : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "extend.s8", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__se8__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0010, 0b10, 0b0000};
}

let Itinerary = II_EXTEND_u16 in
def EXTEND_u16 : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "extend.u16", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__ze16__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0101, 0b10, 0b0000};
}

let Itinerary = II_EXTEND_u8 in
def EXTEND_u8 : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "extend.u8", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__ze8__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0100, 0b10, 0b0000};
}

let Itinerary = II_GEU in
def GEU : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "geu", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sgeu__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1011, 0b1};
}

let Itinerary = II_GE in
def GE : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "ge", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sge__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1001, 0b1};
}

let Itinerary = II_INVSQRT_mOptConvDel_mRx, Defs = [srFPCnvFl2Fx, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPNlfMask] in
def INVSQRT_mOptConvDel_mRx : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins mS3:$m, eR:$s0), "invsqrt", " ${d0}.flt2fx, s3, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mRa__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b01, 0b1, 0b0, 0b00, 0b1000};
}

let Itinerary = II_INVSQRT_mOptConvDel_mRx_mOptConv, Defs = [srFPCnvFl2Fx, srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPCnvFx2FlMask, crFPNlfMask] in
def INVSQRT_mOptConvDel_mRx_mOptConv : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins mS3:$m, eR:$s0, mS2:$m_1), "invsqrt", " ${d0}.flt2fx, s3, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mRa__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mRx__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b01, 0b1, 0b1, 0b00, 0b1000};
}

let Itinerary = II_INVSQRT_mRx, Defs = [srFPNlf], Uses = [crFPNlfMask] in
def INVSQRT_mRx : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "invsqrt", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mRa__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b01, 0b0, 0b0, 0b00, 0b1000};
}

let Itinerary = II_INVSQRT_mRx_mOptConv, Defs = [srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFx2FlMask, crFPNlfMask] in
def INVSQRT_mRx_mOptConv : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, mS2:$m), "invsqrt", " $d0, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_invsqrt__mRa__mRx__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b01, 0b0, 0b1, 0b00, 0b1000};
}

let Itinerary = II_INV_mOptConvDel_mRx, Defs = [srFPCnvFl2Fx, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPNlfMask] in
def INV_mOptConvDel_mRx : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins mS3:$m, eR:$s0), "inv", " ${d0}.flt2fx, s3, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mRa__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b10, 0b1, 0b0, 0b00, 0b1000};
}

let Itinerary = II_INV_mOptConvDel_mRx_mOptConv, Defs = [srFPCnvFl2Fx, srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPCnvFx2FlMask, crFPNlfMask] in
def INV_mOptConvDel_mRx_mOptConv : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins mS3:$m, eR:$s0, mS2:$m_1), "inv", " ${d0}.flt2fx, s3, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mRa__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mRx__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b10, 0b1, 0b1, 0b00, 0b1000};
}

let Itinerary = II_INV_mRx, Defs = [srFPNlf], Uses = [crFPNlfMask] in
def INV_mRx : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "inv", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mRa__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b10, 0b0, 0b0, 0b00, 0b1000};
}

let Itinerary = II_INV_mRx_mOptConv, Defs = [srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFx2FlMask, crFPNlfMask] in
def INV_mRx_mOptConv : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, mS2:$m), "inv", " $d0, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_inv__mRa__mRx__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b10, 0b0, 0b1, 0b00, 0b1000};
}

let Itinerary = II_JL_alumv_or, hasDelaySlot = true, Defs = [lr] in
def JL_alumv_or : AIE2P_inst_alu_instr32 <(outs), (ins eP:$a), "jl", "$a">{
// id: me__instr128_or__alu_mv__alumv_or__alu__jump_link_ind__mPm__mLRx
bits<3> a;

let alu = {dontcare{5-1}, 0b110, dontcare{2-1}, a, 0b000, 0b0000};
}

let Itinerary = II_JL_lng, hasDelaySlot = true, Defs = [lr] in
def JL_lng : AIE2P_inst_lng_instr48 <(outs), (ins cpmaddr:$i), "jl", "#$i">{
// id: me__instr128_or__alu_mv__lng__jump_link_imm__mLRx
bits<20> i;

let lng = {dontcare{5-1}, i, dontcare{1}, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b100};
}

let Itinerary = II_JNZD, hasDelaySlot = true, Defs = [srCarry] in
def JNZD : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eP:$a), "jnzd", "$d0, $s0, $a">{
// id: me__instr128_or__alu_mv__alumv_or__alu__jumpc_decr_ind__mRx__mPm
bits<5> s0;
bits<5> d0;
bits<3> a;

let alu = {s0, d0, a, 0b100, 0b0000};
}

let Itinerary = II_JNZ, hasDelaySlot = true in
def JNZ : AIE2P_inst_lng_instr48 <(outs), (ins eR:$s0, cpmaddr:$i), "jnz", " $s0, #$i">{
// id: me__instr128_or__alu_mv__lng__jumpc_imm__bnez__mRx
bits<5> s0;
bits<20> i;

let lng = {s0, i, 0b1, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b110};
}

let Itinerary = II_JZ, hasDelaySlot = true in
def JZ : AIE2P_inst_lng_instr48 <(outs), (ins eR:$s0, cpmaddr:$i), "jz", " $s0, #$i">{
// id: me__instr128_or__alu_mv__lng__jumpc_imm__beqz__mRx
bits<5> s0;
bits<20> i;

let lng = {s0, i, 0b0, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b110};
}

let Itinerary = II_J_alumv_or, hasDelaySlot = true in
def J_alumv_or : AIE2P_inst_alu_instr32 <(outs), (ins eP:$a), "j", "$a">{
// id: me__instr128_or__alu_mv__alumv_or__alu__jump_ind__mPm
bits<3> a;

let alu = {dontcare{5-1}, 0b100, dontcare{2-1}, a, 0b000, 0b0000};
}

let Itinerary = II_J_lng, hasDelaySlot = true in
def J_lng : AIE2P_inst_lng_instr48 <(outs), (ins cpmaddr:$i), "j", "#$i">{
// id: me__instr128_or__alu_mv__lng__jump_imm
bits<20> i;

let lng = {dontcare{5-1}, i, dontcare{1}, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b010};
}

let Itinerary = II_LDA_2D_dms_lda, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_2D_dms_lda_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_2D_dms_lda_eEho, [OperandRegClass<0, eEho>]>], DecoderMethod = "DecodeDms_lda_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dms_lda : AIE2P_inst_lda_instr32 <(outs OP_mLdaScl:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dms_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<7> dst;

let lda = {ptr, mod, 0b100, dst, 0b1101};
}

let Itinerary = II_LDA_2D_dmv_lda_q, DecoderMethod = "DecodeDmv_lda_q_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_dmv_lda_q : AIE2P_inst_lda_instr32 <(outs OP_mQQsa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_q__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<2> dst;

let lda = {ptr, mod, 0b100, dst, 0b001001111};
}

let Itinerary = II_LDA_2D_s16, DecoderMethod = "DecodeDmhb_lda_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_s16 : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d.s16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_2d__mPa__mDa__lh__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b10, 0b0101};
}

let Itinerary = II_LDA_2D_s8, DecoderMethod = "DecodeDmhb_lda_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_s8 : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d.s8", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_2d__mPa__mDa__lb__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b00, 0b0101};
}

let Itinerary = II_LDA_2D_u16, DecoderMethod = "DecodeDmhb_lda_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_u16 : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d.u16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_2d__mPa__mDa__lhu__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b11, 0b0101};
}

let Itinerary = II_LDA_2D_u8, DecoderMethod = "DecodeDmhb_lda_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_2D_u8 : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.2d.u8", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_2d__mPa__mDa__lbu__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b01, 0b0101};
}

let Itinerary = II_LDA_3D_dms_lda, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_3D_dms_lda_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_3D_dms_lda_eEho, [OperandRegClass<0, eEho>]>], DecoderMethod = "DecodeDms_lda_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dms_lda : AIE2P_inst_lda_instr32 <(outs OP_mLdaScl:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dms_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<7> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b1101};
}

let Itinerary = II_LDA_3D_dmv_lda_q, DecoderMethod = "DecodeDmv_lda_q_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_dmv_lda_q : AIE2P_inst_lda_instr32 <(outs OP_mQQsa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_q__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<2> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b001001111};
}

let Itinerary = II_LDA_3D_s16, DecoderMethod = "DecodeDmhb_lda_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_s16 : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d.s16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_3d__mPa__mDSa__lh__mRa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b10, 0b0101};
}

let Itinerary = II_LDA_3D_s8, DecoderMethod = "DecodeDmhb_lda_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_s8 : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d.s8", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_3d__mPa__mDSa__lb__mRa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b00, 0b0101};
}

let Itinerary = II_LDA_3D_u16, DecoderMethod = "DecodeDmhb_lda_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_u16 : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d.u16", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_3d__mPa__mDSa__lhu__mRa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b11, 0b0101};
}

let Itinerary = II_LDA_3D_u8, DecoderMethod = "DecodeDmhb_lda_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_3D_u8 : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.3d.u8", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_3d__mPa__mDSa__lbu__mRa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b01, 0b0101};
}

let Itinerary = II_LDA_TM_2D, DecoderMethod = "DecodeTm_lda_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_TM_2D : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "lda.tm.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_2d__mPa__mDa__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b100111};
}

let Itinerary = II_LDA_TM_3D, DecoderMethod = "DecodeTm_lda_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_TM_3D : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "lda.tm.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_3d__mPa__mDSa__mRa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b100111};
}

let Itinerary = II_LDA_TM_idx, mayLoad = true in
def LDA_TM_idx : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, eDJ:$dj), "lda.tm", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__idx__mPa__mDJa__mRa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b100111};
}

let Itinerary = II_LDA_TM_idx_imm, mayLoad = true in
def LDA_TM_idx_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, c6s_step4:$imm), "lda.tm", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__idx_imm__mPa__m_c06s_step4__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b100111};
}

let Itinerary = II_LDA_TM_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_TM_pstm_nrm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.tm", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm__mPa__mMa__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b100111};
}

let Itinerary = II_LDA_TM_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_TM_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, c6s_step4:$imm), "lda.tm", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__tm_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm_imm__mPa__m_c06s_step4__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b100111};
}

let Itinerary = II_LDA_dms_lda_idx, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_idx_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true in
def LDA_dms_lda_idx : AIE2P_inst_lda_instr32 <(outs OP_mLdaScl:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dms_lda__agua_dms__nrm__agua_dms__normal__agua_dms__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<7> dst;

let lda = {ptr, dj, 0b000, dst, 0b1101};
}

let Itinerary = II_LDA_dms_lda_idx_imm, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_idx_imm_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_idx_imm_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true in
def LDA_dms_lda_idx_imm : AIE2P_inst_lda_instr32 <(outs OP_mLdaScl:$dst), (ins eP:$ptr, c6s_step4:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dms_lda__agua_dms__nrm__agua_dms__normal__agua_dms__idx_imm__mPa__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<7> dst;

let lda = {ptr, imm, 0b01, dst, 0b1101};
}

let Itinerary = II_LDA_dms_lda_pstm_nrm, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dms_lda_pstm_nrm : AIE2P_inst_lda_instr32 <(outs OP_mLdaScl:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dms_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<7> dst;

let lda = {ptr, mod, 0b010, dst, 0b1101};
}

let Itinerary = II_LDA_dms_lda_pstm_nrm_imm, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_pstm_nrm_imm_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dms_lda_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs OP_mLdaScl:$dst, eP:$ptr_out), (ins eP:$ptr, c6s_step4:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dms_lda__agua_dms__nrm__agua_dms__normal__agua_dms__pstm_nrm_imm__mPa__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<7> dst;

let lda = {ptr, imm, 0b11, dst, 0b1101};
}

let Itinerary = II_LDA_dms_lda_spill, ItineraryRegPairs = [
ItinRegClassPair<II_LDA_dms_lda_spill_mLRa, [OperandRegClass<0, mLRa>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_LDA_dms_lda_spill_eEho, [OperandRegClass<0, eEho>]>], mayLoad = true, Uses = [sp] in
def LDA_dms_lda_spill : AIE2P_inst_lda_instr32 <(outs OP_mLdaScl:$dst), (ins c12n_step4:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dms_lda_spill__agua_dms__nrm_spill__agua_dms__spill__mSPa__m_c12n_step4
bits<9> imm;
bits<7> dst;

let lda = {imm, dst, 0b0010};
}

let Itinerary = II_LDA_dmv_lda_q_idx, mayLoad = true in
def LDA_dmv_lda_q_idx : AIE2P_inst_lda_instr32 <(outs OP_mQQsa:$dst), (ins eP:$ptr, eDJ:$dj), "lda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmv_lda_q__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<2> dst;

let lda = {ptr, dj, 0b000, dst, 0b001001111};
}

let Itinerary = II_LDA_dmv_lda_q_idx_imm, mayLoad = true in
def LDA_dmv_lda_q_idx_imm : AIE2P_inst_lda_instr32 <(outs OP_mQQsa:$dst), (ins eP:$ptr, c8s_step16:$imm), "lda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_q__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<2> dst;

let lda = {ptr, imm, 0b01, dst, 0b001001111};
}

let Itinerary = II_LDA_dmv_lda_q_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dmv_lda_q_pstm_nrm : AIE2P_inst_lda_instr32 <(outs OP_mQQsa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmv_lda_q__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<2> dst;

let lda = {ptr, mod, 0b010, dst, 0b001001111};
}

let Itinerary = II_LDA_dmv_lda_q_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_dmv_lda_q_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs OP_mQQsa:$dst, eP:$ptr_out), (ins eP:$ptr, c8s_step16:$imm), "lda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmv_lda_q__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<2> dst;

let lda = {ptr, imm, 0b11, dst, 0b001001111};
}

let Itinerary = II_LDA_dmv_lda_q_spill, mayLoad = true, Uses = [sp] in
def LDA_dmv_lda_q_spill : AIE2P_inst_lda_instr32 <(outs OP_mQQsa:$dst), (ins c14n_step16:$imm), "lda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_q_spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
bits<9> imm;
bits<2> dst;

let lda = {imm, dst, 0b101001111};
}

let Itinerary = II_LDA_s16_idx, mayLoad = true in
def LDA_s16_idx : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, eDJ:$dj), "lda.s16", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx__mPa__mDJa__lh__mRa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b10, 0b0101};
}

let Itinerary = II_LDA_s16_idx_imm, mayLoad = true in
def LDA_s16_idx_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, c5s_step2:$imm), "lda.s16", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx_imm__mPa__m_c05s_step2__lh__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b10, 0b0101};
}

let Itinerary = II_LDA_s16_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_s16_pstm_nrm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.s16", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm__mPa__mMa__lh__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b10, 0b0101};
}

let Itinerary = II_LDA_s16_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_s16_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, c5s_step2:$imm), "lda.s16", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm_imm__mPa__m_c05s_step2__lh__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b10, 0b0101};
}

let Itinerary = II_LDA_s8_idx, mayLoad = true in
def LDA_s8_idx : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, eDJ:$dj), "lda.s8", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx__mPa__mDJa__lb__mRa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b00, 0b0101};
}

let Itinerary = II_LDA_s8_idx_imm, mayLoad = true in
def LDA_s8_idx_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, c4s:$imm), "lda.s8", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx_imm__mPa__m_c04s__lb__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b00, 0b0101};
}

let Itinerary = II_LDA_s8_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_s8_pstm_nrm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.s8", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm__mPa__mMa__lb__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b00, 0b0101};
}

let Itinerary = II_LDA_s8_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_s8_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, c4s:$imm), "lda.s8", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm_imm__mPa__m_c04s__lb__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b00, 0b0101};
}

let Itinerary = II_LDA_u16_idx, mayLoad = true in
def LDA_u16_idx : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, eDJ:$dj), "lda.u16", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx__mPa__mDJa__lhu__mRa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b11, 0b0101};
}

let Itinerary = II_LDA_u16_idx_imm, mayLoad = true in
def LDA_u16_idx_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, c5s_step2:$imm), "lda.u16", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__idx_imm__mPa__m_c05s_step2__lhu__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b11, 0b0101};
}

let Itinerary = II_LDA_u16_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_u16_pstm_nrm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.u16", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm__mPa__mMa__lhu__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b11, 0b0101};
}

let Itinerary = II_LDA_u16_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_u16_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, c5s_step2:$imm), "lda.u16", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_lda__dmh_lda__agua_dmh__nrm__agua_dmh__normal__agua_dmh__pstm_nrm_imm__mPa__m_c05s_step2__lhu__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b11, 0b0101};
}

let Itinerary = II_LDA_u8_idx, mayLoad = true in
def LDA_u8_idx : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, eDJ:$dj), "lda.u8", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx__mPa__mDJa__lbu__mRa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b01, 0b0101};
}

let Itinerary = II_LDA_u8_idx_imm, mayLoad = true in
def LDA_u8_idx_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins eP:$ptr, c4s:$imm), "lda.u8", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__idx_imm__mPa__m_c04s__lbu__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b01, 0b0101};
}

let Itinerary = II_LDA_u8_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_u8_pstm_nrm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "lda.u8", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm__mPa__mMa__lbu__mRa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b01, 0b0101};
}

let Itinerary = II_LDA_u8_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def LDA_u8_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs eR:$dst, eP:$ptr_out), (ins eP:$ptr, c4s:$imm), "lda.u8", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_lda__dmb_lda__agua_dmb__nrm__agua_dmb__normal__agua_dmb__pstm_nrm_imm__mPa__m_c04s__lbu__mRa
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b01, 0b0101};
}

let Itinerary = II_LSHL in
def LSHL : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "lshl", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__uns__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1101, 0b1};
}

let Itinerary = II_LTU in
def LTU : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "ltu", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sltu__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1100, 0b1};
}

let Itinerary = II_LT in
def LT : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "lt", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__slt__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1010, 0b1};
}

let Itinerary = II_MAC, Constraints = "$a0 = $d0" in
def MAC : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$a0, eR:$s0, eR:$s1), "mac", "$d0, $a0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mac_r_rrr__mRa__mRz__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0, 0b1100};
}

let Itinerary = II_MOVA, ItineraryRegPairs = [
ItinRegClassPair<II_MOVA_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOVA_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_MOVA_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOVA_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOVA_mLCaCg, [OperandRegClass<0, mLCaCg>]>,
ItinRegClassPair<II_MOVA_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOVA_eM, [OperandRegClass<0, eM>]>], isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOVA : AIE2P_inst_lda_instr32 <(outs OP_mLdaCg:$dst), (ins c11s:$i), "mova", "$dst, #$i">{
// id: me__instr128_or__lda__lda_cg
bits<11> i;
bits<7> dst;

let lda = {i, dst, 0b00};
}

let Itinerary = II_MOVS, ItineraryRegPairs = [
ItinRegClassPair<II_MOVS_eDJ_eDJ, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eDC_eDJ, [OperandRegClass<0, eDC>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eDN_eDJ, [OperandRegClass<0, eDN>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eP_eDJ, [OperandRegClass<0, eP>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eM_eDJ, [OperandRegClass<0, eM>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOVS_eDJ_eR, [OperandRegClass<0, eDJ>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOVS_eDJ_eDC, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eDJ_eDN, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eDJ_eP, [OperandRegClass<0, eDJ>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eDJ_eM, [OperandRegClass<0, eDJ>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOVS_eDC_eR, [OperandRegClass<0, eDC>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOVS_eDC_eDC, [OperandRegClass<0, eDC>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eDC_eDN, [OperandRegClass<0, eDC>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eDC_eP, [OperandRegClass<0, eDC>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eDC_eM, [OperandRegClass<0, eDC>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOVS_eDN_eR, [OperandRegClass<0, eDN>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOVS_eDN_eDC, [OperandRegClass<0, eDN>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eDN_eDN, [OperandRegClass<0, eDN>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eDN_eP, [OperandRegClass<0, eDN>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eDN_eM, [OperandRegClass<0, eDN>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOVS_eP_eR, [OperandRegClass<0, eP>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOVS_eP_eDC, [OperandRegClass<0, eP>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eP_eDN, [OperandRegClass<0, eP>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eP_eP, [OperandRegClass<0, eP>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eP_eM, [OperandRegClass<0, eP>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOVS_eM_eR, [OperandRegClass<0, eM>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOVS_eM_eDC, [OperandRegClass<0, eM>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOVS_eM_eDN, [OperandRegClass<0, eM>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOVS_eM_eP, [OperandRegClass<0, eM>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOVS_eM_eM, [OperandRegClass<0, eM>, OperandRegClass<1, eM>]>] in
def MOVS : AIE2P_inst_st_instr32 <(outs OP_mAguDst:$dst), (ins OP_mAguSrc:$src), "movs", "$dst, $src">{
// id: me__instr128_or__st__mvs_agu__agus__mv
bits<6> dst;
bits<7> src;

let st = {dst, src, 0b0010110};
}

let Itinerary = II_MOVXM, ItineraryRegPairs = [
ItinRegClassPair<II_MOVXM_mSRF2IFlags, [OperandRegClass<0, mSRF2IFlags>]>,
ItinRegClassPair<II_MOVXM_mCRFPNlfMask, [OperandRegClass<0, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOVXM_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOVXM_mLEm, [OperandRegClass<0, mLEm>]>,
ItinRegClassPair<II_MOVXM_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOVXM_mSRCarry, [OperandRegClass<0, mSRCarry>]>,
ItinRegClassPair<II_MOVXM_mLRm, [OperandRegClass<0, mLRm>]>,
ItinRegClassPair<II_MOVXM_mCRPackSize, [OperandRegClass<0, mCRPackSize>]>,
ItinRegClassPair<II_MOVXM_mCRRnd, [OperandRegClass<0, mCRRnd>]>,
ItinRegClassPair<II_MOVXM_mSRFifo_of, [OperandRegClass<0, mSRFifo_of>]>,
ItinRegClassPair<II_MOVXM_mCRF2FMask, [OperandRegClass<0, mCRF2FMask>]>,
ItinRegClassPair<II_MOVXM_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOVXM_mSRF2FFlags, [OperandRegClass<0, mSRF2FFlags>]>,
ItinRegClassPair<II_MOVXM_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOVXM_mCRF2IMask, [OperandRegClass<0, mCRF2IMask>]>,
ItinRegClassPair<II_MOVXM_mCRF2BMask, [OperandRegClass<0, mCRF2BMask>]>,
ItinRegClassPair<II_MOVXM_eVaddSign, [OperandRegClass<0, eVaddSign>]>,
ItinRegClassPair<II_MOVXM_eS, [OperandRegClass<0, eS>]>,
ItinRegClassPair<II_MOVXM_mSRFifo_uf, [OperandRegClass<0, mSRFifo_uf>]>,
ItinRegClassPair<II_MOVXM_mCRMCDEn, [OperandRegClass<0, mCRMCDEn>]>,
ItinRegClassPair<II_MOVXM_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOVXM_mSRMS0, [OperandRegClass<0, mSRMS0>]>,
ItinRegClassPair<II_MOVXM_mSRUPS_of, [OperandRegClass<0, mSRUPS_of>]>,
ItinRegClassPair<II_MOVXM_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOVXM_mSRSparse_of, [OperandRegClass<0, mSRSparse_of>]>,
ItinRegClassPair<II_MOVXM_mCRSRSMode, [OperandRegClass<0, mCRSRSMode>]>,
ItinRegClassPair<II_MOVXM_mCRSCDEn, [OperandRegClass<0, mCRSCDEn>]>,
ItinRegClassPair<II_MOVXM_mLSm, [OperandRegClass<0, mLSm>]>,
ItinRegClassPair<II_MOVXM_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOVXM_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOVXM_mCRSat, [OperandRegClass<0, mCRSat>]>,
ItinRegClassPair<II_MOVXM_mLCmCg, [OperandRegClass<0, mLCmCg>]>,
ItinRegClassPair<II_MOVXM_mCRFPMask, [OperandRegClass<0, mCRFPMask>]>,
ItinRegClassPair<II_MOVXM_eUnpackSign, [OperandRegClass<0, eUnpackSign>]>,
ItinRegClassPair<II_MOVXM_mSRSRS_of, [OperandRegClass<0, mSRSRS_of>]>,
ItinRegClassPair<II_MOVXM_mSRFPFlags, [OperandRegClass<0, mSRFPFlags>]>,
ItinRegClassPair<II_MOVXM_mSRF2BFlags, [OperandRegClass<0, mSRF2BFlags>]>,
ItinRegClassPair<II_MOVXM_mCRUPSMode, [OperandRegClass<0, mCRUPSMode>]>,
ItinRegClassPair<II_MOVXM_mSRFPNlf, [OperandRegClass<0, mSRFPNlf>]>,
ItinRegClassPair<II_MOVXM_mCRUnpackSize, [OperandRegClass<0, mCRUnpackSize>]>,
ItinRegClassPair<II_MOVXM_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_MOVXM_eUPSSign, [OperandRegClass<0, eUPSSign>]>,
ItinRegClassPair<II_MOVXM_ePackSign, [OperandRegClass<0, ePackSign>]>,
ItinRegClassPair<II_MOVXM_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOVXM_mSPm, [OperandRegClass<0, mSPm>]>,
ItinRegClassPair<II_MOVXM_mSRSS0, [OperandRegClass<0, mSRSS0>]>,
ItinRegClassPair<II_MOVXM_eSRSSign, [OperandRegClass<0, eSRSSign>]>], isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOVXM : AIE2P_inst_lng_instr48 <(outs OP_mMvSclDstCg:$dst), (ins c32s:$i), "movxm", "$dst, #$i">{
// id: me__instr128_or__alu_mv__lng__lng_cg
bits<32> i;
bits<7> dst;

let lng = {i{31-12}, dst, i{11-0}, 0b001};
}

let Itinerary = II_MOVX_alu_cg, isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOVX_alu_cg : AIE2P_inst_alu_instr32 <(outs OP_mAluCg:$dst), (ins c11s:$i), "movx", "$dst, #$i">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_cg
bits<11> i;
bits<6> dst;

let alu = {i{10-6}, dst, i{5-0}, 0b010};
}

let Itinerary = II_MOVX_mvx_cr_imm, ItineraryRegPairs = [
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRSCDEn, [OperandRegClass<0, mCRSCDEn>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRPackSize, [OperandRegClass<0, mCRPackSize>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRRnd, [OperandRegClass<0, mCRRnd>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRSat, [OperandRegClass<0, mCRSat>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRFPMask, [OperandRegClass<0, mCRFPMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRF2IMask, [OperandRegClass<0, mCRF2IMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRF2FMask, [OperandRegClass<0, mCRF2FMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_eVaddSign, [OperandRegClass<0, eVaddSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRF2BMask, [OperandRegClass<0, mCRF2BMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_eUnpackSign, [OperandRegClass<0, eUnpackSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_eSRSSign, [OperandRegClass<0, eSRSSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_eUPSSign, [OperandRegClass<0, eUPSSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRUPSMode, [OperandRegClass<0, mCRUPSMode>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRUnpackSize, [OperandRegClass<0, mCRUnpackSize>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_ePackSign, [OperandRegClass<0, ePackSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRMCDEn, [OperandRegClass<0, mCRMCDEn>]>,
ItinRegClassPair<II_MOVX_mvx_cr_imm_mCRSRSMode, [OperandRegClass<0, mCRSRSMode>]>], isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOVX_mvx_cr_imm : AIE2P_inst_alu_instr32 <(outs OP_mCRm:$dst), (ins c5u:$src), "movx", "$dst, #$src">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mvx_cr_imm
bits<5> src;
bits<5> dst;

let alu = {src, 0b011, dst, 0b000, 0b0000};
}

let Itinerary = II_MOVX_mvx_cr_r, ItineraryRegPairs = [
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRSCDEn, [OperandRegClass<0, mCRSCDEn>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRPackSize, [OperandRegClass<0, mCRPackSize>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRRnd, [OperandRegClass<0, mCRRnd>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRSat, [OperandRegClass<0, mCRSat>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRFPMask, [OperandRegClass<0, mCRFPMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRF2IMask, [OperandRegClass<0, mCRF2IMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRF2FMask, [OperandRegClass<0, mCRF2FMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eVaddSign, [OperandRegClass<0, eVaddSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRF2BMask, [OperandRegClass<0, mCRF2BMask>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eUnpackSign, [OperandRegClass<0, eUnpackSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eSRSSign, [OperandRegClass<0, eSRSSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_eUPSSign, [OperandRegClass<0, eUPSSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRUPSMode, [OperandRegClass<0, mCRUPSMode>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRUnpackSize, [OperandRegClass<0, mCRUnpackSize>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_ePackSign, [OperandRegClass<0, ePackSign>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRMCDEn, [OperandRegClass<0, mCRMCDEn>]>,
ItinRegClassPair<II_MOVX_mvx_cr_r_mCRSRSMode, [OperandRegClass<0, mCRSRSMode>]>] in
def MOVX_mvx_cr_r : AIE2P_inst_alu_instr32 <(outs OP_mCRm:$dst), (ins eR:$src), "movx", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mvx_cr_r__mRx
bits<5> src;
bits<5> dst;

let alu = {src, 0b111, dst, 0b000, 0b0000};
}

let Itinerary = II_MOV_CPH_mMStream_tlast_imm, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_mMStream_tlast_imm : AIE2P_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, eR:$id), "mov.cph", " ms, $addr, #$nw, #$op, $id">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__noTlast__mMs__mRs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b0, 0b0, 0b0, 0b0, op, addr, id, nw, 0b0100};
}

let Itinerary = II_MOV_CPH_mMStream_tlast_reg, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_mMStream_tlast_reg : AIE2P_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, eR:$id, mR28_tlast:$dst), "mov.cph", " ms, $addr, #$nw, #$op, $id, r28">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_reg__mR28_tlast__mMs__mRs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b0, 0b1, dontcare{1}, 0b0, op, addr, id, nw, 0b0100};
}

let Itinerary = II_MOV_CPH_nb_mMStream_tlast_imm, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_nb_mMStream_tlast_imm : AIE2P_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, eR:$id), "mov.cph.nb", " ms, $addr, #$nw, #$op, $id">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__noTlast__mMs__mRs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b1, 0b0, 0b0, 0b0, op, addr, id, nw, 0b0100};
}

let Itinerary = II_MOV_CPH_nb_mMStream_tlast_reg, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_nb_mMStream_tlast_reg : AIE2P_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, eR:$id, mR28_tlast:$dst), "mov.cph.nb", " ms, $addr, #$nw, #$op, $id, r28">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_reg__mR28_tlast__mMs__mRs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b1, 0b1, dontcare{1}, 0b0, op, addr, id, nw, 0b0100};
}

let Itinerary = II_MOV_CPH_nb_tlast, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_nb_tlast : AIE2P_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, eR:$id), "mov.cph.nb.tlast", " ms, $addr, #$nw, #$op, $id">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__doTlast__mMs__mRs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b1, 0b0, 0b1, 0b0, op, addr, id, nw, 0b0100};
}

let Itinerary = II_MOV_CPH_tlast, hasSideEffects = true, Defs = [srMS0] in
def MOV_CPH_tlast : AIE2P_inst_st_instr32 <(outs), (ins eM:$addr, c2u:$nw, c2u:$op, eR:$id), "mov.cph.tlast", " ms, $addr, #$nw, #$op, $id">{
// id: me__instr128_or__st__st_streams__mv_cph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__doTlast__mMs__mRs
bits<2> op;
bits<3> addr;
bits<5> id;
bits<2> nw;

let st = {0b0, 0b0, 0b1, 0b0, op, addr, id, nw, 0b0100};
}

let Itinerary = II_MOV_PH_mMStream_tlast_imm, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_mMStream_tlast_imm : AIE2P_inst_st_instr32 <(outs), (ins eR:$id, c3u:$pcktType), "mov.ph", " ms, $id, #$pcktType">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__noTlast__mRs
bits<3> pcktType;
bits<5> id;

let st = {0b0, 0b0, 0b0, 0b100, pcktType, id, dontcare{2-1}, 0b0100};
}

let Itinerary = II_MOV_PH_mMStream_tlast_reg, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_mMStream_tlast_reg : AIE2P_inst_st_instr32 <(outs), (ins eR:$id, c3u:$pcktType, mR28_tlast:$dst), "mov.ph", " ms, $id, #$pcktType, r28">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_reg__mR28_tlast__mRs
bits<3> pcktType;
bits<5> id;

let st = {0b0, 0b1, dontcare{1}, 0b100, pcktType, id, dontcare{2-1}, 0b0100};
}

let Itinerary = II_MOV_PH_nb_mMStream_tlast_imm, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_nb_mMStream_tlast_imm : AIE2P_inst_st_instr32 <(outs), (ins eR:$id, c3u:$pcktType), "mov.ph.nb", " ms, $id, #$pcktType">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__noTlast__mRs
bits<3> pcktType;
bits<5> id;

let st = {0b1, 0b0, 0b0, 0b100, pcktType, id, dontcare{2-1}, 0b0100};
}

let Itinerary = II_MOV_PH_nb_mMStream_tlast_reg, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_nb_mMStream_tlast_reg : AIE2P_inst_st_instr32 <(outs), (ins eR:$id, c3u:$pcktType, mR28_tlast:$dst), "mov.ph.nb", " ms, $id, #$pcktType, r28">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_reg__mR28_tlast__mRs
bits<3> pcktType;
bits<5> id;

let st = {0b1, 0b1, dontcare{1}, 0b100, pcktType, id, dontcare{2-1}, 0b0100};
}

let Itinerary = II_MOV_PH_nb_tlast, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_nb_tlast : AIE2P_inst_st_instr32 <(outs), (ins eR:$id, c3u:$pcktType), "mov.ph.nb.tlast", " ms, $id, #$pcktType">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__doTlast__mRs
bits<3> pcktType;
bits<5> id;

let st = {0b1, 0b0, 0b1, 0b100, pcktType, id, dontcare{2-1}, 0b0100};
}

let Itinerary = II_MOV_PH_tlast, hasSideEffects = true, Defs = [srMS0] in
def MOV_PH_tlast : AIE2P_inst_st_instr32 <(outs), (ins eR:$id, c3u:$pcktType), "mov.ph.tlast", " ms, $id, #$pcktType">{
// id: me__instr128_or__st__st_streams__mv_ph2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__doTlast__mRs
bits<3> pcktType;
bits<5> id;

let st = {0b0, 0b0, 0b1, 0b100, pcktType, id, dontcare{2-1}, 0b0100};
}

let Itinerary = II_MOV_alu_mv_alu_flt2fx, Defs = [srFPCnvFl2Fx], Uses = [crFPCnvFl2FxMask] in
def MOV_alu_mv_alu_flt2fx : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, mS3:$m), "mov", "$d0, ${s0}.flt2fx, s3">{
// id: me__instr128_or__alu_mv__alumv_or__alu__flt2fx__mRa__mRx__mFlt2fx__mS3
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, dontcare{3-1}, 0b010, 0b1000};
}

let Itinerary = II_MOV_alu_mv_alu_fx2flt, Defs = [srFPCnvFx2Fl], Uses = [crFPCnvFx2FlMask] in
def MOV_alu_mv_alu_fx2flt : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, mS2:$m), "mov", "$d0, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__fx2flt__mRa__mRx__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, dontcare{3-1}, 0b110, 0b1000};
}

let Itinerary = II_MOV_alu_mv_mv_mv_cg, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRF2IFlags, [OperandRegClass<0, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRFPNlfMask, [OperandRegClass<0, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mLEm, [OperandRegClass<0, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRCarry, [OperandRegClass<0, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mLRm, [OperandRegClass<0, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRPackSize, [OperandRegClass<0, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRRnd, [OperandRegClass<0, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRFifo_of, [OperandRegClass<0, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRF2FMask, [OperandRegClass<0, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRF2FFlags, [OperandRegClass<0, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRF2IMask, [OperandRegClass<0, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRF2BMask, [OperandRegClass<0, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eVaddSign, [OperandRegClass<0, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eS, [OperandRegClass<0, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRFifo_uf, [OperandRegClass<0, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRMCDEn, [OperandRegClass<0, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRMS0, [OperandRegClass<0, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRUPS_of, [OperandRegClass<0, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRSparse_of, [OperandRegClass<0, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRSRSMode, [OperandRegClass<0, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRSCDEn, [OperandRegClass<0, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mLSm, [OperandRegClass<0, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRSat, [OperandRegClass<0, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mLCmCg, [OperandRegClass<0, mLCmCg>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRFPMask, [OperandRegClass<0, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eUnpackSign, [OperandRegClass<0, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRSRS_of, [OperandRegClass<0, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRFPFlags, [OperandRegClass<0, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRF2BFlags, [OperandRegClass<0, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRUPSMode, [OperandRegClass<0, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRFPNlf, [OperandRegClass<0, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mCRUnpackSize, [OperandRegClass<0, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eUPSSign, [OperandRegClass<0, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_ePackSign, [OperandRegClass<0, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSPm, [OperandRegClass<0, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_mSRSS0, [OperandRegClass<0, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_cg_eSRSSign, [OperandRegClass<0, eSRSSign>]>], isMoveImm = true, isReMaterializable = true, isAsCheapAsAMove = true in
def MOV_alu_mv_mv_mv_cg : AIE2P_inst_mv_instr32 <(outs OP_mMvSclDstCg:$dst), (ins c11s:$i), "mov", "$dst, #$i">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cg
bits<7> dst;
bits<11> i;

let mv = {dst, i, 0b0101};
}

let Itinerary = II_MOV_alu_mv_mv_mv_cntr2l, hasSideEffects = true in
def MOV_alu_mv_mv_mv_cntr2l : AIE2P_inst_mv_instr32 <(outs eL:$dst), (ins), "mov", "$dst, cntr">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cntr2l__mLm
bits<4> dst;

let mv = {dst, dontcare{5-1}, 0b0101001010111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_eh_to_eh in
def MOV_alu_mv_mv_mv_e_mv_eh_to_eh : AIE2P_inst_mv_instr32 <(outs OP_mEhm:$dst), (ins OP_mEhm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_eh_to_eh
bits<4> dst;
bits<4> src;

let mv = {dst, 0b10, src, 0b10, 0b0011100111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_eh_to_el in
def MOV_alu_mv_mv_mv_e_mv_eh_to_el : AIE2P_inst_mv_instr32 <(outs OP_mElm:$dst), (ins OP_mEhm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_eh_to_el
bits<4> dst;
bits<4> src;

let mv = {dst, 0b00, src, 0b10, 0b0011100111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_eh_to_r in
def MOV_alu_mv_mv_mv_e_mv_eh_to_r : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mEhm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_eh_to_r__mRm
bits<5> dst;
bits<4> src;

let mv = {dst, 0b1, src, 0b10, 0b0011100111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_el_to_eh in
def MOV_alu_mv_mv_mv_e_mv_el_to_eh : AIE2P_inst_mv_instr32 <(outs OP_mEhm:$dst), (ins OP_mElm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_el_to_eh
bits<4> dst;
bits<4> src;

let mv = {dst, 0b10, src, 0b00, 0b0011100111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_el_to_el in
def MOV_alu_mv_mv_mv_e_mv_el_to_el : AIE2P_inst_mv_instr32 <(outs OP_mElm:$dst), (ins OP_mElm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_el_to_el
bits<4> dst;
bits<4> src;

let mv = {dst, 0b00, src, 0b00, 0b0011100111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_el_to_r in
def MOV_alu_mv_mv_mv_e_mv_el_to_r : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mElm:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_el_to_r__mRm
bits<5> dst;
bits<4> src;

let mv = {dst, 0b1, src, 0b00, 0b0011100111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_r_to_eh in
def MOV_alu_mv_mv_mv_e_mv_r_to_eh : AIE2P_inst_mv_instr32 <(outs OP_mEhm:$dst), (ins eR:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_r_to_eh__mRm
bits<4> dst;
bits<5> src;

let mv = {dst, 0b10, src, 0b1, 0b0011100111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_e_mv_r_to_el in
def MOV_alu_mv_mv_mv_e_mv_r_to_el : AIE2P_inst_mv_instr32 <(outs OP_mElm:$dst), (ins eR:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_e__mv_r_to_el__mRm
bits<4> dst;
bits<5> src;

let mv = {dst, 0b00, src, 0b1, 0b0011100111};
}

let Itinerary = II_MOV_alu_mv_mv_mv_scl, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRF2IFlags, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRF2IFlags, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRF2IFlags, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRF2IFlags, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRF2IFlags, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRF2IFlags, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRF2IFlags, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRF2IFlags, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRF2IFlags, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRF2IFlags, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRF2IFlags, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRF2IFlags, [OperandRegClass<0, eP>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRF2IFlags, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRF2IFlags, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRF2IFlags, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRF2IFlags, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRF2IFlags, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRF2IFlags, [OperandRegClass<0, eS>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRF2IFlags, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRF2IFlags, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRF2IFlags, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRF2IFlags, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRF2IFlags, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRF2IFlags, [OperandRegClass<0, eM>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRF2IFlags, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRF2IFlags, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRF2IFlags, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRF2IFlags, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRF2IFlags, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRF2IFlags, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRF2IFlags, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRF2IFlags, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRF2IFlags, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRF2IFlags, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRF2IFlags, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRF2IFlags, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRF2IFlags, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRF2IFlags, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRF2IFlags, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRF2IFlags, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRF2IFlags, [OperandRegClass<0, eR>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRF2IFlags, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRF2IFlags, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRF2IFlags, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRF2IFlags, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRF2IFlags, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRF2IFlags, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRFPNlfMask, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mLEm, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eDJ, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRCarry, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mLRm, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRPackSize, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRRnd, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRFifo_of, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRF2FMask, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eP, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRF2FFlags, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRF2IMask, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRF2BMask, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eVaddSign, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eS, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRFifo_uf, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRMCDEn, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRMS0, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRUPS_of, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eM, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRSparse_of, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRSRSMode, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mLCm, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRSCDEn, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mLSm, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eDC, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRSat, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRFPMask, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eUnpackSign, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRSRS_of, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRFPFlags, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRF2BFlags, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRUPSMode, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRFPNlf, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mCRUnpackSize, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eR, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eUPSSign, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_ePackSign, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eDN, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSPm, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_mSRSS0, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2IFlags_eSRSSign, [OperandRegClass<0, mSRF2IFlags>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRFPNlfMask, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mLEm, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eDJ, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRCarry, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mLRm, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRPackSize, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRRnd, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRFifo_of, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRF2FMask, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eP, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRF2FFlags, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRF2IMask, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRF2BMask, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eVaddSign, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eS, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRFifo_uf, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRMCDEn, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRMS0, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRUPS_of, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eM, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRSparse_of, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRSRSMode, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mLCm, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRSCDEn, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mLSm, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eDC, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRSat, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRFPMask, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eUnpackSign, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRSRS_of, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRFPFlags, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRF2BFlags, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRUPSMode, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRFPNlf, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mCRUnpackSize, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eR, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eUPSSign, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_ePackSign, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eDN, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSPm, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_mSRSS0, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPNlfMask_eSRSSign, [OperandRegClass<0, mCRFPNlfMask>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRFPNlfMask, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mLEm, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eDJ, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRCarry, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mLRm, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRPackSize, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRRnd, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRFifo_of, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRF2FMask, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eP, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRF2FFlags, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRF2IMask, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRF2BMask, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eVaddSign, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eS, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRFifo_uf, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRMCDEn, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRMS0, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRUPS_of, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eM, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRSparse_of, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRSRSMode, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mLCm, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRSCDEn, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mLSm, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eDC, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRSat, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRFPMask, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eUnpackSign, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRSRS_of, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRFPFlags, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRF2BFlags, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRUPSMode, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRFPNlf, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mCRUnpackSize, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eR, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eUPSSign, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_ePackSign, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eDN, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSPm, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_mSRSS0, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFl2Fx_eSRSSign, [OperandRegClass<0, mSRFPCnvFl2Fx>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRFPNlfMask, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRFPCnvFl2Fx, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mLEm, [OperandRegClass<0, mLEm>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eDJ, [OperandRegClass<0, mLEm>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRCarry, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mLRm, [OperandRegClass<0, mLEm>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRPackSize, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRRnd, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRFifo_of, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRF2FMask, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eP, [OperandRegClass<0, mLEm>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRF2FFlags, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRFPCnvFx2Fl, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRF2IMask, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRF2BMask, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eVaddSign, [OperandRegClass<0, mLEm>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eS, [OperandRegClass<0, mLEm>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRFifo_uf, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRMCDEn, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRFPCnvFx2FlMask, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRMS0, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRUPS_of, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eM, [OperandRegClass<0, mLEm>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRSparse_of, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRSRSMode, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mLCm, [OperandRegClass<0, mLEm>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRSCDEn, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mLSm, [OperandRegClass<0, mLEm>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eDC, [OperandRegClass<0, mLEm>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRFPCnvFl2FxMask, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRSat, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRFPMask, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eUnpackSign, [OperandRegClass<0, mLEm>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRSRS_of, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRFPFlags, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRF2BFlags, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRUPSMode, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRFPNlf, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mCRUnpackSize, [OperandRegClass<0, mLEm>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eR, [OperandRegClass<0, mLEm>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eUPSSign, [OperandRegClass<0, mLEm>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_ePackSign, [OperandRegClass<0, mLEm>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eDN, [OperandRegClass<0, mLEm>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSPm, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_mSRSS0, [OperandRegClass<0, mLEm>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLEm_eSRSSign, [OperandRegClass<0, mLEm>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRFPNlfMask, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRFPCnvFl2Fx, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mLEm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eDJ, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRCarry, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mLRm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRPackSize, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRRnd, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRFifo_of, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRF2FMask, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eP, [OperandRegClass<0, eDJ>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRF2FFlags, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRFPCnvFx2Fl, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRF2IMask, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRF2BMask, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eVaddSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eS, [OperandRegClass<0, eDJ>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRFifo_uf, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRMCDEn, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRFPCnvFx2FlMask, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRMS0, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRUPS_of, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eM, [OperandRegClass<0, eDJ>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRSparse_of, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRSRSMode, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mLCm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRSCDEn, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mLSm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eDC, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRFPCnvFl2FxMask, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRSat, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRFPMask, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eUnpackSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRSRS_of, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRFPFlags, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRF2BFlags, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRUPSMode, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRFPNlf, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mCRUnpackSize, [OperandRegClass<0, eDJ>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eR, [OperandRegClass<0, eDJ>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eUPSSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_ePackSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eDN, [OperandRegClass<0, eDJ>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSPm, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_mSRSS0, [OperandRegClass<0, eDJ>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDJ_eSRSSign, [OperandRegClass<0, eDJ>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRFPNlfMask, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mLEm, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eDJ, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRCarry, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mLRm, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRPackSize, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRRnd, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRFifo_of, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRF2FMask, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eP, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRF2FFlags, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRF2IMask, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRF2BMask, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eVaddSign, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eS, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRFifo_uf, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRMCDEn, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRMS0, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRUPS_of, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eM, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRSparse_of, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRSRSMode, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mLCm, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRSCDEn, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mLSm, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eDC, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRSat, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRFPMask, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eUnpackSign, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRSRS_of, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRFPFlags, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRF2BFlags, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRUPSMode, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRFPNlf, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mCRUnpackSize, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eR, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eUPSSign, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_ePackSign, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eDN, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSPm, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_mSRSS0, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRCarry_eSRSSign, [OperandRegClass<0, mSRCarry>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRFPNlfMask, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRFPCnvFl2Fx, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mLEm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eDJ, [OperandRegClass<0, mLRm>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRCarry, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mLRm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRPackSize, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRRnd, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRFifo_of, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRF2FMask, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eP, [OperandRegClass<0, mLRm>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRF2FFlags, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRFPCnvFx2Fl, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRF2IMask, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRF2BMask, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eVaddSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eS, [OperandRegClass<0, mLRm>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRFifo_uf, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRMCDEn, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRFPCnvFx2FlMask, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRMS0, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRUPS_of, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eM, [OperandRegClass<0, mLRm>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRSparse_of, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRSRSMode, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mLCm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRSCDEn, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mLSm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eDC, [OperandRegClass<0, mLRm>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRFPCnvFl2FxMask, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRSat, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRFPMask, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eUnpackSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRSRS_of, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRFPFlags, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRF2BFlags, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRUPSMode, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRFPNlf, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mCRUnpackSize, [OperandRegClass<0, mLRm>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eR, [OperandRegClass<0, mLRm>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eUPSSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_ePackSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eDN, [OperandRegClass<0, mLRm>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSPm, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_mSRSS0, [OperandRegClass<0, mLRm>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLRm_eSRSSign, [OperandRegClass<0, mLRm>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRFPNlfMask, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mLEm, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eDJ, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRCarry, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mLRm, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRPackSize, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRRnd, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRFifo_of, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRF2FMask, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eP, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRF2FFlags, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRF2IMask, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRF2BMask, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eVaddSign, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eS, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRFifo_uf, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRMCDEn, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRMS0, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRUPS_of, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eM, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRSparse_of, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRSRSMode, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mLCm, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRSCDEn, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mLSm, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eDC, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRSat, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRFPMask, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eUnpackSign, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRSRS_of, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRFPFlags, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRF2BFlags, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRUPSMode, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRFPNlf, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mCRUnpackSize, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eR, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eUPSSign, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_ePackSign, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eDN, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSPm, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_mSRSS0, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRPackSize_eSRSSign, [OperandRegClass<0, mCRPackSize>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRFPNlfMask, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mLEm, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eDJ, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRCarry, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mLRm, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRPackSize, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRRnd, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRFifo_of, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRF2FMask, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eP, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRF2FFlags, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRF2IMask, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRF2BMask, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eVaddSign, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eS, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRFifo_uf, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRMCDEn, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRMS0, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRUPS_of, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eM, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRSparse_of, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRSRSMode, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mLCm, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRSCDEn, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mLSm, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eDC, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRSat, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRFPMask, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eUnpackSign, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRSRS_of, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRFPFlags, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRF2BFlags, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRUPSMode, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRFPNlf, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mCRUnpackSize, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eR, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eUPSSign, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_ePackSign, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eDN, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSPm, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_mSRSS0, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRRnd_eSRSSign, [OperandRegClass<0, mCRRnd>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRFPNlfMask, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mLEm, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eDJ, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRCarry, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mLRm, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRPackSize, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRRnd, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRFifo_of, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRF2FMask, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eP, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRF2FFlags, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRF2IMask, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRF2BMask, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eVaddSign, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eS, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRFifo_uf, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRMCDEn, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRMS0, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRUPS_of, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eM, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRSparse_of, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRSRSMode, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mLCm, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRSCDEn, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mLSm, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eDC, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRSat, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRFPMask, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eUnpackSign, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRSRS_of, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRFPFlags, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRF2BFlags, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRUPSMode, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRFPNlf, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mCRUnpackSize, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eR, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eUPSSign, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_ePackSign, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eDN, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSPm, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_mSRSS0, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_of_eSRSSign, [OperandRegClass<0, mSRFifo_of>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRFPNlfMask, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mLEm, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eDJ, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRCarry, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mLRm, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRPackSize, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRRnd, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRFifo_of, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRF2FMask, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eP, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRF2FFlags, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRF2IMask, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRF2BMask, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eVaddSign, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eS, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRFifo_uf, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRMCDEn, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRMS0, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRUPS_of, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eM, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRSparse_of, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRSRSMode, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mLCm, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRSCDEn, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mLSm, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eDC, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRSat, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRFPMask, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eUnpackSign, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRSRS_of, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRFPFlags, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRF2BFlags, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRUPSMode, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRFPNlf, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mCRUnpackSize, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eR, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eUPSSign, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_ePackSign, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eDN, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSPm, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_mSRSS0, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2FMask_eSRSSign, [OperandRegClass<0, mCRF2FMask>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRFPNlfMask, [OperandRegClass<0, eP>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRFPCnvFl2Fx, [OperandRegClass<0, eP>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mLEm, [OperandRegClass<0, eP>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eDJ, [OperandRegClass<0, eP>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRCarry, [OperandRegClass<0, eP>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mLRm, [OperandRegClass<0, eP>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRPackSize, [OperandRegClass<0, eP>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRRnd, [OperandRegClass<0, eP>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRFifo_of, [OperandRegClass<0, eP>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRF2FMask, [OperandRegClass<0, eP>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eP, [OperandRegClass<0, eP>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRF2FFlags, [OperandRegClass<0, eP>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRFPCnvFx2Fl, [OperandRegClass<0, eP>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRF2IMask, [OperandRegClass<0, eP>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRF2BMask, [OperandRegClass<0, eP>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eVaddSign, [OperandRegClass<0, eP>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eS, [OperandRegClass<0, eP>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRFifo_uf, [OperandRegClass<0, eP>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRMCDEn, [OperandRegClass<0, eP>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRFPCnvFx2FlMask, [OperandRegClass<0, eP>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRMS0, [OperandRegClass<0, eP>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRUPS_of, [OperandRegClass<0, eP>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eM, [OperandRegClass<0, eP>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRSparse_of, [OperandRegClass<0, eP>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRSRSMode, [OperandRegClass<0, eP>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mLCm, [OperandRegClass<0, eP>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRSCDEn, [OperandRegClass<0, eP>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mLSm, [OperandRegClass<0, eP>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eDC, [OperandRegClass<0, eP>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRFPCnvFl2FxMask, [OperandRegClass<0, eP>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRSat, [OperandRegClass<0, eP>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRFPMask, [OperandRegClass<0, eP>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eUnpackSign, [OperandRegClass<0, eP>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRSRS_of, [OperandRegClass<0, eP>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRFPFlags, [OperandRegClass<0, eP>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRF2BFlags, [OperandRegClass<0, eP>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRUPSMode, [OperandRegClass<0, eP>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRFPNlf, [OperandRegClass<0, eP>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mCRUnpackSize, [OperandRegClass<0, eP>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eR, [OperandRegClass<0, eP>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eUPSSign, [OperandRegClass<0, eP>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_ePackSign, [OperandRegClass<0, eP>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eDN, [OperandRegClass<0, eP>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSPm, [OperandRegClass<0, eP>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_mSRSS0, [OperandRegClass<0, eP>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eP_eSRSSign, [OperandRegClass<0, eP>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRFPNlfMask, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mLEm, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eDJ, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRCarry, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mLRm, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRPackSize, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRRnd, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRFifo_of, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRF2FMask, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eP, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRF2FFlags, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRF2IMask, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRF2BMask, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eVaddSign, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eS, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRFifo_uf, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRMCDEn, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRMS0, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRUPS_of, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eM, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRSparse_of, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRSRSMode, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mLCm, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRSCDEn, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mLSm, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eDC, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRSat, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRFPMask, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eUnpackSign, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRSRS_of, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRFPFlags, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRF2BFlags, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRUPSMode, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRFPNlf, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mCRUnpackSize, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eR, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eUPSSign, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_ePackSign, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eDN, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSPm, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_mSRSS0, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2FFlags_eSRSSign, [OperandRegClass<0, mSRF2FFlags>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRFPNlfMask, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mLEm, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eDJ, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRCarry, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mLRm, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRPackSize, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRRnd, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRFifo_of, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRF2FMask, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eP, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRF2FFlags, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRF2IMask, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRF2BMask, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eVaddSign, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eS, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRFifo_uf, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRMCDEn, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRMS0, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRUPS_of, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eM, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRSparse_of, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRSRSMode, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mLCm, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRSCDEn, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mLSm, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eDC, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRSat, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRFPMask, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eUnpackSign, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRSRS_of, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRFPFlags, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRF2BFlags, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRUPSMode, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRFPNlf, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mCRUnpackSize, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eR, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eUPSSign, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_ePackSign, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eDN, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSPm, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_mSRSS0, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPCnvFx2Fl_eSRSSign, [OperandRegClass<0, mSRFPCnvFx2Fl>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRFPNlfMask, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mLEm, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eDJ, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRCarry, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mLRm, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRPackSize, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRRnd, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRFifo_of, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRF2FMask, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eP, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRF2FFlags, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRF2IMask, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRF2BMask, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eVaddSign, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eS, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRFifo_uf, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRMCDEn, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRMS0, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRUPS_of, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eM, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRSparse_of, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRSRSMode, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mLCm, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRSCDEn, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mLSm, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eDC, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRSat, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRFPMask, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eUnpackSign, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRSRS_of, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRFPFlags, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRF2BFlags, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRUPSMode, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRFPNlf, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mCRUnpackSize, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eR, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eUPSSign, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_ePackSign, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eDN, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSPm, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_mSRSS0, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2IMask_eSRSSign, [OperandRegClass<0, mCRF2IMask>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRFPNlfMask, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mLEm, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eDJ, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRCarry, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mLRm, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRPackSize, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRRnd, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRFifo_of, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRF2FMask, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eP, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRF2FFlags, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRF2IMask, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRF2BMask, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eVaddSign, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eS, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRFifo_uf, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRMCDEn, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRMS0, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRUPS_of, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eM, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRSparse_of, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRSRSMode, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mLCm, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRSCDEn, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mLSm, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eDC, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRSat, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRFPMask, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eUnpackSign, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRSRS_of, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRFPFlags, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRF2BFlags, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRUPSMode, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRFPNlf, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mCRUnpackSize, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eR, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eUPSSign, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_ePackSign, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eDN, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSPm, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_mSRSS0, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRF2BMask_eSRSSign, [OperandRegClass<0, mCRF2BMask>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRFPNlfMask, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRFPCnvFl2Fx, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLEm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDJ, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRCarry, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLRm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRPackSize, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRRnd, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRFifo_of, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRF2FMask, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eP, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRF2FFlags, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRFPCnvFx2Fl, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRF2IMask, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRF2BMask, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eVaddSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eS, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRFifo_uf, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRMCDEn, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRFPCnvFx2FlMask, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRMS0, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRUPS_of, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eM, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRSparse_of, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRSRSMode, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLCm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRSCDEn, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mLSm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDC, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRFPCnvFl2FxMask, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRSat, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRFPMask, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eUnpackSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRSRS_of, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRFPFlags, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRF2BFlags, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRUPSMode, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRFPNlf, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mCRUnpackSize, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eR, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eUPSSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_ePackSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eDN, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSPm, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_mSRSS0, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eVaddSign_eSRSSign, [OperandRegClass<0, eVaddSign>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRFPNlfMask, [OperandRegClass<0, eS>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRFPCnvFl2Fx, [OperandRegClass<0, eS>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mLEm, [OperandRegClass<0, eS>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eDJ, [OperandRegClass<0, eS>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRCarry, [OperandRegClass<0, eS>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mLRm, [OperandRegClass<0, eS>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRPackSize, [OperandRegClass<0, eS>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRRnd, [OperandRegClass<0, eS>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRFifo_of, [OperandRegClass<0, eS>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRF2FMask, [OperandRegClass<0, eS>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eP, [OperandRegClass<0, eS>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRF2FFlags, [OperandRegClass<0, eS>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRFPCnvFx2Fl, [OperandRegClass<0, eS>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRF2IMask, [OperandRegClass<0, eS>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRF2BMask, [OperandRegClass<0, eS>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eVaddSign, [OperandRegClass<0, eS>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eS, [OperandRegClass<0, eS>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRFifo_uf, [OperandRegClass<0, eS>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRMCDEn, [OperandRegClass<0, eS>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRFPCnvFx2FlMask, [OperandRegClass<0, eS>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRMS0, [OperandRegClass<0, eS>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRUPS_of, [OperandRegClass<0, eS>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eM, [OperandRegClass<0, eS>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRSparse_of, [OperandRegClass<0, eS>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRSRSMode, [OperandRegClass<0, eS>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mLCm, [OperandRegClass<0, eS>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRSCDEn, [OperandRegClass<0, eS>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mLSm, [OperandRegClass<0, eS>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eDC, [OperandRegClass<0, eS>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRFPCnvFl2FxMask, [OperandRegClass<0, eS>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRSat, [OperandRegClass<0, eS>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRFPMask, [OperandRegClass<0, eS>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eUnpackSign, [OperandRegClass<0, eS>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRSRS_of, [OperandRegClass<0, eS>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRFPFlags, [OperandRegClass<0, eS>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRF2BFlags, [OperandRegClass<0, eS>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRUPSMode, [OperandRegClass<0, eS>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRFPNlf, [OperandRegClass<0, eS>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mCRUnpackSize, [OperandRegClass<0, eS>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eR, [OperandRegClass<0, eS>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eUPSSign, [OperandRegClass<0, eS>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_ePackSign, [OperandRegClass<0, eS>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eDN, [OperandRegClass<0, eS>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSPm, [OperandRegClass<0, eS>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_mSRSS0, [OperandRegClass<0, eS>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eS_eSRSSign, [OperandRegClass<0, eS>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRFPNlfMask, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mLEm, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eDJ, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRCarry, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mLRm, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRPackSize, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRRnd, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRFifo_of, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRF2FMask, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eP, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRF2FFlags, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRF2IMask, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRF2BMask, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eVaddSign, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eS, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRFifo_uf, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRMCDEn, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRMS0, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRUPS_of, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eM, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRSparse_of, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRSRSMode, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mLCm, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRSCDEn, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mLSm, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eDC, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRSat, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRFPMask, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eUnpackSign, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRSRS_of, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRFPFlags, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRF2BFlags, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRUPSMode, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRFPNlf, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mCRUnpackSize, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eR, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eUPSSign, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_ePackSign, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eDN, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSPm, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_mSRSS0, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFifo_uf_eSRSSign, [OperandRegClass<0, mSRFifo_uf>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRFPNlfMask, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mLEm, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eDJ, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRCarry, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mLRm, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRPackSize, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRRnd, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRFifo_of, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRF2FMask, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eP, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRF2FFlags, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRF2IMask, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRF2BMask, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eVaddSign, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eS, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRFifo_uf, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRMCDEn, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRMS0, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRUPS_of, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eM, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRSparse_of, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRSRSMode, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mLCm, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRSCDEn, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mLSm, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eDC, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRSat, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRFPMask, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eUnpackSign, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRSRS_of, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRFPFlags, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRF2BFlags, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRUPSMode, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRFPNlf, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mCRUnpackSize, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eR, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eUPSSign, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_ePackSign, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eDN, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSPm, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_mSRSS0, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRMCDEn_eSRSSign, [OperandRegClass<0, mCRMCDEn>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRFPNlfMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mLEm, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eDJ, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRCarry, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mLRm, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRPackSize, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRRnd, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRFifo_of, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRF2FMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eP, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRF2FFlags, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRF2IMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRF2BMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eVaddSign, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eS, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRFifo_uf, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRMCDEn, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRMS0, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRUPS_of, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eM, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRSparse_of, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRSRSMode, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mLCm, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRSCDEn, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mLSm, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eDC, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRSat, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRFPMask, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eUnpackSign, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRSRS_of, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRFPFlags, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRF2BFlags, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRUPSMode, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRFPNlf, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mCRUnpackSize, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eR, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eUPSSign, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_ePackSign, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eDN, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSPm, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_mSRSS0, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFx2FlMask_eSRSSign, [OperandRegClass<0, mCRFPCnvFx2FlMask>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRFPNlfMask, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mLEm, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eDJ, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRCarry, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mLRm, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRPackSize, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRRnd, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRFifo_of, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRF2FMask, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eP, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRF2FFlags, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRF2IMask, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRF2BMask, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eVaddSign, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eS, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRFifo_uf, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRMCDEn, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRMS0, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRUPS_of, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eM, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRSparse_of, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRSRSMode, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mLCm, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRSCDEn, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mLSm, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eDC, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRSat, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRFPMask, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eUnpackSign, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRSRS_of, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRFPFlags, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRF2BFlags, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRUPSMode, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRFPNlf, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mCRUnpackSize, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eR, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eUPSSign, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_ePackSign, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eDN, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSPm, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_mSRSS0, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRMS0_eSRSSign, [OperandRegClass<0, mSRMS0>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRFPNlfMask, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mLEm, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eDJ, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRCarry, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mLRm, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRPackSize, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRRnd, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRFifo_of, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRF2FMask, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eP, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRF2FFlags, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRF2IMask, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRF2BMask, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eVaddSign, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eS, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRFifo_uf, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRMCDEn, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRMS0, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRUPS_of, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eM, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRSparse_of, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRSRSMode, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mLCm, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRSCDEn, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mLSm, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eDC, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRSat, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRFPMask, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eUnpackSign, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRSRS_of, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRFPFlags, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRF2BFlags, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRUPSMode, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRFPNlf, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mCRUnpackSize, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eR, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eUPSSign, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_ePackSign, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eDN, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSPm, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_mSRSS0, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRUPS_of_eSRSSign, [OperandRegClass<0, mSRUPS_of>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRFPNlfMask, [OperandRegClass<0, eM>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRFPCnvFl2Fx, [OperandRegClass<0, eM>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mLEm, [OperandRegClass<0, eM>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eDJ, [OperandRegClass<0, eM>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRCarry, [OperandRegClass<0, eM>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mLRm, [OperandRegClass<0, eM>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRPackSize, [OperandRegClass<0, eM>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRRnd, [OperandRegClass<0, eM>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRFifo_of, [OperandRegClass<0, eM>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRF2FMask, [OperandRegClass<0, eM>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eP, [OperandRegClass<0, eM>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRF2FFlags, [OperandRegClass<0, eM>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRFPCnvFx2Fl, [OperandRegClass<0, eM>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRF2IMask, [OperandRegClass<0, eM>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRF2BMask, [OperandRegClass<0, eM>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eVaddSign, [OperandRegClass<0, eM>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eS, [OperandRegClass<0, eM>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRFifo_uf, [OperandRegClass<0, eM>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRMCDEn, [OperandRegClass<0, eM>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRFPCnvFx2FlMask, [OperandRegClass<0, eM>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRMS0, [OperandRegClass<0, eM>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRUPS_of, [OperandRegClass<0, eM>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eM, [OperandRegClass<0, eM>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRSparse_of, [OperandRegClass<0, eM>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRSRSMode, [OperandRegClass<0, eM>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mLCm, [OperandRegClass<0, eM>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRSCDEn, [OperandRegClass<0, eM>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mLSm, [OperandRegClass<0, eM>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eDC, [OperandRegClass<0, eM>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRFPCnvFl2FxMask, [OperandRegClass<0, eM>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRSat, [OperandRegClass<0, eM>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRFPMask, [OperandRegClass<0, eM>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eUnpackSign, [OperandRegClass<0, eM>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRSRS_of, [OperandRegClass<0, eM>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRFPFlags, [OperandRegClass<0, eM>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRF2BFlags, [OperandRegClass<0, eM>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRUPSMode, [OperandRegClass<0, eM>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRFPNlf, [OperandRegClass<0, eM>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mCRUnpackSize, [OperandRegClass<0, eM>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eR, [OperandRegClass<0, eM>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eUPSSign, [OperandRegClass<0, eM>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_ePackSign, [OperandRegClass<0, eM>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eDN, [OperandRegClass<0, eM>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSPm, [OperandRegClass<0, eM>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_mSRSS0, [OperandRegClass<0, eM>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eM_eSRSSign, [OperandRegClass<0, eM>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRFPNlfMask, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mLEm, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eDJ, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRCarry, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mLRm, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRPackSize, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRRnd, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRFifo_of, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRF2FMask, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eP, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRF2FFlags, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRF2IMask, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRF2BMask, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eVaddSign, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eS, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRFifo_uf, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRMCDEn, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRMS0, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRUPS_of, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eM, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRSparse_of, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRSRSMode, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mLCm, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRSCDEn, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mLSm, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eDC, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRSat, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRFPMask, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eUnpackSign, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRSRS_of, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRFPFlags, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRF2BFlags, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRUPSMode, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRFPNlf, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mCRUnpackSize, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eR, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eUPSSign, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_ePackSign, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eDN, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSPm, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_mSRSS0, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSparse_of_eSRSSign, [OperandRegClass<0, mSRSparse_of>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRFPNlfMask, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mLEm, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eDJ, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRCarry, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mLRm, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRPackSize, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRRnd, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRFifo_of, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRF2FMask, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eP, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRF2FFlags, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRF2IMask, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRF2BMask, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eVaddSign, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eS, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRFifo_uf, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRMCDEn, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRMS0, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRUPS_of, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eM, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRSparse_of, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRSRSMode, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mLCm, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRSCDEn, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mLSm, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eDC, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRSat, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRFPMask, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eUnpackSign, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRSRS_of, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRFPFlags, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRF2BFlags, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRUPSMode, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRFPNlf, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mCRUnpackSize, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eR, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eUPSSign, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_ePackSign, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eDN, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSPm, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_mSRSS0, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSRSMode_eSRSSign, [OperandRegClass<0, mCRSRSMode>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRFPNlfMask, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRFPCnvFl2Fx, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mLEm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eDJ, [OperandRegClass<0, mLCm>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRCarry, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mLRm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRPackSize, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRRnd, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRFifo_of, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRF2FMask, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eP, [OperandRegClass<0, mLCm>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRF2FFlags, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRFPCnvFx2Fl, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRF2IMask, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRF2BMask, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eVaddSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eS, [OperandRegClass<0, mLCm>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRFifo_uf, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRMCDEn, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRFPCnvFx2FlMask, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRMS0, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRUPS_of, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eM, [OperandRegClass<0, mLCm>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRSparse_of, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRSRSMode, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mLCm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRSCDEn, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mLSm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eDC, [OperandRegClass<0, mLCm>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRFPCnvFl2FxMask, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRSat, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRFPMask, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eUnpackSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRSRS_of, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRFPFlags, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRF2BFlags, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRUPSMode, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRFPNlf, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mCRUnpackSize, [OperandRegClass<0, mLCm>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eR, [OperandRegClass<0, mLCm>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eUPSSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_ePackSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eDN, [OperandRegClass<0, mLCm>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSPm, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_mSRSS0, [OperandRegClass<0, mLCm>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLCm_eSRSSign, [OperandRegClass<0, mLCm>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRFPNlfMask, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mLEm, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eDJ, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRCarry, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mLRm, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRPackSize, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRRnd, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRFifo_of, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRF2FMask, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eP, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRF2FFlags, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRF2IMask, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRF2BMask, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eVaddSign, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eS, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRFifo_uf, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRMCDEn, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRMS0, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRUPS_of, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eM, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRSparse_of, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRSRSMode, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mLCm, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRSCDEn, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mLSm, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eDC, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRSat, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRFPMask, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eUnpackSign, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRSRS_of, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRFPFlags, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRF2BFlags, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRUPSMode, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRFPNlf, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mCRUnpackSize, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eR, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eUPSSign, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_ePackSign, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eDN, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSPm, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_mSRSS0, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSCDEn_eSRSSign, [OperandRegClass<0, mCRSCDEn>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRFPNlfMask, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRFPCnvFl2Fx, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mLEm, [OperandRegClass<0, mLSm>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eDJ, [OperandRegClass<0, mLSm>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRCarry, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mLRm, [OperandRegClass<0, mLSm>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRPackSize, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRRnd, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRFifo_of, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRF2FMask, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eP, [OperandRegClass<0, mLSm>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRF2FFlags, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRFPCnvFx2Fl, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRF2IMask, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRF2BMask, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eVaddSign, [OperandRegClass<0, mLSm>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eS, [OperandRegClass<0, mLSm>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRFifo_uf, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRMCDEn, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRFPCnvFx2FlMask, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRMS0, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRUPS_of, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eM, [OperandRegClass<0, mLSm>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRSparse_of, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRSRSMode, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mLCm, [OperandRegClass<0, mLSm>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRSCDEn, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mLSm, [OperandRegClass<0, mLSm>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eDC, [OperandRegClass<0, mLSm>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRFPCnvFl2FxMask, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRSat, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRFPMask, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eUnpackSign, [OperandRegClass<0, mLSm>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRSRS_of, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRFPFlags, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRF2BFlags, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRUPSMode, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRFPNlf, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mCRUnpackSize, [OperandRegClass<0, mLSm>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eR, [OperandRegClass<0, mLSm>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eUPSSign, [OperandRegClass<0, mLSm>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_ePackSign, [OperandRegClass<0, mLSm>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eDN, [OperandRegClass<0, mLSm>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSPm, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_mSRSS0, [OperandRegClass<0, mLSm>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mLSm_eSRSSign, [OperandRegClass<0, mLSm>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRFPNlfMask, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRFPCnvFl2Fx, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mLEm, [OperandRegClass<0, eDC>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eDJ, [OperandRegClass<0, eDC>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRCarry, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mLRm, [OperandRegClass<0, eDC>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRPackSize, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRRnd, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRFifo_of, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRF2FMask, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eP, [OperandRegClass<0, eDC>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRF2FFlags, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRFPCnvFx2Fl, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRF2IMask, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRF2BMask, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eVaddSign, [OperandRegClass<0, eDC>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eS, [OperandRegClass<0, eDC>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRFifo_uf, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRMCDEn, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRFPCnvFx2FlMask, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRMS0, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRUPS_of, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eM, [OperandRegClass<0, eDC>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRSparse_of, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRSRSMode, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mLCm, [OperandRegClass<0, eDC>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRSCDEn, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mLSm, [OperandRegClass<0, eDC>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eDC, [OperandRegClass<0, eDC>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRFPCnvFl2FxMask, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRSat, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRFPMask, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eUnpackSign, [OperandRegClass<0, eDC>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRSRS_of, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRFPFlags, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRF2BFlags, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRUPSMode, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRFPNlf, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mCRUnpackSize, [OperandRegClass<0, eDC>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eR, [OperandRegClass<0, eDC>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eUPSSign, [OperandRegClass<0, eDC>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_ePackSign, [OperandRegClass<0, eDC>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eDN, [OperandRegClass<0, eDC>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSPm, [OperandRegClass<0, eDC>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_mSRSS0, [OperandRegClass<0, eDC>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDC_eSRSSign, [OperandRegClass<0, eDC>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRFPNlfMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mLEm, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eDJ, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRCarry, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mLRm, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRPackSize, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRRnd, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRFifo_of, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRF2FMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eP, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRF2FFlags, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRF2IMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRF2BMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eVaddSign, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eS, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRFifo_uf, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRMCDEn, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRMS0, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRUPS_of, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eM, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRSparse_of, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRSRSMode, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mLCm, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRSCDEn, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mLSm, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eDC, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRSat, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRFPMask, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eUnpackSign, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRSRS_of, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRFPFlags, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRF2BFlags, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRUPSMode, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRFPNlf, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mCRUnpackSize, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eR, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eUPSSign, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_ePackSign, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eDN, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSPm, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_mSRSS0, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPCnvFl2FxMask_eSRSSign, [OperandRegClass<0, mCRFPCnvFl2FxMask>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRFPNlfMask, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mLEm, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eDJ, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRCarry, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mLRm, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRPackSize, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRRnd, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRFifo_of, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRF2FMask, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eP, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRF2FFlags, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRF2IMask, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRF2BMask, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eVaddSign, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eS, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRFifo_uf, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRMCDEn, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRMS0, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRUPS_of, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eM, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRSparse_of, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRSRSMode, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mLCm, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRSCDEn, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mLSm, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eDC, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRSat, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRFPMask, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eUnpackSign, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRSRS_of, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRFPFlags, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRF2BFlags, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRUPSMode, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRFPNlf, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mCRUnpackSize, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eR, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eUPSSign, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_ePackSign, [OperandRegClass<0, mCRSat>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eDN, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSPm, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_mSRSS0, [OperandRegClass<0, mCRSat>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRSat_eSRSSign, [OperandRegClass<0, mCRSat>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRFPNlfMask, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mLEm, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eDJ, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRCarry, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mLRm, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRPackSize, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRRnd, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRFifo_of, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRF2FMask, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eP, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRF2FFlags, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRF2IMask, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRF2BMask, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eVaddSign, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eS, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRFifo_uf, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRMCDEn, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRMS0, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRUPS_of, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eM, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRSparse_of, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRSRSMode, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mLCm, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRSCDEn, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mLSm, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eDC, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRSat, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRFPMask, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eUnpackSign, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRSRS_of, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRFPFlags, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRF2BFlags, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRUPSMode, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRFPNlf, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mCRUnpackSize, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eR, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eUPSSign, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_ePackSign, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eDN, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSPm, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_mSRSS0, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRFPMask_eSRSSign, [OperandRegClass<0, mCRFPMask>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRFPNlfMask, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRFPCnvFl2Fx, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLEm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDJ, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRCarry, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLRm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRPackSize, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRRnd, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRFifo_of, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRF2FMask, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eP, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRF2FFlags, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRFPCnvFx2Fl, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRF2IMask, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRF2BMask, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eVaddSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eS, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRFifo_uf, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRMCDEn, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRFPCnvFx2FlMask, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRMS0, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRUPS_of, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eM, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRSparse_of, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRSRSMode, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLCm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRSCDEn, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mLSm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDC, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRFPCnvFl2FxMask, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRSat, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRFPMask, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eUnpackSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRSRS_of, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRFPFlags, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRF2BFlags, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRUPSMode, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRFPNlf, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mCRUnpackSize, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eR, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eUPSSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_ePackSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eDN, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSPm, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_mSRSS0, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUnpackSign_eSRSSign, [OperandRegClass<0, eUnpackSign>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRFPNlfMask, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mLEm, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eDJ, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRCarry, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mLRm, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRPackSize, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRRnd, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRFifo_of, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRF2FMask, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eP, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRF2FFlags, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRF2IMask, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRF2BMask, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eVaddSign, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eS, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRFifo_uf, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRMCDEn, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRMS0, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRUPS_of, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eM, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRSparse_of, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRSRSMode, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mLCm, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRSCDEn, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mLSm, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eDC, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRSat, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRFPMask, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eUnpackSign, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRSRS_of, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRFPFlags, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRF2BFlags, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRUPSMode, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRFPNlf, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mCRUnpackSize, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eR, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eUPSSign, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_ePackSign, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eDN, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSPm, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_mSRSS0, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSRS_of_eSRSSign, [OperandRegClass<0, mSRSRS_of>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRFPNlfMask, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mLEm, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eDJ, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRCarry, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mLRm, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRPackSize, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRRnd, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRFifo_of, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRF2FMask, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eP, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRF2FFlags, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRF2IMask, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRF2BMask, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eVaddSign, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eS, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRFifo_uf, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRMCDEn, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRMS0, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRUPS_of, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eM, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRSparse_of, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRSRSMode, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mLCm, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRSCDEn, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mLSm, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eDC, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRSat, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRFPMask, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eUnpackSign, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRSRS_of, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRFPFlags, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRF2BFlags, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRUPSMode, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRFPNlf, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mCRUnpackSize, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eR, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eUPSSign, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_ePackSign, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eDN, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSPm, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_mSRSS0, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPFlags_eSRSSign, [OperandRegClass<0, mSRFPFlags>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRFPNlfMask, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mLEm, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eDJ, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRCarry, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mLRm, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRPackSize, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRRnd, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRFifo_of, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRF2FMask, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eP, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRF2FFlags, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRF2IMask, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRF2BMask, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eVaddSign, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eS, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRFifo_uf, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRMCDEn, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRMS0, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRUPS_of, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eM, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRSparse_of, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRSRSMode, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mLCm, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRSCDEn, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mLSm, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eDC, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRSat, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRFPMask, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eUnpackSign, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRSRS_of, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRFPFlags, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRF2BFlags, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRUPSMode, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRFPNlf, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mCRUnpackSize, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eR, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eUPSSign, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_ePackSign, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eDN, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSPm, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_mSRSS0, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRF2BFlags_eSRSSign, [OperandRegClass<0, mSRF2BFlags>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRFPNlfMask, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mLEm, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eDJ, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRCarry, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mLRm, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRPackSize, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRRnd, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRFifo_of, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRF2FMask, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eP, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRF2FFlags, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRF2IMask, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRF2BMask, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eVaddSign, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eS, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRFifo_uf, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRMCDEn, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRMS0, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRUPS_of, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eM, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRSparse_of, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRSRSMode, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mLCm, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRSCDEn, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mLSm, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eDC, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRSat, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRFPMask, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eUnpackSign, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRSRS_of, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRFPFlags, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRF2BFlags, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRUPSMode, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRFPNlf, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mCRUnpackSize, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eR, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eUPSSign, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_ePackSign, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eDN, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSPm, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_mSRSS0, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUPSMode_eSRSSign, [OperandRegClass<0, mCRUPSMode>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRFPNlfMask, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mLEm, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eDJ, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRCarry, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mLRm, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRPackSize, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRRnd, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRFifo_of, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRF2FMask, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eP, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRF2FFlags, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRF2IMask, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRF2BMask, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eVaddSign, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eS, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRFifo_uf, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRMCDEn, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRMS0, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRUPS_of, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eM, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRSparse_of, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRSRSMode, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mLCm, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRSCDEn, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mLSm, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eDC, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRSat, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRFPMask, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eUnpackSign, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRSRS_of, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRFPFlags, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRF2BFlags, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRUPSMode, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRFPNlf, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mCRUnpackSize, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eR, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eUPSSign, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_ePackSign, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eDN, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSPm, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_mSRSS0, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRFPNlf_eSRSSign, [OperandRegClass<0, mSRFPNlf>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRFPNlfMask, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRFPCnvFl2Fx, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mLEm, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eDJ, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRCarry, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mLRm, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRPackSize, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRRnd, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRFifo_of, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRF2FMask, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eP, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRF2FFlags, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRFPCnvFx2Fl, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRF2IMask, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRF2BMask, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eVaddSign, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eS, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRFifo_uf, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRMCDEn, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRFPCnvFx2FlMask, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRMS0, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRUPS_of, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eM, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRSparse_of, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRSRSMode, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mLCm, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRSCDEn, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mLSm, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eDC, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRFPCnvFl2FxMask, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRSat, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRFPMask, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eUnpackSign, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRSRS_of, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRFPFlags, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRF2BFlags, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRUPSMode, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRFPNlf, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mCRUnpackSize, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eR, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eUPSSign, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_ePackSign, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eDN, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSPm, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_mSRSS0, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mCRUnpackSize_eSRSSign, [OperandRegClass<0, mCRUnpackSize>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRFPNlfMask, [OperandRegClass<0, eR>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRFPCnvFl2Fx, [OperandRegClass<0, eR>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mLEm, [OperandRegClass<0, eR>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eDJ, [OperandRegClass<0, eR>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRCarry, [OperandRegClass<0, eR>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mLRm, [OperandRegClass<0, eR>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRPackSize, [OperandRegClass<0, eR>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRRnd, [OperandRegClass<0, eR>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRFifo_of, [OperandRegClass<0, eR>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRF2FMask, [OperandRegClass<0, eR>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eP, [OperandRegClass<0, eR>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRF2FFlags, [OperandRegClass<0, eR>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRFPCnvFx2Fl, [OperandRegClass<0, eR>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRF2IMask, [OperandRegClass<0, eR>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRF2BMask, [OperandRegClass<0, eR>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eVaddSign, [OperandRegClass<0, eR>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eS, [OperandRegClass<0, eR>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRFifo_uf, [OperandRegClass<0, eR>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRMCDEn, [OperandRegClass<0, eR>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRFPCnvFx2FlMask, [OperandRegClass<0, eR>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRMS0, [OperandRegClass<0, eR>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRUPS_of, [OperandRegClass<0, eR>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eM, [OperandRegClass<0, eR>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRSparse_of, [OperandRegClass<0, eR>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRSRSMode, [OperandRegClass<0, eR>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mLCm, [OperandRegClass<0, eR>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRSCDEn, [OperandRegClass<0, eR>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mLSm, [OperandRegClass<0, eR>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eDC, [OperandRegClass<0, eR>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRFPCnvFl2FxMask, [OperandRegClass<0, eR>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRSat, [OperandRegClass<0, eR>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRFPMask, [OperandRegClass<0, eR>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eUnpackSign, [OperandRegClass<0, eR>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRSRS_of, [OperandRegClass<0, eR>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRFPFlags, [OperandRegClass<0, eR>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRF2BFlags, [OperandRegClass<0, eR>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRUPSMode, [OperandRegClass<0, eR>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRFPNlf, [OperandRegClass<0, eR>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mCRUnpackSize, [OperandRegClass<0, eR>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eR, [OperandRegClass<0, eR>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eUPSSign, [OperandRegClass<0, eR>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_ePackSign, [OperandRegClass<0, eR>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eDN, [OperandRegClass<0, eR>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSPm, [OperandRegClass<0, eR>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_mSRSS0, [OperandRegClass<0, eR>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eR_eSRSSign, [OperandRegClass<0, eR>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRFPNlfMask, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRFPCnvFl2Fx, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLEm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDJ, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRCarry, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLRm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRPackSize, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRRnd, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRFifo_of, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRF2FMask, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eP, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRF2FFlags, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRFPCnvFx2Fl, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRF2IMask, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRF2BMask, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eVaddSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eS, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRFifo_uf, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRMCDEn, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRFPCnvFx2FlMask, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRMS0, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRUPS_of, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eM, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRSparse_of, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRSRSMode, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLCm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRSCDEn, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mLSm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDC, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRFPCnvFl2FxMask, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRSat, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRFPMask, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eUnpackSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRSRS_of, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRFPFlags, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRF2BFlags, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRUPSMode, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRFPNlf, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mCRUnpackSize, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eR, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eUPSSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_ePackSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eDN, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSPm, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_mSRSS0, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eUPSSign_eSRSSign, [OperandRegClass<0, eUPSSign>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRFPNlfMask, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRFPCnvFl2Fx, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLEm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eDJ, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRCarry, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLRm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRPackSize, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRRnd, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRFifo_of, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRF2FMask, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eP, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRF2FFlags, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRFPCnvFx2Fl, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRF2IMask, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRF2BMask, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eVaddSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eS, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRFifo_uf, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRMCDEn, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRFPCnvFx2FlMask, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRMS0, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRUPS_of, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eM, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRSparse_of, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRSRSMode, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLCm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRSCDEn, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mLSm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eDC, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRFPCnvFl2FxMask, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRSat, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRFPMask, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eUnpackSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRSRS_of, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRFPFlags, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRF2BFlags, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRUPSMode, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRFPNlf, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mCRUnpackSize, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eR, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eUPSSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_ePackSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eDN, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSPm, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_mSRSS0, [OperandRegClass<0, ePackSign>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_ePackSign_eSRSSign, [OperandRegClass<0, ePackSign>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRFPNlfMask, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRFPCnvFl2Fx, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mLEm, [OperandRegClass<0, eDN>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eDJ, [OperandRegClass<0, eDN>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRCarry, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mLRm, [OperandRegClass<0, eDN>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRPackSize, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRRnd, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRFifo_of, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRF2FMask, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eP, [OperandRegClass<0, eDN>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRF2FFlags, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRFPCnvFx2Fl, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRF2IMask, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRF2BMask, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eVaddSign, [OperandRegClass<0, eDN>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eS, [OperandRegClass<0, eDN>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRFifo_uf, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRMCDEn, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRFPCnvFx2FlMask, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRMS0, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRUPS_of, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eM, [OperandRegClass<0, eDN>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRSparse_of, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRSRSMode, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mLCm, [OperandRegClass<0, eDN>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRSCDEn, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mLSm, [OperandRegClass<0, eDN>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eDC, [OperandRegClass<0, eDN>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRFPCnvFl2FxMask, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRSat, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRFPMask, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eUnpackSign, [OperandRegClass<0, eDN>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRSRS_of, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRFPFlags, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRF2BFlags, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRUPSMode, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRFPNlf, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mCRUnpackSize, [OperandRegClass<0, eDN>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eR, [OperandRegClass<0, eDN>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eUPSSign, [OperandRegClass<0, eDN>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_ePackSign, [OperandRegClass<0, eDN>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eDN, [OperandRegClass<0, eDN>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSPm, [OperandRegClass<0, eDN>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_mSRSS0, [OperandRegClass<0, eDN>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eDN_eSRSSign, [OperandRegClass<0, eDN>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRFPNlfMask, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRFPCnvFl2Fx, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mLEm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eDJ, [OperandRegClass<0, mSPm>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRCarry, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mLRm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRPackSize, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRRnd, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRFifo_of, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRF2FMask, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eP, [OperandRegClass<0, mSPm>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRF2FFlags, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRFPCnvFx2Fl, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRF2IMask, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRF2BMask, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eVaddSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eS, [OperandRegClass<0, mSPm>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRFifo_uf, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRMCDEn, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRMS0, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRUPS_of, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eM, [OperandRegClass<0, mSPm>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRSparse_of, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRSRSMode, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mLCm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRSCDEn, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mLSm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eDC, [OperandRegClass<0, mSPm>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRSat, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRFPMask, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eUnpackSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRSRS_of, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRFPFlags, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRF2BFlags, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRUPSMode, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRFPNlf, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mCRUnpackSize, [OperandRegClass<0, mSPm>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eR, [OperandRegClass<0, mSPm>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eUPSSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_ePackSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eDN, [OperandRegClass<0, mSPm>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSPm, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_mSRSS0, [OperandRegClass<0, mSPm>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSPm_eSRSSign, [OperandRegClass<0, mSPm>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRFPNlfMask, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRFPCnvFl2Fx, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mLEm, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eDJ, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRCarry, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mLRm, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRPackSize, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRRnd, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRFifo_of, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRF2FMask, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eP, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRF2FFlags, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRFPCnvFx2Fl, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRF2IMask, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRF2BMask, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eVaddSign, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eS, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRFifo_uf, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRMCDEn, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRFPCnvFx2FlMask, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRMS0, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRUPS_of, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eM, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRSparse_of, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRSRSMode, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mLCm, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRSCDEn, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mLSm, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eDC, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRFPCnvFl2FxMask, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRSat, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRFPMask, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eUnpackSign, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRSRS_of, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRFPFlags, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRF2BFlags, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRUPSMode, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRFPNlf, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mCRUnpackSize, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eR, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eUPSSign, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_ePackSign, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eDN, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSPm, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_mSRSS0, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_mSRSS0_eSRSSign, [OperandRegClass<0, mSRSS0>, OperandRegClass<1, eSRSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRFPNlfMask, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRFPCnvFl2Fx, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLEm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDJ, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRCarry, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLRm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRPackSize, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRRnd, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRFifo_of, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRF2FMask, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eP, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRF2FFlags, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRFPCnvFx2Fl, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRF2IMask, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRF2BMask, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eVaddSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eS, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRFifo_uf, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRMCDEn, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRFPCnvFx2FlMask, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRMS0, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRUPS_of, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eM, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRSparse_of, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRSRSMode, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLCm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRSCDEn, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mLSm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDC, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRFPCnvFl2FxMask, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRSat, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRFPMask, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eUnpackSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRSRS_of, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRFPFlags, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRF2BFlags, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRUPSMode, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRFPNlf, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mCRUnpackSize, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eR, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eUPSSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_ePackSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eDN, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSPm, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_mSRSS0, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_alu_mv_mv_mv_scl_eSRSSign_eSRSSign, [OperandRegClass<0, eSRSSign>, OperandRegClass<1, eSRSSign>]>] in
def MOV_alu_mv_mv_mv_scl : AIE2P_inst_mv_instr32 <(outs OP_mMvSclDst:$dst), (ins OP_mMvSclSrc:$src), "mov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_scl
bits<7> dst;
bits<7> src;

let mv = {dst, src, 0b00000111};
}

let Itinerary = II_MOV_lda, hasSideEffects = true, Defs = [srSS0] in
def MOV_lda : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins), "mov", " $dst, ss">{
// id: me__instr128_or__lda__mv_ss2scl__mSStream__mSStream_b__mRa
bits<5> dst;

let lda = {0b0, dontcare{8-1}, dst, 0b000111};
}

let Itinerary = II_MOV_st_mMStream_tlast_imm, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_imm_eM, [OperandRegClass<0, eM>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_st_mMStream_tlast_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mSclMS:$src), "mov", " ms, $src">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__noTlast
bits<7> src;

let st = {0b0, 0b0, 0b0, 0b101, dontcare{3-1}, src, 0b0100};
}

let Itinerary = II_MOV_st_mMStream_tlast_reg, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_st_mMStream_tlast_reg_eM, [OperandRegClass<0, eM>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_st_mMStream_tlast_reg : AIE2P_inst_st_instr32 <(outs), (ins OP_mSclMS:$src, mR28_tlast:$dst), "mov", " ms, $src, r28">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
bits<7> src;

let st = {0b0, 0b1, dontcare{1}, 0b101, dontcare{3-1}, src, 0b0100};
}

let Itinerary = II_MOV_d1, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d1_mSRF2IFlags, [OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_d1_mCRFPNlfMask, [OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_d1_mSRFPCnvFl2Fx, [OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_d1_mLEm, [OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d1_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d1_mSRCarry, [OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_d1_mLRm, [OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d1_mCRPackSize, [OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_d1_mCRRnd, [OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_d1_mSRFifo_of, [OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_d1_mCRF2FMask, [OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_d1_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d1_mSRF2FFlags, [OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_d1_mSRFPCnvFx2Fl, [OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_d1_mCRF2IMask, [OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_d1_mCRF2BMask, [OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_d1_eVaddSign, [OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d1_eS, [OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d1_mSRFifo_uf, [OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_d1_mCRMCDEn, [OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_d1_mCRFPCnvFx2FlMask, [OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_d1_mSRMS0, [OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_d1_mSRUPS_of, [OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_d1_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d1_mSRSparse_of, [OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_d1_mCRSRSMode, [OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_d1_mLCm, [OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d1_mCRSCDEn, [OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_d1_mLSm, [OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d1_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d1_mCRFPCnvFl2FxMask, [OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_d1_mCRSat, [OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_d1_mCRFPMask, [OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_d1_eUnpackSign, [OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d1_mSRSRS_of, [OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_d1_mSRFPFlags, [OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_d1_mSRF2BFlags, [OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_d1_mCRUPSMode, [OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_d1_mSRFPNlf, [OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_d1_mCRUnpackSize, [OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_d1_eR, [OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_d1_eUPSSign, [OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d1_ePackSign, [OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d1_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d1_mSPm, [OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d1_mSRSS0, [OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_d1_eSRSSign, [OperandRegClass<1, eSRSSign>]>] in
def MOV_d1 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mMvSclSrc:$src), "mov.d1", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay1__mRm
bits<5> dst;
bits<7> src;

let mv = {dst, 0b00, src, 0b0, 0b0101011};
}

let Itinerary = II_MOV_d2, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d2_mSRF2IFlags, [OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_d2_mCRFPNlfMask, [OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_d2_mSRFPCnvFl2Fx, [OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_d2_mLEm, [OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d2_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d2_mSRCarry, [OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_d2_mLRm, [OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d2_mCRPackSize, [OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_d2_mCRRnd, [OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_d2_mSRFifo_of, [OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_d2_mCRF2FMask, [OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_d2_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d2_mSRF2FFlags, [OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_d2_mSRFPCnvFx2Fl, [OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_d2_mCRF2IMask, [OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_d2_mCRF2BMask, [OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_d2_eVaddSign, [OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d2_eS, [OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d2_mSRFifo_uf, [OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_d2_mCRMCDEn, [OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_d2_mCRFPCnvFx2FlMask, [OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_d2_mSRMS0, [OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_d2_mSRUPS_of, [OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_d2_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d2_mSRSparse_of, [OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_d2_mCRSRSMode, [OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_d2_mLCm, [OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d2_mCRSCDEn, [OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_d2_mLSm, [OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d2_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d2_mCRFPCnvFl2FxMask, [OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_d2_mCRSat, [OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_d2_mCRFPMask, [OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_d2_eUnpackSign, [OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d2_mSRSRS_of, [OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_d2_mSRFPFlags, [OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_d2_mSRF2BFlags, [OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_d2_mCRUPSMode, [OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_d2_mSRFPNlf, [OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_d2_mCRUnpackSize, [OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_d2_eR, [OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_d2_eUPSSign, [OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d2_ePackSign, [OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d2_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d2_mSPm, [OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d2_mSRSS0, [OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_d2_eSRSSign, [OperandRegClass<1, eSRSSign>]>] in
def MOV_d2 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mMvSclSrc:$src), "mov.d2", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay2__mRm
bits<5> dst;
bits<7> src;

let mv = {dst, 0b00, src, 0b1, 0b0101011};
}

let Itinerary = II_MOV_d3, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d3_mSRF2IFlags, [OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_d3_mCRFPNlfMask, [OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_d3_mSRFPCnvFl2Fx, [OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_d3_mLEm, [OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d3_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d3_mSRCarry, [OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_d3_mLRm, [OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d3_mCRPackSize, [OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_d3_mCRRnd, [OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_d3_mSRFifo_of, [OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_d3_mCRF2FMask, [OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_d3_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d3_mSRF2FFlags, [OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_d3_mSRFPCnvFx2Fl, [OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_d3_mCRF2IMask, [OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_d3_mCRF2BMask, [OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_d3_eVaddSign, [OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d3_eS, [OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d3_mSRFifo_uf, [OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_d3_mCRMCDEn, [OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_d3_mCRFPCnvFx2FlMask, [OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_d3_mSRMS0, [OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_d3_mSRUPS_of, [OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_d3_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d3_mSRSparse_of, [OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_d3_mCRSRSMode, [OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_d3_mLCm, [OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d3_mCRSCDEn, [OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_d3_mLSm, [OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d3_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d3_mCRFPCnvFl2FxMask, [OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_d3_mCRSat, [OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_d3_mCRFPMask, [OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_d3_eUnpackSign, [OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d3_mSRSRS_of, [OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_d3_mSRFPFlags, [OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_d3_mSRF2BFlags, [OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_d3_mCRUPSMode, [OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_d3_mSRFPNlf, [OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_d3_mCRUnpackSize, [OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_d3_eR, [OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_d3_eUPSSign, [OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d3_ePackSign, [OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d3_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d3_mSPm, [OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d3_mSRSS0, [OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_d3_eSRSSign, [OperandRegClass<1, eSRSSign>]>] in
def MOV_d3 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mMvSclSrc:$src), "mov.d3", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay3__mRm
bits<5> dst;
bits<7> src;

let mv = {dst, 0b01, src, 0b0, 0b0101011};
}

let Itinerary = II_MOV_d4, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d4_mSRF2IFlags, [OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_d4_mCRFPNlfMask, [OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_d4_mSRFPCnvFl2Fx, [OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_d4_mLEm, [OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d4_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d4_mSRCarry, [OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_d4_mLRm, [OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d4_mCRPackSize, [OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_d4_mCRRnd, [OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_d4_mSRFifo_of, [OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_d4_mCRF2FMask, [OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_d4_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d4_mSRF2FFlags, [OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_d4_mSRFPCnvFx2Fl, [OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_d4_mCRF2IMask, [OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_d4_mCRF2BMask, [OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_d4_eVaddSign, [OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d4_eS, [OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d4_mSRFifo_uf, [OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_d4_mCRMCDEn, [OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_d4_mCRFPCnvFx2FlMask, [OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_d4_mSRMS0, [OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_d4_mSRUPS_of, [OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_d4_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d4_mSRSparse_of, [OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_d4_mCRSRSMode, [OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_d4_mLCm, [OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d4_mCRSCDEn, [OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_d4_mLSm, [OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d4_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d4_mCRFPCnvFl2FxMask, [OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_d4_mCRSat, [OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_d4_mCRFPMask, [OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_d4_eUnpackSign, [OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d4_mSRSRS_of, [OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_d4_mSRFPFlags, [OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_d4_mSRF2BFlags, [OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_d4_mCRUPSMode, [OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_d4_mSRFPNlf, [OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_d4_mCRUnpackSize, [OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_d4_eR, [OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_d4_eUPSSign, [OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d4_ePackSign, [OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d4_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d4_mSPm, [OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d4_mSRSS0, [OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_d4_eSRSSign, [OperandRegClass<1, eSRSSign>]>] in
def MOV_d4 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mMvSclSrc:$src), "mov.d4", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay4__mRm
bits<5> dst;
bits<7> src;

let mv = {dst, 0b01, src, 0b1, 0b0101011};
}

let Itinerary = II_MOV_d5, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d5_mSRF2IFlags, [OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_d5_mCRFPNlfMask, [OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_d5_mSRFPCnvFl2Fx, [OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_d5_mLEm, [OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d5_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d5_mSRCarry, [OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_d5_mLRm, [OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d5_mCRPackSize, [OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_d5_mCRRnd, [OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_d5_mSRFifo_of, [OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_d5_mCRF2FMask, [OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_d5_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d5_mSRF2FFlags, [OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_d5_mSRFPCnvFx2Fl, [OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_d5_mCRF2IMask, [OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_d5_mCRF2BMask, [OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_d5_eVaddSign, [OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d5_eS, [OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d5_mSRFifo_uf, [OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_d5_mCRMCDEn, [OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_d5_mCRFPCnvFx2FlMask, [OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_d5_mSRMS0, [OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_d5_mSRUPS_of, [OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_d5_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d5_mSRSparse_of, [OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_d5_mCRSRSMode, [OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_d5_mLCm, [OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d5_mCRSCDEn, [OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_d5_mLSm, [OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d5_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d5_mCRFPCnvFl2FxMask, [OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_d5_mCRSat, [OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_d5_mCRFPMask, [OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_d5_eUnpackSign, [OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d5_mSRSRS_of, [OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_d5_mSRFPFlags, [OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_d5_mSRF2BFlags, [OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_d5_mCRUPSMode, [OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_d5_mSRFPNlf, [OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_d5_mCRUnpackSize, [OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_d5_eR, [OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_d5_eUPSSign, [OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d5_ePackSign, [OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d5_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d5_mSPm, [OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d5_mSRSS0, [OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_d5_eSRSSign, [OperandRegClass<1, eSRSSign>]>] in
def MOV_d5 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mMvSclSrc:$src), "mov.d5", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay5__mRm
bits<5> dst;
bits<7> src;

let mv = {dst, 0b10, src, 0b0, 0b0101011};
}

let Itinerary = II_MOV_d6, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_d6_mSRF2IFlags, [OperandRegClass<1, mSRF2IFlags>]>,
ItinRegClassPair<II_MOV_d6_mCRFPNlfMask, [OperandRegClass<1, mCRFPNlfMask>]>,
ItinRegClassPair<II_MOV_d6_mSRFPCnvFl2Fx, [OperandRegClass<1, mSRFPCnvFl2Fx>]>,
ItinRegClassPair<II_MOV_d6_mLEm, [OperandRegClass<1, mLEm>]>,
ItinRegClassPair<II_MOV_d6_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_MOV_d6_mSRCarry, [OperandRegClass<1, mSRCarry>]>,
ItinRegClassPair<II_MOV_d6_mLRm, [OperandRegClass<1, mLRm>]>,
ItinRegClassPair<II_MOV_d6_mCRPackSize, [OperandRegClass<1, mCRPackSize>]>,
ItinRegClassPair<II_MOV_d6_mCRRnd, [OperandRegClass<1, mCRRnd>]>,
ItinRegClassPair<II_MOV_d6_mSRFifo_of, [OperandRegClass<1, mSRFifo_of>]>,
ItinRegClassPair<II_MOV_d6_mCRF2FMask, [OperandRegClass<1, mCRF2FMask>]>,
ItinRegClassPair<II_MOV_d6_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_MOV_d6_mSRF2FFlags, [OperandRegClass<1, mSRF2FFlags>]>,
ItinRegClassPair<II_MOV_d6_mSRFPCnvFx2Fl, [OperandRegClass<1, mSRFPCnvFx2Fl>]>,
ItinRegClassPair<II_MOV_d6_mCRF2IMask, [OperandRegClass<1, mCRF2IMask>]>,
ItinRegClassPair<II_MOV_d6_mCRF2BMask, [OperandRegClass<1, mCRF2BMask>]>,
ItinRegClassPair<II_MOV_d6_eVaddSign, [OperandRegClass<1, eVaddSign>]>,
ItinRegClassPair<II_MOV_d6_eS, [OperandRegClass<1, eS>]>,
ItinRegClassPair<II_MOV_d6_mSRFifo_uf, [OperandRegClass<1, mSRFifo_uf>]>,
ItinRegClassPair<II_MOV_d6_mCRMCDEn, [OperandRegClass<1, mCRMCDEn>]>,
ItinRegClassPair<II_MOV_d6_mCRFPCnvFx2FlMask, [OperandRegClass<1, mCRFPCnvFx2FlMask>]>,
ItinRegClassPair<II_MOV_d6_mSRMS0, [OperandRegClass<1, mSRMS0>]>,
ItinRegClassPair<II_MOV_d6_mSRUPS_of, [OperandRegClass<1, mSRUPS_of>]>,
ItinRegClassPair<II_MOV_d6_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_MOV_d6_mSRSparse_of, [OperandRegClass<1, mSRSparse_of>]>,
ItinRegClassPair<II_MOV_d6_mCRSRSMode, [OperandRegClass<1, mCRSRSMode>]>,
ItinRegClassPair<II_MOV_d6_mLCm, [OperandRegClass<1, mLCm>]>,
ItinRegClassPair<II_MOV_d6_mCRSCDEn, [OperandRegClass<1, mCRSCDEn>]>,
ItinRegClassPair<II_MOV_d6_mLSm, [OperandRegClass<1, mLSm>]>,
ItinRegClassPair<II_MOV_d6_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_MOV_d6_mCRFPCnvFl2FxMask, [OperandRegClass<1, mCRFPCnvFl2FxMask>]>,
ItinRegClassPair<II_MOV_d6_mCRSat, [OperandRegClass<1, mCRSat>]>,
ItinRegClassPair<II_MOV_d6_mCRFPMask, [OperandRegClass<1, mCRFPMask>]>,
ItinRegClassPair<II_MOV_d6_eUnpackSign, [OperandRegClass<1, eUnpackSign>]>,
ItinRegClassPair<II_MOV_d6_mSRSRS_of, [OperandRegClass<1, mSRSRS_of>]>,
ItinRegClassPair<II_MOV_d6_mSRFPFlags, [OperandRegClass<1, mSRFPFlags>]>,
ItinRegClassPair<II_MOV_d6_mSRF2BFlags, [OperandRegClass<1, mSRF2BFlags>]>,
ItinRegClassPair<II_MOV_d6_mCRUPSMode, [OperandRegClass<1, mCRUPSMode>]>,
ItinRegClassPair<II_MOV_d6_mSRFPNlf, [OperandRegClass<1, mSRFPNlf>]>,
ItinRegClassPair<II_MOV_d6_mCRUnpackSize, [OperandRegClass<1, mCRUnpackSize>]>,
ItinRegClassPair<II_MOV_d6_eR, [OperandRegClass<1, eR>]>,
ItinRegClassPair<II_MOV_d6_eUPSSign, [OperandRegClass<1, eUPSSign>]>,
ItinRegClassPair<II_MOV_d6_ePackSign, [OperandRegClass<1, ePackSign>]>,
ItinRegClassPair<II_MOV_d6_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_MOV_d6_mSPm, [OperandRegClass<1, mSPm>]>,
ItinRegClassPair<II_MOV_d6_mSRSS0, [OperandRegClass<1, mSRSS0>]>,
ItinRegClassPair<II_MOV_d6_eSRSSign, [OperandRegClass<1, eSRSSign>]>] in
def MOV_d6 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mMvSclSrc:$src), "mov.d6", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_delay__Delay6__mRm
bits<5> dst;
bits<7> src;

let mv = {dst, 0b10, src, 0b1, 0b0101011};
}

let Itinerary = II_MOV_nb_lda, hasSideEffects = true, Defs = [srSS0] in
def MOV_nb_lda : AIE2P_inst_lda_instr32 <(outs eR:$dst), (ins), "mov.nb", " $dst, ss">{
// id: me__instr128_or__lda__mv_ss2scl__mSStream__mSStream_nb__mRa
bits<5> dst;

let lda = {0b1, dontcare{8-1}, dst, 0b000111};
}

let Itinerary = II_MOV_nb_st_mMStream_tlast_imm, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_imm_eM, [OperandRegClass<0, eM>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_nb_st_mMStream_tlast_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mSclMS:$src), "mov.nb", " ms, $src">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__noTlast
bits<7> src;

let st = {0b1, 0b0, 0b0, 0b101, dontcare{3-1}, src, 0b0100};
}

let Itinerary = II_MOV_nb_st_mMStream_tlast_reg, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_nb_st_mMStream_tlast_reg_eM, [OperandRegClass<0, eM>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_nb_st_mMStream_tlast_reg : AIE2P_inst_st_instr32 <(outs), (ins OP_mSclMS:$src, mR28_tlast:$dst), "mov.nb", " ms, $src, r28">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_reg__mR28_tlast
bits<7> src;

let st = {0b1, 0b1, dontcare{1}, 0b101, dontcare{3-1}, src, 0b0100};
}

let Itinerary = II_MOV_nb_tlast, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_nb_tlast_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_nb_tlast_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_MOV_nb_tlast_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_nb_tlast_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_nb_tlast_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_nb_tlast_eM, [OperandRegClass<0, eM>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_nb_tlast : AIE2P_inst_st_instr32 <(outs), (ins OP_mSclMS:$src), "mov.nb.tlast", " ms, $src">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_nb__mMStream_tlast__mMStream_tlast_imm__doTlast
bits<7> src;

let st = {0b1, 0b0, 0b1, 0b101, dontcare{3-1}, src, 0b0100};
}

let Itinerary = II_MOV_tlast, ItineraryRegPairs = [
ItinRegClassPair<II_MOV_tlast_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_MOV_tlast_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_MOV_tlast_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_MOV_tlast_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_MOV_tlast_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_MOV_tlast_eM, [OperandRegClass<0, eM>]>], hasSideEffects = true, Defs = [srMS0] in
def MOV_tlast : AIE2P_inst_st_instr32 <(outs), (ins OP_mSclMS:$src), "mov.tlast", " ms, $src">{
// id: me__instr128_or__st__st_streams__mv_scl2ms__mMStream__mMStream_b__mMStream_tlast__mMStream_tlast_imm__doTlast
bits<7> src;

let st = {0b0, 0b0, 0b1, 0b101, dontcare{3-1}, src, 0b0100};
}

let Itinerary = II_MSC, Constraints = "$a0 = $d0" in
def MSC : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$a0, eR:$s0, eR:$s1), "msc", "$d0, $a0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mac_r_rrr__mRa__mRz__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1, 0b1100};
}

let Itinerary = II_MUL in
def MUL : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "mul", "$d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__mul_r_rr__mRa__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1, 0b1111};
}

let Itinerary = II_NEZ in
def NEZ : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "nez", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__sne0__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b0111, 0b10, 0b0000};
}

let Itinerary = II_NE in
def NE : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "ne", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sne__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1000, 0b1};
}

let Itinerary = II_NOPA, isSlotNOP = true in
def NOPA : AIE2P_inst_lda_instr32 <(outs), (ins), "nopa", "">{
// id: me__instr128_or__lda__nop_lda

let lda = {dontcare{10-1}, 0b1011001111};
}

let Itinerary = II_NOPB, isSlotNOP = true in
def NOPB : AIE2P_inst_ldb_instr32 <(outs), (ins), "nopb", "">{
// id: me__instr128_or__ldb__ldb_or1__nop_ldb

let ldb = {dontcare{5-1}, dontcare{2-1}, 0b0, 0b0, 0b0000, dontcare{1}, 0b100};
}

let Itinerary = II_NOPM, isSlotNOP = true in
def NOPM : AIE2P_inst_mv_instr32 <(outs), (ins), "nopm", "">{
// id: me__instr128_or__alu_mv__alumv_or__mv__nop_mv

let mv = {dontcare{9-1}, 0b1101001010111};
}

let Itinerary = II_NOPS, isSlotNOP = true in
def NOPS : AIE2P_inst_st_instr32 <(outs), (ins), "nops", "">{
// id: me__instr128_or__st__nop_sts

let st = {dontcare{10-1}, 0b1010110110};
}

let Itinerary = II_NOPV, isSlotNOP = true in
def NOPV : AIE2P_inst_vec_instr32 <(outs), (ins), "nopv", "">{
// id: me__instr128_or__vec__nop_vec

let vec = {dontcare{5-1}, dontcare{3-1}, dontcare{3-1}, dontcare{4-1}, 0b0000, 0b111, 0b00, 0b00};
}

let Itinerary = II_NOPXM, isSlotNOP = true in
def NOPXM : AIE2P_inst_lng_instr48 <(outs), (ins), "nopxm", "">{
// id: me__instr128_or__alu_mv__lng__nop_lng

let lng = {dontcare{5-1}, dontcare{5-1}, dontcare{4-1}, dontcare{4-1}, dontcare{4-1}, dontcare{4-1}, dontcare{4-1}, dontcare{4-1}, dontcare{5-1}, 0b000};
}

let Itinerary = II_NOPX, isSlotNOP = true in
def NOPX : AIE2P_inst_alu_instr32 <(outs), (ins), "nopx", "">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nop_alu

let alu = {dontcare{5-1}, 0b000, dontcare{4-1}, 0b0, 0b000, 0b0000};
}

let Itinerary = II_OR in
def OR : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "or", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__or__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0101, 0b1};
}

let Itinerary = II_PADDA_2D, DecoderMethod = "DecodeLda_ptr_inc_2D", Constraints = "$ptr_out = $ptr" in
def PADDA_2D : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "padda.2d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;

let lda = {ptr, mod, 0b100, dontcare{1}, 0b0011001111};
}

let Itinerary = II_PADDA_3D, DecoderMethod = "DecodeLda_ptr_inc_3D", Constraints = "$ptr_out = $ptr" in
def PADDA_3D : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "padda.3d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;

let lda = {ptr, dontcare{1}, mod, 0b110, dontcare{1}, 0b0011001111};
}

let Itinerary = II_PADDA_pstm_nrm, Constraints = "$ptr_out = $ptr" in
def PADDA_pstm_nrm : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, eM:$mod), "padda", " [$ptr], $mod">{
// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;

let lda = {ptr, mod, 0b010, dontcare{1}, 0b0011001111};
}

let Itinerary = II_PADDA_pstm_nrm_imm, Constraints = "$ptr_out = $ptr" in
def PADDA_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "padda", " [$ptr], #$imm">{
// id: me__instr128_or__lda__lda_ptr_inc__agua_sa__standalone__agua_sa__sa_modes__agua_sa__pstm_nrm_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;

let lda = {ptr, imm, 0b11, dontcare{1}, 0b0011001111};
}

let Itinerary = II_PADDB_2D, DecoderMethod = "DecodeLdb_or1_2D", Constraints = "$ptr_out = $ptr" in
def PADDB_2D : AIE2P_inst_ldb_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "paddb.2d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__ldb_or1__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_2d__mPb__mDb
bits<3> ptr;
bits<3> mod;

let ldb = {ptr, mod, 0b100, 0b111, 0b0, 0b0, 0b100};
}

let Itinerary = II_PADDB_3D, DecoderMethod = "DecodeLdb_or1_3D", Constraints = "$ptr_out = $ptr" in
def PADDB_3D : AIE2P_inst_ldb_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "paddb.3d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__ldb_or1__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_3d__mPb__mDSb
bits<3> ptr;
bits<2> mod;

let ldb = {ptr, dontcare{1}, mod, 0b110, 0b111, 0b0, 0b0, 0b100};
}

let Itinerary = II_PADDB_pstm_nrm, Constraints = "$ptr_out = $ptr" in
def PADDB_pstm_nrm : AIE2P_inst_ldb_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, eM:$mod), "paddb", " [$ptr], $mod">{
// id: me__instr128_or__ldb__ldb_or1__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_nrm__mPb__mMb
bits<3> ptr;
bits<3> mod;

let ldb = {ptr, mod, 0b010, 0b111, 0b0, 0b0, 0b100};
}

let Itinerary = II_PADDB_pstm_nrm_imm, Constraints = "$ptr_out = $ptr" in
def PADDB_pstm_nrm_imm : AIE2P_inst_ldb_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "paddb", " [$ptr], #$imm">{
// id: me__instr128_or__ldb__ldb_or1__ldb_ptr_inc__agub_sa__standalone__agub_sa__sa_modes__agub_sa__pstm_nrm_imm__mPb__m_c10s_step64
bits<3> ptr;
bits<4> imm;

let ldb = {ptr, imm, 0b11, 0b111, 0b0, 0b0, 0b100};
}

let Itinerary = II_PADDS_2D, DecoderMethod = "DecodeSt_ptr_inc_2D", Constraints = "$ptr_out = $ptr" in
def PADDS_2D : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "padds.2d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;

let st = {ptr, mod, 0b100, dontcare{1}, 0b0010110110};
}

let Itinerary = II_PADDS_3D, DecoderMethod = "DecodeSt_ptr_inc_3D", Constraints = "$ptr_out = $ptr" in
def PADDS_3D : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "padds.3d", " [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;

let st = {ptr, dontcare{1}, mod, 0b110, dontcare{1}, 0b0010110110};
}

let Itinerary = II_PADDS_pstm_nrm, Constraints = "$ptr_out = $ptr" in
def PADDS_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, eM:$mod), "padds", " [$ptr], $mod">{
// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;

let st = {ptr, mod, 0b010, dontcare{1}, 0b0010110110};
}

let Itinerary = II_PADDS_pstm_nrm_imm, Constraints = "$ptr_out = $ptr" in
def PADDS_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "padds", " [$ptr], #$imm">{
// id: me__instr128_or__st__st_ptr_inc__agus_sa__standalone__agus_sa__sa_modes__agus_sa__pstm_nrm_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;

let st = {ptr, imm, 0b11, dontcare{1}, 0b0010110110};
}

let Itinerary = II_PADDXM_pstm_sp, Defs = [sp], Uses = [sp] in
def PADDXM_pstm_sp : AIE2P_inst_lng_instr48 <(outs), (ins eM:$mod), "paddxm", " [sp], $mod">{
// id: me__instr128_or__alu_mv__lng__lda_ptr_inc_long__agua_sa_lng__standalone_long__agua_sa_lng__standalone_lng__agua_sa_lng__pstm_sp__mSPa__mMa
bits<3> mod;

let lng = {mod, dontcare{10-1}, 0b1, dontcare{25-1}, 0b111};
}

let Itinerary = II_PADDXM_pstm_sp_imm, Defs = [sp], Uses = [sp] in
def PADDXM_pstm_sp_imm : AIE2P_inst_lng_instr48 <(outs), (ins c19s_step64:$imm), "paddxm", " [sp], #$imm">{
// id: me__instr128_or__alu_mv__lng__lda_ptr_inc_long__agua_sa_lng__standalone_long__agua_sa_lng__standalone_lng__agua_sa_lng__pstm_sp_imm__mSPa__m_c19s_step64
bits<13> imm;

let lng = {imm, 0b0, dontcare{25-1}, 0b111};
}

let Itinerary = II_POPCOUNT in
def POPCOUNT : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "popcount", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_r__popcnt__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b1001, 0b10, 0b0000};
}

let Itinerary = II_REL_COND_mLockId_imm, hasSideEffects = true in
def REL_COND_mLockId_imm : AIE2P_inst_alu_instr32 <(outs), (ins c6u:$id, eR:$s1, mR26_lock:$s2), "rel.cond", "#$id, $s1, r26">{
// id: me__instr128_or__alu_mv__alumv_or__alu__release_cond__mLockId__mLockId_imm__mRy__mR26_lock
bits<6> id;
bits<5> s1;

let alu = {id, 0b0, dontcare{1}, 0b10, s1, 0b1, 0b0000};
}

let Itinerary = II_REL_COND_mLockId_reg, hasSideEffects = true in
def REL_COND_mLockId_reg : AIE2P_inst_alu_instr32 <(outs), (ins eR:$id, eR:$s1, mR26_lock:$s2), "rel.cond", "$id, $s1, r26">{
// id: me__instr128_or__alu_mv__alumv_or__alu__release_cond__mLockId__mLockId_reg__mRx__mRy__mR26_lock
bits<5> id;
bits<5> s1;

let alu = {id, dontcare{1}, 0b1, dontcare{1}, 0b10, s1, 0b1, 0b0000};
}

let Itinerary = II_REL_mLockId_imm, hasSideEffects = true in
def REL_mLockId_imm : AIE2P_inst_alu_instr32 <(outs), (ins c6u:$id, eR:$s1), "rel", "#$id, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__release__mLockId__mLockId_imm__mRy
bits<6> id;
bits<5> s1;

let alu = {id, 0b0, dontcare{1}, 0b00, s1, 0b1, 0b0000};
}

let Itinerary = II_REL_mLockId_reg, hasSideEffects = true in
def REL_mLockId_reg : AIE2P_inst_alu_instr32 <(outs), (ins eR:$id, eR:$s1), "rel", "$id, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__release__mLockId__mLockId_reg__mRx__mRy
bits<5> id;
bits<5> s1;

let alu = {id, dontcare{1}, 0b1, dontcare{1}, 0b00, s1, 0b1, 0b0000};
}

let Itinerary = II_RET, hasDelaySlot = true, Uses = [lr] in
def RET : AIE2P_inst_alu_instr32 <(outs), (ins), "ret", "lr">{
// id: me__instr128_or__alu_mv__alumv_or__alu__jump_return__mLRx

let alu = {dontcare{5-1}, 0b101, dontcare{5-1}, 0b000, 0b0000};
}

let Itinerary = II_SBC, Defs = [srCarry], Uses = [srCarry] in
def SBC : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "sbc", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__subx__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0011, 0b1};
}

let Itinerary = II_SEL_EQZ in
def SEL_EQZ : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1, mR27_select:$s2), "sel.eqz", " $d0, $s0, $s1, r27">{
// id: me__instr128_or__alu_mv__alumv_or__alu__select_r_rr__IF_FALSE__mRx__mRy__mR27_select
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0, 0b0100};
}

let Itinerary = II_SEL_NEZ in
def SEL_NEZ : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1, mR27_select:$s2), "sel.nez", " $d0, $s0, $s1, r27">{
// id: me__instr128_or__alu_mv__alumv_or__alu__select_r_rr__IF_TRUE__mRx__mRy__mR27_select
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b1, 0b0100};
}

let Itinerary = II_SQRT_mOptConvDel_mRx, Defs = [srFPCnvFl2Fx, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPNlfMask] in
def SQRT_mOptConvDel_mRx : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins mS3:$m, eR:$s0), "sqrt", " ${d0}.flt2fx, s3, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mRa__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b00, 0b1, 0b0, 0b00, 0b1000};
}

let Itinerary = II_SQRT_mOptConvDel_mRx_mOptConv, Defs = [srFPCnvFl2Fx, srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFl2FxMask, crFPCnvFx2FlMask, crFPNlfMask] in
def SQRT_mOptConvDel_mRx_mOptConv : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins mS3:$m, eR:$s0, mS2:$m_1), "sqrt", " ${d0}.flt2fx, s3, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mRa__mOptConvDel__mNlfFlt2fx__mFlt2fx_delayed__mS3__mRx__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b00, 0b1, 0b1, 0b00, 0b1000};
}

let Itinerary = II_SQRT_mRx, Defs = [srFPNlf], Uses = [crFPNlfMask] in
def SQRT_mRx : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0), "sqrt", " $d0, $s0">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mRa__mRx
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b00, 0b0, 0b0, 0b00, 0b1000};
}

let Itinerary = II_SQRT_mRx_mOptConv, Defs = [srFPCnvFx2Fl, srFPNlf], Uses = [crFPCnvFx2FlMask, crFPNlfMask] in
def SQRT_mRx_mOptConv : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, mS2:$m), "sqrt", " $d0, ${s0}.fx2flt, s2">{
// id: me__instr128_or__alu_mv__alumv_or__alu__nlf_combo__nlf_sqrt__mRa__mRx__mOptConv__mNlfFx2flt__mFx2flt__mS2
bits<5> s0;
bits<5> d0;

let alu = {s0, d0, 0b00, 0b0, 0b1, 0b00, 0b1000};
}

let Itinerary = II_ST_2D_dms_sts, ItineraryRegPairs = [
ItinRegClassPair<II_ST_2D_dms_sts_eEle, [OperandRegClass<2, eEle>]>,
ItinRegClassPair<II_ST_2D_dms_sts_eElo, [OperandRegClass<2, eElo>]>,
ItinRegClassPair<II_ST_2D_dms_sts_eDJ, [OperandRegClass<2, eDJ>]>,
ItinRegClassPair<II_ST_2D_dms_sts_eR, [OperandRegClass<2, eR>]>,
ItinRegClassPair<II_ST_2D_dms_sts_mLRs, [OperandRegClass<2, mLRs>]>,
ItinRegClassPair<II_ST_2D_dms_sts_eDC, [OperandRegClass<2, eDC>]>,
ItinRegClassPair<II_ST_2D_dms_sts_eDN, [OperandRegClass<2, eDN>]>,
ItinRegClassPair<II_ST_2D_dms_sts_eEhe, [OperandRegClass<2, eEhe>]>,
ItinRegClassPair<II_ST_2D_dms_sts_eP, [OperandRegClass<2, eP>]>,
ItinRegClassPair<II_ST_2D_dms_sts_eM, [OperandRegClass<2, eM>]>,
ItinRegClassPair<II_ST_2D_dms_sts_eEho, [OperandRegClass<2, eEho>]>], DecoderMethod = "DecodeDms_sts_2D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_2D_dms_sts : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mSclSt:$src, eP:$ptr, eD:$mod), "st.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dms_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<7> src;

let st = {ptr, mod, 0b100, src, 0b0011};
}

let Itinerary = II_ST_2D_dmv_sts_q, DecoderMethod = "DecodeDmv_sts_q_2D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_2D_dmv_sts_q : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mQQss:$src, eP:$ptr, eD:$mod), "st.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_q__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<2> src;

let st = {ptr, mod, 0b100, src, 0b000110110};
}

let Itinerary = II_ST_2D_s16, DecoderMethod = "DecodeDmhb_sts_2D", mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_2D_s16 : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eR:$src, eP:$ptr, eD:$mod), "st.2d.s16", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_2d__mPa__mDa__mRs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let lda = {ptr, mod, 0b100, src, 0b1, 0b01110};
}

let Itinerary = II_ST_2D_s8, DecoderMethod = "DecodeDmhb_sts_2D", mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_2D_s8 : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eR:$src, eP:$ptr, eD:$mod), "st.2d.s8", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_2d__mPa__mDa__mRs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let lda = {ptr, mod, 0b100, src, 0b0, 0b01110};
}

let Itinerary = II_ST_3D_dms_sts, ItineraryRegPairs = [
ItinRegClassPair<II_ST_3D_dms_sts_eEle, [OperandRegClass<3, eEle>]>,
ItinRegClassPair<II_ST_3D_dms_sts_eElo, [OperandRegClass<3, eElo>]>,
ItinRegClassPair<II_ST_3D_dms_sts_eDJ, [OperandRegClass<3, eDJ>]>,
ItinRegClassPair<II_ST_3D_dms_sts_eR, [OperandRegClass<3, eR>]>,
ItinRegClassPair<II_ST_3D_dms_sts_mLRs, [OperandRegClass<3, mLRs>]>,
ItinRegClassPair<II_ST_3D_dms_sts_eDC, [OperandRegClass<3, eDC>]>,
ItinRegClassPair<II_ST_3D_dms_sts_eDN, [OperandRegClass<3, eDN>]>,
ItinRegClassPair<II_ST_3D_dms_sts_eEhe, [OperandRegClass<3, eEhe>]>,
ItinRegClassPair<II_ST_3D_dms_sts_eP, [OperandRegClass<3, eP>]>,
ItinRegClassPair<II_ST_3D_dms_sts_eM, [OperandRegClass<3, eM>]>,
ItinRegClassPair<II_ST_3D_dms_sts_eEho, [OperandRegClass<3, eEho>]>], DecoderMethod = "DecodeDms_sts_3D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_3D_dms_sts : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mSclSt:$src, eP:$ptr, eDS:$mod), "st.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dms_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<7> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b0011};
}

let Itinerary = II_ST_3D_dmv_sts_q, DecoderMethod = "DecodeDmv_sts_q_3D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_3D_dmv_sts_q : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mQQss:$src, eP:$ptr, eDS:$mod), "st.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_q__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<2> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b000110110};
}

let Itinerary = II_ST_3D_s16, DecoderMethod = "DecodeDmhb_sts_3D", mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_3D_s16 : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eR:$src, eP:$ptr, eDS:$mod), "st.3d.s16", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_3d__mPa__mDSa__mRs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let lda = {ptr, dontcare{1}, mod, 0b110, src, 0b1, 0b01110};
}

let Itinerary = II_ST_3D_s8, DecoderMethod = "DecodeDmhb_sts_3D", mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_3D_s8 : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eR:$src, eP:$ptr, eDS:$mod), "st.3d.s8", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_3d__mPa__mDSa__mRs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let lda = {ptr, dontcare{1}, mod, 0b110, src, 0b0, 0b01110};
}

let Itinerary = II_ST_TM_2D, DecoderMethod = "DecodeTm_sts_2D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_TM_2D : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eR:$src, eP:$ptr, eD:$mod), "st.tm.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_2d__mPs__mDs__mRs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b111101};
}

let Itinerary = II_ST_TM_3D, DecoderMethod = "DecodeTm_sts_3D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_TM_3D : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eR:$src, eP:$ptr, eDS:$mod), "st.tm.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_3d__mPs__mDSs__mRs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b111101};
}

let Itinerary = II_ST_TM_idx, mayStore = true in
def ST_TM_idx : AIE2P_inst_st_instr32 <(outs), (ins eR:$src, eP:$ptr, eDJ:$dj), "st.tm", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__idx__mPs__mDJs__mRs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b111101};
}

let Itinerary = II_ST_TM_idx_imm, mayStore = true in
def ST_TM_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins eR:$src, eP:$ptr, c6s_step4:$imm), "st.tm", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__idx_imm__mPs__m_c06s_step4__mRs
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b111101};
}

let Itinerary = II_ST_TM_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_TM_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eR:$src, eP:$ptr, eM:$mod), "st.tm", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm__mPs__mMs__mRs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b111101};
}

let Itinerary = II_ST_TM_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_TM_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eR:$src, eP:$ptr, c6s_step4:$imm), "st.tm", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__tm_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm_imm__mPs__m_c06s_step4__mRs
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b111101};
}

let Itinerary = II_ST_dms_sts_idx, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_idx_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_idx_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_idx_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_idx_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_ST_dms_sts_idx_mLRs, [OperandRegClass<0, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_idx_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_idx_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_idx_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_idx_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_ST_dms_sts_idx_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_ST_dms_sts_idx_eEho, [OperandRegClass<0, eEho>]>], mayStore = true in
def ST_dms_sts_idx : AIE2P_inst_st_instr32 <(outs), (ins OP_mSclSt:$src, eP:$ptr, eDJ:$dj), "st", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dms_sts__agus_dms__nrm__agus_dms__normal__agus_dms__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<7> src;

let st = {ptr, dj, 0b000, src, 0b0011};
}

let Itinerary = II_ST_dms_sts_idx_imm, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_idx_imm_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_mLRs, [OperandRegClass<0, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_ST_dms_sts_idx_imm_eEho, [OperandRegClass<0, eEho>]>], mayStore = true in
def ST_dms_sts_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mSclSt:$src, eP:$ptr, c6s_step4:$imm), "st", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dms_sts__agus_dms__nrm__agus_dms__normal__agus_dms__idx_imm__mPs__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<7> src;

let st = {ptr, imm, 0b01, src, 0b0011};
}

let Itinerary = II_ST_dms_sts_pstm_nrm, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eEle, [OperandRegClass<1, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eElo, [OperandRegClass<1, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eR, [OperandRegClass<1, eR>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_mLRs, [OperandRegClass<1, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eEhe, [OperandRegClass<1, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_eEho, [OperandRegClass<1, eEho>]>], mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dms_sts_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mSclSt:$src, eP:$ptr, eM:$mod), "st", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dms_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<7> src;

let st = {ptr, mod, 0b010, src, 0b0011};
}

let Itinerary = II_ST_dms_sts_pstm_nrm_imm, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eEle, [OperandRegClass<1, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eElo, [OperandRegClass<1, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eDJ, [OperandRegClass<1, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eR, [OperandRegClass<1, eR>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_mLRs, [OperandRegClass<1, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eDC, [OperandRegClass<1, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eDN, [OperandRegClass<1, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eEhe, [OperandRegClass<1, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eP, [OperandRegClass<1, eP>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eM, [OperandRegClass<1, eM>]>,
ItinRegClassPair<II_ST_dms_sts_pstm_nrm_imm_eEho, [OperandRegClass<1, eEho>]>], mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dms_sts_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mSclSt:$src, eP:$ptr, c6s_step4:$imm), "st", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dms_sts__agus_dms__nrm__agus_dms__normal__agus_dms__pstm_nrm_imm__mPs__m_c06s_step4
bits<3> ptr;
bits<4> imm;
bits<7> src;

let st = {ptr, imm, 0b11, src, 0b0011};
}

let Itinerary = II_ST_dms_sts_spill, ItineraryRegPairs = [
ItinRegClassPair<II_ST_dms_sts_spill_eEle, [OperandRegClass<0, eEle>]>,
ItinRegClassPair<II_ST_dms_sts_spill_eElo, [OperandRegClass<0, eElo>]>,
ItinRegClassPair<II_ST_dms_sts_spill_eDJ, [OperandRegClass<0, eDJ>]>,
ItinRegClassPair<II_ST_dms_sts_spill_eR, [OperandRegClass<0, eR>]>,
ItinRegClassPair<II_ST_dms_sts_spill_mLRs, [OperandRegClass<0, mLRs>]>,
ItinRegClassPair<II_ST_dms_sts_spill_eDC, [OperandRegClass<0, eDC>]>,
ItinRegClassPair<II_ST_dms_sts_spill_eDN, [OperandRegClass<0, eDN>]>,
ItinRegClassPair<II_ST_dms_sts_spill_eEhe, [OperandRegClass<0, eEhe>]>,
ItinRegClassPair<II_ST_dms_sts_spill_eP, [OperandRegClass<0, eP>]>,
ItinRegClassPair<II_ST_dms_sts_spill_eM, [OperandRegClass<0, eM>]>,
ItinRegClassPair<II_ST_dms_sts_spill_eEho, [OperandRegClass<0, eEho>]>], mayStore = true, Uses = [sp] in
def ST_dms_sts_spill : AIE2P_inst_st_instr32 <(outs), (ins OP_mSclSt:$src, c12n_step4:$imm), "st", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dms_sts_spill__agus_dms__nrm_spill__agus_dms__spill__mSPs__m_c12n_step4
bits<9> imm;
bits<7> src;

let st = {imm, src, 0b1011};
}

let Itinerary = II_ST_dmv_sts_q_idx, mayStore = true in
def ST_dmv_sts_q_idx : AIE2P_inst_st_instr32 <(outs), (ins OP_mQQss:$src, eP:$ptr, eDJ:$dj), "st", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmv_sts_q__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<2> src;

let st = {ptr, dj, 0b000, src, 0b000110110};
}

let Itinerary = II_ST_dmv_sts_q_idx_imm, mayStore = true in
def ST_dmv_sts_q_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mQQss:$src, eP:$ptr, c8s_step16:$imm), "st", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmv_sts_q__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<2> src;

let st = {ptr, imm, 0b01, src, 0b000110110};
}

let Itinerary = II_ST_dmv_sts_q_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dmv_sts_q_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mQQss:$src, eP:$ptr, eM:$mod), "st", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmv_sts_q__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<2> src;

let st = {ptr, mod, 0b010, src, 0b000110110};
}

let Itinerary = II_ST_dmv_sts_q_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def ST_dmv_sts_q_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mQQss:$src, eP:$ptr, c8s_step16:$imm), "st", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmv_sts_q__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<2> src;

let st = {ptr, imm, 0b11, src, 0b000110110};
}

let Itinerary = II_ST_dmv_sts_q_spill, mayStore = true, Uses = [sp] in
def ST_dmv_sts_q_spill : AIE2P_inst_st_instr32 <(outs), (ins OP_mQQss:$src, c14n_step16:$imm), "st", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmv_sts_q_spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
bits<9> imm;
bits<2> src;

let st = {imm, src, 0b100110110};
}

let Itinerary = II_ST_s16_idx, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads] in
def ST_s16_idx : AIE2P_inst_lda_instr32 <(outs), (ins eR:$src, eP:$ptr, eDJ:$dj), "st.s16", " $src, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__idx__mPa__mDJa__mRs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let lda = {ptr, dj, 0b000, src, 0b1, 0b01110};
}

let Itinerary = II_ST_s16_idx_imm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads] in
def ST_s16_idx_imm : AIE2P_inst_lda_instr32 <(outs), (ins eR:$src, eP:$ptr, c5s_step2:$imm), "st.s16", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__idx_imm__mPa__m_c05s_step2__mRs
bits<3> ptr;
bits<4> imm;
bits<5> src;

let lda = {ptr, imm, 0b01, src, 0b1, 0b01110};
}

let Itinerary = II_ST_s16_pstm_nrm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_s16_pstm_nrm : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out), (ins eR:$src, eP:$ptr, eM:$mod), "st.s16", " $src, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_nrm__mPa__mMa__mRs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let lda = {ptr, mod, 0b010, src, 0b1, 0b01110};
}

let Itinerary = II_ST_s16_pstm_nrm_imm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_s16_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out), (ins eR:$src, eP:$ptr, c5s_step2:$imm), "st.s16", " $src, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_sts__dmh_sts__agua_dmh__normal__agua_dmh__pstm_nrm_imm__mPa__m_c05s_step2__mRs
bits<3> ptr;
bits<4> imm;
bits<5> src;

let lda = {ptr, imm, 0b11, src, 0b1, 0b01110};
}

let Itinerary = II_ST_s8_idx, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads] in
def ST_s8_idx : AIE2P_inst_lda_instr32 <(outs), (ins eR:$src, eP:$ptr, eDJ:$dj), "st.s8", " $src, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__idx__mPa__mDJa__mRs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let lda = {ptr, dj, 0b000, src, 0b0, 0b01110};
}

let Itinerary = II_ST_s8_idx_imm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads] in
def ST_s8_idx_imm : AIE2P_inst_lda_instr32 <(outs), (ins eR:$src, eP:$ptr, c4s:$imm), "st.s8", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__idx_imm__mPa__m_c04s__mRs
bits<3> ptr;
bits<4> imm;
bits<5> src;

let lda = {ptr, imm, 0b01, src, 0b0, 0b01110};
}

let Itinerary = II_ST_s8_pstm_nrm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_s8_pstm_nrm : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out), (ins eR:$src, eP:$ptr, eM:$mod), "st.s8", " $src, [$ptr], $mod">{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_nrm__mPa__mMa__mRs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let lda = {ptr, mod, 0b010, src, 0b0, 0b01110};
}

let Itinerary = II_ST_s8_pstm_nrm_imm, mayLoad = true, mayStore = true, Defs = [pe2_ads], Uses = [pe2_ads], Constraints = "$ptr_out = $ptr" in
def ST_s8_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs eP:$ptr_out), (ins eR:$src, eP:$ptr, c4s:$imm), "st.s8", " $src, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmhb_sts__dmb_sts__agua_dmb__normal__agua_dmb__pstm_nrm_imm__mPa__m_c04s__mRs
bits<3> ptr;
bits<4> imm;
bits<5> src;

let lda = {ptr, imm, 0b11, src, 0b0, 0b01110};
}

let Itinerary = II_SUB, Defs = [srCarry] in
def SUB : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "sub", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__sub__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0001, 0b1};
}

let Itinerary = II_VABS_GTZ_16_vaddSign0, Uses = [vaddSign0] in
def VABS_GTZ_16_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXn:$s2), "vabs_gtz.16", " $d, r16, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b01, 0b0, 0b10110010111};
}

let Itinerary = II_VABS_GTZ_16_vaddSign1, Uses = [vaddSign1] in
def VABS_GTZ_16_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXn:$s2), "vabs_gtz.16", " $d, r16, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b01, 0b1, 0b10110010111};
}

let Itinerary = II_VABS_GTZ_32_vaddSign0, Uses = [vaddSign0] in
def VABS_GTZ_32_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXn:$s2), "vabs_gtz.32", " $d, r16, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b00, 0b0, 0b10110010111};
}

let Itinerary = II_VABS_GTZ_32_vaddSign1, Uses = [vaddSign1] in
def VABS_GTZ_32_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXn:$s2), "vabs_gtz.32", " $d, r16, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b00, 0b1, 0b10110010111};
}

let Itinerary = II_VABS_GTZ_8_vaddSign0, Uses = [vaddSign0] in
def VABS_GTZ_8_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXn:$s2), "vabs_gtz.8", " $d, r17:r16, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b10, 0b0, 0b10110010111};
}

let Itinerary = II_VABS_GTZ_8_vaddSign1, Uses = [vaddSign1] in
def VABS_GTZ_8_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXn:$s2), "vabs_gtz.8", " $d, r17:r16, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_abs__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b10, 0b1, 0b10110010111};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_reg_vmul_cm_core_X_QX, Defs = [srSparse_of], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_reg_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<4> s1;
bits<2> s2;

let vec = {acc, acc1, acc2, s1, s2, dontcare{2-1}, 0b100, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_reg_vmul_cm_core_X_QY, Defs = [srSparse_of], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_reg_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eQYs:$s2, eR:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<4> s1;
bits<1> s2;

let vec = {acc, acc1, acc2, s1, s2, dontcare{2-1}, 0b1010, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_reg_vmul_cm_core_X_X, Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_reg_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc1, acc2, s1, s2, 0b000, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_reg_vmul_cm_core_X_Y, Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_reg_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eY:$s2, eR:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<4> s1;
bits<3> s2;

let vec = {acc, acc1, acc2, s1, s2, 0b0010, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_reg_vmul_cm_core_Y_QX, Defs = [srSparse_of], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_reg_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<3> s1;
bits<2> s2;

let vec = {acc, acc1, acc2, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_reg_vmul_cm_core_Y_QY, Defs = [srSparse_of], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_reg_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eQYs:$s2, eR:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<3> s1;
bits<1> s2;

let vec = {acc, acc1, acc2, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_reg_vmul_cm_core_Y_X, Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_reg_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mXw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<3> s1;
bits<4> s2;

let vec = {acc, acc1, acc2, s1, 0b0, s2, 0b110, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_reg_vmul_cm_core_Y_Y, Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_reg_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eY:$s2, eR:$acc), "vaddmac", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<3> s1;
bits<3> s2;

let vec = {acc, acc1, acc2, s1, 0b0, s2, 0b0101, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_vmul_cm_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_scd_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<2> s2;

let vec = {acc, acc1, 0b110, s1, s2, dontcare{2-1}, 0b100, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_vmul_cm_core_X_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_scd_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, eQYs:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, acc1, 0b110, s1, s2, dontcare{2-1}, 0b1010, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_vmul_cm_core_X_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_scd_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc1, 0b110, s1, s2, 0b000, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_vmul_cm_core_X_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_scd_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, eY:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, acc1, 0b110, s1, s2, 0b0010, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_vmul_cm_core_Y_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_scd_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, acc1, 0b110, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_vmul_cm_core_Y_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_scd_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, eQYs:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<1> s2;

let vec = {acc, acc1, 0b110, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_vmul_cm_core_Y_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_scd_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, OP_mXw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<4> s2;

let vec = {acc, acc1, 0b110, s1, 0b0, s2, 0b110, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_vmul_cm_core_Y_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_vmac_cm2_add_scd_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, eY:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, acc1, 0b110, s1, 0b0, s2, 0b0101, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<2> s2;

let vec = {acc, acc1, 0b111, s1, s2, dontcare{2-1}, 0b100, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_X_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, eQYs:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, acc1, 0b111, s1, s2, dontcare{2-1}, 0b1010, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_X_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc1, 0b111, s1, s2, 0b000, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_X_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, eY:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, acc1, 0b111, s1, s2, 0b0010, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, acc1, 0b111, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, eQYs:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<1> s2;

let vec = {acc, acc1, 0b111, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, OP_mXw:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<4> s2;

let vec = {acc, acc1, 0b111, s1, 0b0, s2, 0b110, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, eY:$s2, eR:$acc), "vaddmac", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, acc1, 0b111, s1, 0b0, s2, 0b0101, 0b0, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmac_cm2_add_reg_vmul_bf_core_X_X, Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bf_vmac_cm2_add_reg_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc2, acc1, s1, s2, 0b000, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmac_cm2_add_reg_vmul_bf_core_Y_Y, Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bf_vmac_cm2_add_reg_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eY:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, acc2, acc1, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmac_cm2_add_scd_vmul_bf_core_X_X, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bf_vmac_cm2_add_scd_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, 0b110, acc1, s1, s2, 0b000, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmac_cm2_add_scd_vmul_bf_core_Y_Y, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bf_vmac_cm2_add_scd_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, eY:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, 0b110, acc1, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmac_cm2_add_scd_incr_vmul_bf_core_X_X, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_f_vaddmac_bf_vmac_cm2_add_scd_incr_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, 0b111, acc1, s1, s2, 0b000, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bf_vmac_cm2_add_scd_incr_vmul_bf_core_Y_Y, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_f_vaddmac_bf_vmac_cm2_add_scd_incr_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, eY:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, 0b111, acc1, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EX, Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc2, acc1, s1, s2, 0b010, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EY, Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mEXv:$s1, eEY:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, acc2, acc1, s1, s2, dontcare{1}, 0b001, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_QEY, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, acc2, acc1, s1, s2, dontcare{3-1}, 0b101, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EY_QEX, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, acc2, acc1, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_EX, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, 0b110, acc1, s1, s2, 0b010, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_EY, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mEXv:$s1, eEY:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, 0b110, acc1, s1, s2, dontcare{1}, 0b001, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_QEY, hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, 0b110, acc1, s1, s2, dontcare{3-1}, 0b101, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EY_QEX, hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, 0b110, acc1, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_EX, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, 0b111, acc1, s1, s2, 0b010, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_EY, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mEXv:$s1, eEY:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, 0b111, acc1, s1, s2, dontcare{1}, 0b001, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_QEY, hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, 0b111, acc1, s1, s2, dontcare{3-1}, 0b101, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EY_QEX, hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMAC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vaddmac.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, 0b111, acc1, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b0, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_reg_vmul_cm_core_X_QX, Defs = [srSparse_of], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_reg_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<4> s1;
bits<2> s2;

let vec = {acc, acc1, acc2, s1, s2, dontcare{2-1}, 0b100, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_reg_vmul_cm_core_X_QY, Defs = [srSparse_of], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_reg_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eQYs:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<4> s1;
bits<1> s2;

let vec = {acc, acc1, acc2, s1, s2, dontcare{2-1}, 0b1010, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_reg_vmul_cm_core_X_X, Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_reg_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc1, acc2, s1, s2, 0b000, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_reg_vmul_cm_core_X_Y, Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_reg_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, eY:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<4> s1;
bits<3> s2;

let vec = {acc, acc1, acc2, s1, s2, 0b0010, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_reg_vmul_cm_core_Y_QX, Defs = [srSparse_of], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_reg_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<3> s1;
bits<2> s2;

let vec = {acc, acc1, acc2, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_reg_vmul_cm_core_Y_QY, Defs = [srSparse_of], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_reg_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eQYs:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<3> s1;
bits<1> s2;

let vec = {acc, acc1, acc2, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_reg_vmul_cm_core_Y_X, Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_reg_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, OP_mXw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<3> s1;
bits<4> s2;

let vec = {acc, acc1, acc2, s1, 0b0, s2, 0b110, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_reg_vmul_cm_core_Y_Y, Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_reg_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eY:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, $acc2, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> acc2;
bits<3> s1;
bits<3> s2;

let vec = {acc, acc1, acc2, s1, 0b0, s2, 0b0101, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_vmul_cm_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_scd_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<2> s2;

let vec = {acc, acc1, 0b110, s1, s2, dontcare{2-1}, 0b100, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_vmul_cm_core_X_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_scd_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, eQYs:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, acc1, 0b110, s1, s2, dontcare{2-1}, 0b1010, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_vmul_cm_core_X_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_scd_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc1, 0b110, s1, s2, 0b000, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_vmul_cm_core_X_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_scd_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, eY:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, acc1, 0b110, s1, s2, 0b0010, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_vmul_cm_core_Y_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_scd_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, acc1, 0b110, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_vmul_cm_core_Y_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_scd_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, eQYs:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<1> s2;

let vec = {acc, acc1, 0b110, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_vmul_cm_core_Y_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_scd_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, OP_mXw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<4> s2;

let vec = {acc, acc1, 0b110, s1, 0b0, s2, 0b110, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_vmul_cm_core_Y_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_vmac_cm2_add_scd_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, eY:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, acc1, 0b110, s1, 0b0, s2, 0b0101, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_X_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<2> s2;

let vec = {acc, acc1, 0b111, s1, s2, dontcare{2-1}, 0b100, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_X_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, eQYs:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, acc1, 0b111, s1, s2, dontcare{2-1}, 0b1010, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_X_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc1, 0b111, s1, s2, 0b000, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_X_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, eY:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, acc1, 0b111, s1, s2, 0b0010, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_QX, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, OP_mQXsw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, acc1, 0b111, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_QY, hasSideEffects = true, Defs = [srSparse_of], Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, eQYs:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<1> s2;

let vec = {acc, acc1, 0b111, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_X, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, OP_mXw:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<4> s2;

let vec = {acc, acc1, 0b111, s1, 0b0, s2, 0b110, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_Y, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_vmac_cm2_add_scd_incr_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, eY:$s2, eR:$acc), "vaddmsc", "$dst, $acc1, scd[r31++], $s1, $s2,$acc">{
// id: me__instr128_or__vec__vaddmac__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, acc1, 0b111, s1, 0b0, s2, 0b0101, 0b1, 0b0, dontcare{1}, 0b0};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmac_cm2_add_reg_vmul_bf_core_X_X, Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bf_vmac_cm2_add_reg_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc2, acc1, s1, s2, 0b000, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmac_cm2_add_reg_vmul_bf_core_Y_Y, Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bf_vmac_cm2_add_reg_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eY:$s1, eY:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, acc2, acc1, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmac_cm2_add_scd_vmul_bf_core_X_X, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bf_vmac_cm2_add_scd_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, 0b110, acc1, s1, s2, 0b000, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmac_cm2_add_scd_vmul_bf_core_Y_Y, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bf_vmac_cm2_add_scd_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eY:$s1, eY:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, 0b110, acc1, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmac_cm2_add_scd_incr_vmul_bf_core_X_X, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_f_vaddmac_bf_vmac_cm2_add_scd_incr_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, 0b111, acc1, s1, s2, 0b000, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bf_vmac_cm2_add_scd_incr_vmul_bf_core_Y_Y, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_f_vaddmac_bf_vmac_cm2_add_scd_incr_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eY:$s1, eY:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bf__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, 0b111, acc1, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EX, Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, acc2, acc1, s1, s2, 0b010, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EY, Defs = [srFPFlags], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mEXv:$s1, eEY:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, acc2, acc1, s1, s2, dontcare{1}, 0b001, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_QEY, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, acc2, acc1, s1, s2, dontcare{3-1}, 0b101, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EY_QEX, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_reg_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, $acc2, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc2;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, acc2, acc1, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_EX, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, 0b110, acc1, s1, s2, 0b010, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_EY, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mEXv:$s1, eEY:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, 0b110, acc1, s1, s2, dontcare{1}, 0b001, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_QEY, hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, 0b110, acc1, s1, s2, dontcare{3-1}, 0b101, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EY_QEX, hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, 0b110, acc1, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_EX, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, 0b111, acc1, s1, s2, 0b010, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_EY, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mEXv:$s1, eEY:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, 0b111, acc1, s1, s2, dontcare{1}, 0b001, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_QEY, hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, 0b111, acc1, s1, s2, dontcare{3-1}, 0b101, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EY_QEX, hasSideEffects = true, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask, crSCDEn], Constraints = "$dst = $acc1, $r_out = $r" in
def VADDMSC_f_vaddmac_bfp_vmac_cm2_add_scd_incr_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vaddmsc.f", "$dst, $acc1, scd[r31++], $s1, $s2, $acc">{
// id: me__instr128_or__vec__vaddmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, 0b111, acc1, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b1, 0b0, dontcare{1}, 0b1};
}

let Itinerary = II_VADDSUB_16 in
def VADDSUB_16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, eRS16:$sel), "vaddsub.16", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in32__mRS16m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b01, 0b1, 0b001};
}

let Itinerary = II_VADDSUB_32 in
def VADDSUB_32 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, eRS16:$sel), "vaddsub.32", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in16__mRS16m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b00, 0b1, 0b001};
}

let Itinerary = II_VADDSUB_8 in
def VADDSUB_8 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, eL:$sel), "vaddsub.8", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in64__mLm__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b10, 0b1, 0b001};
}

let Itinerary = II_VADD_16 in
def VADD_16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vadd.16", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vadd__vec_size__vec_size32__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b0, 0b01, 0b1101011};
}

let Itinerary = II_VADD_32 in
def VADD_32 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vadd.32", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vadd__vec_size__vec_size16__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b0, 0b10, 0b1101011};
}

let Itinerary = II_VADD_8 in
def VADD_8 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vadd.8", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vadd__vec_size__vec_size64__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b0, 0b00, 0b1101011};
}

let Itinerary = II_VADD_vmac_cm2_add_reg in
def VADD_vmac_cm2_add_reg : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eR:$acc), "vadd", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> acc2;

let vec = {acc, dst, acc1, acc2, dontcare{4-1}, 0b1111, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VADD_vmac_cm2_add_scd, hasSideEffects = true, Uses = [crSCDEn] in
def VADD_vmac_cm2_add_scd : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eR:$acc), "vadd", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, 0b110, dontcare{4-1}, 0b1111, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VADD_vmac_cm2_add_scd_incr, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$r_out = $r" in
def VADD_vmac_cm2_add_scd_incr : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eR:$acc), "vadd", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, 0b111, dontcare{4-1}, 0b1111, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VADD_f_vmac_cm2_add_reg, Defs = [srFPFlags], Uses = [crFPMask] in
def VADD_f_vmac_cm2_add_reg : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eR:$acc), "vadd.f", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_fp__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> acc2;

let vec = {acc, dst, acc1, acc2, dontcare{4-1}, 0b1111, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VADD_f_vmac_cm2_add_scd, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VADD_f_vmac_cm2_add_scd : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eR:$acc), "vadd.f", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_fp__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, 0b110, dontcare{4-1}, 0b1111, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VADD_f_vmac_cm2_add_scd_incr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$r_out = $r" in
def VADD_f_vmac_cm2_add_scd_incr : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eR:$acc), "vadd.f", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_fp__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, 0b111, dontcare{4-1}, 0b1111, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VBAND in
def VBAND : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vband", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_log__vband__vec_add_mX__vec_add_nX__vec_size16
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b0, 0b10, 0b1011011};
}

let Itinerary = II_VBCSTSHFL_16_vec_broadcast_shuffle_bm in
def VBCSTSHFL_16_vec_broadcast_shuffle_bm : AIE2P_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins eR:$src, mR29_insert:$mod), "vbcstshfl.16", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_16__mRm__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b01, 0b0, 0b1010010111};
}

let Itinerary = II_VBCSTSHFL_16_vec_broadcast_shuffle_x in
def VBCSTSHFL_16_vec_broadcast_shuffle_x : AIE2P_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins eR:$src, mR29_insert:$mod), "vbcstshfl.16", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_16__mRm__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b01, 0b1, 0b1010010111};
}

let Itinerary = II_VBCSTSHFL_32_vec_broadcast_shuffle_bm in
def VBCSTSHFL_32_vec_broadcast_shuffle_bm : AIE2P_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins eR:$src, mR29_insert:$mod), "vbcstshfl.32", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_32__mRm__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b10, 0b0, 0b1010010111};
}

let Itinerary = II_VBCSTSHFL_32_vec_broadcast_shuffle_x in
def VBCSTSHFL_32_vec_broadcast_shuffle_x : AIE2P_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins eR:$src, mR29_insert:$mod), "vbcstshfl.32", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_32__mRm__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b10, 0b1, 0b1010010111};
}

let Itinerary = II_VBCSTSHFL_64_vec_broadcast_shuffle_bm in
def VBCSTSHFL_64_vec_broadcast_shuffle_bm : AIE2P_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins eL:$src, mR29_insert:$mod), "vbcstshfl.64", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mR29_insert
bits<4> dst;
bits<4> src;

let mv = {dst, src, dontcare{1}, 0b11, 0b0, 0b1010010111};
}

let Itinerary = II_VBCSTSHFL_64_vec_broadcast_shuffle_x in
def VBCSTSHFL_64_vec_broadcast_shuffle_x : AIE2P_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins eL:$src, mR29_insert:$mod), "vbcstshfl.64", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mR29_insert
bits<4> dst;
bits<4> src;

let mv = {dst, src, dontcare{1}, 0b11, 0b1, 0b1010010111};
}

let Itinerary = II_VBCSTSHFL_8_vec_broadcast_shuffle_bm in
def VBCSTSHFL_8_vec_broadcast_shuffle_bm : AIE2P_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins eR:$src, mR29_insert:$mod), "vbcstshfl.8", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_bm__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_8__mRm__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b00, 0b0, 0b1010010111};
}

let Itinerary = II_VBCSTSHFL_8_vec_broadcast_shuffle_x in
def VBCSTSHFL_8_vec_broadcast_shuffle_x : AIE2P_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins eR:$src, mR29_insert:$mod), "vbcstshfl.8", " $dst, $src, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast_shuffle__vec_broadcast_shuffle_x__vec_broadcast_shuffle_core__vec_insert_src__vec_insert_src_R__s2v_wide_8__mRm__mR29_insert
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b00, 0b1, 0b1010010111};
}

let Itinerary = II_VBCST_16 in
def VBCST_16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins eR:$src), "vbcst.16", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_R__s2v_wide_16__mRm__mMvXDst
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b01, 0b01110010111};
}

let Itinerary = II_VBCST_32 in
def VBCST_32 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins eR:$src), "vbcst.32", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_R__s2v_wide_32__mRm__mMvXDst
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b10, 0b01110010111};
}

let Itinerary = II_VBCST_64 in
def VBCST_64 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins eL:$src), "vbcst.64", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mMvXDst
bits<4> dst;
bits<4> src;

let mv = {dst, src, dontcare{1}, 0b11, 0b01110010111};
}

let Itinerary = II_VBCST_8 in
def VBCST_8 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins eR:$src), "vbcst.8", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_broadcast__vec_insert_src__vec_insert_src_R__s2v_wide_8__mRm__mMvXDst
bits<4> dst;
bits<5> src;

let mv = {dst, src, 0b00, 0b01110010111};
}

let Itinerary = II_VBNEG_LTZ_s16 in
def VBNEG_LTZ_s16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXn:$s2), "vbneg_ltz.s16", " $d, r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vbneg_ltz__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b01, 0b0, 0b00001010111};
}

let Itinerary = II_VBNEG_LTZ_s32 in
def VBNEG_LTZ_s32 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXn:$s2), "vbneg_ltz.s32", " $d, r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vbneg_ltz__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b00, 0b0, 0b00001010111};
}

let Itinerary = II_VBNEG_LTZ_s8 in
def VBNEG_LTZ_s8 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXn:$s2), "vbneg_ltz.s8", " $d, r17:r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vbneg_ltz__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b10, 0b0, 0b00001010111};
}

let Itinerary = II_VBOR in
def VBOR : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vbor", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_log__vbor__vec_add_mX__vec_add_nX__vec_size16
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b1, 0b10, 0b1011011};
}

let Itinerary = II_VCLR in
def VCLR : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins), "vclr", "$dst">{
// id: me__instr128_or__vec__vclr__mDMa
bits<3> dst;

let vec = {dontcare{5-1}, dst, 0b111, dontcare{4-1}, dontcare{3-1}, 0b0111, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VCONV_bf16_fp32_mv_w_srs_bf, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd] in
def VCONV_bf16_fp32_mv_w_srs_bf : AIE2P_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mBMs:$src), "vconv.bf16.fp32", " $dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_bf__mWbfsrs
bits<5> dst;
bits<5> src;

let st = {dst, 0b0, 0b000, src, 0b1, 0b01100};
}

let Itinerary = II_VCONV_bf16_fp32_mv_x_srs_bf, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd] in
def VCONV_bf16_fp32_mv_x_srs_bf : AIE2P_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mCMs:$src), "vconv.bf16.fp32", " $dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_bf__mXbfsrs
bits<4> dst;
bits<4> src;

let st = {dst, dontcare{1}, 0b0, 0b100, src, dontcare{1}, 0b1, 0b01100};
}

let Itinerary = II_VCONV_bfp16ebs16_ebs8, Defs = [srF2BFlags], Uses = [crF2BMask, crRnd] in
def VCONV_bfp16ebs16_ebs8 : AIE2P_inst_st_instr32 <(outs OP_mEXa:$dst), (ins OP_mEXs:$src), "vconv.bfp16ebs16.ebs8", " $dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_fp_to_bfp__mFp2Bp__mBp2Bp
bits<4> dst;
bits<4> src;

let st = {dst, dontcare{1}, 0b0, 0b010, src, 0b0, 0b1, 0b01100};
}

let Itinerary = II_VCONV_bfp16ebs16_fp32, Defs = [srF2BFlags], Uses = [crF2BMask, crRnd] in
def VCONV_bfp16ebs16_fp32 : AIE2P_inst_st_instr32 <(outs OP_mEXa:$dst), (ins eDM:$src), "vconv.bfp16ebs16.fp32", " $dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_fp_to_bfp__mFp2Bp__mFp2B1__mDMs
bits<4> dst;
bits<3> src;

let st = {dst, dontcare{1}, 0b0, 0b010, src, 0b11, 0b1, 0b01100};
}

let Itinerary = II_VCONV_bfp16ebs8_fp32, Defs = [srF2BFlags], Uses = [crF2BMask, crRnd] in
def VCONV_bfp16ebs8_fp32 : AIE2P_inst_st_instr32 <(outs OP_mEXa:$dst), (ins eDM:$src), "vconv.bfp16ebs8.fp32", " $dst, $src">{
// id: me__instr128_or__st__mv_conv__mv_fp_to_bfp__mFp2Bp__mFp2B0__mDMs
bits<4> dst;
bits<3> src;

let st = {dst, dontcare{1}, 0b0, 0b010, src, 0b01, 0b1, 0b01100};
}

let Itinerary = II_VCONV_fp32_bf16_mv_ups_wbf in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VCONV_fp32_bf16_mv_ups_wbf_eWHNoBypass, [OperandRegClass<1, eWH>]>,
ItinRegClassPair<II_VCONV_fp32_bf16_mv_ups_wbf, [OperandRegClass<1, eWL>]>
] in
// END MANUAL ADDITION
def VCONV_fp32_bf16_mv_ups_wbf : AIE2P_inst_mv_instr32 <(outs OP_mBMm:$dst), (ins OP_mWm:$src), "vconv.fp32.bf16", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_wbf
bits<5> dst;
bits<5> src;

let mv = {dst, dontcare{1}, src, 0b00110010111};
}

let Itinerary = II_VCONV_fp32_bf16_mv_ups_xbf in
def VCONV_fp32_bf16_mv_ups_xbf : AIE2P_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mXm:$src), "vconv.fp32.bf16", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_xbf
bits<4> dst;
bits<4> src;

let mv = {dst, dontcare{2-1}, src, 0b110001010111};
}

let Itinerary = II_VEQZ_16 in
def VEQZ_16 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXn:$s2), "veqz.16", " $cmp, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out32__mRS16m__vec_add_nX
bits<4> cmp;
bits<4> s2;

let mv = {cmp, s2, 0b01, 0b001001010111};
}

let Itinerary = II_VEQZ_32 in
def VEQZ_32 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXn:$s2), "veqz.32", " $cmp, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out16__mRS16m__vec_add_nX
bits<4> cmp;
bits<4> s2;

let mv = {cmp, s2, 0b10, 0b001001010111};
}

let Itinerary = II_VEQZ_8 in
def VEQZ_8 : AIE2P_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXn:$s2), "veqz.8", " $cmp, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out64__mLm__vec_add_nX
bits<4> cmp;
bits<4> s2;

let mv = {cmp, s2, 0b00, 0b001001010111};
}

let Itinerary = II_VEXP2 in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VEXP2_eWHNoBypass, [OperandRegClass<0, eWH>]>,
ItinRegClassPair<II_VEXP2, [OperandRegClass<0, eWL>]>
] in
// END MANUAL ADDITION
def VEXP2 : AIE2P_inst_mv_instr32 <(outs OP_mWm:$dst), (ins OP_mBMm:$src), "vexp2", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf__vexp2_subn
bits<5> dst;
bits<5> src;

let mv = {dst, src, 0b0, 0b11110010111};
}

let Itinerary = II_VEXTBCSTSHFL_128 in
def VEXTBCSTSHFL_128 : AIE2P_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, eR:$idx, mR29_insert:$mod), "vextbcstshfl.128", " $dst, $s1, $idx, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__mRm__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, idx, 0b00, 0b0011011};
}

let Itinerary = II_VEXTBCSTSHFL_16 in
def VEXTBCSTSHFL_16 : AIE2P_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, eR:$idx, mR29_insert:$mod), "vextbcstshfl.16", " $dst, $s1, $idx, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__s2v_wide2_16__mRm__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, idx, 0b01, 0b0011011};
}

let Itinerary = II_VEXTBCSTSHFL_32 in
def VEXTBCSTSHFL_32 : AIE2P_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, eR:$idx, mR29_insert:$mod), "vextbcstshfl.32", " $dst, $s1, $idx, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__s2v_wide2_32__mRm__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, idx, 0b10, 0b0011011};
}

let Itinerary = II_VEXTBCSTSHFL_64 in
def VEXTBCSTSHFL_64 : AIE2P_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, eR:$idx, mR29_insert:$mod), "vextbcstshfl.64", " $dst, $s1, $idx, r29">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_shuffle__s2v_wide2_64__mRm__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, idx, 0b11, 0b0011011};
}

let Itinerary = II_VEXTBCST_128_vec_extract_broadcast_imm in
def VEXTBCST_128_vec_extract_broadcast_imm : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.128", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_128
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b100, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_128_vec_extract_broadcast_r in
def VEXTBCST_128_vec_extract_broadcast_r : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eR:$idx), "vextbcst.128", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_128__mRm
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b100, idx, 0b110011};
}

let Itinerary = II_VEXTBCST_16_vec_extract_broadcast_imm in
def VEXTBCST_16_vec_extract_broadcast_imm : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.16", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_16
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b001, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_16_vec_extract_broadcast_r in
def VEXTBCST_16_vec_extract_broadcast_r : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eR:$idx), "vextbcst.16", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_16__mRm
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b001, idx, 0b110011};
}

let Itinerary = II_VEXTBCST_32_vec_extract_broadcast_imm in
def VEXTBCST_32_vec_extract_broadcast_imm : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.32", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_32
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b010, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_32_vec_extract_broadcast_r in
def VEXTBCST_32_vec_extract_broadcast_r : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eR:$idx), "vextbcst.32", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_32__mRm
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b010, idx, 0b110011};
}

let Itinerary = II_VEXTBCST_64_vec_extract_broadcast_imm in
def VEXTBCST_64_vec_extract_broadcast_imm : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.64", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_64
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b011, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_64_vec_extract_broadcast_r in
def VEXTBCST_64_vec_extract_broadcast_r : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eR:$idx), "vextbcst.64", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_64__mRm
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b011, idx, 0b110011};
}

let Itinerary = II_VEXTBCST_8_vec_extract_broadcast_imm in
def VEXTBCST_8_vec_extract_broadcast_imm : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextbcst.8", " $dst, $s1, #$idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_imm__vec_extract_broadcast_core__s2v_wide1_8
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b000, idx, 0b11010};
}

let Itinerary = II_VEXTBCST_8_vec_extract_broadcast_r in
def VEXTBCST_8_vec_extract_broadcast_r : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eR:$idx), "vextbcst.8", " $dst, $s1, $idx">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_broadcast_r__vec_extract_broadcast_core__s2v_wide1_8__mRm
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b000, idx, 0b110011};
}

let Itinerary = II_VEXTRACT_16_vec_extract_imm_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_16_vec_extract_imm_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.16", " $dst, $s1, #$idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__mRm__vaddSign0__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b01, 0b0, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_16_vec_extract_imm_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_16_vec_extract_imm_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.16", " $dst, $s1, #$idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__mRm__vaddSign1__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b01, 0b1, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_16_vec_extract_r_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_16_vec_extract_r_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, eR:$idx), "vextract.16", " $dst, $s1, $idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__mRm__vaddSign0
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b01, 0b0, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_16_vec_extract_r_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_16_vec_extract_r_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, eR:$idx), "vextract.16", " $dst, $s1, $idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_16__mRm__vaddSign1
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b01, 0b1, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_32_vec_extract_imm_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_32_vec_extract_imm_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.32", " $dst, $s1, #$idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__mRm__vaddSign0__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b10, 0b0, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_32_vec_extract_imm_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_32_vec_extract_imm_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.32", " $dst, $s1, #$idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__mRm__vaddSign1__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b10, 0b1, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_32_vec_extract_r_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_32_vec_extract_r_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, eR:$idx), "vextract.32", " $dst, $s1, $idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__mRm__vaddSign0
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b10, 0b0, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_32_vec_extract_r_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_32_vec_extract_r_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, eR:$idx), "vextract.32", " $dst, $s1, $idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_32__mRm__vaddSign1
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b10, 0b1, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_64_vec_extract_imm_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_64_vec_extract_imm_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eL:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.64", " $dst, $s1, #$idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign0__mIdxImm
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, dontcare{1}, s1, 0b11, 0b0, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_64_vec_extract_imm_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_64_vec_extract_imm_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eL:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.64", " $dst, $s1, #$idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign1__mIdxImm
bits<4> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, dontcare{1}, s1, 0b11, 0b1, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_64_vec_extract_r_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_64_vec_extract_r_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eL:$dst), (ins OP_mXm:$s1, eR:$idx), "vextract.64", " $dst, $s1, $idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign0__mRm
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, dontcare{1}, s1, 0b11, 0b0, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_64_vec_extract_r_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_64_vec_extract_r_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eL:$dst), (ins OP_mXm:$s1, eR:$idx), "vextract.64", " $dst, $s1, $idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_L__s2v_wide_64__mLm__vaddSign1__mRm
bits<4> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, dontcare{1}, s1, 0b11, 0b1, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_8_vec_extract_imm_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_8_vec_extract_imm_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.8", " $dst, $s1, #$idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__mRm__vaddSign0__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b00, 0b0, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_8_vec_extract_imm_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_8_vec_extract_imm_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, c6u:$idx), "vextract.8", " $dst, $s1, #$idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_imm__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__mRm__vaddSign1__mIdxImm
bits<5> dst;
bits<4> s1;
bits<6> idx;

let mv = {dst, s1, 0b00, 0b1, idx, 0b1101};
}

let Itinerary = II_VEXTRACT_8_vec_extract_r_vaddSign0, Uses = [vaddSign0] in
def VEXTRACT_8_vec_extract_r_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, eR:$idx), "vextract.8", " $dst, $s1, $idx, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__mRm__vaddSign0
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b00, 0b0, idx, 0b00110};
}

let Itinerary = II_VEXTRACT_8_vec_extract_r_vaddSign1, Uses = [vaddSign1] in
def VEXTRACT_8_vec_extract_r_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eR:$dst), (ins OP_mXm:$s1, eR:$idx), "vextract.8", " $dst, $s1, $idx, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_extract_r__vec_extract_core__vec_extract_dest__vec_extract_dest_R__s2v_wide_8__mRm__vaddSign1
bits<5> dst;
bits<4> s1;
bits<5> idx;

let mv = {dst, s1, 0b00, 0b1, idx, 0b00110};
}

let Itinerary = II_VFLOOR_s32_bf16_mv_float_to_int_bm, Defs = [srF2IFlags], Uses = [crF2IMask] in
def VFLOOR_s32_bf16_mv_float_to_int_bm : AIE2P_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mBMs:$src, c1u:$imm, eS:$shft), "vfloor.s32.bf16", "$dst, $src[$imm], $shft">{
// id: me__instr128_or__st__mv_conv__mv_float_to_int_bm__mv_float_to_int__mSs__mFl2FxSrc_BM
bits<4> dst;
bits<2> shft;
bits<1> imm;
bits<5> src;

let st = {dst, 0b1, 0b1, shft, imm, src, 0b1, 0b01100};
}

let Itinerary = II_VFLOOR_s32_bf16_mv_float_to_int_w, Defs = [srF2IFlags], Uses = [crF2IMask] in
def VFLOOR_s32_bf16_mv_float_to_int_w : AIE2P_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mFl2FxSrc_W:$src, eS:$shft), "vfloor.s32.bf16", "$dst, $src, $shft">{
// id: me__instr128_or__st__mv_conv__mv_float_to_int_w__mv_float_to_int__mSs
bits<4> dst;
bits<2> shft;
bits<5> src;

let st = {dst, 0b0, 0b1, shft, dontcare{1}, src, 0b1, 0b01100};
}

let Itinerary = II_VGE_16_vaddSign0, Uses = [vaddSign0] in
def VGE_16_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.16", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out32__mRS16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b0, 0b01, 0b010011};
}

let Itinerary = II_VGE_16_vaddSign1, Uses = [vaddSign1] in
def VGE_16_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.16", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out32__mRS16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b1, 0b01, 0b010011};
}

let Itinerary = II_VGE_32_vaddSign0, Uses = [vaddSign0] in
def VGE_32_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.32", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out16__mRS16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b0, 0b10, 0b010011};
}

let Itinerary = II_VGE_32_vaddSign1, Uses = [vaddSign1] in
def VGE_32_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.32", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out16__mRS16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b1, 0b10, 0b010011};
}

let Itinerary = II_VGE_8_vaddSign0, Uses = [vaddSign0] in
def VGE_8_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.8", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b0, 0b00, 0b010011};
}

let Itinerary = II_VGE_8_vaddSign1, Uses = [vaddSign1] in
def VGE_8_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.8", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b1, 0b00, 0b010011};
}

let Itinerary = II_VGE_bf16 in
def VGE_bf16 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vge.bf16", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_bf_compare__vge_bf__vec_cmp_out32__mRS16m__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b1, 0b001100111};
}

let Itinerary = II_VINSERT_16_mIdxImm0 in
def VINSERT_16_mIdxImm0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eR:$src), "vinsert.16", " $dst, $s1, #0, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_16__mRm__mInsertIdxRed__mIdxImm0
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b0, src, 0b01, 0b001011};
}

let Itinerary = II_VINSERT_16_mR29_insert in
def VINSERT_16_mR29_insert : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, mR29_insert:$idx, eR:$src), "vinsert.16", " $dst, $s1, r29, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_16__mRm__mInsertIdxRed__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b1, src, 0b01, 0b001011};
}

let Itinerary = II_VINSERT_32_mIdxImm0 in
def VINSERT_32_mIdxImm0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eR:$src), "vinsert.32", " $dst, $s1, #0, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_32__mRm__mInsertIdxRed__mIdxImm0
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b0, src, 0b10, 0b001011};
}

let Itinerary = II_VINSERT_32_mR29_insert in
def VINSERT_32_mR29_insert : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, mR29_insert:$idx, eR:$src), "vinsert.32", " $dst, $s1, r29, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_32__mRm__mInsertIdxRed__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b1, src, 0b10, 0b001011};
}

let Itinerary = II_VINSERT_64_mIdxImm0 in
def VINSERT_64_mIdxImm0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eL:$src), "vinsert.64", " $dst, $s1, #0, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mInsertIdxRed__mIdxImm0
bits<4> dst;
bits<4> s1;
bits<4> src;

let mv = {dst, s1, 0b0, src, dontcare{1}, 0b11, 0b001011};
}

let Itinerary = II_VINSERT_64_mR29_insert in
def VINSERT_64_mR29_insert : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, mR29_insert:$idx, eL:$src), "vinsert.64", " $dst, $s1, r29, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_L__s2v_wide_64__mLm__mInsertIdxRed__mR29_insert
bits<4> dst;
bits<4> s1;
bits<4> src;

let mv = {dst, s1, 0b1, src, dontcare{1}, 0b11, 0b001011};
}

let Itinerary = II_VINSERT_8_mIdxImm0 in
def VINSERT_8_mIdxImm0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, eR:$src), "vinsert.8", " $dst, $s1, #0, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_8__mRm__mInsertIdxRed__mIdxImm0
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b0, src, 0b00, 0b001011};
}

let Itinerary = II_VINSERT_8_mR29_insert in
def VINSERT_8_mR29_insert : AIE2P_inst_mv_instr32 <(outs OP_mXm:$dst), (ins OP_mXm:$s1, mR29_insert:$idx, eR:$src), "vinsert.8", " $dst, $s1, r29, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_insert__vec_insert_src__vec_insert_src_R__s2v_wide_8__mRm__mInsertIdxRed__mR29_insert
bits<4> dst;
bits<4> s1;
bits<5> src;

let mv = {dst, s1, 0b1, src, 0b00, 0b001011};
}

let Itinerary = II_VLDA_128_dmv_lda_w_idx, mayLoad = true in
def VLDA_128_dmv_lda_w_idx : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst), (ins eP:$ptr, eDJ:$dj), "vlda.128", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b000011};
}

let Itinerary = II_VLDA_128_dmv_lda_w_idx_imm, mayLoad = true in
def VLDA_128_dmv_lda_w_idx_imm : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst), (ins eP:$ptr, c8s_step16:$imm), "vlda.128", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__idx_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b000011};
}

let Itinerary = II_VLDA_128_dmv_lda_w_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_128_dmv_lda_w_pstm_nrm : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda.128", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b000011};
}

let Itinerary = II_VLDA_128_dmv_lda_w_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_128_dmv_lda_w_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out), (ins eP:$ptr, c8s_step16:$imm), "vlda.128", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_nrm_imm__mPa__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b000011};
}

let Itinerary = II_VLDA_128_dmv_lda_w_spill, mayLoad = true, Uses = [sp] in
def VLDA_128_dmv_lda_w_spill : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst), (ins c14n_step16:$imm), "vlda.128", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmv_lda_w_spill__agua_dmv__nrm_spill__agua_dmv__spill__mSPa__m_c14n_step16
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b100011};
}

let Itinerary = II_VLDA_2D_128, DecoderMethod = "DecodeDmv_lda_w_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_128 : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d.128", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b000011};
}

let Itinerary = II_VLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf, DecoderMethod = "DecodeDmw_lda_ups_bf_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_CONV_fp32_bf16_dmw_lda_ups_bf : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$op, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d.conv.fp32.bf16", " $op, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_bf__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_bf_core
bits<3> ptr;
bits<3> mod;
bits<5> op;

let lda = {ptr, mod, 0b100, op, 0b010011};
}

let Itinerary = II_VLDA_2D_CONV_fp32_bf16_dmx_lda_ups_bf, DecoderMethod = "DecodeDmx_lda_ups_bf_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_CONV_fp32_bf16_dmx_lda_ups_bf : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$op, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d.conv.fp32.bf16", " $op, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_bf__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_bf_core
bits<3> ptr;
bits<3> mod;
bits<4> op;

let lda = {ptr, mod, 0b100, op, 0b1010111};
}

let Itinerary = II_VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0, DecoderMethod = "DecodeDmw_lda_ups_w2b_2D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$su, eP:$ptr, eD:$mod), "vlda.2d.ups.2x", " $dst, $su, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<5> dst;
bits<2> su;

let lda = {ptr, mod, 0b100, dst, su, 0b0, 0b001};
}

let Itinerary = II_VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1, DecoderMethod = "DecodeDmw_lda_ups_w2b_2D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_2x_dmw_lda_ups_w2b_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$su, eP:$ptr, eD:$mod), "vlda.2d.ups.2x", " $dst, $su, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<5> dst;
bits<2> su;

let lda = {ptr, mod, 0b100, dst, su, 0b1, 0b001};
}

let Itinerary = II_VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0, DecoderMethod = "DecodeDmx_lda_ups_x2c_2D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$su, eP:$ptr, eD:$mod), "vlda.2d.ups.2x", " $dst, $su, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, mod, 0b100, dst, su, 0b0, 0b0110};
}

let Itinerary = II_VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1, DecoderMethod = "DecodeDmx_lda_ups_x2c_2D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_2x_dmx_lda_ups_x2c_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$su, eP:$ptr, eD:$mod), "vlda.2d.ups.2x", " $dst, $su, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, mod, 0b100, dst, su, 0b1, 0b0110};
}

let Itinerary = II_VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0, DecoderMethod = "DecodeDmw_lda_ups_w2c_2D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$su, eP:$ptr, eD:$mod), "vlda.2d.ups.4x", " $dst, $su, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, mod, 0b100, dst, su, 0b0, 0b1010};
}

let Itinerary = II_VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1, DecoderMethod = "DecodeDmw_lda_ups_w2c_2D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_4x_dmw_lda_ups_w2c_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$su, eP:$ptr, eD:$mod), "vlda.2d.ups.4x", " $dst, $su, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, mod, 0b100, dst, su, 0b1, 0b1010};
}

let Itinerary = II_VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign0, DecoderMethod = "DecodeDmx_lda_ups_x2d_2D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign0 : AIE2P_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$su, eP:$ptr, eD:$mod), "vlda.2d.ups.4x", " $dst, $su, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<3> dst;
bits<2> su;

let lda = {ptr, mod, 0b100, dst, su, 0b0, 0b11110};
}

let Itinerary = II_VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign1, DecoderMethod = "DecodeDmx_lda_ups_x2d_2D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_2D_UPS_4x_dmx_lda_ups_x2d_upsSign1 : AIE2P_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out, eDC:$dc), (ins eS:$su, eP:$ptr, eD:$mod), "vlda.2d.ups.4x", " $dst, $su, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<3> dst;
bits<2> su;

let lda = {ptr, mod, 0b100, dst, su, 0b1, 0b11110};
}

let Itinerary = II_VLDA_2D_dmw_lda_w, DecoderMethod = "DecodeDmw_lda_w_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_dmw_lda_w : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_w__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b110011};
}

let Itinerary = II_VLDA_2D_dmx_lda_bm, DecoderMethod = "DecodeDmx_lda_bm_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_dmx_lda_bm : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_bm__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b100, dst, 0b101011};
}

let Itinerary = II_VLDA_2D_dmx_lda_fifohl, ItineraryRegPairs = [
ItinRegClassPair<II_VLDA_2D_dmx_lda_fifohl_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VLDA_2D_dmx_lda_fifohl_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VLDA_2D_dmx_lda_fifohl_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VLDA_2D_dmx_lda_fifohl_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VLDA_2D_dmx_lda_fifohl_mStFifoh, [OperandRegClass<0, mStFifoh>]>], DecoderMethod = "DecodeDmx_lda_fifohl_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_dmx_lda_fifohl : AIE2P_inst_lda_instr32 <(outs OP_mFifoHLReg:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifohl__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let lda = {ptr, mod, 0b100, dst, 0b00001111};
}

let Itinerary = II_VLDA_2D_dmx_lda_x, DecoderMethod = "DecodeDmx_lda_x_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_2D_dmx_lda_x : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vlda.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_x__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_2d__mPa__mDa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b100, dst, 0b0110111};
}

let Itinerary = II_VLDA_3D_128, DecoderMethod = "DecodeDmv_lda_w_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_128 : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d.128", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmv_lda_w__agua_dmv__nrm__agua_dmv__normal__agua_dmv__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b000011};
}

let Itinerary = II_VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf, DecoderMethod = "DecodeDmw_lda_ups_bf_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_CONV_fp32_bf16_dmw_lda_ups_bf : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$op, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d.conv.fp32.bf16", " $op, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_bf__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_bf_core
bits<3> ptr;
bits<2> mod;
bits<5> op;

let lda = {ptr, dontcare{1}, mod, 0b110, op, 0b010011};
}

let Itinerary = II_VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_bf, DecoderMethod = "DecodeDmx_lda_ups_bf_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_CONV_fp32_bf16_dmx_lda_ups_bf : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$op, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d.conv.fp32.bf16", " $op, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_bf__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_bf_core
bits<3> ptr;
bits<2> mod;
bits<4> op;

let lda = {ptr, dontcare{1}, mod, 0b110, op, 0b1010111};
}

let Itinerary = II_VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0, DecoderMethod = "DecodeDmw_lda_ups_w2b_3D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$su, eP:$ptr, eDS:$mod), "vlda.3d.ups.2x", " $dst, $su, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<2> mod;
bits<5> dst;
bits<2> su;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, su, 0b0, 0b001};
}

let Itinerary = II_VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1, DecoderMethod = "DecodeDmw_lda_ups_w2b_3D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_2x_dmw_lda_ups_w2b_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$su, eP:$ptr, eDS:$mod), "vlda.3d.ups.2x", " $dst, $su, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<2> mod;
bits<5> dst;
bits<2> su;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, su, 0b1, 0b001};
}

let Itinerary = II_VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0, DecoderMethod = "DecodeDmx_lda_ups_x2c_3D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$su, eP:$ptr, eDS:$mod), "vlda.3d.ups.2x", " $dst, $su, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<2> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, su, 0b0, 0b0110};
}

let Itinerary = II_VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1, DecoderMethod = "DecodeDmx_lda_ups_x2c_3D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_2x_dmx_lda_ups_x2c_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$su, eP:$ptr, eDS:$mod), "vlda.3d.ups.2x", " $dst, $su, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<2> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, su, 0b1, 0b0110};
}

let Itinerary = II_VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0, DecoderMethod = "DecodeDmw_lda_ups_w2c_3D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$su, eP:$ptr, eDS:$mod), "vlda.3d.ups.4x", " $dst, $su, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<2> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, su, 0b0, 0b1010};
}

let Itinerary = II_VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1, DecoderMethod = "DecodeDmw_lda_ups_w2c_3D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_4x_dmw_lda_ups_w2c_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$su, eP:$ptr, eDS:$mod), "vlda.3d.ups.4x", " $dst, $su, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<2> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, su, 0b1, 0b1010};
}

let Itinerary = II_VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0, DecoderMethod = "DecodeDmx_lda_ups_x2d_3D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign0 : AIE2P_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$su, eP:$ptr, eDS:$mod), "vlda.3d.ups.4x", " $dst, $su, upssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<2> mod;
bits<3> dst;
bits<2> su;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, su, 0b0, 0b11110};
}

let Itinerary = II_VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1, DecoderMethod = "DecodeDmx_lda_ups_x2d_3D", mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_3D_UPS_4x_dmx_lda_ups_x2d_upsSign1 : AIE2P_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eS:$su, eP:$ptr, eDS:$mod), "vlda.3d.ups.4x", " $dst, $su, upssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<2> mod;
bits<3> dst;
bits<2> su;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, su, 0b1, 0b11110};
}

let Itinerary = II_VLDA_3D_dmw_lda_w, DecoderMethod = "DecodeDmw_lda_w_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_dmw_lda_w : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmw_lda_w__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b110011};
}

let Itinerary = II_VLDA_3D_dmx_lda_bm, DecoderMethod = "DecodeDmx_lda_bm_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_dmx_lda_bm : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_bm__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b101011};
}

let Itinerary = II_VLDA_3D_dmx_lda_fifohl, ItineraryRegPairs = [
ItinRegClassPair<II_VLDA_3D_dmx_lda_fifohl_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VLDA_3D_dmx_lda_fifohl_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VLDA_3D_dmx_lda_fifohl_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VLDA_3D_dmx_lda_fifohl_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VLDA_3D_dmx_lda_fifohl_mStFifoh, [OperandRegClass<0, mStFifoh>]>], DecoderMethod = "DecodeDmx_lda_fifohl_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_dmx_lda_fifohl : AIE2P_inst_lda_instr32 <(outs OP_mFifoHLReg:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifohl__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<3> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b00001111};
}

let Itinerary = II_VLDA_3D_dmx_lda_x, DecoderMethod = "DecodeDmx_lda_x_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_3D_dmx_lda_x : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vlda.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_x__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_3d__mPa__mDSa
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let lda = {ptr, dontcare{1}, mod, 0b110, dst, 0b0110111};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_idx, mayLoad = true in
def VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_idx : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$op), (ins eP:$ptr, eDJ:$dj), "vlda.conv.fp32.bf16", " $op, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_bf__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_bf_core
bits<3> ptr;
bits<3> dj;
bits<5> op;

let lda = {ptr, dj, 0b000, op, 0b010011};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_idx_imm, mayLoad = true in
def VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_idx_imm : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$op), (ins eP:$ptr, c9s_step32:$imm), "vlda.conv.fp32.bf16", " $op, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_bf__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_bf_core
bits<3> ptr;
bits<4> imm;
bits<5> op;

let lda = {ptr, imm, 0b01, op, 0b010011};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_pstm_nrm : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$op, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda.conv.fp32.bf16", " $op, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_bf__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_bf_core
bits<3> ptr;
bits<3> mod;
bits<5> op;

let lda = {ptr, mod, 0b010, op, 0b010011};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_bf16_dmw_lda_ups_bf_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$op, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vlda.conv.fp32.bf16", " $op, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_bf__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_bf_core
bits<3> ptr;
bits<4> imm;
bits<5> op;

let lda = {ptr, imm, 0b11, op, 0b010011};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_idx, mayLoad = true in
def VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_idx : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$op), (ins eP:$ptr, eDJ:$dj), "vlda.conv.fp32.bf16", " $op, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_bf__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_bf_core
bits<3> ptr;
bits<3> dj;
bits<4> op;

let lda = {ptr, dj, 0b000, op, 0b1010111};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_idx_imm, mayLoad = true in
def VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_idx_imm : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$op), (ins eP:$ptr, c10s_step64:$imm), "vlda.conv.fp32.bf16", " $op, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_bf__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_bf_core
bits<3> ptr;
bits<4> imm;
bits<4> op;

let lda = {ptr, imm, 0b01, op, 0b1010111};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_pstm_nrm : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$op, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda.conv.fp32.bf16", " $op, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_bf__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_bf_core
bits<3> ptr;
bits<3> mod;
bits<4> op;

let lda = {ptr, mod, 0b010, op, 0b1010111};
}

let Itinerary = II_VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_CONV_fp32_bf16_dmx_lda_ups_bf_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$op, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vlda.conv.fp32.bf16", " $op, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_bf__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_bf_core
bits<3> ptr;
bits<4> imm;
bits<4> op;

let lda = {ptr, imm, 0b11, op, 0b1010111};
}

let Itinerary = II_VLDA_FILL_512, DecoderMethod = "DecodeVLDA_FILL_512", mayLoad = true in
def VLDA_FILL_512 : AIE2P_inst_lda_instr32 <(outs ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vlda.fill.512", " [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__normal_fill__dmx_lda_fifo_x__core_fill__dmx_lda_fifo_x__normal__mPfa__mLdFifo_a__mRF2a
bits<1> ptr;

let lda = {0b00, ptr, dontcare{3-1}, 0b101, dontcare{4-1}, 0b0010111};
}

let Itinerary = II_VLDA_POP_512_2D, DecoderMethod = "DecodeDmx_lda_fifo_x__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_512_2D : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vlda.pop.512.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__fifo_2d_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__fifo_2d__mPfa__mLdFifo_a__mRF2a__mDa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b01, ptr, mod, 0b100, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_512_3D, DecoderMethod = "DecodeDmx_lda_fifo_x__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_512_3D : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vlda.pop.512.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__fifo_3d_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__fifo_3d__mPfa__mLdFifo_a__mRF2a__mDSa
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let lda = {0b11, ptr, dontcare{1}, mod, 0b000, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_512_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_512_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_512_fifo_1d_pop : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vlda.pop.512", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__fifo_1d_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__fifo_1d__mPfa__mLdFifo_a__mRF2a__mMa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b10, ptr, mod, 0b010, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_512_normal_pop, DecoderMethod = "DecodeVLDA_POP_512_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_512_normal_pop : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vlda.pop.512", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_x__normal_pop__dmx_lda_fifo_x__core_pop__dmx_lda_fifo_x__normal__mPfa__mLdFifo_a__mRF2a
bits<1> ptr;
bits<4> dst;

let lda = {0b00, ptr, dontcare{3-1}, 0b000, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_544_2D, DecoderMethod = "DecodeDmx_lda_fifo_ex_ebs16__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_544_2D : AIE2P_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vlda.pop.544.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_ebs16__fifo_2d_pop__dmx_lda_fifo_ex_ebs16__core_pop__dmx_lda_fifo_ex_ebs16__fifo_2d__mPfa__mLdFifo_a__mRF2a__mDa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b00, ptr, mod, 0b010, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_544_3D, DecoderMethod = "DecodeDmx_lda_fifo_ex_ebs16__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_544_3D : AIE2P_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vlda.pop.544.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_ebs16__fifo_3d_pop__dmx_lda_fifo_ex_ebs16__core_pop__dmx_lda_fifo_ex_ebs16__fifo_3d__mPfa__mLdFifo_a__mRF2a__mDSa
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let lda = {0b10, ptr, dontcare{1}, mod, 0b100, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_544_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_544_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_544_fifo_1d_pop : AIE2P_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vlda.pop.544", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_ebs16__fifo_1d_pop__dmx_lda_fifo_ex_ebs16__core_pop__dmx_lda_fifo_ex_ebs16__fifo_1d__mPfa__mLdFifo_a__mRF2a__mMa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b11, ptr, mod, 0b010, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_544_normal_pop, DecoderMethod = "DecodeVLDA_POP_544_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_544_normal_pop : AIE2P_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vlda.pop.544", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_ebs16__normal_pop__dmx_lda_fifo_ex_ebs16__core_pop__dmx_lda_fifo_ex_ebs16__normal__mPfa__mLdFifo_a__mRF2a
bits<1> ptr;
bits<4> dst;

let lda = {0b01, ptr, dontcare{3-1}, 0b000, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_576_2D, DecoderMethod = "DecodeDmx_lda_fifo_ex_ebs8__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_576_2D : AIE2P_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vlda.pop.576.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_ebs8__fifo_2d_pop__dmx_lda_fifo_ex_ebs8__core_pop__dmx_lda_fifo_ex_ebs8__fifo_2d__mPfa__mLdFifo_a__mRF2a__mDa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b11, ptr, mod, 0b100, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_576_3D, DecoderMethod = "DecodeDmx_lda_fifo_ex_ebs8__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_576_3D : AIE2P_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vlda.pop.576.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_ebs8__fifo_3d_pop__dmx_lda_fifo_ex_ebs8__core_pop__dmx_lda_fifo_ex_ebs8__fifo_3d__mPfa__mLdFifo_a__mRF2a__mDSa
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let lda = {0b00, ptr, dontcare{1}, mod, 0b100, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_576_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_576_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_576_fifo_1d_pop : AIE2P_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vlda.pop.576", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_ebs8__fifo_1d_pop__dmx_lda_fifo_ex_ebs8__core_pop__dmx_lda_fifo_ex_ebs8__fifo_1d__mPfa__mLdFifo_a__mRF2a__mMa
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let lda = {0b01, ptr, mod, 0b010, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_576_normal_pop, DecoderMethod = "DecodeVLDA_POP_576_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_576_normal_pop : AIE2P_inst_lda_instr32 <(outs OP_mEXa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vlda.pop.576", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_ex_ebs8__normal_pop__dmx_lda_fifo_ex_ebs8__core_pop__dmx_lda_fifo_ex_ebs8__normal__mPfa__mLdFifo_a__mRF2a
bits<1> ptr;
bits<4> dst;

let lda = {0b10, ptr, dontcare{3-1}, 0b000, dst, 0b0010111};
}

let Itinerary = II_VLDA_POP_640_2D, DecoderMethod = "DecodeDmx_lda_fifo_qx__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_640_2D : AIE2P_inst_lda_instr32 <(outs OP_mQXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vlda.pop.640.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__fifo_2d_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__fifo_2d__mPfa__mLdFifo_a__mRF2a__mDa
bits<1> ptr;
bits<3> mod;
bits<2> dst;

let lda = {0b00, ptr, mod, 0b001, dst, dontcare{2-1}, 0b0010111};
}

let Itinerary = II_VLDA_POP_640_3D, DecoderMethod = "DecodeDmx_lda_fifo_qx__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_640_3D : AIE2P_inst_lda_instr32 <(outs OP_mQXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vlda.pop.640.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__fifo_3d_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__fifo_3d__mPfa__mLdFifo_a__mRF2a__mDSa
bits<1> ptr;
bits<2> mod;
bits<2> dst;

let lda = {0b01, ptr, dontcare{1}, mod, 0b110, dst, dontcare{2-1}, 0b0010111};
}

let Itinerary = II_VLDA_POP_640_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_640_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_640_fifo_1d_pop : AIE2P_inst_lda_instr32 <(outs OP_mQXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vlda.pop.640", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__fifo_1d_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__fifo_1d__mPfa__mLdFifo_a__mRF2a__mMa
bits<1> ptr;
bits<3> mod;
bits<2> dst;

let lda = {0b01, ptr, mod, 0b001, dst, dontcare{2-1}, 0b0010111};
}

let Itinerary = II_VLDA_POP_640_normal_pop, DecoderMethod = "DecodeVLDA_POP_640_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_640_normal_pop : AIE2P_inst_lda_instr32 <(outs OP_mQXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vlda.pop.640", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qx__normal_pop__dmx_lda_fifo_qx__core_pop__dmx_lda_fifo_qx__normal__mPfa__mLdFifo_a__mRF2a
bits<1> ptr;
bits<2> dst;

let lda = {0b00, ptr, dontcare{3-1}, 0b110, dst, dontcare{2-1}, 0b0010111};
}

let Itinerary = II_VLDA_POP_704_2D, DecoderMethod = "DecodeDmx_lda_fifo_qex_ebs16__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_704_2D : AIE2P_inst_lda_instr32 <(outs OP_mQEXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vlda.pop.704.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qex_ebs16__fifo_2d_pop__dmx_lda_fifo_qex_ebs16__core_pop__dmx_lda_fifo_qex_ebs16__fifo_2d__mPfa__mLdFifo_a__mRF2a__mDa
bits<1> ptr;
bits<3> mod;
bits<2> dst;

let lda = {0b10, ptr, mod, 0b001, dst, dontcare{2-1}, 0b0010111};
}

let Itinerary = II_VLDA_POP_704_3D, DecoderMethod = "DecodeDmx_lda_fifo_qex_ebs16__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_704_3D : AIE2P_inst_lda_instr32 <(outs OP_mQEXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vlda.pop.704.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qex_ebs16__fifo_3d_pop__dmx_lda_fifo_qex_ebs16__core_pop__dmx_lda_fifo_qex_ebs16__fifo_3d__mPfa__mLdFifo_a__mRF2a__mDSa
bits<1> ptr;
bits<2> mod;
bits<2> dst;

let lda = {0b11, ptr, dontcare{1}, mod, 0b110, dst, dontcare{2-1}, 0b0010111};
}

let Itinerary = II_VLDA_POP_704_fifo_1d_pop, DecoderMethod = "DecodeVLDA_POP_704_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_704_fifo_1d_pop : AIE2P_inst_lda_instr32 <(outs OP_mQEXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vlda.pop.704", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qex_ebs16__fifo_1d_pop__dmx_lda_fifo_qex_ebs16__core_pop__dmx_lda_fifo_qex_ebs16__fifo_1d__mPfa__mLdFifo_a__mRF2a__mMa
bits<1> ptr;
bits<3> mod;
bits<2> dst;

let lda = {0b11, ptr, mod, 0b001, dst, dontcare{2-1}, 0b0010111};
}

let Itinerary = II_VLDA_POP_704_normal_pop, DecoderMethod = "DecodeVLDA_POP_704_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDA_POP_704_normal_pop : AIE2P_inst_lda_instr32 <(outs OP_mQEXsa:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vlda.pop.704", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__lda__dmx_lda_fifo__dmx_lda_fifo_qex_ebs16__normal_pop__dmx_lda_fifo_qex_ebs16__core_pop__dmx_lda_fifo_qex_ebs16__normal__mPfa__mLdFifo_a__mRF2a
bits<1> ptr;
bits<2> dst;

let lda = {0b10, ptr, dontcare{3-1}, 0b110, dst, dontcare{2-1}, 0b0010111};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eS:$su, eP:$ptr, eDJ:$dj), "vlda.ups.2x", " $dst, $su, upssign0, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> dj;
bits<5> dst;
bits<2> su;

let lda = {ptr, dj, 0b000, dst, su, 0b0, 0b001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_2x_dmw_lda_ups_w2b_idx_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eS:$su, eP:$ptr, eDJ:$dj), "vlda.ups.2x", " $dst, $su, upssign1, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> dj;
bits<5> dst;
bits<2> su;

let lda = {ptr, dj, 0b000, dst, su, 0b1, 0b001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eS:$su, eP:$ptr, c9s_step32:$imm), "vlda.ups.2x", " $dst, $su, upssign0, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<5> dst;
bits<2> su;

let lda = {ptr, imm, 0b01, dst, su, 0b0, 0b001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_2x_dmw_lda_ups_w2b_idx_imm_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eS:$su, eP:$ptr, c9s_step32:$imm), "vlda.ups.2x", " $dst, $su, upssign1, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<5> dst;
bits<2> su;

let lda = {ptr, imm, 0b01, dst, su, 0b1, 0b001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, eM:$mod), "vlda.ups.2x", " $dst, $su, upssign0, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<5> dst;
bits<2> su;

let lda = {ptr, mod, 0b010, dst, su, 0b0, 0b001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, eM:$mod), "vlda.ups.2x", " $dst, $su, upssign1, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<5> dst;
bits<2> su;

let lda = {ptr, mod, 0b010, dst, su, 0b1, 0b001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, c9s_step32:$imm), "vlda.ups.2x", " $dst, $su, upssign0, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<5> dst;
bits<2> su;

let lda = {ptr, imm, 0b11, dst, su, 0b0, 0b001};
}

let Itinerary = II_VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmw_lda_ups_w2b_pstm_nrm_imm_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, c9s_step32:$imm), "vlda.ups.2x", " $dst, $su, upssign1, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_w2b__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2b_core__ups_lda__w2b__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<5> dst;
bits<2> su;

let lda = {ptr, imm, 0b11, dst, su, 0b1, 0b001};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$su, eP:$ptr, eDJ:$dj), "vlda.ups.2x", " $dst, $su, upssign0, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> dj;
bits<4> dst;
bits<2> su;

let lda = {ptr, dj, 0b000, dst, su, 0b0, 0b0110};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_2x_dmx_lda_ups_x2c_idx_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$su, eP:$ptr, eDJ:$dj), "vlda.ups.2x", " $dst, $su, upssign1, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> dj;
bits<4> dst;
bits<2> su;

let lda = {ptr, dj, 0b000, dst, su, 0b1, 0b0110};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$su, eP:$ptr, c10s_step64:$imm), "vlda.ups.2x", " $dst, $su, upssign0, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> su;

let lda = {ptr, imm, 0b01, dst, su, 0b0, 0b0110};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_2x_dmx_lda_ups_x2c_idx_imm_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$su, eP:$ptr, c10s_step64:$imm), "vlda.ups.2x", " $dst, $su, upssign1, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> su;

let lda = {ptr, imm, 0b01, dst, su, 0b1, 0b0110};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, eM:$mod), "vlda.ups.2x", " $dst, $su, upssign0, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, mod, 0b010, dst, su, 0b0, 0b0110};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, eM:$mod), "vlda.ups.2x", " $dst, $su, upssign1, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, mod, 0b010, dst, su, 0b1, 0b0110};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, c10s_step64:$imm), "vlda.ups.2x", " $dst, $su, upssign0, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> su;

let lda = {ptr, imm, 0b11, dst, su, 0b0, 0b0110};
}

let Itinerary = II_VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_2x_dmx_lda_ups_x2c_pstm_nrm_imm_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, c10s_step64:$imm), "vlda.ups.2x", " $dst, $su, upssign1, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_x2c__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2c_core__ups_lda__x2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> su;

let lda = {ptr, imm, 0b11, dst, su, 0b1, 0b0110};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$su, eP:$ptr, eDJ:$dj), "vlda.ups.4x", " $dst, $su, upssign0, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> dj;
bits<4> dst;
bits<2> su;

let lda = {ptr, dj, 0b000, dst, su, 0b0, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_4x_dmw_lda_ups_w2c_idx_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$su, eP:$ptr, eDJ:$dj), "vlda.ups.4x", " $dst, $su, upssign1, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> dj;
bits<4> dst;
bits<2> su;

let lda = {ptr, dj, 0b000, dst, su, 0b1, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$su, eP:$ptr, c9s_step32:$imm), "vlda.ups.4x", " $dst, $su, upssign0, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> su;

let lda = {ptr, imm, 0b01, dst, su, 0b0, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_4x_dmw_lda_ups_w2c_idx_imm_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins eS:$su, eP:$ptr, c9s_step32:$imm), "vlda.ups.4x", " $dst, $su, upssign1, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> su;

let lda = {ptr, imm, 0b01, dst, su, 0b1, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, eM:$mod), "vlda.ups.4x", " $dst, $su, upssign0, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, mod, 0b010, dst, su, 0b0, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, eM:$mod), "vlda.ups.4x", " $dst, $su, upssign1, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;
bits<2> su;

let lda = {ptr, mod, 0b010, dst, su, 0b1, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign0 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, c9s_step32:$imm), "vlda.ups.4x", " $dst, $su, upssign0, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> su;

let lda = {ptr, imm, 0b11, dst, su, 0b0, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmw_lda_ups_w2c_pstm_nrm_imm_upsSign1 : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, c9s_step32:$imm), "vlda.ups.4x", " $dst, $su, upssign1, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_ups_w2c__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32__dmw_lda_ups_w2c_core__ups_lda__w2c__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;
bits<2> su;

let lda = {ptr, imm, 0b11, dst, su, 0b1, 0b1010};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign0 : AIE2P_inst_lda_instr32 <(outs eDM:$dst), (ins eS:$su, eP:$ptr, eDJ:$dj), "vlda.ups.4x", " $dst, $su, upssign0, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> dj;
bits<3> dst;
bits<2> su;

let lda = {ptr, dj, 0b000, dst, su, 0b0, 0b11110};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_4x_dmx_lda_ups_x2d_idx_upsSign1 : AIE2P_inst_lda_instr32 <(outs eDM:$dst), (ins eS:$su, eP:$ptr, eDJ:$dj), "vlda.ups.4x", " $dst, $su, upssign1, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> dj;
bits<3> dst;
bits<2> su;

let lda = {ptr, dj, 0b000, dst, su, 0b1, 0b11110};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign0 : AIE2P_inst_lda_instr32 <(outs eDM:$dst), (ins eS:$su, eP:$ptr, c10s_step64:$imm), "vlda.ups.4x", " $dst, $su, upssign0, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<3> dst;
bits<2> su;

let lda = {ptr, imm, 0b01, dst, su, 0b0, 0b11110};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VLDA_UPS_4x_dmx_lda_ups_x2d_idx_imm_upsSign1 : AIE2P_inst_lda_instr32 <(outs eDM:$dst), (ins eS:$su, eP:$ptr, c10s_step64:$imm), "vlda.ups.4x", " $dst, $su, upssign1, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<3> dst;
bits<2> su;

let lda = {ptr, imm, 0b01, dst, su, 0b1, 0b11110};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign0 : AIE2P_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, eM:$mod), "vlda.ups.4x", " $dst, $su, upssign0, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<3> mod;
bits<3> dst;
bits<2> su;

let lda = {ptr, mod, 0b010, dst, su, 0b0, 0b11110};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_upsSign1 : AIE2P_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, eM:$mod), "vlda.ups.4x", " $dst, $su, upssign1, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<3> mod;
bits<3> dst;
bits<2> su;

let lda = {ptr, mod, 0b010, dst, su, 0b1, 0b11110};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign0, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign0 : AIE2P_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, c10s_step64:$imm), "vlda.ups.4x", " $dst, $su, upssign0, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign0
bits<3> ptr;
bits<4> imm;
bits<3> dst;
bits<2> su;

let lda = {ptr, imm, 0b11, dst, su, 0b0, 0b11110};
}

let Itinerary = II_VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign1, mayLoad = true, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1], Constraints = "$ptr_out = $ptr" in
def VLDA_UPS_4x_dmx_lda_ups_x2d_pstm_nrm_imm_upsSign1 : AIE2P_inst_lda_instr32 <(outs eDM:$dst, eP:$ptr_out), (ins eS:$su, eP:$ptr, c10s_step64:$imm), "vlda.ups.4x", " $dst, $su, upssign1, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_ups_x2d__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64__dmx_lda_ups_x2d_core__ups_lda__x2d__mDMm__ups_lda__base__mSa__upsSign1
bits<3> ptr;
bits<4> imm;
bits<3> dst;
bits<2> su;

let lda = {ptr, imm, 0b11, dst, su, 0b1, 0b11110};
}

let Itinerary = II_VLDA_dmw_lda_w_idx, mayLoad = true in
def VLDA_dmw_lda_w_idx : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst), (ins eP:$ptr, eDJ:$dj), "vlda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmw_lda_w__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b110011};
}

let Itinerary = II_VLDA_dmw_lda_w_idx_imm, mayLoad = true in
def VLDA_dmw_lda_w_idx_imm : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst), (ins eP:$ptr, c9s_step32:$imm), "vlda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_w__agua_dmw__nrm__agua_dmw__normal__agua_dmw__idx_imm__mPa__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b110011};
}

let Itinerary = II_VLDA_dmw_lda_w_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmw_lda_w_pstm_nrm : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmw_lda_w__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b110011};
}

let Itinerary = II_VLDA_dmw_lda_w_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmw_lda_w_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vlda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmw_lda_w__agua_dmw__nrm__agua_dmw__normal__agua_dmw__pstm_nrm_imm__mPa__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b110011};
}

let Itinerary = II_VLDA_dmw_lda_w_spill, mayLoad = true, Uses = [sp] in
def VLDA_dmw_lda_w_spill : AIE2P_inst_lda_instr32 <(outs OP_mWa:$dst), (ins c15n_step32:$imm), "vlda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmw_lda_w_spill__agua_dmw__nrm_spill__agua_dmw__spill__mSPa__m_c15n_step32
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b001011};
}

let Itinerary = II_VLDA_dmx_lda_bm_idx, mayLoad = true in
def VLDA_dmx_lda_bm_idx : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eP:$ptr, eDJ:$dj), "vlda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_bm__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let lda = {ptr, dj, 0b000, dst, 0b101011};
}

let Itinerary = II_VLDA_dmx_lda_bm_idx_imm, mayLoad = true in
def VLDA_dmx_lda_bm_idx_imm : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_bm__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b01, dst, 0b101011};
}

let Itinerary = II_VLDA_dmx_lda_bm_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_bm_pstm_nrm : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_bm__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let lda = {ptr, mod, 0b010, dst, 0b101011};
}

let Itinerary = II_VLDA_dmx_lda_bm_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_bm_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_bm__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let lda = {ptr, imm, 0b11, dst, 0b101011};
}

let Itinerary = II_VLDA_dmx_lda_bm_spill, mayLoad = true, Uses = [sp] in
def VLDA_dmx_lda_bm_spill : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins c16n_step64:$imm), "vlda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_bm_spill__agua_dmx__nrm_spill__agua_dmx__spill__mSPa__m_c16n_step64
bits<9> imm;
bits<5> dst;

let lda = {imm, dst, 0b011011};
}

let Itinerary = II_VLDA_dmx_lda_fifohl_idx, ItineraryRegPairs = [
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_mStFifoh, [OperandRegClass<0, mStFifoh>]>], mayLoad = true in
def VLDA_dmx_lda_fifohl_idx : AIE2P_inst_lda_instr32 <(outs OP_mFifoHLReg:$dst), (ins eP:$ptr, eDJ:$dj), "vlda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_fifohl__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<3> dst;

let lda = {ptr, dj, 0b000, dst, 0b00001111};
}

let Itinerary = II_VLDA_dmx_lda_fifohl_idx_imm, ItineraryRegPairs = [
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_imm_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_imm_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_imm_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_imm_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_idx_imm_mStFifoh, [OperandRegClass<0, mStFifoh>]>], mayLoad = true in
def VLDA_dmx_lda_fifohl_idx_imm : AIE2P_inst_lda_instr32 <(outs OP_mFifoHLReg:$dst), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_fifohl__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let lda = {ptr, imm, 0b01, dst, 0b00001111};
}

let Itinerary = II_VLDA_dmx_lda_fifohl_pstm_nrm, ItineraryRegPairs = [
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_mStFifoh, [OperandRegClass<0, mStFifoh>]>], mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_fifohl_pstm_nrm : AIE2P_inst_lda_instr32 <(outs OP_mFifoHLReg:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_fifohl__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let lda = {ptr, mod, 0b010, dst, 0b00001111};
}

let Itinerary = II_VLDA_dmx_lda_fifohl_pstm_nrm_imm, ItineraryRegPairs = [
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_imm_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_imm_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_imm_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_imm_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_pstm_nrm_imm_mStFifoh, [OperandRegClass<0, mStFifoh>]>], mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_fifohl_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs OP_mFifoHLReg:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_fifohl__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let lda = {ptr, imm, 0b11, dst, 0b00001111};
}

let Itinerary = II_VLDA_dmx_lda_fifohl_spill, ItineraryRegPairs = [
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_spill_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_spill_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_spill_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_spill_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VLDA_dmx_lda_fifohl_spill_mStFifoh, [OperandRegClass<0, mStFifoh>]>], mayLoad = true, Uses = [sp] in
def VLDA_dmx_lda_fifohl_spill : AIE2P_inst_lda_instr32 <(outs OP_mFifoHLReg:$dst), (ins c16n_step64:$imm), "vlda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_fifohl_spill__agua_dmx__nrm_spill__agua_dmx__spill__mSPa__m_c16n_step64
bits<9> imm;
bits<3> dst;

let lda = {imm, dst, 0b10001111};
}

let Itinerary = II_VLDA_dmx_lda_x_idx, mayLoad = true in
def VLDA_dmx_lda_x_idx : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst), (ins eP:$ptr, eDJ:$dj), "vlda", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__lda__dmx_lda_x__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx__mPa__mDJa
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let lda = {ptr, dj, 0b000, dst, 0b0110111};
}

let Itinerary = II_VLDA_dmx_lda_x_idx_imm, mayLoad = true in
def VLDA_dmx_lda_x_idx_imm : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_x__agua_dmx__nrm__agua_dmx__normal__agua_dmx__idx_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b01, dst, 0b0110111};
}

let Itinerary = II_VLDA_dmx_lda_x_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_x_pstm_nrm : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vlda", " $dst, [$ptr], $mod">{
// id: me__instr128_or__lda__dmx_lda_x__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm__mPa__mMa
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let lda = {ptr, mod, 0b010, dst, 0b0110111};
}

let Itinerary = II_VLDA_dmx_lda_x_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDA_dmx_lda_x_pstm_nrm_imm : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vlda", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__lda__dmx_lda_x__agua_dmx__nrm__agua_dmx__normal__agua_dmx__pstm_nrm_imm__mPa__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let lda = {ptr, imm, 0b11, dst, 0b0110111};
}

let Itinerary = II_VLDA_dmx_lda_x_spill, mayLoad = true, Uses = [sp] in
def VLDA_dmx_lda_x_spill : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst), (ins c16n_step64:$imm), "vlda", " $dst, [sp, #$imm]">{
// id: me__instr128_or__lda__dmx_lda_x_spill__agua_dmx__nrm_spill__agua_dmx__spill__mSPa__m_c16n_step64
bits<9> imm;
bits<4> dst;

let lda = {imm, dst, 0b1110111};
}

let Itinerary = II_VLDB_128_idx, mayLoad = true in
def VLDB_128_idx : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.128", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__idx__mPb__mDJb
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let ldb = {ptr, dj, 0b000, dst, 0b000};
}

let Itinerary = II_VLDB_128_idx_imm, mayLoad = true in
def VLDB_128_idx_imm : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins eP:$ptr, c8s_step16:$imm), "vldb.128", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__idx_imm__mPb__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let ldb = {ptr, imm, 0b01, dst, 0b000};
}

let Itinerary = II_VLDB_128_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_128_pstm_nrm : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.128", " $dst, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_nrm__mPb__mMb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let ldb = {ptr, mod, 0b010, dst, 0b000};
}

let Itinerary = II_VLDB_128_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_128_pstm_nrm_imm : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out), (ins eP:$ptr, c8s_step16:$imm), "vldb.128", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_nrm_imm__mPb__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let ldb = {ptr, imm, 0b11, dst, 0b000};
}

let Itinerary = II_VLDB_2D_128, DecoderMethod = "DecodeDmv_ldb_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_2D_128 : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.128", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_2d__mPb__mDb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let ldb = {ptr, mod, 0b100, dst, 0b000};
}

let Itinerary = II_VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign0, DecoderMethod = "DecodeDmw_ldb_unpack_2D", mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.unpack", " $dst, unpacksign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_2d__mPb__mDb__unpackSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b100, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign1, DecoderMethod = "DecodeDmw_ldb_unpack_2D", mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_2D_UNPACK_dmw_ldb_unpack_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.unpack", " $dst, unpacksign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_2d__mPb__mDb__unpackSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b100, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign0, DecoderMethod = "DecodeDmx_ldb_unpack_2D", mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.unpack", " $dst, unpacksign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_2d__mPb__mDb__mYb__unpackSign0
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let ldb = {ptr, mod, 0b100, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign1, DecoderMethod = "DecodeDmx_ldb_unpack_2D", mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_2D_UNPACK_dmx_ldb_unpack_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d.unpack", " $dst, unpacksign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_2d__mPb__mDb__mYb__unpackSign1
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let ldb = {ptr, mod, 0b100, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_2D_dmw_ldb, DecoderMethod = "DecodeDmw_ldb_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_2D_dmw_ldb : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_2d__mPb__mDb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let ldb = {ptr, mod, 0b100, dst, 0b001};
}

let Itinerary = II_VLDB_2D_dmx_ldb_x, DecoderMethod = "DecodeDmx_ldb_x_2D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_2D_dmx_ldb_x : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDC:$dc), (ins eP:$ptr, eD:$mod), "vldb.2d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_2d__mPb__mDb
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b100, dst, 0b1101};
}

let Itinerary = II_VLDB_3D_128, DecoderMethod = "DecodeDmv_ldb_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_3D_128 : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.128", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmv_ldb__agub_dmv__nrm__agub_dmv__normal__agub_dmv__pstm_3d__mPb__mDSb
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b000};
}

let Itinerary = II_VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0, DecoderMethod = "DecodeDmw_ldb_unpack_3D", mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.unpack", " $dst, unpacksign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_3d__mPb__mDSb__unpackSign0
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1, DecoderMethod = "DecodeDmw_ldb_unpack_3D", mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_3D_UNPACK_dmw_ldb_unpack_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.unpack", " $dst, unpacksign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_3d__mPb__mDSb__unpackSign1
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0, DecoderMethod = "DecodeDmx_ldb_unpack_3D", mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.unpack", " $dst, unpacksign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_3d__mPb__mDSb__mYb__unpackSign0
bits<3> ptr;
bits<2> mod;
bits<3> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1, DecoderMethod = "DecodeDmx_ldb_unpack_3D", mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_3D_UNPACK_dmx_ldb_unpack_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d.unpack", " $dst, unpacksign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_3d__mPb__mDSb__mYb__unpackSign1
bits<3> ptr;
bits<2> mod;
bits<3> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_3D_dmw_ldb, DecoderMethod = "DecodeDmw_ldb_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_3D_dmw_ldb : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_3d__mPb__mDSb
bits<3> ptr;
bits<2> mod;
bits<5> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b001};
}

let Itinerary = II_VLDB_3D_dmx_ldb_x, DecoderMethod = "DecodeDmx_ldb_x_3D", mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_3D_dmx_ldb_x : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eP:$ptr, eDS:$mod), "vldb.3d", " $dst, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_3d__mPb__mDSb
bits<3> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {ptr, dontcare{1}, mod, 0b110, dst, 0b1101};
}

let Itinerary = II_VLDB_4x16_hi, mayLoad = true in
def VLDB_4x16_hi : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x16.hi", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x16__load_4x_pointer_hi
bits<5> src;
bits<5> dst;

let ldb = {src, 0b1, 0b00, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x16_lo, mayLoad = true in
def VLDB_4x16_lo : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x16.lo", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x16__load_4x_pointer_lo
bits<5> src;
bits<5> dst;

let ldb = {src, 0b0, 0b00, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x32_hi, mayLoad = true in
def VLDB_4x32_hi : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x32.hi", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x32__load_4x_pointer_hi
bits<5> src;
bits<5> dst;

let ldb = {src, 0b1, 0b01, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x32_lo, mayLoad = true in
def VLDB_4x32_lo : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x32.lo", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x32__load_4x_pointer_lo
bits<5> src;
bits<5> dst;

let ldb = {src, 0b0, 0b01, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x64_hi, mayLoad = true in
def VLDB_4x64_hi : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x64.hi", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x64__load_4x_pointer_hi
bits<5> src;
bits<5> dst;

let ldb = {src, 0b1, 0b10, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_4x64_lo, mayLoad = true in
def VLDB_4x64_lo : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins OP_mWs:$src), "vldb.4x64.lo", " $dst, $src">{
// id: me__instr128_or__ldb__ldb_or1__dmv_ldb_4x__load_4x64__load_4x_pointer_lo
bits<5> src;
bits<5> dst;

let ldb = {src, 0b0, 0b10, 0b1, dst, 0b100};
}

let Itinerary = II_VLDB_FILLX_512, DecoderMethod = "DecodeVLDB_FILLX_512", mayLoad = true, Defs = [lfe], Uses = [lfe] in
def VLDB_FILLX_512 : AIE2P_inst_ldb_instr32 <(outs ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins mR30_fifo_step_e1:$conf_e1, mR30_fifo_step_e7:$conf_e7, ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vldb.fillx.512", " [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_x__normal_fillx__dmx_ldb_fifo_x__core_fillx__mR30_fifo_step_e1__mR30_fifo_step_e7__dmx_ldb_fifo_x__normalx__mPfb__mLdFifo_b__mRF2b
bits<1> ptr;

let ldb = {0b01, ptr, dontcare{3-1}, 0b101, dontcare{4-1}, 0b0101};
}

let Itinerary = II_VLDB_FILL_512, DecoderMethod = "DecodeVLDB_FILL_512", mayLoad = true in
def VLDB_FILL_512 : AIE2P_inst_ldb_instr32 <(outs ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vldb.fill.512", " [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_x__normal_fill__dmx_ldb_fifo_x__core_fill__dmx_ldb_fifo_x__normal__mPfb__mLdFifo_b__mRF2b
bits<1> ptr;

let ldb = {0b10, ptr, dontcare{3-1}, 0b101, dontcare{4-1}, 0b0101};
}

let Itinerary = II_VLDB_POPX_512, DecoderMethod = "DecodeVLDB_POPX_512", mayLoad = true, Defs = [lfe, srFifo_uf], Uses = [lfe] in
def VLDB_POPX_512 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins mR30_fifo_step_e1:$conf_e1, mR30_fifo_step_e7:$conf_e7, ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vldb.popx.512", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_x__normal_popx__dmx_ldb_fifo_x__core_popx__mR30_fifo_step_e1__mR30_fifo_step_e7__dmx_ldb_fifo_x__normalx__mPfb__mLdFifo_b__mRF2b
bits<1> ptr;
bits<4> dst;

let ldb = {0b10, ptr, dontcare{3-1}, 0b000, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_512_2D, DecoderMethod = "DecodeDmx_ldb_fifo_x__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_512_2D : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vldb.pop.512.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_x__fifo_2d_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__fifo_2d__mPfb__mLdFifo_b__mRF2b__mDb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b11, ptr, mod, 0b100, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_512_3D, DecoderMethod = "DecodeDmx_ldb_fifo_x__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_512_3D : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vldb.pop.512.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_x__fifo_3d_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__fifo_3d__mPfb__mLdFifo_b__mRF2b__mDSb
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {0b00, ptr, dontcare{1}, mod, 0b100, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_512_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_512_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_512_fifo_1d_pop : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vldb.pop.512", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_x__fifo_1d_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__fifo_1d__mPfb__mLdFifo_b__mRF2b__mMb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b01, ptr, mod, 0b010, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_512_normal_pop, DecoderMethod = "DecodeVLDB_POP_512_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_512_normal_pop : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vldb.pop.512", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_x__normal_pop__dmx_ldb_fifo_x__core_pop__dmx_ldb_fifo_x__normal__mPfb__mLdFifo_b__mRF2b
bits<1> ptr;
bits<4> dst;

let ldb = {0b00, ptr, dontcare{3-1}, 0b000, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_544_2D, DecoderMethod = "DecodeDmx_ldb_fifo_ex_ebs16__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_544_2D : AIE2P_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vldb.pop.544.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_ex_ebs16__fifo_2d_pop__dmx_ldb_fifo_ex_ebs16__core_pop__dmx_ldb_fifo_ex_ebs16__fifo_2d__mPfb__mLdFifo_b__mRF2b__mDb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b10, ptr, mod, 0b010, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_544_3D, DecoderMethod = "DecodeDmx_ldb_fifo_ex_ebs16__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_544_3D : AIE2P_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vldb.pop.544.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_ex_ebs16__fifo_3d_pop__dmx_ldb_fifo_ex_ebs16__core_pop__dmx_ldb_fifo_ex_ebs16__fifo_3d__mPfb__mLdFifo_b__mRF2b__mDSb
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {0b01, ptr, dontcare{1}, mod, 0b100, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_544_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_544_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_544_fifo_1d_pop : AIE2P_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vldb.pop.544", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_ex_ebs16__fifo_1d_pop__dmx_ldb_fifo_ex_ebs16__core_pop__dmx_ldb_fifo_ex_ebs16__fifo_1d__mPfb__mLdFifo_b__mRF2b__mMb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b00, ptr, mod, 0b110, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_544_normal_pop, DecoderMethod = "DecodeVLDB_POP_544_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_544_normal_pop : AIE2P_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vldb.pop.544", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_ex_ebs16__normal_pop__dmx_ldb_fifo_ex_ebs16__core_pop__dmx_ldb_fifo_ex_ebs16__normal__mPfb__mLdFifo_b__mRF2b
bits<1> ptr;
bits<4> dst;

let ldb = {0b11, ptr, dontcare{3-1}, 0b000, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_576_2D, DecoderMethod = "DecodeDmx_ldb_fifo_ex_ebs8__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_576_2D : AIE2P_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vldb.pop.576.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_ex_ebs8__fifo_2d_pop__dmx_ldb_fifo_ex_ebs8__core_pop__dmx_ldb_fifo_ex_ebs8__fifo_2d__mPfb__mLdFifo_b__mRF2b__mDb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b00, ptr, mod, 0b010, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_576_3D, DecoderMethod = "DecodeDmx_ldb_fifo_ex_ebs8__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_576_3D : AIE2P_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vldb.pop.576.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_ex_ebs8__fifo_3d_pop__dmx_ldb_fifo_ex_ebs8__core_pop__dmx_ldb_fifo_ex_ebs8__fifo_3d__mPfb__mLdFifo_b__mRF2b__mDSb
bits<1> ptr;
bits<2> mod;
bits<4> dst;

let ldb = {0b10, ptr, dontcare{1}, mod, 0b100, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_576_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_576_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_576_fifo_1d_pop : AIE2P_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vldb.pop.576", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_ex_ebs8__fifo_1d_pop__dmx_ldb_fifo_ex_ebs8__core_pop__dmx_ldb_fifo_ex_ebs8__fifo_1d__mPfb__mLdFifo_b__mRF2b__mMb
bits<1> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {0b11, ptr, mod, 0b010, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_576_normal_pop, DecoderMethod = "DecodeVLDB_POP_576_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_576_normal_pop : AIE2P_inst_ldb_instr32 <(outs OP_mEXb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vldb.pop.576", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_ex_ebs8__normal_pop__dmx_ldb_fifo_ex_ebs8__core_pop__dmx_ldb_fifo_ex_ebs8__normal__mPfb__mLdFifo_b__mRF2b
bits<1> ptr;
bits<4> dst;

let ldb = {0b01, ptr, dontcare{3-1}, 0b000, dst, 0b0101};
}

let Itinerary = II_VLDB_POP_640_2D, DecoderMethod = "DecodeDmx_ldb_fifo_qx__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_640_2D : AIE2P_inst_ldb_instr32 <(outs OP_mQXsb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vldb.pop.640.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_qx__fifo_2d_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__fifo_2d__mPfb__mLdFifo_b__mRF2b__mDb
bits<1> ptr;
bits<3> mod;
bits<2> dst;

let ldb = {0b10, ptr, mod, 0b001, dst, dontcare{2-1}, 0b0101};
}

let Itinerary = II_VLDB_POP_640_3D, DecoderMethod = "DecodeDmx_ldb_fifo_qx__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_640_3D : AIE2P_inst_ldb_instr32 <(outs OP_mQXsb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vldb.pop.640.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_qx__fifo_3d_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__fifo_3d__mPfb__mLdFifo_b__mRF2b__mDSb
bits<1> ptr;
bits<2> mod;
bits<2> dst;

let ldb = {0b11, ptr, dontcare{1}, mod, 0b110, dst, dontcare{2-1}, 0b0101};
}

let Itinerary = II_VLDB_POP_640_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_640_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_640_fifo_1d_pop : AIE2P_inst_ldb_instr32 <(outs OP_mQXsb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vldb.pop.640", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_qx__fifo_1d_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__fifo_1d__mPfb__mLdFifo_b__mRF2b__mMb
bits<1> ptr;
bits<3> mod;
bits<2> dst;

let ldb = {0b11, ptr, mod, 0b001, dst, dontcare{2-1}, 0b0101};
}

let Itinerary = II_VLDB_POP_640_normal_pop, DecoderMethod = "DecodeVLDB_POP_640_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_640_normal_pop : AIE2P_inst_ldb_instr32 <(outs OP_mQXsb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vldb.pop.640", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_qx__normal_pop__dmx_ldb_fifo_qx__core_pop__dmx_ldb_fifo_qx__normal__mPfb__mLdFifo_b__mRF2b
bits<1> ptr;
bits<2> dst;

let ldb = {0b10, ptr, dontcare{3-1}, 0b110, dst, dontcare{2-1}, 0b0101};
}

let Itinerary = II_VLDB_POP_704_2D, DecoderMethod = "DecodeDmx_ldb_fifo_qex_ebs16__fifo_2d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_704_2D : AIE2P_inst_ldb_instr32 <(outs OP_mQEXsb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDC:$dc), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eD:$mod), "vldb.pop.704.2d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_qex_ebs16__fifo_2d_pop__dmx_ldb_fifo_qex_ebs16__core_pop__dmx_ldb_fifo_qex_ebs16__fifo_2d__mPfb__mLdFifo_b__mRF2b__mDb
bits<1> ptr;
bits<3> mod;
bits<2> dst;

let ldb = {0b01, ptr, mod, 0b001, dst, dontcare{2-1}, 0b0101};
}

let Itinerary = II_VLDB_POP_704_3D, DecoderMethod = "DecodeDmx_ldb_fifo_qex_ebs16__fifo_3d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_704_3D : AIE2P_inst_ldb_instr32 <(outs OP_mQEXsb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out, eDCL:$dcl, eDCH:$dch), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eDS:$mod), "vldb.pop.704.3d", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_qex_ebs16__fifo_3d_pop__dmx_ldb_fifo_qex_ebs16__core_pop__dmx_ldb_fifo_qex_ebs16__fifo_3d__mPfb__mLdFifo_b__mRF2b__mDSb
bits<1> ptr;
bits<2> mod;
bits<2> dst;

let ldb = {0b00, ptr, dontcare{1}, mod, 0b001, dst, dontcare{2-1}, 0b0101};
}

let Itinerary = II_VLDB_POP_704_fifo_1d_pop, DecoderMethod = "DecodeVLDB_POP_704_fifo_1d_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_704_fifo_1d_pop : AIE2P_inst_ldb_instr32 <(outs OP_mQEXsb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos, eM:$mod), "vldb.pop.704", " $dst, [$ptr, $fifo_reg, $pos, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_qex_ebs16__fifo_1d_pop__dmx_ldb_fifo_qex_ebs16__core_pop__dmx_ldb_fifo_qex_ebs16__fifo_1d__mPfb__mLdFifo_b__mRF2b__mMb
bits<1> ptr;
bits<3> mod;
bits<2> dst;

let ldb = {0b00, ptr, mod, 0b101, dst, dontcare{2-1}, 0b0101};
}

let Itinerary = II_VLDB_POP_704_normal_pop, DecoderMethod = "DecodeVLDB_POP_704_normal_pop", mayLoad = true, Defs = [srFifo_uf] in
def VLDB_POP_704_normal_pop : AIE2P_inst_ldb_instr32 <(outs OP_mQEXsb:$dst, ePS:$ptr_out, eLdFifoReg:$fifo_reg_out, eRF2:$pos_out), (ins ePS:$ptr, eLdFifoReg:$fifo_reg, eRF2:$pos), "vldb.pop.704", " $dst, [$ptr, $fifo_reg, $pos]">, AIE_HasTiedSubregister{
// id: me__instr128_or__ldb__dmx_ldb_fifo__dmx_ldb_fifo_qex_ebs16__normal_pop__dmx_ldb_fifo_qex_ebs16__core_pop__dmx_ldb_fifo_qex_ebs16__normal__mPfb__mLdFifo_b__mRF2b
bits<1> ptr;
bits<2> dst;

let ldb = {0b01, ptr, dontcare{3-1}, 0b110, dst, dontcare{2-1}, 0b0101};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0] in
def VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.unpack", " $dst, unpacksign0, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx__mPb__mDJb__unpackSign0
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let ldb = {ptr, dj, 0b000, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1] in
def VLDB_UNPACK_dmw_ldb_unpack_idx_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.unpack", " $dst, unpacksign1, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx__mPb__mDJb__unpackSign1
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let ldb = {ptr, dj, 0b000, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0] in
def VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, c9s_step32:$imm), "vldb.unpack", " $dst, unpacksign0, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx_imm__mPb__m_c09s_step32__unpackSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b01, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1] in
def VLDB_UNPACK_dmw_ldb_unpack_idx_imm_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, c9s_step32:$imm), "vldb.unpack", " $dst, unpacksign1, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx_imm__mPb__m_c09s_step32__unpackSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b01, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.unpack", " $dst, unpacksign0, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm__mPb__mMb__unpackSign0
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b010, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.unpack", " $dst, unpacksign1, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm__mPb__mMb__unpackSign1
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b010, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vldb.unpack", " $dst, unpacksign0, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm_imm__mPb__m_c09s_step32__unpackSign0
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b11, dst, 0b0, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmw_ldb_unpack_pstm_nrm_imm_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vldb.unpack", " $dst, unpacksign1, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmw_ldb_unpack__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm_imm__mPb__m_c09s_step32__unpackSign1
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b11, dst, 0b1, 0b010};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0] in
def VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs eY:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.unpack", " $dst, unpacksign0, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx__mPb__mDJb__mYb__unpackSign0
bits<3> ptr;
bits<3> dj;
bits<3> dst;

let ldb = {ptr, dj, 0b000, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1] in
def VLDB_UNPACK_dmx_ldb_unpack_idx_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs eY:$dst), (ins eP:$ptr, eDJ:$dj), "vldb.unpack", " $dst, unpacksign1, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx__mPb__mDJb__mYb__unpackSign1
bits<3> ptr;
bits<3> dj;
bits<3> dst;

let ldb = {ptr, dj, 0b000, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0] in
def VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs eY:$dst), (ins eP:$ptr, c10s_step64:$imm), "vldb.unpack", " $dst, unpacksign0, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx_imm__mPb__m_c10s_step64__mYb__unpackSign0
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let ldb = {ptr, imm, 0b01, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1] in
def VLDB_UNPACK_dmx_ldb_unpack_idx_imm_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs eY:$dst), (ins eP:$ptr, c10s_step64:$imm), "vldb.unpack", " $dst, unpacksign1, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx_imm__mPb__m_c10s_step64__mYb__unpackSign1
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let ldb = {ptr, imm, 0b01, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.unpack", " $dst, unpacksign0, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm__mPb__mMb__mYb__unpackSign0
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let ldb = {ptr, mod, 0b010, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb.unpack", " $dst, unpacksign1, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm__mPb__mMb__mYb__unpackSign1
bits<3> ptr;
bits<3> mod;
bits<3> dst;

let ldb = {ptr, mod, 0b010, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign0, mayLoad = true, Uses = [crUnpackSize, unpackSign0], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vldb.unpack", " $dst, unpacksign0, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm_imm__mPb__m_c10s_step64__mYb__unpackSign0
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let ldb = {ptr, imm, 0b11, dst, dontcare{1}, 0b0, 0b011};
}

let Itinerary = II_VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign1, mayLoad = true, Uses = [crUnpackSize, unpackSign1], Constraints = "$ptr_out = $ptr" in
def VLDB_UNPACK_dmx_ldb_unpack_pstm_nrm_imm_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs eY:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vldb.unpack", " $dst, unpacksign1, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmx_ldb_unpack__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm_imm__mPb__m_c10s_step64__mYb__unpackSign1
bits<3> ptr;
bits<4> imm;
bits<3> dst;

let ldb = {ptr, imm, 0b11, dst, dontcare{1}, 0b1, 0b011};
}

let Itinerary = II_VLDB_dmw_ldb_idx, mayLoad = true in
def VLDB_dmw_ldb_idx : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx__mPb__mDJb
bits<3> ptr;
bits<3> dj;
bits<5> dst;

let ldb = {ptr, dj, 0b000, dst, 0b001};
}

let Itinerary = II_VLDB_dmw_ldb_idx_imm, mayLoad = true in
def VLDB_dmw_ldb_idx_imm : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst), (ins eP:$ptr, c9s_step32:$imm), "vldb", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__idx_imm__mPb__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let ldb = {ptr, imm, 0b01, dst, 0b001};
}

let Itinerary = II_VLDB_dmw_ldb_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_dmw_ldb_pstm_nrm : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb", " $dst, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm__mPb__mMb
bits<3> ptr;
bits<3> mod;
bits<5> dst;

let ldb = {ptr, mod, 0b010, dst, 0b001};
}

let Itinerary = II_VLDB_dmw_ldb_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_dmw_ldb_pstm_nrm_imm : AIE2P_inst_ldb_instr32 <(outs OP_mWb:$dst, eP:$ptr_out), (ins eP:$ptr, c9s_step32:$imm), "vldb", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmw_ldb__agub_dmw__nrm__agub_dmw__normal__agub_dmw__pstm_nrm_imm__mPb__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> dst;

let ldb = {ptr, imm, 0b11, dst, 0b001};
}

let Itinerary = II_VLDB_dmx_ldb_x_idx, mayLoad = true in
def VLDB_dmx_ldb_x_idx : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, eDJ:$dj), "vldb", " $dst, [$ptr, $dj]">{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx__mPb__mDJb
bits<3> ptr;
bits<3> dj;
bits<4> dst;

let ldb = {ptr, dj, 0b000, dst, 0b1101};
}

let Itinerary = II_VLDB_dmx_ldb_x_idx_imm, mayLoad = true in
def VLDB_dmx_ldb_x_idx_imm : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins eP:$ptr, c10s_step64:$imm), "vldb", " $dst, [$ptr, #$imm]">{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__idx_imm__mPb__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b01, dst, 0b1101};
}

let Itinerary = II_VLDB_dmx_ldb_x_pstm_nrm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_dmx_ldb_x_pstm_nrm : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, eM:$mod), "vldb", " $dst, [$ptr], $mod">{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm__mPb__mMb
bits<3> ptr;
bits<3> mod;
bits<4> dst;

let ldb = {ptr, mod, 0b010, dst, 0b1101};
}

let Itinerary = II_VLDB_dmx_ldb_x_pstm_nrm_imm, mayLoad = true, Constraints = "$ptr_out = $ptr" in
def VLDB_dmx_ldb_x_pstm_nrm_imm : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst, eP:$ptr_out), (ins eP:$ptr, c10s_step64:$imm), "vldb", " $dst, [$ptr], #$imm">{
// id: me__instr128_or__ldb__dmx_ldb_x__agub_dmx__nrm__agub_dmx__normal__agub_dmx__pstm_nrm_imm__mPb__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> dst;

let ldb = {ptr, imm, 0b11, dst, 0b1101};
}

let Itinerary = II_VLT_16_vaddSign0, Uses = [vaddSign0] in
def VLT_16_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.16", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out32__mRS16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b0, 0b01, 0b010011};
}

let Itinerary = II_VLT_16_vaddSign1, Uses = [vaddSign1] in
def VLT_16_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.16", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out32__mRS16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b1, 0b01, 0b010011};
}

let Itinerary = II_VLT_32_vaddSign0, Uses = [vaddSign0] in
def VLT_32_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.32", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out16__mRS16m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b0, 0b10, 0b010011};
}

let Itinerary = II_VLT_32_vaddSign1, Uses = [vaddSign1] in
def VLT_32_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.32", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out16__mRS16m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b1, 0b10, 0b010011};
}

let Itinerary = II_VLT_8_vaddSign0, Uses = [vaddSign0] in
def VLT_8_vaddSign0 : AIE2P_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.8", " $cmp, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign0
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b0, 0b00, 0b010011};
}

let Itinerary = II_VLT_8_vaddSign1, Uses = [vaddSign1] in
def VLT_8_vaddSign1 : AIE2P_inst_mv_instr32 <(outs eL:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.8", " $cmp, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign1
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b1, 0b00, 0b010011};
}

let Itinerary = II_VLT_bf16 in
def VLT_bf16 : AIE2P_inst_mv_instr32 <(outs eRS16:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vlt.bf16", " $cmp, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_bf_compare__vlt_bf__vec_cmp_out32__mRS16m__vec_add_mX__vec_add_nX
bits<4> cmp;
bits<4> s1;
bits<4> s2;

let mv = {cmp, s1, s2, 0b0, 0b001100111};
}

let Itinerary = II_VMAC_vmul_cm_core_X_QX, Defs = [srSparse_of] in
def VMAC_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<2> s2;

let vec = {acc, dst, acc1, s1, s2, dontcare{2-1}, 0b100, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMAC_vmul_cm_core_X_QY, Defs = [srSparse_of] in
def VMAC_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eQYs:$s2, eR:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, dst, acc1, s1, s2, dontcare{2-1}, 0b1010, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMAC_vmul_cm_core_X_X in
def VMAC_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, acc1, s1, s2, 0b000, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMAC_vmul_cm_core_X_Y in
def VMAC_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eY:$s2, eR:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, acc1, s1, s2, 0b0010, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMAC_vmul_cm_core_Y_QX, Defs = [srSparse_of] in
def VMAC_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mQXsw:$s2, eR:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, dst, acc1, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMAC_vmul_cm_core_Y_QY, Defs = [srSparse_of] in
def VMAC_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eQYs:$s2, eR:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<1> s2;

let vec = {acc, dst, acc1, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMAC_vmul_cm_core_Y_X in
def VMAC_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mXw:$s2, eR:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, acc1, s1, 0b0, s2, 0b110, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMAC_vmul_cm_core_Y_Y in
def VMAC_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eY:$s2, eR:$acc), "vmac", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, acc1, s1, 0b0, s2, 0b0101, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMAC_f_vmac_bf_vmul_bf_core_X_X, Defs = [srFPFlags], Uses = [crFPMask] in
def VMAC_f_vmac_bf_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bf__mDMa__vmac_bf1_add__mDMa__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, acc1, s1, s2, 0b000, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMAC_f_vmac_bf_vmul_bf_core_Y_Y, Defs = [srFPFlags], Uses = [crFPMask] in
def VMAC_f_vmac_bf_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eY:$s2, eR:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bf__mDMa__vmac_bf1_add__mDMa__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, acc1, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMAC_f_vmac_bfp_vmul_bfp_core_EX_EX, Defs = [srFPFlags], Uses = [crFPMask] in
def VMAC_f_vmac_bfp_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, acc1, s1, s2, 0b010, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMAC_f_vmac_bfp_vmul_bfp_core_EX_EY, Defs = [srFPFlags], Uses = [crFPMask] in
def VMAC_f_vmac_bfp_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mEXv:$s1, eEY:$s2, eR:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, acc1, s1, s2, dontcare{1}, 0b001, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMAC_f_vmac_bfp_vmul_bfp_core_EX_QEY, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask] in
def VMAC_f_vmac_bfp_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, dst, acc1, s1, s2, dontcare{3-1}, 0b101, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMAC_f_vmac_bfp_vmul_bfp_core_EY_QEX, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask] in
def VMAC_f_vmac_bfp_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vmac.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, dst, acc1, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMAXDIFF_LT_16_vaddSign0, Uses = [vaddSign0] in
def VMAXDIFF_LT_16_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b000, 0b0, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_16_vaddSign1, Uses = [vaddSign1] in
def VMAXDIFF_LT_16_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b000, 0b1, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_32_vaddSign0, Uses = [vaddSign0] in
def VMAXDIFF_LT_32_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b000, 0b0, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_32_vaddSign1, Uses = [vaddSign1] in
def VMAXDIFF_LT_32_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b000, 0b1, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_8_vaddSign0, Uses = [vaddSign0] in
def VMAXDIFF_LT_8_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b000, 0b0, 0b0010};
}

let Itinerary = II_VMAXDIFF_LT_8_vaddSign1, Uses = [vaddSign1] in
def VMAXDIFF_LT_8_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmaxdiff_lt.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmaxdiff_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b000, 0b1, 0b0010};
}

let Itinerary = II_VMAX_LT_16_vaddSign0, Uses = [vaddSign0] in
def VMAX_LT_16_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b100, 0b0, 0b0010};
}

let Itinerary = II_VMAX_LT_16_vaddSign1, Uses = [vaddSign1] in
def VMAX_LT_16_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b100, 0b1, 0b0010};
}

let Itinerary = II_VMAX_LT_32_vaddSign0, Uses = [vaddSign0] in
def VMAX_LT_32_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b100, 0b0, 0b0010};
}

let Itinerary = II_VMAX_LT_32_vaddSign1, Uses = [vaddSign1] in
def VMAX_LT_32_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b100, 0b1, 0b0010};
}

let Itinerary = II_VMAX_LT_8_vaddSign0, Uses = [vaddSign0] in
def VMAX_LT_8_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b100, 0b0, 0b0010};
}

let Itinerary = II_VMAX_LT_8_vaddSign1, Uses = [vaddSign1] in
def VMAX_LT_8_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmax_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b100, 0b1, 0b0010};
}

let Itinerary = II_VMAX_LT_bf16 in
def VMAX_LT_bf16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmax_lt.bf16", " $d, r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_bf_min_max__vmax_lt_bf__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b1, 0b101100111};
}

let Itinerary = II_VMIN_GE_16_vaddSign0, Uses = [vaddSign0] in
def VMIN_GE_16_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b011, 0b0, 0b0010};
}

let Itinerary = II_VMIN_GE_16_vaddSign1, Uses = [vaddSign1] in
def VMIN_GE_16_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b011, 0b1, 0b0010};
}

let Itinerary = II_VMIN_GE_32_vaddSign0, Uses = [vaddSign0] in
def VMIN_GE_32_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b011, 0b0, 0b0010};
}

let Itinerary = II_VMIN_GE_32_vaddSign1, Uses = [vaddSign1] in
def VMIN_GE_32_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b011, 0b1, 0b0010};
}

let Itinerary = II_VMIN_GE_8_vaddSign0, Uses = [vaddSign0] in
def VMIN_GE_8_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b011, 0b0, 0b0010};
}

let Itinerary = II_VMIN_GE_8_vaddSign1, Uses = [vaddSign1] in
def VMIN_GE_8_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vmin_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b011, 0b1, 0b0010};
}

let Itinerary = II_VMIN_GE_bf16 in
def VMIN_GE_bf16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vmin_ge.bf16", " $d, r16, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_bf_min_max__vmin_ge_bf__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b0, 0b101100111};
}

let Itinerary = II_VMOV_0_mv_scd_cm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_0_mv_scd_cm : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins), "vmov.0", " $dst, scd">{
// id: me__instr128_or__lda__mv_scd__mv_scd_cm__scd_cm_0__mSCD_E7
bits<4> dst;

let lda = {dontcare{6-1}, 0b010, dst, 0b0, 0b111011};
}

let Itinerary = II_VMOV_0_mv_scd_dm_imm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_0_mv_scd_dm_imm : AIE2P_inst_lda_instr32 <(outs eDM:$dst), (ins), "vmov.0", " $dst, scd">{
// id: me__instr128_or__lda__mv_scd__mv_scd_dm_imm__scd_dm_0__mv_scd_dm_core__mDMm__mSCD_E7
bits<3> dst;

let lda = {dontcare{6-1}, 0b110, dst, 0b00, 0b111011};
}

let Itinerary = II_VMOV_1_mv_scd_cm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_1_mv_scd_cm : AIE2P_inst_lda_instr32 <(outs OP_mCMm:$dst), (ins), "vmov.1", " $dst, scd">{
// id: me__instr128_or__lda__mv_scd__mv_scd_cm__scd_cm_1__mSCD_E7
bits<4> dst;

let lda = {dontcare{6-1}, 0b010, dst, 0b1, 0b111011};
}

let Itinerary = II_VMOV_1_mv_scd_dm_imm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_1_mv_scd_dm_imm : AIE2P_inst_lda_instr32 <(outs eDM:$dst), (ins), "vmov.1", " $dst, scd">{
// id: me__instr128_or__lda__mv_scd__mv_scd_dm_imm__scd_dm_1__mv_scd_dm_core__mDMm__mSCD_E7
bits<3> dst;

let lda = {dontcare{6-1}, 0b110, dst, 0b01, 0b111011};
}

let Itinerary = II_VMOV_2, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_2 : AIE2P_inst_lda_instr32 <(outs eDM:$dst), (ins), "vmov.2", " $dst, scd">{
// id: me__instr128_or__lda__mv_scd__mv_scd_dm_imm__scd_dm_2__mv_scd_dm_core__mDMm__mSCD_E7
bits<3> dst;

let lda = {dontcare{6-1}, 0b110, dst, 0b10, 0b111011};
}

let Itinerary = II_VMOV_3, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_3 : AIE2P_inst_lda_instr32 <(outs eDM:$dst), (ins), "vmov.3", " $dst, scd">{
// id: me__instr128_or__lda__mv_scd__mv_scd_dm_imm__scd_dm_3__mv_scd_dm_core__mDMm__mSCD_E7
bits<3> dst;

let lda = {dontcare{6-1}, 0b110, dst, 0b11, 0b111011};
}

let Itinerary = II_VMOV_D in
def VMOV_D : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1), "vmov.d", "$dst, $acc1">{
// id: me__instr128_or__vec__vmov__mDMa__vmac_cm1_add__mDMa
bits<3> dst;
bits<3> acc1;

let vec = {dontcare{5-1}, dst, acc1, dontcare{4-1}, dontcare{3-1}, 0b0111, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMOV_alu_mv_mv_cm in
def VMOV_alu_mv_mv_cm : AIE2P_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mCMm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_cm
bits<4> dst;
bits<4> src;

let mv = {dst, dontcare{2-1}, src, 0b010001010111};
}

let Itinerary = II_VMOV_alu_mv_mv_ex in
def VMOV_alu_mv_mv_ex : AIE2P_inst_mv_instr32 <(outs OP_mEXm:$dst), (ins OP_mEXm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ex
bits<4> dst;
bits<4> src;

let mv = {dst, dontcare{2-1}, src, dontcare{2-1}, 0b1011100111};
}

let Itinerary = II_VMOV_alu_mv_mv_q in
def VMOV_alu_mv_mv_q : AIE2P_inst_mv_instr32 <(outs OP_mQQsm:$dst), (ins OP_mQQsm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_q
bits<2> dst;
bits<2> src;

let mv = {0b00, dst, dontcare{1}, 0b1, 0b00, src, dontcare{1}, 0b1, 0b0111100111};
}

let Itinerary = II_VMOV_alu_mv_mv_q_to_w in
def VMOV_alu_mv_mv_q_to_w : AIE2P_inst_mv_instr32 <(outs OP_mWm:$dst), (ins OP_mQQsm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_q_to_w
bits<5> dst;
bits<2> src;

let mv = {dst, 0b0, 0b00, src, dontcare{1}, 0b1, 0b1111100111};
}

let Itinerary = II_VMOV_alu_mv_mv_qex in
def VMOV_alu_mv_mv_qex : AIE2P_inst_mv_instr32 <(outs OP_mQEXsm:$dst), (ins OP_mQEXsm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_qex
bits<2> dst;
bits<2> src;

let mv = {0b00, dst, dontcare{2-1}, 0b00, src, dontcare{2-1}, 0b0000010111};
}

let Itinerary = II_VMOV_alu_mv_mv_qx in
def VMOV_alu_mv_mv_qx : AIE2P_inst_mv_instr32 <(outs OP_mQXsm:$dst), (ins OP_mQXsm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_qx
bits<2> dst;
bits<2> src;

let mv = {0b00, dst, dontcare{2-1}, 0b00, src, dontcare{2-1}, 0b1000010111};
}

let Itinerary = II_VMOV_alu_mv_mv_w in
def VMOV_alu_mv_mv_w : AIE2P_inst_mv_instr32 <(outs OP_mWm:$dst), (ins OP_mWm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_w
bits<5> dst;
bits<5> src;

let mv = {dst, 0b0, src, 0b0, 0b0100010111};
}

let Itinerary = II_VMOV_alu_mv_mv_w_to_q in
def VMOV_alu_mv_mv_w_to_q : AIE2P_inst_mv_instr32 <(outs OP_mQQsm:$dst), (ins OP_mWm:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_w_to_q
bits<2> dst;
bits<5> src;

let mv = {0b00, dst, dontcare{1}, 0b1, src, 0b0, 0b1100010111};
}

let Itinerary = II_VMOV_alu_mv_mv_x, ItineraryRegPairs = [
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_eXe, [OperandRegClass<0, eXe>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_eXe, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_eXe, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_eXe, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_eXe, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_eXe, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_eXe, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_eXe, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_eXe, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_eXe, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_eXe, [OperandRegClass<0, eXo>, OperandRegClass<1, eXe>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_mStFifol, [OperandRegClass<0, eXe>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_eLdFifoLReg, [OperandRegClass<0, eXe>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_eBMHH, [OperandRegClass<0, eXe>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_eBMHL, [OperandRegClass<0, eXe>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_eLdFifoHReg, [OperandRegClass<0, eXe>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_mFifoExtra, [OperandRegClass<0, eXe>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_eBMLL, [OperandRegClass<0, eXe>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_eBMLH, [OperandRegClass<0, eXe>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_mStFifoh, [OperandRegClass<0, eXe>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXe_eXo, [OperandRegClass<0, eXe>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_mStFifol, [OperandRegClass<0, mStFifol>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_eLdFifoLReg, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_eBMHH, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_eBMHL, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_eLdFifoHReg, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_mFifoExtra, [OperandRegClass<0, mStFifol>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_eBMLL, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_eBMLH, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_mStFifoh, [OperandRegClass<0, mStFifol>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifol_eXo, [OperandRegClass<0, mStFifol>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_mStFifol, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_eBMHH, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_eBMHL, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_eLdFifoHReg, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_mFifoExtra, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_eBMLL, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_eBMLH, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_mStFifoh, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoLReg_eXo, [OperandRegClass<0, eLdFifoLReg>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_mStFifol, [OperandRegClass<0, eBMHH>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_eLdFifoLReg, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_eBMHH, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_eBMHL, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_eLdFifoHReg, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_mFifoExtra, [OperandRegClass<0, eBMHH>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_eBMLL, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_eBMLH, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_mStFifoh, [OperandRegClass<0, eBMHH>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHH_eXo, [OperandRegClass<0, eBMHH>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_mStFifol, [OperandRegClass<0, eBMHL>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_eLdFifoLReg, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_eBMHH, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_eBMHL, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_eLdFifoHReg, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_mFifoExtra, [OperandRegClass<0, eBMHL>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_eBMLL, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_eBMLH, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_mStFifoh, [OperandRegClass<0, eBMHL>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMHL_eXo, [OperandRegClass<0, eBMHL>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_mStFifol, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_eLdFifoLReg, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_eBMHH, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_eBMHL, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_mFifoExtra, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_eBMLL, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_eBMLH, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_mStFifoh, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eLdFifoHReg_eXo, [OperandRegClass<0, eLdFifoHReg>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_mStFifol, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_eLdFifoLReg, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_eBMHH, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_eBMHL, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_eLdFifoHReg, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_mFifoExtra, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_eBMLL, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_eBMLH, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_mStFifoh, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mFifoExtra_eXo, [OperandRegClass<0, mFifoExtra>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_mStFifol, [OperandRegClass<0, eBMLL>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_eLdFifoLReg, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_eBMHH, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_eBMHL, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_eLdFifoHReg, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_mFifoExtra, [OperandRegClass<0, eBMLL>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_eBMLL, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_eBMLH, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_mStFifoh, [OperandRegClass<0, eBMLL>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLL_eXo, [OperandRegClass<0, eBMLL>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_mStFifol, [OperandRegClass<0, eBMLH>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_eLdFifoLReg, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_eBMHH, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_eBMHL, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_eLdFifoHReg, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_mFifoExtra, [OperandRegClass<0, eBMLH>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_eBMLL, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_eBMLH, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_mStFifoh, [OperandRegClass<0, eBMLH>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eBMLH_eXo, [OperandRegClass<0, eBMLH>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_mStFifol, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_eLdFifoLReg, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_eBMHH, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_eBMHL, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_eLdFifoHReg, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_mFifoExtra, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_eBMLL, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_eBMLH, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_mStFifoh, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_mStFifoh_eXo, [OperandRegClass<0, mStFifoh>, OperandRegClass<1, eXo>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_mStFifol, [OperandRegClass<0, eXo>, OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_eLdFifoLReg, [OperandRegClass<0, eXo>, OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_eBMHH, [OperandRegClass<0, eXo>, OperandRegClass<1, eBMHH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_eBMHL, [OperandRegClass<0, eXo>, OperandRegClass<1, eBMHL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_eLdFifoHReg, [OperandRegClass<0, eXo>, OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_mFifoExtra, [OperandRegClass<0, eXo>, OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_eBMLL, [OperandRegClass<0, eXo>, OperandRegClass<1, eBMLL>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_eBMLH, [OperandRegClass<0, eXo>, OperandRegClass<1, eBMLH>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_mStFifoh, [OperandRegClass<0, eXo>, OperandRegClass<1, mStFifoh>]>,
ItinRegClassPair<II_VMOV_alu_mv_mv_x_eXo_eXo, [OperandRegClass<0, eXo>, OperandRegClass<1, eXo>]>] in
def VMOV_alu_mv_mv_x : AIE2P_inst_mv_instr32 <(outs OP_mMvBMXDst:$dst), (ins OP_mMvBMXSrc:$src), "vmov", "$dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_x
bits<6> dst;
bits<6> src;

let mv = {dst, src, 0b0010010111};
}

let Itinerary = II_VMOV_lda_mv_scd_bm, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_lda_mv_scd_bm : AIE2P_inst_lda_instr32 <(outs OP_mBMm:$dst), (ins), "vmov", "$dst, scd">{
// id: me__instr128_or__lda__mv_scd__mv_scd_bm__mScdBMDst__mSCD_E7
bits<5> dst;

let lda = {dontcare{6-1}, 0b000, dst, 0b111011};
}

let Itinerary = II_VMOV_lda_mv_scd_dm_dyn, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$r_out = $r" in
def VMOV_lda_mv_scd_dm_dyn : AIE2P_inst_lda_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins mR31_scd:$r), "vmov", "$dst, scd, r31++">{
// id: me__instr128_or__lda__mv_scd__mv_scd_dm_dyn__mv_scd_dm_core__mDMm__mSCD_E7__mSCD_r_incr__mR31_scd
bits<3> dst;

let lda = {dontcare{6-1}, 0b101, dst, dontcare{2-1}, 0b111011};
}

let Itinerary = II_VMOV_lda_mv_scd_dm_reg, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_lda_mv_scd_dm_reg : AIE2P_inst_lda_instr32 <(outs eDM:$dst), (ins mR31_scd:$c), "vmov", "$dst, scd, r31">{
// id: me__instr128_or__lda__mv_scd__mv_scd_dm_reg__mv_scd_dm_core__mDMm__mSCD_E7__mSCD_r__mR31_scd
bits<3> dst;

let lda = {dontcare{6-1}, 0b001, dst, dontcare{2-1}, 0b111011};
}

let Itinerary = II_VMOV_lda_mv_scd_x, hasSideEffects = true, Uses = [crSCDEn] in
def VMOV_lda_mv_scd_x : AIE2P_inst_lda_instr32 <(outs OP_mXa:$dst), (ins), "vmov", "$dst, scd">{
// id: me__instr128_or__lda__mv_scd__mv_scd_x__mScdXDst__mSCD_E7
bits<4> dst;

let lda = {dontcare{6-1}, 0b100, dst, dontcare{1}, 0b111011};
}

let Itinerary = II_VMOV_st_mv_mcd_bm, hasSideEffects = true, Uses = [crMCDEn] in
def VMOV_st_mv_mcd_bm : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src), "vmov", "mcd, $src">{
// id: me__instr128_or__st__st_streams__mv_mcd_bm__mMCD__mMcdBMSrc
bits<5> src;

let st = {dontcare{3-1}, 0b110, dontcare{3-1}, src, dontcare{2-1}, 0b0100};
}

let Itinerary = II_VMOV_st_mv_mcd_x, hasSideEffects = true, Uses = [crMCDEn] in
def VMOV_st_mv_mcd_x : AIE2P_inst_st_instr32 <(outs), (ins OP_mXs:$src), "vmov", "mcd, $src">{
// id: me__instr128_or__st__st_streams__mv_mcd_x__mMCD__mMcdXSrc
bits<4> src;

let st = {dontcare{3-1}, 0b111, dontcare{3-1}, src, dontcare{3-1}, 0b0100};
}

let Itinerary = II_VMSC_vmul_cm_core_X_QX, Defs = [srSparse_of] in
def VMSC_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<2> s2;

let vec = {acc, dst, acc1, s1, s2, dontcare{2-1}, 0b100, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMSC_vmul_cm_core_X_QY, Defs = [srSparse_of] in
def VMSC_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eQYs:$s2, eR:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, dst, acc1, s1, s2, dontcare{2-1}, 0b1010, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMSC_vmul_cm_core_X_X in
def VMSC_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, acc1, s1, s2, 0b000, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMSC_vmul_cm_core_X_Y in
def VMSC_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, eY:$s2, eR:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, acc1, s1, s2, 0b0010, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMSC_vmul_cm_core_Y_QX, Defs = [srSparse_of] in
def VMSC_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mQXsw:$s2, eR:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, dst, acc1, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMSC_vmul_cm_core_Y_QY, Defs = [srSparse_of] in
def VMSC_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eQYs:$s2, eR:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<1> s2;

let vec = {acc, dst, acc1, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMSC_vmul_cm_core_Y_X in
def VMSC_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, OP_mXw:$s2, eR:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, acc1, s1, 0b0, s2, 0b110, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMSC_vmul_cm_core_Y_Y in
def VMSC_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eY:$s2, eR:$acc), "vmsc", "$dst, $acc1, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmac__mDMa__vmac_cm1_add__mDMa__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, acc1, s1, 0b0, s2, 0b0101, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMSC_f_vmac_bf_vmul_bf_core_X_X, Defs = [srFPFlags], Uses = [crFPMask] in
def VMSC_f_vmac_bf_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bf__mDMa__vmac_bf1_add__mDMa__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, acc1, s1, s2, 0b000, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bf_vmul_bf_core_Y_Y, Defs = [srFPFlags], Uses = [crFPMask] in
def VMSC_f_vmac_bf_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eY:$s1, eY:$s2, eR:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bf__mDMa__vmac_bf1_add__mDMa__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, acc1, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bfp_vmul_bfp_core_EX_EX, Defs = [srFPFlags], Uses = [crFPMask] in
def VMSC_f_vmac_bfp_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, acc1, s1, s2, 0b010, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bfp_vmul_bfp_core_EX_EY, Defs = [srFPFlags], Uses = [crFPMask] in
def VMSC_f_vmac_bfp_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mEXv:$s1, eEY:$s2, eR:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, acc1, s1, s2, dontcare{1}, 0b001, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bfp_vmul_bfp_core_EX_QEY, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask] in
def VMSC_f_vmac_bfp_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<4> s1;
bits<1> s2;

let vec = {acc, dst, acc1, s1, s2, dontcare{3-1}, 0b101, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMSC_f_vmac_bfp_vmul_bfp_core_EY_QEX, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask] in
def VMSC_f_vmac_bfp_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vmsc.f", "$dst, $acc1, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmac_bfp__mDMa__vmac_bfp1_add__mDMa__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> s1;
bits<2> s2;

let vec = {acc, dst, acc1, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMUL_vmul_cm_core_X_QX, Defs = [srSparse_of] in
def VMUL_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<2> s2;

let vec = {acc, dst, 0b111, s1, s2, dontcare{2-1}, 0b100, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMUL_vmul_cm_core_X_QY, Defs = [srSparse_of] in
def VMUL_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eQYs:$s2, eR:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<1> s2;

let vec = {acc, dst, 0b111, s1, s2, dontcare{2-1}, 0b1010, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMUL_vmul_cm_core_X_X in
def VMUL_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, 0b111, s1, s2, 0b000, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMUL_vmul_cm_core_X_Y in
def VMUL_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eY:$s2, eR:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, 0b111, s1, s2, 0b0010, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMUL_vmul_cm_core_Y_QX, Defs = [srSparse_of] in
def VMUL_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mQXsw:$s2, eR:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<2> s2;

let vec = {acc, dst, 0b111, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMUL_vmul_cm_core_Y_QY, Defs = [srSparse_of] in
def VMUL_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eQYs:$s2, eR:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<1> s2;

let vec = {acc, dst, 0b111, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMUL_vmul_cm_core_Y_X in
def VMUL_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mXw:$s2, eR:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, 0b111, s1, 0b0, s2, 0b110, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMUL_vmul_cm_core_Y_Y in
def VMUL_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eY:$s2, eR:$acc), "vmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, 0b111, s1, 0b0, s2, 0b0101, 0b0, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VMUL_f_vmul_bf_vmul_bf_core_X_X, Defs = [srFPFlags], Uses = [crFPMask] in
def VMUL_f_vmul_bf_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bf__mDMa__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, 0b111, s1, s2, 0b000, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMUL_f_vmul_bf_vmul_bf_core_Y_Y, Defs = [srFPFlags], Uses = [crFPMask] in
def VMUL_f_vmul_bf_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eY:$s2, eR:$acc), "vmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bf__mDMa__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, 0b111, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMUL_f_vmul_bfp_vmul_bfp_core_EX_EX, Defs = [srFPFlags], Uses = [crFPMask] in
def VMUL_f_vmul_bfp_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bfp__mDMa__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, 0b111, s1, s2, 0b010, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMUL_f_vmul_bfp_vmul_bfp_core_EX_EY, Defs = [srFPFlags], Uses = [crFPMask] in
def VMUL_f_vmul_bfp_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mEXv:$s1, eEY:$s2, eR:$acc), "vmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bfp__mDMa__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, 0b111, s1, s2, dontcare{1}, 0b001, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMUL_f_vmul_bfp_vmul_bfp_core_EX_QEY, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask] in
def VMUL_f_vmul_bfp_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bfp__mDMa__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<1> s2;

let vec = {acc, dst, 0b111, s1, s2, dontcare{3-1}, 0b101, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VMUL_f_vmul_bfp_vmul_bfp_core_EY_QEX, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask] in
def VMUL_f_vmul_bfp_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bfp__mDMa__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<2> s2;

let vec = {acc, dst, 0b111, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b0, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VNEGMUL_vmul_cm_core_X_QX, Defs = [srSparse_of] in
def VNEGMUL_vmul_cm_core_X_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mQXsw:$s2, eR:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_X_QX__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<2> s2;

let vec = {acc, dst, 0b111, s1, s2, dontcare{2-1}, 0b100, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VNEGMUL_vmul_cm_core_X_QY, Defs = [srSparse_of] in
def VNEGMUL_vmul_cm_core_X_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eQYs:$s2, eR:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_X_QY__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<1> s2;

let vec = {acc, dst, 0b111, s1, s2, dontcare{2-1}, 0b1010, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VNEGMUL_vmul_cm_core_X_X in
def VNEGMUL_vmul_cm_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_X_X__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, 0b111, s1, s2, 0b000, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VNEGMUL_vmul_cm_core_X_Y in
def VNEGMUL_vmul_cm_core_X_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, eY:$s2, eR:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_X_Y__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, 0b111, s1, s2, 0b0010, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VNEGMUL_vmul_cm_core_Y_QX, Defs = [srSparse_of] in
def VNEGMUL_vmul_cm_core_Y_QX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mQXsw:$s2, eR:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_Y_QX__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<2> s2;

let vec = {acc, dst, 0b111, s1, 0b1, s2, dontcare{2-1}, 0b110, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VNEGMUL_vmul_cm_core_Y_QY, Defs = [srSparse_of] in
def VNEGMUL_vmul_cm_core_Y_QY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eQYs:$s2, eR:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_Y_QY__mYv__mQYsw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<1> s2;

let vec = {acc, dst, 0b111, s1, 0b0, s2, dontcare{2-1}, 0b1101, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VNEGMUL_vmul_cm_core_Y_X in
def VNEGMUL_vmul_cm_core_Y_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, OP_mXw:$s2, eR:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_Y_X__mYv__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<4> s2;

let vec = {acc, dst, 0b111, s1, 0b0, s2, 0b110, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VNEGMUL_vmul_cm_core_Y_Y in
def VNEGMUL_vmul_cm_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eY:$s2, eR:$acc), "vnegmul", "$dst, $s1, $s2,$acc">{
// id: me__instr128_or__vec__vmul__mDMa__vmul_cm_core__vmul_cm_core_Y_Y__mYv__mYw__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, 0b111, s1, 0b0, s2, 0b0101, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_vmul_bf_core_X_X, Defs = [srFPFlags], Uses = [crFPMask] in
def VNEGMUL_f_vmul_bf_vmul_bf_core_X_X : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mXv:$s1, OP_mXw:$s2, eR:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bf__mDMa__vmul_bf_core__vmul_bf_core_X_X__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, 0b111, s1, s2, 0b000, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y, Defs = [srFPFlags], Uses = [crFPMask] in
def VNEGMUL_f_vmul_bf_vmul_bf_core_Y_Y : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eY:$s1, eY:$s2, eR:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bf__mDMa__vmul_bf_core__vmul_bf_core_Y_Y__mYv__mYw__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<3> s2;

let vec = {acc, dst, 0b111, s1, dontcare{1}, s2, dontcare{1}, 0b100, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bfp_vmul_bfp_core_EX_EX, Defs = [srFPFlags], Uses = [crFPMask] in
def VNEGMUL_f_vmul_bfp_vmul_bfp_core_EX_EX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mEXv:$s1, OP_mEXw:$s2, eR:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bfp__mDMa__vmul_bfp_core__vmul_bfp_core_EX_EX__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<4> s2;

let vec = {acc, dst, 0b111, s1, s2, 0b010, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bfp_vmul_bfp_core_EX_EY, Defs = [srFPFlags], Uses = [crFPMask] in
def VNEGMUL_f_vmul_bfp_vmul_bfp_core_EX_EY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mEXv:$s1, eEY:$s2, eR:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bfp__mDMa__vmul_bfp_core__vmul_bfp_core_EX_EY__mEYw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<3> s2;

let vec = {acc, dst, 0b111, s1, s2, dontcare{1}, 0b001, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bfp_vmul_bfp_core_EX_QEY, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask] in
def VNEGMUL_f_vmul_bfp_vmul_bfp_core_EX_QEY : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins OP_mEXv:$s1, eQEYs:$s2, eR:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bfp__mDMa__vmul_bfp_core__vmul_bfp_core_EX_QEY__mQEYsw__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<4> s1;
bits<1> s2;

let vec = {acc, dst, 0b111, s1, s2, dontcare{3-1}, 0b101, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VNEGMUL_f_vmul_bfp_vmul_bfp_core_EY_QEX, Defs = [srFPFlags, srSparse_of], Uses = [crFPMask] in
def VNEGMUL_f_vmul_bfp_vmul_bfp_core_EY_QEX : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eEY:$s1, OP_mQEXsw:$s2, eR:$acc), "vnegmul.f", "$dst, $s1, $s2, $acc">{
// id: me__instr128_or__vec__vmul_bfp__mDMa__vmul_bfp_core__vmul_bfp_core_EY_QEX__mEYv__vacc_bfp_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> s1;
bits<2> s2;

let vec = {acc, dst, 0b111, s1, dontcare{1}, s2, dontcare{2-1}, 0b110, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VNEG_GTZ_16 in
def VNEG_GTZ_16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXn:$s2), "vneg_gtz.16", " $d, r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vneg_gtz__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b01, 0b1, 0b00001010111};
}

let Itinerary = II_VNEG_GTZ_32 in
def VNEG_GTZ_32 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXn:$s2), "vneg_gtz.32", " $d, r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vneg_gtz__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b00, 0b1, 0b00001010111};
}

let Itinerary = II_VNEG_GTZ_8 in
def VNEG_GTZ_8 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXn:$s2), "vneg_gtz.8", " $d, r17:r16, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_not__vneg_gtz__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_nX
bits<4> d;
bits<4> s2;

let mv = {d, s2, 0b10, 0b1, 0b00001010111};
}

let Itinerary = II_VNEG in
def VNEG : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eR:$acc), "vneg", "$dst, $acc1, $acc">{
// id: me__instr128_or__vec__vneg__mDMa__vmac_cm1_add__mDMa__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, dontcare{4-1}, dontcare{3-1}, 0b0111, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VNEG_f, Defs = [srFPFlags], Uses = [crFPMask] in
def VNEG_f : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eR:$acc), "vneg.f", "$dst, $acc1, $acc">{
// id: me__instr128_or__vec__vneg_fp__mDMa__vmac_bf1_add__mDMa__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, dontcare{4-1}, dontcare{3-1}, 0b0111, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VPACK_mv_pack_w_packSign0, Uses = [crPackSize, crSat, packSign0] in
def VPACK_mv_pack_w_packSign0 : AIE2P_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mXs:$src), "vpack", "$dst, $src, packsign0">{
// id: me__instr128_or__st__mv_conv__mv_pack_w__packSign0
bits<5> dst;
bits<4> src;

let st = {dst, 0b0, 0b110, src, 0b0, 0b1, 0b01100};
}

let Itinerary = II_VPACK_mv_pack_w_packSign1, Uses = [crPackSize, crSat, packSign1] in
def VPACK_mv_pack_w_packSign1 : AIE2P_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mXs:$src), "vpack", "$dst, $src, packsign1">{
// id: me__instr128_or__st__mv_conv__mv_pack_w__packSign1
bits<5> dst;
bits<4> src;

let st = {dst, 0b0, 0b110, src, 0b1, 0b1, 0b01100};
}

let Itinerary = II_VPACK_mv_pack_x_packSign0, Uses = [crPackSize, crSat, packSign0] in
def VPACK_mv_pack_x_packSign0 : AIE2P_inst_st_instr32 <(outs OP_mXa:$dst), (ins eY:$src), "vpack", "$dst, $src, packsign0">{
// id: me__instr128_or__st__mv_conv__mv_pack_x__mYs__packSign0
bits<4> dst;
bits<3> src;

let st = {dst, dontcare{1}, 0b0, 0b001, src, dontcare{1}, 0b0, 0b1, 0b01100};
}

let Itinerary = II_VPACK_mv_pack_x_packSign1, Uses = [crPackSize, crSat, packSign1] in
def VPACK_mv_pack_x_packSign1 : AIE2P_inst_st_instr32 <(outs OP_mXa:$dst), (ins eY:$src), "vpack", "$dst, $src, packsign1">{
// id: me__instr128_or__st__mv_conv__mv_pack_x__mYs__packSign1
bits<4> dst;
bits<3> src;

let st = {dst, dontcare{1}, 0b0, 0b001, src, dontcare{1}, 0b1, 0b1, 0b01100};
}

let Itinerary = II_VPUSH_hi_16 in
def VPUSH_hi_16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, eR:$s2), "vpush.hi.16", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_R__s2v_wide_16__mRm
bits<4> d;
bits<4> s1;
bits<5> s2;

let mv = {d, s1, s2, 0b01, 0b0111011};
}

let Itinerary = II_VPUSH_hi_32 in
def VPUSH_hi_32 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, eR:$s2), "vpush.hi.32", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_R__s2v_wide_32__mRm
bits<4> d;
bits<4> s1;
bits<5> s2;

let mv = {d, s1, s2, 0b10, 0b0111011};
}

let Itinerary = II_VPUSH_hi_64 in
def VPUSH_hi_64 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, eL:$s2), "vpush.hi.64", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_L__s2v_wide_64__mLm
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, dontcare{1}, 0b11, 0b0111011};
}

let Itinerary = II_VPUSH_hi_8 in
def VPUSH_hi_8 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, eR:$s2), "vpush.hi.8", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_hi__vec_push_hi_R__s2v_wide_8__mRm
bits<4> d;
bits<4> s1;
bits<5> s2;

let mv = {d, s1, s2, 0b00, 0b0111011};
}

let Itinerary = II_VPUSH_lo_16 in
def VPUSH_lo_16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins eR:$s1, OP_mXn:$s2), "vpush.lo.16", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_R__s2v_wide_16__mRm
bits<4> d;
bits<5> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b1111011};
}

let Itinerary = II_VPUSH_lo_32 in
def VPUSH_lo_32 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins eR:$s1, OP_mXn:$s2), "vpush.lo.32", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_R__s2v_wide_32__mRm
bits<4> d;
bits<5> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b1111011};
}

let Itinerary = II_VPUSH_lo_64 in
def VPUSH_lo_64 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins eL:$s1, OP_mXn:$s2), "vpush.lo.64", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_L__s2v_wide_64__mLm
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, dontcare{1}, s2, 0b11, 0b1111011};
}

let Itinerary = II_VPUSH_lo_8 in
def VPUSH_lo_8 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins eR:$s1, OP_mXn:$s2), "vpush.lo.8", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_push_lo__vec_push_lo_R__s2v_wide_8__mRm
bits<4> d;
bits<5> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b1111011};
}

let Itinerary = II_VSEL_16 in
def VSEL_16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, eRS16:$sel), "vsel.16", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in32__mRS16m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b01, 0b0, 0b001};
}

let Itinerary = II_VSEL_32 in
def VSEL_32 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, eRS16:$sel), "vsel.32", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in16__mRS16m__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b00, 0b0, 0b001};
}

let Itinerary = II_VSEL_8 in
def VSEL_8 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, eL:$sel), "vsel.8", " $d, $s1, $s2, $sel">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in64__mLm__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<4> sel;

let mv = {d, s1, s2, sel, 0b10, 0b0, 0b001};
}

let Itinerary = II_VSHIFT_ALIGN in
def VSHIFT_ALIGN : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, eS:$pre, OP_mXn:$s2, mR30_shiftx:$shift), "vshift.align", "$d, $s1, $pre, $s2, r30">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shift_align__mSm__mR30_shiftx
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<2> pre;

let mv = {d, s1, s2, pre, 0b10100111};
}

let Itinerary = II_VSHIFT in
def VSHIFT : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2, eR:$shift), "vshift", "$d, $s1, $s2, $shift">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shift__mRm
bits<4> d;
bits<4> s1;
bits<4> s2;
bits<5> shift;

let mv = {d, s1, s2, shift, 0b10110};
}

let Itinerary = II_VSHUFFLE_vec_shuffle_bm in
def VSHUFFLE_vec_shuffle_bm : AIE2P_inst_mv_instr32 <(outs OP_mShflBMDst:$dst), (ins OP_mXm:$s1, OP_mXn:$s2, eR:$mod), "vshuffle", "$dst, $s1, $s2, $mod">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_bm__mRm
bits<4> dst;
bits<4> s1;
bits<4> s2;
bits<5> mod;

let mv = {dst, s1, s2, mod, 0b01110};
}

let Itinerary = II_VSHUFFLE_vec_shuffle_ex in
def VSHUFFLE_vec_shuffle_ex : AIE2P_inst_mv_instr32 <(outs OP_mEXm:$dst), (ins OP_mEXm:$s1, OP_mEXn:$s2, eR:$mod), "vshuffle", "$dst, $s1, $s2, $mod">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_ex__mRm
bits<4> dst;
bits<4> s1;
bits<4> s2;
bits<5> mod;

let mv = {dst, s1, s2, mod, 0b11110};
}

let Itinerary = II_VSHUFFLE_vec_shuffle_x in
def VSHUFFLE_vec_shuffle_x : AIE2P_inst_mv_instr32 <(outs OP_mShflXDst:$dst), (ins OP_mXm:$s1, OP_mXn:$s2, eR:$mod), "vshuffle", "$dst, $s1, $s2, $mod">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_shuffle_x__mRm
bits<4> dst;
bits<4> s1;
bits<4> s2;
bits<5> mod;

let mv = {dst, s1, s2, mod, 0b00011};
}

let Itinerary = II_VSRS_2x_mv_w_srs_bm_srsSign0, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VSRS_2x_mv_w_srs_bm_srsSign0 : AIE2P_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mBMs:$src, eS:$su), "vsrs.2x", "$dst, $src, $su, srssign0">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_bm__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<5> dst;
bits<2> su;
bits<5> src;

let st = {dst, 0b0, su, 0b0, src, 0b0, 0b01100};
}

let Itinerary = II_VSRS_2x_mv_w_srs_bm_srsSign1, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VSRS_2x_mv_w_srs_bm_srsSign1 : AIE2P_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mBMs:$src, eS:$su), "vsrs.2x", "$dst, $src, $su, srssign1">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_bm__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<5> dst;
bits<2> su;
bits<5> src;

let st = {dst, 0b0, su, 0b1, src, 0b0, 0b01100};
}

let Itinerary = II_VSRS_2x_mv_x_srs_cm_srsSign0, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VSRS_2x_mv_x_srs_cm_srsSign0 : AIE2P_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mCMs:$src, eS:$su), "vsrs.2x", "$dst, $src, $su, srssign0">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_cm__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<4> dst;
bits<2> su;
bits<4> src;

let st = {dst, 0b0, 0b1, su, 0b0, src, 0b1, 0b0, 0b01100};
}

let Itinerary = II_VSRS_2x_mv_x_srs_cm_srsSign1, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VSRS_2x_mv_x_srs_cm_srsSign1 : AIE2P_inst_st_instr32 <(outs OP_mXa:$dst), (ins OP_mCMs:$src, eS:$su), "vsrs.2x", "$dst, $src, $su, srssign1">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_cm__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<4> dst;
bits<2> su;
bits<4> src;

let st = {dst, 0b0, 0b1, su, 0b1, src, 0b1, 0b0, 0b01100};
}

let Itinerary = II_VSRS_4x_mv_w_srs_cm_srsSign0, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VSRS_4x_mv_w_srs_cm_srsSign0 : AIE2P_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mCMs:$src, eS:$su), "vsrs.4x", "$dst, $src, $su, srssign0">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_cm__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<5> dst;
bits<2> su;
bits<4> src;

let st = {dst, 0b1, su, 0b0, src, 0b0, 0b0, 0b01100};
}

let Itinerary = II_VSRS_4x_mv_w_srs_cm_srsSign1, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VSRS_4x_mv_w_srs_cm_srsSign1 : AIE2P_inst_st_instr32 <(outs OP_mWa:$dst), (ins OP_mCMs:$src, eS:$su), "vsrs.4x", "$dst, $src, $su, srssign1">{
// id: me__instr128_or__st__mv_conv__mv_w_srs_cm__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<5> dst;
bits<2> su;
bits<4> src;

let st = {dst, 0b1, su, 0b1, src, 0b0, 0b0, 0b01100};
}

let Itinerary = II_VSRS_4x_mv_x_srs_dm_srsSign0, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VSRS_4x_mv_x_srs_dm_srsSign0 : AIE2P_inst_st_instr32 <(outs OP_mXa:$dst), (ins eDM:$src, eS:$su), "vsrs.4x", "$dst, $src, $su, srssign0">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_dm__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<4> dst;
bits<2> su;
bits<3> src;

let st = {dst, 0b1, 0b1, su, 0b0, src, dontcare{1}, 0b1, 0b0, 0b01100};
}

let Itinerary = II_VSRS_4x_mv_x_srs_dm_srsSign1, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VSRS_4x_mv_x_srs_dm_srsSign1 : AIE2P_inst_st_instr32 <(outs OP_mXa:$dst), (ins eDM:$src, eS:$su), "vsrs.4x", "$dst, $src, $su, srssign1">{
// id: me__instr128_or__st__mv_conv__mv_x_srs_dm__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<4> dst;
bits<2> su;
bits<3> src;

let st = {dst, 0b1, 0b1, su, 0b1, src, dontcare{1}, 0b1, 0b0, 0b01100};
}

let Itinerary = II_VST_128_dmv_sts_w_idx, mayStore = true in
def VST_128_dmv_sts_w_idx : AIE2P_inst_st_instr32 <(outs), (ins OP_mWs:$src, eP:$ptr, eDJ:$dj), "vst.128", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmv_sts_w__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b011100};
}

let Itinerary = II_VST_128_dmv_sts_w_idx_imm, mayStore = true in
def VST_128_dmv_sts_w_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mWs:$src, eP:$ptr, c8s_step16:$imm), "vst.128", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmv_sts_w__agus_dmv__nrm__agus_dmv__normal__agus_dmv__idx_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b011100};
}

let Itinerary = II_VST_128_dmv_sts_w_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_128_dmv_sts_w_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mWs:$src, eP:$ptr, eM:$mod), "vst.128", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmv_sts_w__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b011100};
}

let Itinerary = II_VST_128_dmv_sts_w_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_128_dmv_sts_w_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mWs:$src, eP:$ptr, c8s_step16:$imm), "vst.128", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmv_sts_w__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_nrm_imm__mPs__m_c08s_step16
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b011100};
}

let Itinerary = II_VST_128_dmv_sts_w_spill, mayStore = true, Uses = [sp] in
def VST_128_dmv_sts_w_spill : AIE2P_inst_st_instr32 <(outs), (ins OP_mWs:$src, c14n_step16:$imm), "vst.128", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmv_sts_w_spill__agus_dmv__nrm_spill__agus_dmv__spill__mSPs__m_c14n_step16
bits<9> imm;
bits<5> src;

let st = {imm, src, 0b111100};
}

let Itinerary = II_VST_2D_128, DecoderMethod = "DecodeDmv_sts_w_2D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_128 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mWs:$src, eP:$ptr, eD:$mod), "vst.2d.128", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_w__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b011100};
}

let Itinerary = II_VST_2D_CONV_bf16_fp32_dmw_sts_srs_bf, DecoderMethod = "DecodeDmw_sts_srs_bf_2D", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_2D_CONV_bf16_fp32_dmw_sts_srs_bf : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mBMs:$src, eP:$ptr, eD:$mod), "vst.2d.conv.bf16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bf__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__mWbfsrs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b100101};
}

let Itinerary = II_VST_2D_CONV_bf16_fp32_dmx_sts_srs_bf, DecoderMethod = "DecodeDmx_sts_srs_bf_2D", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_2D_CONV_bf16_fp32_dmx_sts_srs_bf : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eP:$ptr, eD:$mod), "vst.2d.conv.bf16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_bf__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__mXbfsrs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b1000110};
}

let Itinerary = II_VST_2D_PACK_dmw_sts_pack_packSign0, DecoderMethod = "DecodeDmw_sts_pack_2D", mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_PACK_dmw_sts_pack_packSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mXs:$src, eP:$ptr, eD:$mod), "vst.2d.pack", "$src, packsign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b0, 0b000101};
}

let Itinerary = II_VST_2D_PACK_dmw_sts_pack_packSign1, DecoderMethod = "DecodeDmw_sts_pack_2D", mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_PACK_dmw_sts_pack_packSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mXs:$src, eP:$ptr, eD:$mod), "vst.2d.pack", "$src, packsign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b1, 0b000101};
}

let Itinerary = II_VST_2D_PACK_dmx_sts_pack_packSign0, DecoderMethod = "DecodeDmx_sts_pack_2D", mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_PACK_dmx_sts_pack_packSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eY:$src, eP:$ptr, eD:$mod), "vst.2d.pack", "$src, packsign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b100, src, dontcare{1}, 0b0, 0b011101};
}

let Itinerary = II_VST_2D_PACK_dmx_sts_pack_packSign1, DecoderMethod = "DecodeDmx_sts_pack_2D", mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_PACK_dmx_sts_pack_packSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eY:$src, eP:$ptr, eD:$mod), "vst.2d.pack", "$src, packsign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b100, src, dontcare{1}, 0b1, 0b011101};
}

let Itinerary = II_VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0, DecoderMethod = "DecodeDm_sts_srs_cm_2D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_2x_dm_sts_srs_cm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eD:$mod), "vst.2d.srs.2x", " $src, $su, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> su;

let st = {ptr, mod, 0b100, src, 0b1, su, 0b0, 0b000};
}

let Itinerary = II_VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1, DecoderMethod = "DecodeDm_sts_srs_cm_2D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_2x_dm_sts_srs_cm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eD:$mod), "vst.2d.srs.2x", " $src, $su, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> su;

let st = {ptr, mod, 0b100, src, 0b1, su, 0b1, 0b000};
}

let Itinerary = II_VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0, DecoderMethod = "DecodeDmw_sts_srs_bm_2D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mBMs:$src, eS:$su, eP:$ptr, eD:$mod), "vst.2d.srs.2x", " $src, $su, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<5> src;
bits<2> su;

let st = {ptr, mod, 0b100, src, su, 0b0, 0b001};
}

let Itinerary = II_VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1, DecoderMethod = "DecodeDmw_sts_srs_bm_2D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_2x_dmw_sts_srs_bm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mBMs:$src, eS:$su, eP:$ptr, eD:$mod), "vst.2d.srs.2x", " $src, $su, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<5> src;
bits<2> su;

let st = {ptr, mod, 0b100, src, su, 0b1, 0b001};
}

let Itinerary = II_VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0, DecoderMethod = "DecodeDm_sts_srs_cm_2D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_4x_dm_sts_srs_cm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eD:$mod), "vst.2d.srs.4x", " $src, $su, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> su;

let st = {ptr, mod, 0b100, src, 0b0, su, 0b0, 0b000};
}

let Itinerary = II_VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1, DecoderMethod = "DecodeDm_sts_srs_cm_2D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_4x_dm_sts_srs_cm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eD:$mod), "vst.2d.srs.4x", " $src, $su, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> su;

let st = {ptr, mod, 0b100, src, 0b0, su, 0b1, 0b000};
}

let Itinerary = II_VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0, DecoderMethod = "DecodeDmx_sts_srs_dm_2D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eDM:$src, eS:$su, eP:$ptr, eD:$mod), "vst.2d.srs.4x", " $src, $su, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<3> src;
bits<2> su;

let st = {ptr, mod, 0b100, src, dontcare{2-1}, su, 0b0, 0b010};
}

let Itinerary = II_VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1, DecoderMethod = "DecodeDmx_sts_srs_dm_2D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_2D_SRS_4x_dmx_sts_srs_dm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins eDM:$src, eS:$su, eP:$ptr, eD:$mod), "vst.2d.srs.4x", " $src, $su, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<3> src;
bits<2> su;

let st = {ptr, mod, 0b100, src, dontcare{2-1}, su, 0b1, 0b010};
}

let Itinerary = II_VST_2D_dmw_sts_w, DecoderMethod = "DecodeDmw_sts_w_2D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_dmw_sts_w : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mWs:$src, eP:$ptr, eD:$mod), "vst.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_w__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b010101};
}

let Itinerary = II_VST_2D_dmx_sts_bm, DecoderMethod = "DecodeDmx_sts_bm_2D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_dmx_sts_bm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mBMs:$src, eP:$ptr, eD:$mod), "vst.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_bm__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b100, src, 0b001101};
}

let Itinerary = II_VST_2D_dmx_sts_fifohl, ItineraryRegPairs = [
ItinRegClassPair<II_VST_2D_dmx_sts_fifohl_mStFifol, [OperandRegClass<2, mStFifol>]>,
ItinRegClassPair<II_VST_2D_dmx_sts_fifohl_eLdFifoLReg, [OperandRegClass<2, eLdFifoLReg>]>,
ItinRegClassPair<II_VST_2D_dmx_sts_fifohl_eLdFifoHReg, [OperandRegClass<2, eLdFifoHReg>]>,
ItinRegClassPair<II_VST_2D_dmx_sts_fifohl_mFifoExtra, [OperandRegClass<2, mFifoExtra>]>,
ItinRegClassPair<II_VST_2D_dmx_sts_fifohl_mStFifoh, [OperandRegClass<2, mStFifoh>]>], DecoderMethod = "DecodeDmx_sts_fifohl_2D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_dmx_sts_fifohl : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mFifoHLReg:$src, eP:$ptr, eD:$mod), "vst.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifohl__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b100, src, 0b01010110};
}

let Itinerary = II_VST_2D_dmx_sts_x, DecoderMethod = "DecodeDmx_sts_x_2D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_2D_dmx_sts_x : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDC:$dc), (ins OP_mXs:$src, eP:$ptr, eD:$mod), "vst.2d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_x__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_2d__mPs__mDs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b100, src, 0b0100110};
}

let Itinerary = II_VST_3D_128, DecoderMethod = "DecodeDmv_sts_w_3D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_128 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mWs:$src, eP:$ptr, eDS:$mod), "vst.3d.128", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmv_sts_w__agus_dmv__nrm__agus_dmv__normal__agus_dmv__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b011100};
}

let Itinerary = II_VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf, DecoderMethod = "DecodeDmw_sts_srs_bf_3D", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_3D_CONV_bf16_fp32_dmw_sts_srs_bf : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mBMs:$src, eP:$ptr, eDS:$mod), "vst.3d.conv.bf16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bf__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__mWbfsrs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b100101};
}

let Itinerary = II_VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf, DecoderMethod = "DecodeDmx_sts_srs_bf_3D", mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_3D_CONV_bf16_fp32_dmx_sts_srs_bf : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eP:$ptr, eDS:$mod), "vst.3d.conv.bf16.fp32", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_bf__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__mXbfsrs
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1000110};
}

let Itinerary = II_VST_3D_PACK_dmw_sts_pack_packSign0, DecoderMethod = "DecodeDmw_sts_pack_3D", mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_PACK_dmw_sts_pack_packSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mXs:$src, eP:$ptr, eDS:$mod), "vst.3d.pack", "$src, packsign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b0, 0b000101};
}

let Itinerary = II_VST_3D_PACK_dmw_sts_pack_packSign1, DecoderMethod = "DecodeDmw_sts_pack_3D", mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_PACK_dmw_sts_pack_packSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mXs:$src, eP:$ptr, eDS:$mod), "vst.3d.pack", "$src, packsign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1, 0b000101};
}

let Itinerary = II_VST_3D_PACK_dmx_sts_pack_packSign0, DecoderMethod = "DecodeDmx_sts_pack_3D", mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_PACK_dmx_sts_pack_packSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eY:$src, eP:$ptr, eDS:$mod), "vst.3d.pack", "$src, packsign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<2> mod;
bits<3> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, dontcare{1}, 0b0, 0b011101};
}

let Itinerary = II_VST_3D_PACK_dmx_sts_pack_packSign1, DecoderMethod = "DecodeDmx_sts_pack_3D", mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_PACK_dmx_sts_pack_packSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eY:$src, eP:$ptr, eDS:$mod), "vst.3d.pack", "$src, packsign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<2> mod;
bits<3> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, dontcare{1}, 0b1, 0b011101};
}

let Itinerary = II_VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0, DecoderMethod = "DecodeDm_sts_srs_cm_3D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_2x_dm_sts_srs_cm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eDS:$mod), "vst.3d.srs.2x", " $src, $su, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> su;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1, su, 0b0, 0b000};
}

let Itinerary = II_VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1, DecoderMethod = "DecodeDm_sts_srs_cm_3D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_2x_dm_sts_srs_cm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eDS:$mod), "vst.3d.srs.2x", " $src, $su, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> su;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b1, su, 0b1, 0b000};
}

let Itinerary = II_VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0, DecoderMethod = "DecodeDmw_sts_srs_bm_3D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mBMs:$src, eS:$su, eP:$ptr, eDS:$mod), "vst.3d.srs.2x", " $src, $su, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<2> mod;
bits<5> src;
bits<2> su;

let st = {ptr, dontcare{1}, mod, 0b110, src, su, 0b0, 0b001};
}

let Itinerary = II_VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1, DecoderMethod = "DecodeDmw_sts_srs_bm_3D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_2x_dmw_sts_srs_bm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mBMs:$src, eS:$su, eP:$ptr, eDS:$mod), "vst.3d.srs.2x", " $src, $su, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<2> mod;
bits<5> src;
bits<2> su;

let st = {ptr, dontcare{1}, mod, 0b110, src, su, 0b1, 0b001};
}

let Itinerary = II_VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0, DecoderMethod = "DecodeDm_sts_srs_cm_3D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_4x_dm_sts_srs_cm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eDS:$mod), "vst.3d.srs.4x", " $src, $su, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> su;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b0, su, 0b0, 0b000};
}

let Itinerary = II_VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1, DecoderMethod = "DecodeDm_sts_srs_cm_3D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_4x_dm_sts_srs_cm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eDS:$mod), "vst.3d.srs.4x", " $src, $su, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<2> mod;
bits<4> src;
bits<2> su;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b0, su, 0b1, 0b000};
}

let Itinerary = II_VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0, DecoderMethod = "DecodeDmx_sts_srs_dm_3D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eDM:$src, eS:$su, eP:$ptr, eDS:$mod), "vst.3d.srs.4x", " $src, $su, srssign0, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<2> mod;
bits<3> src;
bits<2> su;

let st = {ptr, dontcare{1}, mod, 0b110, src, dontcare{2-1}, su, 0b0, 0b010};
}

let Itinerary = II_VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1, DecoderMethod = "DecodeDmx_sts_srs_dm_3D", mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_3D_SRS_4x_dmx_sts_srs_dm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins eDM:$src, eS:$su, eP:$ptr, eDS:$mod), "vst.3d.srs.4x", " $src, $su, srssign1, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<2> mod;
bits<3> src;
bits<2> su;

let st = {ptr, dontcare{1}, mod, 0b110, src, dontcare{2-1}, su, 0b1, 0b010};
}

let Itinerary = II_VST_3D_dmw_sts_w, DecoderMethod = "DecodeDmw_sts_w_3D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_dmw_sts_w : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mWs:$src, eP:$ptr, eDS:$mod), "vst.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmw_sts_w__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b010101};
}

let Itinerary = II_VST_3D_dmx_sts_bm, DecoderMethod = "DecodeDmx_sts_bm_3D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_dmx_sts_bm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mBMs:$src, eP:$ptr, eDS:$mod), "vst.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_bm__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<5> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b001101};
}

let Itinerary = II_VST_3D_dmx_sts_fifohl, ItineraryRegPairs = [
ItinRegClassPair<II_VST_3D_dmx_sts_fifohl_mStFifol, [OperandRegClass<3, mStFifol>]>,
ItinRegClassPair<II_VST_3D_dmx_sts_fifohl_eLdFifoLReg, [OperandRegClass<3, eLdFifoLReg>]>,
ItinRegClassPair<II_VST_3D_dmx_sts_fifohl_eLdFifoHReg, [OperandRegClass<3, eLdFifoHReg>]>,
ItinRegClassPair<II_VST_3D_dmx_sts_fifohl_mFifoExtra, [OperandRegClass<3, mFifoExtra>]>,
ItinRegClassPair<II_VST_3D_dmx_sts_fifohl_mStFifoh, [OperandRegClass<3, mStFifoh>]>], DecoderMethod = "DecodeDmx_sts_fifohl_3D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_dmx_sts_fifohl : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mFifoHLReg:$src, eP:$ptr, eDS:$mod), "vst.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifohl__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<3> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b01010110};
}

let Itinerary = II_VST_3D_dmx_sts_x, DecoderMethod = "DecodeDmx_sts_x_3D", mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_3D_dmx_sts_x : AIE2P_inst_st_instr32 <(outs eP:$ptr_out, eDCL:$dcl, eDCH:$dch), (ins OP_mXs:$src, eP:$ptr, eDS:$mod), "vst.3d", " $src, [$ptr], $mod">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_x__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_3d__mPs__mDSs
bits<3> ptr;
bits<2> mod;
bits<4> src;

let st = {ptr, dontcare{1}, mod, 0b110, src, 0b0100110};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmw_sts_srs_bf_idx, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd] in
def VST_CONV_bf16_fp32_dmw_sts_srs_bf_idx : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eP:$ptr, eDJ:$dj), "vst.conv.bf16.fp32", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_srs_bf__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__mWbfsrs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b100101};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmw_sts_srs_bf_idx_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd] in
def VST_CONV_bf16_fp32_dmw_sts_srs_bf_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eP:$ptr, c9s_step32:$imm), "vst.conv.bf16.fp32", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_srs_bf__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__mWbfsrs
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b100101};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmw_sts_srs_bf_pstm_nrm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf16_fp32_dmw_sts_srs_bf_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eP:$ptr, eM:$mod), "vst.conv.bf16.fp32", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_srs_bf__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__mWbfsrs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b100101};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmw_sts_srs_bf_pstm_nrm_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf16_fp32_dmw_sts_srs_bf_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eP:$ptr, c9s_step32:$imm), "vst.conv.bf16.fp32", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_srs_bf__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__mWbfsrs
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b100101};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmx_sts_srs_bf_idx, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd] in
def VST_CONV_bf16_fp32_dmx_sts_srs_bf_idx : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eP:$ptr, eDJ:$dj), "vst.conv.bf16.fp32", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_srs_bf__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__mXbfsrs
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b1000110};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmx_sts_srs_bf_idx_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd] in
def VST_CONV_bf16_fp32_dmx_sts_srs_bf_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eP:$ptr, c10s_step64:$imm), "vst.conv.bf16.fp32", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_srs_bf__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__mXbfsrs
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b1000110};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmx_sts_srs_bf_pstm_nrm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf16_fp32_dmx_sts_srs_bf_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eP:$ptr, eM:$mod), "vst.conv.bf16.fp32", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_srs_bf__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__mXbfsrs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b1000110};
}

let Itinerary = II_VST_CONV_bf16_fp32_dmx_sts_srs_bf_pstm_nrm_imm, mayStore = true, Defs = [srF2FFlags], Uses = [crF2FMask, crRnd], Constraints = "$ptr_out = $ptr" in
def VST_CONV_bf16_fp32_dmx_sts_srs_bf_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eP:$ptr, c10s_step64:$imm), "vst.conv.bf16.fp32", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_srs_bf__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__mXbfsrs
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b1000110};
}

let Itinerary = II_VST_FLUSH_512_2D, DecoderMethod = "DecodeDmx_sts_fifo_bare_x__fifo_2d_flush", mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_512_2D : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out, eDC:$dc), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eD:$mod), "vst.flush.512.2d", " [p2, sf, r26, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_bare_x__fifo_2d_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__fifo_2d__mPfs__mR26_fifo_st__mDs
bits<3> mod;

let st = {0b010, mod, dontcare{3-1}, dontcare{4-1}, 0b0000110};
}

let Itinerary = II_VST_FLUSH_512_3D, DecoderMethod = "DecodeDmx_sts_fifo_bare_x__fifo_3d_flush", mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_512_3D : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out, eDCL:$dcl, eDCH:$dch), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eDS:$mod), "vst.flush.512.3d", " [p2, sf, r26, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_bare_x__fifo_3d_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__fifo_3d__mPfs__mR26_fifo_st__mDSs
bits<2> mod;

let st = {0b100, 0b0, mod, dontcare{3-1}, dontcare{4-1}, 0b0000110};
}

let Itinerary = II_VST_FLUSH_512_CONV_2D, DecoderMethod = "DecodeDmx_sts_fifo_conv_x__fifo_2d_flush", mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_512_CONV_2D : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out, eDC:$dc), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eD:$mod), "vst.flush.512.conv.2d", " [p2, sf, r26, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_conv_x__fifo_2d_flush__dmx_sts_fifo_conv_x__core_flush__mStFifo__dmx_sts_fifo_conv_x__fifo_2d__mPfs__mR26_fifo_st__mDs
bits<3> mod;

let st = {0b011, mod, dontcare{3-1}, dontcare{4-1}, 0b0000110};
}

let Itinerary = II_VST_FLUSH_512_CONV_3D, DecoderMethod = "DecodeDmx_sts_fifo_conv_x__fifo_3d_flush", mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_512_CONV_3D : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out, eDCL:$dcl, eDCH:$dch), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eDS:$mod), "vst.flush.512.conv.3d", " [p2, sf, r26, $mod]">, AIE_HasTiedSubregister{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_conv_x__fifo_3d_flush__dmx_sts_fifo_conv_x__core_flush__mStFifo__dmx_sts_fifo_conv_x__fifo_3d__mPfs__mR26_fifo_st__mDSs
bits<2> mod;

let st = {0b101, 0b0, mod, dontcare{3-1}, dontcare{4-1}, 0b0000110};
}

let Itinerary = II_VST_FLUSH_512_CONV_fifo_1d_flush, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_512_CONV_fifo_1d_flush : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eM:$mod), "vst.flush.512.conv", " [p2, sf, r26, $mod]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_conv_x__fifo_1d_flush__dmx_sts_fifo_conv_x__core_flush__mStFifo__dmx_sts_fifo_conv_x__fifo_1d__mPfs__mR26_fifo_st__mMs
bits<3> mod;

let st = {0b111, mod, dontcare{3-1}, dontcare{4-1}, 0b0000110};
}

let Itinerary = II_VST_FLUSH_512_CONV_normal_flush, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_512_CONV_normal_flush : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail), "vst.flush.512.conv", " [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_conv_x__normal_flush__dmx_sts_fifo_conv_x__core_flush__mStFifo__dmx_sts_fifo_conv_x__normal__mPfs__mR26_fifo_st

let st = {0b001, dontcare{1}, dontcare{2-1}, dontcare{3-1}, dontcare{4-1}, 0b0000110};
}

let Itinerary = II_VST_FLUSH_512_fifo_1d_flush, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_512_fifo_1d_flush : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail, eM:$mod), "vst.flush.512", " [p2, sf, r26, $mod]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_bare_x__fifo_1d_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__fifo_1d__mPfs__mR26_fifo_st__mMs
bits<3> mod;

let st = {0b110, mod, dontcare{3-1}, dontcare{4-1}, 0b0000110};
}

let Itinerary = II_VST_FLUSH_512_normal_flush, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_FLUSH_512_normal_flush : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, mPfs:$ptr, mR26_fifo_st:$avail), "vst.flush.512", " [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_bare_x__normal_flush__dmx_sts_fifo_bare_x__core_flush__mStFifo__dmx_sts_fifo_bare_x__normal__mPfs__mR26_fifo_st

let st = {0b000, dontcare{1}, dontcare{2-1}, dontcare{3-1}, dontcare{4-1}, 0b0000110};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_idx_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0] in
def VST_PACK_dmw_sts_pack_idx_packSign0 : AIE2P_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, eDJ:$dj), "vst.pack", "$src, packsign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b0, 0b000101};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_idx_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1] in
def VST_PACK_dmw_sts_pack_idx_packSign1 : AIE2P_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, eDJ:$dj), "vst.pack", "$src, packsign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b1, 0b000101};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_idx_imm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0] in
def VST_PACK_dmw_sts_pack_idx_imm_packSign0 : AIE2P_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, c9s_step32:$imm), "vst.pack", "$src, packsign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b0, 0b000101};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_idx_imm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1] in
def VST_PACK_dmw_sts_pack_idx_imm_packSign1 : AIE2P_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, c9s_step32:$imm), "vst.pack", "$src, packsign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b1, 0b000101};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_pstm_nrm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmw_sts_pack_pstm_nrm_packSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, eM:$mod), "vst.pack", "$src, packsign0, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b0, 0b000101};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_pstm_nrm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmw_sts_pack_pstm_nrm_packSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, eM:$mod), "vst.pack", "$src, packsign1, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b1, 0b000101};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, c9s_step32:$imm), "vst.pack", "$src, packsign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b0, 0b000101};
}

let Itinerary = II_VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmw_sts_pack_pstm_nrm_imm_packSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, c9s_step32:$imm), "vst.pack", "$src, packsign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_pack__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__dmw_sts_pack_core__packSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b1, 0b000101};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_idx_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0] in
def VST_PACK_dmx_sts_pack_idx_packSign0 : AIE2P_inst_st_instr32 <(outs), (ins eY:$src, eP:$ptr, eDJ:$dj), "vst.pack", "$src, packsign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<3> dj;
bits<3> src;

let st = {ptr, dj, 0b000, src, dontcare{1}, 0b0, 0b011101};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_idx_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1] in
def VST_PACK_dmx_sts_pack_idx_packSign1 : AIE2P_inst_st_instr32 <(outs), (ins eY:$src, eP:$ptr, eDJ:$dj), "vst.pack", "$src, packsign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<3> dj;
bits<3> src;

let st = {ptr, dj, 0b000, src, dontcare{1}, 0b1, 0b011101};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_idx_imm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0] in
def VST_PACK_dmx_sts_pack_idx_imm_packSign0 : AIE2P_inst_st_instr32 <(outs), (ins eY:$src, eP:$ptr, c10s_step64:$imm), "vst.pack", "$src, packsign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b01, src, dontcare{1}, 0b0, 0b011101};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_idx_imm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1] in
def VST_PACK_dmx_sts_pack_idx_imm_packSign1 : AIE2P_inst_st_instr32 <(outs), (ins eY:$src, eP:$ptr, c10s_step64:$imm), "vst.pack", "$src, packsign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b01, src, dontcare{1}, 0b1, 0b011101};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_pstm_nrm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmx_sts_pack_pstm_nrm_packSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eY:$src, eP:$ptr, eM:$mod), "vst.pack", "$src, packsign0, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b010, src, dontcare{1}, 0b0, 0b011101};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_pstm_nrm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmx_sts_pack_pstm_nrm_packSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eY:$src, eP:$ptr, eM:$mod), "vst.pack", "$src, packsign1, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b010, src, dontcare{1}, 0b1, 0b011101};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign0, mayStore = true, Uses = [crPackSize, crSat, packSign0], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eY:$src, eP:$ptr, c10s_step64:$imm), "vst.pack", "$src, packsign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign0
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b11, src, dontcare{1}, 0b0, 0b011101};
}

let Itinerary = II_VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign1, mayStore = true, Uses = [crPackSize, crSat, packSign1], Constraints = "$ptr_out = $ptr" in
def VST_PACK_dmx_sts_pack_pstm_nrm_imm_packSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eY:$src, eP:$ptr, c10s_step64:$imm), "vst.pack", "$src, packsign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_pack__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__dmx_sts_pack_core__mYs__packSign1
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b11, src, dontcare{1}, 0b1, 0b011101};
}

let Itinerary = II_VST_PUSH_512, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_512 : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mXs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.512", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_bare_x__normal_push__dmx_sts_fifo_bare_x__core_push__mStFifo__dmx_sts_fifo_bare_x__normal__mPfs__mR26_fifo_st
bits<4> src;

let st = {0b100, 0b1, 0b00, dontcare{3-1}, src, 0b0000110};
}

let Itinerary = II_VST_PUSH_544_CONV_bfp16ebs16_ebs8, mayStore = true, Defs = [srF2BFlags, srFifo_of], Uses = [crF2BMask, crRnd], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_544_CONV_bfp16ebs16_ebs8 : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mEXs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.544.conv.bfp16ebs16.ebs8", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_conv_ex_ebs8_ebs16__normal_push__dmx_sts_fifo_conv_ex_ebs8_ebs16__core_push__mBp2Bp__mStFifo__dmx_sts_fifo_conv_ex_ebs8_ebs16__normal__mPfs__mR26_fifo_st
bits<4> src;

let st = {0b101, 0b1, 0b01, dontcare{3-1}, src, 0b0000110};
}

let Itinerary = II_VST_PUSH_544_CONV_bfp16ebs16_fp32, mayStore = true, Defs = [srF2BFlags, srFifo_of], Uses = [crF2BMask, crRnd], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_544_CONV_bfp16ebs16_fp32 : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, eDM:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.544.conv.bfp16ebs16.fp32", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_conv_ex_fp32_ebs16__normal_push__dmx_sts_fifo_conv_ex_fp32_ebs16__core_push__mFp2B1__mDMs__mStFifo__dmx_sts_fifo_conv_ex_fp32_ebs16__normal__mPfs__mR26_fifo_st
bits<3> src;

let st = {0b101, 0b1, 0b10, dontcare{3-1}, src, dontcare{1}, 0b0000110};
}

let Itinerary = II_VST_PUSH_544, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_544 : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mEXs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.544", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_bare_ex_ebs16__normal_push__dmx_sts_fifo_bare_ex_ebs16__core_push__mStFifo__dmx_sts_fifo_bare_ex_ebs16__normal__mPfs__mR26_fifo_st
bits<4> src;

let st = {0b100, 0b1, 0b01, dontcare{3-1}, src, 0b0000110};
}

let Itinerary = II_VST_PUSH_576_CONV_bfp16ebs8_fp32, mayStore = true, Defs = [srF2BFlags, srFifo_of], Uses = [crF2BMask, crRnd], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_576_CONV_bfp16ebs8_fp32 : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, eDM:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.576.conv.bfp16ebs8.fp32", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_conv_ex_fp32_ebs8__normal_push__dmx_sts_fifo_conv_ex_fp32_ebs8__core_push__mFp2B0__mDMs__mStFifo__dmx_sts_fifo_conv_ex_fp32_ebs8__normal__mPfs__mR26_fifo_st
bits<3> src;

let st = {0b101, 0b1, 0b00, dontcare{3-1}, src, dontcare{1}, 0b0000110};
}

let Itinerary = II_VST_PUSH_576, mayStore = true, Defs = [srFifo_of], Constraints = "$fifo_reg_out = $fifo_reg, $ptr_out = $ptr, $avail_out = $avail" in
def VST_PUSH_576 : AIE2P_inst_st_instr32 <(outs mStFifo:$fifo_reg_out, mPfs:$ptr_out, mR26_fifo_st:$avail_out), (ins mStFifo:$fifo_reg, OP_mEXs:$src, mPfs:$ptr, mR26_fifo_st:$avail), "vst.push.576", " $src, [p2, sf, r26]">{
// id: me__instr128_or__st__dmx_sts_fifo__dmx_sts_fifo_bare_ex_ebs8__normal_push__dmx_sts_fifo_bare_ex_ebs8__core_push__mStFifo__dmx_sts_fifo_bare_ex_ebs8__normal__mPfs__mR26_fifo_st
bits<4> src;

let st = {0b100, 0b1, 0b10, dontcare{3-1}, src, 0b0000110};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_2x_dm_sts_srs_cm_idx_srsSign0 : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eDJ:$dj), "vst.srs.2x", " $src, $su, srssign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> su;

let st = {ptr, dj, 0b000, src, 0b1, su, 0b0, 0b000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_idx_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_2x_dm_sts_srs_cm_idx_srsSign1 : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eDJ:$dj), "vst.srs.2x", " $src, $su, srssign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> su;

let st = {ptr, dj, 0b000, src, 0b1, su, 0b1, 0b000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign0 : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$su, eP:$ptr, c10s_step64:$imm), "vst.srs.2x", " $src, $su, srssign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> su;

let st = {ptr, imm, 0b01, src, 0b1, su, 0b0, 0b000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_2x_dm_sts_srs_cm_idx_imm_srsSign1 : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$su, eP:$ptr, c10s_step64:$imm), "vst.srs.2x", " $src, $su, srssign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> su;

let st = {ptr, imm, 0b01, src, 0b1, su, 0b1, 0b000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eM:$mod), "vst.srs.2x", " $src, $su, srssign0, [$ptr], $mod">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> su;

let st = {ptr, mod, 0b010, src, 0b1, su, 0b0, 0b000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eM:$mod), "vst.srs.2x", " $src, $su, srssign1, [$ptr], $mod">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> su;

let st = {ptr, mod, 0b010, src, 0b1, su, 0b1, 0b000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$su, eP:$ptr, c10s_step64:$imm), "vst.srs.2x", " $src, $su, srssign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> su;

let st = {ptr, imm, 0b11, src, 0b1, su, 0b0, 0b000};
}

let Itinerary = II_VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$su, eP:$ptr, c10s_step64:$imm), "vst.srs.2x", " $src, $su, srssign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmx_sts_srs_cm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mXdlsrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> su;

let st = {ptr, imm, 0b11, src, 0b1, su, 0b1, 0b000};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign0 : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eS:$su, eP:$ptr, eDJ:$dj), "vst.srs.2x", " $src, $su, srssign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> dj;
bits<5> src;
bits<2> su;

let st = {ptr, dj, 0b000, src, su, 0b0, 0b001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_2x_dmw_sts_srs_bm_idx_srsSign1 : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eS:$su, eP:$ptr, eDJ:$dj), "vst.srs.2x", " $src, $su, srssign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> dj;
bits<5> src;
bits<2> su;

let st = {ptr, dj, 0b000, src, su, 0b1, 0b001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign0 : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eS:$su, eP:$ptr, c9s_step32:$imm), "vst.srs.2x", " $src, $su, srssign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<5> src;
bits<2> su;

let st = {ptr, imm, 0b01, src, su, 0b0, 0b001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_2x_dmw_sts_srs_bm_idx_imm_srsSign1 : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eS:$su, eP:$ptr, c9s_step32:$imm), "vst.srs.2x", " $src, $su, srssign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<5> src;
bits<2> su;

let st = {ptr, imm, 0b01, src, su, 0b1, 0b001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eS:$su, eP:$ptr, eM:$mod), "vst.srs.2x", " $src, $su, srssign0, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<5> src;
bits<2> su;

let st = {ptr, mod, 0b010, src, su, 0b0, 0b001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eS:$su, eP:$ptr, eM:$mod), "vst.srs.2x", " $src, $su, srssign1, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<5> src;
bits<2> su;

let st = {ptr, mod, 0b010, src, su, 0b1, 0b001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eS:$su, eP:$ptr, c9s_step32:$imm), "vst.srs.2x", " $src, $su, srssign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<5> src;
bits<2> su;

let st = {ptr, imm, 0b11, src, su, 0b0, 0b001};
}

let Itinerary = II_VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_2x_dmw_sts_srs_bm_pstm_nrm_imm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eS:$su, eP:$ptr, c9s_step32:$imm), "vst.srs.2x", " $src, $su, srssign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_srs_bm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWlsrsl__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<5> src;
bits<2> su;

let st = {ptr, imm, 0b11, src, su, 0b1, 0b001};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_idx_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_4x_dm_sts_srs_cm_idx_srsSign0 : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eDJ:$dj), "vst.srs.4x", " $src, $su, srssign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> su;

let st = {ptr, dj, 0b000, src, 0b0, su, 0b0, 0b000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_idx_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_4x_dm_sts_srs_cm_idx_srsSign1 : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eDJ:$dj), "vst.srs.4x", " $src, $su, srssign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx__mPs__mDJs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> dj;
bits<4> src;
bits<2> su;

let st = {ptr, dj, 0b000, src, 0b0, su, 0b1, 0b000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign0 : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$su, eP:$ptr, c9s_step32:$imm), "vst.srs.4x", " $src, $su, srssign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> su;

let st = {ptr, imm, 0b01, src, 0b0, su, 0b0, 0b000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_4x_dm_sts_srs_cm_idx_imm_srsSign1 : AIE2P_inst_st_instr32 <(outs), (ins OP_mCMs:$src, eS:$su, eP:$ptr, c9s_step32:$imm), "vst.srs.4x", " $src, $su, srssign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> su;

let st = {ptr, imm, 0b01, src, 0b0, su, 0b1, 0b000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eM:$mod), "vst.srs.4x", " $src, $su, srssign0, [$ptr], $mod">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> su;

let st = {ptr, mod, 0b010, src, 0b0, su, 0b0, 0b000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$su, eP:$ptr, eM:$mod), "vst.srs.4x", " $src, $su, srssign1, [$ptr], $mod">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<4> src;
bits<2> su;

let st = {ptr, mod, 0b010, src, 0b0, su, 0b1, 0b000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$su, eP:$ptr, c9s_step32:$imm), "vst.srs.4x", " $src, $su, srssign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> su;

let st = {ptr, imm, 0b11, src, 0b0, su, 0b0, 0b000};
}

let Itinerary = II_VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dm_sts_srs_cm_pstm_nrm_imm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mCMs:$src, eS:$su, eP:$ptr, c9s_step32:$imm), "vst.srs.4x", " $src, $su, srssign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dm_sts_srs_cm__dmw_sts_srs_cm__agus_dmw__srs__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32__srs__mWssrs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<4> src;
bits<2> su;

let st = {ptr, imm, 0b11, src, 0b0, su, 0b1, 0b000};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign0 : AIE2P_inst_st_instr32 <(outs), (ins eDM:$src, eS:$su, eP:$ptr, eDJ:$dj), "vst.srs.4x", " $src, $su, srssign0, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> dj;
bits<3> src;
bits<2> su;

let st = {ptr, dj, 0b000, src, dontcare{2-1}, su, 0b0, 0b010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_4x_dmx_sts_srs_dm_idx_srsSign1 : AIE2P_inst_st_instr32 <(outs), (ins eDM:$src, eS:$su, eP:$ptr, eDJ:$dj), "vst.srs.4x", " $src, $su, srssign1, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx__mPs__mDJs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> dj;
bits<3> src;
bits<2> su;

let st = {ptr, dj, 0b000, src, dontcare{2-1}, su, 0b1, 0b010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0] in
def VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign0 : AIE2P_inst_st_instr32 <(outs), (ins eDM:$src, eS:$su, eP:$ptr, c10s_step64:$imm), "vst.srs.4x", " $src, $su, srssign0, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<3> src;
bits<2> su;

let st = {ptr, imm, 0b01, src, dontcare{2-1}, su, 0b0, 0b010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1] in
def VST_SRS_4x_dmx_sts_srs_dm_idx_imm_srsSign1 : AIE2P_inst_st_instr32 <(outs), (ins eDM:$src, eS:$su, eP:$ptr, c10s_step64:$imm), "vst.srs.4x", " $src, $su, srssign1, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<3> src;
bits<2> su;

let st = {ptr, imm, 0b01, src, dontcare{2-1}, su, 0b1, 0b010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eDM:$src, eS:$su, eP:$ptr, eM:$mod), "vst.srs.4x", " $src, $su, srssign0, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<3> mod;
bits<3> src;
bits<2> su;

let st = {ptr, mod, 0b010, src, dontcare{2-1}, su, 0b0, 0b010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eDM:$src, eS:$su, eP:$ptr, eM:$mod), "vst.srs.4x", " $src, $su, srssign1, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<3> mod;
bits<3> src;
bits<2> su;

let st = {ptr, mod, 0b010, src, dontcare{2-1}, su, 0b1, 0b010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign0, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign0], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign0 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eDM:$src, eS:$su, eP:$ptr, c10s_step64:$imm), "vst.srs.4x", " $src, $su, srssign0, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign0
bits<3> ptr;
bits<4> imm;
bits<3> src;
bits<2> su;

let st = {ptr, imm, 0b11, src, dontcare{2-1}, su, 0b0, 0b010};
}

let Itinerary = II_VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign1, mayStore = true, Defs = [srSRS_of], Uses = [crRnd, crSRSMode, crSat, srsSign1], Constraints = "$ptr_out = $ptr" in
def VST_SRS_4x_dmx_sts_srs_dm_pstm_nrm_imm_srsSign1 : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins eDM:$src, eS:$su, eP:$ptr, c10s_step64:$imm), "vst.srs.4x", " $src, $su, srssign1, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_srs_dm__agus_dmx__srs__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64__srs__mDdssrs__mDMs__srs__baseSrs__mSs__srsSign1
bits<3> ptr;
bits<4> imm;
bits<3> src;
bits<2> su;

let st = {ptr, imm, 0b11, src, dontcare{2-1}, su, 0b1, 0b010};
}

let Itinerary = II_VST_dmw_sts_w_idx, mayStore = true in
def VST_dmw_sts_w_idx : AIE2P_inst_st_instr32 <(outs), (ins OP_mWs:$src, eP:$ptr, eDJ:$dj), "vst", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmw_sts_w__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b010101};
}

let Itinerary = II_VST_dmw_sts_w_idx_imm, mayStore = true in
def VST_dmw_sts_w_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mWs:$src, eP:$ptr, c9s_step32:$imm), "vst", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmw_sts_w__agus_dmw__nrm__agus_dmw__normal__agus_dmw__idx_imm__mPs__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b010101};
}

let Itinerary = II_VST_dmw_sts_w_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmw_sts_w_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mWs:$src, eP:$ptr, eM:$mod), "vst", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmw_sts_w__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b010101};
}

let Itinerary = II_VST_dmw_sts_w_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmw_sts_w_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mWs:$src, eP:$ptr, c9s_step32:$imm), "vst", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmw_sts_w__agus_dmw__nrm__agus_dmw__normal__agus_dmw__pstm_nrm_imm__mPs__m_c09s_step32
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b010101};
}

let Itinerary = II_VST_dmw_sts_w_spill, mayStore = true, Uses = [sp] in
def VST_dmw_sts_w_spill : AIE2P_inst_st_instr32 <(outs), (ins OP_mWs:$src, c15n_step32:$imm), "vst", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmw_sts_w_spill__agus_dmw__nrm_spill__agus_dmw__spill__mSPs__m_c15n_step32
bits<9> imm;
bits<5> src;

let st = {imm, src, 0b110101};
}

let Itinerary = II_VST_dmx_sts_bm_idx, mayStore = true in
def VST_dmx_sts_bm_idx : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eP:$ptr, eDJ:$dj), "vst", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_bm__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<5> src;

let st = {ptr, dj, 0b000, src, 0b001101};
}

let Itinerary = II_VST_dmx_sts_bm_idx_imm, mayStore = true in
def VST_dmx_sts_bm_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_bm__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b01, src, 0b001101};
}

let Itinerary = II_VST_dmx_sts_bm_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_bm_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eP:$ptr, eM:$mod), "vst", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_bm__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<5> src;

let st = {ptr, mod, 0b010, src, 0b001101};
}

let Itinerary = II_VST_dmx_sts_bm_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_bm_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mBMs:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_bm__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<5> src;

let st = {ptr, imm, 0b11, src, 0b001101};
}

let Itinerary = II_VST_dmx_sts_bm_spill, mayStore = true, Uses = [sp] in
def VST_dmx_sts_bm_spill : AIE2P_inst_st_instr32 <(outs), (ins OP_mBMs:$src, c16n_step64:$imm), "vst", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmx_sts_bm_spill__agus_dmx__nrm_spill__agus_dmx__spill__mSPs__m_c16n_step64
bits<9> imm;
bits<5> src;

let st = {imm, src, 0b101101};
}

let Itinerary = II_VST_dmx_sts_fifohl_idx, ItineraryRegPairs = [
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_mStFifoh, [OperandRegClass<0, mStFifoh>]>], mayStore = true in
def VST_dmx_sts_fifohl_idx : AIE2P_inst_st_instr32 <(outs), (ins OP_mFifoHLReg:$src, eP:$ptr, eDJ:$dj), "vst", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_fifohl__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<3> src;

let st = {ptr, dj, 0b000, src, 0b01010110};
}

let Itinerary = II_VST_dmx_sts_fifohl_idx_imm, ItineraryRegPairs = [
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_imm_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_imm_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_imm_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_imm_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_idx_imm_mStFifoh, [OperandRegClass<0, mStFifoh>]>], mayStore = true in
def VST_dmx_sts_fifohl_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mFifoHLReg:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_fifohl__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b01, src, 0b01010110};
}

let Itinerary = II_VST_dmx_sts_fifohl_pstm_nrm, ItineraryRegPairs = [
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_mStFifol, [OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_eLdFifoLReg, [OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_eLdFifoHReg, [OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_mFifoExtra, [OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_mStFifoh, [OperandRegClass<1, mStFifoh>]>], mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_fifohl_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mFifoHLReg:$src, eP:$ptr, eM:$mod), "vst", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_fifohl__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<3> src;

let st = {ptr, mod, 0b010, src, 0b01010110};
}

let Itinerary = II_VST_dmx_sts_fifohl_pstm_nrm_imm, ItineraryRegPairs = [
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_imm_mStFifol, [OperandRegClass<1, mStFifol>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_imm_eLdFifoLReg, [OperandRegClass<1, eLdFifoLReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_imm_eLdFifoHReg, [OperandRegClass<1, eLdFifoHReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_imm_mFifoExtra, [OperandRegClass<1, mFifoExtra>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_pstm_nrm_imm_mStFifoh, [OperandRegClass<1, mStFifoh>]>], mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_fifohl_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mFifoHLReg:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_fifohl__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<3> src;

let st = {ptr, imm, 0b11, src, 0b01010110};
}

let Itinerary = II_VST_dmx_sts_fifohl_spill, ItineraryRegPairs = [
ItinRegClassPair<II_VST_dmx_sts_fifohl_spill_mStFifol, [OperandRegClass<0, mStFifol>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_spill_eLdFifoLReg, [OperandRegClass<0, eLdFifoLReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_spill_eLdFifoHReg, [OperandRegClass<0, eLdFifoHReg>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_spill_mFifoExtra, [OperandRegClass<0, mFifoExtra>]>,
ItinRegClassPair<II_VST_dmx_sts_fifohl_spill_mStFifoh, [OperandRegClass<0, mStFifoh>]>], mayStore = true, Uses = [sp] in
def VST_dmx_sts_fifohl_spill : AIE2P_inst_st_instr32 <(outs), (ins OP_mFifoHLReg:$src, c16n_step64:$imm), "vst", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmx_sts_fifohl_spill__agus_dmx__nrm_spill__agus_dmx__spill__mSPs__m_c16n_step64
bits<9> imm;
bits<3> src;

let st = {imm, src, 0b11010110};
}

let Itinerary = II_VST_dmx_sts_x_idx, mayStore = true in
def VST_dmx_sts_x_idx : AIE2P_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, eDJ:$dj), "vst", " $src, [$ptr, $dj]">{
// id: me__instr128_or__st__dmx_sts_x__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx__mPs__mDJs
bits<3> ptr;
bits<3> dj;
bits<4> src;

let st = {ptr, dj, 0b000, src, 0b0100110};
}

let Itinerary = II_VST_dmx_sts_x_idx_imm, mayStore = true in
def VST_dmx_sts_x_idx_imm : AIE2P_inst_st_instr32 <(outs), (ins OP_mXs:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr, #$imm]">{
// id: me__instr128_or__st__dmx_sts_x__agus_dmx__nrm__agus_dmx__normal__agus_dmx__idx_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b01, src, 0b0100110};
}

let Itinerary = II_VST_dmx_sts_x_pstm_nrm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_x_pstm_nrm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, eM:$mod), "vst", " $src, [$ptr], $mod">{
// id: me__instr128_or__st__dmx_sts_x__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm__mPs__mMs
bits<3> ptr;
bits<3> mod;
bits<4> src;

let st = {ptr, mod, 0b010, src, 0b0100110};
}

let Itinerary = II_VST_dmx_sts_x_pstm_nrm_imm, mayStore = true, Constraints = "$ptr_out = $ptr" in
def VST_dmx_sts_x_pstm_nrm_imm : AIE2P_inst_st_instr32 <(outs eP:$ptr_out), (ins OP_mXs:$src, eP:$ptr, c10s_step64:$imm), "vst", " $src, [$ptr], #$imm">{
// id: me__instr128_or__st__dmx_sts_x__agus_dmx__nrm__agus_dmx__normal__agus_dmx__pstm_nrm_imm__mPs__m_c10s_step64
bits<3> ptr;
bits<4> imm;
bits<4> src;

let st = {ptr, imm, 0b11, src, 0b0100110};
}

let Itinerary = II_VST_dmx_sts_x_spill, mayStore = true, Uses = [sp] in
def VST_dmx_sts_x_spill : AIE2P_inst_st_instr32 <(outs), (ins OP_mXs:$src, c16n_step64:$imm), "vst", " $src, [sp, #$imm]">{
// id: me__instr128_or__st__dmx_sts_x_spill__agus_dmx__nrm_spill__agus_dmx__spill__mSPs__m_c16n_step64
bits<9> imm;
bits<4> src;

let st = {imm, src, 0b1100110};
}

let Itinerary = II_VSUB_16 in
def VSUB_16 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub.16", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vsub__vec_size__vec_size32__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b1, 0b01, 0b1101011};
}

let Itinerary = II_VSUB_32 in
def VSUB_32 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub.32", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vsub__vec_size__vec_size16__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b1, 0b10, 0b1101011};
}

let Itinerary = II_VSUB_8 in
def VSUB_8 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub.8", " $d, $s1, $s2">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add__vsub__vec_size__vec_size64__vec_add_mX__vec_add_nX
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b1, 0b00, 0b1101011};
}

let Itinerary = II_VSUB_GE_16_vaddSign0, Uses = [vaddSign0] in
def VSUB_GE_16_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b010, 0b0, 0b0010};
}

let Itinerary = II_VSUB_GE_16_vaddSign1, Uses = [vaddSign1] in
def VSUB_GE_16_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b010, 0b1, 0b0010};
}

let Itinerary = II_VSUB_GE_32_vaddSign0, Uses = [vaddSign0] in
def VSUB_GE_32_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b010, 0b0, 0b0010};
}

let Itinerary = II_VSUB_GE_32_vaddSign1, Uses = [vaddSign1] in
def VSUB_GE_32_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b010, 0b1, 0b0010};
}

let Itinerary = II_VSUB_GE_8_vaddSign0, Uses = [vaddSign0] in
def VSUB_GE_8_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b010, 0b0, 0b0010};
}

let Itinerary = II_VSUB_GE_8_vaddSign1, Uses = [vaddSign1] in
def VSUB_GE_8_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_ge.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_ge__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b010, 0b1, 0b0010};
}

let Itinerary = II_VSUB_LT_16_vaddSign0, Uses = [vaddSign0] in
def VSUB_LT_16_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.16", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b001, 0b0, 0b0010};
}

let Itinerary = II_VSUB_LT_16_vaddSign1, Uses = [vaddSign1] in
def VSUB_LT_16_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.16", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single32__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b01, 0b001, 0b1, 0b0010};
}

let Itinerary = II_VSUB_LT_32_vaddSign0, Uses = [vaddSign0] in
def VSUB_LT_32_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.32", " $d, r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b001, 0b0, 0b0010};
}

let Itinerary = II_VSUB_LT_32_vaddSign1, Uses = [vaddSign1] in
def VSUB_LT_32_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mR16_vcompare:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.32", " $d, r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single16__mR16_vcompare__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b00, 0b001, 0b1, 0b0010};
}

let Itinerary = II_VSUB_LT_8_vaddSign0, Uses = [vaddSign0] in
def VSUB_LT_8_vaddSign0 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.8", " $d, r17:r16, $s1, $s2, vaddsign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign0
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b001, 0b0, 0b0010};
}

let Itinerary = II_VSUB_LT_8_vaddSign1, Uses = [vaddSign1] in
def VSUB_LT_8_vaddSign1 : AIE2P_inst_mv_instr32 <(outs OP_mXm:$d, mL8m:$cmp), (ins OP_mXm:$s1, OP_mXn:$s2), "vsub_lt.8", " $d, r17:r16, $s1, $s2, vaddsign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_sub_min_max__vsub_lt__vec_cmp_out_single__vec_cmp_out_single64__mL8m__vec_add_mX__vec_add_nX__vaddSign1
bits<4> d;
bits<4> s1;
bits<4> s2;

let mv = {d, s1, s2, 0b10, 0b001, 0b1, 0b0010};
}

let Itinerary = II_VSUB_vmac_cm2_add_reg in
def VSUB_vmac_cm2_add_reg : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eR:$acc), "vsub", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> acc2;

let vec = {acc, dst, acc1, acc2, dontcare{4-1}, 0b1111, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VSUB_vmac_cm2_add_scd, hasSideEffects = true, Uses = [crSCDEn] in
def VSUB_vmac_cm2_add_scd : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eR:$acc), "vsub", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, 0b110, dontcare{4-1}, 0b1111, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VSUB_vmac_cm2_add_scd_incr, hasSideEffects = true, Uses = [crSCDEn], Constraints = "$r_out = $r" in
def VSUB_vmac_cm2_add_scd_incr : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eR:$acc), "vsub", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc__mDMa__vmac_cm1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vacc_cm_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, 0b111, dontcare{4-1}, 0b1111, 0b1, 0b1, dontcare{1}, 0b0};
}

let Itinerary = II_VSUB_f_vmac_cm2_add_reg, Defs = [srFPFlags], Uses = [crFPMask] in
def VSUB_f_vmac_cm2_add_reg : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, eDM:$acc2, eR:$acc), "vsub.f", "$dst, $acc1, $acc2, $acc">{
// id: me__instr128_or__vec__vacc_fp__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_reg__mDMm__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;
bits<3> acc2;

let vec = {acc, dst, acc1, acc2, dontcare{4-1}, 0b1111, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VSUB_f_vmac_cm2_add_scd, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn] in
def VSUB_f_vmac_cm2_add_scd : AIE2P_inst_vec_instr32 <(outs eDM:$dst), (ins eDM:$acc1, mR31_scd:$c, eR:$acc), "vsub.f", "$dst, $acc1, scd[r31], $acc">{
// id: me__instr128_or__vec__vacc_fp__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd__vmac_scd_dm_reg__vmac_scd_dm_core__mSCD_E4__mSCD_r__mR31_scd__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, 0b110, dontcare{4-1}, 0b1111, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VSUB_f_vmac_cm2_add_scd_incr, hasSideEffects = true, Defs = [srFPFlags], Uses = [crFPMask, crSCDEn], Constraints = "$r_out = $r" in
def VSUB_f_vmac_cm2_add_scd_incr : AIE2P_inst_vec_instr32 <(outs eDM:$dst, mR31_scd:$r_out), (ins eDM:$acc1, mR31_scd:$r, eR:$acc), "vsub.f", "$dst, $acc1, scd[r31++], $acc">{
// id: me__instr128_or__vec__vacc_fp__mDMa__vmac_bf1_add__mDMa__vmac_cm2_add__vmac_cm2_add_scd_incr__vmac_scd_dm_dyn__vmac_scd_dm_core__mSCD_E4__mSCD_r_incr__mR31_scd__vacc_bf_core__mRv
bits<5> acc;
bits<3> dst;
bits<3> acc1;

let vec = {acc, dst, acc1, 0b111, dontcare{4-1}, 0b1111, 0b1, 0b1, dontcare{1}, 0b1};
}

let Itinerary = II_VTANH in
// START MANUAL ADDITION  See ExtraItineraries.td.
let ItineraryRegPairs = [
ItinRegClassPair<II_VTANH_eWHNoBypass, [OperandRegClass<0, eWH>]>,
ItinRegClassPair<II_VTANH, [OperandRegClass<0, eWL>]>] in
// END MANUAL ADDITION
def VTANH : AIE2P_inst_mv_instr32 <(outs OP_mWm:$dst), (ins OP_mBMm:$src), "vtanh", " $dst, $src">{
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf__vtanh
bits<5> dst;
bits<5> src;

let mv = {dst, src, 0b1, 0b11110010111};
}

let Itinerary = II_VUNPACK_mv_unpack_w_unpackSign0, Uses = [crUnpackSize, unpackSign0] in
def VUNPACK_mv_unpack_w_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins OP_mWs:$src), "vunpack", "$dst, $src, unpacksign0">{
// id: me__instr128_or__ldb__ldb_or1__mv_unpack_w__unpackSign0
bits<5> src;
bits<4> dst;

let ldb = {src, dontcare{1}, 0b01, 0b0, dst, 0b0, 0b100};
}

let Itinerary = II_VUNPACK_mv_unpack_w_unpackSign1, Uses = [crUnpackSize, unpackSign1] in
def VUNPACK_mv_unpack_w_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs OP_mXb:$dst), (ins OP_mWs:$src), "vunpack", "$dst, $src, unpacksign1">{
// id: me__instr128_or__ldb__ldb_or1__mv_unpack_w__unpackSign1
bits<5> src;
bits<4> dst;

let ldb = {src, dontcare{1}, 0b01, 0b0, dst, 0b1, 0b100};
}

let Itinerary = II_VUNPACK_mv_unpack_x_unpackSign0, Uses = [crUnpackSize, unpackSign0] in
def VUNPACK_mv_unpack_x_unpackSign0 : AIE2P_inst_ldb_instr32 <(outs eY:$dst), (ins OP_mXs:$src), "vunpack", "$dst, $src, unpacksign0">{
// id: me__instr128_or__ldb__ldb_or1__mv_unpack_x__mYb__unpackSign0
bits<4> src;
bits<3> dst;

let ldb = {src, dontcare{1}, dontcare{1}, 0b11, 0b0, dst, dontcare{1}, 0b0, 0b100};
}

let Itinerary = II_VUNPACK_mv_unpack_x_unpackSign1, Uses = [crUnpackSize, unpackSign1] in
def VUNPACK_mv_unpack_x_unpackSign1 : AIE2P_inst_ldb_instr32 <(outs eY:$dst), (ins OP_mXs:$src), "vunpack", "$dst, $src, unpacksign1">{
// id: me__instr128_or__ldb__ldb_or1__mv_unpack_x__mYb__unpackSign1
bits<4> src;
bits<3> dst;

let ldb = {src, dontcare{1}, dontcare{1}, 0b11, 0b0, dst, dontcare{1}, 0b1, 0b100};
}

let Itinerary = II_VUPS_2x_mv_ups_w2b_upsSign0, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VUPS_2x_mv_ups_w2b_upsSign0 : AIE2P_inst_mv_instr32 <(outs OP_mBMm:$dst), (ins OP_mWm:$src, eS:$su), "vups.2x", "$dst, $src, $su, upssign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2b__ups_mov__w2b__ups_mov__base__mSm__upsSign0
bits<5> dst;
bits<5> src;
bits<2> su;

let mv = {dst, dontcare{1}, src, su, 0b0, 0b10000111};
}

let Itinerary = II_VUPS_2x_mv_ups_w2b_upsSign1, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VUPS_2x_mv_ups_w2b_upsSign1 : AIE2P_inst_mv_instr32 <(outs OP_mBMm:$dst), (ins OP_mWm:$src, eS:$su), "vups.2x", "$dst, $src, $su, upssign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2b__ups_mov__w2b__ups_mov__base__mSm__upsSign1
bits<5> dst;
bits<5> src;
bits<2> su;

let mv = {dst, dontcare{1}, src, su, 0b1, 0b10000111};
}

let Itinerary = II_VUPS_2x_mv_ups_x2c_upsSign0, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VUPS_2x_mv_ups_x2c_upsSign0 : AIE2P_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mXm:$src, eS:$su), "vups.2x", "$dst, $src, $su, upssign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2c__ups_mov__x2c__ups_mov__base__mSm__upsSign0
bits<4> dst;
bits<4> src;
bits<2> su;

let mv = {dst, dontcare{2-1}, src, dontcare{1}, su, 0b0, 0b11000111};
}

let Itinerary = II_VUPS_2x_mv_ups_x2c_upsSign1, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VUPS_2x_mv_ups_x2c_upsSign1 : AIE2P_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mXm:$src, eS:$su), "vups.2x", "$dst, $src, $su, upssign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2c__ups_mov__x2c__ups_mov__base__mSm__upsSign1
bits<4> dst;
bits<4> src;
bits<2> su;

let mv = {dst, dontcare{2-1}, src, dontcare{1}, su, 0b1, 0b11000111};
}

let Itinerary = II_VUPS_4x_mv_ups_w2c_upsSign0, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VUPS_4x_mv_ups_w2c_upsSign0 : AIE2P_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mWm:$src, eS:$su), "vups.4x", "$dst, $src, $su, upssign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2c__ups_mov__w2c__ups_mov__base__mSm__upsSign0
bits<4> dst;
bits<5> src;
bits<2> su;

let mv = {dst, dontcare{2-1}, src, su, 0b0, 0b01000111};
}

let Itinerary = II_VUPS_4x_mv_ups_w2c_upsSign1, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VUPS_4x_mv_ups_w2c_upsSign1 : AIE2P_inst_mv_instr32 <(outs OP_mCMm:$dst), (ins OP_mWm:$src, eS:$su), "vups.4x", "$dst, $src, $su, upssign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_w2c__ups_mov__w2c__ups_mov__base__mSm__upsSign1
bits<4> dst;
bits<5> src;
bits<2> su;

let mv = {dst, dontcare{2-1}, src, su, 0b1, 0b01000111};
}

let Itinerary = II_VUPS_4x_mv_ups_x2d_upsSign0, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign0] in
def VUPS_4x_mv_ups_x2d_upsSign0 : AIE2P_inst_mv_instr32 <(outs eDM:$dst), (ins OP_mXm:$src, eS:$su), "vups.4x", "$dst, $src, $su, upssign0">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2d__ups_mov__x2d__mDMm__ups_mov__base__mSm__upsSign0
bits<3> dst;
bits<4> src;
bits<2> su;

let mv = {dst, dontcare{3-1}, src, dontcare{1}, su, 0b0, 0b00100111};
}

let Itinerary = II_VUPS_4x_mv_ups_x2d_upsSign1, Defs = [srUPS_of], Uses = [crSat, crUPSMode, upsSign1] in
def VUPS_4x_mv_ups_x2d_upsSign1 : AIE2P_inst_mv_instr32 <(outs eDM:$dst), (ins OP_mXm:$src, eS:$su), "vups.4x", "$dst, $src, $su, upssign1">{
// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_x2d__ups_mov__x2d__mDMm__ups_mov__base__mSm__upsSign1
bits<3> dst;
bits<4> src;
bits<2> su;

let mv = {dst, dontcare{3-1}, src, dontcare{1}, su, 0b1, 0b00100111};
}

let Itinerary = II_XOR in
def XOR : AIE2P_inst_alu_instr32 <(outs eR:$d0), (ins eR:$s0, eR:$s1), "xor", " $d0, $s0, $s1">{
// id: me__instr128_or__alu_mv__alumv_or__alu__alu_r_rr__xor__mRx__mRy
bits<5> s0;
bits<5> d0;
bits<5> s1;

let alu = {s0, d0, s1, 0b0110, 0b1};
}

}
