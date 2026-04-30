// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Hand-crafted itinerary data for instructions with eL register operands.
//
// Each entry here uses a distinct II_*_mLBypass itinerary class (declared in
// AIE2PSFixItinDecls.td) so that the forwarding-class equality check in
// getNumBypassedCycles correctly recognises eL-to-eL forwarding paths.
//
// The base generated entries for these instructions (using MV_Bypass for all
// operands) still exist in AIE2PSGenSchedule.td. The _mLBypass variants here
// are selected via ItinRegClassPair in AIE2PSInstrInfo.cpp when the relevant
// operand is allocated to an eL (64-bit scalar) register.
//
// Rules applied:
//   - /*cmp*/ output of VEQZ/VGE/VLT 8-bit variants: MV_L_Bypass
//     (these instructions produce an eL comparison mask)
//   - /*sel*/ input of VADDSUB_8 and VSEL_8: MV_L_Bypass
//     (the selection mask is an eL register consumed via bypass)

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vaddsub__vec_mask_in__vec_mask_in64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VADDSUB_8_mLBypass, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*sel*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*sel*/MV_L_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_eqz__vec_cmp_out__vec_cmp_out64__mLm__vec_add_nX
InstrItinData<II_VEQZ_8_mLBypass, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_L_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VGE_8_vaddSign0_mLBypass, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*cmp*/MV_L_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vge__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VGE_8_vaddSign1_mLBypass, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*cmp*/MV_L_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vge_bf8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VGE_bf8_mLBypass, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_L_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vge_fp8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VGE_fp8_mLBypass, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_L_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign0
InstrItinData<II_VLT_8_vaddSign0_mLBypass, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign0*/1], [/*cmp*/MV_L_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_compare__vlt__vec_cmp_out__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX__vaddSign1
InstrItinData<II_VLT_8_vaddSign1_mLBypass, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1, /*vaddSign1*/1], [/*cmp*/MV_L_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vlt_bf8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VLT_bf8_mLBypass, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_L_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_f8_compare__vlt_fp8__vec_cmp_out64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VLT_fp8_mLBypass, [SimpleCycle<CRBF8CONF_RV>, SimpleCycle<L_WM_L0_PORT>], [/*cmp*/2, /*s1*/1, /*s2*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*cmp*/MV_L_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass]>,
// id: me__instr128_or__alu_mv__alumv_or__mv__vec_add_select__vsel__vec_mask_in__vec_mask_in64__mLm__vec_add_mX__vec_add_nX
InstrItinData<II_VSEL_8_mLBypass, [SimpleCycle<CRBF8CONF_RV>], [/*d*/2, /*s1*/1, /*s2*/1, /*sel*/1, /*crBF8conf*/1, /*crFP8conf*/1], [/*d*/MV_Bypass, /*s1*/MV_Bypass, /*s2*/MV_Bypass, /*sel*/MV_L_Bypass]>,
