// Testing if the default slot(s) are behaving correctly.
// The size of a given slot must be > 0 if the slot is valid and >= 0 if it's
// a default one.

// RUN: llvm-tblgen --gen-instr-format -I %p -I %p/../../../ -I %p/../../../../../../include %s 2>&1 | FileCheck %s

include "Inputs/CommonSlotTesting.td"

let Namespace = "TEST" in
{
  def fu2_slot     : InstSlot<"fu2", 0, true>;
  // CHECK-NOT: assertion failed
  // CHECK-NOT:  Invalid size of the slot fu2_slot
}