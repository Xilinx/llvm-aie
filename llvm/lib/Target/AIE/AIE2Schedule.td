//=- AIE2Schedule.td - AIEngine Scheduling Definitions --------*- tablegen -*-=//
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


// We use the itineraries for latencies and for resource conflicts that are not
// implicit in the VLIW slots.
// The latencies are twofold:
// - The operand latencies for explicitly coded operands and 'static' implicit uses
//   and defs. They determine the latencies of true, anti and output dependences,
//   taking the specified bypasses into account.
// - The instruction latencies, which say when an instruction has committed all its
//   effects. These are mainly relevant for store instructions (load instructions
//   generally finish at the same time as their result operand latency). The
//   instruction latency is the maximum of the operand latencies and the extent
//   of the instruction stages. Hence, store instructions need to fill out their
//   stages until the stage where the memory transfer takes place.
// Semaphore operations have hasSideEffects=true, which introduces ordering edges.
// These edges are detected and have their latencies adjusted using the instruction
// latencies. This is done through a DAG mutator in the BaseSubtarget module.

// Most FuncUnits below correspond to an entry in one of the tables
// in the "Hazards and Conflicts" chapter of the ISA

// 11.1 Structural Conflicts

// Upper half of the SRS
// TESTME-VLIW: conflict between S and M slot SRS
def UPPER_SRS : FuncUnit;

// UPS
def UPS_UNIT : FuncUnit;

// Processor bus
def PROC_BUS : FuncUnit;

// Memory Store interface
def STORE_UNIT : FuncUnit;

// Memory load interface A
// TESTME-VLIW: conflict on VLDB.4x. Note that this is the only non-lda instruction
// that touches this resource, and all instruction that touch it do so in E2
def LOAD_UNIT_A : FuncUnit;

// The Semaphore interface
def SEMAPHORE : FuncUnit;
// This is telling other ops to stay clear of a semaphore op.
class AvoidSemaphore<int n> : InstrStage<n, [SEMAPHORE], 0, Reserved>;

// DONE interface
// Doesn't look likely to cause a conflict, just for completeness
def DONE_UNIT : FuncUnit;

// Execution Tracing
def EXEC_TRACE_UNIT : FuncUnit;

// 11.2 General Purpose Register Ports

// Reading (rv port)
def R_RV_PORT : FuncUnit;
// Writing (wm port), shared with S RegClass
def RS_WM_PORT : FuncUnit;
// Writing (wx port)
def R_WX_PORT : FuncUnit;
// Writing (wa port)
def R_WA_PORT : FuncUnit;


// 11.3 Addressing Register Ports

// Reading D or P
def P_RM_PORT : FuncUnit;

def P_WM_PORT : FuncUnit;
def M_WM_PORT : FuncUnit;
def DJ_WM_PORT : FuncUnit;
def DN_WM_PORT : FuncUnit;
def DC_WM_PORT : FuncUnit;


// 11.4 Other Scalar Register Ports
// The S register is a separate register file, however, there is a common part between the 
// R and S register file wm port which prevents them from being used in parallel.
// RS_WM_PORT ports model the necessary behavior

// 11.5 Vector Register Ports

// Reading (rs port)
def W_RS_PORT : FuncUnit;
// Writing (wa port)
def W_WA_PORT : FuncUnit;
// Writing (wm port)
// TESTME-VLIW: needs tests once VSRSM is added
def W_WM_PORT : FuncUnit;


// 11.6 Accumulator Register Ports

// Reading (rm port)
// TESTME-VLIW: needs testing once two-input accumulator instructions, e.g. VADD/VSUB
// are added
def CM_RM_PORT : FuncUnit;
// TESTME-VLIW: needs tests once floating point vector instructions are added
// Writing (wm port)
def CM_WM_PORT : FuncUnit;
// Writing (wa port)
def CM_WA_PORT : FuncUnit;

// Dummy resource to code empty cycles. Preferably used
// through the EmptyCycles class below.
def EMPTY_FU : FuncUnit;
// Insert n empty cycles. Note that we use EMPTY_FU for zero cycles
class EmptyCycles<int n> : InstrStage<0, [EMPTY_FU], n>;

// Convenience class coding one cycle with one resource
class SimpleCycle<FuncUnit FU> : InstrStage<1, [FU]>;

// Used for non-tail resources of a multi-resource cycle
class PrefixCycle<FuncUnit FU> : InstrStage<1, [FU], 0>;

// Partword stores have complicated dependences due to their read-modify-write nature
// We use this resource to keep other memory ops at a distance.
def PART_WORD_STORE : FuncUnit;
// This resource use is telling there's a partword store in progress.
// ISA says load is in E5, store is in E11, so we need to be 7 cycles apart.
def IsPartWordStore : InstrStage<7, [PART_WORD_STORE], 0>;
// This is telling other memory ops to stay clear of a part word store.
// A reserved resource is weak, so it blocks only if a part word store is holding it.
def AvoidPartWordStore : InstrStage<1, [PART_WORD_STORE], 0, Reserved>;

// Bypasses
def MOV_Bypass : Bypass;
def VEC_Bypass : Bypass;

