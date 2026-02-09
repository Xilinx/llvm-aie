//=== lib/Target/AIE/aie2ps/AIE2PSConvertFP16ScalarOperation.cpp
//-------------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===--------------------------------------------------------------------===//
//
// This pass converts float16 scalar operation into AIE2PS intrinsic before
// IRTranslator since GloblISel will drop type information
//
//===--------------------------------------------------------------------===//
#include "AIE2PS.h"
#include "llvm/Analysis/TargetFolder.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/IntrinsicsAIE2PS.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"

#define DEBUG_TYPE "aie2ps-fp16-converter"

using namespace llvm;

namespace {
/// Implementation class that visits and transforms FP16 scalar operations.
/// This visitor pattern-based implementation traverses instructions and
/// converts FP16 operations to use AIE2PS intrinsics.
class AIE2PSConvertFP16ScalarOperationImpl
    : public InstVisitor<AIE2PSConvertFP16ScalarOperationImpl, Instruction *> {
public:
  AIE2PSConvertFP16ScalarOperationImpl(Function &F) : Builder(F.getContext()) {}

  /// Main implementation function that iterates through all instructions
  /// in the function and applies appropriate transformations.
  /// Returns true if any changes were made to the function.
  bool runImpl(Function &F) {
    bool Changed = false;
    // Iterate through all basic blocks in the function
    for (auto &B : F) {
      SmallVector<Instruction *> WorkList;
      // Build worklist of instructions, skipping existing call instructions
      // to avoid transforming already-converted intrinsics
      for (auto &I : B) {
        if (isa<CallInst>(&I))
          continue;
        WorkList.push_back(&I);
      }
      // Process each instruction using the visitor pattern
      for (auto I : WorkList) {
        if (Instruction *Result = visit(*I)) {
          // Check if the visitor returned a replacement instruction
          if (Result != I) {
            LLVM_DEBUG(dbgs() << "IC: Old = " << *I << '\n'
                              << "    New = " << *Result << '\n');

            // Preserve debug information: copy the old instruction's DebugLoc
            // to the new instruction, unless a more specific one was already
            // set.
            if (!Result->getDebugLoc())
              Result->setDebugLoc(I->getDebugLoc());
            // Preserve annotation metadata from the original instruction
            Result->copyMetadata(*I, LLVMContext::MD_annotation);
            // Replace all uses of the old instruction with the new one
            I->replaceAllUsesWith(Result);

            // Transfer the instruction name and remove the old instruction
            Result->takeName(I);
            I->eraseFromParent();
            Changed = true;
          }
        }
      }
    }
    return Changed;
  }

  // Visitor methods for different instruction types
  /// Convert FP16 floating-point negation to AIE2PS intrinsic
  Instruction *visitFNeg(UnaryOperator &I);
  /// Convert FP16 floating-point addition to FP32 add with conversions
  Instruction *visitFAdd(BinaryOperator &I);
  /// Convert FP16 floating-point subtraction to FP32 sub with conversions
  Instruction *visitFSub(BinaryOperator &I);
  /// Convert FP16 floating-point multiplication to AIE2PS intrinsic
  Instruction *visitFMul(BinaryOperator &I);
  /// Convert FP16 floating-point division to FP32 div with conversions
  Instruction *visitFDiv(BinaryOperator &I);
  /// Convert FP16 floating-point remainder to FP32 rem with conversions
  Instruction *visitFRem(BinaryOperator &I);
  /// Convert FP32->FP16 truncation using AIE2PS intrinsic
  Instruction *visitFPTrunc(FPTruncInst &I);
  /// Convert FP16->FP32 extension using AIE2PS intrinsic
  Instruction *visitFPExt(FPExtInst &I);
  /// Convert signed integer to FP16 via FP32 intermediate
  Instruction *visitSIToFP(SIToFPInst &I);
  /// Convert unsigned integer to FP16 via FP32 intermediate
  Instruction *visitUIToFP(UIToFPInst &I);
  /// Convert FP16 to signed integer via FP32 intermediate
  Instruction *visitFPToSI(FPToSIInst &I);
  /// Convert FP16 to unsigned integer via FP32 intermediate
  Instruction *visitFPToUI(FPToUIInst &I);
  /// Convert FP16 comparisons to AIE2PS intrinsic comparisons
  Instruction *visitFCmp(FCmpInst &I);
  /// Default handler for instructions that don't need transformation
  Instruction *visitInstruction(Instruction &I) { return &I; }

private:
  /// Convert a vector of FP32 values to FP16 using AIE2PS intrinsic
  Instruction *convertFP32ToFP16(Value *VecFP32, Instruction &I);
  /// Convert an FP16 value to FP32 using AIE2PS intrinsic
  Instruction *convertFP16ToFP32(Value *FP16, Instruction &I);
  /// Perform FP16 comparison using AIE2PS intrinsics
  Instruction *compareFP16(Value *Op0, Value *Op1, Intrinsic::ID Id,
                           bool Reverse, Instruction &I);
  /// Create a 32-element vector with the operand in the first position
  Value *getVector32FP(Value *Op, Instruction &I);
  /// IR builder for creating new instructions
  IRBuilder<> Builder;
};

/// LLVM function pass that converts FP16 scalar operations into AIE2PS
/// intrinsics. This pass is necessary because GlobalISel drops type
/// information, so we need to convert FP16 operations before the IR translation
/// phase.
class AIE2PSConvertFP16ScalarOperationPass : public FunctionPass {
public:
  static char ID; // Pass identification, replacement for typeid
  AIE2PSConvertFP16ScalarOperationPass() : FunctionPass(ID) {
    initializeAIE2PSConvertFP16ScalarOperationPassPass(
        *PassRegistry::getPassRegistry());
  }

