//===-- AIE1ISelLowering.cpp - AIE DAG Lowering Interface -------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that AIEv1 uses to lower LLVM code into a
// selection DAG. Even though some of the SelectionDAG code isn't AIEv1
// specific, it is moved here because future AIE versions exclusively support
// GlobalISel.
//
//===----------------------------------------------------------------------===//

#include "AIE1ISelLowering.h"
#include "AIE.h"
#include "AIEMachineFunctionInfo.h"
#include "AIERegisterInfo.h"
#include "AIESubtarget.h"
#include "AIETargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "aie-lower"

STATISTIC(NumTailCalls, "Number of tail calls");

static SDValue getInstr(unsigned MachineOpc, const SDLoc &dl, MVT Ty,
                        ArrayRef<SDValue> Ops, SelectionDAG &DAG) {
  SDNode *N = DAG.getMachineNode(MachineOpc, dl, Ty, Ops);
  return SDValue(N, 0);
}
static MVT ty(SDValue Op) { return Op.getValueType().getSimpleVT(); }

AIE1TargetLowering::AIE1TargetLowering(const TargetMachine &TM,
                                       const AIEBaseSubtarget &STI)
    : AIEBaseTargetLowering(TM, STI) {

  setOperationAction(ISD::VASTART, MVT::Other, Custom);
  setOperationAction(ISD::VACOPY, MVT::Other, Expand);
  setOperationAction(ISD::VAEND, MVT::Other, Expand);

  // SelectionDAG syntax is weird, but basically, the next lines say:
  // - In the general case, do wathever type legalization first and then (after
  //   type legalization is done), custom-lower the actual vaarg instruction.
  // - However, for i64 (and generally for base types that do not fit in
  //   registers), start by lowering vaarg (via a call to ReplaceNodeResults()),
  //   and then legalize the types.
  setOperationAction(ISD::VAARG, MVT::Other, Custom);
  for (MVT MachineVT : MVT::all_valuetypes()) {
    if (!MachineVT.isInteger() && !MachineVT.isFloatingPoint())
      continue;
    TypeSize MVTSize = MachineVT.getSizeInBits();
    // Types larger than a stack slot should not be expanded before lowering the
    // vaarg instruction. The ABI has tricky ordering properties, and
    // splitting large arguments first (therefore creating multiple vaarg
    // instructions) makes lowering un-necessarily complex.
    if (!MVTSize.isScalable() &&
        MVTSize.getFixedValue() > getStackSlotSizeInBits()) {
      setOperationAction(ISD::VAARG, MachineVT, Custom);
    }
  }

  // Set up the register classes.
  addRegisterClass(MVT::i32, &AIE::mRCmRegClass);
  addRegisterClass(MVT::i20, &AIE::PTRRegClass);

  // We don't really have i64 data, but have 64-bit values for some control
  // registers
  addRegisterClass(MVT::v2i32, &AIE::mCRegClass);
  addRegisterClass(MVT::f32, &AIE::GPRRegClass);
  addRegisterClass(MVT::v8i1, &AIE::GPRRegClass);
  addRegisterClass(MVT::v16i1, &AIE::GPRRegClass);

  addRegisterClass(MVT::v16i8, &AIE::VEC128RegClass);
  addRegisterClass(MVT::v8i16, &AIE::VEC128RegClass);
  addRegisterClass(MVT::v4i32, &AIE::VEC128RegClass);
  addRegisterClass(MVT::v4f32, &AIE::VEC128RegClass);

  addRegisterClass(MVT::v32i8, &AIE::VEC256RegClass);
  addRegisterClass(MVT::v16i16, &AIE::VEC256RegClass);
  addRegisterClass(MVT::v8i32, &AIE::VEC256RegClass);
  addRegisterClass(MVT::v8f32, &AIE::VEC256RegClass);
  addRegisterClass(MVT::v4i64, &AIE::VEC256RegClass);

  addRegisterClass(MVT::v64i8, &AIE::VEC512RegClass);
  addRegisterClass(MVT::v32i16, &AIE::VEC512RegClass);
  addRegisterClass(MVT::v16i32, &AIE::VEC512RegClass);
  addRegisterClass(MVT::v16f32, &AIE::VEC512RegClass);
  addRegisterClass(MVT::v8i64, &AIE::VEC512RegClass);

  addRegisterClass(MVT::v128i8, &AIE::VEC1024RegClass);
  addRegisterClass(MVT::v64i16, &AIE::VEC1024RegClass);
  addRegisterClass(MVT::v32i32, &AIE::VEC1024RegClass);
  addRegisterClass(MVT::v32f32, &AIE::VEC1024RegClass);
  addRegisterClass(MVT::v8i48, &AIE::ACC384RegClass);
  addRegisterClass(MVT::v16i48, &AIE::ACC768RegClass);

  // Compute derived properties from the register classes.
  // In particular, this ensures that types without native support are expanded
  // (to multiple supported smaller types) or softened (to a supported type of
  // the same size).
  computeRegisterProperties(STI.getRegisterInfo());

  setStackPointerRegisterToSaveRestore(AIE::SP);

  // Declare our legalizations
  for (auto VT : {MVT::i8, MVT::i16, MVT::i20, MVT::i32}) {
    setOperationAction(ISD::GlobalAddress, VT, Custom);
    setOperationAction(ISD::GlobalTLSAddress, VT, Custom);
  }

  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i20, Custom);
  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32, Custom);

  // Eliminate BR_CC operations (a combined branch with comparison)).
  // Expand them into BRCOND (a conditional branch) + SET_CC (a comparison)
  // We'll match these patterns explicitly.
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  setOperationAction(ISD::BR_CC, MVT::i20, Expand);
  setOperationAction(ISD::BR_CC, MVT::f32, Expand);
  setOperationAction(ISD::BR_CC, MVT::i16, Expand);
  setOperationAction(ISD::BR_CC, MVT::i8, Expand);

  // Eliminate umul_hilo operations.  We can't support them for 32 bit.
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::MULHU, MVT::i32, Expand);
  setOperationAction(ISD::MULHS, MVT::i32, Expand);

  // Eliminate wide shift operations. We can't support them.
  setOperationAction(ISD::SHL_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRL_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::SRA_PARTS, MVT::i32, Expand);
  setOperationAction(ISD::FSHL, MVT::i32, Expand);
  setOperationAction(ISD::FSHR, MVT::i32, Expand);
  setOperationAction(ISD::ROTL, MVT::i32, Expand);
  setOperationAction(ISD::ROTR, MVT::i32, Expand);

  // Expand SELECT_CC for most types.
  setOperationAction(ISD::SELECT_CC, MVT::i8, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i16, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i20, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::f32, Expand);

  // These comparisons could be implemented with the vector datapath,
  // but are probably more efficient on the scalar processor.
  // There are libcalls for all the SETCC fp ops, but they
  // aren't implemented in the regular legalizer structure.
  setCondCodeAction(ISD::SETO, MVT::f32, Expand);
  setCondCodeAction(ISD::SETUO, MVT::f32, Expand);

  // Eliminate byte swap operations. We can't support them.
  setOperationAction(ISD::BSWAP, MVT::i32, Expand);

  // Eliminate counting operations which are not supported.
  // Note that CTLZ *is* supported.
  setOperationAction(ISD::CTTZ, MVT::i32, Expand);
  setOperationAction(ISD::CTPOP, MVT::i32, Expand);

  // Eliminate division operations.  These can be supported,
  // but have to be microcoded.
  setOperationAction(ISD::UDIV, MVT::i32, Expand);
  setOperationAction(ISD::SDIV, MVT::i32, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::UREM, MVT::i32, Expand);
  setOperationAction(ISD::SREM, MVT::i32, Expand);

  // Handle v2i32 types, first Expand All operations.
  for (unsigned Op = 0; Op < ISD::BUILTIN_OP_END; ++Op) {
    setOperationAction(Op, MVT::v2i32, Expand);
  }
  // Expand Truncating/extending stores/loads for v2i32 type.
  for (MVT VT : MVT::integer_fixedlen_vector_valuetypes()) {
    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::v2i32, Expand);
    setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::v2i32, Expand);
    setLoadExtAction(ISD::EXTLOAD, VT, MVT::v2i32, Expand);

    setLoadExtAction(ISD::SEXTLOAD, MVT::v2i32, VT, Expand);
    setLoadExtAction(ISD::ZEXTLOAD, MVT::v2i32, VT, Expand);
    setLoadExtAction(ISD::EXTLOAD, MVT::v2i32, VT, Expand);

    setTruncStoreAction(VT, MVT::v2i32, Expand);
    setTruncStoreAction(MVT::v2i32, VT, Expand);
  }
  // custom lower v2i32 load and store.
  // Covert v2i32 into two scalar operations of type i32
  setOperationAction(ISD::LOAD, MVT::v2i32, Custom);
  setOperationAction(ISD::STORE, MVT::v2i32, Custom);
  // Write the pattern for extracting i32's from v2i32
  // and building vector v2i32 from i32's
  setOperationAction(ISD::EXTRACT_VECTOR_ELT, MVT::v2i32, Legal);
  setOperationAction(ISD::BUILD_VECTOR, MVT::v2i32, Legal);
  // Floating point division needs a library.
  setOperationAction(ISD::FDIV, MVT::f32, Expand);
  setOperationAction(ISD::FREM, MVT::f32, Expand);
  setOperationAction(ISD::FSQRT, MVT::f32, Legal);
  setOperationAction(ISD::FMA, MVT::f32, Expand);

  setOperationAction(ISD::ConstantFP, MVT::f32, Legal);
  setOperationAction(ISD::FMINIMUM, MVT::f32, Legal);
  setOperationAction(ISD::FMAXIMUM, MVT::f32, Legal);
  setOperationAction(ISD::FMINNUM, MVT::f32, Legal);
  setOperationAction(ISD::FMAXNUM, MVT::f32, Legal);
  setOperationAction(ISD::LRINT, MVT::f32, Legal);
  setOperationAction(ISD::SINT_TO_FP, MVT::f32, Legal);
  setOperationAction(ISD::FROUND, MVT::f32, LibCall);
  setOperationAction(ISD::UINT_TO_FP, MVT::i32, LibCall);
  setOperationAction(ISD::FP_TO_SINT, MVT::i32, LibCall);
  setOperationAction(ISD::FP_TO_UINT, MVT::i32, LibCall);
  setOperationAction(ISD::FP16_TO_FP, MVT::f32, LibCall);
  setOperationAction(ISD::FP_TO_FP16, MVT::f32, LibCall);
  setOperationAction(ISD::BF16_TO_FP, MVT::f32, LibCall);
  setOperationAction(ISD::FP_TO_BF16, MVT::f32, LibCall);

  setOperationAction(ISD::FMINIMUM, MVT::v8f32, Legal);
  setOperationAction(ISD::FMAXIMUM, MVT::v8f32, Legal);
  setOperationAction(ISD::FMINNUM, MVT::v8f32, Legal);
  setOperationAction(ISD::FMAXNUM, MVT::v8f32, Legal);
  //  setOperationAction(ISD::ConstantPool,         MVT::i16, Legal);

  // Deal with small types by expanding them to i32.
  for (auto VT : {MVT::i1}) {
    setOperationAction(ISD::SIGN_EXTEND_INREG, VT, Expand);
    setOperationAction(ISD::SIGN_EXTEND, VT, Expand);
    setOperationAction(ISD::ZERO_EXTEND, VT, Expand);
    setOperationAction(ISD::ANY_EXTEND, VT, Expand);
  }
  for (auto N : {ISD::EXTLOAD, ISD::SEXTLOAD, ISD::ZEXTLOAD}) {
    setLoadExtAction(N, MVT::i32, MVT::i1, Promote);
    setLoadExtAction(N, MVT::i20, MVT::i1, Promote);
  }

  // Types natively supported:
  for (MVT NativeVT : {
           MVT::v16i8, MVT::v8i16, MVT::v4i32, MVT::v4f32,  // 128-bit
           MVT::v32i8, MVT::v16i16, MVT::v8i32, MVT::v8f32, // 256-bit
           MVT::v8i1, MVT::v16i1                            // Vector conditions
       }) {
    // setOperationAction(ISD::ConstantPool,       NativeVT, Expand);
    setOperationAction(ISD::BUILD_VECTOR, NativeVT, Custom);
    setOperationAction(ISD::SMAX, NativeVT, Legal);
    setOperationAction(ISD::SMIN, NativeVT, Legal);
    setOperationAction(ISD::AND, NativeVT, Expand);
    setOperationAction(ISD::OR, NativeVT, Expand);
    setOperationAction(ISD::XOR, NativeVT, Expand);
    //    setOperationAction(ISD::EXTRACT_VECTOR_ELT, NativeVT, Custom);
    // setOperationAction(ISD::INSERT_VECTOR_ELT,  NativeVT, Custom);
    // setOperationAction(ISD::EXTRACT_SUBVECTOR,  NativeVT, Custom);
    // setOperationAction(ISD::INSERT_SUBVECTOR,   NativeVT, Custom);
    // setOperationAction(ISD::CONCAT_VECTORS,     NativeVT, Custom);
  }

  for (MVT NativeVT : {
           MVT::v64i8, MVT::v32i16, MVT::v16i32, MVT::v16f32, // 512-bit
       }) {
    setOperationAction(ISD::ConstantPool, NativeVT, Custom);
    setOperationAction(ISD::BUILD_VECTOR, NativeVT, Custom);
    setOperationAction(ISD::SMAX, NativeVT, Legal);
    setOperationAction(ISD::SMIN, NativeVT, Legal);
    setOperationAction(ISD::AND, NativeVT, Expand);
    setOperationAction(ISD::OR, NativeVT, Expand);
    setOperationAction(ISD::XOR, NativeVT, Expand);
  }

  for (MVT NativeVT : {
           MVT::v128i8, MVT::v64i16, MVT::v32i32, MVT::v32f32, // 1024-bit
       }) {
    setOperationAction(ISD::ConstantPool, NativeVT, Custom);
    setOperationAction(ISD::BUILD_VECTOR, NativeVT, Custom);
  }
}

