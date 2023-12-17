//===- aiev2_addr.h ---------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_ADDR_H__
#define __AIEV2_ADDR_H__

template <typename T> INTRINSIC(T *) byte_incr(T *a, int i) {
  return (T *)(((char *)a) + i);
}

struct dims_2d_t {
  // Represents the number of element on wich you still need to jump on when you
  // are at the beginning of a 1D sequence.
  unsigned int num1;
  // The offset between where you point and the next element on which you want
  // to point. This can represent bytes or offset in index.
  int inc1;
  // The offset applied once you made the 1D sequence. This can represent bytes
  // or offset in index.
  int inc2;
  // Represents the same as @ref num1, but it acts as a counter for the
  // intrinsic. This is useful when iterating.
  addr_t count1;

  dims_2d_t(unsigned int size1, int inc1, int inc2)
      : num1(size1), inc1(inc1), inc2(inc2), count1(0){};
  dims_2d_t(unsigned int size1, int inc1, int inc2, addr_t count1)
      : num1(size1), inc1(inc1), inc2(inc2), count1(count1){};
};

INTRINSIC(dims_2d_t)
dims_2d_from_steps(unsigned int size1, int step1, int step2) {
  return dims_2d_t(size1 - 1, step1, -((size1 - 1) * step1) + step2);
};

INTRINSIC(dims_2d_t)
dims_2d_from_steps(unsigned int size1, int step1, int step2, addr_t count1) {
  return dims_2d_t(size1 - 1, step1, -((size1 - 1) * step1) + step2, count1);
};

template <typename T>
INTRINSIC(T *)
add_2d_ptr(T *a, int off, int size1, int &count1, int inc1) {
  return (T *)__builtin_aiev2_add_2d((void *)a, (off * sizeof(T)),
                                     (inc1 * sizeof(T)), size1, count1);
}

template <typename T>
INTRINSIC(T *)
add_2d_byte(T *a, int off, int size1, int &count1, int inc1) {
  return (T *)__builtin_aiev2_add_2d((void *)a, off, inc1, size1, count1);
}

template <typename T>
INTRINSIC(T *)
add_3d_ptr(T *a, int off, int size1, int &count1, int inc1, int size2,
           int &count2, int inc2) {
  return (T *)__builtin_aiev2_add_3d((void *)a, (off * sizeof(T)),
                                     (inc1 * sizeof(T)), (inc2 * sizeof(T)),
                                     size1, count1, size2, count2);
}

template <typename T>
INTRINSIC(T *)
add_3d_byte(T *a, int off, int size1, int &count1, int inc1, int size2,
            int &count2, int inc2) {
  return (T *)__builtin_aiev2_add_3d((void *)a, off, inc1, inc2, size1, count1,
                                     size2, count2);
}

struct dims_3d_t {
  // The number of elements in your 1D sequence.
  unsigned int num1;
  // The offset between where you point and the next element on which you want
  // to point, forming the 1D sequence. This can represent bytes or the
  // increment in index.
  int inc1;
  // The number of 2D sequences you want to make on in your structure.
  unsigned int num2;
  // The offset between the last element in the 1D sequence and the first of the
  // next sequence, forming the 2D sequence. This can represent bytes or the
  // increment in index.
  int inc2;
  // The offset between the last element of your 2D sequence and the first of
  // the next, forming the 3D sequence. This can represent bytes or offset in
  // index.
  int inc3;
  // Once you have iterated count1_in times over your structure the next call
  // will point you on the element inc2 further. It will be decremented by inc1
  // on each call, once it reaches 0 the next call will point you on the element
  // size1 further. It is a reference so that the intrinsic can keep up on where
  // you are in your structure.
  addr_t count1;
  // Once you have made count2_in 1D sequences on your array the next call will
  // point you on the element off bytes further. Same. It will be decremented
  // each time you make a 1D sequence. Once count1_out and count2_out reach 0,
  // on the next call the output will point on the element off bytes further in
  // your structure. It is a reference so that the intrinsic can keep up on
  // where you are in your structure.
  addr_t count2;

