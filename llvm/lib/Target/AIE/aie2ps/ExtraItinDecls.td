// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Hand-crafted bypass and itinerary-class declarations for eL register
// specialization in AIE2PS.
//
// MV_L_Bypass: a forwarding class distinct from MV_Bypass, assigned to
//   operands that are in the eL (64-bit scalar) register class.  This lets
//   the forwarding-class equality check in getNumBypassedCycles correctly
//   differentiate eL↔eL bypasses from eR↔eR bypasses, without needing a
//   separate register-class membership test.
//
// II_*_mLBypass itinerary classes: one per instruction that has a fixed eL
//   operand (comparison-output or selection-mask input).  They carry
//   MV_L_Bypass on the relevant operand slot and will be wired to their
//   parent instructions via ItinRegClassPair entries.

def MV_L_Bypass : Bypass;

def II_VADDSUB_8_mLBypass        : InstrItinClass;
def II_VEQZ_8_mLBypass           : InstrItinClass;
def II_VGE_8_vaddSign0_mLBypass  : InstrItinClass;
def II_VGE_8_vaddSign1_mLBypass  : InstrItinClass;
def II_VGE_bf8_mLBypass          : InstrItinClass;
def II_VGE_fp8_mLBypass          : InstrItinClass;
def II_VLT_8_vaddSign0_mLBypass  : InstrItinClass;
def II_VLT_8_vaddSign1_mLBypass  : InstrItinClass;
def II_VLT_bf8_mLBypass          : InstrItinClass;
def II_VLT_fp8_mLBypass          : InstrItinClass;
def II_VSEL_8_mLBypass           : InstrItinClass;
