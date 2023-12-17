//===- aiev2_locks.h --------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIEV2_LOCKS_H__
#define __AIEV2_LOCKS_H__

INTRINSIC(void)
acquire_equal(unsigned id, unsigned val) {
  return __builtin_aiev2_acquire(id, val);
}
INTRINSIC(void)
acquire_equal(unsigned id, unsigned val, int cond) {
  return __builtin_aiev2_acquire_cond(id, val, cond);
}
// The below definitions with ptr arg is not officially used anywhere.
// The ptr arg is intentionally not used.
INTRINSIC(void)
acquire_equal(const void *a, unsigned id, unsigned val) {
  return acquire_equal(id, val);
}
INTRINSIC(void)
acquire_equal(const void *a, unsigned id, unsigned val, int cond) {
  return acquire_equal(id, val, cond);
}
INTRINSIC(void)
acquire_greater_equal(unsigned id, unsigned val) {
  return __builtin_aiev2_acquire(id, -val);
}
INTRINSIC(void)
acquire_greater_equal(unsigned id, unsigned val, unsigned cond) {
  return __builtin_aiev2_acquire_cond(id, -val, cond);
}
INTRINSIC(void)
acquire_greater_equal(const void *a, unsigned id, unsigned val) {
  return acquire_greater_equal(id, val);
}
INTRINSIC(void)
acquire_greater_equal(const void *a, unsigned id, unsigned val, unsigned cond) {
  return acquire_greater_equal(id, val, cond);
}
INTRINSIC(void)
release(unsigned id, int val) { return __builtin_aiev2_release(id, val); }
INTRINSIC(void)
release(unsigned id, int val, int cond) {
  return __builtin_aiev2_release_cond(id, val, cond);
}
INTRINSIC(void)
release(void *a, unsigned id, int val) { return release(id, val); }
INTRINSIC(void)
release(void *a, unsigned id, int val, int cond) {
  return release(id, val, cond);
}
INTRINSIC(void)
done(void) {
  __builtin_aiev2_sched_barrier();
  __builtin_aiev2_done();
  __builtin_aiev2_sched_barrier();
}

#endif // __AIEV2_LOCKS_H__
