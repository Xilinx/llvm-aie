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
#include "llvm/ADT/iterator.h"
#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ErrorHandling.h"
#include <memory>
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

  SlotBits getSlotSet() const { return SlotSet; }

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

class FormatIterator {
public:
  using const_iterator = const VLIWFormat *;
  FormatIterator(const VLIWFormat *Fmts, const SlotBits Slots) : Slots(Slots) {
    if (Fmts->covers(Slots))
      CurrentFormat = Fmts;
    else {
      CurrentFormat = *(++(*this));
    }
  }

  FormatIterator &operator++() {
    findNextMatchingFormat();
    return *this;
  }

  const VLIWFormat *operator*() const { return CurrentFormat; }

  bool operator!=(const FormatIterator &other) const {
    return CurrentFormat != other.CurrentFormat;
  }
  ~FormatIterator() = default;

private:
  SlotBits Slots;
  const VLIWFormat *CurrentFormat;

  void findNextMatchingFormat() {
    while (CurrentFormat != nullptr && CurrentFormat->Opcode) {
      ++CurrentFormat;
      if (CurrentFormat && CurrentFormat->covers(Slots)) {
        break;
      }
    }
  }
};

class PacketFormats {
public:
  using const_iterator = FormatIterator;
  PacketFormats(const VLIWFormat *Formats) : FormatsTable(Formats){};

  const VLIWFormat *getFormat(SlotBits SlotSet) const;

  const VLIWFormat *getFormatBySize(SlotBits SlotSet, unsigned Size) const;

  llvm::iterator_range<FormatIterator>
  getFormatsRangeBySlots(SlotBits SlotSet) const;

private:
  const VLIWFormat *findFirstMatchingFormat(const VLIWFormat *Fmts,
                                            SlotBits Slots) const {
    while (Fmts->Opcode && !Fmts->covers(Slots)) {
      ++Fmts;
    }
    return Fmts->Opcode ? Fmts : nullptr;
  }

  const VLIWFormat *findLastMatchingFormat(const VLIWFormat *Fmts,
                                           SlotBits Slots) const {
    const VLIWFormat *lastFormat = nullptr;
    while (Fmts->Opcode) {
      if (Fmts->covers(Slots)) {
        lastFormat = Fmts;
        Fmts++;
        continue;
      }
      ++Fmts;
    }
    return lastFormat;
  }
  const VLIWFormat *FormatsTable;
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_FORMAT_H