/* custom lower ISD::LOAD , currently handles lowering for v2i32 */
SDValue AIE1TargetLowering::LowerLOAD(SDValue Op, SelectionDAG &DAG) const {
  LoadSDNode *LoadNode = cast<LoadSDNode>(Op);
  SDLoc DL(Op);
  EVT VT = Op.getValueType();

  if (VT.isVector() && VT == MVT::v2i32) {
    SDValue Ops[2];
    std::tie(Ops[0], Ops[1]) = scalarizeVectorLoad(LoadNode, DAG);
    return DAG.getMergeValues(Ops, DL);
  }
  return SDValue();
}

/* custom lower ISD::STORE , currently handles lowering for v2i32 */
SDValue AIE1TargetLowering::LowerSTORE(SDValue Op, SelectionDAG &DAG) const {
  StoreSDNode *StoreNode = cast<StoreSDNode>(Op);

  SDValue Value = StoreNode->getValue();
  EVT VT = Value.getValueType();
  if (VT.isVector() && VT == MVT::v2i32) {
    return scalarizeVectorStore(StoreNode, DAG);
  }
  return SDValue();
}

SDValue AIE1TargetLowering::LowerVASTART(SDValue Op, SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();
  auto *FuncInfo = MF.getInfo<AIEMachineFunctionInfo>();

  SDLoc DL(Op);
  SDValue FI = DAG.getFrameIndex(FuncInfo->getVarArgsFrameIndex(),
                                 getPointerTy(MF.getDataLayout()));

  // vastart just stores the address of the VarArgsFrameIndex slot into the
  // memory location argument.
  const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
  return DAG.getStore(Op.getOperand(0), DL, FI, Op.getOperand(1),
                      MachinePointerInfo(SV));
}

