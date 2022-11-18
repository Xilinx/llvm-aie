// RUN: llvm-tblgen -gen-global-isel -optimize-match-table=false -warn-on-skipped-patterns -I %p/../../include -I %p/Common %s -o - < %s | FileCheck %s

include "llvm/Target/Target.td"
include "GlobalISelEmitterCommon.td"

// Verify that using an instruction with implicit defs in the middle of a
// pattern is supported in the GlobalISelEmitter.

def LSHL : I<(outs GPR32:$dst), (ins GPR32:$src0, GPR32:$src1), []>;
let Defs = [B0] in
def NEG : I<(outs GPR32:$dst), (ins GPR32:$src0), []>;
def : Pat<(srl (i32 GPR32:$rs1), (i32 GPR32:$rs2)),
        (LSHL GPR32:$rs1, (NEG GPR32:$rs2))>;

def ASHL : I<(outs GPR32:$dst), (ins GPR32:$src0, GPR32:$src1), []>;
let Defs = [B0,F0] in
def NEG2 : I<(outs GPR32:$dst), (ins GPR32:$src0), []>;
def : Pat<(sra (i32 GPR32:$rs1), (i32 GPR32:$rs2)),
        (ASHL GPR32:$rs1, (NEG2 GPR32:$rs2))>;

// CHECK:      GIM_CheckNumOperands, /*MI*/0, /*Expected*/3,
// CHECK-NEXT:       GIM_CheckOpcode, /*MI*/0, GIMT_Encode2(TargetOpcode::G_ASHR),
// CHECK-NEXT:       // MIs[0] DstI[dst]
// CHECK-NEXT:       GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
// CHECK-NEXT:       GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/GIMT_Encode2(MyTarget::GPR32RegClassID),
// CHECK-NEXT:       // MIs[0] rs1
// CHECK-NEXT:       GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
// CHECK-NEXT:       GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/GIMT_Encode2(MyTarget::GPR32RegClassID),
// CHECK-NEXT:       // MIs[0] rs2
// CHECK-NEXT:       GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
// CHECK-NEXT:       GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/GIMT_Encode2(MyTarget::GPR32RegClassID),
// CHECK-NEXT:       // (sra:{ *:[i32] } GPR32:{ *:[i32] }:$rs1, GPR32:{ *:[i32] }:$rs2)  =>  (ASHL:{ *:[i32] } GPR32:{ *:[i32] }:$rs1, (NEG2:{ *:[i32] }:{ *:[i8] } GPR32:{ *:[i32] }:$rs2))
// CHECK-NEXT:       GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
// CHECK-NEXT:       GIR_BuildMI, /*InsnID*/1, /*Opcode*/GIMT_Encode2(MyTarget::NEG2),
// CHECK-NEXT:       GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/GIMT_Encode2(RegState::Define),
// CHECK-NEXT:       GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/2, // rs2
// CHECK-NEXT:       GIR_SetImplicitDefDead, /*InsnID*/1, /*OpIdx for MyTarget::B0*/0,
// CHECK-NEXT:       GIR_SetImplicitDefDead, /*InsnID*/1, /*OpIdx for MyTarget::F0*/1,
// CHECK-NEXT:       GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
// CHECK-NEXT:       GIR_BuildMI, /*InsnID*/0, /*Opcode*/GIMT_Encode2(MyTarget::ASHL),
// CHECK-NEXT:       GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // DstI[dst]
// CHECK-NEXT:       GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs1
// CHECK-NEXT:       GIR_AddSimpleTempRegister, /*InsnID*/0, /*TempRegID*/0,
// CHECK-NEXT:       GIR_EraseFromParent, /*InsnID*/0,

// CHECK:      GIM_CheckNumOperands, /*MI*/0, /*Expected*/3,
// CHECK-NEXT:       GIM_CheckOpcode, /*MI*/0, GIMT_Encode2(TargetOpcode::G_LSHR),
// CHECK-NEXT:       // MIs[0] DstI[dst]
// CHECK-NEXT:       GIM_CheckType, /*MI*/0, /*Op*/0, /*Type*/GILLT_s32,
// CHECK-NEXT:       GIM_CheckRegBankForClass, /*MI*/0, /*Op*/0, /*RC*/GIMT_Encode2(MyTarget::GPR32RegClassID),
// CHECK-NEXT:       // MIs[0] rs1
// CHECK-NEXT:       GIM_CheckType, /*MI*/0, /*Op*/1, /*Type*/GILLT_s32,
// CHECK-NEXT:       GIM_CheckRegBankForClass, /*MI*/0, /*Op*/1, /*RC*/GIMT_Encode2(MyTarget::GPR32RegClassID),
// CHECK-NEXT:       // MIs[0] rs2
// CHECK-NEXT:       GIM_CheckType, /*MI*/0, /*Op*/2, /*Type*/GILLT_s32,
// CHECK-NEXT:       GIM_CheckRegBankForClass, /*MI*/0, /*Op*/2, /*RC*/GIMT_Encode2(MyTarget::GPR32RegClassID),
// CHECK-NEXT:       // (srl:{ *:[i32] } GPR32:{ *:[i32] }:$rs1, GPR32:{ *:[i32] }:$rs2)  =>  (LSHL:{ *:[i32] } GPR32:{ *:[i32] }:$rs1, (NEG:{ *:[i32] }:{ *:[i8] } GPR32:{ *:[i32] }:$rs2))
// CHECK-NEXT:       GIR_MakeTempReg, /*TempRegID*/0, /*TypeID*/GILLT_s32,
// CHECK-NEXT:       GIR_BuildMI, /*InsnID*/1, /*Opcode*/GIMT_Encode2(MyTarget::NEG),
// CHECK-NEXT:       GIR_AddTempRegister, /*InsnID*/1, /*TempRegID*/0, /*TempRegFlags*/GIMT_Encode2(RegState::Define),
// CHECK-NEXT:       GIR_Copy, /*NewInsnID*/1, /*OldInsnID*/0, /*OpIdx*/2, // rs2
// CHECK-NEXT:       GIR_SetImplicitDefDead, /*InsnID*/1, /*OpIdx for MyTarget::B0*/0,
// CHECK-NEXT:       GIR_ConstrainSelectedInstOperands, /*InsnID*/1,
// CHECK-NEXT:       GIR_BuildMI, /*InsnID*/0, /*Opcode*/GIMT_Encode2(MyTarget::LSHL),
// CHECK-NEXT:       GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/0, // DstI[dst]
// CHECK-NEXT:       GIR_Copy, /*NewInsnID*/0, /*OldInsnID*/0, /*OpIdx*/1, // rs1
// CHECK-NEXT:       GIR_AddSimpleTempRegister, /*InsnID*/0, /*TempRegID*/0,
// CHECK-NEXT:       GIR_EraseFromParent, /*InsnID*/0,
