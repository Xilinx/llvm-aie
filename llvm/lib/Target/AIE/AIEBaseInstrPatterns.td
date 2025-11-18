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

// Make our generic extract vector elt instructions available to TableGen patterns.
def vextract_zext : SDNode<"G_AIE_ZEXT_EXTRACT_VECTOR_ELT",
                          SDTypeProfile<1, 2, [SDTCisInt<0>,SDTCisVec<1>, SDTCisInt<2>]>>;
def : GINodeEquiv<G_AIE_ZEXT_EXTRACT_VECTOR_ELT, vextract_zext>;

def vextract_sext : SDNode<"G_AIE_SEXT_EXTRACT_VECTOR_ELT",
                          SDTypeProfile<1, 2, [SDTCisInt<0>,SDTCisVec<1>, SDTCisInt<2>]>>;
def : GINodeEquiv<G_AIE_SEXT_EXTRACT_VECTOR_ELT, vextract_sext>;

class Extr512Pat<ValueType DstTy, ValueType SrcTy, dag Idx, SDNode Op, Instruction Inst> :
    Pat<(DstTy (Op SrcTy:$src1, Idx)),
        (Inst SrcTy:$src1, Idx)>;

multiclass Extract_512<ValueType DstTy, ValueType SrcTy, dag Idx, Instruction UnsignedOpc, Instruction SignedOpc> {
  def : Extr512Pat<DstTy, SrcTy, Idx, vextract_zext, UnsignedOpc>;
  def : Extr512Pat<DstTy, SrcTy, Idx, vextract_sext, SignedOpc>;
}

// Make our generic insert vector elt instructions available to TableGen patterns.
def vinsert : SDNode<"G_AIE_INSERT_VECTOR_ELT",
                      SDTypeProfile<1, 3, [SDTCisSameAs<0,1>, SDTCisVec<0>, SDTCisInt<2>, SDTCisInt<3>]>>;
def : GINodeEquiv<G_AIE_INSERT_VECTOR_ELT, vinsert>;

class Insert512Pat<ValueType VecTy, dag Val, dag Idx, Instruction Inst> :
    Pat<(VecTy (vinsert VecTy:$src1, Val, Idx)),
        (Inst VecTy:$src1, Idx, Val)>;