SDValue AIE1TargetLowering::LowerVAARG(SDValue Op, SelectionDAG &DAG) const {
  const TargetFrameLowering *TFI = Subtarget.getFrameLowering();
  assert(TFI->getStackGrowthDirection() == TargetFrameLowering::StackGrowsUp);
  const Value *V = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
  EVT VT = Op.getValueType();
  SDLoc DL(Op);
  SDValue Chain = Op.getOperand(0);
  SDValue Addr = Op.getOperand(1);
  MaybeAlign Align(Op.getConstantOperandVal(3));
  assert((!Align || *Align <= getMinStackArgumentAlignment()) &&
         "Vaarg has a larger alignment than stack slots.");

  auto PtrVT = getPointerTy(DAG.getDataLayout());
  SDValue PrevVAPtr =
      DAG.getLoad(PtrVT, DL, Chain, Addr, MachinePointerInfo(V));
  Chain = PrevVAPtr.getValue(1);

  // Compute the address of the current VAARG by subtracting its size
  // from the previous VAARG address.
  Type *ArgTy = VT.getTypeForEVT(*DAG.getContext());
  unsigned ArgSize = DAG.getDataLayout().getTypeAllocSize(ArgTy);
  assert(ArgSize >= getStackSlotSize() &&
         "Vararg was not extended to match a stack slot.");
  assert(ArgSize % getStackSlotSize() == 0 &&
         "Vararg size isn't a multiple of a stack slot size.");
  SDValue CurVAPtr = DAG.getNode(ISD::SUB, DL, PtrVT, PrevVAPtr,
                                 DAG.getConstant(ArgSize, DL, PtrVT));

  // Now store the new VA pointer, and load it to get the current VAARG.
  SDValue APStore =
      DAG.getStore(Chain, DL, CurVAPtr, Addr, MachinePointerInfo(V));
  return DAG.getLoad(VT, DL, APStore, CurVAPtr, MachinePointerInfo());
}

SDValue AIE1TargetLowering::LowerOperation(SDValue Op,
                                           SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
    report_fatal_error("unimplemented operand");
  // case ISD::CONCAT_VECTORS:       return LowerCONCAT_VECTORS(Op, DAG);
  // case ISD::INSERT_SUBVECTOR:     return LowerINSERT_SUBVECTOR(Op, DAG);
  // case ISD::INSERT_VECTOR_ELT:    return LowerINSERT_VECTOR_ELT(Op, DAG);
  // case ISD::EXTRACT_SUBVECTOR:    return LowerEXTRACT_SUBVECTOR(Op, DAG);
  // case ISD::EXTRACT_VECTOR_ELT:   return LowerEXTRACT_VECTOR_ELT(Op, DAG);
  case ISD::BUILD_VECTOR:
    return LowerBUILD_VECTOR(Op, DAG);
  // case ISD::VECTOR_SHUFFLE:    return LowerVECTOR_SHUFFLE(Op, DAG);
  case ISD::DYNAMIC_STACKALLOC:
    return LowerDYNAMIC_STACKALLOC(Op, DAG);
  case ISD::GlobalAddress:
  case ISD::GlobalTLSAddress:
    return LowerGlobalAddress(Op, DAG);
  case ISD::TargetGlobalAddress:
    return LowerTargetGlobalAddress(Op, DAG);
  case ISD::ConstantPool:
    return LowerConstantPool(Op, DAG);
  case ISD::LOAD:
    return LowerLOAD(Op, DAG);
  case ISD::STORE:
    return LowerSTORE(Op, DAG);
  case ISD::VASTART:
    return LowerVASTART(Op, DAG);
  case ISD::VAARG:
    return LowerVAARG(Op, DAG);
  }
}

SDValue AIE1TargetLowering::LowerBUILD_VECTOR_i1(SDValue Op,
                                                 SelectionDAG &DAG) const {
  SDLoc DL(Op);
  MVT VecTy = ty(Op);
  unsigned N = VecTy.getVectorNumElements();

  // First create base with bits set where known
  unsigned Bits32 = 0;
  for (unsigned i = 0; i < N; ++i) {
    SDValue V = Op.getOperand(i);
    if (!isa<ConstantSDNode>(V) && !V.isUndef())
      continue;
    bool BitSet = V.isUndef() ? false : cast<ConstantSDNode>(V)->getZExtValue();
    if (BitSet)
      Bits32 |= 1 << i;
  }

  // Add in unknown nodes
  SDValue Base = DAG.getNode(AIEISD::GPR_CAST, DL, VecTy,
                             DAG.getConstant(Bits32, DL, MVT::i32));
  for (unsigned i = 0; i < N; ++i) {
    SDValue V = Op.getOperand(i);
    if (isa<ConstantSDNode>(V) || V.isUndef())
      continue;
    Base = getInstr(AIE::BITSET, DL, VecTy,
                    {Base, DAG.getConstant(i, DL, MVT::i32), V}, DAG);
  }
  return Base;
}

SDValue AIE1TargetLowering::LowerBUILD_VECTOR_x8(SDValue Op, SelectionDAG &DAG,
                                                 MVT VecTy, int start) const {
  SDLoc DL(Op);
  unsigned N = 8;

  SmallVector<SDValue, 8> Ops;
  for (unsigned i = start, e = Op.getNumOperands(); i != e; ++i)
    Ops.push_back(Op.getOperand(i));

  SDValue chain = DAG.getUNDEF(Op.getValueType());
  // Shift each operand in.
  unsigned i = 0;
  // Don't bother shifting in 'UNDEF' at the end of the vector.
  while ((i < N) && Ops[N - 1 - i].isUndef())
    i++;
  for (; i < N; ++i) {
    // FIXME: this should probably be a pseudo-instruction to support
    // later mapping to either load path.
    chain =
        getInstr(AIE::S2V_SHIFTW0_R32, DL, VecTy, {chain, Ops[N - 1 - i]}, DAG);
  }
  return chain;
}

