//===--AIEInstrGISel.td - gMIR instructions for AIE2 -------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file adds gMIR instructions for the AIEngine target
//
//===----------------------------------------------------------------------===//

let InOperandList = (ins type0:$base, type1:$offset), hasSideEffects = 0  in {
  let OutOperandList = (outs type2:$val, type0:$ptr), mayLoad = 1 in {
    def G_AIE_POSTINC_LOAD : AIEGenericInstruction;
    def G_AIE_POSTINC_ZEXTLOAD : AIEGenericInstruction;
    def G_AIE_POSTINC_SEXTLOAD : AIEGenericInstruction;
  }
  let OutOperandList = (outs type2:$val), mayLoad = 1 in {
    def G_AIE_OFFSET_LOAD : AIEGenericInstruction;
    def G_AIE_OFFSET_ZEXTLOAD : AIEGenericInstruction;
    def G_AIE_OFFSET_SEXTLOAD : AIEGenericInstruction;
  }
}
let InOperandList = (ins type0:$base, type1:$modifier, type1:$size, type1: $stride, type1:$counter), hasSideEffects = 0  in {
  let OutOperandList = (outs type2:$val, type0:$ptr, type1:$counter_new), mayLoad = 1 in {
    def G_AIE_POSTINC_2D_LOAD : AIEGenericInstruction;
    def G_AIE_POSTINC_2D_ZEXTLOAD : AIEGenericInstruction;
    def G_AIE_POSTINC_2D_SEXTLOAD : AIEGenericInstruction;
  }
}
let InOperandList = (ins type0:$base, type1:$modifier, type1:$size1, type1:$size2, type1: $stride1, type1:$stride2, type1:$counter1, type1:$counter2), hasSideEffects = 0  in {
  let OutOperandList = (outs type2:$val, type0:$ptr, type1:$counter1_new, type1:$counter2_new), mayLoad = 1 in {
    def G_AIE_POSTINC_3D_LOAD : AIEGenericInstruction;
    def G_AIE_POSTINC_3D_ZEXTLOAD : AIEGenericInstruction;
    def G_AIE_POSTINC_3D_SEXTLOAD : AIEGenericInstruction;
  }
}
let InOperandList = (ins type2:$val, type0:$base, type1:$offset), hasSideEffects = 0, mayStore = 1  in {
  def G_AIE_POSTINC_STORE : AIEGenericInstruction {
    let OutOperandList = (outs type0:$ptr);
  }
  def G_AIE_OFFSET_STORE : AIEGenericInstruction {
    let OutOperandList = (outs);
  }
}
let InOperandList = (ins type2:$val, type0:$base, type1:$modifier, type1:$size, type1: $stride, type1:$counter), hasSideEffects = 0, mayStore = 1 in {
  def G_AIE_POSTINC_2D_STORE : AIEGenericInstruction {
    let OutOperandList = (outs type0:$ptr, type1:$counter_new);
  }
}
let InOperandList = (ins type2:$val, type0:$base, type1:$modifier, type1:$size1, type1:$size2, type1: $stride1, type1:$stride2, type1:$counter1, type1:$counter2), hasSideEffects = 0, mayStore = 1  in {
  def G_AIE_POSTINC_3D_STORE : AIEGenericInstruction {
    let OutOperandList = (outs type0:$ptr, type1:$counter1_new, type1:$counter2_new);
  }
}
// AIE Generic vector extractelement. Zero-extend the element
// after extracting the element from a vector. This gMIR is added
// to avoid zero extending smaller size elements (S8, S16) to S32
def G_AIE_ZEXT_EXTRACT_VECTOR_ELT : AIEGenericInstruction {
  let OutOperandList = (outs type0:$dst);
  let InOperandList = (ins type1:$src, type2:$idx);
  let hasSideEffects = false;
}
// AIE Generic vector extractelement. Sign-extend the element
// after extracting the element from a vector. This gMIR is added
// to avoid sign extending smaller size elements (S8, S16) to S32
def G_AIE_SEXT_EXTRACT_VECTOR_ELT : AIEGenericInstruction {
  let OutOperandList = (outs type0:$dst);
  let InOperandList = (ins type1:$src, type2:$idx);
  let hasSideEffects = false;
}

// AIE generic insert vector element. This is used to legalize
// S32 (large) type values being inserted to a vector with
// S8 or S16 (small) element type. This is OK because internally
// we select it to a VINSERT that will truncate the bits to the
// required element type.
def G_AIE_INSERT_VECTOR_ELT : AIEGenericInstruction {
  let OutOperandList = (outs type0:$dst);
  let InOperandList = (ins type0:$src, type1:$elt, type2:$idx);
  let hasSideEffects = false;
}

def G_AIE_ADD_VECTOR_ELT_HI : AIEGenericInstruction {
  let OutOperandList = (outs type0:$dst);
  let InOperandList = (ins type0:$src, type1:$elt);
  let hasSideEffects = false;
}

def G_AIE_BROADCAST_VECTOR : AIEGenericInstruction {
  let OutOperandList = (outs type0:$dst);
  let InOperandList = (ins type1:$src);
  let hasSideEffects = false;
}

// Create a larger vector by padding undefined values in the high bits
def G_AIE_PAD_VECTOR_UNDEF : AIEGenericInstruction {
  let OutOperandList = (outs type0:$dst);
  let InOperandList = (ins type1:$src);
  let hasSideEffects = false;
}

// Extract a smaller vector by ignoring the high bits
def G_AIE_UNPAD_VECTOR : AIEGenericInstruction {
  let OutOperandList = (outs type0:$dst);
  let InOperandList = (ins type1:$src);
  let hasSideEffects = false;
}
