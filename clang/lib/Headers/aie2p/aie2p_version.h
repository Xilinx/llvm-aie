//===- aie2p_version.h ------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef __AIE2P_VERSION_H__
#define __AIE2P_VERSION_H__

#define __AIE2P__
#define __AIE_ARCH__ 21
#define __AIE_ARCH_NAME__ AIE2P
#define __AIE_ARCH_NAME_STR__ "AIE2P"
#define __AIE_MODEL_VERSION__ 11500
#define __AIE_ARCH_MODEL_VERSION__ 21011500
#define __AIE_MODEL_VERSION_NAME_STR__ "aie2p_arch_r1p15"

#define __AIE_ARCH_MODEL_VERSION__LABEL                                        \
  __AIE_ARCH_MODEL_VERSION__##__AIE_ARCH_MODEL_VERSION__

#endif // __AIE2P_VERSION_H__