  /// Declare that this pass preserves the Control Flow Graph
  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesCFG();
  }

  /// Convert FP16 operations into AIE2PS intrinsics.
  /// Runs twice if changes are made to handle newly created FPExt/FPTrunc
  /// instructions.
  bool runOnFunction(Function &F) override {
    if (skipFunction(F))
      return false;
    AIE2PSConvertFP16ScalarOperationImpl Impl(F);
    bool Changed = false;
    // First pass: convert original FP16 operations
    Changed |= Impl.runImpl(F);
    if (Changed) {
      // Second pass: convert any newly created FPExtInst or FPTruncInst
      // generated during the first pass
      Impl.runImpl(F);
    }
    return Changed;
  }
};

/// Convert FP16 addition by extending to FP32, performing the addition,
/// and truncating back to FP16. This workaround is needed because native
/// FP16 arithmetic is not directly supported in the target.
Instruction *
AIE2PSConvertFP16ScalarOperationImpl::visitFAdd(BinaryOperator &I) {
  // Only process FP16 operations
  if (!I.getType()->isHalfTy()) {
    return &I;
  }
  Builder.SetInsertPoint(&I);
  auto FPTy = Type::getFloatTy(I.getContext());
  // Extend both operands from FP16 to FP32
  auto Fp0 = Builder.CreateFPExt(I.getOperand(0), FPTy);
  auto Fp1 = Builder.CreateFPExt(I.getOperand(1), FPTy);
  // Perform FP32 addition
  auto FPAddRes = Builder.CreateFAdd(Fp0, Fp1);
  // Truncate result back to FP16
  auto Res = Builder.CreateFPTrunc(FPAddRes, I.getType());
  return cast<Instruction>(Res);
}

/// Convert FP16 subtraction using the extend-operate-truncate pattern.
/// Extends operands to FP32, performs subtraction, then truncates to FP16.
Instruction *
AIE2PSConvertFP16ScalarOperationImpl::visitFSub(BinaryOperator &I) {
  // Only process FP16 operations
  if (!I.getType()->isHalfTy()) {
    return &I;
  }
  Builder.SetInsertPoint(&I);
  auto FPTy = Type::getFloatTy(I.getContext());
  // Extend both operands from FP16 to FP32
  auto Fp0 = Builder.CreateFPExt(I.getOperand(0), FPTy);
  auto Fp1 = Builder.CreateFPExt(I.getOperand(1), FPTy);
  // Perform FP32 subtraction
  auto FPSubRes = Builder.CreateFSub(Fp0, Fp1);
  // Truncate result back to FP16
  auto Res = Builder.CreateFPTrunc(FPSubRes, I.getType());
  return cast<Instruction>(Res);
}

