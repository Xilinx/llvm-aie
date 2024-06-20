//===- aiev1_ldst.h ---------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV1_LDST_H__
#define __AIEV1_LDST_H__

INTRINSIC(v16int8) pack(v16int16 vec) {
  return __builtin_aie_pack_v16int16(vec);
}

#endif /*__AIEV1_LDST_H__*/
