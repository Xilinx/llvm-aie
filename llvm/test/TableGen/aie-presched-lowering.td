// RUN: llvm-tblgen --gen-aie-presched-lowering -I %p/../../include -I %p/../../lib/Target/AIE %s | FileCheck %s

// Verify how TableGen uses PreSchedInstExpansion to generate
// getPseudoBranchExpandInfo().

include "llvm/Target/Target.td"
include "AIEBaseInstrInfo.td"

def TestAIETargetInstrInfo : InstrInfo;

def TestAIE : Target {
  let InstructionSet = TestAIETargetInstrInfo;
}

def REG : Register<"REG">;
def GPR : RegisterClass<"TestAIE", [i32], 32, (add REG)>;

class TestInstruction : Instruction {
  let Namespace = "TestAIE";
}
class Pseudo<dag outs, dag ins> : TestInstruction {
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let isPseudo = 1;
}

def TARGET_INSTR : TestInstruction {
  let OutOperandList = (outs GPR:$rd);
  let InOperandList = (ins i32imm:$val);
}
def BARRIER_INSTR : TestInstruction {
  let OutOperandList = (outs);
  let InOperandList = (ins);
}

def PSEUDO : Pseudo<(outs GPR:$rd), (ins)>,
             PreSchedInstExpansion<TARGET_INSTR, BARRIER_INSTR>;

// CHECK: std::optional<AIEBaseInstrInfo::PseudoBranchExpandInfo>
// CHECK: TestAIEInstrInfo::getPseudoBranchExpandInfo(const MachineInstr &MI) const {
// CHECK:   switch (MI.getOpcode()) {
// CHECK:   default: return {};
// CHECK:   case TestAIE::PSEUDO:
// CHECK:     return PseudoBranchExpandInfo{TestAIE::TARGET_INSTR, TestAIE::BARRIER_INSTR};
// CHECK:   }
// CHECK: }
