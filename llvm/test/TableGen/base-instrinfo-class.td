// RUN: llvm-tblgen -gen-instr-info -I %p/../../include -I %p/Common %s \
// RUN:     | FileCheck %s --check-prefix=DEFAULT
// RUN: llvm-tblgen -gen-instr-info -I %p/../../include -I %p/Common %s -base-instrinfo-class BaseInstrInfoClass \
// RUN:     | FileCheck %s --check-prefix=BASE

// Verify the -base-instrinfo-class option

include "llvm/Target/Target.td"

def TestInstrInfo : InstrInfo { }
def TestArch : Target {
  let InstructionSet = TestInstrInfo;
}

def Reg : Register<"reg">;
def RegClass : RegisterClass<"foo", [i32], 0, (add Reg)>;

def Op : Operand<i32>;

def InstA : Instruction {
  let Size = 1;
  let OutOperandList = (outs Op:$a);
  let InOperandList = (ins Op:$b);
  field bits<8> Inst;
  field bits<8> SoftFail = 0;
  let Namespace = "MyNamespace";
}

// DEFAULT: struct TestArchGenInstrInfo : public TargetInstrInfo
// DEFAULT: TestArchGenInstrInfo::TestArchGenInstrInfo(
// DEFAULT:  : TargetInstrInfo(

// BASE: struct TestArchGenInstrInfo : public BaseInstrInfoClass
// BASE: TestArchGenInstrInfo::TestArchGenInstrInfo(
// BASE:  : BaseInstrInfoClass(
