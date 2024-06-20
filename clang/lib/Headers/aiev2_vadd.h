//===- aiev2_vadd.h ---------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

INTRINSIC(v64uint8) add(v64uint8 a, v64uint8 b) { return a + b; }

INTRINSIC(v64uint8) sub(v64uint8 a, v64uint8 b) { return a - b; }

INTRINSIC(v64uint8) addsub(v64uint8 a, v64uint8 b, unsigned long long as) {
  return __builtin_aiev2_vaddsub8(a, b, (v2int32)as);
}

INTRINSIC(v64uint8) neg_gtz(v64uint8 a, unsigned long long &cmp) {
  return __builtin_aiev2_vneg_gtz8(a, cmp);
}

INTRINSIC(v64uint8) neg(v64uint8 a) {
  unsigned long long cmp;
  return __builtin_aiev2_vneg_gtz8(a, cmp);
}

INTRINSIC(v64uint8) sub_lt(v64uint8 a, v64uint8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vsub_lt8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
sub_lt(v64uint8 a, v64uint8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vsub_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8) sub_ge(v64uint8 a, v64uint8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vsub_ge8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
sub_ge(v64uint8 a, v64uint8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vsub_ge8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8)
maxdiff_lt(v64uint8 a, v64uint8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vmaxdiff_lt8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
maxdiff_lt(v64uint8 a, v64uint8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vmaxdiff_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8) maxdiff(v64uint8 a, v64uint8 b) {
  unsigned long long cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v64uint8) maxdiff(v64uint8 a, v64uint8 b, bool sgn) {
  unsigned long long cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v64uint8) min_ge(v64uint8 a, v64uint8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vmin_ge8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
min_ge(v64uint8 a, v64uint8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vmin_ge8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8) min(v64uint8 a, v64uint8 b) {
  unsigned long long cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v64uint8) min(v64uint8 a, v64uint8 b, bool sgn) {
  unsigned long long cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v64uint8) max_lt(v64uint8 a, v64uint8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vmax_lt8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
max_lt(v64uint8 a, v64uint8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vmax_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8) max(v64uint8 a, v64uint8 b) {
  unsigned long long cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v64uint8) max(v64uint8 a, v64uint8 b, bool sgn) {
  unsigned long long cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v64uint8) band(v64uint8 a, v64uint8 b) { return a & b; }

INTRINSIC(v64uint8) bor(v64uint8 a, v64uint8 b) { return a | b; }

INTRINSIC(v64uint8) bneg_ltz(v64uint8 a, unsigned long long &cmp) {
  v64uint8 r = __builtin_aiev2_vbneg_ltz8(a, cmp);
  cmp = (unsigned long long)(-1);
  return r;
}

INTRINSIC(v64uint8) bneg_ltz(v64uint8 a, bool sgn, unsigned long long &cmp) {
  v64uint8 r = __builtin_aiev2_vbneg_ltz8(a, cmp);
  if (!sgn)
    cmp = (unsigned long long)(-1);
  return r;
}

INTRINSIC(v64uint8) bneg(v64uint8 a) {
  unsigned long long cmp;
  v64uint8 r = bneg_ltz(a, cmp);
  return r;
}

INTRINSIC(v64uint8) bxor(v64uint8 a, v64uint8 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}

INTRINSIC(v64uint8) abs_gtz(v64uint8 a, unsigned long long &cmp) {
  return __builtin_aiev2_vabs_gtz8(a, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8) abs_gtz(v64uint8 a, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vabs_gtz8(a, sgn, cmp);
}

INTRINSIC(v64uint8) abs(v64uint8 a) { return a; }

INTRINSIC(v64uint8) abs(v64uint8 a, bool sgn) {
  unsigned long long cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned long long) lt(v64uint8 a, v64uint8 b) {
  return (unsigned long long)__builtin_aiev2_vlt8(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned long long) ge(v64uint8 a, v64uint8 b) {
  return (unsigned long long)__builtin_aiev2_vge8(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned long long) le(v64uint8 a, v64uint8 b) { return ge(b, a); }

INTRINSIC(unsigned long long) gt(v64uint8 a, v64uint8 b) { return lt(b, a); }

INTRINSIC(unsigned long long) lt(v64uint8 a, v64uint8 b, bool sgn) {
  return (unsigned long long)__builtin_aiev2_vlt8(a, b, sgn);
}

INTRINSIC(unsigned long long) ge(v64uint8 a, v64uint8 b, bool sgn) {
  return (unsigned long long)__builtin_aiev2_vge8(a, b, sgn);
}

INTRINSIC(unsigned long long) le(v64uint8 a, v64uint8 b, bool sgn) {
  return ge(b, a, sgn);
}

INTRINSIC(unsigned long long) gt(v64uint8 a, v64uint8 b, bool sgn) {
  return lt(b, a, sgn);
}

INTRINSIC(unsigned long long) ltz(v64uint8 a) {
  unsigned long long cmp;
  bneg_ltz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) ltz(v64uint8 a, bool sgn) {
  unsigned long long cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) gtz(v64uint8 a) {
  unsigned long long cmp;
  abs_gtz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) gtz(v64uint8 a, bool sgn) {
  unsigned long long cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) eqz(v64uint8 a) {
  return (unsigned long long)__builtin_aiev2_veqz8(a);
}

INTRINSIC(unsigned long long) eq(v64uint8 a, v64uint8 b) {
  return eqz(sub(a, b));
}

INTRINSIC(unsigned long long) ne(v64uint8 a, v64uint8 b) { return ~(eq(a, b)); }

INTRINSIC(v64uint8) sel(v64uint8 a, v64uint8 b, unsigned long long sel) {
  return __builtin_aiev2_vsel8(a, b, (v2int32)sel);
}

INTRINSIC(v64int8) add(v64int8 a, v64int8 b) { return a + b; }

INTRINSIC(v64int8) sub(v64int8 a, v64int8 b) { return a - b; }

INTRINSIC(v64int8) addsub(v64int8 a, v64int8 b, unsigned long long as) {
  return __builtin_aiev2_vaddsub8(a, b, (v2int32)as);
}

INTRINSIC(v64int8) neg_gtz(v64int8 a, unsigned long long &cmp) {
  return __builtin_aiev2_vneg_gtz8(a, cmp);
}

INTRINSIC(v64int8) neg(v64int8 a) {
  unsigned long long cmp;
  return __builtin_aiev2_vneg_gtz8(a, cmp);
}

INTRINSIC(v64int8) sub_lt(v64int8 a, v64int8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vsub_lt8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
sub_lt(v64int8 a, v64int8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vsub_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64int8) sub_ge(v64int8 a, v64int8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vsub_ge8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
sub_ge(v64int8 a, v64int8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vsub_ge8(a, b, sgn, cmp);
}

INTRINSIC(v64int8) maxdiff_lt(v64int8 a, v64int8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vmaxdiff_lt8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
maxdiff_lt(v64int8 a, v64int8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vmaxdiff_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64int8) maxdiff(v64int8 a, v64int8 b) {
  unsigned long long cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v64int8) maxdiff(v64int8 a, v64int8 b, bool sgn) {
  unsigned long long cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v64int8) min_ge(v64int8 a, v64int8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vmin_ge8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
min_ge(v64int8 a, v64int8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vmin_ge8(a, b, sgn, cmp);
}

INTRINSIC(v64int8) min(v64int8 a, v64int8 b) {
  unsigned long long cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v64int8) min(v64int8 a, v64int8 b, bool sgn) {
  unsigned long long cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v64int8) max_lt(v64int8 a, v64int8 b, unsigned long long &cmp) {
  return __builtin_aiev2_vmax_lt8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
max_lt(v64int8 a, v64int8 b, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vmax_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64int8) max(v64int8 a, v64int8 b) {
  unsigned long long cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v64int8) max(v64int8 a, v64int8 b, bool sgn) {
  unsigned long long cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v64int8) band(v64int8 a, v64int8 b) { return a & b; }

INTRINSIC(v64int8) bor(v64int8 a, v64int8 b) { return a | b; }

INTRINSIC(v64int8) bneg_ltz(v64int8 a, unsigned long long &cmp) {
  v64int8 r = __builtin_aiev2_vbneg_ltz8(a, cmp);
  return r;
}

INTRINSIC(v64int8) bneg_ltz(v64int8 a, bool sgn, unsigned long long &cmp) {
  v64int8 r = __builtin_aiev2_vbneg_ltz8(a, cmp);
  if (!sgn)
    cmp = (unsigned long long)(-1);
  return r;
}

INTRINSIC(v64int8) bneg(v64int8 a) {
  unsigned long long cmp;
  v64int8 r = bneg_ltz(a, cmp);
  return r;
}

INTRINSIC(v64int8) bxor(v64int8 a, v64int8 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}

INTRINSIC(v64int8) abs_gtz(v64int8 a, unsigned long long &cmp) {
  return __builtin_aiev2_vabs_gtz8(a, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8) abs_gtz(v64int8 a, bool sgn, unsigned long long &cmp) {
  return __builtin_aiev2_vabs_gtz8(a, sgn, cmp);
}

INTRINSIC(v64int8) abs(v64int8 a) {
  unsigned long long cmp;
  return abs_gtz(a, cmp);
}

INTRINSIC(v64int8) abs(v64int8 a, bool sgn) {
  unsigned long long cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned long long) lt(v64int8 a, v64int8 b) {
  return (unsigned long long)__builtin_aiev2_vlt8(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned long long) ge(v64int8 a, v64int8 b) {
  return (unsigned long long)__builtin_aiev2_vge8(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned long long) le(v64int8 a, v64int8 b) { return ge(b, a); }

INTRINSIC(unsigned long long) gt(v64int8 a, v64int8 b) { return lt(b, a); }

INTRINSIC(unsigned long long) lt(v64int8 a, v64int8 b, bool sgn) {
  return (unsigned long long)__builtin_aiev2_vlt8(a, b, sgn);
}

INTRINSIC(unsigned long long) ge(v64int8 a, v64int8 b, bool sgn) {
  return (unsigned long long)__builtin_aiev2_vge8(a, b, sgn);
}

INTRINSIC(unsigned long long) le(v64int8 a, v64int8 b, bool sgn) {
  return ge(b, a, sgn);
}

INTRINSIC(unsigned long long) gt(v64int8 a, v64int8 b, bool sgn) {
  return lt(b, a, sgn);
}

INTRINSIC(unsigned long long) ltz(v64int8 a) {
  unsigned long long cmp;
  bneg_ltz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) ltz(v64int8 a, bool sgn) {
  unsigned long long cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) gtz(v64int8 a) {
  unsigned long long cmp;
  abs_gtz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) gtz(v64int8 a, bool sgn) {
  unsigned long long cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) eqz(v64int8 a) {
  return (unsigned long long)__builtin_aiev2_veqz8(a);
}

INTRINSIC(unsigned long long) eq(v64int8 a, v64int8 b) {
  return eqz(sub(a, b));
}

INTRINSIC(unsigned long long) ne(v64int8 a, v64int8 b) { return ~(eq(a, b)); }

INTRINSIC(v64int8) sel(v64int8 a, v64int8 b, unsigned long long sel) {
  return __builtin_aiev2_vsel8(a, b, (v2int32)sel);
}

INTRINSIC(v32uint16) add(v32uint16 a, v32uint16 b) { return a + b; }

INTRINSIC(v32uint16) sub(v32uint16 a, v32uint16 b) { return a - b; }

INTRINSIC(v32uint16) addsub(v32uint16 a, v32uint16 b, unsigned int as) {
  return __builtin_aiev2_vaddsub16(a, b, as);
}

INTRINSIC(v32uint16) neg_gtz(v32uint16 a, unsigned int &cmp) {
  return __builtin_aiev2_vneg_gtz16(a, cmp);
}

INTRINSIC(v32uint16) neg(v32uint16 a) {
  unsigned int cmp;
  return __builtin_aiev2_vneg_gtz16(a, cmp);
}

INTRINSIC(v32uint16) sub_lt(v32uint16 a, v32uint16 b, unsigned int &cmp) {
  return __builtin_aiev2_vsub_lt16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
sub_lt(v32uint16 a, v32uint16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vsub_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16) sub_ge(v32uint16 a, v32uint16 b, unsigned int &cmp) {
  return __builtin_aiev2_vsub_ge16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
sub_ge(v32uint16 a, v32uint16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vsub_ge16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16) maxdiff_lt(v32uint16 a, v32uint16 b, unsigned int &cmp) {
  return __builtin_aiev2_vmaxdiff_lt16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
maxdiff_lt(v32uint16 a, v32uint16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmaxdiff_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16) maxdiff(v32uint16 a, v32uint16 b) {
  unsigned int cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v32uint16) maxdiff(v32uint16 a, v32uint16 b, bool sgn) {
  unsigned int cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v32uint16) min_ge(v32uint16 a, v32uint16 b, unsigned int &cmp) {
  return __builtin_aiev2_vmin_ge16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
min_ge(v32uint16 a, v32uint16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmin_ge16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16) min(v32uint16 a, v32uint16 b) {
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v32uint16) min(v32uint16 a, v32uint16 b, bool sgn) {
  unsigned int cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v32uint16) max_lt(v32uint16 a, v32uint16 b, unsigned int &cmp) {
  return __builtin_aiev2_vmax_lt16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
max_lt(v32uint16 a, v32uint16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmax_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16) max(v32uint16 a, v32uint16 b) {
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v32uint16) max(v32uint16 a, v32uint16 b, bool sgn) {
  unsigned int cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v32uint16) band(v32uint16 a, v32uint16 b) { return a & b; }

INTRINSIC(v32uint16) bor(v32uint16 a, v32uint16 b) { return a | b; }

INTRINSIC(v32uint16) bneg_ltz(v32uint16 a, unsigned int &cmp) {
  v32uint16 r = __builtin_aiev2_vbneg_ltz16(a, cmp);
  cmp = (unsigned int)(-1);
  return r;
}

INTRINSIC(v32uint16) bneg_ltz(v32uint16 a, bool sgn, unsigned int &cmp) {
  v32uint16 r = __builtin_aiev2_vbneg_ltz16(a, cmp);
  if (!sgn)
    cmp = (unsigned int)(-1);
  return r;
}

INTRINSIC(v32uint16) bneg(v32uint16 a) {
  unsigned int cmp;
  v32uint16 r = bneg_ltz(a, cmp);
  return r;
}

INTRINSIC(v32uint16) bxor(v32uint16 a, v32uint16 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}

INTRINSIC(v32uint16) abs_gtz(v32uint16 a, unsigned int &cmp) {
  return __builtin_aiev2_vabs_gtz16(a, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16) abs_gtz(v32uint16 a, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vabs_gtz16(a, sgn, cmp);
}

INTRINSIC(v32uint16) abs(v32uint16 a) { return a; }

INTRINSIC(v32uint16) abs(v32uint16 a, bool sgn) {
  unsigned int cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned int) lt(v32uint16 a, v32uint16 b) {
  return __builtin_aiev2_vlt16(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int) ge(v32uint16 a, v32uint16 b) {
  return __builtin_aiev2_vge16(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int) le(v32uint16 a, v32uint16 b) { return ge(b, a); }

INTRINSIC(unsigned int) gt(v32uint16 a, v32uint16 b) { return lt(b, a); }

INTRINSIC(unsigned int) lt(v32uint16 a, v32uint16 b, bool sgn) {
  return __builtin_aiev2_vlt16(a, b, sgn);
}

INTRINSIC(unsigned int) ge(v32uint16 a, v32uint16 b, bool sgn) {
  return __builtin_aiev2_vge16(a, b, sgn);
}

INTRINSIC(unsigned int) le(v32uint16 a, v32uint16 b, bool sgn) {
  return ge(b, a, sgn);
}

INTRINSIC(unsigned int) gt(v32uint16 a, v32uint16 b, bool sgn) {
  return lt(b, a, sgn);
}

INTRINSIC(unsigned int) ltz(v32uint16 a) {
  unsigned int cmp;
  bneg_ltz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned int) ltz(v32uint16 a, bool sgn) {
  unsigned int cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) gtz(v32uint16 a) {
  unsigned int cmp;
  abs_gtz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned int) gtz(v32uint16 a, bool sgn) {
  unsigned int cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) eqz(v32uint16 a) { return __builtin_aiev2_veqz16(a); }

INTRINSIC(unsigned int) eq(v32uint16 a, v32uint16 b) { return eqz(sub(a, b)); }

INTRINSIC(unsigned int) ne(v32uint16 a, v32uint16 b) { return ~(eq(a, b)); }

INTRINSIC(v32uint16) sel(v32uint16 a, v32uint16 b, unsigned int sel) {
  return __builtin_aiev2_vsel16(a, b, sel);
}

INTRINSIC(v32int16) add(v32int16 a, v32int16 b) { return a + b; }

INTRINSIC(v32int16) sub(v32int16 a, v32int16 b) { return a - b; }

INTRINSIC(v32int16) addsub(v32int16 a, v32int16 b, unsigned int as) {
  return __builtin_aiev2_vaddsub16(a, b, as);
}

INTRINSIC(v32int16) neg_gtz(v32int16 a, unsigned int &cmp) {
  return __builtin_aiev2_vneg_gtz16(a, cmp);
}

INTRINSIC(v32int16) neg(v32int16 a) {
  unsigned int cmp;
  return __builtin_aiev2_vneg_gtz16(a, cmp);
}

INTRINSIC(v32int16) sub_lt(v32int16 a, v32int16 b, unsigned int &cmp) {
  return __builtin_aiev2_vsub_lt16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
sub_lt(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vsub_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32int16) sub_ge(v32int16 a, v32int16 b, unsigned int &cmp) {
  return __builtin_aiev2_vsub_ge16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
sub_ge(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vsub_ge16(a, b, sgn, cmp);
}

INTRINSIC(v32int16) maxdiff_lt(v32int16 a, v32int16 b, unsigned int &cmp) {
  return __builtin_aiev2_vmaxdiff_lt16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
maxdiff_lt(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmaxdiff_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32int16) maxdiff(v32int16 a, v32int16 b) {
  unsigned int cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v32int16) maxdiff(v32int16 a, v32int16 b, bool sgn) {
  unsigned int cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v32int16) min_ge(v32int16 a, v32int16 b, unsigned int &cmp) {
  return __builtin_aiev2_vmin_ge16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
min_ge(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmin_ge16(a, b, sgn, cmp);
}

INTRINSIC(v32int16) min(v32int16 a, v32int16 b) {
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v32int16) min(v32int16 a, v32int16 b, bool sgn) {
  unsigned int cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v32int16) max_lt(v32int16 a, v32int16 b, unsigned int &cmp) {
  return __builtin_aiev2_vmax_lt16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
max_lt(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmax_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32int16) max(v32int16 a, v32int16 b) {
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v32int16) max(v32int16 a, v32int16 b, bool sgn) {
  unsigned int cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v32int16) band(v32int16 a, v32int16 b) { return a & b; }

INTRINSIC(v32int16) bor(v32int16 a, v32int16 b) { return a | b; }

INTRINSIC(v32int16) bneg_ltz(v32int16 a, unsigned int &cmp) {
  v32int16 r = __builtin_aiev2_vbneg_ltz16(a, cmp);
  return r;
}

INTRINSIC(v32int16) bneg_ltz(v32int16 a, bool sgn, unsigned int &cmp) {
  v32int16 r = __builtin_aiev2_vbneg_ltz16(a, cmp);
  if (!sgn)
    cmp = (unsigned int)(-1);
  return r;
}

INTRINSIC(v32int16) bneg(v32int16 a) {
  unsigned int cmp;
  v32int16 r = bneg_ltz(a, cmp);
  return r;
}

INTRINSIC(v32int16) bxor(v32int16 a, v32int16 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}

INTRINSIC(v32int16) abs_gtz(v32int16 a, unsigned int &cmp) {
  return __builtin_aiev2_vabs_gtz16(a, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16) abs_gtz(v32int16 a, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vabs_gtz16(a, sgn, cmp);
}

INTRINSIC(v32int16) abs(v32int16 a) {
  unsigned int cmp;
  return abs_gtz(a, cmp);
}

INTRINSIC(v32int16) abs(v32int16 a, bool sgn) {
  unsigned int cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned int) lt(v32int16 a, v32int16 b) {
  return __builtin_aiev2_vlt16(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned int) ge(v32int16 a, v32int16 b) {
  return __builtin_aiev2_vge16(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned int) le(v32int16 a, v32int16 b) { return ge(b, a); }

INTRINSIC(unsigned int) gt(v32int16 a, v32int16 b) { return lt(b, a); }

INTRINSIC(unsigned int) lt(v32int16 a, v32int16 b, bool sgn) {
  return __builtin_aiev2_vlt16(a, b, sgn);
}

INTRINSIC(unsigned int) ge(v32int16 a, v32int16 b, bool sgn) {
  return __builtin_aiev2_vge16(a, b, sgn);
}

INTRINSIC(unsigned int) le(v32int16 a, v32int16 b, bool sgn) {
  return ge(b, a, sgn);
}

INTRINSIC(unsigned int) gt(v32int16 a, v32int16 b, bool sgn) {
  return lt(b, a, sgn);
}

INTRINSIC(unsigned int) ltz(v32int16 a) {
  unsigned int cmp;
  bneg_ltz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned int) ltz(v32int16 a, bool sgn) {
  unsigned int cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) gtz(v32int16 a) {
  unsigned int cmp;
  abs_gtz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned int) gtz(v32int16 a, bool sgn) {
  unsigned int cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) eqz(v32int16 a) { return __builtin_aiev2_veqz16(a); }

INTRINSIC(unsigned int) eq(v32int16 a, v32int16 b) { return eqz(sub(a, b)); }

INTRINSIC(unsigned int) ne(v32int16 a, v32int16 b) { return ~(eq(a, b)); }

INTRINSIC(v32int16) sel(v32int16 a, v32int16 b, unsigned int sel) {
  return __builtin_aiev2_vsel16(a, b, sel);
}

INTRINSIC(v16uint32) add(v16uint32 a, v16uint32 b) { return a + b; }

INTRINSIC(v16uint32) sub(v16uint32 a, v16uint32 b) { return a - b; }

INTRINSIC(v16uint32) addsub(v16uint32 a, v16uint32 b, unsigned int as) {
  return __builtin_aiev2_vaddsub32(a, b, as);
}

INTRINSIC(v16uint32) neg_gtz(v16uint32 a, unsigned int &cmp) {
  return __builtin_aiev2_vneg_gtz32(a, cmp);
}

INTRINSIC(v16uint32) neg(v16uint32 a) {
  unsigned int cmp;
  return __builtin_aiev2_vneg_gtz32(a, cmp);
}

INTRINSIC(v16uint32) sub_lt(v16uint32 a, v16uint32 b, unsigned int &cmp) {
  return __builtin_aiev2_vsub_lt32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
sub_lt(v16uint32 a, v16uint32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vsub_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32) sub_ge(v16uint32 a, v16uint32 b, unsigned int &cmp) {
  return __builtin_aiev2_vsub_ge32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
sub_ge(v16uint32 a, v16uint32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vsub_ge32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32) maxdiff_lt(v16uint32 a, v16uint32 b, unsigned int &cmp) {
  return __builtin_aiev2_vmaxdiff_lt32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
maxdiff_lt(v16uint32 a, v16uint32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmaxdiff_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32) maxdiff(v16uint32 a, v16uint32 b) {
  unsigned int cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v16uint32) maxdiff(v16uint32 a, v16uint32 b, bool sgn) {
  unsigned int cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v16uint32) min_ge(v16uint32 a, v16uint32 b, unsigned int &cmp) {
  return __builtin_aiev2_vmin_ge32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
min_ge(v16uint32 a, v16uint32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmin_ge32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32) min(v16uint32 a, v16uint32 b) {
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v16uint32) min(v16uint32 a, v16uint32 b, bool sgn) {
  unsigned int cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v16uint32) max_lt(v16uint32 a, v16uint32 b, unsigned int &cmp) {
  return __builtin_aiev2_vmax_lt32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
max_lt(v16uint32 a, v16uint32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmax_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32) max(v16uint32 a, v16uint32 b) {
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v16uint32) max(v16uint32 a, v16uint32 b, bool sgn) {
  unsigned int cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v16uint32) band(v16uint32 a, v16uint32 b) { return a & b; }

INTRINSIC(v16uint32) bor(v16uint32 a, v16uint32 b) { return a | b; }

INTRINSIC(v16uint32) bneg_ltz(v16uint32 a, unsigned int &cmp) {
  v16uint32 r = __builtin_aiev2_vbneg_ltz32(a, cmp);
  cmp = (unsigned int)(-1);
  return r;
}

INTRINSIC(v16uint32) bneg_ltz(v16uint32 a, bool sgn, unsigned int &cmp) {
  v16uint32 r = __builtin_aiev2_vbneg_ltz32(a, cmp);
  if (!sgn)
    cmp = (unsigned int)(-1);
  return r;
}

INTRINSIC(v16uint32) bneg(v16uint32 a) {
  unsigned int cmp;
  v16uint32 r = bneg_ltz(a, cmp);
  return r;
}

INTRINSIC(v16uint32) bxor(v16uint32 a, v16uint32 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}

INTRINSIC(v16uint32) abs_gtz(v16uint32 a, unsigned int &cmp) {
  return __builtin_aiev2_vabs_gtz32(a, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32) abs_gtz(v16uint32 a, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vabs_gtz32(a, sgn, cmp);
}

INTRINSIC(v16uint32) abs(v16uint32 a) { return a; }

INTRINSIC(v16uint32) abs(v16uint32 a, bool sgn) {
  unsigned int cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned int) lt(v16uint32 a, v16uint32 b) {
  return __builtin_aiev2_vlt32(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int) ge(v16uint32 a, v16uint32 b) {
  return __builtin_aiev2_vge32(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int) le(v16uint32 a, v16uint32 b) { return ge(b, a); }

INTRINSIC(unsigned int) gt(v16uint32 a, v16uint32 b) { return lt(b, a); }

INTRINSIC(unsigned int) lt(v16uint32 a, v16uint32 b, bool sgn) {
  return __builtin_aiev2_vlt32(a, b, sgn);
}

INTRINSIC(unsigned int) ge(v16uint32 a, v16uint32 b, bool sgn) {
  return __builtin_aiev2_vge32(a, b, sgn);
}

INTRINSIC(unsigned int) le(v16uint32 a, v16uint32 b, bool sgn) {
  return ge(b, a, sgn);
}

INTRINSIC(unsigned int) gt(v16uint32 a, v16uint32 b, bool sgn) {
  return lt(b, a, sgn);
}

INTRINSIC(unsigned int) ltz(v16uint32 a) {
  unsigned int cmp;
  bneg_ltz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned int) ltz(v16uint32 a, bool sgn) {
  unsigned int cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) gtz(v16uint32 a) {
  unsigned int cmp;
  abs_gtz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned int) gtz(v16uint32 a, bool sgn) {
  unsigned int cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) eqz(v16uint32 a) { return __builtin_aiev2_veqz32(a); }

INTRINSIC(unsigned int) eq(v16uint32 a, v16uint32 b) { return eqz(sub(a, b)); }

INTRINSIC(unsigned int) ne(v16uint32 a, v16uint32 b) { return ~(eq(a, b)); }

INTRINSIC(v16uint32) sel(v16uint32 a, v16uint32 b, unsigned int sel) {
  return __builtin_aiev2_vsel32(a, b, sel);
}

INTRINSIC(v16int32) add(v16int32 a, v16int32 b) { return a + b; }

INTRINSIC(v16int32) sub(v16int32 a, v16int32 b) { return a - b; }

INTRINSIC(v16int32) addsub(v16int32 a, v16int32 b, unsigned int as) {
  return __builtin_aiev2_vaddsub32(a, b, as);
}

INTRINSIC(v16int32) neg_gtz(v16int32 a, unsigned int &cmp) {
  return __builtin_aiev2_vneg_gtz32(a, cmp);
}

INTRINSIC(v16int32) neg(v16int32 a) {
  unsigned int cmp;
  return __builtin_aiev2_vneg_gtz32(a, cmp);
}

INTRINSIC(v16int32) sub_lt(v16int32 a, v16int32 b, unsigned int &cmp) {
  return __builtin_aiev2_vsub_lt32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
sub_lt(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vsub_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16int32) sub_ge(v16int32 a, v16int32 b, unsigned int &cmp) {
  return __builtin_aiev2_vsub_ge32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
sub_ge(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vsub_ge32(a, b, sgn, cmp);
}

INTRINSIC(v16int32) maxdiff_lt(v16int32 a, v16int32 b, unsigned int &cmp) {
  return __builtin_aiev2_vmaxdiff_lt32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
maxdiff_lt(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmaxdiff_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16int32) maxdiff(v16int32 a, v16int32 b) {
  unsigned int cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v16int32) maxdiff(v16int32 a, v16int32 b, bool sgn) {
  unsigned int cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v16int32) min_ge(v16int32 a, v16int32 b, unsigned int &cmp) {
  return __builtin_aiev2_vmin_ge32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
min_ge(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmin_ge32(a, b, sgn, cmp);
}

INTRINSIC(v16int32) min(v16int32 a, v16int32 b) {
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v16int32) min(v16int32 a, v16int32 b, bool sgn) {
  unsigned int cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v16int32) max_lt(v16int32 a, v16int32 b, unsigned int &cmp) {
  return __builtin_aiev2_vmax_lt32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
max_lt(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vmax_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16int32) max(v16int32 a, v16int32 b) {
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v16int32) max(v16int32 a, v16int32 b, bool sgn) {
  unsigned int cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v16int32) band(v16int32 a, v16int32 b) { return a & b; }

INTRINSIC(v16int32) bor(v16int32 a, v16int32 b) { return a | b; }

INTRINSIC(v16int32) bneg_ltz(v16int32 a, unsigned int &cmp) {
  v16int32 r = __builtin_aiev2_vbneg_ltz32(a, cmp);
  return r;
}

INTRINSIC(v16int32) bneg_ltz(v16int32 a, bool sgn, unsigned int &cmp) {
  v16int32 r = __builtin_aiev2_vbneg_ltz32(a, cmp);
  if (!sgn)
    cmp = (unsigned int)(-1);
  return r;
}

INTRINSIC(v16int32) bneg(v16int32 a) {
  unsigned int cmp;
  v16int32 r = bneg_ltz(a, cmp);
  return r;
}

INTRINSIC(v16int32) bxor(v16int32 a, v16int32 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}

INTRINSIC(v16int32) abs_gtz(v16int32 a, unsigned int &cmp) {
  return __builtin_aiev2_vabs_gtz32(a, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32) abs_gtz(v16int32 a, bool sgn, unsigned int &cmp) {
  return __builtin_aiev2_vabs_gtz32(a, sgn, cmp);
}

INTRINSIC(v16int32) abs(v16int32 a) {
  unsigned int cmp;
  return abs_gtz(a, cmp);
}

INTRINSIC(v16int32) abs(v16int32 a, bool sgn) {
  unsigned int cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned int) lt(v16int32 a, v16int32 b) {
  return __builtin_aiev2_vlt32(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned int) ge(v16int32 a, v16int32 b) {
  return __builtin_aiev2_vge32(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned int) le(v16int32 a, v16int32 b) { return ge(b, a); }

INTRINSIC(unsigned int) gt(v16int32 a, v16int32 b) { return lt(b, a); }

INTRINSIC(unsigned int) lt(v16int32 a, v16int32 b, bool sgn) {
  return __builtin_aiev2_vlt32(a, b, sgn);
}

INTRINSIC(unsigned int) ge(v16int32 a, v16int32 b, bool sgn) {
  return __builtin_aiev2_vge32(a, b, sgn);
}

INTRINSIC(unsigned int) le(v16int32 a, v16int32 b, bool sgn) {
  return ge(b, a, sgn);
}

INTRINSIC(unsigned int) gt(v16int32 a, v16int32 b, bool sgn) {
  return lt(b, a, sgn);
}

INTRINSIC(unsigned int) ltz(v16int32 a) {
  unsigned int cmp;
  bneg_ltz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned int) ltz(v16int32 a, bool sgn) {
  unsigned int cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) gtz(v16int32 a) {
  unsigned int cmp;
  abs_gtz(a, cmp);
  return cmp;
}

INTRINSIC(unsigned int) gtz(v16int32 a, bool sgn) {
  unsigned int cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) eqz(v16int32 a) { return __builtin_aiev2_veqz32(a); }

INTRINSIC(unsigned int) eq(v16int32 a, v16int32 b) { return eqz(sub(a, b)); }

INTRINSIC(unsigned int) ne(v16int32 a, v16int32 b) { return ~(eq(a, b)); }

INTRINSIC(v16int32) sel(v16int32 a, v16int32 b, unsigned int sel) {
  return __builtin_aiev2_vsel32(a, b, sel);
}

// Vector bitwise AND operation for datatype v32bfloat16
INTRINSIC(v32bfloat16) band(v32bfloat16 a, v32bfloat16 b) {
  return band((v32int16)a, (v32int16)b);
}

// Vector bitwise OR operation for datatype v32bfloat16
INTRINSIC(v32bfloat16) bor(v32bfloat16 a, v32bfloat16 b) {
  return bor((v32int16)a, (v32int16)b);
}

// Vector bitwise operation bitwise negation
INTRINSIC(v32bfloat16) bneg(v32bfloat16 a) { return bneg((v32int16)a); }

// Vector Bitwise XOR
INTRINSIC(v32bfloat16) bxor(v32bfloat16 a, v32bfloat16 b) {
  return bxor((v32int16)a, (v32int16)b);
}

// Vector absolute operation.
INTRINSIC(v32bfloat16) abs(v32bfloat16 a) {
  return band(a, (v32bfloat16)broadcast_u16(0x7fff));
}

INTRINSIC(v32bfloat16) sel(v32bfloat16 a, v32bfloat16 b, unsigned int sel) {
  return __builtin_aiev2_vsel16(a, b, sel);
}

INTRINSIC(v32bfloat16) max_lt(v32bfloat16 a, v32bfloat16 b, unsigned int &cmp) {
  return __builtin_aiev2_vmax_ltbf16(a, b, cmp);
}

// Calculates the maximum between two input vectors.
INTRINSIC(v32bfloat16) max(v32bfloat16 a, v32bfloat16 b) {
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v32bfloat16) min_ge(v32bfloat16 a, v32bfloat16 b, unsigned int &cmp) {
  return __builtin_aiev2_vmin_gebf16(a, b, cmp);
}

// Calculates the minimum between two input vectors.
INTRINSIC(v32bfloat16) min(v32bfloat16 a, v32bfloat16 b) {
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

// Checks for which vector lanes a < b is fulfilled.
INTRINSIC(unsigned int) lt(v32bfloat16 a, v32bfloat16 b) {
  return __builtin_aiev2_vltbf16(a, b);
}

// Checks for which vector lanes a >= b is fulfilled.
INTRINSIC(unsigned int) ge(v32bfloat16 a, v32bfloat16 b) {
  return __builtin_aiev2_vgebf16(a, b);
}

// Checks for which vector lanes a <= b is fulfilled.
INTRINSIC(unsigned int) le(v32bfloat16 a, v32bfloat16 b) { return ge(b, a); }

// Checks for which vector lanes a > b is fulfilled.
INTRINSIC(unsigned int) gt(v32bfloat16 a, v32bfloat16 b) { return lt(b, a); }

// Checks for which vector lanes a < 0 is fulfilled.
INTRINSIC(unsigned int) ltz(v32bfloat16 a) {
  return lt(a, (v32bfloat16)broadcast_u16(0x8000)); // a < -0.0f
}

// Checks for which vector lanes a > 0 is fulfilled.
INTRINSIC(unsigned int) gtz(v32bfloat16 a) {
  return gt(a, (v32bfloat16)broadcast_u16(0x0000)); // a > +0.0f;
}

// Checks for which vector lanes a == 0 is fulfilled.
INTRINSIC(unsigned int) eqz(v32bfloat16 a) { return __builtin_aiev2_veqz16(a); }

// Checks for which vector lanes a == b is fulfilled.
INTRINSIC(unsigned int) eq(v32bfloat16 a, v32bfloat16 b) {
  return le(a, b) & ge(a, b);
}

// Checks for which vector lanes a != b is fulfilled.
INTRINSIC(unsigned int) ne(v32bfloat16 a, v32bfloat16 b) {
  return lt(a, b) | gt(a, b);
}

// Ideally this should go in differnt file, but there is just
// one conversion, should we continue keeping it here?
INTRINSIC(v16int32) bfloat16_to_int(v16bfloat16 a, int shft) {
  return __builtin_aiev2_v16bf16_to_v16i32(a, shft);
}

INTRINSIC(v16float) sel(v16float a, v16float b, unsigned int sel) {
  return __builtin_aiev2_vsel32(a, b, sel);
}