SDValue AIE1TargetLowering::LowerBUILD_VECTOR(SDValue Op,
                                              SelectionDAG &DAG) const {
  SDLoc DL(Op);
  MVT VecTy = ty(Op);
  unsigned BW = VecTy.getScalarSizeInBits();

  if (BW == 1)
    return LowerBUILD_VECTOR_i1(Op, DAG);

  assert(BW == 32 && "LowerBUILD_VECTOR only supports 32-bit vectors");

  unsigned N = VecTy.getVectorNumElements();
  if (N == 8)
    return LowerBUILD_VECTOR_x8(Op, DAG, VecTy, 0);

  else if (N == 16) {
    auto HalfVecTy = VecTy.getHalfNumVectorElementsVT();
    SDValue v0 = LowerBUILD_VECTOR_x8(Op, DAG, HalfVecTy, 0);
    SDValue v1 = LowerBUILD_VECTOR_x8(Op, DAG, HalfVecTy, 8);
    SDValue chain = DAG.getUNDEF(Op.getValueType());
    chain = DAG.getTargetInsertSubreg(AIE::sub_256bit_lo, DL, VecTy, chain, v0);
    chain = DAG.getTargetInsertSubreg(AIE::sub_256bit_hi, DL, VecTy, chain, v1);
    return chain;
  } else if (N == 32) {
    auto HalfVecTy = VecTy.getHalfNumVectorElementsVT();
    auto QuarterVecTy = HalfVecTy.getHalfNumVectorElementsVT();
    SDValue v0 = LowerBUILD_VECTOR_x8(Op, DAG, QuarterVecTy, 0);
    SDValue v1 = LowerBUILD_VECTOR_x8(Op, DAG, QuarterVecTy, 8);
    SDValue v2 = LowerBUILD_VECTOR_x8(Op, DAG, QuarterVecTy, 16);
    SDValue v3 = LowerBUILD_VECTOR_x8(Op, DAG, QuarterVecTy, 24);
    SDValue chain0 = DAG.getUNDEF(HalfVecTy);
    chain0 = DAG.getTargetInsertSubreg(AIE::sub_256bit_lo, DL, HalfVecTy,
                                       chain0, v0);
    chain0 = DAG.getTargetInsertSubreg(AIE::sub_256bit_hi, DL, HalfVecTy,
                                       chain0, v1);
    SDValue chain1 = DAG.getUNDEF(HalfVecTy);
    chain1 = DAG.getTargetInsertSubreg(AIE::sub_256bit_lo, DL, HalfVecTy,
                                       chain1, v2);
    chain1 = DAG.getTargetInsertSubreg(AIE::sub_256bit_hi, DL, HalfVecTy,
                                       chain1, v3);
    SDValue chain = DAG.getUNDEF(Op.getValueType());
    chain =
        DAG.getTargetInsertSubreg(AIE::sub_512bit_lo, DL, VecTy, chain, chain0);
    chain =
        DAG.getTargetInsertSubreg(AIE::sub_512bit_hi, DL, VecTy, chain, chain1);
    return chain;
  }
  return SDValue();
}
SDValue AIE1TargetLowering::LowerDYNAMIC_STACKALLOC(SDValue Op,
                                                    SelectionDAG &DAG) const {
  SDValue Chain = Op.getOperand(0); // Legalize the chain.
  SDValue Size = Op.getOperand(1);  // Legalize the size.
  MaybeAlign Alignment =
      cast<ConstantSDNode>(Op.getOperand(2))->getMaybeAlignValue();
  const MachineFunction &MF = DAG.getMachineFunction();
  Align StackAlign =
      MF.getSubtarget<AIESubtarget>().getFrameLowering()->getStackAlign();
  EVT VT = Size->getValueType(0);
  SDLoc dl(Op);

  // TODO: implement over-aligned alloca. (Note: also implies
  // supporting support for overaligned function frames + dynamic
  // allocations, at all, which currently isn't supported)
  if (Alignment && *Alignment > StackAlign) {
    report_fatal_error("Function \"" + Twine(MF.getName()) +
                       "\": "
                       "over-aligned dynamic alloca not supported.");
  }

  unsigned SPReg = AIE::SP;
  SDValue SP = DAG.getCopyFromReg(Chain, dl, SPReg, VT);
  DAG.getNode(AIE::PADDA_sp_imm, dl, VT, Chain, Size); // Value

  SDValue Ops[2] = {SP, Chain};
  return DAG.getMergeValues(Ops, dl);
}

void AIE1TargetLowering::ReplaceNodeResults(SDNode *N,
                                            SmallVectorImpl<SDValue> &Results,
                                            SelectionDAG &DAG) const {
  SDLoc DL(N);
  SDValue Op(N, 0);
  switch (N->getOpcode()) {

  case ISD::TargetGlobalAddress: {
    GlobalAddressSDNode *NO = cast<GlobalAddressSDNode>(Op);
    int64_t Offset = NO->getOffset();
    SDValue Addr =
        DAG.getTargetGlobalAddress(NO->getGlobal(), DL, MVT::i32, Offset);
    Results.push_back(Addr);
    break;
  }
  case ISD::GlobalAddress:
  case ISD::GlobalTLSAddress: {
    // No threads... TLS storage is regular storage
    GlobalAddressSDNode *NO = cast<GlobalAddressSDNode>(Op);
    int64_t Offset = NO->getOffset();
    SDValue Addr = DAG.getTargetGlobalAddress(NO->getGlobal(), DL, MVT::i32,
                                              Offset, AIEII::MO_GLOBAL);
    Results.push_back(
        DAG.getNode(AIEISD::GLOBALADDRESSWRAPPER, DL, MVT::i32, Addr));
    break;
  }
  case ISD::VAARG: {
    // Delegate to LowerVAARG before caring about legalizing types.
    LowerOperationWrapper(N, Results, DAG);
    break;
  }
  }
}

SDValue AIE1TargetLowering::LowerTargetGlobalAddress(SDValue Op,
                                                     SelectionDAG &DAG) const {
  SDLoc DL(Op);
  GlobalAddressSDNode *N = cast<GlobalAddressSDNode>(Op);
  int64_t Offset = N->getOffset();
  if (isPositionIndependent())
    report_fatal_error("Unable to lowerTargetGlobalAddress");

  SDValue Addr = DAG.getTargetGlobalAddress(
      N->getGlobal(), DL, Op.getValueType(), Offset, AIEII::MO_GLOBAL);
  return Addr;
}

SDValue AIE1TargetLowering::LowerGlobalAddress(SDValue Op,
                                               SelectionDAG &DAG) const {
  SDLoc DL(Op);
  EVT Ty = Op.getValueType();
  GlobalAddressSDNode *N = cast<GlobalAddressSDNode>(Op);
  int64_t Offset = N->getOffset();

  if (isPositionIndependent())
    report_fatal_error("Unable to lowerGlobalAddress");

  SDValue Addr = DAG.getTargetGlobalAddress(N->getGlobal(), DL, Ty, Offset,
                                            AIEII::MO_GLOBAL);
  return DAG.getNode(AIEISD::GLOBALADDRESSWRAPPER, DL, Ty, Addr);
}

