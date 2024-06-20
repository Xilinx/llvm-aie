//===----- AIEFormat.cpp - Internal VLIW bundle representation ------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// This file implements support related to the VLIW bundles
//===----------------------------------------------------------------------===//

#include "AIEFormat.h"
#include "AIE.h"

namespace llvm {

bool VLIWFormat::covers(SlotBits Slots) const { return !(Slots & ~SlotSet); }

const VLIWFormat *PacketFormats::getFormat(SlotBits Slots) const {
  for (const VLIWFormat *Fmt = FormatsTable; Fmt->Opcode; Fmt++) {
    if (Fmt->covers(Slots)) {
      return Fmt;
    }
  }
  return nullptr;
}

const VLIWFormat *PacketFormats::getFormatBySize(SlotBits Slots,
                                                 unsigned Size) const {
  const VLIWFormat *Fmt = nullptr;
  auto FormatsRange = getFormatsRangeBySlots(Slots);
  for (auto it = FormatsRange.begin(), end = FormatsRange.end(); it != end;
       ++it) {
    const VLIWFormat *Format = *it;
    // Find the first match
    if (Format->getSize() == Size) {
      Fmt = Format;
      break;
    }
  }
  return Fmt;
}

llvm::iterator_range<FormatIterator>
PacketFormats::getFormatsRangeBySlots(SlotBits SlotSet) const {
  const_iterator b =
      FormatIterator(findFirstMatchingFormat(FormatsTable, SlotSet), SlotSet);
  const_iterator e =
      FormatIterator(findLastMatchingFormat(FormatsTable, SlotSet), SlotSet);
  return llvm::make_range(b, ++e);
}

} // namespace llvm
