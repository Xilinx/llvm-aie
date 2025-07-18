//===-- AIEISelDAGToDAG.cpp - A dag to dag inst selector for AIE ------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the AIE target.
//
//===----------------------------------------------------------------------===//

#include "AIEISelDAGToDAG.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
using namespace llvm;

#define DEBUG_TYPE "aie-isel"

void AIEDAGToDAGISel::Select(SDNode *Node) {
  // If we have a custom node, we have already selected.
  if (Node->isMachineOpcode()) {
    LLVM_DEBUG(dbgs() << "== "; Node->dump(CurDAG); dbgs() << "\n");
    Node->setNodeId(-1);
    return;
  }
  // Instruction Selection not handled by the auto-generated tablegen selection
  // should be handled here.
  unsigned Opcode = Node->getOpcode();
  SDLoc DL(Node);
  EVT VT = Node->getValueType(0);

  switch (Opcode) {
  case ISD::Constant: {
    auto ConstNode = cast<ConstantSDNode>(Node);
    // Force Null values to zero.
    if (VT == MVT::i32 && ConstNode->isZero()) {
        // Init to zero.
        SDValue Imm = CurDAG->getTargetConstant(0, DL, MVT::i32);
        auto NewNode = CurDAG->getMachineNode(AIE::MOV_U20, DL, VT, Imm);
        ReplaceNode(Node, NewNode);
      return;
    }
    break;
  }
  // case ISD::FrameIndex: {
  //     // Replace FrameIndex with Target-specific FrameIndex ops.
  //     SDValue Imm = CurDAG->getTargetConstant(0, DL, MVT::i32);
  //     int FI = cast<FrameIndexSDNode>(Node)->getIndex();
  //     SDValue TFI = CurDAG->getTargetFrameIndex(FI, VT);
  //     ReplaceNode(Node, CurDAG->getMachineNode(AIE::ADDI, DL, VT, TFI, Imm));
  //     return;
  // }
  }
  // Select the default instruction.
  SelectCode(Node);
}

// Match a frame index that can be used in an addressing mode.
bool AIEDAGToDAGISel::SelectFrameIndex(SDValue &N, SDValue &R) {
  if (N.getOpcode() != ISD::FrameIndex)
    return false;
  int FI = cast<FrameIndexSDNode>(N)->getIndex();
  LLVM_DEBUG(dbgs() << "SelectFrameIndex: " << FI << "\n");
  R = CurDAG->getTargetFrameIndex(FI, MVT::i32);
  return true;
}

class AIEDAGToDAGISelLegacy : public SelectionDAGISelLegacy {
  static char ID;

public:
  explicit AIEDAGToDAGISelLegacy(TargetMachine &TM)
      : SelectionDAGISelLegacy(ID, std::make_unique<AIEDAGToDAGISel>(TM)) {}

  StringRef getPassName() const override {
    return "AIE DAG->DAG Pattern Instruction Selection";
  }
};

char AIEDAGToDAGISelLegacy::ID = 0;

// This pass converts a legalized DAG into a AIE-specific DAG, ready
// for instruction scheduling.
FunctionPass *llvm::createAIEISelDag(AIETargetMachine &TM) {
  return new AIEDAGToDAGISelLegacy(TM);
}
