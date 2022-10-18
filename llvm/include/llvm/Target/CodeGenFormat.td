//===- CodeGenFormat.td - TableGen interface of CodeGenFormat ---*- tablegen -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file defines the (target-independent) interfaces for CodeGenFormat,
// despite the fact that it was made for AIE primarily.
//
//===---------------------------------------------------------------------------===//

// This class represents an abstraction of a slot, defined as an Operand
class InstSlot<string slotname, int Size, bit isDefault=false>  : Operand<OtherVT>
{
  assert !if(!not(isDefault), !gt(Size, 0), !ge(Size, 0)),
  "Size of the Slot must be > 0 if the slot isn't the default one"
  ", current size being: " # Size;

  assert !ne(slotname, ""), "Empty string is not a valid argument for slotname";

  string Namespace = "";

  // Name of the slot.
  // This name should be unique for each slot instantiated.
  // NOTE: It is used to create the enumeration constant of the slot by writing
  // "SLOT_" + capitalize(SlotName)
  string SlotName = slotname;

  // Size (in bits) of the Slot
  int SlotSize = Size;

  // Name of the field to find in the encoding hierarchy
  // If this string is empty, then the Backend will look by default for the
  // combination of SlotName + "_base" to find the specific field describing
  // the slot.
  string FieldToFind = "";

  // Indicate if the Slot is a default slot, meaning it is just a placeholder,
  // there will be no info generated in the C++ code for this slot.
  // CodeGenFormat needs exactly one default slot by set of slots in a given
  // Namespace.
  bit isDefaultSlot = isDefault;
}

class CodeGenFormat
{
  // Metadata indicating the name of the C++ class to print when emitting the
  // Format Tables. If this is equal to "", then the default name used will be
  // the value of the concatenation of (Target, "FormatDesc"). Different
  // variant of the same Target could use the same class.
  string FormatClassEmitted = "";
}

// TableGen Class defining metadata about the format infos
class InstFormat
{
  // Metadata defining the Slot of the current Instruction
  // If the instruction is a sub-instruction (of a composite VLIW instruction),
  // then this field must contain the information on the Kind of
  // the Slot.
  InstSlot Slot;

  // Metadata expressing if the instruction is actually a Composite (VLIW)
  // instruction i.e. the instruction defines a Packet Format.
  // If set to true, Slot informations (of the Composite Instruction) will be
  // retrieved from the operands of the Record itself and not from the Slot
  // attribute.
  bit isComposite = false;

  // Metadata defining the mapping between a NOP instruction and its according
  // slot.
  // FIXME: Every slot (except the default one) should be mapped to a NOP
  // instruction. There is currently no checks in the Backend to ensure this is
  // the case.
  bit isSlotNOP = false;
}