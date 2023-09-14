// RUN: llvm-tblgen --gen-aie-memory-cycles -I %p/../../include -I %p/Common %s | FileCheck %s

// Verify TableGen generates valid getFirstMemoryCycle() and
// getLastMemoryCycle() in absence of MemInstrItinData record.

include "aie-arch-with-regs.td"

def II_OTHER : InstrItinClass;
def AIEItineraries : ProcessorItineraries<[], [], [
  InstrItinData<II_OTHER, /*stages*/[], [1,1]>
]>;

def AIESchedModel : SchedMachineModel {
  let Itineraries = AIEItineraries;
  let CompleteModel = 0;
}

def : ProcessorModel<"TestAIE", AIESchedModel, []>;

def FOO : TestInstruction {
  let OutOperandList = (outs );
  let InOperandList = (ins GPR:$val, i32imm:$adr);
  let Itinerary = II_OTHER;
}

// CHECK:      std::optional<unsigned>
// CHECK-NEXT: TestAIEInstrInfo::getFirstMemoryCycle(unsigned SchedClass) const {
// CHECK-NEXT:   switch (SchedClass) {
// CHECK-NEXT:   default: return {};
// CHECK-NEXT:   }
// CHECK-NEXT: }

// CHECK:      std::optional<unsigned>
// CHECK-NEXT: TestAIEInstrInfo::getLastMemoryCycle(unsigned SchedClass) const {
// CHECK-NEXT:   switch (SchedClass) {
// CHECK-NEXT:   default: return {};
// CHECK-NEXT:   }
// CHECK-NEXT: }
