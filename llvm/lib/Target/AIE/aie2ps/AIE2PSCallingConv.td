//==-AIE2PSCallingConv.td -Calling Conventions AIE2ps *- -------tablegen -*-==//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the AIE2ps architecture.
//
//===----------------------------------------------------------------------===//

/*
 // number of hardware loop levels exposed to compiler
    zloop_levels                  : 1;
    zloop_count_reserved_register : LC;
    zloop_start_reserved_register : LS;
    zloop_end_reserved_register   : LE;

    zloop_relative;

    counting_loop_levels          : unlimited;
    counting_loop_prelude         : -1;


  // ABI
    callee_saved                      : r8, r9, r10, r11, r12, r13, r14, r15, p6, p7;
    argument_registers                : r0, r1, r2, r3, r4, r5, r6, r7;
    argument_registers void*          : p0, p1, p2, p3, p4, p5;

    argument_registers_return         : r0,r1;
    argument_registers_return void*   : p0,p1;
    argument_registers v8w32          : W;
    argument_registers v4w32          : W;
    argument_registers v16w32         : X;
    argument_registers v32w32         : y2, y3, y4, y5;
    argument_registers v4w64          : W;
    argument_registers v8w64          : BM;
    argument_registers v16w64         : CM;
    argument_registers v32w64         : DM;
    argument_registers w64            : L;
    argument_registers w128           : EG2;
    argument_registers v64mx6         : FEW;
    argument_registers v128mx6        : FEX;
    argument_registers v256mx6        : FEY;
    argument_registers FEY_t          : FEY;
    argument_registers FEW_t          : FEW;
    argument_registers EGW_t          : EW;
    argument_registers EGX_t          : EX;
    argument_registers EGY_t          : EY;

*/

class CCIfNotInReg<CCAction A> : CCIf<"!ArgFlags.isInReg()", A> {}

// Scalar regs
def AssignToPTR     : CCAssignToReg<[p0, p1, p2, p3, p4, p5]>;
def AssignToGPR     : CCAssignToReg<[r0, r1, r2, r3, r4, r5, r6, r7]>;
def AssignToLReg    : CCAssignToReg<[l0, l1, l2, l3, l4, l5, l6, l7,
                                     l8, l9, l10, l11, l12, l13, l15]>;

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
def AssignToVecBM : CCAssignToReg<[
  bmll0,bmll1,bmll2,bmll3,bmll4,bmll5,bmll6,bmll7,
  bmhl0,bmhl1,bmhl2,bmhl3,bmhl4,bmhl5,bmhl6,bmhl7,
  bmlh0,bmlh1,bmlh2,bmlh3,bmlh4,bmlh5,bmlh6,bmlh7,
  bmhh0,bmhh1,bmhh2,bmhh3,bmhh4,bmhh5,bmhh6,bmhh7]>;
def AssignToVecCM : CCAssignToReg<[
  cml0,cml1,cml2,cml3,cml4,cml5,cml6,cml7,
  cmh0,cmh1,cmh2,cmh3,cmh4,cmh5,cmh6,cmh7]>;
def AssignToVecDM : CCAssignToReg<[
  dm0,dm1,dm2,dm3,dm4,dm5,dm6,dm7]>;

// Mask registers
def AssignToEG : CCAssignToReg<[eg0, eg2, eg4, eg6, eg8, eg10, eg1, eg2, eg3, eg4, eg5, eg6, eg7, eg8, eg9, eg11]>;

// Let's define the list of vector types in a single place.
// TODO: bfloat and complex types are not yet supported.
class VectorTypes {
  list<ValueType> V32Bits   = [v4i8, v2i16, v2bf16, v2f16];
  list<ValueType> V64Bits   = [v8i8, v4i16, v4bf16, v4f16, v2i32, v2f32];
  list<ValueType> V128Bits  = [v16i8, v8i16, v4i32, v8bf16, v8f16, v4f32];
  list<ValueType> V256Bits  = [v32i8, v16i16, v8i32, v16bf16, v16f16, v8f32, v4i64];
  list<ValueType> V512Bits  = [v64i8, v32i16, v32bf16, v32f16, v16i32, v8i64, v16f32];
  list<ValueType> V1024Bits = [v128i8, v64i16, v64bf16, v64f16, v32i32, v16i64, v32f32];
  list<ValueType> Acc2048Bits = [v256i8, v128i16, v64i32, v32i64, v64f32];
}
def AIE2PSVTypes : VectorTypes<>;

