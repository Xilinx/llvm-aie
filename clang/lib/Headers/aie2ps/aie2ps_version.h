//===- aie2ps_version.h -----------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2PS_VERSION_H__
#define __AIE2PS_VERSION_H__

#define __AIE2PS__
#define __AIE_ARCH__ 22
#define __AIE_ARCH_NAME__ AIE2PS
#define __AIE_ARCH_NAME_STR__ "AIE2PS"
#define __AIE_MODEL_VERSION__ 10500
#define __AIE_ARCH_MODEL_VERSION__ 22010500
#define __AIE_MODEL_VERSION_NAME_STR__ "aie2ps_arch_r1p5"

#define __AIE_ARCH_MODEL_VERSION__LABEL                                        \
  __AIE_ARCH_MODEL_VERSION__##__AIE_ARCH_MODEL_VERSION__

#endif // __AIE2PS_VERSION_H__
