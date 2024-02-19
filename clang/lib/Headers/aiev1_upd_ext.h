//===- aiev1_upd_ext.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV1_UPD_EXT_H__
#define __AIEV1_UPD_EXT_H__

// handle compile time assert in AIE
#define DIAGNOSE_UPD_IDX                                                       \
  __attribute__((                                                              \
      diagnose_if(idx < 0 || idx > 1, "index out of range [0,1]", "error")))

INTRINSIC(v8int32) upd_v(v8int32 sbuff, int idx, v4int32 val) DIAGNOSE_UPD_IDX {
  v8int32 ret;
  if (idx == 0) {
    ret = __builtin_aie_upd_v_v8i32_lo(sbuff, val);
  } else {
    ret = __builtin_aie_upd_v_v8i32_hi(sbuff, val);
  }
  return ret;
}

INTRINSIC(v16int32)
upd_w(v16int32 sbuff, int idx, v8int32 val) DIAGNOSE_UPD_IDX {
  v16int32 ret;
  if (idx == 0) {
    ret = __builtin_aie_upd_w_v16i32_lo(sbuff, val);
  } else {
    ret = __builtin_aie_upd_w_v16i32_hi(sbuff, val);
  }
  return ret;
}

INTRINSIC(v4int32) ext_v(v8int32 sbuff, int idx) DIAGNOSE_UPD_IDX {
  v4int32 ret;
  if (idx == 0) {
    ret = __builtin_aie_ext_v_v8i32_lo(sbuff);
  } else {
    ret = __builtin_aie_ext_v_v8i32_hi(sbuff);
  }
  return ret;
}

INTRINSIC(v8int32) ext_w(v16int32 sbuff, int idx) DIAGNOSE_UPD_IDX {

  v8int32 ret;
  if (idx == 0) {
    ret = __builtin_aie_ext_w_v16i32_lo(sbuff);
  } else {
    ret = __builtin_aie_ext_w_v16i32_hi(sbuff);
  }
  return ret;
}

INTRINSIC(v16int16) ext_w(v32int16 sbuff, int idx) DIAGNOSE_UPD_IDX {
  v16int16 ret;
  if (idx == 0) {
    ret = __builtin_aie_ext_w_v32i16_lo(sbuff);
  } else {
    ret = __builtin_aie_ext_w_v32i16_hi(sbuff);
  }
  return ret;
}

INTRINSIC(v32int16) concat(v16int16 abuff, v16int16 bbuff) {
  return __builtin_aie_concat_v16i16(abuff, bbuff);
}

INTRINSIC(v64int16) concat(v32int16 abuff, v32int16 bbuff) {
  return __builtin_aie_concat_v32i16(abuff, bbuff);
}

#endif /*__AIEV1_UPD_EXT_H__*/
