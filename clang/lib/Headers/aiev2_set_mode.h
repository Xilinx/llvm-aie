//===- aiev2_set_mode.h -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_SET_MODE_H__
#define __AIEV2_SET_MODE_H__

INTRINSIC(void)
set_satmode(unsigned int val) { __builtin_aiev2_set_ctrl_reg(crSat, val); }

INTRINSIC(void)
set_sat() { __builtin_aiev2_set_ctrl_reg(crSat, 1); }

INTRINSIC(void)
set_symsat() { __builtin_aiev2_set_ctrl_reg(crSat, 3); }

INTRINSIC(void)
clr_sat() { __builtin_aiev2_set_ctrl_reg(crSat, 0); }

INTRINSIC(void)
set_rnd(unsigned int val) { __builtin_aiev2_set_ctrl_reg(crRnd, val); }

INTRINSIC(void)
set_fpmulmac_mask(unsigned int val) {
  __builtin_aiev2_set_ctrl_reg(crFPMask, val);
}

INTRINSIC(void)
set_fp2int_mask(unsigned int val) {
  __builtin_aiev2_set_ctrl_reg(crF2IMask, val);
}

INTRINSIC(void)
set_fp2bf_mask(unsigned int val) {
  __builtin_aiev2_set_ctrl_reg(crF2FMask, val);
}

INTRINSIC(unsigned int)
get_satmode() { return __builtin_aiev2_get_ctrl_reg(crSat); }

INTRINSIC(unsigned int)
get_sat() { return __builtin_aiev2_get_ctrl_reg(crSat); }

INTRINSIC(unsigned int)
get_rnd() { return __builtin_aiev2_get_ctrl_reg(crRnd); }

INTRINSIC(unsigned int) get_fpmulmac_mask() {
  return __builtin_aiev2_get_ctrl_reg(crFPMask);
}

INTRINSIC(unsigned int)
get_fp2int_mask() { return __builtin_aiev2_get_ctrl_reg(crF2IMask); }

INTRINSIC(unsigned int)
get_fp2bf_mask() { return __builtin_aiev2_get_ctrl_reg(crF2FMask); }

#endif // __AIEV2_SET_MODE_H__
