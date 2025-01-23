//===- aie2p_defines.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2P_DEFINES_H__
#define __AIE2P_DEFINES_H__

#define __AIE2P__
#define __AIE_ARCH__ 21
#define __AIE_ARCH_NAME__ AIE2P
#define __AIE_ARCH_NAME_STR__ "AIE2P"
// Natural alignment of vectors
#define AIE_VECTOR_LDST_ALIGN 64

// scalar compare
#define cmp_seq 0
#define cmp_sne 1
#define cmp_sge 2
#define cmp_slt 3
#define cmp_sgeu 6
#define cmp_sltu 7

// Rounding Modes

// No rounding - Truncate LSB, always round down (towards negative
//! infinity)
#define rnd_floor 0
// No rounding - Always round up (towards positive infinity)
#define rnd_ceil 1
// No rounding - Truncate LSB, always round towards 0
#define rnd_sym_floor 2
// No rounding - Always round up towards infinity
#define rnd_sym_ceil 3
// Round halfway towards negative infinity
#define rnd_neg_inf 8
// Round halfway towards positive infinity
#define rnd_pos_inf 9
// Round halfway towards zero (away from infinity)
#define rnd_sym_zero 10
// Round halfway towards infinity (away from zero)
#define rnd_sym_inf 11
// Round halfway towards nearest even number
#define rnd_conv_even 12
// Round halfway towards nearest odd number
#define rnd_conv_odd 13

#define crSat 0
#define crRnd 1
#define crFPMask 2
#define crF2IMask 3
#define crF2FMask 4
#define crF2BMask 5
#define crSRSMode 6
#define crUPSMode 7
#define crUnpackSize 8
#define crPackSize 9

#define srsSign0 10
#define srsSign1 11
#define upsSign0 12
#define upsSign1 13
#define packSign0 14
#define packSign1 15
#define unpackSign0 16
#define unpackSign1 17
#define vaddSign0 18
#define vaddSign1 19

#define crSCDEn 20
#define crMCDEn 21

#define crFPNlfMask 22
#define crFPCnvFx2FlMask 23
#define crFPCnvFl2FxMask 24

#define srCarry 25
#define srSS0 26
#define srMS0 27
#define srSRS_of 28
#define srUPS_of 29
#define srSparse_of 30
#define srFifo_of 31
#define srFifo_uf 32
#define srFPFlags 33
#define srF2IFlags 34
#define srF2FFlags 35
#define srF2BFlags 36
#define srFPNlf 37
#define srFPCnvFx2Fl 38
#define srFPCnvFl2Fx 39
// Term negation used in standard complex operations
#define OP_TERM_NEG_COMPLEX 0x0A
// Term negation used in complex operations with real part conjugate
#define OP_TERM_NEG_COMPLEX_CONJUGATE_X 0xA0
// Term negation used in complex operations with imaginary part conjugate
#define OP_TERM_NEG_COMPLEX_CONJUGATE_Y 0x50
// Term negation used in complex operations with real and imaginary parts
// conjugate
#define OP_TERM_NEG_COMPLEX_CONJUGATE_X_Y 0xFA
// Term negation used in butterfly complex with real and imaginary parts
// conjugate operations (only for complex 16-bit * 16-bit)
#define OP_TERM_NEG_COMPLEX_CONJUGATE_BUTTERFLY 0xC6
// Term negation used in butterfly complex operations (only for complex 16-bit *
// 16-bit)
#define OP_TERM_NEG_COMPLEX_BUTTERFLY 0x9C

// ups-size
#define ups_shrt 0
#define ups_long 1
// assign high part of UPS
#define ups_nohi 0
#define ups_dohi 1

enum class aie_dm_resource {
  none,
  PM,
  DM,
  DM_test,
  stack,
  a,
  b,
  c,
  d,
  ab,
  ac,
  ad,
  bc,
  bd,
  cd,
  TM
};

#define AIE_DM_RESOURCE(X)                                                     \
  __attribute__((address_space(static_cast<signed>(aie_dm_resource::X))))
#define __aie_dm_resource_stack AIE_DM_RESOURCE(stack)
#define __aie_dm_resource_a AIE_DM_RESOURCE(a)
#define __aie_dm_resource_b AIE_DM_RESOURCE(b)
#define __aie_dm_resource_c AIE_DM_RESOURCE(c)
#define __aie_dm_resource_d AIE_DM_RESOURCE(d)
#define __aie_dm_resource_ab AIE_DM_RESOURCE(ab)
#define __aie_dm_resource_ac AIE_DM_RESOURCE(ac)
#define __aie_dm_resource_ad AIE_DM_RESOURCE(ad)
#define __aie_dm_resource_bc AIE_DM_RESOURCE(bc)
#define __aie_dm_resource_bd AIE_DM_RESOURCE(bd)
#define __aie_dm_resource_cd AIE_DM_RESOURCE(cd)
#define __aie_dm_resource_TM AIE_DM_RESOURCE(TM)

