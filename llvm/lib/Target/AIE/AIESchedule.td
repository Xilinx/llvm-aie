//=- AIESchedule.td - AIEngine Scheduling Definitions --------*- tablegen -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// AIEngine is an exposed pipeline architecture.  As a result, this file
// must model the processor pipeline with reasonably high fidelity in order
// to enable the generation of correct code.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Functional units
//===----------------------------------------------------------------------===//
def AGUA    : FuncUnit; // Address generation unit. A Special ALU for pointer regs.
def AGUB    : FuncUnit; // Address generation unit. A Special ALU for pointer regs.
def FU_LDA  : FuncUnit; // Load path A.
def FU_LDB  : FuncUnit; // Load path B.
def FU_STORE: FuncUnit; // The store path.

// The lock interface has particular scheduling hazards.
def LOCK    : FuncUnit; // Lock accesses
// The multiplier, vector unit, and other scalar operations share a memory port.
// Because multiply and vector operations complete several cycles later we need
// to explicitly represent this scheduling hazard.
def SCALAR_REG_WRITE : FuncUnit; // writeback of scalar operations.

// There's a conflict on GPR read ports, e.g. between ST and MOV
def SCALAR_REG_READ : FuncUnit;

// These units can run independently (as part of the conversion instruction) or fused
// as part of a non-linear approximation instruction
def FU_FX2FLT : FuncUnit;
def FU_FLT2FX : FuncUnit;

// The vector unit has some operations that perform a delayed read of the register
// file.

def VECTOR_REG_READ : FuncUnit; //

// Modeling of VLIW issue slots.
def SLOT_ALU : FuncUnit;
def SLOT_LNG : FuncUnit;
def SLOT_MV0 : FuncUnit;
def SLOT_MV1 : FuncUnit;
def SLOT_LDA : FuncUnit;
def SLOT_LDB : FuncUnit;
def SLOT_ST  : FuncUnit;
def SLOT_VEC : FuncUnit;
def EMPTY_FU : FuncUnit;

//===----------------------------------------------------------------------===//
// Instruction Itinerary classes
//===----------------------------------------------------------------------===//
def II_ALU             : InstrItinClass;
def II_EVENT           : InstrItinClass;
def II_LOCK            : InstrItinClass;
def II_LDA             : InstrItinClass;
def II_LDB             : InstrItinClass;
def II_LDASP_R         : InstrItinClass;
def II_LDASP_RW        : InstrItinClass;
def II_LDBSP_R         : InstrItinClass;
def II_LDBSP_RW        : InstrItinClass;
def II_SCL2MS          : InstrItinClass;
def II_SS2SCL          : InstrItinClass;
def II_STS             : InstrItinClass;
def II_ST_SPIL         : InstrItinClass;
def II_SRS             : InstrItinClass;
def II_STSRS           : InstrItinClass;
def II_STS_HB          : InstrItinClass;
def II_MOV             : InstrItinClass;
def II_MOVConvert      : InstrItinClass;
def II_MOVI            : InstrItinClass;
def II_MUL             : InstrItinClass;
def II_NLF_FLT_FLT     : InstrItinClass;
def II_NLF_FIX_FLT     : InstrItinClass;
def II_NLF_FLT_FIX     : InstrItinClass;
def II_NLF_FIX_FIX     : InstrItinClass;
def II_PADDA           : InstrItinClass;
def II_PADDA_sp        : InstrItinClass;
def II_PADDB           : InstrItinClass;
def II_PADDB_sp        : InstrItinClass;
def II_PKTHD           : InstrItinClass;
def II_CPKTHD          : InstrItinClass;
def II_RET             : InstrItinClass;
def II_VSTS            : InstrItinClass;
def II_VST_SPIL        : InstrItinClass;
def II_VFMUL           : InstrItinClass;
def II_VFMAC           : InstrItinClass;
def II_VMUL            : InstrItinClass;
def II_VMAC            : InstrItinClass;
def II_VMOV            : InstrItinClass;
def II_VCMP            : InstrItinClass;
def II_VEXTRACT        : InstrItinClass;
def II_VSEL            : InstrItinClass;
def II_VUPDATE         : InstrItinClass;

// Insert n empty cycles
class EmptyCycles<int n> : InstrStage<0, [EMPTY_FU], n>;