SDValue AIE1TargetLowering::LowerConstantPool(SDValue Op,
                                              SelectionDAG &DAG) const {
  SDLoc DL(Op);
  EVT PtrVT = getPointerTy(DAG.getDataLayout());
  ConstantPoolSDNode *N = cast<ConstantPoolSDNode>(Op);

  SDValue Addr = DAG.getTargetConstantPool(N->getConstVal(), PtrVT,
                                           N->getAlign(), N->getOffset());
  return DAG.getNode(AIEISD::GLOBALADDRESSWRAPPER, DL, PtrVT, Addr);
}

/// Use custom function instead of CCState::AnalyzeCallOperands so it is
/// possible to change the CC for each argument.
void AIE1TargetLowering::analyzeCallOperands(
    const TargetLowering::CallLoweringInfo &CLI, CCState &CCInfo) const {
  const SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  unsigned NumOps = Outs.size();
  bool UsesVarargCC = false; // Whether the vararg CC was used
  for (unsigned i = 0; i != NumOps; ++i) {
    if (!Outs[i].IsFixed && !UsesVarargCC) {
      alignFirstVASlot(CCInfo);
    }
    UsesVarargCC |= !Outs[i].IsFixed;
    assert((!UsesVarargCC || !Outs[i].IsFixed) &&
           "Got a fixed argument after varargs");
    MVT ArgVT = Outs[i].VT;
    ISD::ArgFlagsTy ArgFlags = Outs[i].Flags;
    CCAssignFn *AssignFn = CCAssignFnForCall(UsesVarargCC);
    if (AssignFn(i, ArgVT, ArgVT, CCValAssign::Full, ArgFlags, CCInfo)) {
#ifndef NDEBUG
      dbgs() << "Call operand #" << i << " has unhandled type "
             << EVT(ArgVT).getEVTString() << '\n';
#endif
      llvm_unreachable(nullptr);
    }
  }
}

// Lower a call to a callseq_start + CALL + callseq_end chain, and add input
// and output parameter nodes.
SDValue AIE1TargetLowering::LowerCall(CallLoweringInfo &CLI,
                                      SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  bool &IsTailCall = CLI.IsTailCall;
  CallingConv::ID CallConv = CLI.CallConv;
  bool IsVarArg = CLI.IsVarArg;

  EVT PtrVT = getPointerTy(DAG.getDataLayout());

  MachineFunction &MF = DAG.getMachineFunction();

  // Analyze the operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState ArgCCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  analyzeCallOperands(CLI, ArgCCInfo);

  // Check if it's really possible to do a tail call.
  if (IsTailCall)
    IsTailCall = isEligibleForTailCallOptimization(ArgCCInfo, CLI, MF, ArgLocs);

  if (IsTailCall)
    ++NumTailCalls;
  else if (CLI.CB && CLI.CB->isMustTailCall())
    report_fatal_error("failed to perform tail call elimination on a call "
                       "site marked musttail");

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = ArgCCInfo.getStackSize();

  // Create local copies for byval args
  SmallVector<SDValue, 8> ByValArgs;
  for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
    ISD::ArgFlagsTy Flags = Outs[i].Flags;
    if (!Flags.isByVal())
      continue;

    SDValue Arg = OutVals[i];
    unsigned Size = Flags.getByValSize();
    Align align = Flags.getNonZeroByValAlign();

    int FI = MF.getFrameInfo().CreateStackObject(Size, align, /*isSS=*/false);
    SDValue FIPtr = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
    SDValue SizeNode = DAG.getConstant(Size, DL, MVT::i32);

    Chain = DAG.getMemcpy(Chain, DL, FIPtr, Arg, SizeNode, align,
                          /*IsVolatile=*/false,
                          /*AlwaysInline=*/false, IsTailCall,
                          MachinePointerInfo(), MachinePointerInfo());
    ByValArgs.push_back(FIPtr);
  }

  if (!IsTailCall)
    Chain = DAG.getCALLSEQ_START(Chain, NumBytes, 0, CLI.DL);

  // Copy argument values to their designated locations.
  SmallVector<std::pair<unsigned, SDValue>, 8> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  for (unsigned i = 0, realArgIdx = 0, byvalArgIdx = 0, e = ArgLocs.size();
       i != e; ++i, ++realArgIdx) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue = OutVals[realArgIdx];

    ISD::ArgFlagsTy Flags = Outs[realArgIdx].Flags;

    // Use local copy if it is a byval arg.
    if (Flags.isByVal()) {
      ArgValue = ByValArgs[byvalArgIdx++];
      if (!ArgValue) {
        continue;
      }
    }
    if (VA.needsCustom() && VA.getLocVT() == MVT::v2i32) {
      SDValue Elem0 = DAG.getNode(
          ISD::EXTRACT_VECTOR_ELT, DL, MVT::i32, ArgValue,
          DAG.getConstant(0, DL, getVectorIdxTy(DAG.getDataLayout())));
      SDValue Elem1 = DAG.getNode(
          ISD::EXTRACT_VECTOR_ELT, DL, MVT::i32, ArgValue,
          DAG.getConstant(1, DL, getVectorIdxTy(DAG.getDataLayout())));
      if (VA.isRegLoc()) {
        RegsToPass.push_back(std::make_pair(VA.getLocReg(), Elem0));
        assert(i + 1 != e);
        CCValAssign &NextVA = ArgLocs[++i];
        if (NextVA.isRegLoc()) {
          RegsToPass.push_back(std::make_pair(NextVA.getLocReg(), Elem1));
        } else {
          // Note that the code here needs to match
          // AIERegisterInfo::eliminateFrameIndex.
          SDValue PtrOff =
              DAG.getIntPtrConstant(-(int64_t)NextVA.getLocMemOffset() - 4, DL);
          // Emit the store as a pseudo-op that will get lowered later
          MemOpChains.push_back(Chain = DAG.getNode(AIEISD::STACK_SAVE, DL,
                                                    MVT::Other, Chain, Elem1,
                                                    PtrOff));
        }
      } else {
        SDValue PtrOff =
            DAG.getIntPtrConstant(-(int64_t)VA.getLocMemOffset() - 4, DL);
        MemOpChains.push_back(Chain = DAG.getNode(AIEISD::STACK_SAVE, DL,
                                                  MVT::Other, Chain, Elem0,
                                                  PtrOff));

        // Store the second part.
        PtrOff = DAG.getIntPtrConstant(-(int64_t)VA.getLocMemOffset() - 8, DL);
        ;
        MemOpChains.push_back(Chain = DAG.getNode(AIEISD::STACK_SAVE, DL,
                                                  MVT::Other, Chain, Elem1,
                                                  PtrOff));
      }
      continue;
    }
    if (VA.isRegLoc()) {
      // Queue up the argument copies and emit them at the end.
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), ArgValue));
    } else {
      assert(VA.isMemLoc() && "Argument not register or memory");
      // Note that the code here needs to match
      // AIERegisterInfo::eliminateFrameIndex.
      SDValue PtrOff =
          DAG.getIntPtrConstant(-(int64_t)VA.getLocMemOffset() - 4, DL);

      // Emit the store as a pseudo-op that will get lowered later
      MemOpChains.push_back(Chain =
                                DAG.getNode(AIEISD::STACK_SAVE, DL, MVT::Other,
                                            Chain, ArgValue, PtrOff));
    }
  }
  // Join the stores, which are independent of one another.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

  SDValue Glue;

  // Build a sequence of copy-to-reg nodes, chained and glued together.
  for (auto &Reg : RegsToPass) {
    Chain = DAG.getCopyToReg(Chain, DL, Reg.first, Reg.second, Glue);
    Glue = Chain.getValue(1);
  }

  // If the callee is a GlobalAddress/ExternalSymbol node, turn it into a
  // TargetGlobalAddress/TargetExternalSymbol node so that legalize won't
  // split it and then direct call can be matched by PseudoCALL.
  if (GlobalAddressSDNode *S = dyn_cast<GlobalAddressSDNode>(Callee)) {
    Callee = DAG.getTargetGlobalAddress(S->getGlobal(), DL, PtrVT);
  } else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee)) {
    Callee = DAG.getTargetExternalSymbol(S->getSymbol(), PtrVT);
  }

  // The first call operand is the chain and the second is the target address.
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (auto &Reg : RegsToPass)
    Ops.push_back(DAG.getRegister(Reg.first, Reg.second.getValueType()));

  if (!IsTailCall) {
    // Add a register mask operand representing the call-preserved registers.
    const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
    const uint32_t *Mask = TRI->getCallPreservedMask(MF, CallConv);
    assert(Mask && "Missing call preserved mask for calling convention");
    Ops.push_back(DAG.getRegisterMask(Mask));
  }

  // Glue the call to the argument copies, if any.
  if (Glue.getNode())
    Ops.push_back(Glue);

  // Emit the call.
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  if (IsTailCall) {
    MF.getFrameInfo().setHasTailCall();
    return DAG.getNode(AIEISD::TAIL, DL, NodeTys, Ops);
  }

  Chain = DAG.getNode(AIEISD::CALL, DL, NodeTys, Ops);
  Glue = Chain.getValue(1);

  // Mark the end of the call, which is glued to the call itself.
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getConstant(NumBytes, DL, PtrVT, true),
                             DAG.getConstant(0, DL, PtrVT, true), Glue, DL);
  Glue = Chain.getValue(1);
  SmallVector<CCValAssign, 16> RVLocs;
  CCState RetCCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());
  RetCCInfo.AllocateStack(ArgCCInfo.getStackSize(), Align(4));
  RetCCInfo.AnalyzeCallResult(Ins, CCAssignFnForReturn());
  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    if (RVLocs[i].getLocVT() == MVT::v2i32) {
      SDValue Vec = DAG.getNode(ISD::UNDEF, DL, MVT::v2i32);
      SDValue Lo = DAG.getCopyFromReg(Chain, DL, RVLocs[i++].getLocReg(),
                                      MVT::i32, Glue);
      Chain = Lo.getValue(1);
      Glue = Lo.getValue(2);
      Vec = DAG.getNode(ISD::INSERT_VECTOR_ELT, DL, MVT::v2i32, Vec, Lo,
                        DAG.getConstant(0, DL, MVT::i32));
      SDValue Hi =
          DAG.getCopyFromReg(Chain, DL, RVLocs[i].getLocReg(), MVT::i32, Glue);
      Chain = Hi.getValue(1);
      Glue = Hi.getValue(2);
      Vec = DAG.getNode(ISD::INSERT_VECTOR_ELT, DL, MVT::v2i32, Vec, Hi,
                        DAG.getConstant(1, DL, MVT::i32));
      InVals.push_back(Vec);
    } else if (RVLocs[i].isRegLoc()) {
      SDValue RetValue;
      // Copy the value out
      RetValue = DAG.getCopyFromReg(Chain, DL, RVLocs[i].getLocReg(),
                                    RVLocs[i].getLocVT(), Glue);
      // Glue the RetValue to the end of the call sequence
      Chain = RetValue.getValue(1);
      Glue = RetValue.getValue(2);
      InVals.push_back(RetValue);
    } else {
      llvm_unreachable("Return location not a register");
    }
  }
  return Chain;
}