template <typename T> struct aie_dm_resource_remove {
  using type = T;
};

template <typename T, int N>
struct aie_dm_resource_remove<T __attribute__((address_space(N)))> {
  using type = T;
};
template <typename T, int N>
struct aie_dm_resource_remove<const T __attribute__((address_space(N)))> {
  using type = const T;
};
template <typename T, int N>
struct aie_dm_resource_remove<volatile T __attribute__((address_space(N)))> {
  using type = T;
};
template <typename T, int N>
struct aie_dm_resource_remove<const volatile T
                              __attribute__((address_space(N)))> {
  using type = const T;
};

template <typename T>
using aie_dm_resource_remove_t = typename aie_dm_resource_remove<T>::type;

template <typename T, aie_dm_resource Resource> struct aie_dm_resource_set {
  using type = T;
};

template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::a> {
  using type = T __aie_dm_resource_a;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::b> {
  using type = T __aie_dm_resource_b;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::c> {
  using type = T __aie_dm_resource_c;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::d> {
  using type = T __aie_dm_resource_d;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::stack> {
  using type = T __aie_dm_resource_stack;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::ab> {
  using type = T __aie_dm_resource_ab;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::ac> {
  using type = T __aie_dm_resource_ac;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::ad> {
  using type = T __aie_dm_resource_ad;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::bc> {
  using type = T __aie_dm_resource_bc;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::bd> {
  using type = T __aie_dm_resource_bd;
};
template <typename T> struct aie_dm_resource_set<T, aie_dm_resource::cd> {
  using type = T __aie_dm_resource_cd;
};

template <typename T, aie_dm_resource Resource>
using aie_dm_resource_set_t =
    typename aie_dm_resource_set<aie_dm_resource_remove_t<T>, Resource>::type;

template <typename T> struct aie_dm_resource_get {
  static constexpr aie_dm_resource value = aie_dm_resource::none;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_a> {
  static constexpr aie_dm_resource value = aie_dm_resource::a;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_a> {
  static constexpr aie_dm_resource value = aie_dm_resource::a;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_a> {
  static constexpr aie_dm_resource value = aie_dm_resource::a;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_a> {
  static constexpr aie_dm_resource value = aie_dm_resource::a;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_b> {
  static constexpr aie_dm_resource value = aie_dm_resource::b;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_b> {
  static constexpr aie_dm_resource value = aie_dm_resource::b;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_b> {
  static constexpr aie_dm_resource value = aie_dm_resource::b;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_b> {
  static constexpr aie_dm_resource value = aie_dm_resource::b;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_c> {
  static constexpr aie_dm_resource value = aie_dm_resource::c;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_c> {
  static constexpr aie_dm_resource value = aie_dm_resource::c;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_c> {
  static constexpr aie_dm_resource value = aie_dm_resource::c;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_c> {
  static constexpr aie_dm_resource value = aie_dm_resource::c;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_d> {
  static constexpr aie_dm_resource value = aie_dm_resource::d;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_d> {
  static constexpr aie_dm_resource value = aie_dm_resource::d;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_d> {
  static constexpr aie_dm_resource value = aie_dm_resource::d;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_d> {
  static constexpr aie_dm_resource value = aie_dm_resource::d;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_stack> {
  static constexpr aie_dm_resource value = aie_dm_resource::stack;
};
template <typename T>
struct aie_dm_resource_get<const T __aie_dm_resource_stack> {
  static constexpr aie_dm_resource value = aie_dm_resource::stack;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_stack> {
  static constexpr aie_dm_resource value = aie_dm_resource::stack;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_stack> {
  static constexpr aie_dm_resource value = aie_dm_resource::stack;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_ab> {
  static constexpr aie_dm_resource value = aie_dm_resource::ab;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_ab> {
  static constexpr aie_dm_resource value = aie_dm_resource::ab;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_ab> {
  static constexpr aie_dm_resource value = aie_dm_resource::ab;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_ab> {
  static constexpr aie_dm_resource value = aie_dm_resource::ab;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_ac> {
  static constexpr aie_dm_resource value = aie_dm_resource::ac;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_ac> {
  static constexpr aie_dm_resource value = aie_dm_resource::ac;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_ac> {
  static constexpr aie_dm_resource value = aie_dm_resource::ac;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_ac> {
  static constexpr aie_dm_resource value = aie_dm_resource::ac;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_ad> {
  static constexpr aie_dm_resource value = aie_dm_resource::ad;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_ad> {
  static constexpr aie_dm_resource value = aie_dm_resource::ad;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_ad> {
  static constexpr aie_dm_resource value = aie_dm_resource::ad;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_ad> {
  static constexpr aie_dm_resource value = aie_dm_resource::ad;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_bc> {
  static constexpr aie_dm_resource value = aie_dm_resource::bc;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_bc> {
  static constexpr aie_dm_resource value = aie_dm_resource::bc;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_bc> {
  static constexpr aie_dm_resource value = aie_dm_resource::bc;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_bc> {
  static constexpr aie_dm_resource value = aie_dm_resource::bc;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_bd> {
  static constexpr aie_dm_resource value = aie_dm_resource::bd;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_bd> {
  static constexpr aie_dm_resource value = aie_dm_resource::bd;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_bd> {
  static constexpr aie_dm_resource value = aie_dm_resource::bd;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_bd> {
  static constexpr aie_dm_resource value = aie_dm_resource::bd;
};

template <typename T> struct aie_dm_resource_get<T __aie_dm_resource_cd> {
  static constexpr aie_dm_resource value = aie_dm_resource::cd;
};
template <typename T> struct aie_dm_resource_get<const T __aie_dm_resource_cd> {
  static constexpr aie_dm_resource value = aie_dm_resource::cd;
};
template <typename T>
struct aie_dm_resource_get<volatile T __aie_dm_resource_cd> {
  static constexpr aie_dm_resource value = aie_dm_resource::cd;
};
template <typename T>
struct aie_dm_resource_get<const volatile T __aie_dm_resource_cd> {
  static constexpr aie_dm_resource value = aie_dm_resource::cd;
};

template <typename T>
static constexpr aie_dm_resource aie_dm_resource_get_v =
    aie_dm_resource_get<T>::value;

template <typename T, aie_dm_resource Resource>
static constexpr bool aie_dm_resource_is_same_v =
    (Resource == aie_dm_resource_get_v<T>);

enum class aie_stream_resource_in { none, a, b };
enum class aie_stream_resource_out { none, a, b };

#define __aie_stream_resource_in_a
#define __aie_stream_resource_in_b
#define __aie_stream_resource_out_a
#define __aie_stream_resource_out_b

template <typename T> struct aie_stream_resource_remove {
  using type = T;
};

template <typename T, int N>
struct aie_stream_resource_remove<T __attribute__((address_space(N)))> {
  using type = T;
};
template <typename T, int N>
struct aie_stream_resource_remove<const T __attribute__((address_space(N)))> {
  using type = const T;
};
template <typename T, int N>
struct aie_stream_resource_remove<volatile T
                                  __attribute__((address_space(N)))> {
  using type = T;
};
template <typename T, int N>
struct aie_stream_resource_remove<const volatile T
                                  __attribute__((address_space(N)))> {
  using type = const T;
};

template <typename T>
using aie_stream_resource_remove_t =
    typename aie_stream_resource_remove<T>::type;

template <typename T, aie_stream_resource_in Resource>
struct aie_stream_resource_in_set {
  using type = T;
};

template <typename T, aie_stream_resource_out Resource>
struct aie_stream_resource_out_set {
  using type = T;
};

template <typename T, aie_stream_resource_in Resource>
using aie_stream_resource_in_set_t =
    typename aie_stream_resource_in_set<aie_stream_resource_remove_t<T>,
                                        Resource>::type;

template <typename T, aie_stream_resource_out Resource>
using aie_stream_resource_out_set_t =
    typename aie_stream_resource_out_set<aie_stream_resource_remove_t<T>,
                                         Resource>::type;

template <typename T> struct aie_stream_resource_in_get {
  static constexpr aie_stream_resource_in value = aie_stream_resource_in::none;
};

template <typename T> struct aie_stream_resource_out_get {
  static constexpr aie_stream_resource_out value =
      aie_stream_resource_out::none;
};

template <typename T>
static constexpr aie_stream_resource_in aie_stream_resource_in_get_v =
    aie_stream_resource_in_get<T>::value;

template <typename T>
static constexpr aie_stream_resource_out aie_stream_resource_out_get_v =
    aie_stream_resource_out_get<T>::value;

template <typename T, aie_stream_resource_in Resource>
static constexpr bool aie_stream_resource_in_is_same_v =
    (Resource == aie_stream_resource_in_get_v<T>);

template <typename T, aie_stream_resource_out Resource>
static constexpr bool aie_stream_resource_out_is_same_v =
    (Resource == aie_stream_resource_out_get_v<T>);

#endif // __AIE2P_DEFINES_H__
