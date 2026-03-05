//===-- AIE2CommonAddrSpace.h - AIE2 Common Address Space -------*- C++-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares the common Address Space and DM banks shared across
// AIE2 variants (AIE2, AIE2P, AIE2PS).
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_AIE2COMMONADDRSPACE_H
#define LLVM_SUPPORT_AIE2COMMONADDRSPACE_H

#include "AIEBaseAddrSpaceInfo.h"
#include <bitset>

namespace llvm {

namespace AIE2X {

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

} // end namespace AIE2X

/// Common address space information shared across AIE2 variants
class AIE2CommonAddrSpaceInfo : public AIEBaseAddrSpaceInfo {

public:
  MemoryBankBits getDefaultMemoryBank() const override {
    std::bitset<32> MemoryBanks;
    using namespace AIE2X;
    MemoryBanks.set(static_cast<unsigned>(AIEBanks::A))
        .set(static_cast<unsigned>(AIEBanks::B))
        .set(static_cast<unsigned>(AIEBanks::C))
        .set(static_cast<unsigned>(AIEBanks::D));
    return MemoryBanks.to_ulong();
  }

  MemoryBankBits
  getMemoryBanksFromAddressSpace(unsigned AddrSpace) const override {
    std::bitset<32> MemoryBanks;
    using namespace AIE2X;
    switch (static_cast<AddressSpaces>(AddrSpace)) {
    case AddressSpaces::a:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::A));
      break;
    case AddressSpaces::b:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::B));
      break;
    case AddressSpaces::c:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::C));
      break;
    case AddressSpaces::d:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::D));
      break;
    case AddressSpaces::ab:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::A))
          .set(static_cast<unsigned>(AIEBanks::B));
      break;
    case AddressSpaces::ac:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::A))
          .set(static_cast<unsigned>(AIEBanks::C));
      break;
    case AddressSpaces::ad:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::A))
          .set(static_cast<unsigned>(AIEBanks::D));
      break;
    case AddressSpaces::bc:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::B))
          .set(static_cast<unsigned>(AIEBanks::C));
      break;
    case AddressSpaces::bd:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::B))
          .set(static_cast<unsigned>(AIEBanks::D));
      break;
    case AddressSpaces::cd:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::C))
          .set(static_cast<unsigned>(AIEBanks::D));
      break;
    case AddressSpaces::TM:
      MemoryBanks.set(static_cast<unsigned>(AIEBanks::TileMemory));
      break;
    default:
      MemoryBanks.set();
      break;
    }

    return MemoryBanks.to_ulong();
  }
};

} // end namespace llvm

#endif // LLVM_SUPPORT_AIE2COMMONADDRSPACE_H
