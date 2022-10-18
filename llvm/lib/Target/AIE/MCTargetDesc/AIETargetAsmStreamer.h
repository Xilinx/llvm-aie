//===-- AIETargetStreamer.h - AIE Target Streamer ----------*- C++ -*--===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIETARGETSTREAMER_H
#define LLVM_LIB_TARGET_AIE_AIETARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace llvm {

class formatted_raw_ostream;

class AIETargetStreamer : public MCTargetStreamer {
public:
  AIETargetStreamer(MCStreamer &S);

  virtual void emitDirectiveOptionPush() = 0;
  virtual void emitDirectiveOptionPop() = 0;
  virtual void emitDirectiveOptionRVC() = 0;
  virtual void emitDirectiveOptionNoRVC() = 0;
  virtual void emitDirectiveOptionRelax() = 0;
  virtual void emitDirectiveOptionNoRelax() = 0;
};

// This part is for ascii assembly output
class AIETargetAsmStreamer : public AIETargetStreamer {
  formatted_raw_ostream &OS;

public:
  AIETargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);

  void emitDirectiveOptionPush() override;
  void emitDirectiveOptionPop() override;
  void emitDirectiveOptionRVC() override;
  void emitDirectiveOptionNoRVC() override;
  void emitDirectiveOptionRelax() override;
  void emitDirectiveOptionNoRelax() override;
};

}
#endif
