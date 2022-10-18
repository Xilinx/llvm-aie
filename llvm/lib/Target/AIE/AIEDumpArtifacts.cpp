//===--- AIEDumpArtifacts.cpp - Module and Function dumping passes -------===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//
//
// DumpModulePass & MachineFunctionDumper implementations for AIEX.
//
//===----------------------------------------------------------------------===//

#include "AIEDumpArtifacts.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SlotIndexes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PrintPasses.h"
#include "llvm/InitializePasses.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

cl::opt<std::string>
    AIEArtifactDir("aie-artifact-dir",
                   cl::desc("Specify the dir to dump artifacts"), cl::Hidden);

bool AIEDumpArtifacts;
cl::opt<bool, true> AIEDumpArts("aie-dump-artifacts",
                                cl::desc("Dump intermediate artifacts for AIE"),
                                cl::location(AIEDumpArtifacts), cl::init(false),
                                cl::Hidden);

namespace {

class AIEDumpModulePassWrapper : public ModulePass {
  std::string Suffix;
  bool ShouldPreserveUseListOrder;

public:
  static char ID;
  AIEDumpModulePassWrapper() : ModulePass(ID) {}
  AIEDumpModulePassWrapper(const std::string &Suffix,
                           bool ShouldPreserveUseListOrder)
      : ModulePass(ID), Suffix(Suffix),
        ShouldPreserveUseListOrder(ShouldPreserveUseListOrder) {}

  bool runOnModule(Module &M) override {
    if (!llvm::sys::fs::exists(AIEArtifactDir)) {
      llvm::errs() << "Failed to find directory: " << AIEArtifactDir << '\n';
      return false;
    }
    llvm::SmallString<200> SourceFile(M.getSourceFileName());
    llvm::SmallString<128> OutputFileName(
        llvm::sys::path::filename(SourceFile));
    // Remove file extension and get the raw file name.
    llvm::sys::path::replace_extension(OutputFileName, "");
    OutputFileName += "-" + Suffix + ".ll";
    llvm::SmallString<200> OutputFilePath(AIEArtifactDir);
    llvm::sys::path::append(OutputFilePath, OutputFileName);
    llvm::sys::fs::remove(OutputFilePath);
    std::error_code EC;
    llvm::raw_fd_ostream OutputFileStream(OutputFilePath, EC,
                                          sys::fs::OF_Append);
    if (EC) {
      llvm::errs() << "Failed to open output file: " << EC.message() << '\n';
      return false;
    }

    if (llvm::isFunctionInPrintList("*"))
      M.print(OutputFileStream, nullptr, ShouldPreserveUseListOrder);
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
  }

  StringRef getPassName() const override { return "AIE Dump Module IR"; }
};

/// AIEMachineFunctionDumper - This is a pass to dump the MIR of a
/// MachineFunction for AIE.
///
class AIEMachineFunctionDumper : public MachineFunctionPass {
  std::string Suffix;

public:
  static char ID;
  AIEMachineFunctionDumper() : MachineFunctionPass(ID) {}
  AIEMachineFunctionDumper(const std::string &Suffix)
      : MachineFunctionPass(ID), Suffix(Suffix) {}

  bool runOnMachineFunction(MachineFunction &MF) override {
    if (!llvm::sys::fs::exists(AIEArtifactDir)) {
      llvm::errs() << "Failed to find directory: " << AIEArtifactDir << '\n';
      return false;
    }
    llvm::SmallString<200> SourceFile(
        MF.getFunction().getParent()->getSourceFileName());
    llvm::SmallString<128> OutputFileName(
        llvm::sys::path::filename(SourceFile));
    // Remove file extension and get the raw file name.
    llvm::sys::path::replace_extension(OutputFileName, "");
    OutputFileName += "-" + Suffix + ".mir";
    llvm::SmallString<200> OutputFilePath(AIEArtifactDir);
    llvm::sys::path::append(OutputFilePath, OutputFileName);
    std::error_code EC;
    llvm::raw_fd_ostream OutputFileStream(OutputFilePath, EC,
                                          sys::fs::OF_Append);
    if (EC) {
      llvm::errs() << "Failed to open output file: " << EC.message() << '\n';
      return false;
    }

    if (!isFunctionInPrintList(MF.getName()))
      return false;
    MF.print(OutputFileStream, getAnalysisIfAvailable<SlotIndexes>());
    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    AU.addUsedIfAvailable<SlotIndexes>();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  StringRef getPassName() const override {
    return "AIE MachineFunction Dumper";
  }
};
} // end anonymous namespace

char AIEDumpModulePassWrapper::ID = 0;
INITIALIZE_PASS(AIEDumpModulePassWrapper, "aie-dump-module",
                "AIE Dump module to storage", false, true)
char AIEMachineFunctionDumper::ID = 0;
INITIALIZE_PASS(AIEMachineFunctionDumper, "aie-machineinstr-dumper",
                "AIE Dump Machine Function to storage", false, true)

namespace llvm {
ModulePass *createDumpModulePass(const std::string &Suffix,
                                 bool ShouldPreserveUseListOrder) {
  return new AIEDumpModulePassWrapper(Suffix, ShouldPreserveUseListOrder);
}

MachineFunctionPass *
createMachineFunctionDumperPass(const std::string &Suffix) {
  return new AIEMachineFunctionDumper(Suffix);
}
} // end namespace llvm
