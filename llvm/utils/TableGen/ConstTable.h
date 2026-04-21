//===- ConstTable.h - Helper class for generating constexpr tables --------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the ConstTable class, which provides utilities for
// generating constexpr tables in TableGen backends. It supports marking
// ranges of entries and generating ArrayRef expressions to reference slices
// of the table.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_UTILS_TABLEGEN_CONSTTABLE_H
#define LLVM_UTILS_TABLEGEN_CONSTTABLE_H

#include "llvm/Support/raw_ostream.h"
#include <sstream>
#include <string>

namespace llvm {

/// Helper class to create flat constexpr tables and utilities for referencing
/// slices of those tables.
///
/// Example usage:
/// \code
///   ConstTable OpTable("OperandInfo", "OperandInfoTable");
///   OpTable.mark("Entry1");
///   OpTable << "  {0, &SomeRegClass}";
///   OpTable.next();
///   OpTable << "  {1, &OtherRegClass}";
///   OpTable.next();
///   std::string slice = OpTable.arrayRef();  //
///   "llvm::ArrayRef<OperandInfo>(&OperandInfoTable[0], 2)" OpTable.finish();
///   OS << OpTable;  // Emit the table definition
/// \endcode
class ConstTable {
  std::stringstream Text;
  std::string Type;
  std::string Name;
  unsigned Mark = 0;
  unsigned Size = 0;

public:
  ConstTable(std::string Type, std::string Name) : Type(Type), Name(Name) {
    Text << "static constexpr " << Type << " const " << Name << "[] = {\n";
  }

  const std::stringstream &text() const { return Text; }
  std::stringstream &text() { return Text; }
  unsigned size() { return Size; }

  /// Mark the start of a block of items. Returns the current index.
  unsigned mark(const char *Comment = nullptr) {
    if (Comment) {
      Text << "// " << Comment << " " << Size << "\n";
    }
    Mark = Size;
    return Mark;
  }

  /// Make a reference to entry Idx, relative to the start of the table.
  std::string absRef(unsigned Idx) const {
    return "&" + Name + "[" + std::to_string(Idx) + "]";
  }

  /// Make a reference to entry Idx relative to the last marked block.
  std::string ref(unsigned Idx) const { return absRef(Mark + Idx); }

  /// Make a reference to the next entry.
  std::string refNext() const { return absRef(Size); }

  /// Return an ArrayRef expression for the entries since the last marked
  /// block.
  std::string arrayRef() const {
    unsigned NumElements = Size - Mark;
    if (NumElements == 0) {
      return "{}";
    }
    return "llvm::ArrayRef<" + Type + ">(" + ref(0) + ", " +
           std::to_string(NumElements) + ")";
  }

  /// Move to the next entry.
  void next() {
    Text << ",\n";
    Size++;
  }

  /// Finish the table definition.
  ConstTable &finish() {
    Text << "};\n\n";
    return *this;
  }
};

template <typename T> ConstTable &operator<<(ConstTable &Table, T Item) {
  Table.text() << Item;
  return Table;
}

inline raw_ostream &operator<<(raw_ostream &O, const ConstTable &Table) {
  O << Table.text().str();
  return O;
}

} // namespace llvm

#endif // LLVM_UTILS_TABLEGEN_CONSTTABLE_H