/// Convert FP16 division using the extend-operate-truncate pattern.
/// Extends operands to FP32, performs division, then truncates to FP16.
Instruction *
AIE2PSConvertFP16ScalarOperationImpl::visitFDiv(BinaryOperator &I) {
  // Only process FP16 operations
  if (!I.getType()->isHalfTy()) {
    return &I;
  }
  Builder.SetInsertPoint(&I);
  auto FPTy = Type::getFloatTy(I.getContext());
  // Extend both operands from FP16 to FP32
  auto Fp0 = Builder.CreateFPExt(I.getOperand(0), FPTy);
  auto Fp1 = Builder.CreateFPExt(I.getOperand(1), FPTy);
  // Perform FP32 division
  auto FPDivRes = Builder.CreateFDiv(Fp0, Fp1);
  // Truncate result back to FP16
  auto Res = Builder.CreateFPTrunc(FPDivRes, I.getType());
  return cast<Instruction>(Res);
}

/// Convert FP16 remainder using the extend-operate-truncate pattern.
/// Extends operands to FP32, performs remainder, then truncates to FP16.
Instruction *
AIE2PSConvertFP16ScalarOperationImpl::visitFRem(BinaryOperator &I) {
  // Only process FP16 operations
  if (!I.getType()->isHalfTy()) {
    return &I;
  }
  Builder.SetInsertPoint(&I);
  auto FPTy = Type::getFloatTy(I.getContext());
  // Extend both operands from FP16 to FP32
  auto Fp0 = Builder.CreateFPExt(I.getOperand(0), FPTy);
  auto Fp1 = Builder.CreateFPExt(I.getOperand(1), FPTy);
  // Perform FP32 remainder
  auto FPRemRes = Builder.CreateFRem(Fp0, Fp1);
  // Truncate result back to FP16
  auto Res = Builder.CreateFPTrunc(FPRemRes, I.getType());
  return cast<Instruction>(Res);
}

/// Convert FP16 multiplication using AIE2PS hardware intrinsic.
/// Unlike other operations, multiplication uses a dedicated hardware intrinsic
/// that operates on 32-element vectors and returns FP32 accumulator results.
Instruction *
AIE2PSConvertFP16ScalarOperationImpl::visitFMul(BinaryOperator &I) {
  // Only process FP16 operations
  if (!I.getType()->isHalfTy()) {
    return &I;
  }
  Builder.SetInsertPoint(&I);
  auto M = I.getFunction()->getParent();
  // Get the AIE2PS FP16 multiplication intrinsic
  auto MulFP16Intrinsic = Intrinsic::getOrInsertDeclaration(
      M, Intrinsic::aie2ps_fp16_I512_fp16_I512_ACC1024_bf_mul_conf);
  // Configuration value for the multiplication operation
  auto MulConfig = ConstantInt::get(Type::getInt32Ty(I.getContext()), 60);
  SmallVector<Value *> Args;
  // Convert scalar operands to 32-element vectors
  Args.push_back(getVector32FP(I.getOperand(0), I));
  Args.push_back(getVector32FP(I.getOperand(1), I));
  Args.push_back(MulConfig);
  // Call the intrinsic (returns FP32 accumulator vector)
  auto FPMulRes = Builder.CreateCall(MulFP16Intrinsic, Args);
  // Convert result from FP32 vector to FP16 scalar
  return convertFP32ToFP16(FPMulRes, I);
}