  dims_3d_t(unsigned int size1, int inc1, unsigned int size2, int inc2,
            int inc3)
      : num1(size1), inc1(inc1), num2(size2), inc2(inc2), inc3(inc3), count1(0),
        count2(0){};
  dims_3d_t(unsigned int size1, int inc1, unsigned int size2, int inc2,
            int inc3, addr_t count1, addr_t count2)
      : num1(size1), inc1(inc1), num2(size2), inc2(inc2), inc3(inc3),
        count1(count1), count2(count2){};
};

INTRINSIC(dims_3d_t)
dims_3d_from_steps(unsigned int size1, int step1, unsigned int size2, int step2,
                   int step3) {
  return dims_3d_t(size1 - 1, step1, size2 - 1, -((size1 - 1) * step1) + step2,
                   -(((size2 - 1) * step2) + ((size1 - 1) * step1)) + step3);
};

INTRINSIC(dims_3d_t)
dims_3d_from_steps(unsigned int size1, int step1, unsigned int size2, int step2,
                   int step3, addr_t count1, addr_t count2) {
  return dims_3d_t(size1 - 1, step1, size2 - 1, -((size1 - 1) * step1) + step2,
                   -(((size2 - 1) * step2) + ((size1 - 1) * step1)) + step3,
                   count1, count2);
};

INTRINSIC(v8int32) load_4x16_lo(v8int32 addr) {
  return __builtin_aiev2_load_4x16_lo(addr);
}
INTRINSIC(v8int32) load_4x16_hi(v8int32 addr) {
  return __builtin_aiev2_load_4x16_hi(addr);
}
INTRINSIC(v8int32) load_4x32_lo(v8int32 addr) {
  return __builtin_aiev2_load_4x32_lo(addr);
}
INTRINSIC(v8int32) load_4x32_hi(v8int32 addr) {
  return __builtin_aiev2_load_4x32_hi(addr);
}
INTRINSIC(v8int32) load_4x64_lo(v8int32 addr) {
  return __builtin_aiev2_load_4x64_lo(addr);
}
INTRINSIC(v8int32) load_4x64_hi(v8int32 addr) {
  return __builtin_aiev2_load_4x64_hi(addr);
}

INTRINSIC(v16int32)
lut_pointer(const void *lut1, const void *lut2, v16int32 offset) {
  v16int32 base =
      sel(broadcast_s32((long)lut1), broadcast_s32((long)lut2), 0xCCCC);
  v16int32 ptr = base + offset;
  return ptr;
}

INTRINSIC(v16uint32)
lut_pointer(const void *lut1, const void *lut2, v16uint32 offset) {
  v16uint32 base =
      sel(broadcast_u32((long)lut1), broadcast_u32((long)lut2), 0xCCCC);
  v16uint32 ptr = base + offset;
  return ptr;
}

#define _LUT(pos, hilo, n, N, N2, W, W2)                                       \
  INTRINSIC(v##N2##int##W2)                                                    \
  read_lut##W##_##pos(const void *lut1, const void *lut2, v16int32 offset) {   \
    v16int32 ptr = lut_pointer(lut1, lut2, offset);                            \
    return load_4x##W##_##hilo(extract_v8int32(ptr, n));                       \
  }                                                                            \
  INTRINSIC(v##N2##int##W2)                                                    \
  read_lut##W##_##pos(const void *lut1, const void *lut2, v16uint32 offset) {  \
    v16uint32 ptr = lut_pointer(lut1, lut2, offset);                           \
    return load_4x##W##_##hilo(extract_v8uint32(ptr, n));                      \
  }

_LUT(0, lo, 0, 64, 32, 16, 8)
_LUT(1, hi, 0, 64, 32, 16, 8)
_LUT(2, lo, 1, 64, 32, 16, 8)
_LUT(3, hi, 1, 64, 32, 16, 8)

_LUT(0, lo, 0, 32, 16, 32, 16)
_LUT(1, hi, 0, 32, 16, 32, 16)
_LUT(2, lo, 1, 32, 16, 32, 16)
_LUT(3, hi, 1, 32, 16, 32, 16)

