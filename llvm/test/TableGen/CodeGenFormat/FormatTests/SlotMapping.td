// RUN: llvm-tblgen --gen-instr-format -I %p -I %p/../ -I %p/../../../../include %s 2>&1 | FileCheck %s
include "llvm/Target/Target.td"
include "llvm/Target/CodeGenFormat.td"

let Namespace = "TEST" in {
def alu_slot  : InstSlot<"aluaaa", 19> {
  let FieldToFind = "alu_base";
}
def lng_slot     : InstSlot<"ddddd", 28>;
def veca_slot : InstSlot<"veca", 37>;
}

include "Inputs/CommonFormatTesting.td"

// CHECK:        GET_ALTERNATE_INST_OPCODE_FUNC
// CHECK:        static std::vector<unsigned int> const AlternateInsts[] = {
// CHECK:        TEST::MultiSlotPseudo
// CHECK:        { TEST::ADD, TEST::VMUL80_S32_S32_BM }

// CHECK:        const std::vector<unsigned int> *TESTMCFormats::getAlternateInstsOpcode(unsigned int Opcode) const {
// CHECK:        case TEST::MultiSlotPseudo:
// CHECK:        return &AlternateInsts[0];

def ADD      : TEST_alu_r_rr<0b0000, "add">;

// CHECK:        TEST::ADD - Index : 1
// CHECK-NOT:    TESTSlotKind::TEST_SLOT_ALUAAA
// CHECK:        "alu_base"
// CHECK:        { 1, 19 }
// CHECK:        TESTSlotKind::TEST_SLOT_ALUAAA
// CHECK-NOT:    TESTSlotKind::TEST_SLOT_VECA

def VMUL80_S32_S32_BM : TEST_vec_acm_bm_zero
    <0b1, 0b0, 0b01, 0b0, 0b01, (outs), (ins), [], "", "">;

// CHECK:        TEST::VMUL80_S32_S32_BM - Index : 2
// CHECK-NOT:    TESTSlotKind::TEST_SLOT_VECA
// CHECK:        "veca_base"
// CHECK:        { 21, 57 }
// CHECK:        TESTSlotKind::TEST_SLOT_VECA
// CHECK-NOT:    TESTSlotKind::TEST_SLOT_VECA

def MultiSlotPseudo : TESTMultiSlot_Pseudo<[ADD,VMUL80_S32_S32_BM]>;

// CHECK:        TEST::MultiSlotPseudo - Index : 0
// CHECK:        false /* isComposite */
// CHECK:        true /* hasMultipleSlotOptions */
// CHECK:        TEST::ADD - Index : 1
// CHECK:        false /* isComposite */
// CHECK:        false /* hasMultipleSlotOptions */
// CHECK:        TEST::VMUL80_S32_S32_BM - Index : 2
// CHECK:        false /* isComposite */
// CHECK:        false /* hasMultipleSlotOptions */