/// Convert FP16 multiplication using AIE2PS hardware intrinsic.
/// Unlike other operations, neg uses a dedicated hardware intrinsic
/// that operates on 32-element vectors and returns FP32 accumulator results.
Instruction *AIE2PSConvertFP16ScalarOperationImpl::visitFNeg(UnaryOperator &I) {
  // Only process FP16 operations
  if (!I.getType()->isHalfTy()) {
    return &I;
  }
  Builder.SetInsertPoint(&I);
  auto M = I.getFunction()->getParent();
  // Get the AIE2PS FP16 multiplication intrinsic
  auto MulFP16Intrinsic = Intrinsic::getOrInsertDeclaration(
      M, Intrinsic::aie2ps_fp16_I512_fp16_I512_ACC1024_bf_mul_conf);
  // Configuration value for the multiplication operation
  auto MulConfig = ConstantInt::get(Type::getInt32Ty(I.getContext()), 60);
  SmallVector<Value *> Args;
  // Convert scalar operands to 32-element vectors
  Args.push_back(getVector32FP(I.getOperand(0), I));
  Args.push_back(getVector32FP(
      ConstantFP::get(Type::getHalfTy(I.getContext()), -1.0f), I));
  Args.push_back(MulConfig);
  // Call the intrinsic (returns FP32 accumulator vector)
  auto FPMulRes = Builder.CreateCall(MulFP16Intrinsic, Args);
  // Convert result from FP32 vector to FP16 scalar
  return convertFP32ToFP16(FPMulRes, I);
}

/// Convert a 32-element FP32 accumulator vector to FP16 and extract scalar
/// result. Uses AIE2PS intrinsic to convert the vector, then extracts the first
/// element.
/// @param VecFP32 A 32-element FP32 accumulator vector from a previous
/// operation
/// @param I The instruction context for building new instructions
/// @return An instruction producing a scalar FP16 value
Instruction *
AIE2PSConvertFP16ScalarOperationImpl::convertFP32ToFP16(Value *VecFP32,
                                                        Instruction &I) {
  auto M = I.getFunction()->getParent();
  auto Int32Zero = ConstantInt::get(Type::getInt32Ty(M->getContext()), 0);
  // Get the AIE2PS intrinsic for converting FP32 accumulator to FP16 vector
  auto ConvFP32ToFP16Intrinsic = Intrinsic::getOrInsertDeclaration(
      M, Intrinsic::aie2ps_v32accfloat_to_v32float16);
  SmallVector<Value *> Args;
  Args.push_back(VecFP32);
  // Convert the FP32 accumulator vector to FP16 vector
  auto VecFP16 = Builder.CreateCall(ConvFP32ToFP16Intrinsic, Args);
  // Extract the first element (index 0) as the scalar result
  auto Res = Builder.CreateExtractElement(VecFP16, Int32Zero);
  return cast<Instruction>(Res);
}

/// Convert a scalar FP16 value to FP32 using AIE2PS intrinsic.
/// First wraps the scalar in a 32-element vector, converts to FP32 accumulator,
/// then extracts the first element.
/// @param FP16 A scalar FP16 value to convert
/// @param I The instruction context for building new instructions
/// @return An instruction producing a scalar FP32 value
Instruction *
AIE2PSConvertFP16ScalarOperationImpl::convertFP16ToFP32(Value *FP16,
                                                        Instruction &I) {
  auto Int32Zero = ConstantInt::get(Type::getInt32Ty(I.getContext()), 0);
  auto M = I.getFunction()->getParent();
  // Get the AIE2PS intrinsic for converting FP16 vector to FP32 accumulator
  auto ConvFP16ToFP32Intrinsic = Intrinsic::getOrInsertDeclaration(
      M, Intrinsic::aie2ps_v32float16_to_v32accfloat);
  SmallVector<Value *> Args;
  // Wrap the scalar FP16 in a 32-element vector
  Args.push_back(getVector32FP(FP16, I));
  // Convert to FP32 accumulator vector
  auto VecFP32 = Builder.CreateCall(ConvFP16ToFP32Intrinsic, Args);
  // Extract the first element as the scalar result
  auto Res = Builder.CreateExtractElement(VecFP32, Int32Zero);
  return cast<Instruction>(Res);
}

/// Convert FP16 to FP32 extension using AIE2PS intrinsic.
/// Handles fpext instructions that extend FP16 to FP32.
Instruction *AIE2PSConvertFP16ScalarOperationImpl::visitFPExt(FPExtInst &I) {
  auto Op = I.getOperand(0);
  // Only handle FP16->FP32 extensions
  if (!Op->getType()->isHalfTy())
    return &I;
  assert(I.getType()->isFloatTy());
  Builder.SetInsertPoint(&I);
  // Use the AIE2PS intrinsic for conversion
  return convertFP16ToFP32(Op, I);
}

