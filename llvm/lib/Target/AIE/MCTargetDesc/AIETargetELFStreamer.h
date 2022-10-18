//===-- AIEELFStreamer.h - AIE ELF Target Streamer ---------*- C++ -*--===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEELFSTREAMER_H
#define LLVM_LIB_TARGET_AIE_AIEELFSTREAMER_H

#include "AIETargetAsmStreamer.h"
#include "llvm/MC/MCELFStreamer.h"

// AIETargetStreamer is an abstract base class defined in AIETargetAsmStreamer.h

namespace llvm {

class AIETargetELFStreamer : public AIETargetStreamer {
public:
  MCELFStreamer &getStreamer();
  AIETargetELFStreamer(MCStreamer &S, const MCSubtargetInfo &STI);

  virtual void emitDirectiveOptionPush() override;
  virtual void emitDirectiveOptionPop() override;
  virtual void emitDirectiveOptionRVC() override;
  virtual void emitDirectiveOptionNoRVC() override;
  virtual void emitDirectiveOptionRelax() override;
  virtual void emitDirectiveOptionNoRelax() override;
  virtual void finish() override;

private:
  const MCSubtargetInfo &STI;
};
}
#endif
