//===--- AIE.cpp - AIE ToolChain Implementations ----------*- C++ -*-===//
//
// This file is licensed under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates
//
//===----------------------------------------------------------------------===//

#include "AIE.h"
#include "CommonArgs.h"
#include "clang/Driver/Compilation.h"
#include "clang/Driver/DriverDiagnostic.h"
#include "llvm/Option/ArgList.h"
#include "llvm/Option/Option.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/VirtualFileSystem.h"

using namespace clang::driver;
using namespace clang::driver::tools;
using namespace clang::driver::toolchains;
using namespace clang;
using namespace llvm::opt;

void aie::Linker::ConstructJob(Compilation &C, const JobAction &JA,
                               const InputInfo &Output,
                               const InputInfoList &Inputs, const ArgList &Args,
                               const char *LinkingOutput) const {

  const ToolChain &ToolChain = getToolChain();
  std::string Linker = ToolChain.GetProgramPath(getShortName());
  ArgStringList CmdArgs;
  // We should not use the default page alignment.
  CmdArgs.push_back(Args.MakeArgString("--nmagic"));
  AddLinkerInputs(ToolChain, Inputs, Args, CmdArgs, JA);
  // CmdArgs.push_back("-shared");
  ToolChain.AddFilePathLibArgs(Args, CmdArgs);
  if (!Args.hasArg(options::OPT_nostdlib) &&
      !Args.hasArg(options::OPT_nodefaultlibs)) {
    AddRunTimeLibs(ToolChain, ToolChain.getDriver(), CmdArgs, Args);
    CmdArgs.push_back(Args.MakeArgString("-lc"));
    CmdArgs.push_back(Args.MakeArgString("-lm"));
  }

  if (!Args.hasArg(options::OPT_nostdlib, options::OPT_nostartfiles,
                   options::OPT_r)) {
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crt0.o")));
    CmdArgs.push_back(Args.MakeArgString(ToolChain.GetFilePath("crt1.o")));
  }
  CmdArgs.push_back("-o");
  CmdArgs.push_back(Output.getFilename());
  C.addCommand(std::make_unique<Command>(JA, *this, ResponseFileSupport::None(),
                                         Args.MakeArgString(Linker), CmdArgs,
                                         Inputs));
}

/// AIE Toolchain
AIEToolChain::AIEToolChain(const Driver &D, const llvm::Triple &Triple,
                           const ArgList &Args)
    : Generic_ELF(D, Triple, Args) {}

void AIEToolChain::AddClangSystemIncludeArgs(const ArgList &DriverArgs,
                                             ArgStringList &CC1Args) const {

  llvm::Triple::ArchType arch = getTriple().getArch();

  // Always include our instrinsics, for compatibility with existing toolchain.
  if (!DriverArgs.hasArg(options::OPT_mno_vitis_headers)) {
    std::string path = (arch == llvm::Triple::aie2)
                           ? GetFilePath("aiev2intrin.h")
                           : GetFilePath("aiev1intrin.h");
    CC1Args.append({"-include", DriverArgs.MakeArgString(path)});
  }

  CC1Args.push_back("-D__AIENGINE__");
  if (arch == llvm::Triple::aie)
    CC1Args.push_back("-D__AIEARCH__=10");
  else if (arch == llvm::Triple::aie2)
    CC1Args.push_back("-D__AIEARCH__=20");

  // Don't pull in system headers from /usr/include or /usr/local/include.
  // All of the basic headers that we need come from the compiler.
  CC1Args.push_back("-nostdsysteminc");

  if (DriverArgs.hasArg(options::OPT_nostdlibinc))
    return;

  const Driver &D = getDriver();
  std::string Target = getTripleString();
  SmallString<128> Path(D.Dir);
  llvm::sys::path::append(Path, "..", "include", Target);
  if (getVFS().exists(Path))
    addExternCSystemInclude(DriverArgs, CC1Args, Path.str());
}

void AIEToolChain::addLibCxxIncludePaths(
    const llvm::opt::ArgList &DriverArgs,
    llvm::opt::ArgStringList &CC1Args) const {

  if (DriverArgs.hasArg(options::OPT_nostdinc, options::OPT_nostdlibinc,
                        options::OPT_nostdincxx))
    return;

  // We don't ship libc/libcxx for AIE1
  if (getTriple().isAIE1())
    return;

  const Driver &D = getDriver();
  const std::string Target = getTripleString();

  SmallString<128> Path(D.Dir);
  llvm::sys::path::append(Path, "..", "include");

  const std::string Version = detectLibcxxVersion(Path);
  if (Version.empty())
    return;

  // First add the per-target include path.
  SmallString<128> TargetDir(Path);
  llvm::sys::path::append(TargetDir, Target, "c++", Version);
  if (getVFS().exists(TargetDir))
    addSystemInclude(DriverArgs, CC1Args, TargetDir);

  // Second add the generic one.
  SmallString<128> Dir(Path);
  llvm::sys::path::append(Dir, "c++", Version);
  addSystemInclude(DriverArgs, CC1Args, Dir);
}

void AIEToolChain::addClangTargetOptions(
    const llvm::opt::ArgList &DriverArgs, llvm::opt::ArgStringList &CC1Args,
    Action::OffloadKind DeviceOffloadKind) const {
  // For now, we disable the auto-vectorizers by default, as the backend cannot
  // handle many vector types. For experimentation the vectorizers can still be
  // enabled explicitly by the user

  if (!DriverArgs.hasFlag(options::OPT_fuse_init_array,
                          options::OPT_fno_use_init_array, false))
    CC1Args.push_back("-fno-use-init-array");

  if (!DriverArgs.hasArg(options::OPT_fvectorize))
    CC1Args.append({"-mllvm", "-vectorize-loops=false"});

  if (!DriverArgs.hasArg(options::OPT_fslp_vectorize))
    CC1Args.append({"-mllvm", "-vectorize-slp=false"});

  // An if-then-else cascade requires at least 5 delay slots for evaluating the
  // condition and 5 delay slots for one of the branches, thus speculating 10
  // instructions should be fine
  CC1Args.append({"-mllvm", "--two-entry-phi-node-folding-threshold=10"});

  // Pass -fno-threadsafe-statics to prevent dependence on lock acquire/release
  // handling for static local variables.
  if (!DriverArgs.hasFlag(options::OPT_fthreadsafe_statics,
                          options::OPT_fno_threadsafe_statics, false))
    CC1Args.push_back("-fno-threadsafe-statics");

  // Make sure to perform most optimizations before mandatory inlinings,
  // otherwise noalias attributes can get lost and hurt AA results.
  CC1Args.append({"-mllvm", "-mandatory-inlining-before-opt=false"});

  // Perform complete AA analysis on phi nodes.
  CC1Args.append({"-mllvm", "-basic-aa-full-phi-analysis=true"});

  // Extend the max limit of the search depth in BasicAA
  CC1Args.append({"-mllvm", "-basic-aa-max-lookup-search-depth=10"});
}

// Avoid using newer dwarf versions, as the simulator doesn't understand newer
// dwarf.
unsigned AIEToolChain::getMaxDwarfVersion() const { return 4; }