/// Create a 32-element vector with the operand in position 0.
/// AIE2PS intrinsics operate on 32-element vectors, so scalars must be wrapped.
/// Uses undef for the other elements since they won't be accessed.
/// @param Op The scalar value to insert into the vector
/// @param I The instruction context for building new instructions
/// @return A 32-element vector value with Op at index 0, other elements undef
Value *AIE2PSConvertFP16ScalarOperationImpl::getVector32FP(Value *Op,
                                                           Instruction &I) {
  // Create a 32-element vector type matching the operand's type
  auto Vector32FPTy = VectorType::get(Op->getType(), 32, false);
  // Use undef for initial values (more efficient than zeros since unused)
  auto V32fpUndef = UndefValue::get(Vector32FPTy);
  auto Int32Zero = ConstantInt::get(Type::getInt32Ty(I.getContext()), 0);
  // Insert the operand at index 0 of the undef vector
  return Builder.CreateInsertElement(V32fpUndef, Op, Int32Zero);
}

/// Convert FP32 to FP16 truncation using AIE2PS intrinsic.
/// Handles fptrunc instructions that truncate FP32 to FP16.
Instruction *
AIE2PSConvertFP16ScalarOperationImpl::visitFPTrunc(FPTruncInst &I) {
  auto Op = I.getOperand(0);
  // Only handle FP32->FP16 truncations
  if (!I.getType()->isHalfTy())
    return &I;
  assert(Op->getType()->isFloatTy());
  Builder.SetInsertPoint(&I);
  // Wrap the FP32 scalar in a vector and use AIE2PS intrinsic for conversion
  return convertFP32ToFP16(getVector32FP(Op, I), I);
}

/// Convert signed integer to FP16 using two-stage conversion.
/// Since AIE2PS doesn't natively support sitofp to FP16, this converts the
/// integer to FP32 first, then uses AIE2PS intrinsics to convert FP32 to FP16.
/// @param I The sitofp instruction to convert
/// @return Instruction producing an FP16 result, or the original instruction if
/// not FP16
Instruction *AIE2PSConvertFP16ScalarOperationImpl::visitSIToFP(SIToFPInst &I) {
  // Only process conversions that target FP16
  if (!I.getType()->isHalfTy())
    return &I;
  Builder.SetInsertPoint(&I);
  // Convert signed integer to FP32 (natively supported)
  auto FP32 =
      Builder.CreateSIToFP(I.getOperand(0), Type::getFloatTy(I.getContext()));
  // Wrap FP32 scalar in vector and convert to FP16 using AIE2PS intrinsic
  return convertFP32ToFP16(getVector32FP(FP32, I), I);
}

/// Convert unsigned integer to FP16 using two-stage conversion.
/// Since AIE2PS doesn't natively support uitofp to FP16, this converts the
/// integer to FP32 first, then uses AIE2PS intrinsics to convert FP32 to FP16.
/// @param I The uitofp instruction to convert
/// @return Instruction producing an FP16 result, or the original instruction if
/// not FP16
Instruction *AIE2PSConvertFP16ScalarOperationImpl::visitUIToFP(UIToFPInst &I) {
  // Only process conversions that target FP16
  if (!I.getType()->isHalfTy())
    return &I;
  Builder.SetInsertPoint(&I);
  // Convert unsigned integer to FP32 (natively supported)
  auto FP32 =
      Builder.CreateUIToFP(I.getOperand(0), Type::getFloatTy(I.getContext()));
  // Wrap FP32 scalar in vector and convert to FP16 using AIE2PS intrinsic
  return convertFP32ToFP16(getVector32FP(FP32, I), I);
}

