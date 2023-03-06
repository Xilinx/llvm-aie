// RUN: llvm-tblgen --gen-aie-presched-lowering -I %p/../../include -I %p/../../lib/Target/AIE %s | FileCheck %s

// Verify TableGen generates a valid getPseudoBranchExpandInfo()
// without any PreSchedInstExpansion records.

include "llvm/Target/Target.td"
include "AIEBaseInstrInfo.td"

def TestAIETargetInstrInfo : InstrInfo;

def TestAIE : Target {
  let InstructionSet = TestAIETargetInstrInfo;
}

class TestInstruction : Instruction {
  let Namespace = "TestAIE";
}
def TEST_INSTR : TestInstruction {
  let OutOperandList = (outs);
  let InOperandList = (ins);
}

// CHECK: std::optional<AIEBaseInstrInfo::PseudoBranchExpandInfo>
// CHECK: TestAIEInstrInfo::getPseudoBranchExpandInfo(const MachineInstr &MI) const {
// CHECK:   return {};
// CHECK: }
