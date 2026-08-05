//===- AIEMCTargetDesc.cpp - AIE Target Descriptions ------------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2026 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// This file provides AIE specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "AIEMCTargetDesc.h"
#include "AIE1AsmBackend.h"
#include "AIE2AsmBackend.h"
#include "AIE2MCTargetDesc.h"
#include "AIEMCAsmInfo.h"
#include "AIEMCELFStreamer.h"
#include "AIETargetAsmStreamer.h"
#include "AIETargetELFStreamer.h"
#include "InstPrinter/AIE2InstPrinter.h"
#include "InstPrinter/AIE2PInstPrinter.h"
#include "InstPrinter/AIE2PSInstPrinter.h"
#include "InstPrinter/AIEInstPrinter.h"
#include "aie2p/AIE2PAsmBackend.h"
#include "aie2p/AIE2PMCTargetDesc.h"
#include "aie2ps/AIE2PSAsmBackend.h"
#include "aie2ps/AIE2PSMCTargetDesc.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCDwarf.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "AIEGenInstrInfo.inc"

#define FUNCUNIT_REPRESENTATION(x) (x)
#define GET_SUBTARGETINFO_MC_DESC
#include "AIEGenSubtargetInfo.inc"

#define NoSchedModel NoSchedModelAIE2
#define GET_SUBTARGETINFO_MC_DESC
#include "AIE2GenSubtargetInfo.inc"
#undef NoSchedModel

#define NoSchedModel NoSchedModelAIE2P
#define GET_SUBTARGETINFO_MC_DESC
#include "AIE2PGenSubtargetInfo.inc"
#undef NoSchedModel

#define NoSchedModel NoSchedModelAIE2PS
#define GET_SUBTARGETINFO_MC_DESC
#include "AIE2PSGenSubtargetInfo.inc"
#undef NoSchedModel

#define GET_REGINFO_MC_DESC
#include "AIEGenRegisterInfo.inc"

static MCInstrInfo *createAIEMCInstrInfo() {
  auto *X = new MCInstrInfo();
  InitAIEMCInstrInfo(X);
  return X;
}

// AIE1 is a conceptual subtarget of AIE
static MCRegisterInfo *createAIE1MCRegisterInfo(const Triple &TT) {
  auto *X = new MCRegisterInfo();
  InitAIEMCRegisterInfo(X, AIE::lr);
  return X;
}

// This function creates the relevant register info, dependent
// on the subtarget. The 'main' target aie is defined in this file,
// the subtargets in the target-specific cpp file
static MCRegisterInfo *createAIEMCRegisterInfo(const Triple &TT) {
  switch (TT.getArch()) {
  case Triple::aie:
    return createAIE1MCRegisterInfo(TT);
  case Triple::aie2:
    return createAIE2MCRegisterInfo(TT);
  case Triple::aie2p:
    return createAIE2PMCRegisterInfo(TT);
  case Triple::aie2ps:
    return createAIE2PSMCRegisterInfo(TT);
  default:
    llvm_unreachable("Unknown AIE target");
  }

  return nullptr;
}

static MCSubtargetInfo *createAIEMCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
  if (TT.getArch() == Triple::aie2)
    return createAIE2MCSubtargetInfoImpl(TT, CPU, CPU, FS);
  else if (TT.getArch() == Triple::aie2p)
    return createAIE2PMCSubtargetInfoImpl(TT, CPU, CPU, FS);
  else if (TT.getArch() == Triple::aie2ps)
    return createAIE2PSMCSubtargetInfoImpl(TT, CPU, CPU, FS);
  return createAIEMCSubtargetInfoImpl(TT, CPU, CPU, FS);
}

static MCAsmInfo *createAIEMCAsmInfo(const MCRegisterInfo &MRI,
                                     const Triple &TT,
                                     const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new AIEMCAsmInfo(TT);

  // Initial state of the frame pointer is SP.
  MCCFIInstruction Inst = MCCFIInstruction::cfiDefCfa(nullptr, AIE::SP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCInstPrinter *createAIEMCInstPrinter(const Triple &TT,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {
  if (TT.getArch() == Triple::aie2)
    return new AIE2InstPrinter(MAI, MII, MRI);
  else if (TT.getArch() == Triple::aie2p)
    return new AIE2PInstPrinter(MAI, MII, MRI);
  else if (TT.getArch() == Triple::aie2ps)
    return new AIE2PSInstPrinter(MAI, MII, MRI);
  return new AIEInstPrinter(MAI, MII, MRI);
}

static MCTargetStreamer *
createAIEObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  return new AIETargetELFStreamer(S, STI);
}

static MCTargetStreamer *createTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS,
                                                 MCInstPrinter *InstPrint) {
  return new AIETargetAsmStreamer(S, OS);
}

static MCAsmBackend *createAIEAsmBackend(const Target &T,
                                         const MCSubtargetInfo &STI,
                                         const MCRegisterInfo &MRI,
                                         const MCTargetOptions &Options) {
  const Triple &TT = STI.getTargetTriple();
  uint8_t OSABI = MCELFObjectTargetWriter::getOSABI(TT.getOS());
  switch (TT.getArch()) {
  case Triple::aie:
    return new AIE1AsmBackend(STI, OSABI, Options);
  case Triple::aie2:
    return new AIE2AsmBackend(STI, OSABI, Options);
  case Triple::aie2p:
    return new AIE2PAsmBackend(STI, OSABI, Options);
  case Triple::aie2ps:
    return new AIE2PSAsmBackend(STI, OSABI, Options);
  default:
    llvm_unreachable("Unsupported AIE target");
  }
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeAIETargetMC() {
  for (Target *T : {&getTheAIETarget(), &getTheAIE2Target(),
                    &getTheAIE2PTarget(), &getTheAIE2PSTarget()}) {
    // Register the MC asm info.
    RegisterMCAsmInfoFn X(*T, createAIEMCAsmInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createAIEMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T, createAIEMCSubtargetInfo);

    // Register the MCInstPrinter
    TargetRegistry::RegisterMCInstPrinter(*T, createAIEMCInstPrinter);
    // Support for ELF output files
    TargetRegistry::RegisterMCAsmBackend(*T, createAIEAsmBackend);

    TargetRegistry::RegisterObjectTargetStreamer(*T,
                                                 createAIEObjectTargetStreamer);
    // Registers symbols referenced from a bundle's slots; see AIEMCELFStreamer.
    TargetRegistry::RegisterELFStreamer(*T, createAIEELFStreamer);
    // Support for ASM output files
    TargetRegistry::RegisterAsmTargetStreamer(*T, createTargetAsmStreamer);
  }
  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(getTheAIETarget(), createAIEMCInstrInfo);
  TargetRegistry::RegisterMCInstrInfo(getTheAIE2Target(),
                                      createAIE2MCInstrInfo);
  TargetRegistry::RegisterMCInstrInfo(getTheAIE2PTarget(),
                                      createAIE2PMCInstrInfo);
  TargetRegistry::RegisterMCInstrInfo(getTheAIE2PSTarget(),
                                      createAIE2PSMCInstrInfo);

  TargetRegistry::RegisterMCCodeEmitter(getTheAIETarget(),
                                        createAIEMCCodeEmitter);
  TargetRegistry::RegisterMCCodeEmitter(getTheAIE2Target(),
                                        createAIE2MCCodeEmitter);
  TargetRegistry::RegisterMCCodeEmitter(getTheAIE2PTarget(),
                                        createAIE2PMCCodeEmitter);
  TargetRegistry::RegisterMCCodeEmitter(getTheAIE2PSTarget(),
                                        createAIE2PSMCCodeEmitter);
}
