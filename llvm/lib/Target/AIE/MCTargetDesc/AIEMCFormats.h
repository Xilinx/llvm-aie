//===- AIEMCFormats.h - Interfaces for the auto-generated Formats ---------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
// Utility classes to interface the generated Formats from CodeGenFormat with
// our C++ AIE Backend.
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCFORMATS_H
#define LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCFORMATS_H

#include "AIEFormat.h"
#include "llvm/ADT/APInt.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Support/ErrorHandling.h"
#include <cstddef>
#include <map>
#include <unordered_map>

namespace llvm {

using SlotBits = uint64_t;
class MCSlotInfo;
class MCSlotKind {
  /// Kind of the slot
  int Kind;

public:
  // For use in unordered_map
  class Hasher {
  public:
    size_t operator()(MCSlotKind SK) const { return SK; }
  };

  /// Set of Kinds of all AIE subtargets
  enum SlotKind : int {
    SLOT_UNKNOWN = -1,
  };
  enum AIESlotKind : int {
#define GET_FORMATS_SLOTKINDS
#include "AIEGenFormats.inc"
#undef GET_FORMATS_SLOTKINDS
  };
  enum AIE2SlotKind : int {
#define GET_FORMATS_SLOTKINDS
#include "AIE2GenFormats.inc"
#undef GET_FORMATS_SLOTKINDS
  };

  /// Ctor with SlotKind initialization.
  constexpr MCSlotKind(int Kind) : Kind(Kind) {}

  /// Default constructor, initialize the value at UNKNOWN
  constexpr MCSlotKind() : Kind(SLOT_UNKNOWN) {}
  /// Copy/Move Ctor
  constexpr MCSlotKind(const MCSlotKind &) = default;
  constexpr MCSlotKind(MCSlotKind &&) = default;

  inline bool operator==(const MCSlotKind &OtherKind) const {
    return Kind == OtherKind.Kind;
  }

  inline bool operator!=(const MCSlotKind &OtherKind) const {
    return !(*this == OtherKind);
  }

  inline MCSlotKind &operator=(const MCSlotKind &SlotKind) {
    Kind = SlotKind.Kind;
    return *this;
  }

  /// NOTE: We use it to define a method DerivedClass::getSlotInfo() with
  /// a switch on an instance of the DerivedClass itself.
  operator int() const { return Kind; }
};

// "Generic" Slot Information class implementation
class MCSlotInfo {
private:
  /// Name of the slot
  /// NOTE: a bit old-fashion but needed to guarantee the constexpr nature of
  /// this class.
  const char *SlotName;
  /// Size of the slot (in bits)
  const unsigned Size;
  /// Bitset representing the occupancy of the slot
  const SlotBits SlotOccupancy;
  /// Opcode of the NOP inst. attached to the slot
  const unsigned NopOpc;

public:
  constexpr MCSlotInfo(const char *SlotName, unsigned Size, SlotBits Bits,
                       unsigned NopOpc)
      : SlotName(SlotName), Size(Size), SlotOccupancy(Bits), NopOpc(NopOpc) {}

  const char *getName() const { return SlotName; }
  SlotBits getSlotSet() const { return SlotOccupancy; }
  unsigned getNOPOpcode() const { return NopOpc; }
  unsigned getSize() const { return Size; }
};

/// A field of an instruction format
class MCFormatField {
public:
  using GlobalOffsets = struct {
    unsigned LeftOffset;
    unsigned RightOffset;
  };

  enum class MCFormatFieldType { Variable, FixedBits };

private:
  /// ID of the Parent node in the current tree.
  /// If the Parent is unknown, it is set to -1.
  /// NOTE: Currently unused
  const int ParentID;
  /// Type of the field
  const MCFormatFieldType Type;
  /// Name of the field.
  /// Set to nullptr if the field isn't defining a Variable field.
  const char *FieldName = nullptr;
  /// Global (instruction encoding scope) offsets of the field indexed on the
  /// most significant bits
  std::optional<const GlobalOffsets> LocInfos;
  /// Kind of the slot
  const MCSlotKind SlotKind;

public:
  constexpr MCFormatField(int ParentID, MCFormatFieldType Type,
                          const char *const FieldName,
                          std::optional<const GlobalOffsets> LocInfos,
                          const MCSlotKind &Slot)
      : ParentID(ParentID), Type(Type), FieldName(FieldName),
        LocInfos(LocInfos), SlotKind(Slot) {}

