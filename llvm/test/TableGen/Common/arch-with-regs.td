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

// Arch1 Instruction
class Arch1TestInstruction : Instruction {
  let Size = 4;
  let OutOperandList = (outs);
  let InOperandList = (ins);
  field bits<32> Inst;
  field bits<32> SoftFail = 0;
}

// Arch1 register classes
class Arch1RegisterClass4Bits <dag reglist> :
               RegisterClass<"arch1", [], /*alignment*/32, reglist> {
}
class Arch1RegisterClass2Bits <dag reglist> :
               RegisterClass<"arch1", [], /*alignment*/32, reglist> {
}

// Arch1 registers
class Arch1Reg4Bits<bits<4> Enc, string n> : Register<n> {
  let HWEncoding{3-0} = Enc;
}
class Arch1Reg2Bits<bits<2> Enc, string n> : Register<n> {
  let HWEncoding{1-0} = Enc;
}
def a0 : Arch1Reg4Bits<0b0000, "a0">;
def b0 : Arch1Reg4Bits<0b0000, "b0">;
def c0 : Arch1Reg2Bits<0b00, "c0">;
def d0 : Arch1Reg2Bits<0b00, "d0">;
