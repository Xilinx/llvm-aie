// Testing that the Backend behaves correctly when no default slot is defined
// and when another slot already took the name of the "reserved" default one.

// RUN: not llvm-tblgen --gen-instr-format -I %p -I %p/../ -I %p/../../../../include %s 2>&1 | FileCheck %s

include "Inputs/CommonSlotTesting.td"

let Namespace = "TEST" in
{
  def fu0_slot        : InstSlot<"fu0",  12>;
  // The malicious name "thief".
  // He is using the same slotName ("unknown") that the implicit slot is going
  // to use.
  def unknown_stealer : InstSlot<"unknown",  42>;

// Let's check that this TableGen program crash.
// CHECK: Valid slot (unknown_stealer) already has "unknown" as SlotName: put this one as default or write explicitely another default slot record.
}