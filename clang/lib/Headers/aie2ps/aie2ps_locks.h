//===- aie2ps_locks.h -------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===---------------------------------------------------------------------===//

#ifndef __AIE2PS_LOCKS_H__
#define __AIE2PS_LOCKS_H__

// Acquire and release instructions
INTRINSIC(void)
acquire_equal(unsigned id, unsigned val) {
  return __builtin_aie2ps_acquire(id, val);
}
INTRINSIC(void)
acquire_equal(unsigned id, unsigned val, int cond) {
  return __builtin_aie2ps_acquire_cond(id, val, cond);
}
INTRINSIC(void)
acquire_greater_equal(unsigned id, unsigned val) {
  return __builtin_aie2ps_acquire(id, -val);
}
INTRINSIC(void)
acquire_greater_equal(unsigned id, unsigned val, int cond) {
  return __builtin_aie2ps_acquire_cond(id, -val, cond);
}
INTRINSIC(void)
release(unsigned id, int val) { return __builtin_aie2ps_release(id, val); }
INTRINSIC(void)
release(unsigned id, int val, int cond) {
  return __builtin_aie2ps_release_cond(id, val, cond);
}
// Acquire and release instructions considering a memory pointer
INTRINSIC(void)
acquire_equal(const void *a, unsigned id, unsigned val) {
  return __builtin_aie2ps_acquire_ptr(a, id, val);
}
INTRINSIC(void)
acquire_equal(const void *a, unsigned id, unsigned val, int cond) {
  return __builtin_aie2ps_acquire_cond_ptr(a, id, val, cond);
}
INTRINSIC(void)
acquire_greater_equal(const void *a, unsigned id, unsigned val) {
  return __builtin_aie2ps_acquire_ptr(a, id, -val);
}
INTRINSIC(void)
acquire_greater_equal(const void *a, unsigned id, unsigned val, int cond) {
  return __builtin_aie2ps_acquire_cond_ptr(a, id, -val, cond);
}
INTRINSIC(void)
release(void *a, unsigned id, int val) {
  return __builtin_aie2ps_release_ptr(a, id, val);
}
INTRINSIC(void)
release(void *a, unsigned id, int val, int cond) {
  return __builtin_aie2ps_release_cond_ptr(a, id, val, cond);
}
// Disable the core
INTRINSIC(void)
done(void) {
  __builtin_aie2ps_sched_barrier();
  __builtin_aie2ps_done();
  __builtin_aie2ps_sched_barrier();
}
// TODO Acquire and release instructions considering a memory pointer in TM
#endif // __AIE2PS_LOCKS_H__
