//===- AIEBaseInstrPatterns.td -ISel patterns for AIEngine -*-tablegen -*----//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2024-2025 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file describes instruction selection patterns for the AIEngine
// instructions in TableGen format.
//
//===----------------------------------------------------------------------===//


// Defining a custom renderer seems to be the easiest way to negate immediates.
// Using a !sub "bang operantor" is overly complicated, requiring dag<->int
// conversions and some black magic.
def NegateImm : SDNodeXForm<imm, [{
  return CurDAG->getConstant(-N->getSExtValue(), SDLoc(N), MVT::i32);
}]>;
def gi_NegateImm : GICustomOperandRenderer<"renderNegateImm">,
  GISDNodeXFormEquiv<NegateImm>;

// This converts a generic FrameIndex into a TargetFrameIndex, frameindex
// cannot be used in an output pattern.
def frameindex_to_targetframeindex : SDNodeXForm<frameindex, [{
  auto FI = cast<FrameIndexSDNode>(N);
  return CurDAG->getTargetFrameIndex(FI->getIndex(), MVT::i32);
}]>;

// Contrary to SelectionDAG which adds the FrameIndex to the selected instruction
// automatically (in SelectionDAG/InstrEmitter.cpp), GlobalIsel does not.
// This is done in a manually written renderFrameIndex function instead.
def gi_frameindex_to_targetframeindex : GICustomOperandRenderer<"renderFrameIndex">,
  GISDNodeXFormEquiv<frameindex_to_targetframeindex>;

// VBOR / VBAND / XOR
foreach vec512Ty = [v64i8, v32i16, v16i32] in {
  def : Pat<(vec512Ty (or VEC512:$src1, VEC512:$src2)), (VBOR VEC512:$src1, VEC512:$src2)>;
  def : Pat<(vec512Ty (and VEC512:$src1, VEC512:$src2)), (VBAND VEC512:$src1, VEC512:$src2)>;
}