def II_ABS : InstrItinClass;
def II_ACQ : InstrItinClass;
def II_ACQ_COND : InstrItinClass;
def II_ADC : InstrItinClass;
def II_ADD : InstrItinClass;
def II_ADD_NC : InstrItinClass;
def II_ADD_NC_DC : InstrItinClass;
def II_ADD_NC_DJ : InstrItinClass;
def II_ADD_NC_DN : InstrItinClass;
def II_ADD_NC_M : InstrItinClass;
def II_ADD_NC_P : InstrItinClass;
def II_ADD_NC_RS : InstrItinClass;
def II_AND : InstrItinClass;
def II_ASHL : InstrItinClass;
def II_CLB : InstrItinClass;
def II_CLZ : InstrItinClass;
def II_DIVS : InstrItinClass;
def II_DONE : InstrItinClass;
def II_EQ : InstrItinClass;
def II_EQZ : InstrItinClass;
def II_EVENT : InstrItinClass;
def II_EXTENDs8 : InstrItinClass;
def II_EXTENDs16 : InstrItinClass;
def II_EXTENDu8 : InstrItinClass;
def II_EXTENDu16 : InstrItinClass;
def II_GE : InstrItinClass;
def II_GEU : InstrItinClass;
def II_J : InstrItinClass;
def II_J_IND : InstrItinClass;
def II_JL : InstrItinClass;
def II_JL_IND : InstrItinClass;
def II_JNZ : InstrItinClass;
def II_JNZD : InstrItinClass;
def II_JZ : InstrItinClass;
def II_LDA : InstrItinClass;
def II_LDA_DC : InstrItinClass;
def II_LDA_DJ : InstrItinClass;
def II_LDA_DN : InstrItinClass;
def II_LDA_M : InstrItinClass;
def II_LDA_P : InstrItinClass;
def II_LDA_R : InstrItinClass;
def II_LDA_POST_1D : InstrItinClass;
def II_LDA_POST_1D_DC : InstrItinClass;
def II_LDA_POST_1D_DJ : InstrItinClass;
def II_LDA_POST_1D_DN : InstrItinClass;
def II_LDA_POST_1D_M : InstrItinClass;
def II_LDA_POST_1D_P : InstrItinClass;
def II_LDA_POST_1D_R : InstrItinClass;
def II_LDA_POST_2D : InstrItinClass;
def II_LDA_POST_2D_DC : InstrItinClass;
def II_LDA_POST_2D_DJ : InstrItinClass;
def II_LDA_POST_2D_DN : InstrItinClass;
def II_LDA_POST_2D_M : InstrItinClass;
def II_LDA_POST_2D_P : InstrItinClass;
def II_LDA_POST_2D_R : InstrItinClass;
def II_LDA_POST_3D : InstrItinClass;
def II_LDA_POST_3D_DC : InstrItinClass;
def II_LDA_POST_3D_DJ : InstrItinClass;
def II_LDA_POST_3D_DN : InstrItinClass;
def II_LDA_POST_3D_M : InstrItinClass;
def II_LDA_POST_3D_P : InstrItinClass;
def II_LDA_POST_3D_R : InstrItinClass;
def II_LDA_TM : InstrItinClass;
def II_LSHL : InstrItinClass;
def II_LT : InstrItinClass;
def II_LTU : InstrItinClass;
def II_MOV : InstrItinClass;
def II_MOV_DC : InstrItinClass;
def II_MOV_DJ : InstrItinClass;
def II_MOV_DN : InstrItinClass;
def II_MOV_M : InstrItinClass;
def II_MOV_P : InstrItinClass;
def II_MOV_RS : InstrItinClass;
def II_MOVA : InstrItinClass;
def II_MOVA_DC : InstrItinClass;
def II_MOVA_DJ : InstrItinClass;
def II_MOVA_DN : InstrItinClass;
def II_MOVA_M : InstrItinClass;
def II_MOVA_P : InstrItinClass;
def II_MOVA_R : InstrItinClass;
def II_MOVX : InstrItinClass;
def II_MOV_SCL : InstrItinClass;
def II_MOV_SCL_DC : InstrItinClass;
def II_MOV_SCL_DJ : InstrItinClass;
def II_MOV_SCL_DN : InstrItinClass;
def II_MOV_SCL_M : InstrItinClass;
def II_MOV_SCL_P : InstrItinClass;
def II_MOV_SCL_RS : InstrItinClass;
def II_MOV_SS : InstrItinClass;
def II_MUL : InstrItinClass;
def II_NE : InstrItinClass;
def II_NEZ : InstrItinClass;
def II_NOP : InstrItinClass;
def II_OR : InstrItinClass;
def II_PADD : InstrItinClass;
def II_PADD_2D : InstrItinClass;
def II_PADD_3D : InstrItinClass;
def II_REL : InstrItinClass;
def II_REL_COND : InstrItinClass;
def II_RET : InstrItinClass;
def II_SBC : InstrItinClass;
def II_SELEQZ : InstrItinClass;
def II_SELNEZ : InstrItinClass;
def II_ST : InstrItinClass;
def II_ST_MS : InstrItinClass;
def II_ST_MS_NB : InstrItinClass;
def II_ST_MS_NB_TLAST_REG : InstrItinClass;
def II_MOV_PH_B : InstrItinClass;
def II_MOV_PH_NB : InstrItinClass;
def II_MOV_PH_NB_TLAST_REG : InstrItinClass;
def II_MOV_CPH_B : InstrItinClass;
def II_MOV_CPH_NB : InstrItinClass;
def II_MOV_CPH_NB_TLAST_REG : InstrItinClass;
def II_STHB : InstrItinClass;
def II_ST_POST_1D : InstrItinClass;
def II_ST_POST_2D : InstrItinClass;
def II_ST_POST_3D : InstrItinClass;
def II_ST_TM : InstrItinClass;
def II_ST_Q : InstrItinClass;
def II_ST_POSTINC_Q : InstrItinClass;
def II_ST_2D_Q: InstrItinClass;
def II_ST_3D_Q : InstrItinClass;
def II_STHB_POST_1D : InstrItinClass;
def II_STHB_2D : InstrItinClass;
def II_STHB_3D : InstrItinClass;
def II_SUB : InstrItinClass;
def II_VABS_GTZ : InstrItinClass;
def II_VACC : InstrItinClass;
def II_VACCf : InstrItinClass;
def II_VADD : InstrItinClass;
def II_VADDMAC : InstrItinClass;
def II_VADDMACf : InstrItinClass;
def II_VBCST : InstrItinClass;
def II_VBCSTSHFL : InstrItinClass;
def II_VBLOG : InstrItinClass;
def II_VBNEG_LTZ : InstrItinClass;
def II_VCLR : InstrItinClass;
def II_VCLRf : InstrItinClass;
def II_VCONV : InstrItinClass;
def II_VCONVfp32bf16 : InstrItinClass;
def II_VCONVfp32bf16_WL : InstrItinClass;
def II_VEQZ : InstrItinClass;
def II_VEXTBCST : InstrItinClass;
def II_VEXTRACT : InstrItinClass;
def II_VEXTRACT_DC : InstrItinClass;
def II_VEXTRACT_DJ : InstrItinClass;
def II_VEXTRACT_DN : InstrItinClass;
def II_VEXTRACT_M : InstrItinClass;
def II_VEXTRACT_P : InstrItinClass;
def II_VEXTRACT_RS : InstrItinClass;
def II_VFLOORs32bf16_AM : InstrItinClass;
def II_VFLOORs32bf16_W : InstrItinClass;
def II_VINSERT : InstrItinClass;
def II_VLDA_W : InstrItinClass;
def II_VLDA_POSTINC_W : InstrItinClass;
def II_VLDA_2D_W : InstrItinClass;
def II_VLDA_3D_W : InstrItinClass;
def II_VLDA_AM : InstrItinClass;
def II_VLDA_POSTINC_AM : InstrItinClass;
def II_VLDA_2D_AM : InstrItinClass;
def II_VLDA_3D_AM : InstrItinClass;
def II_VLDA_CONV : InstrItinClass;
def II_VLDA_POSTINC_CONV : InstrItinClass;
def II_VLDA_2D_CONV : InstrItinClass;
def II_VLDA_3D_CONV : InstrItinClass;
def II_VLDB : InstrItinClass;
def II_VLDB_POSTINC : InstrItinClass;
def II_VLDB_2D : InstrItinClass;
def II_VLDB_3D : InstrItinClass;
def II_VLDB_4X : InstrItinClass;
def II_VLDB_SPARSE_RESET : InstrItinClass;
def II_VLDB_SPARSE_FILL : InstrItinClass;
def II_VLDB_SPARSE_PEEK : InstrItinClass;
def II_VLDB_SPARSE_POP : InstrItinClass;
def II_VLDB_COMPR_RESET : InstrItinClass;
def II_VLDB_COMPR_FILL : InstrItinClass;
def II_VLDB_COMPR_PEEK : InstrItinClass;
def II_VLDB_COMPR_POP : InstrItinClass;
def II_VLDB_PTR : InstrItinClass;
def II_VLDB_PTR_POSTINC : InstrItinClass;
def II_VLDB_PTR_2D : InstrItinClass;
def II_VLDB_PTR_3D : InstrItinClass;
def II_VLDB_UNPACK : InstrItinClass;
def II_VLDB_UNPACK_POSTINC : InstrItinClass;
def II_VLDB_UNPACK_2D : InstrItinClass;
def II_VLDB_UNPACK_3D : InstrItinClass;
def II_LDA_Q : InstrItinClass;
def II_LDA_POSTINC_Q : InstrItinClass;
def II_LDA_2D_Q : InstrItinClass;
def II_LDA_3D_Q : InstrItinClass;
def II_VMAC : InstrItinClass;
def II_VMACf : InstrItinClass;
def II_VMAX_LT : InstrItinClass;
def II_VMIN_GE : InstrItinClass;
def II_VMOV_W : InstrItinClass;
def II_VMOV_W_CM_CM : InstrItinClass;
def II_VMOV_W_CM_WMH : InstrItinClass;
def II_VMOV_W_WMH_CM : InstrItinClass;
def II_VMOV_W_CM_WML : InstrItinClass;
def II_VMOV_W_WML_CM : InstrItinClass;
def II_VMOV_W_WMH_WMH : InstrItinClass;
def II_VMOV_W_WML_WMH : InstrItinClass;
def II_VMOV_W_WMH_WML : InstrItinClass;
def II_VMOV_W_WML_WML : InstrItinClass;
def II_VMOV_X : InstrItinClass;
def II_VMOV_X_BM_BM : InstrItinClass;
def II_VMOV_X_BM_XM : InstrItinClass;
def II_VMOV_X_XM_BM : InstrItinClass;
def II_VMOV_X_XM_XM : InstrItinClass;
def II_VMOV_CM : InstrItinClass;
def II_VMOV_CASCADE_READ : InstrItinClass;
def II_VMOV_CASCADE_WRITE : InstrItinClass;
def II_VMSC : InstrItinClass;
def II_VMSCf : InstrItinClass;
def II_VMUL : InstrItinClass;
def II_VMULf : InstrItinClass;
def II_VNEG : InstrItinClass;
def II_VNEG_GTZ : InstrItinClass;
def II_VNEGADD : InstrItinClass;
def II_VNEGADDf : InstrItinClass;
def II_VNEGMAC : InstrItinClass;
def II_VNEGMSC : InstrItinClass;
def II_VNEGMUL : InstrItinClass;
def II_VNEGMULf : InstrItinClass;
def II_VNEGf : InstrItinClass;
def II_VPACK : InstrItinClass;
def II_VPUSH_HI : InstrItinClass;
def II_VPUSH_LO : InstrItinClass;
def II_VSEL : InstrItinClass;
def II_VADDSUB : InstrItinClass;
def II_VSHIFT : InstrItinClass;
def II_VSHIFT_ALIGN : InstrItinClass;
def II_VSHUFFLE : InstrItinClass;
def II_VSRS : InstrItinClass;
def II_VSRSM : InstrItinClass;
def II_VST_W : InstrItinClass;
def II_VST_POSTINC_W : InstrItinClass;
def II_VST_2D_W : InstrItinClass;
def II_VST_3D_W : InstrItinClass;
def II_VST_AM : InstrItinClass;
def II_VST_POSTINC_AM : InstrItinClass;
def II_VST_2D_AM : InstrItinClass;
def II_VST_3D_AM : InstrItinClass;
def II_VST_SRS : InstrItinClass;
def II_VST_POSTINC_SRS : InstrItinClass;
def II_VST_2D_SRS : InstrItinClass;
def II_VST_3D_SRS : InstrItinClass;
def II_VST_PACK : InstrItinClass;
def II_VST_PACK_POSTINC : InstrItinClass;
def II_VST_2D_PACK : InstrItinClass;
def II_VST_3D_PACK : InstrItinClass;
def II_VST_CONV : InstrItinClass;
def II_VST_CONV_POSTINC : InstrItinClass;
def II_VST_2D_CONV : InstrItinClass;
def II_VST_3D_CONV : InstrItinClass;
def II_VSUB : InstrItinClass;
def II_VSUB_CMP : InstrItinClass;
def II_VUNPACK : InstrItinClass;
def II_VUPS : InstrItinClass;
def II_VLDA_UPS : InstrItinClass;
def II_VLDA_POSTINC_UPS : InstrItinClass;
def II_VLDA_2D_UPS: InstrItinClass;
def II_VLDA_3D_UPS: InstrItinClass;
def II_VCMP : InstrItinClass;
def II_XOR : InstrItinClass;
def II_VMAXDIFF_LT : InstrItinClass;
def II_MOVd1 : InstrItinClass;
def II_MOVd2 : InstrItinClass;
def II_MOVd3 : InstrItinClass;
def II_MOVd4 : InstrItinClass;
def II_MOVd5 : InstrItinClass;
def II_MOVd6 : InstrItinClass;
def II_MOV_CNTR : InstrItinClass;
def II_VMOV_D : InstrItinClass;

