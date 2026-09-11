//===-- AIERelocationPatch.cpp - AIE Relocation Patching Utilities -------===//
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
/// This file implements utilities for AIE instruction bit-field patching,
/// shared between the MC layer (assembler) and lld (linker).
///
//===----------------------------------------------------------------------===//

#include "llvm/MC/AIERelocationPatch.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::support::endian;

namespace llvm {
namespace AIE {

// Read an N byte value from Loc in little-endian fashion.
// N should be even and not more than eight.
uint64_t readNBytes(int N, uint8_t *Loc) {
  uint64_t Result = 0;
  switch (N) {
  case 0:
    break;
  case 2:
    Result = read16le(Loc);
    break;
  case 4:
    Result = read32le(Loc);
    break;
  case 6:
    Result = read16le(Loc + 4);
    Result = (Result << 32) | read32le(Loc);
    break;
  case 8:
    Result = read32le(Loc + 4);
    Result = (Result << 32) | read32le(Loc);
    break;
  default:
    assert(false && "Unexpected read size");
  }
  return Result;
}

// Write N bytes of Value to Loc in little-endian fashion.
// N should be even and not more than eight.
void writeNBytes(int N, uint8_t *Loc, uint64_t Value) {
  const uint64_t M32 = 0xffffffff;
  const uint64_t M16 = 0xffff;
  switch (N) {
  case 0:
    break;
  case 2:
    write16le(Loc, Value & M16);
    break;
  case 4:
    write32le(Loc, Value & M32);
    break;
  case 6:
    write32le(Loc, Value & M32);
    write16le(Loc + 4, (Value >> 32) & M16);
    break;
  case 8:
    write32le(Loc, Value & M32);
    write32le(Loc + 4, (Value >> 32) & M32);
    break;
  default:
    assert(false && "Unexpected write size");
  }
}

RelocationPatch::RelocationPatch() {
  val[0] = 0;
  val[1] = 0;
  val[2] = 0;
  val[3] = 0;
}

RelocationPatch::RelocationPatch(uint64_t vl, uint64_t vh) {
  val[0] = vl;
  val[1] = vh;
  val[2] = 0;
  val[3] = 0;
}

/// Construct a value from the \p n bytes in memory pointed by \p loc
/// Widened to support up to 32 bytes (256 bits).
RelocationPatch::RelocationPatch(int n, uint8_t *loc) {
  assert(n <= 32);
  int i = 0;
  while (n > 8) {
    val[i++] = readNBytes(8, loc);
    n -= 8;
    loc += 8;
  }
  val[i] = readNBytes(n, loc);
  // Zero any remaining lanes
  while (++i < 4)
    val[i] = 0;
}

/// Shift \p *this left by \p shift bits (0 <= shift < 256)
RelocationPatch RelocationPatch::operator<<(int shift) const {
  RelocationPatch r = *this;
  assert(shift < 256);
  assert(shift >= 0);
  if (shift == 0) {
    // Avoid UB: val[0] >> (64 - shift) would right-shift by 64 when shift==0.
    return r;
  }

  // First shift by words
  while (shift >= 64) {
    for (int i = 3; i > 0; i--) {
      r.val[i] = r.val[i - 1];
    }
    r.val[0] = 0;
    shift -= 64;
  }

  // Then shift remaining bits across words
  if (shift != 0) {
    const int rshift = 64 - shift;
    for (int i = 3; i > 0; i--) {
      r.val[i] = (r.val[i] << shift) | (r.val[i - 1] >> rshift);
    }
    r.val[0] = r.val[0] << shift;
  }

  return r;
}

/// Patch \p size bits of \p field at position \p shift in the workspace
void RelocationPatch::patch(RelocationPatch field, uint32_t size,
                            uint32_t shift) {
  assert(size <= 64);
  // Create a mask of the field size
  RelocationPatch mask(~(size == 64 ? uint64_t(0) : ~uint64_t(0) << size));

  // Shift both into position
  field = field << shift;
  mask = mask << shift;

  // Do the insertion across all lanes
  val[0] = (val[0] & ~mask.val[0]) | (field.val[0] & mask.val[0]);
  val[1] = (val[1] & ~mask.val[1]) | (field.val[1] & mask.val[1]);
  val[2] = (val[2] & ~mask.val[2]) | (field.val[2] & mask.val[2]);
  val[3] = (val[3] & ~mask.val[3]) | (field.val[3] & mask.val[3]);
}

/// Write the \p n bytes patch back to memory at location \p loc
void RelocationPatch::write(int n, uint8_t *loc) {
  assert(n <= 32);
  int i = 0;
  while (n > 8) {
    writeNBytes(8, loc, val[i++]);
    n -= 8;
    loc += 8;
  }
  writeNBytes(n, loc, val[i]);
}

// Patch the memory bytes Loc[0..N-1] with a field extracted from V.
// The memory is organised in little endian fashion, i.e. Loc[0] holds the least
// significant bits. We handle the memory content (the image) as one value
// of N*8 bits.
// Hi and Lo are the positions, counting from lsb = 0, of the msb and lsb
// of the field of V.
// Pos indicates the msb of the destination field in the image.
void patchNBytes(uint32_t N, uint8_t *Loc, uint64_t V, uint32_t Hi, uint32_t Lo,
                 uint32_t Pos) {

  assert(N <= 32);
  assert(Hi >= Lo);
  assert(Hi <= 63);

  // The size of the area to be patched
  const uint32_t BitSize = N * 8;
  // The field size, which is the same in V and Image
  const uint32_t FieldSize = Hi - Lo + 1;
  assert(Pos < BitSize);
  // We could just return, but I think relocations of zero bits
  // should be avoided.
  assert(FieldSize > 0);
  assert(FieldSize <= 64);
  assert(Pos + FieldSize <= BitSize);

  // Read bytes to be patched in wide representation
  RelocationPatch Image(N, Loc);

  // Pos is the msb, the shift count needs the lsb
  const uint32_t Shift = BitSize - Pos - FieldSize;

  // Align field with bit 0, and put it in a wide representation.
  // Excess high bits will be masked off when patching
  RelocationPatch Field(V >> Lo);

  // Patch it
  Image.patch(Field, FieldSize, Shift);

  // Write back
  Image.write(N, Loc);
}

} // end namespace AIE
} // end namespace llvm