bool AIE1TargetLowering::CanLowerReturn(
    CallingConv::ID CallConv, MachineFunction &MF, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, Context);

  //    FIXME: ??
  //      for (unsigned i = 0, e = Outs.size(); i != e; ++i) {
  //     MVT VT = Outs[i].VT;
  //     ISD::ArgFlagsTy ArgFlags = Outs[i].Flags;
  //     AIEABI::ABI ABI = MF.getSubtarget<AIESubtarget>().getTargetABI();
  //     if (CC_AIE(MF.getDataLayout(), ABI, i, VT, VT, CCValAssign::Full,
  //                  ArgFlags, CCInfo, /*IsFixed=*/true, /*IsRet=*/true,
  //                  nullptr))
  //       return false;
  // }

  return true;
}

SDValue
AIE1TargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                bool IsVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                const SDLoc &DL, SelectionDAG &DAG) const {
  // Stores the assignment of the return value to a location.
  SmallVector<CCValAssign, 16> RVLocs;

  // Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                 *DAG.getContext());

  // Analyze return values.
  if (!IsVarArg)
    CCInfo.AllocateStack(0, Align(4)); // AFI->getReturnStackOffset(), 4);

  CCInfo.AnalyzeReturn(Outs, CCAssignFnForReturn());

  SDValue Glue;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0, e = RVLocs.size(); i < e; ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");
    /*
    if (VA.getLocVT() == MVT::i32 && VA.getValVT() == MVT::f64) {
      // Handle returning f64 on RV32D with a soft float ABI.
      assert(VA.isRegLoc() && "Expected return via registers");
      SDValue SplitF64 = DAG.getNode(AIEISD::SplitF64, DL,
                                     DAG.getVTList(MVT::i32, MVT::i32), Val);
      SDValue Lo = SplitF64.getValue(0);
      SDValue Hi = SplitF64.getValue(1);
      unsigned RegLo = VA.getLocReg();
      unsigned RegHi = RegLo + 1;
      Chain = DAG.getCopyToReg(Chain, DL, RegLo, Lo, Glue);
      Glue = Chain.getValue(1);
      RetOps.push_back(DAG.getRegister(RegLo, MVT::i32));
      Chain = DAG.getCopyToReg(Chain, DL, RegHi, Hi, Glue);
      Glue = Chain.getValue(1);
      RetOps.push_back(DAG.getRegister(RegHi, MVT::i32));
    } else {
    */
    // Handle a 'normal' return.
    //      Val = convertValVTToLocVT(DAG, Val, VA, DL);
    if (VA.needsCustom() && VA.getLocVT() == MVT::v2i32) {
      // Legalize ret of type v2i32 -> ret 2 x i32
      // TODO revisit this code to refactor
      SDValue Elem0 = DAG.getNode(
          ISD::EXTRACT_VECTOR_ELT, DL, MVT::i32, Val,
          DAG.getConstant(0, DL, getVectorIdxTy(DAG.getDataLayout())));
      SDValue Elem1 = DAG.getNode(
          ISD::EXTRACT_VECTOR_ELT, DL, MVT::i32, Val,
          DAG.getConstant(1, DL, getVectorIdxTy(DAG.getDataLayout())));

      Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Elem0, Glue);
      Glue = Chain.getValue(1);
      RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
      VA = RVLocs[++i]; // next result value location
      Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Elem1, Glue);
    } else {
      Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Glue);
    }

    // Guarantee that all emitted copies are stuck together.
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    // }
  }

  RetOps[0] = Chain; // Update chain.

  // Add the glue node if we have it.
  if (Glue.getNode()) {
    RetOps.push_back(Glue);
  }

  // Interrupt service routines use different return instructions.
  const Function &Func = DAG.getMachineFunction().getFunction();
  if (Func.hasFnAttribute("interrupt")) {
    report_fatal_error("Interrupt Functions not supported!");
    //   if (!Func.getReturnType()->isVoidTy())
    //     report_fatal_error(
    //         "Functions with the interrupt attribute must have void return
    //         type!");

    //   MachineFunction &MF = DAG.getMachineFunction();
    //   StringRef Kind =
    //     MF.getFunction().getFnAttribute("interrupt").getValueAsString();

    //   unsigned RetOpc;
    //   if (Kind == "user")
    //     RetOpc = AIEISD::URET_FLAG;
    //   else if (Kind == "supervisor")
    //     RetOpc = AIEISD::SRET_FLAG;
    //   else
    //     RetOpc = AIEISD::MRET_FLAG;

    //   return DAG.getNode(RetOpc, DL, MVT::Other, RetOps);
  }
  return DAG.getNode(AIEISD::RET_FLAG, DL, MVT::Other, RetOps);
}

