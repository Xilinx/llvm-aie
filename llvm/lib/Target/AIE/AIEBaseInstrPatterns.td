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

// VBOR / VBAND
foreach vec256Ty = [v32i8, v16i16, v8i32] in {
  def : Pat<(vec256Ty (or VEC256:$src1, VEC256:$src2)),
            (vec256Ty (EXTRACT_SUBREG (VBOR
                        (v32i16 (REG_SEQUENCE VEC512, VEC256:$src1, sub_256_lo)),
                        (v32i16 (REG_SEQUENCE VEC512, VEC256:$src2, sub_256_lo))),
                       sub_256_lo))>;
  def : Pat<(vec256Ty (and VEC256:$src1, VEC256:$src2)),
            (vec256Ty (EXTRACT_SUBREG (VBAND
                        (v32i16 (REG_SEQUENCE VEC512, VEC256:$src1, sub_256_lo)),
                        (v32i16 (REG_SEQUENCE VEC512, VEC256:$src2, sub_256_lo))),
                       sub_256_lo))>;
}
foreach vec512Ty = [v64i8, v32i16, v16i32] in {
  def : Pat<(vec512Ty (or VEC512:$src1, VEC512:$src2)), (VBOR VEC512:$src1, VEC512:$src2)>;
  def : Pat<(vec512Ty (and VEC512:$src1, VEC512:$src2)), (VBAND VEC512:$src1, VEC512:$src2)>;
}
foreach vec1024Ty = [v128i8, v64i16, v32i32] in {
def : Pat<(vec1024Ty (or VEC1024:$src1, VEC1024:$src2)),
          (REG_SEQUENCE VEC1024,
                 (VBOR (v16i32 (EXTRACT_SUBREG VEC1024:$src1, sub_512_lo)),
                       (v16i32 (EXTRACT_SUBREG VEC1024:$src2, sub_512_lo))), sub_512_lo,
                 (VBOR (v16i32 (EXTRACT_SUBREG VEC1024:$src1, sub_512_hi)),
                       (v16i32 (EXTRACT_SUBREG VEC1024:$src2, sub_512_hi))), sub_512_hi
          )>;
def : Pat<(vec1024Ty (and VEC1024:$src1, VEC1024:$src2)),
          (REG_SEQUENCE VEC1024,
                 (VBAND (v16i32 (EXTRACT_SUBREG VEC1024:$src1, sub_512_lo)),
                        (v16i32 (EXTRACT_SUBREG VEC1024:$src2, sub_512_lo))), sub_512_lo,
                 (VBAND (v16i32 (EXTRACT_SUBREG VEC1024:$src1, sub_512_hi)),
                        (v16i32 (EXTRACT_SUBREG VEC1024:$src2, sub_512_hi))), sub_512_hi
          )>;
}

// For vector select, VSEL instruction is used, since it does select word by word
// sel input is modified, "sel ? v1 : v2 is equivalent to vsel(sel-1, v1, v2)"
// rs1(sel) is zero-extended from the original 1 bit type
foreach vec512Ty = [v64i8, v32i16, v16i32] in {
  def : Pat<(vec512Ty (select (i32 eR:$rs1), VEC512:$rs2, VEC512:$rs3)),
            (vec512Ty (VSEL_32 VEC512:$rs2, VEC512:$rs3, (ADD_add_r_ri eR:$rs1, (i32 -1))))>;
}

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