  inline unsigned getSize() const {
    return LocInfos->RightOffset - LocInfos->LeftOffset + 1;
  }

  /// Returns a couple of Offset (begin, end), indexed on the most significant
  /// bit.
  inline const GlobalOffsets &getOffsets() const { return LocInfos.value(); }

  MCFormatFieldType getType() const { return Type; }

  const char *getName() const { return FieldName; }

  int getParentID() const { return ParentID; }

  MCSlotKind getSlotKind() const { return SlotKind; }
};

} // end namespace llvm

namespace llvm {
/// Generic class representing a description of a format
class MCFormatDesc {
public:
  enum MCFormatKind { FK_Packet, FK_Instr };

  using SlotsFieldsMap =
      std::unordered_map<const MCSlotKind, const MCFormatField *,
                         MCSlotKind::Hasher>;
  /// This behaves like an array of variable sized arrays.
  /// All elements are consecutive in memory. The outer dimension
  /// is an array of pointers into that memory array, which is passed
  /// in as \p Base
  /// An array's upper bound is the next array's lower bound.
  /// The pointer table contains an extra element pointing beyond
  /// the last entry in the contguous memory.
  /// Hence we can create an array reference from two adjacent
  /// pointers.
  class OperandsFieldsMap {

  public:
    constexpr OperandsFieldsMap(const MCFormatField *const *const *Base,
                                unsigned Size)
        : Base(Base), Size(Size) {}
    unsigned size() const { return Size; }
    ArrayRef<const MCFormatField *> at(unsigned Idx) const {
      assert(Idx < Size);
      return {Base[Idx], Base[Idx + 1]};
    }

  private:
    const MCFormatField *const *const *Base;
    unsigned Size;
  };

protected:
  /// Kind of the Format, packet or subinstruction
  const MCFormatKind Kind;
  /// Is it a MultiSlotPseudo instruction ?
  const bool HasMultipleSlotOptions = false;
  /// Opcode of the MCInst on which the format is attached
  unsigned Opcode;
  /// Container binding any Slot Kind with its corresponding Field in the
  /// instruction (Composite or not)
  const SlotsFieldsMap SlotsMap;
  /// Map between an MCOperand index and a vector of fields (possibly of size
  /// n). For example in AIE1, the encoding of the 20-bit immediate of the
  /// instructions MOV_U20/MOV_S20 is split in two parts:
  ///  ...|imm{19-14}|reg{6}|imm{13-0}|...
  ///          (0)              (1)
  /// In this case, the set of fields has a size of 2 and has the order {0, 1}.
  const OperandsFieldsMap OpFormatMapper;
  // Pointers on the definition of the base field of the tree.
  // Currently used to recover the size of the instruction (Composite or not).
  const MCFormatField *const BaseField;

public:
  MCFormatDesc(unsigned Opcode, bool IsComposite, bool HasMultipleSlotOptions,
               const SlotsFieldsMap &Slots, OperandsFieldsMap OpFormatMapper,
               const MCFormatField *BaseField)
      : Kind(IsComposite ? FK_Packet : FK_Instr),
        HasMultipleSlotOptions(HasMultipleSlotOptions), Opcode(Opcode),
        SlotsMap(Slots), OpFormatMapper(OpFormatMapper), BaseField(BaseField) {
    // IsComposite is set only when the instruction is used to define supported
    // VLIW type, whereas HasMultipleSlotOptions is set only when the Pseudo
    // instruction represents multiple target instruction. These conditions are
    // mutually exclusive.
    assert(!(IsComposite && HasMultipleSlotOptions));
  };

  /// Return whether the format describes a Packet (Composite Instruction) or a
  /// sub-instruction.
  MCFormatKind getKind() const { return Kind; }
  bool isPacket() const { return Kind == FK_Packet; }
  bool isSubInst() const { return Kind == FK_Instr; }
  bool hasMultipleSlotOptions() const { return HasMultipleSlotOptions; }

  /// Return the opcode of the format
  inline unsigned getOpcode() const { return Opcode; }

  /// Returns the size of the encoding of the current format (in bits)
  /// NOTE: Same as asking the size to the MCInstrDesc except the fact that
  /// the MCInstrDesc returns the size in bytes.
  inline unsigned getFormatSize() const { return BaseField->getSize(); }