const char *AIE1TargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch ((AIEISD::NodeType)Opcode) {
  case AIEISD::FIRST_NUMBER:
    break;
  case AIEISD::RET_FLAG:
    return "AIEISD::RET_FLAG";
  case AIEISD::CALL:
    return "AIEISD::CALL";
  case AIEISD::SELECT_CC:
    return "AIEISD::SELECT_CC";
  case AIEISD::BuildPairF64:
    return "AIEISD::BuildPairF64";
  case AIEISD::SplitF64:
    return "AIEISD::SplitF64";
  case AIEISD::TAIL:
    return "AIEISD::TAIL";
  case AIEISD::GLOBALADDRESSWRAPPER:
    return "AIEISD::GLOBALADDRESSWRAPPER";
  case AIEISD::GPR_CAST:
    return "AIEISD::GPR_CAST";
  case AIEISD::STACK_SAVE:
    return "AIEISD::STACK_SAVE";
  case AIEISD::STACK_RESTORE:
    return "AIEISD::STACK_RESTORE";
  }
  return nullptr;
}

std::pair<unsigned, const TargetRegisterClass *>
AIE1TargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                 StringRef Constraint,
                                                 MVT VT) const {
  // First, see if this is a constraint that directly corresponds to a
  // AIE register class.
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    case 'r':
      return std::make_pair(0U, &AIE::GPRRegClass);
    default:
      break;
    }
  }

  return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}

EVT AIE1TargetLowering::getShiftAmountTy(EVT LHSTy, const DataLayout &DL,
                                         bool LegalTypes) const {
  assert(LHSTy.isInteger() && "Shift amount is not an integer type!");
  if (LHSTy.isVector())
    return LHSTy;
  if (LHSTy.isInteger())
    return LHSTy;
  return LegalTypes ? getScalarShiftAmountTy(DL, LHSTy) : getPointerTy(DL);
}

EVT AIE1TargetLowering::getSetCCResultType(const DataLayout &DL,
                                           LLVMContext &Context, EVT VT) const {
  if (!VT.isVector())
    return MVT::i32;
  return VT.changeVectorElementTypeToInteger();
}

// The caller is responsible for loading the full value if the argument is
// passed with CCValAssign::Indirect.
static SDValue unpackFromRegLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL) {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  EVT LocVT = VA.getLocVT();
  SDValue Val;
  const TargetRegisterClass *RC;

  switch (LocVT.getSimpleVT().SimpleTy) {
  default:
    llvm_unreachable("Unexpected register type");
  case MVT::i8:
  case MVT::i16:
  case MVT::i20:
  case MVT::i32:
  case MVT::f32:
  case MVT::v8i1:
  case MVT::v16i1:
    RC = &AIE::GPRRegClass;
    break;
  case MVT::v2i32:
    RC = &AIE::mCRegClass;
    break;
  case MVT::v16i8:
  case MVT::v8i16:
  case MVT::v4i32:
  case MVT::v4f32:
    RC = &AIE::VEC128RegClass;
    break;
  case MVT::v32i8:
  case MVT::v16i16:
  case MVT::v8i32:
  case MVT::v8f32:
    RC = &AIE::VEC256RegClass;
    break;
  case MVT::v64i8:
  case MVT::v32i16:
  case MVT::v16i32:
  case MVT::v16f32:
    RC = &AIE::VEC512RegClass;
    break;
  case MVT::v128i8:
  case MVT::v64i16:
  case MVT::v32i32:
  case MVT::v32f32:
    RC = &AIE::VEC1024RegClass;
    break;
  case MVT::v8i48:
    RC = &AIE::ACC384RegClass;
    break;
  case MVT::v16i48:
    RC = &AIE::ACC768RegClass;
    break;

    // case MVT::f32:
    //   RC = &AIE::FPR32RegClass;
    //   break;
    // case MVT::f64:
    //   RC = &AIE::FPR64RegClass;
    //   break;
  }

  unsigned VReg = RegInfo.createVirtualRegister(RC);
  RegInfo.addLiveIn(VA.getLocReg(), VReg);
  if (LocVT.getSimpleVT().SimpleTy == MVT::v2i32)
    Val = DAG.getCopyFromReg(Chain, DL, VReg, MVT::i32);
  else
    Val = DAG.getCopyFromReg(Chain, DL, VReg, LocVT);

  // If this is an 8 or 16-bit value, it is really passed promoted
  // to 32 bits.  Insert an assert[sz]ext to capture this, then
  // truncate to the right size.
  switch (VA.getLocInfo()) {
  default:
    llvm_unreachable("Unknown loc info!");
  case CCValAssign::AExt:
    break;
  case CCValAssign::Full:
    break;
  case CCValAssign::BCvt:
    Val = DAG.getNode(ISD::BITCAST, DL, VA.getValVT(), Val);
    break;
  case CCValAssign::SExt:
    Val = DAG.getNode(ISD::AssertSext, DL, LocVT, Val,
                      DAG.getValueType(VA.getValVT()));
    Val = DAG.getNode(ISD::TRUNCATE, DL, VA.getValVT(), Val);
    break;
  case CCValAssign::ZExt:
    Val = DAG.getNode(ISD::AssertZext, DL, LocVT, Val,
                      DAG.getValueType(VA.getValVT()));
    Val = DAG.getNode(ISD::TRUNCATE, DL, VA.getValVT(), Val);
    break;
  }
  if (VA.getLocInfo() == CCValAssign::Indirect)
    return Val;

  return Val; // convertLocVTToValVT(DAG, Val, VA, DL);
}

