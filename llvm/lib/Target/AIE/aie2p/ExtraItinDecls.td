//===- AIE2PFixItinDecls.td - AIE2P hand-crafted itin declarations-*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Hand-crafted itinerary-class declarations for eWH register specialization
// in AIE2P.
//
// II_*_eWHNoBypass itinerary classes: one per instruction whose Move-slot
//   bypass is only valid for eWL (lower-W) operands.  When the operand is
//   allocated to eWH (upper-W), MV_Bypass does not apply, so a NoBypass
//   variant is used instead.  These classes are selected via ItinRegClassPair
//   entries in AIE2PGenInstrInfo.td when the relevant operand is confirmed to
//   be in eWH.
//
// The base generated itinerary entries (with MV_Bypass) remain unchanged and
// serve as the default, handling the eWL and unallocated cases correctly.

def II_VCONV_fp32_bf16_mv_ups_wbf_eWHNoBypass : InstrItinClass;
def II_VEXP2_eWHNoBypass                       : InstrItinClass;
def II_VTANH_eWHNoBypass                       : InstrItinClass;
