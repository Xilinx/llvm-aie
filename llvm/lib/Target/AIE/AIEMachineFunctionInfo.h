//=- AIEMachineFunctionInfo.h - AIE machine function info -----*- C++ -*-=//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file declares AIE-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_AIEMACHINEFUNCTIONINFO_H
#define LLVM_LIB_TARGET_AIE_AIEMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MIRYamlMapping.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include <optional>

namespace llvm {
// Target-specific memory operand kinds. A PseudoSourceValue represents
// a part of memory that is distinct from any other object.
// By adding this as address value to a memory operand, the schedule
// dag constructor can avoid false memory dependences.
// We use it to separate tile memory access from the rest of memory.
// More can be added.
// These object representations live in the machine function, and
// are (de)serialized to/from mir using some overloads in MIRFormatter
enum AIETargetPSV { AIETileMem = PseudoSourceValue::TargetCustom };

class AIEPseudoSourceValue : public PseudoSourceValue {
public:
  AIEPseudoSourceValue(AIETargetPSV Kind, const TargetMachine &TM)
      : PseudoSourceValue(Kind, TM) {}

  // A PseudoSourceValue represents an object, which is compared to other
  // objects. If a memory access doesn't carry an object, it may alias
  // with other objects. With the alias methods below, we say that we
  // can not leak an address in TileMemory to such an unannotated pointer,
  // and that an access annotated with TileMemory can not touch anything
  // which is not annotated as TileMemory. (We can not be killed and we
  // don't kill.).
  // To make this safe, we need to make sure that we don't lose the
  // PseudoSourceValue on these accesses.
protected:
  bool isConstant(const MachineFrameInfo *) const override { return false; }
  bool mayAlias(const MachineFrameInfo *) const override { return false; }
  bool isAliased(const MachineFrameInfo *) const override { return false; }
};

class TileMemoryPSV : public AIEPseudoSourceValue {
public:
  TileMemoryPSV(const TargetMachine &TM)
      : AIEPseudoSourceValue(AIETileMem, TM) {}
  void printCustom(raw_ostream &O) const override { O << "TileMemory"; }
};

namespace yaml {
struct AIEMachineFunctionInfo;
} // namespace yaml

/// AIEMachineFunctionInfo - This class is derived from MachineFunctionInfo
/// and contains private AIE-specific information for each MachineFunction.
class AIEMachineFunctionInfo : public MachineFunctionInfo {
private:
  /// FrameIndex for start of varargs area
  int VarArgsFrameIndex = 0;

  const TileMemoryPSV TileMemory;

public:
  //  AIEMachineFunctionInfo() = default;

  AIEMachineFunctionInfo(const Function &F, const TargetSubtargetInfo *STI,
                         const TargetMachine &TM);

  void initializeBaseYamlFields(const yaml::AIEMachineFunctionInfo &YamlMFI);

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }
  const PseudoSourceValue *getTileMemory() const { return &TileMemory; }
};

namespace yaml {
struct AIEMachineFunctionInfo final : public yaml::MachineFunctionInfo {
  std::optional<int> VarArgsFrameIndex;

  AIEMachineFunctionInfo() = default;
  AIEMachineFunctionInfo(const llvm::AIEMachineFunctionInfo &MFI);

  void mappingImpl(yaml::IO &YamlIO) override;
  ~AIEMachineFunctionInfo() = default;
};

template <> struct MappingTraits<AIEMachineFunctionInfo> {
  static void mapping(IO &YamlIO, AIEMachineFunctionInfo &MFI) {
    YamlIO.mapOptional("varArgsFrameIndex", MFI.VarArgsFrameIndex);
  }
};

} // end namespace yaml

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_AIEMACHINEFUNCTIONINFO_H