def RetCC_AIE2PS : CallingConv<[

  // MX type handling
  // TODO: Might have some conflict with sparse type, handle it when sparse type is supported.
  CCIf<"ArgFlags.isInConsecutiveRegs()", CCCustom<"CC_AIE2PS_Handle_Consecutive_Regs">>,

  // pointers are passed in registers
  CCIfType<[ptr0], AssignToPTR>,

  // Promote i1/i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16, bf16, f16], CCPromoteToType<i32>>,

  // Remaining scalars are returned in GPRs
  CCIfType<[i32, f32, v2bf16, v2f16, i64], AssignToGPR>,

  // 128-bits masks go in EG regs
  CCIfType<[i128], AssignToEG>,

    // 32-bits vectors go in GPRs
  CCIfType<AIE2PSVTypes.V32Bits, AssignToGPR>,
  // 64-bits vectors go in L regs
  CCIfType<AIE2PSVTypes.V64Bits, AssignToLReg>,
  // 128-bits vectors go in W regs
  CCIfType<AIE2PSVTypes.V128Bits, AssignToVecW>,
  // 256-bits vectors go in W regs
  CCIfType<AIE2PSVTypes.V256Bits, AssignToVecW>,
  // 512-bits vectors go in X regs
  CCIfNotInReg<CCIfType<AIE2PSVTypes.V512Bits, AssignToVecX>>,
  // 1024-bits vectors go in Y regs
  CCIfNotInReg<CCIfType<AIE2PSVTypes.V1024Bits, AssignToVecY>>,

  // 512-bits accumulators go in BM regs
  CCIfInReg<CCIfType<AIE2PSVTypes.V512Bits, AssignToVecBM>>,
  // 1024-bits accumulators go in CM regs
  CCIfInReg<CCIfType<AIE2PSVTypes.V1024Bits, AssignToVecCM>>,
  // 2048-bits accumulators go in DM regs
  CCIfInReg<CCIfType<AIE2PSVTypes.Acc2048Bits, AssignToVecDM>>,
  CCIfType<[v64i32, v128f16], AssignToVecDM>,
]>;

//===----------------------------------------------------------------------===//
// AIE2ps Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_AIE2PS_Stack : CallingConv<[
  // Promote i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,

  // Ensure split arguments are stored in natural order on the stack.
  CCIf<"ArgFlags.isSplit() || !State.getPendingLocs().empty()",
       CCCustom<"CC_AIEX_Handle_Split_Arg_Stack">>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned. Same for pointers
  CCIfType<[i32, f32, ptr0, bf16, f16, v2bf16, v2f16], CCAssignToStack<4, 4>>,

    // 32-bits vectors are 4-bytes aligned
  CCIfType<AIE2PSVTypes.V32Bits, CCAssignToStack<4, 4>>,
  // 64-bits vectors are 4-bytes aligned
  CCIfType<AIE2PSVTypes.V64Bits, CCAssignToStack<8, 4>>,
  // 128-bits vectors are 16-bytes aligned
  CCIfType<AIE2PSVTypes.V128Bits, CCAssignToStack<16, 16>>,
  CCIfType<[i128], CCAssignToStack<16, 16>>,
  // 256, 512 and 1024 bits vectors are 32-bytes aligned
  CCIfType<AIE2PSVTypes.V256Bits, CCAssignToStack<32, 32>>,
  CCIfType<AIE2PSVTypes.V512Bits, CCAssignToStack<64, 32>>,
  CCIfType<AIE2PSVTypes.V1024Bits, CCAssignToStack<128, 32>>,

  // 512, 1024, 2048 bits accumulator are 32-bytes aligned
  CCIfType<AIE2PSVTypes.V512Bits, CCAssignToStack<64, 32>>,
  CCIfType<AIE2PSVTypes.V1024Bits, CCAssignToStack<128, 32>>,
  CCIfType<AIE2PSVTypes.Acc2048Bits, CCAssignToStack<256, 32>>,
  CCIfType<[v64i32, v128f16], CCAssignToStack<256, 32>>

]>;

