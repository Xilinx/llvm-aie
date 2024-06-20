//===-- AIEMIRFormatter.h ----------------------------------------*- C++
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// AIE specific overrides of MIRFormatter.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEMIRFORMATTER_H
#define LLVM_LIB_TARGET_AIE_AIEMIRFORMATTER_H

#include "llvm/CodeGen/MIRFormatter.h"

namespace llvm {

class MachineFunction;
struct PerFunctionMIParsingState;

class AIEMIRFormatter final : public MIRFormatter {
public:
  AIEMIRFormatter() = default;
  virtual ~AIEMIRFormatter() = default;

  /// Implement target specific parsing of target custom pseudo source value.
  bool
  parseCustomPseudoSourceValue(StringRef Src, MachineFunction &MF,
                               PerFunctionMIParsingState &PFS,
                               const PseudoSourceValue *&PSV,
                               ErrorCallbackType ErrorCallback) const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEMIRFORMATTER_H
