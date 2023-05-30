// Common target boilerplate for CodeGenFormat tests.
def TESTInstrInfo : InstrInfo;

def TEST : Target {
  let InstructionSet = TESTInstrInfo;
}

def TESTCodeGenFormat : CodeGenFormat {
  let FormatClassEmitted = "";
}

let Namespace = "TEST" in
def default_slot : InstSlot<"default", 0, true>;

class TESTInst<dag outs, dag ins, string opcodestr, string argstr,
               list<dag> pattern>
    : Instruction, InstFormat
{
  let Namespace = "TEST";

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = opcodestr # "\t" # argstr;
  let Pattern = pattern;

  let Slot = default_slot;
}

class TESTSubInst<dag outs, dag ins, string opcodestr, string argstr,
                  list<dag> pattern>
    : TESTInst<outs, ins, opcodestr, argstr, pattern>
{
  let isComposite = false;
}

class TESTPacketFormat<dag outs, dag ins, string opcodestr, string argstr,
                       list<dag> pattern>
    : TESTInst<outs, ins, opcodestr, argstr, pattern>
{
  let isComposite = true;
}

class TESTMultiSlot_Pseudo<list<TESTInst> insts = [], string opcodestr = "",
             string argstr = "">
    : TESTInst<(outs),(ins), opcodestr, argstr, []>{
  let isPseudo = 1;
  bits<1> isMultiSlotPseudo = 1;
  list<TESTInst> materializableInto = insts;
  let isCodeGenOnly = 1;
  let isComposite = 0;
}

class TEST_instr32<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : TESTSubInst<outs, ins, opcodestr, argstr, pattern> {
  field bits<32> SoftFail = 0;
  field bits<32> Inst;
  field bits<29> instr32;
  let Inst = {instr32, 0b011};
}

class TEST_i32_shrt<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : TEST_instr32<outs, ins, pattern, opcodestr, argstr> {
   field bits<21> i32_shrt;
   let instr32 = {i32_shrt, 0b00000000};
}

class TEST_lng<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : TEST_instr32<outs, ins, pattern, opcodestr, argstr> {
  field bits<28> lng_base;
  let instr32 = {lng_base, 0b1};
  let Slot = lng_slot;
}

class TEST_mov20<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : TEST_lng<outs, ins, pattern, opcodestr, argstr> {
  bits<20> imm;
  bits<6> reg;

  field bits<26> mov = {imm{19-14}, reg, imm{13-0}};
  let lng_base = {0b01, mov};
}

class TEST_alu_format<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : TEST_i32_shrt<outs, ins, pattern, opcodestr, argstr> {
   field bits<19> alu_base;
   let i32_shrt = {0b0, alu_base, 0b0};
   let Slot = alu_slot;
}

class TEST_alu_r_rr<bits<4> opcode, string opcodestr = "", string argstr = "$d0, $s0, $s1">
    : TEST_alu_format<(outs), (ins), [], opcodestr, argstr> {
   bits<4> ALU_opcode = opcode;
   bits<4> s1;
   bits<4> s0;
   bits<4> d0;
   let hasSideEffects = 0;
	let mayLoad = 0;
	let mayStore = 0;
   let alu_base = {0b011, opcode, s1, s0, d0};
}

class TEST_instr64<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : TESTSubInst<outs, ins, opcodestr, argstr, pattern> {
  field bits<64> SoftFail = 0;
  field bits<64> Inst;
  field bits<60> instr64;
  let Size = 8; // 8 Byte instruction
  let Inst = {instr64, 0b0111};
}

class TEST_veca_base<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : TEST_instr64<outs, ins, pattern, opcodestr, argstr> {
  field bits<37> veca_base;
  field bits<21> i32_shrt = 0;
  let instr64 = {i32_shrt, veca_base, 0b00};
  let Size = 8; // 8 Byte instruction
  let hasSideEffects = 0;
  let mayLoad = 0;
  let mayStore = 0;
  let Slot = veca_slot;
}

class TEST_vec_bm<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : TEST_veca_base<outs, ins, pattern, opcodestr, argstr> {
  field bits<36> vec_bm;
  let veca_base = {0b1, vec_bm};
}

class TEST_vec_acm_bm_all<
  bits<1> osz, /* 48 or 80 bits? */
  bits<1> xcx, /* x complex? */
  bits<2> xsz, /* i16, i32, u8, i8 */
  bits<1> zcx, /* z complex? */
  bits<2> zsz, /* i16, i32, u8, i8 */
  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : TEST_vec_bm<outs, ins, pattern, opcodestr, argstr> {
  field bits<3> acm_bm_all_0;
  field bits<5> acm_bm_all_1;

  bits<4> xstart; // GPR
  bits<3> xoffs; // mC
  bits<4> zstart; // t04u
  bits<3> zoffs; // mC
  bits<3> ystart; // GPR0_7
  bits<3> conf; // mC
  bits<1> ipm; // wc0 or wc1
  let vec_bm = {acm_bm_all_1, zcx, zsz, zstart, osz, xcx, xsz, xstart, ystart, xoffs, zoffs, conf, ipm, acm_bm_all_0};
}

