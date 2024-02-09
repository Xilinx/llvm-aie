//===-- AIE2CallingConv.td -Calling Conventions AIEngine V2 *- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the AIEngine V2 architecture.
//
//===----------------------------------------------------------------------===//
/*
 // ABI
    callee_saved                    : r16, r17, r18, r19, r20, r21, r22, r23, p6, p7;
    argument_registers              : r0, r1, r2, r3, r4, r5, r6, r7;
    argument_registers void*        : p0, p1, p2, p3, p4, p5;

    argument_registers_return       : r0,r1;
    argument_registers_return void* : p0,p1;
    argument_registers v8w32        : W, AM;
    argument_registers v4w32        : W;
    argument_registers v16w32       : X;
    argument_registers v32w32       : y2, y3, y4, y5;
    argument_registers v4w64        : AM, W;
    argument_registers v8w64        : BM;
    argument_registers v16w64       : CM;
    argument_registers w64          : L;
    argument_registers w128         : QQs;
    argument_registers w640         : QQXs;
*/

// Scalar regs
def AssignToPTR     : CCAssignToReg<[p0, p1, p2, p3, p4, p5]>;
def AssignToGPR     : CCAssignToReg<[r0, r1, r2, r3, r4, r5, r6, r7]>;
def AssignToLReg    : CCAssignToReg<[l0, l1, l2, l3, l4, l5, l6, l7]>;

// "General-purpose" vregs
def AssignToVecW : CCAssignToReg<[
  wl0,wl2,wl4,wl6,wl8,wl10,
  wl1,wl3,wl5,wl7,wl9,wl11,
  wh0,wh2,wh4,wh6,wh8,wh10,
  wh1,wh3,wh5,wh7,wh9,wh11]>;
def AssignToVecX : CCAssignToReg<[
  x0,x2,x4,x6,x8,x10,
  x1,x3,x5,x7,x9,x11]>;
def AssignToVecY : CCAssignToReg<[y2,y3,y4,y5]>;

// Accumulator vregs
def AssignToVecAM : CCAssignToReg<[
  amll0,amll1,amll2,amll3,amll4,amll5,amll6,amll7,amll8,
  amhl0,amhl1,amhl2,amhl3,amhl4,amhl5,amhl6,amhl7,amhl8,
  amlh0,amlh1,amlh2,amlh3,amlh4,amlh5,amlh6,amlh7,amlh8,
  amhh0,amhh1,amhh2,amhh3,amhh4,amhh5,amhh6,amhh7,amhh8]>;
def AssignToVecBM : CCAssignToReg<[
  bml0,bml1,bml2,bml3,bml4,bml5,bml6,bml7,bml8,
  bmh0,bmh1,bmh2,bmh3,bmh4,bmh5,bmh6,bmh7,bmh8]>;
def AssignToVecCM : CCAssignToReg<[
  cm0,cm1,cm2,cm3,cm4,cm5,cm6,cm7,cm8]>;

// Mask registers
def AssignToVecQ : CCAssignToReg<[q0, q2, q1, q3]>;

// Let's define the list of vector types in a single place.
// TODO: bfloat and complex types are not yet supported.
class VectorTypes {
  list<ValueType> V32Bits   = [v4i8, v2i16, v2bf16];
  list<ValueType> V64Bits   = [v8i8, v4i16, v4bf16, v2i32];
  list<ValueType> V128Bits  = [v16i8, v8i16, v4i32, v8bf16, v4f32];
  list<ValueType> V256Bits  = [v32i8, v16i16, v8i32, v16bf16, v8f32];
  list<ValueType> Acc256Bits  = [v4i64];
  list<ValueType> V512Bits  = [v64i8, v32i16, v16i32, v32bf16, v16f32];
  list<ValueType> Acc512Bits  = [v8i64];
  list<ValueType> V1024Bits = [v128i8, v64i16, v32i32, v64bf16, v32f32];
  list<ValueType> Acc1024Bits = [v16i64];
}
def AIE2VTypes : VectorTypes<>;


def RetCC_AIE2 : CallingConv<[

  // pointers are passed in registers
  CCIfType<[ptr0], AssignToPTR>,

  // Promote i1/i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16, bf16], CCPromoteToType<i32>>,

  // Remaining scalars are returned in GPRs
  CCIfType<[i32, f32, v2bf16], AssignToGPR>,

  // 32-bits vectors go in GPRs
  CCIfType<AIE2VTypes.V32Bits, AssignToGPR>,
  // 64-bits vectors go in L regs
  CCIfType<AIE2VTypes.V64Bits, AssignToLReg>,
  // 128-bits vectors go in W regs
  CCIfType<AIE2VTypes.V128Bits, AssignToVecW>,
  // 256-bits vectors go in W regs
  CCIfType<AIE2VTypes.V256Bits, AssignToVecW>,
  // 512-bits vectors go in X regs
  CCIfType<AIE2VTypes.V512Bits, AssignToVecX>,
  // 1024-bits vectors go in Y regs
  CCIfType<AIE2VTypes.V1024Bits, AssignToVecY>,

  // 256-bits accumulators go in AM regs
  CCIfType<AIE2VTypes.Acc256Bits, AssignToVecAM>,
  // 512-bits accumulators go in BM regs
  CCIfType<AIE2VTypes.Acc512Bits, AssignToVecBM>,
  // 1024-bits accumulators go in CM regs
  CCIfType<AIE2VTypes.Acc1024Bits, AssignToVecCM>,
  // 128-bits masks go in Q regs
  CCIfType<[i128], AssignToVecQ>,
]>;

