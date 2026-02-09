//===- aie2ps_vadd.h -------------------------------------------*- C++ -*-===//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_VADD_H__
#define __AIE2PS_VADD_H__

// ------------------------------------------------------------

INTRINSIC(v64int8) add(v64int8 a, v64int8 b) { return (a + b); }
INTRINSIC(v64int8) sub(v64int8 a, v64int8 b) { return (a - b); }
INTRINSIC(v64int8) addsub(v64int8 a, v64int8 b, unsigned long long as) {
  return __builtin_aie2ps_vaddsub8(a, b, (v2int32)as);
}

INTRINSIC(v64int8) neg_gtz(v64int8 a, unsigned long long &cmp) {
  return __builtin_aie2ps_vneg_gtz8(a, cmp);
}
INTRINSIC(v64int8) neg(v64int8 a) {
  unsigned long long cmp;
  return __builtin_aie2ps_vneg_gtz8(a, cmp);
}

INTRINSIC(v64int8)
sub_lt(v64int8 a, v64int8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vsub_lt8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
sub_lt(v64int8 a, v64int8 b, bool sgn, unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64int8)
sub_ge(v64int8 a, v64int8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vsub_ge8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
sub_ge(v64int8 a, v64int8 b, bool sgn, unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_ge8(a, b, sgn, cmp);
}

INTRINSIC(v64int8)
maxdiff_lt(v64int8 a, v64int8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vmaxdiff_lt8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
maxdiff_lt(v64int8 a, v64int8 b, bool sgn,
           unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmaxdiff_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64int8)
maxdiff(v64int8 a, v64int8 b) // static sign
{
  unsigned long long cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v64int8)
maxdiff(v64int8 a, v64int8 b, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v64int8)
min_ge(v64int8 a, v64int8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vmin_ge8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
min_ge(v64int8 a, v64int8 b, bool sgn, unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmin_ge8(a, b, sgn, cmp);
}

INTRINSIC(v64int8)
min(v64int8 a, v64int8 b) // static sign
{
  unsigned long long cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v64int8)
min(v64int8 a, v64int8 b, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v64int8)
max_lt(v64int8 a, v64int8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vmax_lt8(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
max_lt(v64int8 a, v64int8 b, bool sgn, unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmax_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64int8)
max(v64int8 a, v64int8 b) // static sign
{
  unsigned long long cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v64int8)
max(v64int8 a, v64int8 b, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v64int8) band(v64int8 a, v64int8 b) { return a & b; }

INTRINSIC(v64int8) bor(v64int8 a, v64int8 b) { return a | b; }

INTRINSIC(v64int8)
bneg_ltz(v64int8 a, unsigned long long &cmp) // static sign
{
  v64int8 r = __builtin_aie2ps_vbneg_ltz8(a, cmp);
  return r;
}

INTRINSIC(v64int8)
bneg_ltz(v64int8 a, bool sgn, unsigned long long &cmp) // dynamic sign
{
  v64int8 r = __builtin_aie2ps_vbneg_ltz8(a, cmp);
  if (!sgn)
    cmp = 0;
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
INTRINSIC(v64int8)
abs_gtz(v64int8 a, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vabs_gtz8(a, __SIGN_SIGNED, cmp);
}

INTRINSIC(v64int8)
abs_gtz(v64int8 a, bool sgn, unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vabs_gtz8(a, (uint1_t)sgn, cmp);
}

INTRINSIC(v64int8)
abs(v64int8 a) // static sign
{
  unsigned long long cmp;
  return abs_gtz(a, cmp);
}
INTRINSIC(v64int8)
abs(v64int8 a, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned long long)
lt(v64int8 a, v64int8 b) // static sign
{
  return (unsigned long long)__builtin_aie2ps_vlt8(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned long long)
ge(v64int8 a, v64int8 b) // static sign
{
  return (unsigned long long)__builtin_aie2ps_vge8(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned long long) le(v64int8 a, v64int8 b) { return ge(b, a); }
INTRINSIC(unsigned long long) gt(v64int8 a, v64int8 b) { return lt(b, a); }
INTRINSIC(unsigned long long)
lt(v64int8 a, v64int8 b, bool sgn) // dynamic sign
{
  return (unsigned long long)__builtin_aie2ps_vlt8(a, b, sgn);
}

INTRINSIC(unsigned long long)
ge(v64int8 a, v64int8 b, bool sgn) // dynamic sign
{
  return (unsigned long long)__builtin_aie2ps_vge8(a, b, sgn);
}

INTRINSIC(unsigned long long) le(v64int8 a, v64int8 b, bool sgn) {
  return ge(b, a, sgn);
}
INTRINSIC(unsigned long long) gt(v64int8 a, v64int8 b, bool sgn) {
  return lt(b, a, sgn);
}
INTRINSIC(unsigned long long)
ltz(v64int8 a) // static sign
{
  unsigned long long cmp;
  bneg_ltz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned long long)
ltz(v64int8 a, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}
INTRINSIC(unsigned long long)
gtz(v64int8 a) // static sign
{
  unsigned long long cmp;
  abs_gtz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned long long)
gtz(v64int8 a, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) eqz(v64int8 a) {
  return (unsigned long long)__builtin_aie2ps_veqz8(a);
}

INTRINSIC(unsigned long long) eq(v64int8 a, v64int8 b) {
  return eqz(sub(a, b));
}

INTRINSIC(unsigned long long) ne(v64int8 a, v64int8 b) {
  return eq(a, b) ^ 0xffffffffffffffffLL;
}
INTRINSIC(v64int8) sel(v64int8 a, v64int8 b, unsigned long long sel) {
  return __builtin_aie2ps_vsel8(a, b, (v2int32)sel);
}

// ------------------------------------------------------------

INTRINSIC(v64uint8) add(v64uint8 a, v64uint8 b) { return (a + b); }
INTRINSIC(v64uint8) sub(v64uint8 a, v64uint8 b) { return (a - b); }
INTRINSIC(v64uint8) addsub(v64uint8 a, v64uint8 b, unsigned long long as) {
  return __builtin_aie2ps_vaddsub8(a, b, (v2int32)as);
}

INTRINSIC(v64uint8) neg_gtz(v64uint8 a, unsigned long long &cmp) {
  return __builtin_aie2ps_vneg_gtz8(a, cmp);
}
INTRINSIC(v64uint8) neg(v64uint8 a) {
  unsigned long long cmp;
  return __builtin_aie2ps_vneg_gtz8(a, cmp);
}

INTRINSIC(v64uint8)
sub_lt(v64uint8 a, v64uint8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vsub_lt8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
sub_lt(v64uint8 a, v64uint8 b, bool sgn,
       unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8)
sub_ge(v64uint8 a, v64uint8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vsub_ge8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
sub_ge(v64uint8 a, v64uint8 b, bool sgn,
       unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_ge8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8)
maxdiff_lt(v64uint8 a, v64uint8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vmaxdiff_lt8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
maxdiff_lt(v64uint8 a, v64uint8 b, bool sgn,
           unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmaxdiff_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8)
maxdiff(v64uint8 a, v64uint8 b) // static sign
{
  unsigned long long cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v64uint8)
maxdiff(v64uint8 a, v64uint8 b, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v64uint8)
min_ge(v64uint8 a, v64uint8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vmin_ge8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
min_ge(v64uint8 a, v64uint8 b, bool sgn,
       unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmin_ge8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8)
min(v64uint8 a, v64uint8 b) // static sign
{
  unsigned long long cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v64uint8)
min(v64uint8 a, v64uint8 b, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v64uint8)
max_lt(v64uint8 a, v64uint8 b, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vmax_lt8(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
max_lt(v64uint8 a, v64uint8 b, bool sgn,
       unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmax_lt8(a, b, sgn, cmp);
}

INTRINSIC(v64uint8)
max(v64uint8 a, v64uint8 b) // static sign
{
  unsigned long long cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v64uint8)
max(v64uint8 a, v64uint8 b, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v64uint8) band(v64uint8 a, v64uint8 b) { return a & b; }

INTRINSIC(v64uint8) bor(v64uint8 a, v64uint8 b) { return a | b; }

INTRINSIC(v64uint8)
bneg_ltz(v64uint8 a, unsigned long long &cmp) // static sign
{
  v64uint8 r = __builtin_aie2ps_vbneg_ltz8(a, cmp);
  cmp = 0;
  return r;
}

INTRINSIC(v64uint8)
bneg_ltz(v64uint8 a, bool sgn, unsigned long long &cmp) // dynamic sign
{
  v64uint8 r = __builtin_aie2ps_vbneg_ltz8(a, cmp);
  if (!sgn)
    cmp = 0;
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
INTRINSIC(v64uint8)
abs_gtz(v64uint8 a, unsigned long long &cmp) // static sign
{
  return __builtin_aie2ps_vabs_gtz8(a, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v64uint8)
abs_gtz(v64uint8 a, bool sgn, unsigned long long &cmp) // dynamic sign
{
  return __builtin_aie2ps_vabs_gtz8(a, (uint1_t)sgn, cmp);
}

INTRINSIC(v64uint8)
abs(v64uint8 a) // static sign
{
  return a;
}
INTRINSIC(v64uint8)
abs(v64uint8 a, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned long long)
lt(v64uint8 a, v64uint8 b) // static sign
{
  return (unsigned long long)__builtin_aie2ps_vlt8(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned long long)
ge(v64uint8 a, v64uint8 b) // static sign
{
  return (unsigned long long)__builtin_aie2ps_vge8(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned long long) le(v64uint8 a, v64uint8 b) { return ge(b, a); }
INTRINSIC(unsigned long long) gt(v64uint8 a, v64uint8 b) { return lt(b, a); }
INTRINSIC(unsigned long long)
lt(v64uint8 a, v64uint8 b, bool sgn) // dynamic sign
{
  return (unsigned long long)__builtin_aie2ps_vlt8(a, b, sgn);
}

INTRINSIC(unsigned long long)
ge(v64uint8 a, v64uint8 b, bool sgn) // dynamic sign
{
  return (unsigned long long)__builtin_aie2ps_vge8(a, b, sgn);
}

INTRINSIC(unsigned long long) le(v64uint8 a, v64uint8 b, bool sgn) {
  return ge(b, a, sgn);
}
INTRINSIC(unsigned long long) gt(v64uint8 a, v64uint8 b, bool sgn) {
  return lt(b, a, sgn);
}
INTRINSIC(unsigned long long)
ltz(v64uint8 a) // static sign
{
  unsigned long long cmp;
  bneg_ltz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned long long)
ltz(v64uint8 a, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}
INTRINSIC(unsigned long long)
gtz(v64uint8 a) // static sign
{
  unsigned long long cmp;
  abs_gtz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned long long)
gtz(v64uint8 a, bool sgn) // dynamic sign
{
  unsigned long long cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned long long) eqz(v64uint8 a) {
  return (unsigned long long)__builtin_aie2ps_veqz8(a);
}

INTRINSIC(unsigned long long) eq(v64uint8 a, v64uint8 b) {
  return eqz(sub(a, b));
}

INTRINSIC(unsigned long long) ne(v64uint8 a, v64uint8 b) {
  return eq(a, b) ^ 0xffffffffffffffffLL;
}
INTRINSIC(v64uint8) sel(v64uint8 a, v64uint8 b, unsigned long long sel) {
  return __builtin_aie2ps_vsel8(a, b, (v2int32)sel);
}

// ------------------------------------------------------------

// ------------------------------------------------------------

INTRINSIC(v32int16) add(v32int16 a, v32int16 b) { return (a + b); }
INTRINSIC(v32int16) sub(v32int16 a, v32int16 b) { return (a - b); }
INTRINSIC(v32int16) addsub(v32int16 a, v32int16 b, unsigned int as) {
  return __builtin_aie2ps_vaddsub16(a, b, as);
}

INTRINSIC(v32int16) neg_gtz(v32int16 a, unsigned int &cmp) {
  return __builtin_aie2ps_vneg_gtz16(a, cmp);
}
INTRINSIC(v32int16) neg(v32int16 a) {
  unsigned int cmp;
  return __builtin_aie2ps_vneg_gtz16(a, cmp);
}

INTRINSIC(v32int16)
sub_lt(v32int16 a, v32int16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vsub_lt16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
sub_lt(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32int16)
sub_ge(v32int16 a, v32int16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vsub_ge16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
sub_ge(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_ge16(a, b, sgn, cmp);
}

INTRINSIC(v32int16)
maxdiff_lt(v32int16 a, v32int16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmaxdiff_lt16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
maxdiff_lt(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmaxdiff_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32int16)
maxdiff(v32int16 a, v32int16 b) // static sign
{
  unsigned int cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v32int16)
maxdiff(v32int16 a, v32int16 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v32int16)
min_ge(v32int16 a, v32int16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmin_ge16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
min_ge(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmin_ge16(a, b, sgn, cmp);
}

INTRINSIC(v32int16)
min(v32int16 a, v32int16 b) // static sign
{
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v32int16)
min(v32int16 a, v32int16 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v32int16)
max_lt(v32int16 a, v32int16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmax_lt16(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
max_lt(v32int16 a, v32int16 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmax_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32int16)
max(v32int16 a, v32int16 b) // static sign
{
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v32int16)
max(v32int16 a, v32int16 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v32int16) band(v32int16 a, v32int16 b) { return a & b; }

INTRINSIC(v32int16) bor(v32int16 a, v32int16 b) { return a | b; }

INTRINSIC(v32int16)
bneg_ltz(v32int16 a, unsigned int &cmp) // static sign
{
  v32int16 r = __builtin_aie2ps_vbneg_ltz16(a, cmp);
  return r;
}

INTRINSIC(v32int16)
bneg_ltz(v32int16 a, bool sgn, unsigned int &cmp) // dynamic sign
{
  v32int16 r = __builtin_aie2ps_vbneg_ltz16(a, cmp);
  if (!sgn)
    cmp = 0;
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
INTRINSIC(v32int16)
abs_gtz(v32int16 a, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vabs_gtz16(a, __SIGN_SIGNED, cmp);
}

INTRINSIC(v32int16)
abs_gtz(v32int16 a, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vabs_gtz16(a, (uint1_t)sgn, cmp);
}

INTRINSIC(v32int16)
abs(v32int16 a) // static sign
{
  unsigned int cmp;
  return abs_gtz(a, cmp);
}
INTRINSIC(v32int16)
abs(v32int16 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned int)
lt(v32int16 a, v32int16 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vlt16(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned int)
ge(v32int16 a, v32int16 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vge16(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned int) le(v32int16 a, v32int16 b) { return ge(b, a); }
INTRINSIC(unsigned int) gt(v32int16 a, v32int16 b) { return lt(b, a); }
INTRINSIC(unsigned int)
lt(v32int16 a, v32int16 b, bool sgn) // dynamic sign
{
  return (unsigned int)__builtin_aie2ps_vlt16(a, b, sgn);
}

INTRINSIC(unsigned int)
ge(v32int16 a, v32int16 b, bool sgn) // dynamic sign
{
  return (unsigned int)__builtin_aie2ps_vge16(a, b, sgn);
}

INTRINSIC(unsigned int) le(v32int16 a, v32int16 b, bool sgn) {
  return ge(b, a, sgn);
}
INTRINSIC(unsigned int) gt(v32int16 a, v32int16 b, bool sgn) {
  return lt(b, a, sgn);
}
INTRINSIC(unsigned int)
ltz(v32int16 a) // static sign
{
  unsigned int cmp;
  bneg_ltz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
ltz(v32int16 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
gtz(v32int16 a) // static sign
{
  unsigned int cmp;
  abs_gtz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
gtz(v32int16 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) eqz(v32int16 a) {
  return (unsigned int)__builtin_aie2ps_veqz16(a);
}

INTRINSIC(unsigned int) eq(v32int16 a, v32int16 b) { return eqz(sub(a, b)); }

INTRINSIC(unsigned int) ne(v32int16 a, v32int16 b) {
  return eq(a, b) ^ 0xffffffff;
}
INTRINSIC(v32int16) sel(v32int16 a, v32int16 b, unsigned int sel) {
  return __builtin_aie2ps_vsel16(a, b, sel);
}

// ------------------------------------------------------------

INTRINSIC(v32uint16) add(v32uint16 a, v32uint16 b) { return (a + b); }
INTRINSIC(v32uint16) sub(v32uint16 a, v32uint16 b) { return (a - b); }
INTRINSIC(v32uint16) addsub(v32uint16 a, v32uint16 b, unsigned int as) {
  return __builtin_aie2ps_vaddsub16(a, b, as);
}

INTRINSIC(v32uint16) neg_gtz(v32uint16 a, unsigned int &cmp) {
  return __builtin_aie2ps_vneg_gtz16(a, cmp);
}
INTRINSIC(v32uint16) neg(v32uint16 a) {
  unsigned int cmp;
  return __builtin_aie2ps_vneg_gtz16(a, cmp);
}

INTRINSIC(v32uint16)
sub_lt(v32uint16 a, v32uint16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vsub_lt16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
sub_lt(v32uint16 a, v32uint16 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16)
sub_ge(v32uint16 a, v32uint16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vsub_ge16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
sub_ge(v32uint16 a, v32uint16 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_ge16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16)
maxdiff_lt(v32uint16 a, v32uint16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmaxdiff_lt16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
maxdiff_lt(v32uint16 a, v32uint16 b, bool sgn,
           unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmaxdiff_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16)
maxdiff(v32uint16 a, v32uint16 b) // static sign
{
  unsigned int cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v32uint16)
maxdiff(v32uint16 a, v32uint16 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v32uint16)
min_ge(v32uint16 a, v32uint16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmin_ge16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
min_ge(v32uint16 a, v32uint16 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmin_ge16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16)
min(v32uint16 a, v32uint16 b) // static sign
{
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v32uint16)
min(v32uint16 a, v32uint16 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v32uint16)
max_lt(v32uint16 a, v32uint16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmax_lt16(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
max_lt(v32uint16 a, v32uint16 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmax_lt16(a, b, sgn, cmp);
}

INTRINSIC(v32uint16)
max(v32uint16 a, v32uint16 b) // static sign
{
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v32uint16)
max(v32uint16 a, v32uint16 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v32uint16) band(v32uint16 a, v32uint16 b) { return a & b; }

INTRINSIC(v32uint16) bor(v32uint16 a, v32uint16 b) { return a | b; }

INTRINSIC(v32uint16)
bneg_ltz(v32uint16 a, unsigned int &cmp) // static sign
{
  v32uint16 r = __builtin_aie2ps_vbneg_ltz16(a, cmp);
  cmp = 0;
  return r;
}

INTRINSIC(v32uint16)
bneg_ltz(v32uint16 a, bool sgn, unsigned int &cmp) // dynamic sign
{
  v32uint16 r = __builtin_aie2ps_vbneg_ltz16(a, cmp);
  if (!sgn)
    cmp = 0;
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
INTRINSIC(v32uint16)
abs_gtz(v32uint16 a, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vabs_gtz16(a, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v32uint16)
abs_gtz(v32uint16 a, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vabs_gtz16(a, (uint1_t)sgn, cmp);
}

INTRINSIC(v32uint16)
abs(v32uint16 a) // static sign
{
  return a;
}
INTRINSIC(v32uint16)
abs(v32uint16 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned int)
lt(v32uint16 a, v32uint16 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vlt16(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int)
ge(v32uint16 a, v32uint16 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vge16(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int) le(v32uint16 a, v32uint16 b) { return ge(b, a); }
INTRINSIC(unsigned int) gt(v32uint16 a, v32uint16 b) { return lt(b, a); }
INTRINSIC(unsigned int)
lt(v32uint16 a, v32uint16 b, bool sgn) // dynamic sign
{
  return (unsigned int)__builtin_aie2ps_vlt16(a, b, sgn);
}

INTRINSIC(unsigned int)
ge(v32uint16 a, v32uint16 b, bool sgn) // dynamic sign
{
  return (unsigned int)__builtin_aie2ps_vge16(a, b, sgn);
}

INTRINSIC(unsigned int) le(v32uint16 a, v32uint16 b, bool sgn) {
  return ge(b, a, sgn);
}
INTRINSIC(unsigned int) gt(v32uint16 a, v32uint16 b, bool sgn) {
  return lt(b, a, sgn);
}
INTRINSIC(unsigned int)
ltz(v32uint16 a) // static sign
{
  unsigned int cmp;
  bneg_ltz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
ltz(v32uint16 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
gtz(v32uint16 a) // static sign
{
  unsigned int cmp;
  abs_gtz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
gtz(v32uint16 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) eqz(v32uint16 a) {
  return (unsigned int)__builtin_aie2ps_veqz16(a);
}

INTRINSIC(unsigned int) eq(v32uint16 a, v32uint16 b) { return eqz(sub(a, b)); }

INTRINSIC(unsigned int) ne(v32uint16 a, v32uint16 b) {
  return eq(a, b) ^ 0xffffffff;
}
INTRINSIC(v32uint16) sel(v32uint16 a, v32uint16 b, unsigned int sel) {
  return __builtin_aie2ps_vsel16(a, b, sel);
}

// ------------------------------------------------------------

// ------------------------------------------------------------

INTRINSIC(v16int32) add(v16int32 a, v16int32 b) { return (a + b); }
INTRINSIC(v16int32) sub(v16int32 a, v16int32 b) { return (a - b); }
INTRINSIC(v16int32) addsub(v16int32 a, v16int32 b, unsigned int as) {
  return __builtin_aie2ps_vaddsub32(a, b, as);
}

INTRINSIC(v16int32) neg_gtz(v16int32 a, unsigned int &cmp) {
  return __builtin_aie2ps_vneg_gtz32(a, cmp);
}
INTRINSIC(v16int32) neg(v16int32 a) {
  unsigned int cmp;
  return __builtin_aie2ps_vneg_gtz32(a, cmp);
}

INTRINSIC(v16int32)
sub_lt(v16int32 a, v16int32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vsub_lt32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
sub_lt(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16int32)
sub_ge(v16int32 a, v16int32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vsub_ge32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
sub_ge(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_ge32(a, b, sgn, cmp);
}

INTRINSIC(v16int32)
maxdiff_lt(v16int32 a, v16int32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmaxdiff_lt32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
maxdiff_lt(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmaxdiff_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16int32)
maxdiff(v16int32 a, v16int32 b) // static sign
{
  unsigned int cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v16int32)
maxdiff(v16int32 a, v16int32 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v16int32)
min_ge(v16int32 a, v16int32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmin_ge32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
min_ge(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmin_ge32(a, b, sgn, cmp);
}

INTRINSIC(v16int32)
min(v16int32 a, v16int32 b) // static sign
{
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v16int32)
min(v16int32 a, v16int32 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v16int32)
max_lt(v16int32 a, v16int32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmax_lt32(a, b, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
max_lt(v16int32 a, v16int32 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmax_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16int32)
max(v16int32 a, v16int32 b) // static sign
{
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v16int32)
max(v16int32 a, v16int32 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v16int32) band(v16int32 a, v16int32 b) { return a & b; }

INTRINSIC(v16int32) bor(v16int32 a, v16int32 b) { return a | b; }

INTRINSIC(v16int32)
bneg_ltz(v16int32 a, unsigned int &cmp) // static sign
{
  v16int32 r = __builtin_aie2ps_vbneg_ltz32(a, cmp);
  return r;
}

INTRINSIC(v16int32)
bneg_ltz(v16int32 a, bool sgn, unsigned int &cmp) // dynamic sign
{
  v16int32 r = __builtin_aie2ps_vbneg_ltz32(a, cmp);
  if (!sgn)
    cmp = 0;
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
INTRINSIC(v16int32)
abs_gtz(v16int32 a, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vabs_gtz32(a, __SIGN_SIGNED, cmp);
}

INTRINSIC(v16int32)
abs_gtz(v16int32 a, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vabs_gtz32(a, (uint1_t)sgn, cmp);
}

INTRINSIC(v16int32)
abs(v16int32 a) // static sign
{
  unsigned int cmp;
  return abs_gtz(a, cmp);
}
INTRINSIC(v16int32)
abs(v16int32 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned int)
lt(v16int32 a, v16int32 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vlt32(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned int)
ge(v16int32 a, v16int32 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vge32(a, b, __SIGN_SIGNED);
}

INTRINSIC(unsigned int) le(v16int32 a, v16int32 b) { return ge(b, a); }
INTRINSIC(unsigned int) gt(v16int32 a, v16int32 b) { return lt(b, a); }
INTRINSIC(unsigned int)
lt(v16int32 a, v16int32 b, bool sgn) // dynamic sign
{
  return (unsigned int)__builtin_aie2ps_vlt32(a, b, sgn);
}

INTRINSIC(unsigned int)
ge(v16int32 a, v16int32 b, bool sgn) // dynamic sign
{
  return (unsigned int)__builtin_aie2ps_vge32(a, b, sgn);
}

INTRINSIC(unsigned int) le(v16int32 a, v16int32 b, bool sgn) {
  return ge(b, a, sgn);
}
INTRINSIC(unsigned int) gt(v16int32 a, v16int32 b, bool sgn) {
  return lt(b, a, sgn);
}
INTRINSIC(unsigned int)
ltz(v16int32 a) // static sign
{
  unsigned int cmp;
  bneg_ltz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
ltz(v16int32 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
gtz(v16int32 a) // static sign
{
  unsigned int cmp;
  abs_gtz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
gtz(v16int32 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) eqz(v16int32 a) {
  return (unsigned int)__builtin_aie2ps_veqz32(a);
}

INTRINSIC(unsigned int) eq(v16int32 a, v16int32 b) { return eqz(sub(a, b)); }

INTRINSIC(unsigned int) ne(v16int32 a, v16int32 b) { return eq(a, b) ^ 0xffff; }
INTRINSIC(v16int32) sel(v16int32 a, v16int32 b, unsigned int sel) {
  return __builtin_aie2ps_vsel32(a, b, sel);
}

// ------------------------------------------------------------

INTRINSIC(v16uint32) add(v16uint32 a, v16uint32 b) { return (a + b); }
INTRINSIC(v16uint32) sub(v16uint32 a, v16uint32 b) { return (a - b); }
INTRINSIC(v16uint32) addsub(v16uint32 a, v16uint32 b, unsigned int as) {
  return __builtin_aie2ps_vaddsub32(a, b, as);
}

INTRINSIC(v16uint32) neg_gtz(v16uint32 a, unsigned int &cmp) {
  return __builtin_aie2ps_vneg_gtz32(a, cmp);
}
INTRINSIC(v16uint32) neg(v16uint32 a) {
  unsigned int cmp;
  return __builtin_aie2ps_vneg_gtz32(a, cmp);
}

INTRINSIC(v16uint32)
sub_lt(v16uint32 a, v16uint32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vsub_lt32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
sub_lt(v16uint32 a, v16uint32 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32)
sub_ge(v16uint32 a, v16uint32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vsub_ge32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
sub_ge(v16uint32 a, v16uint32 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vsub_ge32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32)
maxdiff_lt(v16uint32 a, v16uint32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmaxdiff_lt32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
maxdiff_lt(v16uint32 a, v16uint32 b, bool sgn,
           unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmaxdiff_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32)
maxdiff(v16uint32 a, v16uint32 b) // static sign
{
  unsigned int cmp;
  return maxdiff_lt(a, b, cmp);
}

INTRINSIC(v16uint32)
maxdiff(v16uint32 a, v16uint32 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return maxdiff_lt(a, b, sgn, cmp);
}

INTRINSIC(v16uint32)
min_ge(v16uint32 a, v16uint32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmin_ge32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
min_ge(v16uint32 a, v16uint32 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmin_ge32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32)
min(v16uint32 a, v16uint32 b) // static sign
{
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v16uint32)
min(v16uint32 a, v16uint32 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return min_ge(a, b, sgn, cmp);
}

INTRINSIC(v16uint32)
max_lt(v16uint32 a, v16uint32 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmax_lt32(a, b, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
max_lt(v16uint32 a, v16uint32 b, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vmax_lt32(a, b, sgn, cmp);
}

INTRINSIC(v16uint32)
max(v16uint32 a, v16uint32 b) // static sign
{
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v16uint32)
max(v16uint32 a, v16uint32 b, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return max_lt(a, b, sgn, cmp);
}

INTRINSIC(v16uint32) band(v16uint32 a, v16uint32 b) { return a & b; }

INTRINSIC(v16uint32) bor(v16uint32 a, v16uint32 b) { return a | b; }

INTRINSIC(v16uint32)
bneg_ltz(v16uint32 a, unsigned int &cmp) // static sign
{
  v16uint32 r = __builtin_aie2ps_vbneg_ltz32(a, cmp);
  cmp = 0;
  return r;
}

INTRINSIC(v16uint32)
bneg_ltz(v16uint32 a, bool sgn, unsigned int &cmp) // dynamic sign
{
  v16uint32 r = __builtin_aie2ps_vbneg_ltz32(a, cmp);
  if (!sgn)
    cmp = 0;
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
INTRINSIC(v16uint32)
abs_gtz(v16uint32 a, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vabs_gtz32(a, __SIGN_UNSIGNED, cmp);
}

INTRINSIC(v16uint32)
abs_gtz(v16uint32 a, bool sgn, unsigned int &cmp) // dynamic sign
{
  return __builtin_aie2ps_vabs_gtz32(a, (uint1_t)sgn, cmp);
}

INTRINSIC(v16uint32)
abs(v16uint32 a) // static sign
{
  return a;
}
INTRINSIC(v16uint32)
abs(v16uint32 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  return abs_gtz(a, sgn, cmp);
}

INTRINSIC(unsigned int)
lt(v16uint32 a, v16uint32 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vlt32(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int)
ge(v16uint32 a, v16uint32 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vge32(a, b, __SIGN_UNSIGNED);
}

INTRINSIC(unsigned int) le(v16uint32 a, v16uint32 b) { return ge(b, a); }
INTRINSIC(unsigned int) gt(v16uint32 a, v16uint32 b) { return lt(b, a); }
INTRINSIC(unsigned int)
lt(v16uint32 a, v16uint32 b, bool sgn) // dynamic sign
{
  return (unsigned int)__builtin_aie2ps_vlt32(a, b, sgn);
}

INTRINSIC(unsigned int)
ge(v16uint32 a, v16uint32 b, bool sgn) // dynamic sign
{
  return (unsigned int)__builtin_aie2ps_vge32(a, b, sgn);
}

INTRINSIC(unsigned int) le(v16uint32 a, v16uint32 b, bool sgn) {
  return ge(b, a, sgn);
}
INTRINSIC(unsigned int) gt(v16uint32 a, v16uint32 b, bool sgn) {
  return lt(b, a, sgn);
}
INTRINSIC(unsigned int)
ltz(v16uint32 a) // static sign
{
  unsigned int cmp;
  bneg_ltz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
ltz(v16uint32 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  bneg_ltz(a, sgn, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
gtz(v16uint32 a) // static sign
{
  unsigned int cmp;
  abs_gtz(a, cmp);
  return cmp;
}
INTRINSIC(unsigned int)
gtz(v16uint32 a, bool sgn) // dynamic sign
{
  unsigned int cmp;
  abs_gtz(a, sgn, cmp);
  return cmp;
}

INTRINSIC(unsigned int) eqz(v16uint32 a) {
  return (unsigned int)__builtin_aie2ps_veqz32(a);
}

INTRINSIC(unsigned int) eq(v16uint32 a, v16uint32 b) { return eqz(sub(a, b)); }

INTRINSIC(unsigned int) ne(v16uint32 a, v16uint32 b) {
  return eq(a, b) ^ 0xffff;
}
INTRINSIC(v16uint32) sel(v16uint32 a, v16uint32 b, unsigned int sel) {
  return __builtin_aie2ps_vsel32(a, b, sel);
}

// ------------------------------------------------------------

INTRINSIC(v32bfloat16)
min_ge(v32bfloat16 a, v32bfloat16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmin_gebfloat16(a, b, cmp);
}

INTRINSIC(v32bfloat16)
min(v32bfloat16 a, v32bfloat16 b) // static sign
{
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v32bfloat16)
max_lt(v32bfloat16 a, v32bfloat16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmax_ltbfloat16(a, b, cmp);
}

INTRINSIC(v32bfloat16)
max(v32bfloat16 a, v32bfloat16 b) // static sign
{
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v32bfloat16) band(v32bfloat16 a, v32bfloat16 b) {
  return __builtin_bit_cast(v16int32, __builtin_bit_cast(v16int32, a) &
                                          __builtin_bit_cast(v16int32, b));
}

INTRINSIC(v32bfloat16) bor(v32bfloat16 a, v32bfloat16 b) {
  return __builtin_bit_cast(v16int32, __builtin_bit_cast(v16int32, a) |
                                          __builtin_bit_cast(v16int32, b));
}

INTRINSIC(v32bfloat16) bneg(v32bfloat16 a) {
  unsigned cmp;
  v32bfloat16 r = (v32bfloat16)bneg_ltz((v32int16)a, cmp);
  return r;
}

INTRINSIC(v32bfloat16) bxor(v32bfloat16 a, v32bfloat16 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}
INTRINSIC(v32bfloat16) abs(v32bfloat16 a) {
  return band(a, (v32bfloat16)broadcast_u16(0x7fff));
}

INTRINSIC(unsigned int)
lt(v32bfloat16 a, v32bfloat16 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vltbfloat16(a, b);
}

INTRINSIC(unsigned int)
ge(v32bfloat16 a, v32bfloat16 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vgebfloat16(a, b);
}

INTRINSIC(unsigned int) le(v32bfloat16 a, v32bfloat16 b) { return ge(b, a); }
INTRINSIC(unsigned int) gt(v32bfloat16 a, v32bfloat16 b) { return lt(b, a); }
INTRINSIC(unsigned int) ltz(v32bfloat16 a) {
  return lt(a, (v32bfloat16)broadcast_u16(0x8000)); // a < -0.0f
}
INTRINSIC(unsigned int) gtz(v32bfloat16 a) {
  return gt(a, (v32bfloat16)broadcast_u16(0x0000)); // a > +0.0f;
}

INTRINSIC(unsigned int) eqz(v32bfloat16 a) {
  return (unsigned int)__builtin_aie2ps_veqz16(abs(a));
}

INTRINSIC(unsigned int) eq(v32bfloat16 a, v32bfloat16 b) {
  return le(a, b) & ge(a, b);
}
INTRINSIC(unsigned int) ne(v32bfloat16 a, v32bfloat16 b) {
  return lt(a, b) | gt(a, b);
}
INTRINSIC(v32bfloat16) sel(v32bfloat16 a, v32bfloat16 b, unsigned int sel) {
  return __builtin_aie2ps_vsel16(a, b, sel);
}

// ------------------------------------------------------------

INTRINSIC(v32float16)
min_ge(v32float16 a, v32float16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmin_gefloat16(a, b, cmp);
}

INTRINSIC(v32float16)
min(v32float16 a, v32float16 b) // static sign
{
  unsigned int cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v32float16)
max_lt(v32float16 a, v32float16 b, unsigned int &cmp) // static sign
{
  return __builtin_aie2ps_vmax_ltfloat16(a, b, cmp);
}

INTRINSIC(v32float16)
max(v32float16 a, v32float16 b) // static sign
{
  unsigned int cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v32float16) band(v32float16 a, v32float16 b) {
  return __builtin_bit_cast(v16int32, __builtin_bit_cast(v16int32, a) &
                                          __builtin_bit_cast(v16int32, b));
}

INTRINSIC(v32float16) bor(v32float16 a, v32float16 b) {
  return __builtin_bit_cast(v16int32, __builtin_bit_cast(v16int32, a) |
                                          __builtin_bit_cast(v16int32, b));
}

INTRINSIC(v32float16) bneg(v32float16 a) {
  unsigned cmp;
  v32float16 r = (v32float16)bneg_ltz((v32int16)a, cmp);
  return r;
}

INTRINSIC(v32float16) bxor(v32float16 a, v32float16 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}
INTRINSIC(v32float16) abs(v32float16 a) {
  return band(a, (v32float16)broadcast_u16(0x7fff));
}

INTRINSIC(unsigned int)
lt(v32float16 a, v32float16 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vltfloat16(a, b);
}

INTRINSIC(unsigned int)
ge(v32float16 a, v32float16 b) // static sign
{
  return (unsigned int)__builtin_aie2ps_vgefloat16(a, b);
}

INTRINSIC(unsigned int) le(v32float16 a, v32float16 b) { return ge(b, a); }
INTRINSIC(unsigned int) gt(v32float16 a, v32float16 b) { return lt(b, a); }
INTRINSIC(unsigned int) ltz(v32float16 a) {
  return lt(a, (v32float16)broadcast_u16(0x8000)); // a < -0.0f
}
INTRINSIC(unsigned int) gtz(v32float16 a) {
  return gt(a, (v32float16)broadcast_u16(0x0000)); // a > +0.0f;
}

INTRINSIC(unsigned int) eqz(v32float16 a) {
  return (unsigned int)__builtin_aie2ps_veqz16(abs(a));
}

INTRINSIC(unsigned int) eq(v32float16 a, v32float16 b) {
  return le(a, b) & ge(a, b);
}
INTRINSIC(unsigned int) ne(v32float16 a, v32float16 b) {
  return lt(a, b) | gt(a, b);
}
INTRINSIC(v32float16) sel(v32float16 a, v32float16 b, unsigned int sel) {
  return __builtin_aie2ps_vsel16(a, b, sel);
}

// ------------------------------------------------------------

INTRINSIC(v64bfloat8)
min_ge(v64bfloat8 a, v64bfloat8 b, unsigned long long &cmp) // static sign
{
  v64bfloat8 result;
  result.data = __builtin_aie2ps_vmin_gebfloat8(a.data, b.data, cmp);
  return result;
}

INTRINSIC(v64bfloat8)
min(v64bfloat8 a, v64bfloat8 b) // static sign
{
  unsigned long long cmp;
  v64bfloat8 result;
  result = min_ge(a, b, cmp);
  return result;
}

INTRINSIC(v64bfloat8)
max_lt(v64bfloat8 a, v64bfloat8 b, unsigned long long &cmp) // static sign
{
  v64bfloat8 result;
  result.data = __builtin_aie2ps_vmax_ltbfloat8(a.data, b.data, cmp);
  return result;
}

INTRINSIC(v64bfloat8)
max(v64bfloat8 a, v64bfloat8 b) // static sign
{
  unsigned long long cmp;
  v64bfloat8 result;
  result = max_lt(a, b, cmp);
  return result;
}

INTRINSIC(v64bfloat8) band(v64bfloat8 a, v64bfloat8 b) {
  v64bfloat8 result;
  result.data = __builtin_bit_cast(v16int32, a.data & b.data);
  return result;
}

INTRINSIC(v64bfloat8) bor(v64bfloat8 a, v64bfloat8 b) {
  v64bfloat8 result;
  result.data = __builtin_bit_cast(v16int32, a.data | b.data);
  return result;
}

INTRINSIC(v64bfloat8) bneg(v64bfloat8 a) {
  unsigned cmp;
  v64bfloat8 r;
  r.data = bneg_ltz((v32int16)a.data, cmp);
  return r;
}

INTRINSIC(v64bfloat8) bxor(v64bfloat8 a, v64bfloat8 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}
INTRINSIC(v64bfloat8) abs(v64bfloat8 a) {
  v64bfloat8 result;
  result.data = broadcast_u8(0x7f);
  return band(a, result);
}

INTRINSIC(unsigned long long)
lt(v64bfloat8 a, v64bfloat8 b) // static sign
{
  return (unsigned long long)__builtin_aie2ps_vltbfloat8(a.data, b.data);
}

INTRINSIC(unsigned long long)
ge(v64bfloat8 a, v64bfloat8 b) // static sign
{
  return (unsigned long long)__builtin_aie2ps_vgebfloat8(a.data, b.data);
}

INTRINSIC(unsigned long long) le(v64bfloat8 a, v64bfloat8 b) {
  return ge(b, a);
}
INTRINSIC(unsigned long long) gt(v64bfloat8 a, v64bfloat8 b) {
  return lt(b, a);
}
INTRINSIC(unsigned long long) ltz(v64bfloat8 a) {
  v64bfloat8 result;
  result.data = broadcast_u8(0x80);
  return lt(a, result);
}
INTRINSIC(unsigned long long) gtz(v64bfloat8 a) {
  v64bfloat8 result;
  result.data = broadcast_u8(0x00);
  return gt(a, result);
}

INTRINSIC(unsigned long long) eqz(v64bfloat8 a) {
  v64bfloat8 result = abs(a);
  return (unsigned long long)__builtin_aie2ps_veqz8(result.data);
}

INTRINSIC(unsigned long long) eq(v64bfloat8 a, v64bfloat8 b) {
  return le(a, b) & ge(a, b);
}
INTRINSIC(unsigned long long) ne(v64bfloat8 a, v64bfloat8 b) {
  return lt(a, b) | gt(a, b);
}
INTRINSIC(v64bfloat8) sel(v64bfloat8 a, v64bfloat8 b, unsigned long long sel) {
  v64bfloat8 result;
  result.data = __builtin_aie2ps_vsel8(a.data, b.data, (v2int32)sel);
  return result;
}

// ------------------------------------------------------------

INTRINSIC(v64float8)
min_ge(v64float8 a, v64float8 b, unsigned long long &cmp) // static sign
{
  v64float8 result;
  result.data = __builtin_aie2ps_vmin_gefloat8(a.data, b.data, cmp);
  return result;
}

INTRINSIC(v64float8)
min(v64float8 a, v64float8 b) // static sign
{
  unsigned long long cmp;
  return min_ge(a, b, cmp);
}

INTRINSIC(v64float8)
max_lt(v64float8 a, v64float8 b, unsigned long long &cmp) // static sign
{
  v64float8 result;
  result.data = __builtin_aie2ps_vmax_ltfloat8(a.data, b.data, cmp);
  return result;
}

INTRINSIC(v64float8)
max(v64float8 a, v64float8 b) // static sign
{
  unsigned long long cmp;
  return max_lt(a, b, cmp);
}

INTRINSIC(v64float8) band(v64float8 a, v64float8 b) {
  v64float8 result;
  result.data = __builtin_bit_cast(v16int32, a.data & b.data);
  return result;
}

INTRINSIC(v64float8) bor(v64float8 a, v64float8 b) {
  v64float8 result;
  result.data = __builtin_bit_cast(v16int32, a.data | b.data);
  return result;
}

INTRINSIC(v64float8) bneg(v64float8 a) {
  unsigned cmp;
  v64float8 r;
  r.data = bneg_ltz((v32int16)a.data, cmp);
  return r;
}

INTRINSIC(v64float8) bxor(v64float8 a, v64float8 b) {
  return bor(band(a, bneg(b)), band(bneg(a), b));
}
INTRINSIC(v64float8) abs(v64float8 a) {
  v64float8 result;
  result.data = broadcast_u8(0x7f);
  return band(a, result);
}

INTRINSIC(unsigned long long)
lt(v64float8 a, v64float8 b) // static sign
{
  return (unsigned long long)__builtin_aie2ps_vltfloat8(a.data, b.data);
}

INTRINSIC(unsigned long long)
ge(v64float8 a, v64float8 b) // static sign
{
  return (unsigned long long)__builtin_aie2ps_vgefloat8(a.data, b.data);
}

INTRINSIC(unsigned long long) le(v64float8 a, v64float8 b) { return ge(b, a); }
INTRINSIC(unsigned long long) gt(v64float8 a, v64float8 b) { return lt(b, a); }
INTRINSIC(unsigned long long) ltz(v64float8 a) {
  v64float8 result;
  result.data = broadcast_u8(0x80);
  return lt(a, result);
}
INTRINSIC(unsigned long long) gtz(v64float8 a) {
  v64float8 result;
  result.data = broadcast_u8(0x00);
  return gt(a, result);
}

INTRINSIC(unsigned long long) eqz(v64float8 a) {
  v64float8 result = abs(a);
  return (unsigned long long)__builtin_aie2ps_veqz8(result.data);
}

INTRINSIC(unsigned long long) eq(v64float8 a, v64float8 b) {
  return le(a, b) & ge(a, b);
}
INTRINSIC(unsigned long long) ne(v64float8 a, v64float8 b) {
  return lt(a, b) | gt(a, b);
}
INTRINSIC(v64float8) sel(v64float8 a, v64float8 b, unsigned long long sel) {
  v64float8 result;
  result.data = __builtin_aie2ps_vsel8(a.data, b.data, (v2int32)sel);
  return result;
}

// ------------------------------------------------------------
INTRINSIC(v16float) sel(v16float a, v16float b, unsigned int sel) {
  return (v16float)__builtin_aie2ps_vsel32(a, b, sel);
}

INTRINSIC(unsigned int) lt(v16float v1, v16float v2) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16(sub((v16accfloat)v1, (v16accfloat)v2)));
  return lt(a, b);
}

INTRINSIC(v16float) max_lt(v16float v1, v16float v2, unsigned int &cmp) {
  unsigned int res_cmp = lt(v1, v2);
  cmp = res_cmp;
  return sel(v1, v2, res_cmp);
}

INTRINSIC(v16float) max(v16float v1, v16float v2) {
  unsigned int res_cmp = lt(v1, v2);
  return sel(v1, v2, res_cmp);
}

INTRINSIC(unsigned int) ge(v16float v1, v16float v2) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16(sub((v16accfloat)v1, (v16accfloat)v2)));
  return (ge(a, b)) & 0x0000FFFF;
}

INTRINSIC(v16float) min_ge(v16float v1, v16float v2, unsigned int &cmp) {
  unsigned int res_cmp = ge(v1, v2);
  cmp = res_cmp;
  return sel(v1, v2, res_cmp);
}

INTRINSIC(v16float) min(v16float v1, v16float v2) {
  unsigned int res_cmp = ge(v1, v2);
  return sel(v1, v2, res_cmp);
}

INTRINSIC(unsigned int) gt(v16float v1, v16float v2) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  a = insert(a, 0, to_v16bfloat16(sub((v16accfloat)v1, (v16accfloat)v2)));
  return gt(a, b);
}

INTRINSIC(v16float) abs(v16float v1) {
  return (v16float)band((v16uint32)v1,
                        broadcast_to_v16uint32((unsigned)2147483647));
}

INTRINSIC(v16float) min_abs(v16float v1, v16float v2) {
  unsigned int res_cmp = ge(v1, (v16float)abs(v2));
  return (v16float)sel((v16int32)v1, (v16int32)v2, res_cmp);
}

INTRINSIC(v16float) max_abs(v16float v1, v16float v2) {
  unsigned int res_cmp = lt(v1, (v16float)abs(v2));
  return (v16float)sel((v16int32)v1, (v16int32)v2, res_cmp);
}

INTRINSIC(unsigned int) ge_abs(v16float v1, v16float v2) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v16float v3 = (v16float)abs(v2);
  a = insert(a, 0, to_v16bfloat16(sub((v16accfloat)v1, (v16accfloat)v3)));
  return (ge(a, b)) & 0x0000FFFF;
}

INTRINSIC(unsigned int) lt_abs(v16float v1, v16float v2) {
  v32bfloat16 a = broadcast_zero_to_v32bfloat16();
  v32bfloat16 b = broadcast_zero_to_v32bfloat16();
  v16float v3 = (v16float)abs(v2);
  a = insert(a, 0, to_v16bfloat16(sub((v16accfloat)v1, (v16accfloat)v3)));
  return lt(a, b);
}

// me_float_to_int.h header - added here since that header has just one
// intrinsic
INTRINSIC(v16int32) bfloat16_to_int(v16bfloat16 a, int shft) {
  return __builtin_aie2ps_v16bf16_to_v16i32(a, shft);
}

#endif // __AIE2PS_VADD_H__
