//===- AIE2GenInstrInfo.td ---------------------------------*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

let hasSideEffects = false, mayLoad = false, mayStore = false in {
let Itinerary = II_ADD in {
  let Defs = [srCarry] in
  def ADD_add_r_ri : AIE2_add_r_ri_inst_alu<(outs eR:$mRx), (ins eR:$mRx0, simm7:$c7s), "add", "$mRx, $mRx0, $c7s">;
}
let Itinerary = II_ADD_NC in {
  let ItineraryRegPairs = [ItinRegClassPair<II_ADD_NC_RS,[OperandRegClass<0, eR>]>,
                           ItinRegClassPair<II_ADD_NC_P,[OperandRegClass<0, eP>]>, 
                           ItinRegClassPair<II_ADD_NC_M,[OperandRegClass<0, eM>]>, 
                           ItinRegClassPair<II_ADD_NC_RS,[OperandRegClass<0, eS>]>, 
                           ItinRegClassPair<II_ADD_NC_DC,[OperandRegClass<0, eDC>]>,
                           ItinRegClassPair<II_ADD_NC_DJ,[OperandRegClass<0, eDJ>]>, 
                           ItinRegClassPair<II_ADD_NC_DN,[OperandRegClass<0, eDN>]>] in {
    def ADD_NC : AIE2_mv_add_inst_mv<(outs OP_mMvSclDst:$dst), (ins eR:$s0, simm6:$imm), "add.nc", "$dst, $s0, $imm">;
  }
}
let Itinerary = II_ABS in {
  let Defs = [srCarry] in
  def ABS : AIE2_alu_r_r_inst_alu<0b1000, (outs eR:$mRx), (ins eR:$mRx0), "abs", "$mRx, $mRx0">;
}
let Itinerary = II_CLB in {
  def CLB : AIE2_alu_r_r_inst_alu<0b0000, (outs eR:$mRx), (ins eR:$mRx0), "clb", "$mRx, $mRx0">;
}
let Itinerary = II_DIVS, Constraints = "$sd_out = $sd_in" in {
  def DIVS : AIE2_alu_dstep_inst_alu<(outs eR:$d0, eR31:$sd_out) , (ins eR31:$sd_in, eR:$s0, eR:$s1), "divs", "$d0, r31, $s0, $s1">;
}
let Itinerary = II_CLZ in {
  def CLZ : AIE2_alu_r_r_inst_alu<0b0001, (outs eR:$mRx), (ins eR:$mRx0), "clz", "$mRx, $mRx0">;
}
let Itinerary = II_EQZ in {
  def EQZ : AIE2_alu_r_r_inst_alu<0b0110, (outs eR:$mRx), (ins eR:$mRx0), "eqz", "$mRx, $mRx0">;
}
let Itinerary = II_NEZ in {
  def NEZ : AIE2_alu_r_r_inst_alu<0b0111, (outs eR:$mRx), (ins eR:$mRx0), "nez", "$mRx, $mRx0">;
}

let Itinerary = II_EXTENDs8 in {
  def EXTENDs8 : AIE2_alu_r_r_inst_alu<0b0010, (outs eR:$mRx), (ins eR:$mRx0), "extend.s8", "$mRx, $mRx0">;
}
let Itinerary = II_EXTENDs16 in {
  def EXTENDs16 : AIE2_alu_r_r_inst_alu<0b0011, (outs eR:$mRx), (ins eR:$mRx0), "extend.s16", "$mRx, $mRx0">;
}
let Itinerary = II_EXTENDu8 in {
  def EXTENDu8 : AIE2_alu_r_r_inst_alu<0b0100, (outs eR:$mRx), (ins eR:$mRx0), "extend.u8", "$mRx, $mRx0">;
}
let Itinerary = II_EXTENDu16 in {
  def EXTENDu16 : AIE2_alu_r_r_inst_alu<0b0101, (outs eR:$mRx), (ins eR:$mRx0), "extend.u16", "$mRx, $mRx0">;
}

let Itinerary = II_ADC in {
  let Defs = [srCarry], Uses = [srCarry] in
  def ADC : AIE2_alu_r_rr_inst_alu<0b0010, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "adc", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_ADD in {
  let Defs = [srCarry] in
  def ADD : AIE2_alu_r_rr_inst_alu<0b0000, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "add", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_AND in {
  def AND : AIE2_alu_r_rr_inst_alu<0b0100, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "and", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_ASHL in {
  def ASHL : AIE2_alu_r_rr_inst_alu<0b1110, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "ashl", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_EQ in {
  def EQ : AIE2_alu_r_rr_inst_alu<0b0111, (outs eR:$mRx), (ins eR:$mRx0,  eR:$mRy), "eq", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_GE in {
  def GE : AIE2_alu_r_rr_inst_alu<0b1001, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "ge", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_GEU in {
  def GEU : AIE2_alu_r_rr_inst_alu<0b1011, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "geu", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_LSHL in {
  def LSHL : AIE2_alu_r_rr_inst_alu<0b1101, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "lshl", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_LT in {
  def LT : AIE2_alu_r_rr_inst_alu<0b1010, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "lt", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_LTU in {
  def LTU : AIE2_alu_r_rr_inst_alu<0b1100, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "ltu", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_NE in {
  def NE : AIE2_alu_r_rr_inst_alu<0b1000, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "ne", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_OR in {
  def OR : AIE2_alu_r_rr_inst_alu<0b0101, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "or", "$mRx, $mRx0, $mRy">;
  let isCodeGenOnly = 1 in 
    def MOV_OR : AIE2_alu_r_rr_inst_alu_mov_gpr<0b0101, (outs eR:$mRx), (ins eR:$mRx0), "or", "$mRx, $mRx0, $mRx0">;
}
let Itinerary = II_SBC in {
  let Defs = [srCarry], Uses = [srCarry] in
  def SBC : AIE2_alu_r_rr_inst_alu<0b0011, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "sbc", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_SUB in {
  let Defs = [srCarry] in
  def SUB : AIE2_alu_r_rr_inst_alu<0b0001, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "sub", "$mRx, $mRx0, $mRy">;
}
let Itinerary = II_XOR in {
  def XOR : AIE2_alu_r_rr_inst_alu<0b0110, (outs eR:$mRx), (ins eR:$mRx0, eR:$mRy), "xor", "$mRx, $mRx0, $mRy">;
}
let  mayLoad = true, mayStore = false in {
  // Part-word load writes only to R type register
  let Itinerary = II_LDA_R in {
    def LDA_S8_ag_idx_imm : AIE2_dmhb_lda_inst_lda<0b00, (outs eR:$mRa) , (ins  eP:$ptr, simm3:$imm  ), "lda.s8", "$mRa, [$ptr, $imm]">,
      AIE2_ag_nospill__ag_idx_imm;
    def LDA_S8_ag_idx : AIE2_dmhb_lda_inst_lda<0b00, (outs eR:$mRa) , (ins  eP:$ptr, eDJ:$dj  ), "lda.s8", "$mRa, [$ptr, $dj]">,
      AIE2_ag_nospill__ag_idx;

    def LDA_U8_ag_idx_imm : AIE2_dmhb_lda_inst_lda<0b01, (outs eR:$mRa) , (ins  eP:$ptr, simm3:$imm  ), "lda.u8", "$mRa, [$ptr, $imm]">,
      AIE2_ag_nospill__ag_idx_imm;
    def LDA_U8_ag_idx : AIE2_dmhb_lda_inst_lda<0b01, (outs eR:$mRa) , (ins  eP:$ptr, eDJ:$dj  ), "lda.u8", "$mRa, [$ptr, $dj]">,
      AIE2_ag_nospill__ag_idx;

    def LDA_S16_ag_idx_imm : AIE2_dmhb_lda_inst_lda<0b10, (outs eR:$mRa) , (ins  eP:$ptr, simm3:$imm  ), "lda.s16", "$mRa, [$ptr, $imm]">,
      AIE2_ag_nospill__ag_idx_imm;
    def LDA_S16_ag_idx : AIE2_dmhb_lda_inst_lda<0b10, (outs eR:$mRa) , (ins  eP:$ptr, eDJ:$dj  ), "lda.s16", "$mRa, [$ptr, $dj]">,
      AIE2_ag_nospill__ag_idx;

    def LDA_U16_ag_idx_imm : AIE2_dmhb_lda_inst_lda<0b11, (outs eR:$mRa) , (ins  eP:$ptr, simm3:$imm  ), "lda.u16", "$mRa, [$ptr, $imm]">,
      AIE2_ag_nospill__ag_idx_imm;
    def LDA_U16_ag_idx : AIE2_dmhb_lda_inst_lda<0b11, (outs eR:$mRa) , (ins  eP:$ptr, eDJ:$dj  ), "lda.u16", "$mRa, [$ptr, $dj]">,
      AIE2_ag_nospill__ag_idx;
  }

  //Part-word load writes only to R type register
  let Itinerary = II_LDA_POST_1D_R, Constraints = "$ptr_out = $ptr" in {
    def LDA_S8_ag_pstm_nrm_imm : AIE2_dmhb_lda_inst_lda<0b00, (outs eR:$mRa, eP:$ptr_out) , (ins  eP:$ptr, simm4:$imm  ), "lda.s8", "$mRa, [$ptr], $imm">,
      AIE2_ag_nospill__ag_pstm_nrm_imm;
    def LDA_S8_ag_pstm_nrm : AIE2_dmhb_lda_inst_lda<0b00, (outs eR:$mRa, eP:$ptr_out) , (ins  eP:$ptr, eM:$mod  ), "lda.s8", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_nrm;

    def LDA_U8_ag_pstm_nrm_imm : AIE2_dmhb_lda_inst_lda<0b01, (outs eR:$mRa, eP:$ptr_out) , (ins  eP:$ptr, simm4:$imm  ), "lda.u8", "$mRa, [$ptr], $imm">,
      AIE2_ag_nospill__ag_pstm_nrm_imm;
    def LDA_U8_ag_pstm_nrm : AIE2_dmhb_lda_inst_lda<0b01, (outs eR:$mRa, eP:$ptr_out) , (ins  eP:$ptr, eM:$mod  ), "lda.u8", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_nrm;

    def LDA_S16_ag_pstm_nrm_imm : AIE2_dmhb_lda_inst_lda<0b10, (outs eR:$mRa, eP:$ptr_out) , (ins  eP:$ptr, simm4:$imm  ), "lda.s16", "$mRa, [$ptr], $imm">,
      AIE2_ag_nospill__ag_pstm_nrm_imm;
    def LDA_S16_ag_pstm_nrm : AIE2_dmhb_lda_inst_lda<0b10, (outs eR:$mRa, eP:$ptr_out) , (ins  eP:$ptr, eM:$mod  ), "lda.s16", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_nrm;

    def LDA_U16_ag_pstm_nrm_imm : AIE2_dmhb_lda_inst_lda<0b11, (outs eR:$mRa, eP:$ptr_out) , (ins  eP:$ptr, simm4:$imm  ), "lda.u16", "$mRa, [$ptr], $imm">,
      AIE2_ag_nospill__ag_pstm_nrm_imm;
    def LDA_U16_ag_pstm_nrm : AIE2_dmhb_lda_inst_lda<0b11, (outs eR:$mRa, eP:$ptr_out) , (ins  eP:$ptr, eM:$mod  ), "lda.u16", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_nrm;
  }
}

// Note that this is a store in an lda slot. Assuming these are read-modify-write.
let mayLoad = true, mayStore = true in {
  let Itinerary = II_STHB in {
    def ST_S8_ag_idx_imm : AIE2_dmhb_sts_inst_lda<0b0, (outs ) , (ins eR:$mRv, eP:$ptr, simm3:$imm ), "st.s8", "$mRv, [$ptr, $imm]">,
      AIE2_ag_nospill__ag_idx_imm;
    def ST_S8_ag_idx : AIE2_dmhb_sts_inst_lda<0b0, (outs ) , (ins eR:$mRv, eP:$ptr, eDJ:$dj ), "st.s8", "$mRv, [$ptr, $dj]">,
      AIE2_ag_nospill__ag_idx;

    def ST_S16_ag_idx_imm : AIE2_dmhb_sts_inst_lda<0b1, (outs ) , (ins eR:$mRv, eP:$ptr, simm3:$imm ), "st.s16", "$mRv, [$ptr, $imm]">,
      AIE2_ag_nospill__ag_idx_imm;
    def ST_S16_ag_idx : AIE2_dmhb_sts_inst_lda<0b1, (outs ) , (ins eR:$mRv, eP:$ptr, eDJ:$dj ), "st.s16", "$mRv, [$ptr, $dj]">,
      AIE2_ag_nospill__ag_idx;
  }

  let Itinerary = II_STHB_POST_1D, Constraints = "$ptr_out = $ptr" in {
    def ST_S8_ag_pstm_nrm_imm : AIE2_dmhb_sts_inst_lda<0b0, (outs eP:$ptr_out ) , (ins eR:$mRv, eP:$ptr, simm4:$imm ), "st.s8", "$mRv, [$ptr], $imm">,
      AIE2_ag_nospill__ag_pstm_nrm_imm;
    def ST_S8_ag_pstm_nrm : AIE2_dmhb_sts_inst_lda<0b0, (outs eP:$ptr_out ) , (ins eR:$mRv, eP:$ptr, eM:$mod ), "st.s8", "$mRv, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_nrm;

    def ST_S16_ag_pstm_nrm_imm : AIE2_dmhb_sts_inst_lda<0b1, (outs eP:$ptr_out ) , (ins eR:$mRv, eP:$ptr, simm4:$imm ), "st.s16", "$mRv, [$ptr], $imm">,
      AIE2_ag_nospill__ag_pstm_nrm_imm;
    def ST_S16_ag_pstm_nrm : AIE2_dmhb_sts_inst_lda<0b1, (outs eP:$ptr_out ) , (ins eR:$mRv, eP:$ptr, eM:$mod ), "st.s16", "$mRv, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_nrm;
  }

  let Itinerary = II_STHB_2D, Constraints = "$ptr_out = $ptr" in {
    let DecoderMethod = "DecodeST_2D_HBInstruction" in {
      def ST_2D_S8 : AIE2_dmhb_sts_inst_lda<0b0,
        (outs eP:$ptr_out, eDC:$count_out ),
        (ins eR:$mRv, eP:$ptr, eD:$mod ), "st.2d.s8", "$mRv, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_2d;
      def ST_2D_S16 : AIE2_dmhb_sts_inst_lda<0b1,
        (outs eP:$ptr_out, eDC:$count_out ),
        (ins eR:$mRv, eP:$ptr, eD:$mod ), "st.2d.s16", "$mRv, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_2d;
    }
  }
  let Itinerary = II_STHB_3D, Constraints = "$ptr_out = $ptr" in {
    let DecoderMethod = "DecodeST_3D_HBInstruction" in {
      def ST_3D_S8 : AIE2_dmhb_sts_inst_lda<0b0,
        (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out ),
        (ins eR:$mRv, eP:$ptr, eDS:$mod ), "st.3d.s8", "$mRv, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_3d;
      def ST_3D_S16 : AIE2_dmhb_sts_inst_lda<0b1,
        (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out ),
        (ins eR:$mRv, eP:$ptr, eDS:$mod ), "st.3d.s16", "$mRv, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_3d;
    }
  }
}
let mayLoad = true, mayStore = false in {
  let Itinerary = II_LDA in {
  let ItineraryRegPairs = [ItinRegClassPair<II_LDA_R,[OperandRegClass<0, eR>]>,
                           ItinRegClassPair<II_LDA_P,[OperandRegClass<0, eP>]>,
                           ItinRegClassPair<II_LDA_M,[OperandRegClass<0, eM>]>,
                           ItinRegClassPair<II_LDA_DC,[OperandRegClass<0, eDC>]>,
                           ItinRegClassPair<II_LDA_DJ,[OperandRegClass<0, eDJ>]>,
                           ItinRegClassPair<II_LDA_DN,[OperandRegClass<0, eDN>]>] in {
    def LDA_dms_lda_idx : AIE2_dms_lda_inst_lda< (outs OP_mLdaScl:$mLdaScl) , (ins eP:$ptr, eDJ:$dj),
        "lda", "$mLdaScl, [$ptr, $dj]">, 
      AIE2_ag_all__ag_idx;
    def LDA_dms_lda_idx_imm : AIE2_dms_lda_inst_lda< (outs OP_mLdaScl:$mLdaScl) , (ins eP:$ptr, imm6x4:$imm),
        "lda", "$mLdaScl, [$ptr, $imm]">, 
      AIE2_ag_all__ag_idx_imm;
    }
  }
  let Itinerary = II_LDA_POST_1D, Constraints = "$ptr = $ptr_out" in {
    let ItineraryRegPairs = [ItinRegClassPair<II_LDA_POST_1D_R,[OperandRegClass<0, eR>]>,
                             ItinRegClassPair<II_LDA_POST_1D_P,[OperandRegClass<0, eP>]>, 
                             ItinRegClassPair<II_LDA_POST_1D_M,[OperandRegClass<0, eM>]>,
                             ItinRegClassPair<II_LDA_POST_1D_DC,[OperandRegClass<0, eDC>]>,
                             ItinRegClassPair<II_LDA_POST_1D_DJ,[OperandRegClass<0, eDJ>]>,
                             ItinRegClassPair<II_LDA_POST_1D_DN,[OperandRegClass<0, eDN>]>] in {
      def LDA_dms_lda_pstm_nrm : AIE2_dms_lda_inst_lda< (outs OP_mLdaScl:$mLdaScl, eP:$ptr_out ) , (ins eP:$ptr, eM:$mod),
          "lda", "$mLdaScl, [$ptr], $mod">,
        AIE2_ag_all__ag_pstm_nrm;
      def LDA_dms_lda_pstm_nrm_imm : AIE2_dms_lda_inst_lda< (outs OP_mLdaScl:$mLdaScl, eP:$ptr_out ) , (ins eP:$ptr, imm7x4:$imm),
          "lda", "$mLdaScl, [$ptr], $imm">,
        AIE2_ag_all__ag_pstm_nrm_imm;
    }
  }

  let Itinerary = II_LDA_POST_2D, Constraints = "$ptr = $ptr_out" in {
    let ItineraryRegPairs = [ItinRegClassPair<II_LDA_POST_2D_R,[OperandRegClass<0, eR>]>,
                             ItinRegClassPair<II_LDA_POST_2D_P,[OperandRegClass<0, eP>]>, 
                             ItinRegClassPair<II_LDA_POST_2D_M,[OperandRegClass<0, eM>]>,
                             ItinRegClassPair<II_LDA_POST_2D_DC,[OperandRegClass<0, eDC>]>,
                             ItinRegClassPair<II_LDA_POST_2D_DJ,[OperandRegClass<0, eDJ>]>,
                             ItinRegClassPair<II_LDA_POST_2D_DN,[OperandRegClass<0, eDN>]>], 
               DecoderMethod = "DecodeLDA_2D_Instruction" in {
      def LDA_2D_dms_lda : AIE2_dms_lda_inst_lda<
        (outs OP_mLdaScl:$mLdaScl, eP:$ptr_out, eDC:$count_out),
        (ins eP:$ptr, eD:$mod),
        "lda.2d", "$mLdaScl, [$ptr], $mod">,
      AIE2_ag_all__ag_pstm_2d;
    }
  }

  let Itinerary = II_LDA_POST_2D_R, Constraints = "$ptr = $ptr_out" in {
    let DecoderMethod = "DecodeLDA_2D_HBInstruction" in {
      def LDA_2D_S8_dmhb_lda : AIE2_dmhb_lda_inst_lda<0b00,
        (outs eR:$mRa, eP:$ptr_out, eDC:$count_out),
        (ins  eP:$ptr, eD:$mod  ),
        "lda.2d.s8", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_2d;

      def LDA_2D_U8_dmhb_lda : AIE2_dmhb_lda_inst_lda<0b01,
        (outs eR:$mRa, eP:$ptr_out, eDC:$count_out),
        (ins  eP:$ptr, eD:$mod  ),
        "lda.2d.u8", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_2d;

      def LDA_2D_S16_dmhb_lda : AIE2_dmhb_lda_inst_lda<0b10,
        (outs eR:$mRa, eP:$ptr_out, eDC:$count_out),
        (ins  eP:$ptr, eD:$mod  ),
        "lda.2d.s16", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_2d;

      def LDA_2D_U16_dmhb_lda : AIE2_dmhb_lda_inst_lda<0b11,
        (outs eR:$mRa, eP:$ptr_out, eDC:$count_out),
        (ins  eP:$ptr, eD:$mod  ),
        "lda.2d.u16", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_2d;
    }
  }

  let Itinerary = II_LDA_POST_3D, Constraints = "$ptr = $ptr_out" in {
    let ItineraryRegPairs = [ItinRegClassPair<II_LDA_POST_3D_R,[OperandRegClass<0, eR>]>,
                             ItinRegClassPair<II_LDA_POST_3D_P,[OperandRegClass<0, eP>]>, 
                             ItinRegClassPair<II_LDA_POST_3D_M,[OperandRegClass<0, eM>]>,
                             ItinRegClassPair<II_LDA_POST_3D_DC,[OperandRegClass<0, eDC>]>,
                             ItinRegClassPair<II_LDA_POST_3D_DJ,[OperandRegClass<0, eDJ>]>,
                             ItinRegClassPair<II_LDA_POST_3D_DN,[OperandRegClass<0, eDN>]>],
               DecoderMethod = "DecodeLDA_3D_Instruction" in {
      def LDA_3D_dms_lda : AIE2_dms_lda_inst_lda<
        (outs OP_mLdaScl:$mLdaScl, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
        (ins eP:$ptr, eDS:$mod),
        "lda.3d", "$mLdaScl, [$ptr], $mod">,
      AIE2_ag_all__ag_pstm_3d;
    }
  }

  let Itinerary = II_LDA_POST_3D_R, Constraints = "$ptr = $ptr_out" in {
    let DecoderMethod = "DecodeLDA_3D_HBInstruction" in {

      def LDA_3D_S8_dmhb_lda : AIE2_dmhb_lda_inst_lda<0b00,
        (outs eR:$mRa, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
        (ins eP:$ptr, eDS:$mod),
        "lda.3d.s8", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_3d;

      def LDA_3D_U8_dmhb_lda : AIE2_dmhb_lda_inst_lda<0b01,
        (outs eR:$mRa, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
        (ins eP:$ptr, eDS:$mod),
        "lda.3d.u8", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_3d;

      def LDA_3D_S16_dmhb_lda : AIE2_dmhb_lda_inst_lda<0b10,
        (outs eR:$mRa, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
        (ins eP:$ptr, eDS:$mod),
        "lda.3d.s16", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_3d;

      def LDA_3D_U16_dmhb_lda : AIE2_dmhb_lda_inst_lda<0b11,
        (outs eR:$mRa, eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
        (ins eP:$ptr, eDS:$mod),
        "lda.3d.u16", "$mRa, [$ptr], $mod">,
      AIE2_ag_nospill__ag_pstm_3d;
    }
  }
}
let mayLoad = false, mayStore = true in {
  let Itinerary = II_ST in {
    def ST_dms_sts_idx : AIE2_dms_sts_inst_st< (outs) , (ins OP_mSclSt:$mSclSt, eP:$ptr, eDJ:$dj),
        "st", "$mSclSt, [$ptr, $dj]">,
      AIE2_ag_all__ag_idx;
    def ST_dms_sts_idx_imm : AIE2_dms_sts_inst_st< (outs) , (ins OP_mSclSt:$mSclSt, eP:$ptr, imm6x4:$imm),
        "st", "$mSclSt, [$ptr, $imm]">,
      AIE2_ag_all__ag_idx_imm;
  }
  let Itinerary = II_ST_POST_1D, Constraints = "$ptr = $ptr_out" in {
    def ST_dms_sts_pstm_nrm : AIE2_dms_sts_inst_st< (outs eP:$ptr_out) , (ins OP_mSclSt:$mSclSt, eP:$ptr, eM:$mod),
        "st", "$mSclSt, [$ptr], $mod">,
      AIE2_ag_all__ag_pstm_nrm;
    def ST_dms_sts_pstm_nrm_imm : AIE2_dms_sts_inst_st< (outs eP:$ptr_out) , (ins OP_mSclSt:$mSclSt, eP:$ptr, imm7x4:$imm),
        "st", "$mSclSt, [$ptr], $imm">,
      AIE2_ag_all__ag_pstm_nrm_imm;
  }
  let Itinerary = II_ST_POST_2D, Constraints = "$ptr = $ptr_out" in {
    let DecoderMethod = "DecodeST_2D_Instruction" in {
      def ST_2D_dms_sts : AIE2_dms_sts_inst_st<
        (outs eP:$ptr_out, eDC:$count_out),
        (ins OP_mSclSt:$mSclSt, eP:$ptr, eD:$mod),
        "st.2d", "$mSclSt, [$ptr], $mod">,
      AIE2_ag_all__ag_pstm_2d;
    }
  }
    let Itinerary = II_ST_POST_3D, Constraints = "$ptr = $ptr_out" in {
    let DecoderMethod = "DecodeST_3D_Instruction" in {
      def ST_3D_dms_sts : AIE2_dms_sts_inst_st<
        (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
        (ins OP_mSclSt:$mSclSt, eP:$ptr, eDS:$mod),
        "st.3d", "$mSclSt, [$ptr], $mod">,
      AIE2_ag_all__ag_pstm_3d;
    }
  }
}

let hasSideEffects = true, mayLoad = false, mayStore = false in {
  let Itinerary = II_ST_MS in {
    def MOV_mv_scl2ms : AIE2_mv_scl2ms_b_noTlast< (outs) , (ins OP_mSclMS:$src),
        "mov", "ms, $src">;
    def MOV_TLAST_mv_scl2ms : AIE2_mv_scl2ms_b_doTlast< (outs) , (ins OP_mSclMS:$src),
        "mov.tlast", "ms, $src">;
    def MOV_mv_scl2ms_doTlast_reg : AIE2_mv_scl2ms_b_doTlast_reg< (outs) , (ins OP_mSclMS:$src, eR28:$tlast),
        "mov", "ms, $src, $tlast">;
  }
  let Defs = [srMS0] in {
    let Itinerary = II_ST_MS_NB in {
      def MOV_NB_mv_scl2ms : AIE2_mv_scl2ms_nb_noTlast< (outs) , (ins OP_mSclMS:$src),
          "mov.nb", "ms, $src">;
      def MOV_NB_TLAST_mv_scl2ms : AIE2_mv_scl2ms_nb_doTlast< (outs) , (ins OP_mSclMS:$src),
          "mov.nb.tlast", "ms, $src">;
    }
    let Itinerary = II_ST_MS_NB_TLAST_REG in {
      def MOV_NB_mv_scl2ms_doTlast_reg : AIE2_mv_scl2ms_nb_doTlast_reg< (outs) , (ins OP_mSclMS:$src, eR28:$tlast),
          "mov.nb", "ms, $src, $tlast">;
    }
  }
}

// 6.2 MOV – Packet switched stream access
let hasSideEffects = true, mayLoad = false, mayStore = false in {
  let Itinerary = II_MOV_PH_B in {
    def MOV_mv_ph2ms : AIE2_mv_ph2ms_b_noTlast< (outs) , (ins eR:$id, imm3:$pcktType),
        "mov.ph", "ms, $id, $pcktType">;
    def MOV_TLAST_mv_ph2ms : AIE2_mv_ph2ms_b_doTlast< (outs) , (ins eR:$id, imm3:$pcktType),
        "mov.ph.tlast", "ms, $id, $pcktType">;
    def MOV_mv_ph2ms_doTlast_reg : AIE2_mv_ph2ms_b_doTlast_reg< (outs) , (ins eR:$id, imm3:$pcktType, eR28:$tlast),
        "mov.ph", "ms, $id, $pcktType, $tlast">;
  }
  let Defs = [srMS0] in {
    let Itinerary = II_MOV_PH_NB in {
      def MOV_NB_mv_ph2ms : AIE2_mv_ph2ms_nb_noTlast< (outs) , (ins eR:$id, imm3:$pcktType),
          "mov.ph.nb", "ms, $id, $pcktType">;
      def MOV_NB_TLAST_mv_ph2ms : AIE2_mv_ph2ms_nb_doTlast< (outs) , (ins eR:$id, imm3:$pcktType),
          "mov.ph.nb.tlast", "ms, $id, $pcktType">;
    }
    let Itinerary = II_MOV_PH_NB_TLAST_REG in {
      def MOV_NB_mv_ph2ms_doTlast_reg : AIE2_mv_ph2ms_nb_doTlast_reg< (outs) , (ins eR:$id, imm3:$pcktType, eR28:$tlast),
          "mov.ph.nb", "ms, $id, $pcktType, $tlast">;
    }
  }

  let Itinerary = II_MOV_CPH_B in {
    def MOV_mv_cph2ms : AIE2_mv_cph2ms_b_noTlast< (outs) , (ins eM:$addr, imm2:$nw, imm2:$op, eR:$id),
        "mov.cph", "ms, $addr, $nw, $op, $id">;
    def MOV_TLAST_mv_cph2ms : AIE2_mv_cph2ms_b_doTlast< (outs) , (ins eM:$addr, imm2:$nw, imm2:$op, eR:$id),
        "mov.cph.tlast", "ms, $addr, $nw, $op, $id">;
    def MOV_mv_cph2ms_doTlast_reg : AIE2_mv_cph2ms_b_doTlast_reg< (outs) , (ins eM:$addr, imm2:$nw, imm2:$op, eR:$id, eR28:$tlast),
        "mov.cph", "ms, $addr, $nw, $op, $id, $tlast">;
  }
  let Defs = [srMS0] in {
    let Itinerary = II_MOV_CPH_NB in {
      def MOV_NB_mv_cph2ms : AIE2_mv_cph2ms_nb_noTlast< (outs) , (ins eM:$addr, imm2:$nw, imm2:$op, eR:$id),
          "mov.cph.nb", "ms, $addr, $nw, $op, $id">;
      def MOV_NB_TLAST_mv_cph2ms : AIE2_mv_cph2ms_nb_doTlast< (outs) , (ins eM:$addr, imm2:$nw, imm2:$op, eR:$id),
          "mov.cph.nb.tlast", "ms, $addr, $nw, $op, $id">;
    }
    let Itinerary = II_MOV_CPH_NB_TLAST_REG in {
      def MOV_NB_mv_cph2ms_doTlast_reg : AIE2_mv_cph2ms_nb_doTlast_reg< (outs) , (ins eM:$addr, imm2:$nw, imm2:$op, eR:$id, eR28:$tlast),
          "mov.cph.nb", "ms, $addr, $nw, $op, $id, $tlast">;
    }
  }
}

let Itinerary = II_J, hasDelaySlot = true in {
  def J_jump_imm : AIE2_jump_imm_lng<(outs), 
      (ins addr20:$cpmaddr), "j", "$cpmaddr">;
}

let Itinerary = II_J_IND, hasDelaySlot = true in {
  def J_jump_ind : AIE2_jump_ind_inst_alu<(outs), (ins eP:$mPm), "j", "$mPm">;
}

let hasDelaySlot = true, Defs = [lr] in {
  let Itinerary = II_JL in {
    def JL : AIE2_jump_link_imm_lng<(outs), (ins imm20:$cpmaddr), "jl", "$cpmaddr">;
  }
  let Itinerary = II_JL_IND in {
    def JL_IND : AIE2_jump_link_ind_inst_alu<(outs), (ins eP:$mPm), "jl", "$mPm">;
  }
}

let Itinerary = II_JNZD, hasDelaySlot = true, Defs = [srCarry] in {
  def JNZD : AIE2_jumpc_decr_ind_inst_alu< (outs eR:$mRx),
      (ins eR:$mRx0, eP:$mPm), "jnzd", "$mRx, $mRx0, $mPm">;
}

let Itinerary = II_JNZ, hasDelaySlot = true in {
  def JNZ : AIE2_jumpc_imm_lng<0b1, (outs), 
      (ins eR:$mRx, addr20:$cpmaddr), "jnz", "$mRx, $cpmaddr">;
}

let Itinerary = II_JZ, hasDelaySlot = true in {
  def JZ : AIE2_jumpc_imm_lng<0b0, (outs),
      (ins eR:$mRx, addr20:$cpmaddr), "jz", "$mRx, $cpmaddr">;
}

let Itinerary = II_MOVA, isMoveImm = 1, isReMaterializable = 1, isAsCheapAsAMove = 1 in {
  let ItineraryRegPairs = [ItinRegClassPair<II_MOVA_R,[OperandRegClass<0, eR>]>, ItinRegClassPair<II_MOVA_P,[OperandRegClass<0, eP>]>, 
                           ItinRegClassPair<II_MOVA_M,[OperandRegClass<0, eM>]>, ItinRegClassPair<II_MOVA_DC,[OperandRegClass<0, eDC>]>,
                           ItinRegClassPair<II_MOVA_DJ,[OperandRegClass<0, eDJ>]>, ItinRegClassPair<II_MOVA_DN,[OperandRegClass<0, eDN>]>] in {  
    def MOVA_lda_cg : AIE2_lda_cg_inst_lda<(outs OP_mLdaCg:$mLdaCg),
        (ins simm11:$c11s), "mova", "$mLdaCg, $c11s">;
  }
}

let Itinerary = II_MOV_SS, Defs = [srSS0],
    hasSideEffects = true, mayLoad = false, mayStore = false in {
  def MOV_mv_ss2scl : AIE2_mv_ss2scl_b<(outs eR:$mRa),
      (ins), "mov", "$mRa, SS">;
  def MOV_NB_mv_ss2scl : AIE2_mv_ss2scl_nb<(outs eR:$mRa),
      (ins), "mov.nb", "$mRa, SS">;
}

// Unit "A" / "S"
let Itinerary = II_PADD in {
  let Constraints = "$ptr = $res" in {
    def PADDA_lda_ptr_inc_idx : AIE2_lda_ptr_inc_inst_lda< (outs eP:$res), 
        (ins eP:$ptr, eM:$mod), "padda", "[$ptr], $mod">,
      AIE2_ag_standalone__ag_pstm_nrm;
    def PADDA_lda_ptr_inc_idx_imm : AIE2_lda_ptr_inc_inst_lda< (outs eP:$res), 
        (ins eP:$ptr, imm10x4:$imm), "padda", "[$ptr], $imm">,
      AIE2_ag_standalone__ag_pstm_nrm_imm;

    def PADDS_st_ptr_inc_idx : AIE2_st_ptr_inc_inst_st< (outs eP:$res),
        (ins eP:$ptr, eM:$mod), "padds", "[$ptr], $mod">,
      AIE2_ag_standalone__ag_pstm_nrm;
    def PADDS_st_ptr_inc_idx_imm : AIE2_st_ptr_inc_inst_st< (outs eP:$res),
        (ins eP:$ptr, imm10x4:$imm), "padds", "[$ptr], $imm">,
      AIE2_ag_standalone__ag_pstm_nrm_imm;
  }
}

// Unit "B"
let Itinerary = II_PADD in {
  let Constraints = "$ptr = $res" in {
    def PADDB_ldb_ptr_inc_nospill_nrm : AIE2_ldb_ptr_inc_inst_ldb< (outs eP:$res),
        (ins eP:$ptr, eM:$mod), "paddb", "[$ptr], $mod">,
      AIE2_agb_standalone__agb_nospill__ag_pstm_nrm;
    def PADDB_ldb_ptr_inc_nrm_imm : AIE2_ldb_ptr_inc_inst_ldb< (outs eP:$res),
        (ins eP:$ptr, imm9x4:$imm), "paddb", "[$ptr], $imm">,
      AIE2_agb_standalone__ag_pstm_nrm_imm;
  }
}

// SP can be adjusted up and down in steps of 32
let Itinerary = II_PADD in {
  let Defs = [SP], Uses = [SP] in {
    def PADDA_sp_imm : AIE2_lda_ptr_inc_inst_lda< (outs), (ins imm13x32:$imm),
      "padda", "[sp], $imm">, AIE2_ag_standalone__ag_pstm_sp_imm;
    def PADDB_sp_imm : AIE2_ldb_ptr_inc_inst_ldb< (outs), (ins imm12x32:$imm),
      "paddb", "[sp], $imm">, AIE2_agb_standalone__agb_pstm_sp_imm;
  }
}

let Itinerary = II_PADD_2D in {
  let Constraints = "$ptr_out = $ptr" in {
	let DecoderMethod = "DecodePADD_2DInstruction" in {
      def PADDA_2D : AIE2_lda_ptr_inc_inst_lda<
          (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod),
          "padda.2d", "[$ptr], $mod">,
        AIE2_ag_standalone__ag_pstm_2d;

      def PADDS_2D : AIE2_st_ptr_inc_inst_st<
          (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod),
          "padds.2d", "[$ptr], $mod">,
        AIE2_ag_standalone__ag_pstm_2d;
    }
    let DecoderMethod = "DecodePADDB_2DInstruction" in {
      def PADDB_2D : AIE2_ldb_ptr_inc_inst_ldb<
          (outs eP:$ptr_out, eDC:$count_out),
          (ins eP:$ptr, eD:$mod),
          "paddb.2d", "[$ptr], $mod">,
        AIE2_agb_standalone__agb_nospill__ag_pstm_2d;
    }
  }
}

let Itinerary = II_PADD_3D in {
  let Constraints = "$ptr_out = $ptr" in {
	let DecoderMethod = "DecodePADD_3DInstruction" in {
      def PADDA_3D : AIE2_lda_ptr_inc_inst_lda<
          (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod),
          "padda.3d", "[$ptr], $mod">,
        AIE2_ag_standalone__ag_pstm_3d;
      def PADDS_3D : AIE2_st_ptr_inc_inst_st<
          (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod),
          "padds.3d", "[$ptr], $mod">,
        AIE2_ag_standalone__ag_pstm_3d;
    }
  let DecoderMethod = "DecodePADDB_3DInstruction" in {
      def PADDB_3D : AIE2_ldb_ptr_inc_inst_ldb<
          (outs eP:$ptr_out, eDC:$count_lo_out, eDC:$count_hi_out),
          (ins eP:$ptr, eDS:$mod),
          "paddb.3d", "[$ptr], $mod">,
        AIE2_agb_standalone__agb_nospill__ag_pstm_3d;
    }
  }
}

// MOV & MOVXM instruction have identical itineraries, they differ only in slot usage 
let Itinerary = II_MOV, isMoveImm = 1, isReMaterializable = 1 in {
  let ItineraryRegPairs = [ItinRegClassPair<II_MOV_RS,[OperandRegClass<0, eR>]>, ItinRegClassPair<II_MOV_P,[OperandRegClass<0, eP>]>, 
                          ItinRegClassPair<II_MOV_M,[OperandRegClass<0, eM>]>, ItinRegClassPair<II_MOV_RS,[OperandRegClass<0, eS>]>,
                          ItinRegClassPair<II_MOV_DC,[OperandRegClass<0, eDC>]>, ItinRegClassPair<II_MOV_DJ,[OperandRegClass<0, eDJ>]>,
                          ItinRegClassPair<II_MOV_DN,[OperandRegClass<0, eDN>]>] in {
    // MOVXM is quite expensive in code size, and will block an extra slot, so we don't
    // mark it as isAsCheapAsAMove
    let isAsCheapAsAMove = 0 in {
      def MOVXM_lng_cg : AIE2_lng_cg_lng< (outs OP_mMvSclDstCg:$mMvSclDstCg), 
          (ins simm32:$i), "movxm", "$mMvSclDstCg, $i">;
    }
    let isAsCheapAsAMove = 1 in {  
      def MOV_mv_cg : AIE2_mv_cg_inst_mv< (outs OP_mMvSclDstCg:$dst) , (ins simm10:$i),
          "mov", "$dst, $i">;
    }
  }
}

let Itinerary = II_MOV_SCL, isMoveReg = 1 in {
  let ItineraryRegPairs = [ItinRegClassPair<II_MOV_SCL_RS,[OperandRegClass<0, eR>]>, ItinRegClassPair<II_MOV_SCL_P,[OperandRegClass<0, eP>]>, 
                           ItinRegClassPair<II_MOV_SCL_M,[OperandRegClass<0, eM>]>, ItinRegClassPair<II_MOV_SCL_RS,[OperandRegClass<0, eS>]>,
                           ItinRegClassPair<II_MOV_SCL_DC,[OperandRegClass<0, eDC>]>, ItinRegClassPair<II_MOV_SCL_DJ,[OperandRegClass<0, eDJ>]>,
                           ItinRegClassPair<II_MOV_SCL_DN,[OperandRegClass<0, eDN>]>] in {
    def MOV_mv_scl : AIE2_mv_scl_inst_mv<(outs OP_mMvSclDst:$mMvSclDst), 
        (ins OP_mMvSclSrc:$mMvSclSrc), "mov", "$mMvSclDst, $mMvSclSrc">;
  }
}

let Itinerary = II_MUL in {
  def MUL_mul_r_rr : AIE2_mul_r_rr_inst_alu< (outs eR:$mRx), 
      (ins eR:$mRx0, eR:$mRy), "mul", "$mRx, $mRx0, $mRy">;
}

let Itinerary = II_MOVX, isMoveReg = 1 in {
  def MOVX_mvx_scl : AIE2_mvx_scl_inst_alu< (outs mCRm:$mCRm), (ins eR:$mRx),
      "movx", "$mCRm, $mRx">;
}

let Itinerary = II_MOVX,  isMoveImm = 1, isReMaterializable = 1, isAsCheapAsAMove = 1 in {
  def MOVX_alu_cg : AIE2_alu_cg_inst_alu< (outs OP_mAluCg:$dst), (ins simm11:$i),
      "movx", "$dst, $i">;
}

let Itinerary = II_SELEQZ in {
  def SELEQZ : AIE2_select_r_rr_inst_alu< 0b0,(outs eR:$mRx), 
      (ins eR:$mRx0, eR:$mRy, eR27:$s2), "sel.eqz", "$mRx, $mRx0, $mRy, r27">;
}

let Itinerary = II_SELNEZ in {
  def SELNEZ : AIE2_select_r_rr_inst_alu< 0b1,(outs eR:$mRx),
      (ins eR:$mRx0, eR:$mRy, eR27:$s2), "sel.nez", "$mRx, $mRx0, $mRy, r27">;
}

let Itinerary = II_DONE, hasSideEffects = true in {
  def DONE : AIE2_done_inst_alu<(outs), (ins), "done", "">;
}

let Itinerary = II_ACQ, hasSideEffects = true in {
  def ACQ_mLockId_reg : AIE2_acquire_inst_alu<(outs), (ins eR:$mRx, eR:$mRy), "acq", "$mRx, $mRy">, AIE2_mLockId_reg;
  def ACQ_mLockId_imm : AIE2_acquire_inst_alu<(outs), (ins imm6:$id, eR:$mRy), "acq", "$id, $mRy">, AIE2_mLockId_imm;
}

let Itinerary = II_ACQ_COND, hasSideEffects = true in {
  def ACQ_COND_mLockId_reg : AIE2_acquire_cond_inst_alu<(outs), (ins eR:$mRx, eR:$mRy, eR26:$s2), "acq.cond", "$mRx, $mRy, r26">, AIE2_mLockId_reg;
  def ACQ_COND_mLockId_imm : AIE2_acquire_cond_inst_alu<(outs), (ins imm6:$id, eR:$mRy, eR26:$s2), "acq.cond", "$id, $mRy, r26">, AIE2_mLockId_imm;
}

let Itinerary = II_REL, hasSideEffects = true in {
  def REL_mLockId_reg : AIE2_release_inst_alu<(outs), (ins eR:$mRx, eR:$mRy), "rel", "$mRx, $mRy">, AIE2_mLockId_reg;
  def REL_mLockId_imm : AIE2_release_inst_alu<(outs), (ins imm6:$id, eR:$mRy), "rel", "$id, $mRy">, AIE2_mLockId_imm;
}

let Itinerary = II_REL_COND, hasSideEffects = true in {
  def REL_COND_mLockId_reg : AIE2_release_cond_inst_alu<(outs), (ins eR:$mRx, eR:$mRy, eR26:$s2), "rel.cond", "$mRx, $mRy, r26">, AIE2_mLockId_reg;
  def REL_COND_mLockId_imm : AIE2_release_cond_inst_alu<(outs), (ins imm6:$id, eR:$mRy, eR26:$s2), "rel.cond", "$id, $mRy, r26">, AIE2_mLockId_imm;
}

let hasNoSchedulingInfo = true, hasSideEffects = true, mayLoad = false, mayStore = false, isConvergent = true, isMeta = true in {
  def SCHED_BARRIER : Pseudo<(outs), (ins)>;
}

}
