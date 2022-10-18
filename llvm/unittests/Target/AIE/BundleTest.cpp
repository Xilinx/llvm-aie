//===- BundleTest.cpp -------------------------------------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "AIEInstrInfo.h"
#include "MCTargetDesc/AIEFormat.h"
#include "MCTargetDesc/AIEMCFormats.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/TargetOpcodes.h"
#include "llvm/MC/TargetRegistry.h"
#include "gtest/gtest.h"
#include <type_traits>

#include "AIEBundle.h"

using namespace llvm;

// Test the bundle logic with our own 'architecture' by overriding
// the format tables and the slot kind methods
constexpr const MCSlotKind SLOTALU(0);
constexpr const MCSlotKind SLOTMV(1);
constexpr const MCSlotKind SLOTST(2);
constexpr const MCSlotKind SLOTLNG(3);

const MCSlotKind Slots[] = {
    SLOTALU, SLOTMV, SLOTLNG, SLOTST, SLOTALU, SLOTMV, SLOTLNG, SLOTST,
};

const VLIWFormat FormatData[] = {
    {0b1100, "ALUMV", {&Slots[0], &Slots[2]}, 0b11},   // in slotkind order
    {0b0011, "LNGST", {&Slots[2], &Slots[4]}, 0b1100}, // not in slotkind order
    // Superslot, only use if necessary
    {0b1111, "ALL", {&Slots[4], &Slots[8]}, 0b1111},
    {0, nullptr, {}, 0}};
const PacketFormats MyFormats{FormatData};

constexpr MCSlotInfo SlotInfos[4] = {{"ALU", 1, 0b0001, 0},
                                     {"MV", 1, 0b0010, 0},
                                     {"ST", 1, 0b0100, 0},
                                     {"LNG", 1, 0b1000, 0}};

// Stay clear of standard opcodes
static const int FirstOpcode = 1000;
class MockInstr {
public:
  unsigned Opcode = 0;

  unsigned getOpcode() const { return Opcode; }
};
using MockBundle = AIE::Bundle<MockInstr>;

class MockMCFormats : public AIEMCFormats {
  const MCSlotInfo *getSlotInfo(const MCSlotKind Kind) const override {
    return &SlotInfos[Kind];
  }
  const PacketFormats &getPacketFormats() const override { return MyFormats; }

  // Real instructions are constructed 1-1 with slotkinds
  const MCSlotKind getSlotKind(unsigned int Opcode) const override {
    switch (Opcode) {
    case FirstOpcode:
    case FirstOpcode + 1:
    case FirstOpcode + 2:
    case FirstOpcode + 3:
      return Opcode - FirstOpcode;
    default:
      break;
    }
    return {};
  }

  const std::vector<MCSlotKind>
  getSlotAlternatives(unsigned int Opcode) const override {
    std::vector<MCSlotKind> SlotKind;
    switch (Opcode) {
    case FirstOpcode:
    case FirstOpcode + 1:
    case FirstOpcode + 2:
    case FirstOpcode + 3:
      SlotKind.push_back(Opcode - FirstOpcode);
      return SlotKind;
    default:
      break;
    }
    return {};
  }
};

class MockTII : public AIEInstrInfo {
public:
  const PacketFormats &getPacketFormats() const override { return MyFormats; }
  MockTII() {
    for (int i = 0; i < 10; i++) {
      MyInstrs[i].Opcode = FirstOpcode + i;
    }
    getMeta()->Opcode = TargetOpcode::IMPLICIT_DEF;
  }
  const MCSlotInfo *getSlotInfo(const MCSlotKind Kind) const override {
    return &SlotInfos[Kind];
  }
  MCSlotKind getSlotKind(unsigned Opcode) const override {
    for (unsigned s = 0; s < 10; s++) {
      if (Opcode == MyInstrs[s].getOpcode()) {
        return s;
      }
    }
    return {};
  }
  MockInstr *operator[](unsigned i) { return &MyInstrs[i]; }
  MockInstr *getMeta() { return &MyInstrs[9]; }

private:
  MockInstr MyInstrs[10];
};

TEST(Bundle, Construct) {
  MockTII TII;
  MockMCFormats FormatInterface;
  MockBundle Empty(&FormatInterface);

  unsigned Zero = 0;
  EXPECT_TRUE(Empty.empty());
  EXPECT_EQ(Empty.size(), Zero);

  // Empty bundles can accept any instruction
  EXPECT_TRUE(Empty.canAdd(TII[SLOTALU]));
  EXPECT_TRUE(Empty.canAdd(TII[SLOTMV]));
  EXPECT_TRUE(Empty.canAdd(TII[SLOTST]));
  EXPECT_TRUE(Empty.canAdd(TII[SLOTLNG]));
}

