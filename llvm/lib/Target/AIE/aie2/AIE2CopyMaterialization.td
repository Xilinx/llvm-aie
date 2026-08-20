//===-- AIE2CopyMaterialization.td - AIE2 copy tables -*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

include "llvm/Target/AIECopyMaterialization.td"

defm COPY_SCL_eR : DirectCopy<eR, eR, MOV_SCL_pseudo, 20>;
defm COPY_SCL : DirectCopy<mMvSclDst, mMvSclSrc, MOV_mv_scl>;

def COPY_eD : CopyRecipe<eD, eD, [CopyAllSubRegs]>;
def COPY_eDS : CopyRecipe<eDS, eDS, [CopyAllSubRegs]>;

def COPY_eL : CopyRecipe<eL, eL, [
  SameSubRegCopy<sub_l_even, MOV_mv_scl>,
  SameSubRegCopy<sub_l_odd, MOV_mv_scl>
]>;

defm COPY_VEC128_VEC128 : DirectCopy<VEC128, VEC128, VMOV_mv_w>;
defm COPY_VEC128_VEC256 : DirectCopy<VEC256, VEC128, VMOV_mv_w>;
defm COPY_VEC128_ACC256 : DirectCopy<ACC256, VEC128, VMOV_mv_w>;
defm COPY_VEC256_VEC128 : DirectCopy<VEC128, VEC256, VMOV_mv_w>;
defm COPY_VEC256_VEC256 : DirectCopy<VEC256, VEC256, VMOV_mv_w>;
defm COPY_VEC256_ACC256 : DirectCopy<ACC256, VEC256, VMOV_mv_w>;
defm COPY_ACC256_VEC128 : DirectCopy<VEC128, ACC256, VMOV_mv_w>;
defm COPY_ACC256_VEC256 : DirectCopy<VEC256, ACC256, VMOV_mv_w>;
defm COPY_ACC256_ACC256 : DirectCopy<ACC256, ACC256, VMOV_mv_w>;
defm COPY_VEC512_VEC512 : DirectCopy<VEC512, VEC512, VMOV_mv_x>;
defm COPY_VEC512_ACC512 : DirectCopy<ACC512, VEC512, VMOV_mv_x>;
defm COPY_ACC512_VEC512 : DirectCopy<VEC512, ACC512, VMOV_mv_x>;
defm COPY_ACC512_ACC512 : DirectCopy<ACC512, ACC512, VMOV_mv_x>;

def COPY_VEC1024 : CopyRecipe<VEC1024, VEC1024, [
  SameSubRegCopy<sub_512_lo, VMOV_mv_x>,
  SameSubRegCopy<sub_512_hi, VMOV_mv_x>
]>;
def COPY_ACC1024 : CopyRecipe<ACC1024, ACC1024,
                              [FullRegCopy<VMOV_mv_cm>]>;
def COPY_VEC1024_ACC1024 : CopyRecipe<ACC1024, VEC1024, [
  SameSubRegCopy<sub_512_lo, VMOV_mv_x>,
  SameSubRegCopy<sub_512_hi, VMOV_mv_x>
]>;
def COPY_ACC1024_VEC1024 : CopyRecipe<VEC1024, ACC1024, [
  SameSubRegCopy<sub_512_lo, VMOV_mv_x>,
  SameSubRegCopy<sub_512_hi, VMOV_mv_x>
]>;
def COPY_SPARSEVEC640 : CopyRecipe<SPARSEVEC640, SPARSEVEC640, [
  SameSubRegCopy<sub_sparse_x, VMOV_mv_x>,
  SameSubRegCopy<sub_sparse_q, VMOV_mv_w>
]>;
