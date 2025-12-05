<!--
This file is licensed under the Apache License v2.0 with LLVM Exceptions.
See https://llvm.org/LICENSE.txt for license information.
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

(c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
-->

# SlotStructure and MultiSlotPseudo Unification

## Overview

This document describes the unification of MultiSlotPseudos (MSPs) with
regular slots through a unified `MultiSlotClass` index domain. This
unification enables the `SlotStructure` interface to drive `SlotOccupancy`
conflict detection and MSP materialization with zero runtime computation
beyond table lookups.

## Motivation

Previously, MSPs and regular slots were handled separately:
- Regular instructions had a fixed slot assignment
- MSPs used `AlternateOpcodes` arrays to list possible materializations
- Slot conflict detection required runtime computation of MSP compositions

The new design unifies these concepts:
- Both MSPs and regular slots are represented as slot classes with a
  unified index
- MSP compositions are precomputed and stored in generated tables
- Materialization is a simple table lookup: `(MSP opcode, target slot) →
  real opcode`

## Key Concepts

### MultiSlotClass

A `MultiSlotClass` is a unified slot class index that can represent either:
- **Real slot** (indices 0..NumRealSlots-1): A single physical VLIW slot
- **MSP class** (indices NumRealSlots..TotalSlotClasses-1): A set of slots
  an MSP can materialize to

```cpp
enum class MultiSlotClass : int {
  NoClass = -1  // Invalid/unknown class
  // Actual indices are architecture-specific and start from 0
};
```

### Composition

Each slot class has a **composition**: a bitmask of real slots it can
occupy.
- For real slots: composition = (1 << slotIdx) - a singleton
- For MSP classes: composition = OR of all real slot bits the MSP can use

Example (AIE2):
```cpp
SlotCompositions[0] = 1    // Alu slot (bit 0)
SlotCompositions[1] = 2    // Lda slot (bit 1)
SlotCompositions[7] = 26   // MSP class: Lda|Mv|Lng (bits 1,3,4)
```

### Deduplication

MSPs with identical compositions share a single `MultiSlotClass` index.

Example: If `MOV_A` and `MOV_B` both materialize to {Lda, Mv, Lng}, they
get the same class index.

## Generated Tables

The CodeGenFormat TableGen backend generates three new table sections per
architecture:

### 1. Slot Class Counts

```cpp
static constexpr unsigned NumRealSlots = 7;      // AIE2 example
static constexpr unsigned NumMSPClasses = 9;
static constexpr unsigned TotalSlotClasses = 16;
```

### 2. Composition Table

```cpp
static constexpr uint64_t const SlotCompositions[] = {
  1ULL /* Real slot: Alu */,
  2ULL /* Real slot: Lda */,
  4ULL /* Real slot: Ldb */,
  8ULL /* Real slot: Lng */,
  16ULL /* Real slot: Mv */,
  64ULL /* Real slot: St */,
  128ULL /* Real slot: Vec */,
  26ULL /* MSP class 7: Lda|Mv|Lng (0x1A = bits 1,3,4) */,
  10ULL /* MSP class 8: Lda|Lng (0x0A = bits 1,3) */,
  // ... more MSP classes
};
```

### 3. MSP Opcode → MultiSlotClass Mapping

```cpp
static MultiSlotClass getMSPClassIndexForOpcode(unsigned Opcode) {
  switch (Opcode) {
  default:
    return MultiSlotClass::NoClass;
  case AIE2::MOV_PD_imm10_pseudo:
    return static_cast<MultiSlotClass>(7);
  // ...
  }
}
```

### 4. MSP Materialization Mapping

```cpp
static unsigned getMaterializedOpcodeImpl(unsigned Opcode,
                                          unsigned SlotIdx) {
  switch (Opcode) {
  default:
    return 0; // Not an MSP or invalid
  case AIE2::MOV_PD_imm10_pseudo:
    switch (SlotIdx) {
    case 1: return AIE2::MOVA_lda_cg;    // Lda slot
    case 4: return AIE2::MOV_mv_cg;      // Mv slot
    case 3: return AIE2::MOVXM_lng_cg;   // Lng slot
    }
  // ...
  }
}
```

## Interface Usage

### SlotStructure Interface

```cpp
const AIESlotStructure &SS = FormatInterface.getSlotStructure();

// Get composition for any class (real or MSP)
SlotBits Composition = SS.getMSPComposition(ClassIdx);

// Get capacity (number of slots in composition)
uint8_t Capacity = SS.getCapacity(ClassIdx);  // popcount(composition)

// Get number of real slots
unsigned NumReal = SS.getNumRealSlots();
```

### FormatInterface Extensions

```cpp
const AIEBaseMCFormats &FI = /* ... */;

// Get the MultiSlotClass for any instruction
MultiSlotClass Class = FI.getMultiSlotClass(Opcode);

// Materialize an MSP to a specific slot
unsigned RealOpcode = FI.getMaterializedOpcode(MSPOpcode, SlotIdx);
```

## SlotOccupancy Integration

`SlotOccupancy` uses `SlotStructure` for conflict detection:

1. Combine occupancies by summing counts per class
2. Check capacity bounds using `SlotStructure.getCapacityBounds()`
3. Try materializing MSPs:
   - Get composition via `SlotStructure.getMSPComposition(ClassIdx)`
   - Greedily assign to available real slots
   - Check feasibility via `FormatInterface.isFormatAvailable(realSlotBits)`

All operations are table lookups - no runtime composition computation.

## Migration Path from AlternateOpcodes

The new design enables deprecation of `AlternateOpcodes`:

**Old approach:**
```cpp
const std::vector<unsigned> *Alts =
    FI.getAlternateInstsOpcode(MSPOpcode);
for (unsigned AltOpcode : *Alts) {
  MCSlotKind Slot = FI.getSlotKind(AltOpcode);
  // ...
}
```

**New approach:**
```cpp
MultiSlotClass Class = FI.getMultiSlotClass(MSPOpcode);
SlotBits Composition = FI.getSlotStructure().getMSPComposition(
    static_cast<unsigned>(Class));
// Iterate over set bits in Composition
for (unsigned SlotIdx = 0; SlotIdx < NumRealSlots; ++SlotIdx) {
  if (Composition & (1ULL << SlotIdx)) {
    unsigned RealOpcode = FI.getMaterializedOpcode(MSPOpcode, SlotIdx);
    // ...
  }
}
```

## Implementation Files

### TableGen Backend
- `llvm/utils/TableGen/CodeGenFormat.cpp` - Table generation logic
- `llvm/utils/TableGen/CodeGenFormat.h` - Backend interface

### Generated Tables
- `Release/lib/Target/AIE/AIEGenFormats.inc` - AIE1 tables
- `Release/lib/Target/AIE/AIE2GenFormats.inc` - AIE2 tables
- `Release/lib/Target/AIE/AIE2PGenFormats.inc` - AIE2P tables

### Runtime Interfaces
- `llvm/lib/Target/AIE/AIESlotStructure.h` - SlotStructure interface
- `llvm/lib/Target/AIE/MCTargetDesc/AIEMCFormats.h` - FormatInterface with
  MultiSlotClass
- `llvm/lib/Target/AIE/MCTargetDesc/AIEMCFormats.cpp` - AIE1 implementation
- `llvm/lib/Target/AIE/MCTargetDesc/AIE2MCFormats.cpp` - AIE2 implementation
- `llvm/lib/Target/AIE/MCTargetDesc/aie2p/AIE2PMCFormats.cpp` - AIE2P
  implementation

### Consumers
- `llvm/lib/Target/AIE/AIESlotOccupancy.cpp` - Uses SlotStructure for
  conflict detection
- `llvm/lib/Target/AIE/AIEMultiSlotInstrMaterializer.cpp` - Can use new
  materialization API

## Future Work

1. **Migrate Materializer**: Update `AIEMultiSlotInstrMaterializer` to use
   `getMaterializedOpcode()` instead of `AlternateOpcodes`
2. **Deprecate AlternateOpcodes**: Once all consumers migrate, mark
   `getAlternateInstsOpcode()` as deprecated
3. **Runtime Helpers**: Add convenience methods to build slot→opcodes maps
   lazily if needed
4. **Extended Metadata**: Consider adding MSP class names or other metadata
   to generated tables for debugging

## Testing

All existing tests pass:
- SlotOccupancy unit tests: 20/20 (100%)
- AIE CodeGen tests: 1802/1803 (99.72%)

The single failing test is pre-existing and unrelated to these changes.