/// Convert FP16 to signed integer using two-stage conversion.
/// Since AIE2PS doesn't natively support fptosi from FP16, this converts the
/// FP16 to FP32 first using intrinsics, then converts FP32 to signed integer.
/// @param I The fptosi instruction to convert
/// @return Instruction producing a signed integer result, or the original
/// instruction if not FP16
Instruction *AIE2PSConvertFP16ScalarOperationImpl::visitFPToSI(FPToSIInst &I) {
  auto Op = I.getOperand(0);
  // Only process conversions from FP16
  if (!Op->getType()->isHalfTy())
    return &I;
  Builder.SetInsertPoint(&I);
  // Convert FP16 to FP32 using AIE2PS intrinsic (via vector wrapper)
  auto FP32 = convertFP16ToFP32(Op, I);
  // Convert FP32 to signed integer (natively supported)
  auto res = Builder.CreateFPToSI(FP32, I.getType());
  return cast<Instruction>(res);
}

/// Convert FP16 to unsigned integer using two-stage conversion.
/// Since AIE2PS doesn't natively support fptoui from FP16, this converts the
/// FP16 to FP32 first using intrinsics, then converts FP32 to unsigned integer.
/// @param I The fptoui instruction to convert
/// @return Instruction producing an unsigned integer result, or the original
/// instruction if not FP16
Instruction *AIE2PSConvertFP16ScalarOperationImpl::visitFPToUI(FPToUIInst &I) {
  auto Op = I.getOperand(0);
  // Only process conversions from FP16
  if (!Op->getType()->isHalfTy())
    return &I;
  Builder.SetInsertPoint(&I);
  // Convert FP16 to FP32 using AIE2PS intrinsic (via vector wrapper)
  auto FP32 = convertFP16ToFP32(Op, I);
  // Convert FP32 to unsigned integer (natively supported)
  auto res = Builder.CreateFPToUI(FP32, I.getType());
  return cast<Instruction>(res);
}

/// Perform FP16 comparison using AIE2PS vector intrinsics.
/// Wraps scalar operands in 32-element vectors, calls the comparison intrinsic,
/// and extracts the boolean result. Can reverse the result for unordered
/// predicates.
/// @param Op0 First operand (FP16 scalar)
/// @param Op1 Second operand (FP16 scalar)
/// @param Id The AIE2PS comparison intrinsic ID to use
/// @param Reverse Whether to invert the comparison result (for unordered
/// predicates)
/// @param I The instruction context for building new instructions
/// @return An instruction producing a boolean (i1) comparison result
Instruction *AIE2PSConvertFP16ScalarOperationImpl::compareFP16(
    Value *Op0, Value *Op1, Intrinsic::ID Id, bool Reverse, Instruction &I) {
  auto M = I.getFunction()->getParent();
  // Get the AIE2PS comparison intrinsic
  auto CmpIntrinsic = Intrinsic::getOrInsertDeclaration(M, Id);
  SmallVector<Value *> Args;
  // Convert both scalar operands to 32-element vectors
  Args.push_back(getVector32FP(Op0, I));
  Args.push_back(getVector32FP(Op1, I));
  // Call the comparison intrinsic
  auto CmpCall = Builder.CreateCall(CmpIntrinsic, Args);
  // Truncate the result to i1 (boolean)
  auto res = Builder.CreateTrunc(CmpCall, Type::getInt1Ty(I.getContext()));
  // Invert result if needed (for unordered predicates)
  if (Reverse)
    res = Builder.CreateXor(res, ConstantInt::getTrue(I.getContext()));
  return cast<Instruction>(res);
}