// Locks uses the distinction between reserved and required in the scoreboard.
// The idea is that instructions that just reserve a resource don't mutually exclude
// each other, whereas they are causing a hazard with instruction that require
// the same resource. In our case, the lock instruction will push memory accessing
// instructions out of its vicinity, without the latter pushing eachother away.
// Reserve lock for n cycles, start next stage m cycles later
class ReserveLocks<int n, int m> : InstrStage<n, [LOCK], m, Reserved>;
// Require locks for n cycles, start the next stage m cycles later
class RequireLocks<int n, int m> : InstrStage<n, [LOCK], m>;

//===----------------------------------------------------------------------===//
// AIEngine instruction itineraries.
//===----------------------------------------------------------------------===//
// Note: the itineraries here define two different kinds of latencies.
// 1) The argument to 'InstrStage' determines how many cycles the pipeline stage
// takes to execute.  Other instructions cannot use the pipeline stage while the
// instruction is executing.  This is primarily used to represent sequential
// operations.  The 'LOCK' instructions are defined to take 5 cycles in order to
// avoid contention for locks.
// 2) The third argument to 'InstrItinData' specifies the operand latencies of the
// associated instruction(s).  This array of integers is closely tied to the
// register order of corresponding instruction(s).  Generally, cycle 1 represents
// the EX1 cycle in the AIE architecture description.  Results will be available on
// the following cycle. In most cases, operands are read in cycle 1 and written in
// cycle 1 or later. The order of correspondance is:
// Explicit Defs, Explicit Uses, Implicit Defs, Implicit Uses
// Note that although regular stores and loads effectively happen late in the pipeline,
// they always occur in order.  Hence the latency of the memory path doesn't appear
// in the itineraries.  The exception is dealing with 8 and 16 bit stores where
// a read-modify-write process occurs.  This is currently not accounted for.
def AIEItineraries : ProcessorItineraries<
  [EMPTY_FU, AGUA, AGUB, FU_LDA, FU_LDB, FU_STORE, LOCK, SCALAR_REG_WRITE, VECTOR_REG_READ, SCALAR_REG_READ,
   FU_FX2FLT, FU_FLT2FX], [], [
  InstrItinData<II_ALU          , [InstrStage<1,  [SCALAR_REG_WRITE]>], [1, 1, 1, 1]>,
  InstrItinData<II_EVENT        , [], [1]>,
  InstrItinData<II_LDA          , [InstrStage<1,  [FU_LDA], 0>,
                                   ReserveLocks<5, 7>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>], [8, 1, 0 /*cs read in ID stage*/]>,    // 8 cycle loads
  InstrItinData<II_LDB          , [InstrStage<1,  [FU_LDB], 0>,
                                   ReserveLocks<5, 7>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>], [8, 1]>,    // 8 cycle loads
  // Lda instructions with read only of the stack pointer register
  // FIXME: Is there a conflict on PTR write ports? In this case, we should
  // split further this itinerary to model the write operation on pointer
  // registers for the addressing mode pstm_nrm and pstm_nrm_imm.
  InstrItinData<II_LDASP_R      , [InstrStage<1,  [FU_LDA], 0>,
                                   InstrStage<1,  [AGUA], 0>,
                                   ReserveLocks<5, 7>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>],
                                  [/*DestRegLatency=*/8,
                                   /*SrcReg/ImmediatePseudoLatency=*/1,
                                   /*StackPointerReadLatency=*/1]>,    // Like LDA, but implicit stack pointer
  // Lda instructions with read-write of the stack pointer register. It occurs
  // when using the addressing modes pstm_sp_imm or pstm_sp. See Tables 2-9 and
  // and 2-10 of the ISA spec for the operand latencies.
  InstrItinData<II_LDASP_RW     , [InstrStage<1,  [FU_LDA], 0>,
                                   InstrStage<1,  [AGUA], 0>,
                                   ReserveLocks<5, 7>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>],
                                  [/*DestRegLatency=*/8,
                                   /*SrcReg/ImmediatePseudoLatency=*/1,
                                   /*StackPointerWriteLatency=*/2,
                                   /*StackPointerReadLatency=*/1]>,    // Like LDA, but implicit stack pointer
  // Ldb instructions with read only of the stack pointer register
  InstrItinData<II_LDBSP_R      , [InstrStage<1,  [FU_LDB], 0>,
                                   InstrStage<1,  [AGUB], 0>,
                                   ReserveLocks<5, 7>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>],
                                  [/*DestRegLatency=*/8,
                                   /*SrcReg/ImmediatePseudoLatency=*/1,
                                   /*StackPointerReadLatency=*/1]>,
  // Ldb instructions with read-write of the stack pointer register. Same as
  // II_LDASP_RW but with AGUB.
  InstrItinData<II_LDBSP_RW     , [InstrStage<1,  [FU_LDB], 0>,
                                   InstrStage<1,  [AGUB], 0>,
                                   ReserveLocks<5, 7>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>],
                                  [/*DestRegLatency=*/8,
                                   /*SrcReg/ImmediatePseudoLatency=*/1,
                                   /*StackPointerWriteLatency=*/2,
                                   /*StackPointerReadLatency=*/1]>,
  InstrItinData<II_LOCK         , [RequireLocks<5, 0>], [1, 1]>,           // Limit access to lock unit.
  InstrItinData<II_MOV          , [InstrStage<1, [SCALAR_REG_READ], 0>], [1, 1]>,
  InstrItinData<II_MOVConvert   , [InstrStage<1, [SCALAR_REG_READ, FU_FX2FLT, FU_FLT2FX], 0>], [2, 1, 1]>,
  InstrItinData<II_MOVI         , [], [1, 1]>,
  InstrItinData<II_NLF_FLT_FLT  , [InstrStage<1, [SCALAR_REG_READ]>], [4, 1]>,
  InstrItinData<II_NLF_FLT_FIX  , [InstrStage<1, [SCALAR_REG_READ], 3>,
                                   InstrStage<1, [FU_FLT2FX]>], [4, 1, 1]>,
  InstrItinData<II_NLF_FIX_FLT  , [InstrStage<1, [SCALAR_REG_READ, FU_FX2FLT]>], [4, 4, 1]>,
  InstrItinData<II_NLF_FIX_FIX  , [InstrStage<1, [SCALAR_REG_READ, FU_FX2FLT], 3>,
                                   InstrStage<1, [FU_FLT2FX]>], [4, 4, 1, 1]>,
  InstrItinData<II_VMOV         , [InstrStage<1,  [VECTOR_REG_READ]>], [1, 1]>,
  InstrItinData<II_VFMUL        , [EmptyCycles<5>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>],
                                   [6, 6, 1, 0, 0, 1, 0, 0, 0, 0]>, // Multiply with 6 cycle latency
  InstrItinData<II_VFMAC        , [EmptyCycles<4>,
                                   InstrStage<1,  [VECTOR_REG_READ], 1>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>],
                                   [6, 6, 4, 1, 0, 0, 1, 0, 0, 0, 0]>, // MAC with 6 cycle latency, accumulator read in cycle 4
  InstrItinData<II_VMUL         , [EmptyCycles<5>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>],
                                  [6, 6, 1, 0, 0, 1, 0, 0, 0, 0]>, // Multiply with 6 cycle latency
  InstrItinData<II_VMAC         , [EmptyCycles<4>,
                                   InstrStage<1,  [VECTOR_REG_READ], 1>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>],
                                  [6, 6, 1, 0, 0, 1, 0, 0, 0, 0]>, // Multiply with 6 cycle latency
  InstrItinData<II_VCMP         , [EmptyCycles<1>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>], [2, 2, 1, 0, 0, 0, 0, 0]>, // Vector compare through pre-adder
  InstrItinData<II_VSEL         , [], [2, 1, 0, 0, 0, 0, 0, 2 /*select*/]>, // Vector select through pre-adder
  InstrItinData<II_VUPDATE      , [InstrStage<1,  [VECTOR_REG_READ]>], [1, 1, 1, 1]>, // Vector update/shift instructions
  InstrItinData<II_VEXTRACT     , [], [1, 1, 1]>, // Vector extract instructions
  InstrItinData<II_MUL          , [EmptyCycles<2>,
                                   InstrStage<1,  [SCALAR_REG_WRITE]>], [3, 1, 1]>, // Multiply with 3 cycle latency
  InstrItinData<II_PADDA_sp     , [InstrStage<1,  [AGUA]>], [1 /*Read mod*/, 2 /*Write SP*/, 1 /*Read SP*/]>, // 2 cycles to update a pointer
  InstrItinData<II_PADDB_sp     , [InstrStage<1,  [AGUB]>], [1 /*Read mod*/, 2 /*Write SP*/, 1 /*Read SP*/]>, // 2 cycles to update a pointer
  InstrItinData<II_PADDA        , [InstrStage<1,  [AGUA]>], [2 /*Write PTR*/, 1 /*Read PTR*/, 1  /*Read mod*/]>, // 2 cycles to update a pointer
  InstrItinData<II_PADDB        , [InstrStage<1,  [AGUB]>], [2 /*Write PTR*/, 1 /*Read PTR*/, 1  /*Read mod*/]>, // 2 cycles to update a pointer
  InstrItinData<II_RET          , [], [/*LR*/ 1, /*r0*/ 5]>,
  InstrItinData<II_STS          , [ReserveLocks<5, 0>,
                                   InstrStage<1, [SCALAR_REG_READ], 0>,
                                   InstrStage<1, [FU_STORE], 0>
                                   ], [1, 1, 0 /*cs read in ID stage*/]>,
  InstrItinData<II_ST_SPIL      , [ReserveLocks<5, 0>,
                                   InstrStage<1, [SCALAR_REG_READ], 0>,
                                   InstrStage<1, [FU_STORE], 0>
                                   ], [1, 1 /*SP*/, 1 /*SP*/]>,
  InstrItinData<II_VSTS         , [ReserveLocks<5, 0>,
                                   InstrStage<1, [FU_STORE], 0>
                                   ], [1, 1, 0 /*cs read in ID stage*/]>,
  InstrItinData<II_VST_SPIL      , [ReserveLocks<5, 0>,
                                   InstrStage<1, [FU_STORE], 0>
                                   ], [1, 1 /*SP*/, 1 /*SP*/]>,
  InstrItinData<II_SRS          , [], [3, 3, 1, 1, 2]>,
  InstrItinData<II_STSRS        , [ReserveLocks<5, 0>,
                                   InstrStage<1, [FU_STORE], 0>
                                   ], [3, 1, 1, 1, 2]>,
  InstrItinData<II_STS_HB       , [InstrStage<7,  [FU_LDA], 0, Reserved>,
                                   InstrStage<7, [FU_STORE], 0, Reserved>,
                                   ReserveLocks<11, 7>,
                                   InstrStage<1, [SCALAR_REG_READ], 0>
                                   ], [7, 1, 0 /*cs read in ID stage*/]>,
  InstrItinData<II_SCL2MS       , [], [2, 1, 2, 2]>,
  InstrItinData<II_SS2SCL       , [], [1, 1, 0]>,
  InstrItinData<II_PKTHD        , [], [2, 1, 2, 2]>,
  InstrItinData<II_CPKTHD       , [], [2, 1, 1, 2, 2]>,
]>;

