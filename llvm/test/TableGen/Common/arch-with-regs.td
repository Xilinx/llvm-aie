include "llvm/Target/Target.td"

def archInstrInfo : InstrInfo { }

def arch : Target {
  let InstructionSet = archInstrInfo;
}

class ArchRegister<bits<4> Enc, string n> : Register<n> {
  let HWEncoding{3-0} = Enc;
}
def r0 : ArchRegister<0b0000, "r0">;
def r1 : ArchRegister<0b0001, "r1">;

class ArchRegisterClass <dag reglist> :
               RegisterClass<"arch", [i32], /*alignment*/32, reglist> {
}

class TestInstruction : Instruction {
  let Size = 1;
  let OutOperandList = (outs);
  let InOperandList = (ins);
  field bits<8> Inst;
  field bits<8> SoftFail = 0;
}
