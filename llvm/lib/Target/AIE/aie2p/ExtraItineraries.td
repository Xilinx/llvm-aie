//===- AIE2PScheduleWHNoBypass.td - AIE2P eWH no-bypass itineraries -*-tablegen-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Hand-crafted itinerary data for instructions whose Move-slot bypass does
// not apply when the relevant operand is in the eWH (upper-W) register class.
//
// Each entry uses a distinct II_*_eWHNoBypass itinerary class (declared in
// AIE2PFixItinDecls.td) and sets NoBypass on the operand that would otherwise
// carry MV_Bypass.  These entries are selected via ItinRegClassPair in
// AIE2PGenInstrInfo.td when the operand is allocated to an eWH register.
//
// The base generated entries (with MV_Bypass) remain in AIE2PGenSchedule.td
// and act as the default, covering the eWL case where the bypass is valid.
//
// Rules applied:
//   - /*src*/ input of VCONV_fp32_bf16_mv_ups_wbf: NoBypass when eWH.
//     (the Move-slot bypass for W-register inputs only works for WL)
//   - /*dst*/ output of VEXP2: NoBypass when eWH.
//   - /*dst*/ output of VTANH: NoBypass when eWH.

// id: me__instr128_or__alu_mv__alumv_or__mv__mv_ups_wbf
InstrItinData<II_VCONV_fp32_bf16_mv_ups_wbf_eWHNoBypass, [EmptyCycles<1>, SimpleCycle<DM_WM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf__vexp2_subn
InstrItinData<II_VEXP2_eWHNoBypass, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,

// id: me__instr128_or__alu_mv__alumv_or__mv__vec_nlf__vtanh
InstrItinData<II_VTANH_eWHNoBypass, [SimpleCycle<DM_RM_L0_PORT>], [/*dst*/2, /*src*/1], [/*dst*/NoBypass, /*src*/NoBypass]>,