//===----------------------------------------------------------------------===//
// AIEngine V2 Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_AIE2_Stack : CallingConv<[
  // Promote i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

  // Ensure split arguments are stored in natural order on the stack.
  CCIf<"ArgFlags.isSplit() || !State.getPendingLocs().empty()",
       CCCustom<"CC_AIEX_Handle_Split_Arg_Stack">>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned. Same for pointers
  CCIfType<[i32, f32, ptr0, bf16, v2bf16], CCAssignToStack<4, 4>>,

  // 32-bits vectors are 4-bytes aligned
  CCIfType<AIE2VTypes.V32Bits, CCAssignToStack<4, 4>>,
  // 64-bits vectors are 4-bytes aligned
  CCIfType<AIE2VTypes.V64Bits, CCAssignToStack<8, 4>>,
  // 128-bits vectors and masks are 16-bytes aligned
  CCIfType<AIE2VTypes.V128Bits, CCAssignToStack<16, 16>>,
  CCIfType<[i128], CCAssignToStack<16, 16>>,
  // 256, 512 and 1024 bits vectors are 32-bytes aligned
  CCIfType<AIE2VTypes.V256Bits, CCAssignToStack<32, 32>>,
  CCIfType<AIE2VTypes.V512Bits, CCAssignToStack<64, 32>>,
  CCIfType<AIE2VTypes.V1024Bits, CCAssignToStack<128, 32>>,

  // 256, 512 and 1024 bits accumulator are 32-bytes aligned
  CCIfType<AIE2VTypes.Acc256Bits, CCAssignToStack<32, 32>>,
  CCIfType<AIE2VTypes.Acc512Bits, CCAssignToStack<64, 32>>,
  CCIfType<AIE2VTypes.Acc1024Bits, CCAssignToStack<128, 32>>,
]>;

def CC_AIE2 : CallingConv<[
  // isInConsecutiveRegs flag is unset by default for all arguments. We set it
  // for sparse arguments in AIECallLowering.cpp and detect it here to
  // dynamically switch the calling convention.
  CCIf<"ArgFlags.isInConsecutiveRegs()",
       CCCustom<"CC_AIE2_SPARSE">>,

  // pointers are passed in registers
  CCIfType<[ptr0], AssignToPTR>,

  // Promote i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16, bf16], CCPromoteToType<i32>>,

  // Do not split arguments half on the stack, and half in registers.
  // And ensure split arguments are stored in natural order on the stack.
  CCIf<"ArgFlags.isSplit() || !State.getPendingLocs().empty()",
       CCCustom<"CC_AIE2_Handle_Split_Arg">>,

  // Integer arguments are passed in integer registers if available.
  CCIfType<[i32, f32, v2bf16], AssignToGPR>,

  // 32-bits vectors go in GPRs
  CCIfType<AIE2VTypes.V32Bits, AssignToGPR>,
  // 64-bits vectors go in L regs
  CCIfType<AIE2VTypes.V64Bits, AssignToLReg>,
  // 128-bits vectors go in W regs
  CCIfType<AIE2VTypes.V128Bits, AssignToVecW>,
  // 256-bits vectors go in W and AM regs
  CCIfType<AIE2VTypes.V256Bits, AssignToVecW>,
  CCIfType<AIE2VTypes.V256Bits, AssignToVecAM>,
  // 512-bits vectors go in X regs
  CCIfType<AIE2VTypes.V512Bits, AssignToVecX>,
  // 1024-bits vectors go in Y regs
  CCIfType<AIE2VTypes.V1024Bits, AssignToVecY>,

  // 256-bits accumulators go in AM and W regs
  CCIfType<AIE2VTypes.Acc256Bits, AssignToVecAM>,
  CCIfType<AIE2VTypes.Acc256Bits, AssignToVecW>,
  // 512-bits accumulators go in BM regs
  CCIfType<AIE2VTypes.Acc512Bits, AssignToVecBM>,
  // 1024-bits accumulators go in CM regs
  CCIfType<AIE2VTypes.Acc1024Bits, AssignToVecCM>,
  // 128-bits masks go in Q regs
  CCIfType<[i128], AssignToVecQ>,

  // Couldn't pass in registers, must be on the stack
  CCDelegateTo<CC_AIE2_Stack>
]>;

def CSR_AIE2
    : CalleeSavedRegs<(add lr, r16, r17, r18, r19, r20, r21, r22, r23, p6, p7)>;

// Needed for implementation of AIERegisterInfo::getNoPreservedMask()
def CSR_NoRegs : CalleeSavedRegs<(add)>;