/// Convert FP16 floating-point comparisons to AIE2PS intrinsics.
/// Maps LLVM fcmp predicates to appropriate AIE2PS comparison intrinsics.
/// Handles both ordered and unordered comparisons by using the Reverse flag.
/// AIE2PS provides vltfloat16 (less-than) and vgefloat16 (greater-or-equal),
/// so other predicates are implemented by swapping operands or inverting
/// results.
Instruction *AIE2PSConvertFP16ScalarOperationImpl::visitFCmp(FCmpInst &I) {
  auto Op0 = I.getOperand(0);
  auto Op1 = I.getOperand(1);
  // Only handle FP16 comparisons
  if (!Op0->getType()->isHalfTy())
    return &I;
  Builder.SetInsertPoint(&I);
  bool Reverse = false;
  switch (I.getPredicate()) {
  case CmpInst::Predicate::FCMP_ULE: // True if unordered, less than, or equal
    // Implemented as NOT(Op1 < Op0) to get unordered behavior
    Reverse = true;
    [[fallthrough]];
  case CmpInst::Predicate::FCMP_OGT: { // True if ordered and greater than
    // Op0 > Op1 is equivalent to Op1 < Op0
    auto Id = Intrinsic::aie2ps_vltfloat16;
    return compareFP16(Op1, Op0, Id, Reverse, I);
    break;
  }
  case CmpInst::Predicate::FCMP_ULT: // True if unordered or less than
    // Implemented as NOT(Op0 >= Op1) to get unordered behavior
    Reverse = true;
    [[fallthrough]];
  case CmpInst::Predicate::FCMP_OGE: { // True if ordered and greater than or
                                       // equal
    // Use >= intrinsic directly
    auto Id = Intrinsic::aie2ps_vgefloat16;
    return compareFP16(Op0, Op1, Id, Reverse, I);
    break;
  }
  case CmpInst::Predicate::FCMP_UGE: // True if unordered, greater than, or
                                     // equal
    // Implemented as NOT(Op0 < Op1) to get unordered behavior
    Reverse = true;
    [[fallthrough]];

  case CmpInst::Predicate::FCMP_OLT: { // True if ordered and less than
    // Use < intrinsic directly
    auto Id = Intrinsic::aie2ps_vltfloat16;
    return compareFP16(Op0, Op1, Id, Reverse, I);
    break;
  }
  case CmpInst::Predicate::FCMP_UGT: // True if unordered or greater than
    // Implemented as NOT(Op1 >= Op0) to get unordered behavior
    Reverse = true;
    [[fallthrough]];
  case CmpInst::Predicate::FCMP_OLE: { // True if ordered and less than or equal
    // Op0 <= Op1 is equivalent to Op1 >= Op0
    auto Id = Intrinsic::aie2ps_vgefloat16;
    return compareFP16(Op1, Op0, Id, Reverse, I);
    break;
  }
  case CmpInst::Predicate::FCMP_UNE: // True if unordered or unequal
    // Implemented as NOT(Op0 == Op1) to get unordered behavior
    Reverse = true;
    [[fallthrough]];
  case CmpInst::Predicate::FCMP_OEQ: { /// True if ordered and equal
    // Equality is implemented as (Op0 >= Op1) AND (Op1 >= Op0)
    auto Id = Intrinsic::aie2ps_vgefloat16;
    auto res1 = compareFP16(Op0, Op1, Id, false, I);
    auto res2 = compareFP16(Op1, Op0, Id, false, I);
    auto res = Builder.CreateAnd(res1, res2);
    // Invert for unordered-not-equal predicate
    if (Reverse)
      res = Builder.CreateXor(res, ConstantInt::getTrue(I.getContext()));
    return cast<Instruction>(res);
    break;
  }
  default: {
    // Unsupported predicates (e.g., FCMP_FALSE, FCMP_TRUE, FCMP_ORD, FCMP_UNO)
    assert(false && "not supported yet");
    return &I;
    break;
  }
  }
  return &I;
}

} // namespace

// Pass registration and initialization
char AIE2PSConvertFP16ScalarOperationPass::ID = 0;
INITIALIZE_PASS_BEGIN(AIE2PSConvertFP16ScalarOperationPass, DEBUG_TYPE,
                      "Convert fp16 operation into aie2ps intrinsic", false,
                      false)
INITIALIZE_PASS_END(AIE2PSConvertFP16ScalarOperationPass, DEBUG_TYPE,
                    "Convert fp16 operation into aie2ps intrinsic", false,
                    false)

/// Public interface to create the FP16 conversion pass.
/// This pass should be run before IRTranslator to ensure FP16 types
/// are properly converted to AIE2PS intrinsics.
FunctionPass *llvm::createAIE2PSConvertFP16ScalarOperationPass() {
  return new AIE2PSConvertFP16ScalarOperationPass();
}
