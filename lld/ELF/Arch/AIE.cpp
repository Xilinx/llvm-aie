//===- AIE.cpp ------------------------------------------------------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "InputFiles.h"
#include "Target.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;

namespace {

// This is vector of NOP instructions of sizes from 1 to 8 bytes.  The
// appropriately sized instructions are used to fill the gaps between sections
// which are executed during fall through.
static const std::vector<std::vector<uint8_t>> nopInstructions = {
    {0x00},
    {0x01, 0x00},
    {0x01, 0x00, 0x00},
    {0x01, 0x00, 0x01, 0x00},
    {0x01, 0x00, 0x01, 0x00, 0x00},
    {0x01, 0x00, 0x01, 0x00, 0x01, 0x00},
    {0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00},
    {0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00}};

class AIE final : public TargetInfo {
public:
  AIE();
  uint32_t calcEFlags() const override;
  RelExpr getRelExpr(RelType Type, const Symbol &S,
                     const uint8_t *Loc) const override;
  void relocate(uint8_t *Loc, const Relocation &rel, uint64_t Val) const override;

private:
  void relocateAIE1(uint8_t *Loc, const Relocation &rel, uint64_t Val) const;
  void relocateAIE2(uint8_t *Loc, const Relocation &rel, uint64_t Val) const;
};

} // end anonymous namespace

AIE::AIE() {
  // FIXME: How do we represent this?
  // noneRel = R_AIE_NONE;

  symbolicRel = -1;
  nopInstrs = nopInstructions;
}

static uint32_t getEFlags(InputFile *F) {
  return cast<ObjFile<ELF32LE>>(F)->getObj().getHeader().e_flags;
}

uint32_t AIE::calcEFlags() const {
  assert(!ctx.objectFiles.empty());

  uint32_t Target = getEFlags(ctx.objectFiles.front());
  return Target;
}

RelExpr AIE::getRelExpr(const RelType Type, const Symbol &S,
                          const uint8_t *Loc) const {
  switch (Type) {
  default:
    return R_ABS;
  }
}