TEST(Bundle, SlotConflict) {
  MockTII TII;
  MockMCFormats FormatInterface;
  MockBundle B(&FormatInterface);

  auto *Alu = TII[SLOTALU];
  auto *Mv0 = TII[SLOTMV];

  EXPECT_TRUE(B.canAdd(Alu));
  B.add(Alu);
  EXPECT_FALSE(B.canAdd(Alu));
  EXPECT_TRUE(B.canAdd(Mv0));
}

TEST(Bundle, Insert) {
  MockTII TII;
  MockMCFormats FormatInterface;
  MockBundle B(&FormatInterface);

  auto *St = TII[SLOTST];
  auto *Lng = TII[SLOTLNG];

  EXPECT_TRUE(B.canAdd(St));
  B.add(St);
  EXPECT_EQ(B.size(), unsigned(1));
  EXPECT_TRUE(B.canAdd(Lng));
  B.add(Lng);
  EXPECT_EQ(B.size(), unsigned(2));

  auto *Fmt = B.getFormatOrNull();
  ASSERT_TRUE(Fmt);
  EXPECT_EQ(Fmt->Name, "LNGST");

  std::vector<MockInstr *> Ref{St, Lng};
  EXPECT_EQ(B.getInstrs(), Ref);
  EXPECT_EQ(B.at(SLOTLNG), Lng);
  EXPECT_EQ(B.at(SLOTST), St);

  B.clear();
  EXPECT_TRUE(B.empty());
  EXPECT_TRUE(B.canAdd(St));
  EXPECT_TRUE(B.canAdd(Lng));
}

TEST(Bundle, Standalone) {
  MockTII TII;
  MockMCFormats FormatInterface;
  MockBundle B(&FormatInterface);

  auto *St = TII[SLOTST];
  auto *Lng = TII[SLOTLNG];
  auto *Unknown = TII[4];

  // Create a standalone bundle
  EXPECT_TRUE(B.canAdd(Unknown));
  B.add(Unknown);
  EXPECT_EQ(B.size(), unsigned(1));
  EXPECT_TRUE(B.isStandalone());

  // Test that we can't add a valid instruction in a standalone bundle
  EXPECT_FALSE(B.canAdd(Lng));

  // Test that we can't add another valid instruction in a standalone bundle
  EXPECT_FALSE(B.canAdd(St));

  // Reset the bundle
  B.clear();

  EXPECT_TRUE(B.canAdd(Lng));
  B.add(Lng);
  EXPECT_EQ(B.size(), unsigned(1));

  // Test that we can't add a not valid instruction in a not standalone bundle
  EXPECT_FALSE(B.canAdd(Unknown));

  // Test that we can add a valid instruction in a not standalone bundle after
  // trying to add an invalid one (no internal modification of the state)
  EXPECT_TRUE(B.canAdd(St));
  B.add(St);
  EXPECT_EQ(B.size(), unsigned(2));

  // Test that we still can't add a not valid instruction in a not standalone
  // bundle
  EXPECT_FALSE(B.canAdd(Unknown));
}

TEST(Bundle, Meta) {
  MockTII TII;
  MockMCFormats FormatInterface;
  MockBundle B(&FormatInterface);

  auto *St = TII[SLOTST];
  auto *Lng = TII[SLOTLNG];
  auto *Meta = TII.getMeta();

  // We can always add meta instructions
  EXPECT_TRUE(B.canAdd(Meta));
  EXPECT_TRUE(B.canAdd(St));
  B.add(St);
  EXPECT_TRUE(B.canAdd(Meta));
  EXPECT_TRUE(B.canAdd(Lng));
  B.add(Lng);
  EXPECT_TRUE(B.canAdd(Meta));
  B.add(Meta);
  EXPECT_TRUE(B.canAdd(Meta));
  B.add(Meta);

  // Meta instructions don't influence bundle size
  EXPECT_EQ(B.size(), size_t(2));
  EXPECT_EQ(B.getMetaInstrs().size(), size_t(2));

  // Clearing a bundle also clear the meta instructions
  B.clear();
  EXPECT_TRUE(B.empty());
  EXPECT_TRUE(B.getMetaInstrs().empty());

  // Meta instructions don't block real ones
  B.add(Meta);
  B.add(Meta);
  EXPECT_TRUE(B.canAdd(St));
  EXPECT_TRUE(B.canAdd(Lng));
  B.add(Lng);
  EXPECT_TRUE(B.canAdd(St));
  B.add(St);
}