  /// Returns a pair of Offset of the Slot Kind, indexed on the High bits:
  ///
  /// From MSB/High = Big-endian indexing
  ///    MSB              |    Slot     |                   LSB
  ///     |---------------|-------------|------------------->|
  ///     |               |<= numBits =>|                    |
  ///     0 ============> L ==========> R               FormatSize-1
  ///
  /// GlobalOffsets = {L, R}
  MCFormatField::GlobalOffsets
  getSlotOffsetsHiBit(const MCSlotKind &Kind) const {
    assert(!HasMultipleSlotOptions);
    assert(SlotsMap.find(Kind) != SlotsMap.end());
    return SlotsMap.at(Kind)->getOffsets();
  }

  /// Returns a pair of Offset of the Slot Kind, indexed on the Low bits:
  ///
  /// From LSB/Low = little-endian indexing
  ///    MSB              |    Slot     |                   LSB
  ///     |---------------|-------------|------------------->|
  ///     |               |<= numBits =>|                    |
  /// FormatSize-1        L <========== R <================= 0
  ///
  /// GlobalOffsets = {L, R}
  MCFormatField::GlobalOffsets
  getSlotOffsetsLoBit(const MCSlotKind &Kind) const {
    assert(!HasMultipleSlotOptions);
    using GlobalOffsets = typename MCFormatField::GlobalOffsets;
    GlobalOffsets HiOffsets = SlotsMap.at(Kind)->getOffsets();
    // Offsets are stored as big-endian indexes.
    // We need to make a transformation:
    GlobalOffsets LoOffsets;
    LoOffsets.LeftOffset = getFormatSize() - HiOffsets.LeftOffset - 1;
    LoOffsets.RightOffset = getFormatSize() - HiOffsets.RightOffset - 1;
    return LoOffsets;
  }

  /// Returns the list of field(s) covering the MCOperand Idx.
  /// Pre-condition: Idx must be valid. Otherwise, it triggers an assertion.
  ArrayRef<const MCFormatField *> getFieldsCoveredByOpIdx(unsigned Idx) const {
    assert(!HasMultipleSlotOptions);
    return OpFormatMapper.at(Idx);
  }
};

} // end namespace llvm

namespace llvm {

// Downcasting MCFormatDesc -> AIEPacketFmt can be done iff isComposite is
// set to true for a given instruction in TableGen.
class AIEPacketFormat : public MCFormatDesc {
public:
  /// Returns a given Slot Kind of the Packet based on Idx, the index of the
  /// MCOperand.
  /// NOTE: Normally, the index of the MCOperand is the same than the position
  /// of the slot in the VLIW, e.g. for the packet:
  /// I128_LDA_LDB_NOP_ALU_LNG_NOP_VEC_ALL
  /// - LDA is at the position 0
  /// - LDB is at the position 1
  /// ...
  /// - VEC_ALL is at the position 4.
  /// This is based on the ordering of the operands in the TableGen definition.
  const MCSlotKind getSlot(unsigned Idx) const;


  /// Returns the number of slots in the Packet Format
  inline unsigned getNumberOfSlot() const;

  /// Returns the Offset of the slot Idx/Kind, indexed on the Low (respect.
  /// High) bits:
  ///
  /// From MSB/High = Big-endian indexing
  ///    MSB              |    Slot     |                   LSB
  ///     |---------------|-------------|------------------->|
  ///     |               |<= numBits =>|                    |
  ///     0 ==========> Offset                          FormatSize-1
  ///
  /// From LSB/Low = little-endian indexing
  ///    MSB              |    Slot     |                   LSB
  ///     |---------------|-------------|------------------->|
  ///     |               |<= numBits =>|                    |
  /// FormatSize-1                    Offset <============== 0
  unsigned getSlotOffsetLoBit(unsigned Idx) const;
  unsigned getSlotOffsetHiBit(unsigned Idx) const;
  unsigned getSlotOffsetLoBit(const MCSlotKind Kind) const;
  unsigned getSlotOffsetHiBit(const MCSlotKind Kind) const;

