// Common target boilerplate for CodeGenFormat tests.
include "llvm/Target/Target.td"
include "llvm/Target/CodeGenFormat.td"

def TESTInstrInfo : InstrInfo;

def TEST : Target {
  let InstructionSet = TESTInstrInfo;
}

def TESTCodeGenFormat : CodeGenFormat {
  let FormatClassEmitted = "";
}