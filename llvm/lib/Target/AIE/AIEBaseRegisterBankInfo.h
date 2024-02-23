//===- AIEBaseRegisterBankInfo.h --------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the RegisterBankInfo class for AIE.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEBASEREGISTERBANKINFO_H
#define LLVM_LIB_TARGET_AIE_AIEBASEREGISTERBANKINFO_H

#include "llvm/CodeGen/RegisterBankInfo.h"

namespace llvm {

/// Base class to contain common register bank selection code for
/// all AIE versions.
class AIEBaseRegisterBankInfo : public RegisterBankInfo {
public:
  using RegisterBankInfo::RegisterBankInfo;

protected:
  enum PartialMappingIdx {
    PMI_None = -1,
    PMI_GPR = 1,
    PMI_PTR = 2,
    PMI_MOD = 3,
    PMI_CB = 4,
    PMI_GPR64 = 5,
    PMI_VREG256 = 8,
    PMI_VREG512 = 9,
    PMI_VREG1024 = 10,
    PMI_ACC256 = 11,
    PMI_ACC512 = 12,
    PMI_ACC1024 = 13,
    PMI_VREGMin = PMI_VREG256,
    PMI_Min = PMI_GPR,
    PMI_VREG128 = 14
  };

  enum ValueMappingIdx {
    InvalidIdx = 0,
    First3OpsIdx = 1,
    Last3OpsIdx = 10,
    DistanceBetweenRegBanks = 3,
  };

  static unsigned getRegBankBaseIdxOffset(unsigned RBIdx, unsigned Size);

  /// Get the pointer to the ValueMapping representing the RegisterBank
  /// at \p RBIdx with a size of \p Size.
  ///
  /// The returned mapping works for instructions with the same kind of
  /// operands for up to 3 operands.
  ///
  /// \pre \p RBIdx != PartialMappingIdx::None
  virtual const RegisterBankInfo::ValueMapping *
  getValueMapping(PartialMappingIdx RBIdx, unsigned Size) const = 0;

  /// See RegisterBankInfo::applyMapping.
  void applyMappingImpl(MachineIRBuilder &Builder,
                        const OperandsMapper &OpdMapper) const override;

  /// Get an instruction mapping where all the operands map to
  /// the same register bank and have similar size.
  ///
  /// \pre MI.getNumOperands() <= 3
  ///
  /// \return An InstructionMappings with a statically allocated
  /// OperandsMapping.
  const InstructionMapping &
  getSameKindOfOperandsMapping(const MachineInstr &MI) const;

  static PartialMappingIdx getPartialMappingIdx(const LLT &Ty);

public:
  InstructionMappings
  getInstrAlternativeMappings(const MachineInstr &MI) const override;

  const InstructionMapping &
  getInstrMapping(const MachineInstr &MI) const override;

  bool requiresGPRRegBank(const MachineInstr &MI,
                          const MachineRegisterInfo &MRI, unsigned Depth) const;

  bool requiresPTRRegBank(const MachineInstr &MI,
                          const MachineRegisterInfo &MRI, unsigned Depth) const;

protected:
  // AIE specific generic instructions have target-specific OpCodes
  virtual void setAIEGenericInstrMapping(
      const MachineInstr &MI, SmallVector<unsigned, 4> &OpSize,
      SmallVector<PartialMappingIdx, 4> &OpRegBankIdx) const = 0;

  // Final stage of getInstrMapping for reuse in derived classes
  const InstructionMapping &getInstrMappingFinal(
      const MachineInstr &MI, unsigned Cost,
      const SmallVector<unsigned, 4> &OpSize,
      const SmallVector<PartialMappingIdx, 4> &OpRegBankIdx) const;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEBASEREGISTERBANKINFO_H
