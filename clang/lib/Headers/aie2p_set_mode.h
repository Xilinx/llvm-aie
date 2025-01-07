//===-------------------- AIEngine AIE2p intrinsics ------------------------===
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef AIE2P_SET_MODE_H
#define AIE2P_SET_MODE_H

//* Automatically generated file, do not edit! *
//
typedef unsigned int crsat_t;

typedef unsigned int crrnd_t;

INTRINSIC(unsigned int) get_satmode() {
  return __builtin_aie2p_get_ctrl_reg(crSat);
}
INTRINSIC(void) set_satmode(unsigned int val) {
  __builtin_aie2p_set_ctrl_reg(crSat, val);
}
INTRINSIC(void) set_sat() { __builtin_aie2p_set_ctrl_reg(crSat, 1); }
INTRINSIC(void) set_symsat() { __builtin_aie2p_set_ctrl_reg(crSat, 3); }
INTRINSIC(void) clr_sat() { __builtin_aie2p_set_ctrl_reg(crSat, 0); }
INTRINSIC(unsigned int) get_sat() {
  return __builtin_aie2p_get_ctrl_reg(crSat);
}
INTRINSIC(unsigned int) get_symsat() {
  return __builtin_aie2p_get_ctrl_reg(crSat);
}

INTRINSIC(unsigned int) get_rnd() {
  return __builtin_aie2p_get_ctrl_reg(crRnd);
}
INTRINSIC(void) set_rnd(unsigned int val) {
  __builtin_aie2p_set_ctrl_reg(crRnd, val);
}

INTRINSIC(unsigned int) get_fpmulmac_mask() {
  return __builtin_aie2p_get_ctrl_reg(crFPMask);
}
INTRINSIC(void) set_fpmulmac_mask(unsigned int val) {
  __builtin_aie2p_set_ctrl_reg(crFPMask, val);
}

INTRINSIC(unsigned int) get_fp2int_mask() {
  return __builtin_aie2p_get_ctrl_reg(crF2IMask);
}
INTRINSIC(void) set_fp2int_mask(unsigned int val) {
  __builtin_aie2p_set_ctrl_reg(crF2IMask, val);
}

INTRINSIC(unsigned int) get_fp2bf_mask() {
  return __builtin_aie2p_get_ctrl_reg(crF2FMask);
}
INTRINSIC(void) set_fp2bf_mask(unsigned int val) {
  __builtin_aie2p_set_ctrl_reg(crF2FMask, val);
}

INTRINSIC(unsigned int) get_fp2bfp_mask() {
  return __builtin_aie2p_get_ctrl_reg(crF2BMask);
}
INTRINSIC(void) set_fp2bfp_mask(unsigned int val) {
  __builtin_aie2p_set_ctrl_reg(crF2BMask, val);
}
INTRINSIC(unsigned int) get_srs_of() {
  return __builtin_aie2p_get_status_reg(srSRS_of);
}
INTRINSIC(void) set_srs_of() { __builtin_aie2p_set_status_reg(srSRS_of, 1); }
INTRINSIC(void) set_srs_of(unsigned int val) {
  __builtin_aie2p_set_status_reg(srSRS_of, val);
}
INTRINSIC(void) clr_srs_of() { __builtin_aie2p_set_status_reg(srSRS_of, 0); }

INTRINSIC(unsigned int) get_ups_of() {
  return __builtin_aie2p_get_status_reg(srUPS_of);
}
INTRINSIC(void) set_ups_of() { __builtin_aie2p_set_status_reg(srUPS_of, 1); }
INTRINSIC(void) set_ups_of(unsigned int val) {
  __builtin_aie2p_set_status_reg(srUPS_of, val);
}
INTRINSIC(void) clr_ups_of() { __builtin_aie2p_set_status_reg(srUPS_of, 0); }

INTRINSIC(unsigned int) get_fpmulmac_flags() {
  return __builtin_aie2p_get_status_reg(srFPFlags);
}
INTRINSIC(void) set_fpmulmac_flags(unsigned int val) {
  __builtin_aie2p_set_status_reg(srFPFlags, val);
}
INTRINSIC(unsigned int) get_fp2int_flags() {
  return __builtin_aie2p_get_status_reg(srF2IFlags);
}
INTRINSIC(void) set_fp2int_flags(unsigned int val) {
  __builtin_aie2p_set_status_reg(srF2IFlags, val);
}
INTRINSIC(unsigned int) get_fpf2f_flags() {
  return __builtin_aie2p_get_status_reg(srF2FFlags);
}
INTRINSIC(void) set_fpf2f_flags(unsigned int val) {
  __builtin_aie2p_set_status_reg(srF2FFlags, val);
}
INTRINSIC(unsigned int) get_fpf2bfp_flags() {
  return __builtin_aie2p_get_status_reg(srF2BFlags);
}
INTRINSIC(void) set_fpf2bfp_flags(unsigned int val) {
  __builtin_aie2p_set_status_reg(srF2BFlags, val);
}

INTRINSIC(unsigned int) get_sparse_of() {
  return __builtin_aie2p_get_status_reg(srSparse_of);
}
INTRINSIC(void) set_sparse_of() {
  __builtin_aie2p_set_status_reg(srSparse_of, 1);
}
INTRINSIC(void) set_sparse_of(unsigned int val) {
  __builtin_aie2p_set_status_reg(srSparse_of, val);
}
INTRINSIC(void) clr_sparse_of() {
  __builtin_aie2p_set_status_reg(srSparse_of, 0);
}

INTRINSIC(unsigned int) get_fifo_st_of() {
  return __builtin_aie2p_get_status_reg(srFifo_of);
}
INTRINSIC(void) set_fifo_st_of() {
  __builtin_aie2p_set_status_reg(srFifo_of, 1);
}
INTRINSIC(void) set_fifo_st_of(unsigned int val) {
  __builtin_aie2p_set_status_reg(srFifo_of, val);
}
INTRINSIC(void) clr_fifo_st_of() {
  __builtin_aie2p_set_status_reg(srFifo_of, 0);
}
INTRINSIC(unsigned int) get_fifo_ld_uf() {
  return __builtin_aie2p_get_status_reg(srFifo_uf);
}
INTRINSIC(void) set_fifo_ld_uf() {
  __builtin_aie2p_set_status_reg(srFifo_uf, 1);
}
INTRINSIC(void) set_fifo_ld_uf(unsigned int val) {
  __builtin_aie2p_set_status_reg(srFifo_uf, val);
}
INTRINSIC(void) clr_fifo_ld_uf() {
  __builtin_aie2p_set_status_reg(srFifo_uf, 0);
}

#endif // AIE2P_SET_MODE_H