class TEST_vec_acm_bm_zero<
  bits<1> osz, /* 48 or 80 bits? */
  bits<1> xcx, /* x complex? */
  bits<2> xsz, /* i16, i32, u8, i8 */
  bits<1> zcx, /* z complex? */
  bits<2> zsz, /* i16, i32, u8, i8 */
  dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : TEST_vec_acm_bm_all<osz, xcx, xsz, zcx, zsz, outs, ins, pattern, opcodestr, argstr> {
  bits<1> pmx; /* ya or yd */
  bits<2> sd; // mBM
  bits<3> acm_bm_all_0 = {pmx, sd};
  bits<5> acm_bm_all_1 = {0b0, 0b1011};
}

class TEST_instr16<dag ins, string opcodestr, string argstr>
    : TESTPacketFormat<(outs), ins, opcodestr, argstr, []> {
  field bits<16> SoftFail = 0;
  field bits<16> Inst;
  bits<14> instr16;
  let Size = 2;
}

class TEST_instr64_Packet<dag ins, string opcodestr, string argstr> :
      TESTPacketFormat<(outs), ins, opcodestr, argstr, []> {
  field bits<64> SoftFail = 0;
  field bits<64> Inst;
  field bits<60> instr64;
  let Size = 8; // 8 Byte instruction
  let Inst = {instr64, 0b0111};
}

class TEST_i64_novec<dag ins, string opcodestr, string argstr>
    : TEST_instr64_Packet<ins, opcodestr, argstr> {
   field bits<59> i64_novec;
   let instr64 = {i64_novec, 0b1};
}

class TEST_i64_lda<dag ins, string opcodestr, string argstr>
    : TEST_i64_novec<ins, opcodestr, argstr> {
   bits<18> lda;
   field bits<41> i64_lda_fmts;
   let i64_novec = {lda, i64_lda_fmts};
}

class TEST_i64_lda_fmts_i64_ldb<dag ins, string opcodestr, string argstr>
    : TEST_i64_lda<ins, opcodestr, argstr> {
  bits<40> i64_ldb;
  let i64_lda_fmts = {i64_ldb, 0b0};
}

class TEST_instr128<dag outs, dag ins, list<dag> pattern, string opcodestr = "", string argstr = "">
    : TESTPacketFormat<outs, ins, opcodestr, argstr, pattern> {
  field bits<128> SoftFail = 0;
  field bits<128> Inst;
  field bits<127> instr128;
  let Size = 16;
  let Inst = {instr128, 0b0};
}

class TEST__instr128__V_formats<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : TEST_instr128<outs, ins, pattern, opcodestr, argstr>
{
  bits<18> lda;
  field bits<109> V_formats;

  let instr128 = {lda, V_formats};
}

class TEST__instr128__V_formats__V_move<dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr>
    : TEST__instr128__V_formats<outs, ins, pattern, opcodestr, argstr>
{
  field bits<108> V_move;
  let V_formats = {V_move, 0b0};
}

class TEST__instr128__V_formats__V_move__ldb_alu_st_mv_all_vec_all<
    dag outs, dag ins, list<dag> pattern, string opcodestr, string argstr
  >: TEST__instr128__V_formats__V_move<outs, ins, pattern, opcodestr, argstr>
{
  bits<17> ldb;
  field bits<21> alu_st;
  field bits<31> mv_all;
  bits<37> vec_all;

  field bits<108> V_move;

  let V_move = {ldb, alu_st, 0b0, mv_all, 0b1, vec_all};
}

class TESTRegisterClass <int size, list<ValueType> regTypes, dag reglist, RegAltNameIndex idx = NoRegAltName> :
		RegisterClass<"TEST", regTypes, /*alignment*/size, reglist, idx> {
  dag Regs = reglist;
  let RegInfos = RegInfoByHwMode<
      [DefaultMode],
      [RegInfo</*size*/size, /*spill size*/size, /*spill alignment*/size>]>;
}

class TESTScalarRegisterClass <dag reglist, RegAltNameIndex idx = NoRegAltName> :
		TESTRegisterClass<32, [i32], reglist, idx>;

def GPR : TESTScalarRegisterClass<(add (sequence "r%u", 10, 15))>;
def mMv0Cg20 : TESTScalarRegisterClass<(add GPR)>;
def uimm20   : Operand<i32>;
