//===-- AIE2AddrSpace.h - Define Address Space for AIEngine V2 ---*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIEngine V2 Address Space and DM banks
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_AIE2ADDRSPACE_H
#define LLVM_SUPPORT_AIE2ADDRSPACE_H

namespace llvm {

namespace AIE2 {

enum class AddressSpaces {
  none, // Default address space
  PM,   // Address space for Program Memory (PM)
  DM,   // Address space for Data Memory(DM) includes Bank A, B, C, D
  DM_test,
  stack, // Address space for stack
  a,     // Address space for DM Bank A
  b,     // Address space for DM Bank B
  c,     // Address space for DM Bank C
  d,     // Address space for DM Bank D
  ab,    // Address space for DM Bank A and B
  ac,    // Address space for DM Bank A and C
  ad,    // Address space for DM Bank A and D
  bc,    // Address space for DM Bank B and C
  bd,    // Address space for DM Bank B and D
  cd,    // Address space for DM Bank C and D
  TM     // Address space for TM (Tile Memory)
};

enum class AIEBanks { A, B, C, D, TileMemory };

} // end namespace AIE2
} // end namespace llvm

#endif // LLVM_SUPPORT_AIE2ADDRSPACE_H