def AIESchedModel : SchedMachineModel {
  // NOTE: the maximum issue-limit for AIE1 seems to be 9, reachable with the
  // following composite instruction:
  // I128_LDA_LDB_ST_MV0_MV1SSB_ALU_VEC_SHFT_VEC_STRM_VEC_SHORT
  int IssueWidth = 9;
  int MicroOpBufferSize = 0; // We are in-order

  let Itineraries = AIEItineraries;
  int LoadLatency = 5; // FIXME
  int MispredictPenalty = 4; // FIXME

  int HighLatency = 37; // FIXME
  //  list<Predicate> UnsupportedFeatures = [];

  let CompleteModel = 0;
  // We must use the AIEHazardRecognizer during Post-RA scheduling to
  // insert pipeline no-ops
  let PostRAScheduler = 1;
}



// let SchedModel = AIESchedModel in {

// // ALU Pipeline
// // ============

// def ALUResource : ProcResource<1> { let BufferSize = 1; }
// def MULResource : ProcResource<1> { let BufferSize = 1; }

// // Sched definitions for integer pipeline instructions
// //
// // Basic ALU operation.
// def WriteALU : SchedWrite;
// def ReadALU : SchedRead;

// // Basic MUL operation.
// def WriteMUL : SchedWrite;
// def ReadMUL : SchedRead;

// def : WriteRes<WriteALU, [ALUResource]> { let Latency = 1; }
// def : WriteRes<WriteMUL, [MULResource]> { let Latency = 3; }

// // and, lui, nor, or, slti, sltiu, sub, subu, xor
// // add, addi, addiu, addu, andi, ori, rotr, se[bh], sllv?, sr[al]v?, slt, sltu,
// // xori
// //def : ItinRW<[GenericWriteALU], [II_ALU]>;
// //def : ItinRW<[GenericWriteALU], [II_MUL]>;

// }