def CC_AIE2PS : CallingConv<[
  // isInConsecutiveRegs flag is unset by default for all arguments. We set it
  // for sparse arguments and for bfp type in AIECallLowering.cpp and detect it
  // here to dynamically switch the calling convention.
  CCIf<"ArgFlags.isInConsecutiveRegs()", CCCustom<"CC_AIE2PS_Handle_Consecutive_Regs">>,

  // pointers are passed in registers
  CCIfType<[ptr0], AssignToPTR>,

  // Promote i8/i16 arguments to i32.
  CCIfType<[i1, i8, i16, bf16, f16], CCPromoteToType<i32>>,

  // 128-bits masks go in EG regs
  CCIfType<[i128], AssignToEG>,

  // Do not split arguments half on the stack, and half in registers.
  // And ensure split arguments are stored in natural order on the stack.
  CCIf<"ArgFlags.isSplit() || !State.getPendingLocs().empty()",
       CCCustom<"CC_AIE2PS_Handle_Split_Arg">>,

  // Integer arguments are passed in integer registers if available.
  CCIfType<[i32, f32, v2bf16, v2f16], AssignToGPR>,

  // 32-bits vectors go in GPRs
  CCIfType<AIE2PSVTypes.V32Bits, AssignToGPR>,
  // 64-bits vectors go in L regs
  CCIfType<AIE2PSVTypes.V64Bits, AssignToLReg>,
  // 128-bits vectors go in W regs
  CCIfType<AIE2PSVTypes.V128Bits, AssignToVecW>,
  // 256-bits vectors go in W regs
  CCIfType<AIE2PSVTypes.V256Bits, AssignToVecW>, 
  // 512-bits vectors go X regs
  CCIfNotInReg<CCIfType<AIE2PSVTypes.V512Bits, AssignToVecX>>,
  // 1024-bits vectors go in Y regs
  CCIfNotInReg<CCIfType<AIE2PSVTypes.V1024Bits, AssignToVecY>>,
 
  // 512-bits accumulators go in BM regs
  CCIfInReg<CCIfType<AIE2PSVTypes.V512Bits, AssignToVecBM>>,
  // 1024-bits accumulators go in CM regs
  CCIfInReg<CCIfType<AIE2PSVTypes.V1024Bits, AssignToVecCM>>,
  // 2048-bits accumulators go in DM regs
  CCIfInReg<CCIfType<AIE2PSVTypes.Acc2048Bits, AssignToVecDM>>,
  CCIfType<[v64i32, v128f16], AssignToVecDM>,
  // Couldn't pass in registers, must be on the stack
  CCDelegateTo<CC_AIE2PS_Stack>
]>;

def CSR_AIE2PS
    : CalleeSavedRegs<(add lr, r8, r9, r10, r11, r12, r13, r14, r15, p6, p7)>;

def CSR_AIE2PS_Vec
    : CalleeSavedRegs<(add lr, r8, r9, r10, r11, r12, r13, r14, r15, p6, p7,
          wl0, wl2, wl4, wl6, wl8, wl10, wl1, wl3, wl5, wl7, wl9, wl11,
          wh0, wh2, wh4, wh6, wh8, wh10, wh1, wh3, wh5, wh7, wh9, wh11,
          bmll0, bmll1, bmll2, bmll3, bmll4, bmll5, bmll6, bmll7,
          bmhl0, bmhl1, bmhl2, bmhl3, bmhl4, bmhl5, bmhl6, bmhl7,
          bmlh0, bmlh1, bmlh2, bmlh3, bmlh4, bmlh5, bmlh6, bmlh7,
          bmhh0, bmhh1, bmhh2, bmhh3, bmhh4, bmhh5, bmhh6, bmhh7)>;

// Needed for implementation of AIERegisterInfo::getNoPreservedMask()
def CSR_NoRegs : CalleeSavedRegs<(add)>;