def AIE2Itineraries : ProcessorItineraries<
[EMPTY_FU,
 R_WX_PORT, R_WA_PORT, R_RV_PORT, RS_WM_PORT,
 P_RM_PORT, P_WM_PORT,
 M_WM_PORT, DC_WM_PORT, DJ_WM_PORT, DN_WM_PORT,
 W_RS_PORT, W_WA_PORT, W_WM_PORT,
 CM_RM_PORT, CM_WA_PORT, CM_WM_PORT, PROC_BUS, PART_WORD_STORE,
 UPPER_SRS, STORE_UNIT, LOAD_UNIT_A, SEMAPHORE, UPS_UNIT, DONE_UNIT],
[MOV_Bypass,VEC_Bypass], [
InstrItinData<II_ABS, [InstrStage<1,  [R_WX_PORT]>], [1,1,/*def:srCarry*/1]>,

// Note that ACQ operations are not completely modeled here.  Some aspects of their
// scheduling dependencies are implemented as ScheduleDAGMutations.
InstrItinData<II_ACQ, [EmptyCycles<1>, InstrStage<4, [SEMAPHORE]>], [1, 1]>,
InstrItinData<II_ACQ_COND, [EmptyCycles<1>, InstrStage<4, [SEMAPHORE]>], [1, 1, 1]>,

InstrItinData<II_ADC, [InstrStage<1,  [R_WX_PORT]>],
              [1,1,1,/*def:srCarry*/1,/*use:srCarry*/1]>,
InstrItinData<II_ADD, [InstrStage<1,  [R_WX_PORT]>],
              [1,1,1,/*def:srCarry*/1]>,
InstrItinData<II_ADD_NC, [PrefixCycle<P_WM_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<DJ_WM_PORT>,
                         PrefixCycle<DN_WM_PORT>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RS_WM_PORT>],
                         [1,1,1]>,
InstrItinData<II_ADD_NC_DC, [PrefixCycle<RS_WM_PORT>, SimpleCycle<DC_WM_PORT>], [1,1,1]>,
InstrItinData<II_ADD_NC_DJ, [PrefixCycle<RS_WM_PORT>, SimpleCycle<DJ_WM_PORT>], [1,1,1]>,			  
InstrItinData<II_ADD_NC_DN, [PrefixCycle<RS_WM_PORT>, SimpleCycle<DN_WM_PORT>], [1,1,1]>,
InstrItinData<II_ADD_NC_M, [PrefixCycle<RS_WM_PORT>, SimpleCycle<M_WM_PORT>], [1,1,1]>,
InstrItinData<II_ADD_NC_P, [PrefixCycle<RS_WM_PORT>, SimpleCycle<P_WM_PORT>], [1,1,1]>,
InstrItinData<II_ADD_NC_RS, [SimpleCycle<RS_WM_PORT>], [1,1,1]>,	                         
InstrItinData<II_AND, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_ASHL, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_CLB, [InstrStage<1,  [R_WX_PORT]>], [1,1,1]>,
InstrItinData<II_CLZ, [InstrStage<1,  [R_WX_PORT]>], [1,1,1]>,
InstrItinData<II_DIVS, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1,1]>,
InstrItinData<II_DONE, [EmptyCycles<3>, InstrStage<3, [DONE_UNIT]>]>,
InstrItinData<II_EQ, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_EQZ, [InstrStage<1,  [R_WX_PORT]>], [1,1,1]>,
InstrItinData<II_EVENT, [], [ 1,1]>,
InstrItinData<II_EXTENDs8, [InstrStage<1,  [R_WX_PORT]>], [1,1,1]>,
InstrItinData<II_EXTENDs16, [InstrStage<1,  [R_WX_PORT]>], [1,1,1]>,
InstrItinData<II_EXTENDu8, [InstrStage<1,  [R_WX_PORT]>], [1,1,1]>,
InstrItinData<II_EXTENDu16, [InstrStage<1,  [R_WX_PORT]>], [1,1,1]>,
InstrItinData<II_GE, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_GEU, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_J, [], [1,1]>,
InstrItinData<II_J_IND, [SimpleCycle<P_RM_PORT>], [1]>,
InstrItinData<II_JL, [], [1,4]>,
InstrItinData<II_JL_IND, [SimpleCycle<P_RM_PORT>], [1, 4]>,
InstrItinData<II_JNZ, [], [1,1]>,
InstrItinData<II_JNZD, [SimpleCycle<P_RM_PORT>], [1,1,1,/*def:srCarry*/1]>,
InstrItinData<II_JZ, [], [ 1,1]>,
MemInstrItinData<II_LDA,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>, EmptyCycles<4>,
     PrefixCycle<P_WM_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<DJ_WM_PORT>,
     PrefixCycle<DN_WM_PORT>, PrefixCycle<DC_WM_PORT>, SimpleCycle<R_WA_PORT>],
    [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_DC,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_DJ,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_DN,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_M,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_P,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_R,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<R_WA_PORT>], [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_1D,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>, EmptyCycles<4>,
     PrefixCycle<P_WM_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<DJ_WM_PORT>,
     PrefixCycle<DN_WM_PORT>, PrefixCycle<DC_WM_PORT>, SimpleCycle<R_WA_PORT>],
    [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_1D_DC,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_1D_DJ,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_1D_DN,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_1D_M,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_1D_P,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_1D_R,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<R_WA_PORT>], [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_2D,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
  	 EmptyCycles<4>,
     PrefixCycle<P_WM_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<DJ_WM_PORT>,
     PrefixCycle<DN_WM_PORT>, PrefixCycle<DC_WM_PORT>, SimpleCycle<R_WA_PORT>],
    [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_2D_DC,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_2D_DJ,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_2D_DN,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_2D_M,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_2D_P,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_2D_R,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<R_WA_PORT>], [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_3D,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>, EmptyCycles<4>,
     PrefixCycle<P_WM_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<DJ_WM_PORT>,
     PrefixCycle<DN_WM_PORT>, PrefixCycle<DC_WM_PORT>, SimpleCycle<R_WA_PORT>],
    [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_3D_DC,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DC_WM_PORT>], [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_3D_DJ,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DJ_WM_PORT>], [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_3D_DN,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<DN_WM_PORT>], [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_3D_M,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<M_WM_PORT>], [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_3D_P,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<P_WM_PORT>], [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POST_3D_R,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
	EmptyCycles<4>, SimpleCycle<R_WA_PORT>], [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_TM,
    [SimpleCycle<PROC_BUS>, SimpleCycle<LOAD_UNIT_A>, EmptyCycles<4>,
     SimpleCycle<R_WA_PORT>],
    [7,1,1],
    MemoryCycles<[5]>>,
InstrItinData<II_LSHL, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_LT, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_LTU, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_MOV, [PrefixCycle<P_WM_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<DJ_WM_PORT>,
                       PrefixCycle<DN_WM_PORT>, PrefixCycle<DC_WM_PORT>, SimpleCycle<RS_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_DC, [PrefixCycle<RS_WM_PORT>, SimpleCycle<DC_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_DJ, [PrefixCycle<RS_WM_PORT>, SimpleCycle<DJ_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_DN, [PrefixCycle<RS_WM_PORT>, SimpleCycle<DN_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_M, [PrefixCycle<RS_WM_PORT>, SimpleCycle<M_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_P, [PrefixCycle<RS_WM_PORT>, SimpleCycle<P_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_RS, [SimpleCycle<RS_WM_PORT>], [1,1]>,
InstrItinData<II_MOVA, [PrefixCycle<P_WM_PORT>, PrefixCycle<M_WM_PORT>, PrefixCycle<DJ_WM_PORT>,
                        PrefixCycle<DN_WM_PORT>, PrefixCycle<DC_WM_PORT>, SimpleCycle<R_WA_PORT>], [1,1]>,
InstrItinData<II_MOVA_DC, [SimpleCycle<DC_WM_PORT>], [1,1]>,
InstrItinData<II_MOVA_DJ, [SimpleCycle<DJ_WM_PORT>], [1,1]>,
InstrItinData<II_MOVA_DN, [SimpleCycle<DN_WM_PORT>], [1,1]>,
InstrItinData<II_MOVA_M, [SimpleCycle<M_WM_PORT>], [1,1]>,
InstrItinData<II_MOVA_P, [SimpleCycle<P_WM_PORT>], [1,1]>,
InstrItinData<II_MOVA_R, [SimpleCycle<R_WA_PORT>], [1,1]>,
InstrItinData<II_MOVX, [SimpleCycle<R_WX_PORT>], [1,1]>,
InstrItinData<II_MOV_SCL, [PrefixCycle<P_RM_PORT>, PrefixCycle<RS_WM_PORT>, PrefixCycle<P_WM_PORT>, PrefixCycle<M_WM_PORT>,
                           PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, SimpleCycle<DC_WM_PORT>],
                           [1,1]>,
InstrItinData<II_MOV_SCL_DC, [PrefixCycle<P_RM_PORT>,PrefixCycle<RS_WM_PORT>,  SimpleCycle<DC_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_SCL_DJ, [PrefixCycle<P_RM_PORT>,PrefixCycle<RS_WM_PORT>,  SimpleCycle<DJ_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_SCL_DN, [PrefixCycle<P_RM_PORT>,PrefixCycle<RS_WM_PORT>,  SimpleCycle<DN_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_SCL_M, [PrefixCycle<P_RM_PORT>,PrefixCycle<RS_WM_PORT>,  SimpleCycle<M_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_SCL_P, [PrefixCycle<P_RM_PORT>,PrefixCycle<RS_WM_PORT>,  SimpleCycle<P_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_SCL_RS, [PrefixCycle<P_RM_PORT>, SimpleCycle<RS_WM_PORT>], [1,1]>,
InstrItinData<II_MOV_SS, [AvoidSemaphore<7>, EmptyCycles<6>, SimpleCycle<R_WA_PORT>], [7,/*def:srSS0*/8]>,
InstrItinData<II_MOVd1, [SimpleCycle<P_RM_PORT>, SimpleCycle<RS_WM_PORT>], [2,1]>,
InstrItinData<II_MOVd2, [SimpleCycle<P_RM_PORT>, EmptyCycles<1>, SimpleCycle<RS_WM_PORT>], [3,1]>,
InstrItinData<II_MOVd3, [SimpleCycle<P_RM_PORT>, EmptyCycles<2>, SimpleCycle<RS_WM_PORT>], [4,1]>,
InstrItinData<II_MOVd4, [SimpleCycle<P_RM_PORT>, EmptyCycles<3>, SimpleCycle<RS_WM_PORT>], [5,1]>,
InstrItinData<II_MOVd5, [SimpleCycle<P_RM_PORT>, EmptyCycles<4>, SimpleCycle<RS_WM_PORT>], [6,1]>,
InstrItinData<II_MOVd6, [SimpleCycle<P_RM_PORT>, EmptyCycles<5>, SimpleCycle<RS_WM_PORT>], [7,1]>,
InstrItinData<II_MUL, [EmptyCycles<1>, SimpleCycle<R_WX_PORT>], [2,1,1]>,
InstrItinData<II_NE, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_NEZ, [InstrStage<1,  [R_WX_PORT]>], [1,1,1]>,
InstrItinData<II_NOP, [], []>,
InstrItinData<II_OR, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_PADD, [], [1,1,1]>,
InstrItinData<II_PADD_2D, [], [1,1,1,1]>,
InstrItinData<II_PADD_3D, [], [1,1,1,1,1]>,
InstrItinData<II_MOV_CNTR, [], [1,/*CNTR*/1]>,

// Note that REL operations are not completely modeled here.  Some aspects of their
// scheduling dependencies are implemented as ScheduleDAGMutations.
InstrItinData<II_REL,[EmptyCycles<1>, InstrStage<4, [SEMAPHORE]>], [1,1]>,
InstrItinData<II_REL_COND, [EmptyCycles<1>, InstrStage<4, [SEMAPHORE]>], [1,1,1]>,

InstrItinData<II_RET, [], [1,1]>,
InstrItinData<II_SBC, [InstrStage<1,  [R_WX_PORT]>],
              [1,1,1,/*def:srCarry*/1,/*use:srCarry*/1]>,
InstrItinData<II_SELEQZ, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,
InstrItinData<II_SELNEZ, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>,

// FIXME-DYN-ITIN: P_RM_PORT will hurt. We only need it if storing ptr/modifiers
MemInstrItinData<II_ST,
    [AvoidPartWordStore, PrefixCycle<R_RV_PORT>, SimpleCycle<P_RM_PORT>,
     SimpleCycle<STORE_UNIT>],
    [1,1,1],
    MemoryCycles<[5]>>,
InstrItinData<II_ST_MS,
    [PrefixCycle<R_RV_PORT>, SimpleCycle<P_RM_PORT>, AvoidSemaphore<2>],
    [1,1]>,
InstrItinData<II_ST_MS_NB,
    [PrefixCycle<R_RV_PORT>, SimpleCycle<P_RM_PORT>, AvoidSemaphore<2>],
    [1,/*def:srMS0*/3]>,
InstrItinData<II_ST_MS_NB_TLAST_REG,
    [PrefixCycle<R_RV_PORT>, SimpleCycle<P_RM_PORT>, AvoidSemaphore<2>],
    [1,1,/*def:srMS0*/3]>,
InstrItinData<II_MOV_PH_B,
    [SimpleCycle<R_RV_PORT>, AvoidSemaphore<5>],
    [1,1,/*tlast*/1]>,
InstrItinData<II_MOV_PH_NB,
    [SimpleCycle<R_RV_PORT>, AvoidSemaphore<5>],
    [1,1,/*def:srMS0*/3]>,
InstrItinData<II_MOV_PH_NB_TLAST_REG,
    [SimpleCycle<R_RV_PORT>, AvoidSemaphore<5>],
    [1,1,/*tlast*/1,/*def:srMS0*/3]>,
InstrItinData<II_MOV_CPH_B,
    [PrefixCycle<P_RM_PORT>, SimpleCycle<R_RV_PORT>, AvoidSemaphore<5>],
    [1,1,1,1,/*tlast*/1]>,
InstrItinData<II_MOV_CPH_NB,
    [PrefixCycle<P_RM_PORT>, SimpleCycle<R_RV_PORT>, AvoidSemaphore<5>],
    [1,1,1,1,/*def:srMS0*/3]>,
InstrItinData<II_MOV_CPH_NB_TLAST_REG,
    [PrefixCycle<P_RM_PORT>, SimpleCycle<R_RV_PORT>, AvoidSemaphore<5>],
    [1,1,1,1,/*tlast*/1,/*def:srMS0*/3]>,
MemInstrItinData<II_ST_POST_1D,
    [AvoidPartWordStore, PrefixCycle<R_RV_PORT>, SimpleCycle<P_RM_PORT>,
     SimpleCycle<STORE_UNIT>],
    [1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_ST_POST_2D,
    [AvoidPartWordStore, PrefixCycle<R_RV_PORT>, SimpleCycle<P_RM_PORT>,
     SimpleCycle<STORE_UNIT>],
    [1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_ST_POST_3D,
    [AvoidPartWordStore, PrefixCycle<R_RV_PORT>, SimpleCycle<P_RM_PORT>,
     SimpleCycle<STORE_UNIT>],
    [1,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_ST_TM,
    [PrefixCycle<PROC_BUS>, PrefixCycle<R_RV_PORT>,
     SimpleCycle<P_RM_PORT>, SimpleCycle<STORE_UNIT>],
    [1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_ST_Q,
    [AvoidPartWordStore, EmptyCycles<1>,
     SimpleCycle<STORE_UNIT>],
     [1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_ST_POSTINC_Q,
    [AvoidPartWordStore, EmptyCycles<1>,
     SimpleCycle<STORE_UNIT>],
    [1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_ST_2D_Q,
    [AvoidPartWordStore, EmptyCycles<1>,
     SimpleCycle<STORE_UNIT>],
    [1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_ST_3D_Q,
    [AvoidPartWordStore, EmptyCycles<1>,
     SimpleCycle<STORE_UNIT>],
    [1,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_STHB,
    [IsPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>, EmptyCycles<4>,
     PrefixCycle<R_WA_PORT>, SimpleCycle<R_RV_PORT>, SimpleCycle<STORE_UNIT>],
    [7,1,1],
    MemoryCycles<[5, 11]>>,
MemInstrItinData<II_STHB_POST_1D,
    [IsPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>, EmptyCycles<4>,
     PrefixCycle<R_WA_PORT>, SimpleCycle<R_RV_PORT>, SimpleCycle<STORE_UNIT>],
    [1,7,1,1],
    MemoryCycles<[5, 11]>>,
MemInstrItinData<II_STHB_2D,
    [IsPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>, EmptyCycles<4>,
     PrefixCycle<R_WA_PORT>, SimpleCycle<R_RV_PORT>, SimpleCycle<STORE_UNIT>],
    [1,1,7,1,1],
    MemoryCycles<[5, 11]>>,
MemInstrItinData<II_STHB_3D,
    [IsPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>, EmptyCycles<4>,
     PrefixCycle<R_WA_PORT>, SimpleCycle<R_RV_PORT>, SimpleCycle<STORE_UNIT>],
    [1,1,1,7,1,1],
    MemoryCycles<[5, 11]>>,
InstrItinData<II_SUB, [InstrStage<1,  [R_WX_PORT]>],
              [1,1,1,/*def:srCarry*/1]>,
InstrItinData<II_VABS_GTZ, [EmptyCycles<1>, PrefixCycle<W_WM_PORT>, SimpleCycle<RS_WM_PORT>],
              [2,2,1,/*crVaddSign*/1], [MOV_Bypass,NoBypass,MOV_Bypass]>,
InstrItinData<II_VACC, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<1>, InstrStage<1, [CM_RM_PORT]>,
              EmptyCycles<1>, InstrStage<1, [CM_WA_PORT]>],
              [5,3,3,1], [VEC_Bypass,VEC_Bypass,VEC_Bypass,NoBypass]>,
InstrItinData<II_VACCf, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<1>, InstrStage<1, [CM_RM_PORT]>,
              EmptyCycles<2>, InstrStage<1, [CM_WA_PORT]>],
              [6,3,3,1,/*srFPFlags*/7, /*crFPMask*/7]>,
InstrItinData<II_VNEG, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<3>, InstrStage<1, [CM_WA_PORT]>],
              [5,3,1], [VEC_Bypass, VEC_Bypass, NoBypass]>,
InstrItinData<II_VNEGf, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<4>, InstrStage<1, [CM_WA_PORT]>],
              [6,3,1,/*srFPFlags*/7, /*crFPMask*/7]>,
/* This is the Note from ISA-Spec2.1: Currently the instruction shows the following wrong behavior.
 * The variant of this instruction which writes to a BM register may generate floating-point
 * exception flags and events (see Section Floating-point Event Generation).
 * It reads control register crFPMask and writes status register srFPFlags in stage E7.
 */
InstrItinData<II_VCLRf, [EmptyCycles<5>, InstrStage<1, [CM_WA_PORT]>],
              [6,/*srFPFlags*/7, /*crFPMask*/7]>,
InstrItinData<II_VCLR, [EmptyCycles<4>, InstrStage<1, [CM_WA_PORT]>],
              [5], [VEC_Bypass]>,
InstrItinData<II_VADD, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1], [MOV_Bypass,MOV_Bypass,MOV_Bypass]>,
InstrItinData<II_VADDMAC, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<1>, InstrStage<1, [CM_RM_PORT]>,
              EmptyCycles<1>, InstrStage<1, [CM_WA_PORT]>],
              [5,3,3,1,1,1], [VEC_Bypass,VEC_Bypass,VEC_Bypass,NoBypass,NoBypass,NoBypass]>,
InstrItinData<II_VADDMACf, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<1>, InstrStage<1, [CM_RM_PORT]>,
              EmptyCycles<2>, InstrStage<1, [CM_WA_PORT]>],
              [6,3,3,1,1,1,/*srFPFlags*/7, /*crFPMask*/7]>,
InstrItinData<II_VSUB, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1], [MOV_Bypass,MOV_Bypass,MOV_Bypass]>,
InstrItinData<II_VBLOG, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1], [MOV_Bypass,MOV_Bypass,MOV_Bypass]>,
InstrItinData<II_VBCST, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1], [MOV_Bypass,NoBypass]>,
InstrItinData<II_VBCSTSHFL, [EmptyCycles<1>, InstrStage<1,[W_WM_PORT],0>, InstrStage<1,[CM_WM_PORT],0>],
              [2,1,1], [MOV_Bypass,NoBypass,NoBypass]>,
InstrItinData<II_VBNEG_LTZ, [EmptyCycles<1>, PrefixCycle<W_WM_PORT>, SimpleCycle<RS_WM_PORT>],
              [2,2,1], [MOV_Bypass,NoBypass,MOV_Bypass]>,
InstrItinData<II_VEXTBCST, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1], [MOV_Bypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VEXTRACT, [EmptyCycles<1>, PrefixCycle<RS_WM_PORT>, PrefixCycle<P_WM_PORT>, PrefixCycle<M_WM_PORT>,
                            PrefixCycle<DJ_WM_PORT>, PrefixCycle<DN_WM_PORT>, SimpleCycle<DC_WM_PORT>],
              [2,1,1,/*crVaddSign*/1], [NoBypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VEXTRACT_DC, [EmptyCycles<1>, PrefixCycle<RS_WM_PORT>, SimpleCycle<DC_WM_PORT>],
              [2,1,1,/*crVaddSign*/1], [NoBypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VEXTRACT_DJ, [EmptyCycles<1>, PrefixCycle<RS_WM_PORT>, SimpleCycle<DJ_WM_PORT>],
              [2,1,1,/*crVaddSign*/1], [NoBypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VEXTRACT_DN, [EmptyCycles<1>, PrefixCycle<RS_WM_PORT>, SimpleCycle<DN_WM_PORT>],
              [2,1,1,/*crVaddSign*/1], [NoBypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VEXTRACT_M, [EmptyCycles<1>, PrefixCycle<RS_WM_PORT>, SimpleCycle<M_WM_PORT>],
              [2,1,1,/*crVaddSign*/1], [NoBypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VEXTRACT_P, [EmptyCycles<1>, PrefixCycle<RS_WM_PORT>, SimpleCycle<P_WM_PORT>],
              [2,1,1,/*crVaddSign*/1], [NoBypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VEXTRACT_RS, [EmptyCycles<1>, SimpleCycle<RS_WM_PORT>],
              [2,1,1,/*crVaddSign*/1], [NoBypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VINSERT, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1,1], [MOV_Bypass,MOV_Bypass,NoBypass,NoBypass]>,
// Note: Similar to SRS instructions, VUPS/VLDA.UPS have structural hazard
MemInstrItinData<II_VLDA_UPS,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>, EmptyCycles<4>,
     InstrStage<1,[UPS_UNIT]>, EmptyCycles<1>, InstrStage<1,[CM_WM_PORT]>],
    [9,7,1,1,/*def:srUPS_of*/8,/*crSat*/8,/*crUPSSign*/7],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_POSTINC_UPS,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>, EmptyCycles<4>,
     SimpleCycle<UPS_UNIT>, EmptyCycles<1>, SimpleCycle<CM_WM_PORT>],
    [9,1,7,1,1,/*def:srUPS_of*/8,/*crSat*/8,/*crUPSSign*/7],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_2D_UPS,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>, EmptyCycles<4>,
     InstrStage<1,[UPS_UNIT]>, EmptyCycles<1>, InstrStage<1,[CM_WM_PORT]>],
    [9,1,1,7,1,1,/*def:srUPS_of*/8,/*crSat*/8,/*crUPSSign*/7],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_3D_UPS,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>, EmptyCycles<4>,
     InstrStage<1,[UPS_UNIT]>, EmptyCycles<1>, InstrStage<1,[CM_WM_PORT]>],
    [9,1,1,1,7,1,1,/*def:srUPS_of*/8,/*crSat*/8,/*crUPSSign*/7],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_W,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [W_WA_PORT]>],
    [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_POSTINC_W,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
     EmptyCycles<4>, SimpleCycle<W_WA_PORT>],
    [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_2D_W,
    [AvoidPartWordStore, EmptyCycles<1>,InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [W_WA_PORT]>],
    [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_3D_W,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [W_WA_PORT]>],
    [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_AM,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [CM_WM_PORT]>],
    [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_POSTINC_AM,
    [AvoidPartWordStore, EmptyCycles<1>, SimpleCycle<LOAD_UNIT_A>,
     EmptyCycles<4>, SimpleCycle<CM_WM_PORT>],
    [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_2D_AM,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [CM_WM_PORT]>],
    [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_3D_AM,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [CM_WM_PORT]>],
    [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_CONV,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [CM_WM_PORT]>],
    [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_POSTINC_CONV,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [CM_WM_PORT]>],
    [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_2D_CONV,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [CM_WM_PORT]>],
    [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDA_3D_CONV,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>,
     EmptyCycles<4>, InstrStage<1, [CM_WM_PORT]>],
    [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB, [AvoidPartWordStore], [7,1,1], MemoryCycles<[5]>>,

MemInstrItinData<II_VLDB_SPARSE_FILL, [AvoidPartWordStore],
    [/*ptr_out*/1,/*ptr*/1,/*def:srSparse_of*/6,/*use:DP*/6],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_SPARSE_RESET, [AvoidPartWordStore],
    [/*ptr_out*/1,/*ptr*/1,/*def:srSparse_of*/6,/*def:DP*/6,/*use:DP*/6],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_SPARSE_PEEK, [AvoidPartWordStore],
    [/*ptr_out*/1,/*dst*/7,/*ptr*/1,/*def:srSparse_of*/6, /*def:srCompr_uf*/5,/*use:DP*/6],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_SPARSE_POP, [AvoidPartWordStore],
    [/*ptr_out*/1,/*dst*/7,/*ptr*/1,/*def:srSparse_of*/6, /*def:srCompr_uf*/5, /*def:DP*/6,/*use:DP*/6],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_COMPR_FILL, [AvoidPartWordStore],
    [/*ptr_out*/1,/*ptr*/1,/*use:DP*/6],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_COMPR_RESET, [AvoidPartWordStore],
    [/*ptr_out*/1,/*ptr*/1,/*def:DP*/6,/*use:DP*/6],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_COMPR_PEEK, [AvoidPartWordStore],
    [/*ptr_out*/1,/*dst*/7,/*ptr*/1,/*def:srCompr_uf*/5,/*use:DP*/6],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_COMPR_POP, [AvoidPartWordStore],
    [/*ptr_out*/1,/*dst*/7,/*ptr*/1,/*def:srCompr_uf*/5, /*def:DP*/6,/*use:DP*/6],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_POSTINC, [AvoidPartWordStore], [7,1,1,1], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_2D, [AvoidPartWordStore], [7,1,1,1,1], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_3D, [AvoidPartWordStore], [7,1,1,1,1,1], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_4X,
    [AvoidPartWordStore, SimpleCycle<W_RS_PORT>, SimpleCycle<LOAD_UNIT_A>],
    [7,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_PTR, [AvoidPartWordStore], [7,1,1], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_PTR_POSTINC, [AvoidPartWordStore], [7,1,1,1], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_PTR_2D, [AvoidPartWordStore], [7,1,1,1,1], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_PTR_3D, [AvoidPartWordStore], [7,1,1,1,1,1], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_UNPACK, [AvoidPartWordStore], [7,1,1,/*crUnpackSign*/7], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_UNPACK_POSTINC, [AvoidPartWordStore], [7,1,1,1,/*crUnpackSign*/7], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_UNPACK_2D, [AvoidPartWordStore], [7,1,1,1,1,/*crUnpackSign*/7], MemoryCycles<[5]>>,
MemInstrItinData<II_VLDB_UNPACK_3D, [AvoidPartWordStore], [7,1,1,1,1,1,/*crUnpackSign*/7], MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_Q,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>],
    [7,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_POSTINC_Q,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>],
    [7,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_2D_Q,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>],
    [7,1,1,1,1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_LDA_3D_Q,
    [AvoidPartWordStore, EmptyCycles<1>, InstrStage<1, [LOAD_UNIT_A]>],
    [7,1,1,1,1,1],
    MemoryCycles<[5]>>,
InstrItinData<II_VMAC, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<3>, InstrStage<1, [CM_WA_PORT]>],
              [5,3,1,1,1], [VEC_Bypass,VEC_Bypass,NoBypass,NoBypass]>,
InstrItinData<II_VMAX_LT, [EmptyCycles<1>, PrefixCycle<W_WM_PORT>, SimpleCycle<RS_WM_PORT>],
              [2,2,1,1,/*crVaddSign*/1], [MOV_Bypass,NoBypass,MOV_Bypass,MOV_Bypass]>,
InstrItinData<II_VMIN_GE, [EmptyCycles<1>, PrefixCycle<W_WM_PORT>, SimpleCycle<RS_WM_PORT>],
              [2,2,1,1,/*crVaddSign*/1], [MOV_Bypass,NoBypass,MOV_Bypass,MOV_Bypass]>,
InstrItinData<II_VSUB_CMP, [EmptyCycles<1>, PrefixCycle<W_WM_PORT>, SimpleCycle<RS_WM_PORT>],
              [2,2,1,1,/*crVaddSign*/1], [MOV_Bypass,NoBypass,MOV_Bypass,MOV_Bypass]>,
InstrItinData<II_VMAXDIFF_LT, [EmptyCycles<1>, PrefixCycle<W_WM_PORT>, SimpleCycle<RS_WM_PORT>],
              [2,2,1,1,/*crVaddSign*/1], [MOV_Bypass,NoBypass,MOV_Bypass,MOV_Bypass]>,
// We don't reserve the W_WM_PORT for VGE/VLT/VEQZ because they don't write to W or X register file.
InstrItinData<II_VCMP, [EmptyCycles<1>, SimpleCycle<RS_WM_PORT>],
              [2,1,1,/*crVaddSign*/1], [NoBypass,MOV_Bypass,MOV_Bypass]>,
InstrItinData<II_VEQZ, [EmptyCycles<1>, SimpleCycle<RS_WM_PORT>],
              [2,1], [NoBypass,MOV_Bypass]>,
InstrItinData<II_VMSC, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<3>, InstrStage<1, [CM_WA_PORT]>],
              [5,3,1,1,1], [VEC_Bypass,VEC_Bypass,NoBypass,NoBypass]>,
InstrItinData<II_VNEG_GTZ, [EmptyCycles<1>, PrefixCycle<W_WM_PORT>, SimpleCycle<RS_WM_PORT>],
              [2,2,1], [MOV_Bypass,NoBypass,MOV_Bypass]>,
InstrItinData<II_VNEGMAC, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<3>, InstrStage<1, [CM_WA_PORT]>],
              [5,3,1,1,1], [VEC_Bypass,VEC_Bypass,NoBypass,NoBypass]>,
InstrItinData<II_VNEGMSC, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<3>, InstrStage<1, [CM_WA_PORT]>],
              [5,3,1,1,1], [VEC_Bypass,VEC_Bypass,NoBypass,NoBypass]>,
InstrItinData<II_VMACf, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<4>, InstrStage<1, [CM_WA_PORT]>],
              [6,3,1,1,1,/*srFPFlags*/7, /*crFPMask*/7]>,
InstrItinData<II_VMSCf, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<4>, InstrStage<1, [CM_WA_PORT]>],
              [6,3,1,1,1,/*srFPFlags*/7, /*crFPMask*/7]>,
InstrItinData<II_VMOV_D, [EmptyCycles<4>, InstrStage<1, [CM_WA_PORT]>],
              [5,3], [VEC_Bypass,NoBypass]>,
InstrItinData<II_VMOV_W, [SimpleCycle<CM_RM_PORT>, PrefixCycle<W_WM_PORT>, SimpleCycle<CM_WM_PORT>],
              [2,1], [NoBypass, NoBypass]>,
InstrItinData<II_VMOV_W_CM_CM, [SimpleCycle<CM_RM_PORT>, SimpleCycle<CM_WM_PORT>],
              [2,1], [NoBypass, NoBypass]>,
InstrItinData<II_VMOV_W_WMH_CM, [SimpleCycle<CM_RM_PORT>, SimpleCycle<W_WM_PORT>],
              [2,1], [NoBypass, NoBypass]>,
InstrItinData<II_VMOV_W_CM_WMH, [EmptyCycles<1>, SimpleCycle<CM_WM_PORT>],
              [2,1], [NoBypass, NoBypass]>,
InstrItinData<II_VMOV_W_WML_CM, [SimpleCycle<CM_RM_PORT>, SimpleCycle<W_WM_PORT>],
              [2,1], [MOV_Bypass, NoBypass]>,
InstrItinData<II_VMOV_W_CM_WML, [EmptyCycles<1>, SimpleCycle<CM_WM_PORT>],
              [2,1], [NoBypass, MOV_Bypass]>,
InstrItinData<II_VMOV_W_WMH_WMH, [EmptyCycles<1>, SimpleCycle<W_WM_PORT>],
              [2,1], [NoBypass, NoBypass]>,
InstrItinData<II_VMOV_W_WML_WMH, [EmptyCycles<1>, SimpleCycle<W_WM_PORT>],
              [2,1], [MOV_Bypass, NoBypass]>,
InstrItinData<II_VMOV_W_WMH_WML, [EmptyCycles<1>, SimpleCycle<W_WM_PORT>],
              [2,1], [NoBypass, MOV_Bypass]>,
InstrItinData<II_VMOV_W_WML_WML, [EmptyCycles<1>, SimpleCycle<W_WM_PORT>],
              [2,1], [MOV_Bypass, MOV_Bypass]>,
InstrItinData<II_VMOV_X, [SimpleCycle<CM_RM_PORT>, PrefixCycle<W_WM_PORT>, SimpleCycle<CM_WM_PORT>],
              [2,1], [NoBypass, NoBypass]>,
InstrItinData<II_VMOV_X_BM_BM, [SimpleCycle<CM_RM_PORT>, SimpleCycle<CM_WM_PORT>],
              [2,1], [NoBypass, NoBypass]>,
InstrItinData<II_VMOV_X_XM_BM, [SimpleCycle<CM_RM_PORT>, SimpleCycle<W_WM_PORT>],
              [2,1], [MOV_Bypass, NoBypass]>,
InstrItinData<II_VMOV_X_BM_XM, [EmptyCycles<1>, SimpleCycle<CM_WM_PORT>],
              [2,1], [NoBypass, MOV_Bypass]>,
InstrItinData<II_VMOV_X_XM_XM, [EmptyCycles<1>, SimpleCycle<W_WM_PORT>],
              [2,1], [MOV_Bypass, MOV_Bypass]>,
InstrItinData<II_VMOV_CM, [InstrStage<1, [CM_RM_PORT]>, InstrStage<1,  [CM_WM_PORT]>], [2,1]>,

// Note: We reserve the semaphore interface at the same cycles as ACQ/REL to
// steer cascade streams sufficiently away from them. This is very conservative,
// and will cause at least three cycles between stream operations and locks.
InstrItinData<II_VMOV_CASCADE_READ, [EmptyCycles<1>, AvoidSemaphore<6>, EmptyCycles<5>, PrefixCycle<W_WA_PORT>, SimpleCycle<CM_WM_PORT>],
              [7,/*crSCDEn*/5]>,
InstrItinData<II_VMOV_CASCADE_WRITE,[SimpleCycle<W_RS_PORT>, AvoidSemaphore<6>],
              [1,/*crMCDEn*/1]>,

InstrItinData<II_VMUL, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<3>, InstrStage<1, [CM_WA_PORT]>],
              [5,1,1,1], [VEC_Bypass,NoBypass,NoBypass]>,
InstrItinData<II_VNEGMUL, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<3>, InstrStage<1, [CM_WA_PORT]>],
              [5,1,1,1], [VEC_Bypass,NoBypass,NoBypass]>,
InstrItinData<II_VMULf, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<4>, InstrStage<1, [CM_WA_PORT]>],
              [6,1,1,1,/*srFPFlags*/7, /*crFPMask*/7]>,
InstrItinData<II_VNEGMULf, [InstrStage<1, [R_RV_PORT]>, EmptyCycles<4>, InstrStage<1, [CM_WA_PORT]>],
              [6,1,1,1,/*srFPFlags*/7, /*crFPMask*/7]>,
InstrItinData<II_VPUSH_HI, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1], [MOV_Bypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VPUSH_LO, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1], [MOV_Bypass,NoBypass,MOV_Bypass]>,
// Note: Apparently, srSRS_of can be simultaneously written by parallel
// VSRS/VST.SRS and VSRSM instructions when they have a 512-bit BM input.
InstrItinData<II_VSRS, [PrefixCycle<CM_RM_PORT>, SimpleCycle<UPPER_SRS>, EmptyCycles<2>, SimpleCycle<W_WA_PORT>],
              [4,1,1,/*def:srSRS_of*/3,/*crSat*/1,/*crRnd*/1,/*crSRSSign*/1]>,
InstrItinData<II_VSRSM, [PrefixCycle<CM_RM_PORT>, SimpleCycle<UPPER_SRS>, EmptyCycles<2>, SimpleCycle<W_WM_PORT>],
              [4,1,1,/*def:srSRS_of*/3,/*crSat*/1,/*crRnd*/1,/*crSRSSign*/1]>,
// Note: VST_SRS's store happens in E7 instead of E5.
// To accomodate for this extra delay we have to move the AvoidPartWordStore
// from E1 to E3.
MemInstrItinData<II_VST_SRS, [PrefixCycle<CM_RM_PORT>, SimpleCycle<UPPER_SRS>, EmptyCycles<1>,
                           AvoidPartWordStore, EmptyCycles<1>,
                           SimpleCycle<STORE_UNIT>],
              [1,1,1,1,/*def:srSRS_of*/3,/*crSat*/1,/*crRnd*/1,/*crSRSSign*/1],
              MemoryCycles<[7]>>,
MemInstrItinData<II_VST_POSTINC_SRS, [PrefixCycle<CM_RM_PORT>, SimpleCycle<UPPER_SRS>, EmptyCycles<1>,
                           AvoidPartWordStore, EmptyCycles<1>,
                           SimpleCycle<STORE_UNIT>],
              [1,1,1,1,1,/*def:srSRS_of*/3,/*crSat*/1,/*crRnd*/1,/*crSRSSign*/1],
              MemoryCycles<[7]>>,
MemInstrItinData<II_VST_2D_SRS, [PrefixCycle<CM_RM_PORT>, SimpleCycle<UPPER_SRS>, EmptyCycles<1>,
                           AvoidPartWordStore, EmptyCycles<1>,
                           SimpleCycle<STORE_UNIT>],
              [1,1,1,1,1,1,/*def:srSRS_of*/3,/*crSat*/1,/*crRnd*/1,/*crSRSSign*/1],
              MemoryCycles<[7]>>,
MemInstrItinData<II_VST_3D_SRS, [PrefixCycle<CM_RM_PORT>, SimpleCycle<UPPER_SRS>, EmptyCycles<1>,
                           AvoidPartWordStore, EmptyCycles<1>,
                           SimpleCycle<STORE_UNIT>],
              [1,1,1,1,1,1,1,/*def:srSRS_of*/3,/*crSat*/1,/*crRnd*/1,/*crSRSSign*/1],
              MemoryCycles<[7]>>,
MemInstrItinData<II_VST_PACK,
    [AvoidPartWordStore, SimpleCycle<W_RS_PORT>, SimpleCycle<STORE_UNIT>],
    [1,1,1,/*crSat*/1,/*crPackSign*/1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VST_PACK_POSTINC,
    [AvoidPartWordStore, SimpleCycle<W_RS_PORT>, SimpleCycle<STORE_UNIT>],
    [1,1,1,1,/*crSat*/1,/*crPackSign*/1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VST_2D_PACK,
    [AvoidPartWordStore, SimpleCycle<W_RS_PORT>, SimpleCycle<STORE_UNIT>],
    [1,1,1,1,1,/*crSat*/1,/*crPackSign*/1],
    MemoryCycles<[5]>>,
MemInstrItinData<II_VST_3D_PACK,
    [AvoidPartWordStore, SimpleCycle<W_RS_PORT>, SimpleCycle<STORE_UNIT>],
    [1,1,1,1,1,1,/*crSat*/1,/*crPackSign*/1],
    MemoryCycles<[5]>>,
// Note: VST_CONV's (bf16.fp32) store happens in E7 instead of E5.
// To accomodate for this extra delay we have to move the AvoidPartWordStore
// from E1 to E3.
MemInstrItinData<II_VST_CONV, [SimpleCycle<CM_RM_PORT>, SimpleCycle<STORE_UNIT>,
                         AvoidPartWordStore, EmptyCycles<1>],
              [1,1,1,/*def:srF2FFlags*/1,/*crRnd*/1,/*crF2FMask*/1],
              MemoryCycles<[7]>>,
MemInstrItinData<II_VST_CONV_POSTINC, [SimpleCycle<CM_RM_PORT>, SimpleCycle<STORE_UNIT>,
                         AvoidPartWordStore, EmptyCycles<1>],
              [1,1,1,1,/*def:srF2FFlags*/1,/*crRnd*/1,/*crF2FMask*/1],
              MemoryCycles<[7]>>,
MemInstrItinData<II_VST_2D_CONV, [SimpleCycle<CM_RM_PORT>, SimpleCycle<STORE_UNIT>,
                         AvoidPartWordStore, EmptyCycles<1>],
              [1,1,1,1,1,/*def:srF2FFlags*/1,/*crRnd*/1,/*crF2FMask*/1],
              MemoryCycles<[7]>>,
MemInstrItinData<II_VST_3D_CONV, [SimpleCycle<CM_RM_PORT>, SimpleCycle<STORE_UNIT>,
                         AvoidPartWordStore, EmptyCycles<1>],
              [1,1,1,1,1,1,/*def:srF2FFlags*/1,/*crRnd*/1,/*crF2FMask*/1],
              MemoryCycles<[7]>>,
// VCONV.bf16.fp32
InstrItinData<II_VCONV, [SimpleCycle<CM_RM_PORT>, SimpleCycle<W_WA_PORT>],
              [2,1,/*def:srF2FFlags*/1,/*def:crF2FMask*/1,/*crRnd*/1]>,
// VFLOOR – Vector bfloat16 to Integer Conversion, VFLOOR.s32.bf16
InstrItinData<II_VFLOORs32bf16_AM, [SimpleCycle<CM_RM_PORT>, SimpleCycle<W_WA_PORT>],
              [2,1,1,/*srF2IFlags*/2,/*crF2IMask*/1]>,
InstrItinData<II_VFLOORs32bf16_W, [SimpleCycle<W_RS_PORT>, SimpleCycle<W_WA_PORT>],
              [2,1,1,/*srF2IFlags*/2,/*crF2IMask*/1]>,
InstrItinData<II_VCONVfp32bf16, [EmptyCycles<1>, SimpleCycle<CM_WM_PORT>],
              [2,1]>,
InstrItinData<II_VCONVfp32bf16_WL, [EmptyCycles<1>, SimpleCycle<CM_WM_PORT>],
              [2,1], [NoBypass, MOV_Bypass]>, 
InstrItinData<II_VSHUFFLE, [EmptyCycles<1>, InstrStage<1,[W_WM_PORT],0>, InstrStage<1,[CM_WM_PORT],0>],
              [2,1,1,1], [MOV_Bypass,MOV_Bypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VSHIFT, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1,1], [MOV_Bypass,MOV_Bypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VSHIFT_ALIGN, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1,1,1], [MOV_Bypass,MOV_Bypass,NoBypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VSEL, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1,1], [MOV_Bypass,MOV_Bypass,MOV_Bypass,NoBypass]>,
InstrItinData<II_VADDSUB, [EmptyCycles<1>, InstrStage<1,  [W_WM_PORT]>],
              [2,1,1,1], [MOV_Bypass,MOV_Bypass,MOV_Bypass,NoBypass]>,
MemInstrItinData<II_VST_W,
    [AvoidPartWordStore, SimpleCycle<W_RS_PORT>,
                         SimpleCycle<STORE_UNIT>], [1,1,1],
                         MemoryCycles<[5]>>,
MemInstrItinData<II_VST_POSTINC_W, [AvoidPartWordStore, SimpleCycle<W_RS_PORT>,
                         SimpleCycle<STORE_UNIT>], [1,1,1,1],
                         MemoryCycles<[5]>>,
MemInstrItinData<II_VST_2D_W, [AvoidPartWordStore, SimpleCycle<W_RS_PORT>,
                         SimpleCycle<STORE_UNIT>], [1,1,1,1,1],
                         MemoryCycles<[5]>>,
MemInstrItinData<II_VST_3D_W, [AvoidPartWordStore, SimpleCycle<W_RS_PORT>,
                         SimpleCycle<STORE_UNIT>], [1,1,1,1,1,1],
                         MemoryCycles<[5]>>,
MemInstrItinData<II_VST_AM, [AvoidPartWordStore, SimpleCycle<CM_RM_PORT>,
                         SimpleCycle<STORE_UNIT>], [1,1,1],
                         MemoryCycles<[5]>>,
MemInstrItinData<II_VST_POSTINC_AM, [AvoidPartWordStore, SimpleCycle<CM_RM_PORT>,
                         SimpleCycle<STORE_UNIT>], [1,1,1,1],
                         MemoryCycles<[5]>>,
MemInstrItinData<II_VST_2D_AM, [AvoidPartWordStore, SimpleCycle<CM_RM_PORT>,
                         SimpleCycle<STORE_UNIT>], [1,1,1,1,1],
                         MemoryCycles<[5]>>,
MemInstrItinData<II_VST_3D_AM, [AvoidPartWordStore, SimpleCycle<CM_RM_PORT>,
                         SimpleCycle<STORE_UNIT>], [1,1,1,1,1,1],
                         MemoryCycles<[5]>>,
InstrItinData<II_VUNPACK,[EmptyCycles<6>, InstrStage<1, [W_RS_PORT]>],
              [7,7,/*crUnpackSign*/7]>,
InstrItinData<II_VPACK, [SimpleCycle<W_RS_PORT>, SimpleCycle<W_WA_PORT>],
              [2,1,/*crSat*/1,/*crPackSign*/1]>,
// Note: Similar to SRS instructions, VUPS/VLDA.UPS have structural hazard
InstrItinData<II_VUPS, [InstrStage<1,[UPS_UNIT]>, EmptyCycles<1>, InstrStage<1,[CM_WM_PORT]>],
              [3,1,1,/*def:srUPS_of*/1,/*crSat*/1,/*crUPSSign*/1]>,
InstrItinData<II_XOR, [InstrStage<1,  [R_WX_PORT]>], [1,1,1,1]>
]>;

def AIE2SchedModel : SchedMachineModel {
  // Issue limit is anyway limited by VLIW formats, but there is no universal
  // syntax for "no issue limit".
  int IssueWidth = 1000;

  // Allow some kind of controlled out-order execution through negative latencies
  // This is mostly to convince the scheduler that it doesn't need to bump the
  // current cycle by `x` after scheduling an instruction in cycle `CurrCycle + x`.
  // However, in pre-RA mode, MicroOpBufferSize also enables the heuristics for
  // acyclic latency of the local region vs cyclic critical path latency of the
  // enclosing loop in pre-RA mode. See checkAcyclicLatency().
  // I haven't seen any effect on QoR at this point.
  int MicroOpBufferSize = 1000;

  let Itineraries = AIE2Itineraries;
  int LoadLatency = 5; // FIXME
  int MispredictPenalty = 4; // FIXME
  int HighLatency = 37; // FIXME
  let CompleteModel = 0;
  let PostRAScheduler = 1;
}
