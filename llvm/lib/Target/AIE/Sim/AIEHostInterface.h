//===- AIEHostInterface.h - Environment port of an AIE core -----*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
/// \file
/// The abstract port through which an executing core reaches everything that is
/// not its own register file.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_AIE_SIM_AIEHOSTINTERFACE_H
#define LLVM_LIB_TARGET_AIE_SIM_AIEHOSTINTERFACE_H

#include "llvm/ADT/APInt.h"
#include "llvm/ADT/ArrayRef.h"
#include <cstdint>

namespace llvm {
namespace AIESim {

/// Outcome of an access that leaves the core datapath.
enum class PortStatus {
  Ok,
  /// The resource is not ready. The bundle is retried with no architectural
  /// change, so the embedder owns all scheduling and the core never blocks.
  Stall,
  /// Unmapped, misaligned, or not provided by this embedder.
  Fault,
};

/// Everything a core touches outside its own registers.
///
/// llvm-aie-run backs this with flat memory and refusing ports; an array model
/// backs it with tiles, DMAs and a stream switch. The executor cannot tell the
/// difference, so neither embedder can be surprised by hidden state.
class AIEHostInterface {
public:
  virtual ~AIEHostInterface();

  /// Program bytes visible from \p Addr, at least a full bundle where one
  /// starts there. An empty result means \p Addr is not mapped.
  virtual ArrayRef<uint8_t> fetch(uint64_t Addr) = 0;

  virtual PortStatus load(uint64_t Addr, unsigned NumBytes, APInt &Out) = 0;
  virtual PortStatus store(uint64_t Addr, unsigned NumBytes,
                           const APInt &Value) = 0;

  /// Ports an embedder need not have. Refusing is a fault rather than a silent
  /// no-op, so a design that reaches one cannot appear to run correctly.
  virtual PortStatus acquireLock(unsigned Id, int32_t Value) {
    return PortStatus::Fault;
  }
  virtual PortStatus releaseLock(unsigned Id, int32_t Value) {
    return PortStatus::Fault;
  }
  virtual PortStatus readStream(unsigned Port, APInt &Out) {
    return PortStatus::Fault;
  }
  virtual PortStatus writeStream(unsigned Port, const APInt &Value,
                                 bool TLast) {
    return PortStatus::Fault;
  }
  virtual PortStatus readCascade(APInt &Out) { return PortStatus::Fault; }
  virtual PortStatus writeCascade(const APInt &Value) {
    return PortStatus::Fault;
  }

  /// Observation side-channels. These default to a no-op rather than a fault
  /// because nothing in the datapath can read them back: a design that reaches
  /// one with no sink attached still computes the same result.
  virtual void putChar(char C) {}

  /// Raise trace event \p Id, 0..3. The core cannot block on this and cannot
  /// observe whether anything was listening.
  virtual void raiseEvent(unsigned Id) {}
};

} // namespace AIESim
} // namespace llvm

#endif // LLVM_LIB_TARGET_AIE_SIM_AIEHOSTINTERFACE_H