_LUT(0, lo, 0, 16, 8, 64, 32)
_LUT(1, hi, 0, 16, 8, 64, 32)
_LUT(2, lo, 1, 16, 8, 64, 32)
_LUT(3, hi, 1, 16, 8, 64, 32)

#undef _LUT

#define _LOAD_LUT_DEFS(lanes, type, off_type, func)                            \
  INTRINSIC(void)                                                              \
  load_lut_##type(const void *lut1, const void *lut2, off_type offset,         \
                  v##lanes##type &v1) {                                        \
    v1 = insert(v1, 0, read_##func##_0(lut1, lut2, offset));                   \
    v1 = insert(v1, 1, read_##func##_1(lut1, lut2, offset));                   \
  }                                                                            \
  INTRINSIC(void)                                                              \
  load_lut_2x_##type(const void *lut1, const void *lut2, off_type offset,      \
                     v##lanes##type &v1, v##lanes##type &v2) {                 \
    v1 = insert(v1, 0, read_##func##_0(lut1, lut2, offset));                   \
    v1 = insert(v1, 1, read_##func##_1(lut1, lut2, offset));                   \
    v2 = insert(v2, 0, read_##func##_2(lut1, lut2, offset));                   \
    v2 = insert(v2, 1, read_##func##_3(lut1, lut2, offset));                   \
  }

_LOAD_LUT_DEFS(16, int32, v16int32, lut64)
_LOAD_LUT_DEFS(32, int16, v16int32, lut32)
_LOAD_LUT_DEFS(64, int8, v16int32, lut16)
_LOAD_LUT_DEFS(16, int32, v16uint32, lut64)
_LOAD_LUT_DEFS(32, int16, v16uint32, lut32)
_LOAD_LUT_DEFS(64, int8, v16uint32, lut16)

#undef _LOAD_LUT_DEFS

INTRINSIC(void)
load_lut_float(const void *lut1, const void *lut2, v16int32 offset,
               v32bfloat16 &v1) {
  v1 =
      (v32bfloat16)insert(v1, 0, (v16bfloat16)read_lut64_0(lut1, lut2, offset));
  v1 =
      (v32bfloat16)insert(v1, 1, (v16bfloat16)read_lut64_1(lut1, lut2, offset));
}

INTRINSIC(void)
load_lut_2x_float(const void *lut1, const void *lut2, v16int32 offset,
                  v32bfloat16 &v1, v32bfloat16 &v2) {
  v1 =
      (v32bfloat16)insert(v1, 0, (v16bfloat16)read_lut64_0(lut1, lut2, offset));
  v1 =
      (v32bfloat16)insert(v1, 1, (v16bfloat16)read_lut64_1(lut1, lut2, offset));
  v2 =
      (v32bfloat16)insert(v2, 0, (v16bfloat16)read_lut64_2(lut1, lut2, offset));
  v2 =
      (v32bfloat16)insert(v2, 1, (v16bfloat16)read_lut64_3(lut1, lut2, offset));
}

INTRINSIC(void)
load_lut_float(const void *lut1, const void *lut2, v16uint32 offset,
               v32bfloat16 &v1) {
  v1 =
      (v32bfloat16)insert(v1, 0, (v16bfloat16)read_lut64_0(lut1, lut2, offset));
  v1 =
      (v32bfloat16)insert(v1, 1, (v16bfloat16)read_lut64_1(lut1, lut2, offset));
}

INTRINSIC(void)
load_lut_2x_float(const void *lut1, const void *lut2, v16uint32 offset,
                  v32bfloat16 &v1, v32bfloat16 &v2) {
  v1 =
      (v32bfloat16)insert(v1, 0, (v16bfloat16)read_lut64_0(lut1, lut2, offset));
  v1 =
      (v32bfloat16)insert(v1, 1, (v16bfloat16)read_lut64_1(lut1, lut2, offset));
  v2 =
      (v32bfloat16)insert(v2, 0, (v16bfloat16)read_lut64_2(lut1, lut2, offset));
  v2 =
      (v32bfloat16)insert(v2, 1, (v16bfloat16)read_lut64_3(lut1, lut2, offset));
}

#endif /*__AIEV2_ADDR_H__*/
