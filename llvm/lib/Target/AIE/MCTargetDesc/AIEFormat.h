//===-- AIEFormat.h  Format utilities for AIE -----------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This defines the format data necessary for dealing with VLIW bundles
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_FORMAT_H
#define LLVM_LIB_TARGET_AIE_FORMAT_H

#include "AIE.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/ErrorHandling.h"
#include <vector>

namespace llvm {
using SlotBits = uint64_t;
const int MaxSlots = 64;
class MCSlotKind;

class VLIWFormat {
public:
  // A range to iterate over the slots of a format
  class SlotKindRange {
  public:
    constexpr SlotKindRange() = default;
    constexpr SlotKindRange(const MCSlotKind *Begin, const MCSlotKind *End)
        : Begin(Begin), End(End) {}
    constexpr const MCSlotKind *begin() const { return Begin; }
    constexpr const MCSlotKind *end() const { return End; }

  private:
    const MCSlotKind *Begin = nullptr;
    const MCSlotKind *End = nullptr;
  };
  constexpr VLIWFormat(unsigned Opcode, const char *Name, SlotKindRange Range,
                       unsigned Size, SlotBits Bits)
      : Opcode(Opcode), Name(Name), Slots(Range), Size(Size), SlotSet(Bits) {}

  const SlotKindRange &getSlots() const { return Slots; }

  const unsigned &getSize() const { return Size; }

  /// \returns whether this format can accommodate all slots in \p Slots
  bool covers(SlotBits Slots) const;

  // Opcode of the format
  unsigned Opcode;

  // Name of the format
  const char *Name;

private:
  // The slots in the format-specific order
  SlotKindRange Slots;

  // Size of the format
  unsigned Size;

  // Precomputed set of the above slots
  SlotBits SlotSet = 0;
};

class PacketFormats {
public:
  PacketFormats(const VLIWFormat *Formats) : FormatsTable(Formats){};
  const VLIWFormat *getFormat(SlotBits SlotSet) const;

private:
  const VLIWFormat *FormatsTable;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_FORMAT_H