// Read an N byte value from Loc in little-endian fashion.
// N should be even and not more than eight.
static uint64_t readNBytes(int N, uint8_t *Loc) {
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
static void writeNBytes(int N, uint8_t *Loc, uint64_t Value) {
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

/// Implements patching instruction bundles of even size <= 16 bytes.
/// Basic operation reads in a multi-word image from the section data,
/// aligns the field value to the right position, selects bits from the
/// image and the patch field using a selection mask and writes the result
/// back to the section data.
class Patch {
  // Workspace
  uint64_t val[2];

public:
  Patch() {
    val[0] = 0;
    val[1] = 0;
  }
  Patch(uint64_t vl, uint64_t vh = 0) {
    val[0] = vl;
    val[1] = vh;
  }

  /// Construct a value from the \p n bytes in memory pointed by \p loc
  Patch(int n, uint8_t *loc) {
    assert(n <= 16);
    int i = 0;
    while (n > 8) {
      val[i++] = readNBytes(8, loc);
      n -= 8;
      loc += 8;
    }
    val[i] = readNBytes(n, loc);
  }

  /// Shift \p *this left by \p shift bits
  Patch operator<<(int shift) {
    Patch r;
    assert(shift < 64);
    if (shift >= 64) {
      r.val[1] = val[0] << (shift - 64);
      r.val[0] = 0;
    } else {
      r.val[0] = val[0] << shift;
      r.val[1] = (val[0] >> (64 - shift)) | (val[1] << shift);
    }
    return r;
  }

  /// Patch \size bits of \p field at position \p shift in the workspace
  void patch(Patch field, uint32_t size, uint32_t shift) {
    assert(size <= 64);
    // Create a mask of the field size
    Patch mask(~(~uint64_t(0) << size));

    // Shift both into position
    field = field << shift;
    mask = mask << shift;

    // Do the insertion
    val[0] = (val[0] & ~mask.val[0]) | (field.val[0] & mask.val[0]);
    val[1] = (val[1] & ~mask.val[1]) | (field.val[1] & mask.val[1]);
  }

  /// Write the \p n bytes patch back to memory at location \p loc
  void write(int n, uint8_t *loc) {
    assert(n <= 16);
    int i = 0;
    while (n > 8) {
      writeNBytes(8, loc, val[i++]);
      n -= 8;
      loc += 8;
    }
    writeNBytes(n, loc, val[i]);
  }
};

// Patch the memory bytes Loc[0..N-1] with a field extracted from V.
// The memory is organised in little endian fashion, i.e. Loc[0] holds the least
// significant bits. We handle the memory content (the image) as one value
// of N*8 bits.
// Hi and Lo are the positions, counting from lsb = 0, of the msb and lsb
// of the field of V.
// Pos indicates the msb of the destination field in the image.
static void patchNBytes(uint32_t N, uint8_t *Loc, uint64_t V, uint32_t Hi,
                        uint32_t Lo, uint32_t Pos) {

  assert(N <= 16);
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
  Patch Image(N, Loc);

  // Pos is the msb, the shift count needs the lsb
  uint32_t Shift = BitSize - Pos - FieldSize;

  // Align field with bit 0, and put it in a wide representation.
  // Excess high bits will be masked off when patching
  Patch Field(V >> Lo);

  // Patch it
  Image.patch(Field, FieldSize, Shift);

  // Write back
  Image.write(N, Loc);
}

static void patch4bytes(uint8_t *Loc, const uint64_t V, uint32_t Begin,
                        uint32_t End, uint32_t Pos) {
  patchNBytes(4, Loc, V, Begin, End, Pos);
}

static void patch6bytes(uint8_t *Loc, const uint64_t V, uint32_t Begin,
                        uint32_t End, uint32_t Pos) {
  patchNBytes(6, Loc, V, Begin, End, Pos);
}

static void patch8bytes(uint8_t *Loc, const uint64_t V, uint32_t Begin,
                        uint32_t End, uint32_t Pos) {
  patchNBytes(8, Loc, V, Begin, End, Pos);
}

static void patch10bytes(uint8_t *Loc, const uint64_t V, uint32_t Begin,
                         uint32_t End, uint32_t Pos) {
  patchNBytes(10, Loc, V, Begin, End, Pos);
}

static void patch12bytes(uint8_t *Loc, const uint64_t V, uint32_t Begin,
                         uint32_t End, uint32_t Pos) {
  patchNBytes(12, Loc, V, Begin, End, Pos);
}

static void patch14bytes(uint8_t *Loc, const uint64_t V, uint32_t Begin,
                         uint32_t End, uint32_t Pos) {
  patchNBytes(14, Loc, V, Begin, End, Pos);
}

static void patch16bytes(uint8_t *Loc, const uint64_t V, uint32_t Begin,
                         uint32_t End, uint32_t Pos) {
  patchNBytes(16, Loc, V, Begin, End, Pos);
}

void AIE::relocateAIE1(uint8_t *Loc, const Relocation &rel,
                       uint64_t Val) const {
  if (errorHandler().verbose)
        lld::outs() << "Relocation expr=" << rel.expr << " " << rel.type << "@"
                    << getErrorLocation(Loc) << "\n";

  // Relocation applied to debug_info
  if (rel.expr == R_NONE) {
        checkUInt(Loc, Val, 20, rel);
        patch4bytes(Loc, Val, 19, 0, 12);
        return;
  }

  switch (rel.type) {
    // Most relocations are implemented in a file which is
    // automatically generated from the processor description.
#include "AIE_rela.inc"

      //72 : (symbol_addr_AR  + addend )  :  addr [19..0]@0 in w08[4]      // with default addend 0
      //73 : (symbol_addr_AR  + addend )  :  addr [19..0]@0 in w32[1]      // with default addend 0
  case 72:
  case 73:
    checkUInt(Loc, Val, 20, rel);
    patch4bytes(Loc, Val, 19, 0, 12);
    return;
    // 74 : (symbol_addr_AR  + addend )  :  t01u [0..0]@0 in w08[4]      //
    // with default addend 0 75 : (symbol_addr_AR  + addend )  :  t01u
    // [0..0]@0 in w32[1]      // with default addend 0
  case 74:
  case 75:
    checkUInt(Loc, Val, 1, rel);
    patch4bytes(Loc, Val, 0, 0, 31);
    return;

  default:
    error(getErrorLocation(Loc) +
          "unimplemented relocation: " + toString(rel.type));
    return;
  }
}
void AIE::relocateAIE2(uint8_t *Loc, const Relocation &rel,
                       uint64_t Val) const {
  if (errorHandler().verbose)
    lld::outs() << "Relocation expr=" << rel.expr << " " << rel.type << "@"
                << getErrorLocation(Loc) << "\n";

  // Relocation applied to debug_info
  if (rel.expr == R_NONE) {
    checkUInt(Loc, Val, 20, rel);
    patch4bytes(Loc, Val, 19, 0, 12);
    return;
  }

  switch (rel.type) {
    // Most relocations are implemented in a file which is
    // automatically generated from the processor description.
#include "AIE2_rela.inc"

    // 72 : (symbol_addr_AR  + addend )  :  addr [19..0]@0 in w08[4]      //
    // with default addend 0 73 : (symbol_addr_AR  + addend )  :  addr
    // [19..0]@0 in w32[1]      // with default addend 0
  case 72:
  case 73:
      checkUInt(Loc, Val, 20, rel);
      patch4bytes(Loc, Val, 19, 0, 12);
      return;
      //74 : (symbol_addr_AR  + addend )  :  t01u [0..0]@0 in w08[4]      // with default addend 0
      //75 : (symbol_addr_AR  + addend )  :  t01u [0..0]@0 in w32[1]      // with default addend 0
  case 74:
  case 75:
      checkUInt(Loc, Val, 1, rel);
      patch4bytes(Loc, Val, 0, 0, 31);
      return;

  default:
    error(getErrorLocation(Loc) +
          "unimplemented relocation: " + toString(rel.type));
    return;
  }
}
void AIE::relocate(uint8_t *Loc, const Relocation &rel, uint64_t Val) const {
  unsigned Arch = calcEFlags() & 0x3;
  switch (Arch) {
  case 0:
    // Backward compatible with not setting arch flags.
  case 1:
    relocateAIE1(Loc, rel, Val);
    break;
  case 2:
    relocateAIE2(Loc, rel, Val);
    break;
  default:
    llvm_unreachable("Unknown AIE version in EFLAGS");
  }
}

TargetInfo *elf::getAIETargetInfo() {
  static AIE Target;
  return &Target;
}
