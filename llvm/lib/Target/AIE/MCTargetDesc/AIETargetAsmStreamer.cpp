//===-- AIETargetStreamer.cpp - AIE Target Streamer Methods -----------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file provides AIE specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "AIETargetAsmStreamer.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

AIETargetStreamer::AIETargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

// This part is for ascii assembly output
AIETargetAsmStreamer::AIETargetAsmStreamer(MCStreamer &S,
                                               formatted_raw_ostream &OS)
    : AIETargetStreamer(S), OS(OS) {}

void AIETargetAsmStreamer::emitDirectiveOptionPush() {
  OS << "\t.option\tpush\n";
}

void AIETargetAsmStreamer::emitDirectiveOptionPop() {
  OS << "\t.option\tpop\n";
}

void AIETargetAsmStreamer::emitDirectiveOptionRVC() {
  OS << "\t.option\trvc\n";
}

void AIETargetAsmStreamer::emitDirectiveOptionNoRVC() {
  OS << "\t.option\tnorvc\n";
}

void AIETargetAsmStreamer::emitDirectiveOptionRelax() {
  OS << "\t.option\trelax\n";
}

void AIETargetAsmStreamer::emitDirectiveOptionNoRelax() {
  OS << "\t.option\tnorelax\n";
}