/*
static SDValue convertValVTToLocVT(SelectionDAG &DAG, SDValue Val,
                                   const CCValAssign &VA, const SDLoc &DL) {
  EVT LocVT = VA.getLocVT();

  switch (VA.getLocInfo()) {
  default:
    llvm_unreachable("Unexpected CCValAssign::LocInfo");
  case CCValAssign::Full:
    break;
  case CCValAssign::BCvt:
    if (VA.getLocVT() == MVT::i64 && VA.getValVT() == MVT::f32) {
      Val = DAG.getNode(AIEISD::FMV_X_ANYEXTW_RV64, DL, MVT::i64, Val);
      break;
    }
    Val = DAG.getNode(ISD::BITCAST, DL, LocVT, Val);
    break;
  }
  return Val;
}
*/

// The caller is responsible for loading the full value if the argument is
// passed with CCValAssign::Indirect.
static SDValue unpackFromMemLoc(SelectionDAG &DAG, SDValue Chain,
                                const CCValAssign &VA, const SDLoc &DL) {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  EVT LocVT = VA.getLocVT();
  EVT ValVT = VA.getValVT();
  EVT PtrVT = MVT::getIntegerVT(DAG.getDataLayout().getPointerSizeInBits(0));
  int FI = MFI.CreateFixedObject(ValVT.getSizeInBits() / 8,
                                 -(int64_t)VA.getLocMemOffset() - 4,
                                 /*Immutable=*/true);
  SDValue FIN = DAG.getFrameIndex(FI, PtrVT);
  SDValue Val;

  ISD::LoadExtType ExtType;
  switch (VA.getLocInfo()) {
  default:
    llvm_unreachable("Unexpected CCValAssign::LocInfo");
  case CCValAssign::Full:
  case CCValAssign::Indirect:
  case CCValAssign::BCvt:
    ExtType = ISD::NON_EXTLOAD;
    break;
  }
  Val = DAG.getExtLoad(
      ExtType, DL, LocVT, Chain, FIN,
      MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI), ValVT);
  return Val;
}

// Transform physical registers into virtual registers.
SDValue AIE1TargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {

  switch (CallConv) {
  default:
    report_fatal_error("Unsupported calling convention");
  case CallingConv::C:
  case CallingConv::Fast:
    break;
  }

  MachineFunction &MF = DAG.getMachineFunction();
  MachineRegisterInfo &RegInfo = MF.getRegInfo();
  bool IsLittleEndian = DAG.getDataLayout().isLittleEndian();

  // Used with vargs to acumulate store chains.
  std::vector<SDValue> OutChains;

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());
  CCInfo.AnalyzeFormalArguments(Ins, CCAssignFnForCall());

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    SDValue ArgValue;
    if (VA.isRegLoc()) {
      if (VA.needsCustom() && VA.getLocVT() == MVT::v2i32) {
        Register VRegHi = RegInfo.createVirtualRegister(&AIE::mCHRegClass);
        MF.getRegInfo().addLiveIn(VA.getLocReg(), VRegHi);
        SDValue HiVal = DAG.getCopyFromReg(Chain, DL, VRegHi, MVT::i32);
        assert(i + 1 < e);
        CCValAssign &NextVA = ArgLocs[++i];
        SDValue LoVal;
        if (NextVA.isMemLoc()) {
          int FI = MF.getFrameInfo().CreateFixedObject(
              4, -(int64_t)NextVA.getLocMemOffset() - 4, true);
          EVT PtrVT =
              MVT::getIntegerVT(DAG.getDataLayout().getPointerSizeInBits(0));
          SDValue FIN = DAG.getFrameIndex(FI, PtrVT);
          LoVal = DAG.getLoad(MVT::i32, DL, Chain, FIN, MachinePointerInfo());
        } else {
          Register VRegLo = RegInfo.createVirtualRegister(&AIE::mCLRegClass);
          MF.getRegInfo().addLiveIn(NextVA.getLocReg(), VRegLo);
          LoVal = DAG.getCopyFromReg(Chain, DL, VRegLo, MVT::i32);
        }
        // TODO AIE is little endian, so take this in account to avoid this swap
        // code
        if (IsLittleEndian)
          std::swap(LoVal, HiVal);
        // Given two values of the same integer value type, this produces a
        // value twice as big MVT::i64
        SDValue V2I32Val =
            DAG.getNode(ISD::BUILD_PAIR, DL, MVT::i64, LoVal, HiVal);
        V2I32Val = DAG.getNode(ISD::BITCAST, DL, VA.getLocVT(), V2I32Val);
        InVals.push_back(V2I32Val);
        continue;
      } else {
        ArgValue = unpackFromRegLoc(DAG, Chain, VA, DL);
        InVals.push_back(ArgValue);
        continue;
      }
    } else {
      assert(VA.isMemLoc());
      if (VA.needsCustom() && VA.getLocVT() == MVT::v2i32) {
        int FI = MF.getFrameInfo().CreateFixedObject(
            4, -(int64_t)VA.getLocMemOffset() - 4, true);
        EVT PtrVT =
            MVT::getIntegerVT(DAG.getDataLayout().getPointerSizeInBits(0));
        SDValue FIN = DAG.getFrameIndex(FI, PtrVT);
        SDValue HiVal =
            DAG.getLoad(MVT::i32, DL, Chain, FIN, MachinePointerInfo());
        int FI2 = MF.getFrameInfo().CreateFixedObject(
            4, -(int64_t)VA.getLocMemOffset(), true);
        SDValue FIN1 = DAG.getFrameIndex(FI2, PtrVT);
        SDValue LoVal =
            DAG.getLoad(MVT::i32, DL, Chain, FIN1, MachinePointerInfo());
        // TODO AIE is little endian, so take this in account to avoid this swap
        // code
        if (IsLittleEndian)
          std::swap(LoVal, HiVal);
        SDValue V2I32Val =
            DAG.getNode(ISD::BUILD_PAIR, DL, MVT::i64, LoVal, HiVal);
        V2I32Val = DAG.getNode(ISD::BITCAST, DL, VA.getValVT(), V2I32Val);
        InVals.push_back(V2I32Val);
        continue;
      } else {
        ArgValue = unpackFromMemLoc(DAG, Chain, VA, DL);
        InVals.push_back(ArgValue);
        continue;
      }
    }
  }

  if (IsVarArg) {
    MachineFrameInfo &MFI = MF.getFrameInfo();
    AIEMachineFunctionInfo *FuncInfo = MF.getInfo<AIEMachineFunctionInfo>();

    // Offset of the end address of the first vararg from stack pointer
    // The stack grows up, and we allocate close to SP first, so we cannot know
    // the begin address. This would be (VaArgEndOffset - AlignedVarArgSize),
    // but AlignedVarArgSize is only known when meeting a va_arg instruction.
    int FirstVAEndOffset = -alignTo(CCInfo.getStackSize(), Align(32));

    // Record the frame index for FirstVAEndOffset, it will be used by VASTART.
    int FI = MFI.CreateFixedObject(4, FirstVAEndOffset, true);
    FuncInfo->setVarArgsFrameIndex(FI);
  }

  // All stores are grouped in one node to allow the matching between
  // the size of Ins and InVals. This only happens for vararg functions.
  if (!OutChains.empty()) {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, OutChains);
  }
  return Chain;
}
