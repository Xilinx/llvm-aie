//===-- AIERelocationPatch.h - AIE Relocation Patching Utilities -*- C++
//-*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2025-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains utilities for AIE instruction bit-field patching,
/// shared between the MC layer (assembler) and lld (linker).
///
/// AIE instructions can be up to 32 bytes with arbitrary bit-field positions,
/// requiring specialized patching logic that operates on multi-word values.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_MC_AIERELOCATIONPATCH_H
#define LLVM_MC_AIERELOCATIONPATCH_H

#include <cassert>
#include <cstdint>

namespace llvm {
namespace AIE {

/// Read an N byte value from Loc in little-endian fashion.
/// N should be even and not more than eight.
uint64_t readNBytes(int N, uint8_t *Loc);

/// Write N bytes of Value to Loc in little-endian fashion.
/// N should be even and not more than eight.
void writeNBytes(int N, uint8_t *Loc, uint64_t Value);

/// Implements patching instruction bundles of even size <= 32 bytes.
/// Basic operation reads in a multi-word image from the section data,
/// aligns the field value to the right position, selects bits from the
/// image and the patch field using a selection mask and writes the result
/// back to the section data.
class RelocationPatch {
  // Workspace (256-bit = 4x64-bit lanes)
  uint64_t val[4];

public:
  RelocationPatch();
  RelocationPatch(uint64_t vl, uint64_t vh = 0);

  /// Construct a value from the \p n bytes in memory pointed by \p loc
  /// Widened to support up to 32 bytes (256 bits).
  RelocationPatch(int n, uint8_t *loc);

  /// Shift \p *this left by \p shift bits (0 <= shift < 256)
  RelocationPatch operator<<(int shift) const;

  /// Patch \p size bits of \p field at position \p shift in the workspace
  void patch(RelocationPatch field, uint32_t size, uint32_t shift);

  /// Write the \p n bytes patch back to memory at location \p loc
  void write(int n, uint8_t *loc);
};

/// Patch the memory bytes Loc[0..N-1] with a field extracted from V.
/// The memory is organised in little endian fashion, i.e. Loc[0] holds the
/// least significant bits. We handle the memory content (the image) as one
/// value of N*8 bits.
/// Hi and Lo are the positions, counting from lsb = 0, of the msb and lsb
/// of the field of V.
/// Pos indicates the msb of the destination field in the image.
void patchNBytes(uint32_t N, uint8_t *Loc, uint64_t V, uint32_t Hi, uint32_t Lo,
                 uint32_t Pos);

} // end namespace AIE
} // end namespace llvm

#endif // LLVM_MC_AIERELOCATIONPATCH_H
