//===- AIEMCAsmInfo.h - AIE asm properties ----------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the AIEMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCASMINFO_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class AIEMCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit AIEMCAsmInfo(const Triple &TT);
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCASMINFO_H
