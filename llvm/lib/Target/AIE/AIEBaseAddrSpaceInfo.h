//===-- AIEBaseAddrSpaceInfo.h - Define Base AddressSpace Class  -*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the AIEngine Base Address Space class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_AIEBASEADDRSPACEINFO_H
#define LLVM_SUPPORT_AIEBASEADDRSPACEINFO_H

#include <cstdint>

namespace llvm {

using MemoryBankBits = uint64_t;

class AIEBaseAddrSpaceInfo {
public:
  virtual MemoryBankBits getDefaultMemoryBank() const {
    // By default assume conflicts.
    return ~0;
  }

  virtual MemoryBankBits
  getMemoryBanksFromAddressSpace(unsigned AddrSpace) const {
    // By default assume conflicts.
    return ~0;
  }
};

} // end namespace llvm

#endif // LLVM_SUPPORT_AIEBASEADDRSPACEINFO_H