  static bool classof(const MCFormatDesc *FormatDesc) {
    return FormatDesc->isPacket();
  }
};

// Downcasting MCFormatDesc -> AIEInstFmt can be done iff isComposite is set
// to false for a given instruction in TableGen.
class AIEInstFormat : public MCFormatDesc {
public:
  /// Returns true whether the instruction has a valid slot
  bool hasSingleSlot() const;
  /// Returns the Slot Kind of the instruction
  const MCSlotKind getSlot() const;

  /// Returns the Offset of the slot, indexed on the Low (respect. High) bits:
  ///
  /// From MSB/High = Big-endian indexing
  ///    MSB              |    Slot     |                   LSB
  ///     |---------------|-------------|------------------->|
  ///     |               |<= numBits =>|                    |
  ///     0 ==========> Offset                           InstSize-1
  ///
  /// From LSB/Low = little-endian indexing
  ///    MSB              |    Slot     |                   LSB
  ///     |---------------|-------------|------------------->|
  ///     |               |<= numBits =>|                    |
  /// InstSize-1                      Offset <============== 0
  unsigned getSlotOffsetLoBit() const;
  unsigned getSlotOffsetHiBit() const;

  static bool classof(const MCFormatDesc *FormatDesc) {
    return FormatDesc->isSubInst();
  }
};
class AIEBaseMCFormats {
public:
  virtual ~AIEBaseMCFormats() = default;

  /// Retrieve the Format Description, based on the opcode.
  /// \pre isSupportedInstruction(Opcode)
  virtual const MCFormatDesc &getFormatDesc(unsigned int Opcode) const;

  /// Returns whether the instruction is supported (i.e there is an entry in
  /// the table for this particular instruction).
  virtual bool isSupportedInstruction(unsigned int Opcode) const;

  /// Returns true if the instruction is a MultiSlotPseudo instruction
  virtual bool hasMultipleSlotOptions(unsigned Opcode) const;

  /// Returns Format Description, index based on the opcode.
  virtual std::optional<unsigned int>
  getFormatDescIndex(unsigned int Opcode) const = 0;

  /// Returns a set of opcode for a given multi-slot pseudo intr, for an
  /// unsupported opcode it returns an empty set
  virtual const std::vector<unsigned int> *
  getAlternateInstsOpcode(unsigned int Opcode) const = 0;

  /// Based on the Opcode and TargetSlot return the real instructions.
  /// The Opcode is expected to be of Multi-Slot Pseudo instruction.
  std::optional<const unsigned>
  getMaterializableOpcodeForSlot(unsigned int Opcode,
                                 MCSlotKind TargetSlot) const;

  /// Retrieve directly the Packet Format, based on the opcode.
  /// \pre isPacket()
  const AIEPacketFormat &getPacketFormat(unsigned int Opcode) const;

  /// Retrieve directly the Sub-Inst Format, based on the opcode
  /// \pre isSubInst()
  const AIEInstFormat &getSubInstFormat(unsigned int Opcode) const;

  virtual const MCSlotKind getSlotKind(unsigned int Opcode) const;

  virtual const std::vector<MCSlotKind>
  getSlotAlternatives(unsigned int Opcode) const;

  virtual const MCSlotInfo *getSlotInfo(const MCSlotKind Kind) const = 0;

  virtual const MCFormatDesc *getMCFormats() const = 0;

  virtual const PacketFormats &getPacketFormats() const = 0;

protected:
  /// Check if the Instruction is indeed into the Tables.
  void checkInstructionIsSupported(unsigned int Opcode) const;
};

class AIEMCFormats : public AIEBaseMCFormats {
public:
  const std::vector<unsigned int> *
  getAlternateInstsOpcode(unsigned int Opcode) const override;
  std::optional<unsigned int>
  getFormatDescIndex(unsigned int Opcode) const override;
  const MCSlotInfo *getSlotInfo(const MCSlotKind Kind) const override;
  const MCFormatDesc *getMCFormats() const override;
  const PacketFormats &getPacketFormats() const override;
};

class AIE2MCFormats : public AIEBaseMCFormats {
public:
  const std::vector<unsigned int> *
  getAlternateInstsOpcode(unsigned int Opcode) const override;
  std::optional<unsigned int>
  getFormatDescIndex(unsigned int Opcode) const override;
  const MCSlotInfo *getSlotInfo(const MCSlotKind Kind) const override;
  const MCFormatDesc *getMCFormats() const override;
  const PacketFormats &getPacketFormats() const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_AIE_MCTARGETDESC_AIEMCFORMATS_H
