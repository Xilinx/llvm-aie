# AIEngine Fork of LLVM

This repository extends the LLVM framework to generate code for use with AMD/Xilinx AI Engine processors.

## Architecture Overview

Generally speaking, AI Engine processors are in-order, exposed-pipeline VLIW processors.  These processors are implemented as part of an array of processors focused on application acceleration targeting AI, Machine Learning, and DSP applications.  They have been integrated in a number of commercial devices including the [Versal AI Core Series](https://www.xilinx.com/products/silicon-devices/acap/versal-ai-core.html) and [Ryzen-AI SOCs](https://www.amd.com/en/products/processors/consumer/ryzen-ai.html).

Each VLIW instruction bundle specifies the behavior of one or more functional units, which begin executing a new instruction at the same time.  The processor pipeline does not include stall logic, and instructions will continue executing in order regardless of other instructions in the pipeline.  As a result, the compiler is able to schedule machine instructions which access the same register in ways that potentially overlap.  e.g.

```
1:   lda r12, [p0]     // writes r12 after cycle 8.
2:   nop
3:   nop
4:   mul r12, r12, r12 // reads r12 initial value and writes r12 after cycle 6.
5:   mov r14, r12      // reads r12 initial value
6:   nop
7:   add r13, r12, r6  // reads r12 from instruction 4.
8:   nop
9:   mul r14, r12, r7  // reads r12 from instruction 1.
```

Other key architectural characteristics include varying width instruction slots between different instruction encodings and relatively small address spaces (20-bit pointer registers).  The presence of varying-width instruction slots implies some code alignment restrictions for instructions which are branch or return targets.

For more information, see:
[AIE1 architecture manual](https://docs.xilinx.com/r/en-US/am009-versal-ai-engine) and 
[AIE2 architecture manual](https://docs.xilinx.com/r/en-US/am020-versal-aie-ml)


## Implementation

The AIE target includes basic support for both AIE1 and AIE2 instruction sets.  These instruction sets are significantly different, but share enough basic concepts and implementation to be handled as LLVM subtargets.  Current development is focused on maturing the AIE2 subtarget and the AIE1 subtarget should eventually be aligned with the updated approach for AIE2.

| Subtarget | Instruction Selection | VLIW scheduling | Software Pipelining | Hardware Loops | delay slot filling | Floating point | Vector Intrinsics | post-increment addressing |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| AIE1 | 	SelectionDAG	| PostRA Scheduler	| Not Yet	| Not Yet	    | Not Yet	| Yes, vector fp32	    | Yes, i8, i16, i32	| No  |
| AIE2 |	GlobalISel	    | Machine Scheduler	| Experimental | Experimental	| Yes	    | Yes, vector bfloat	| Yes, i8, i16   	| Yes |

Support for Clang, LLD, binutils (e.g. 'llvm-objdump'), Compiler-RT, and LLVM-LIBC is also included.

In order to support the unusual architecture features of AI Engine, this repository adds LLVM support for several specific features:
- support for non-power of 2 pointers;
- improved TableGen support for specifying operand latencies and resource conflicts of exposed pipeline instructions;
- scheduler support for negative operand latencies;
- support for selecting relocations for instructions with multiple encodings;
- support for architectures with code alignment restrictions;
- improved register allocation support for complex register hierarchies;

## Disclaimer

Note that this repository does not implement a generic compiler and may not completely support other technologies.  If you require a generic compiler or need to compile code for use with different technologies, you will need to select a different compiler.  The implementation maturity is generally similar to other 'Experimental' LLVM architectures.  For critical designs, please use the production compiler.

Modifications (c) Copyright 2022-2024 Advanced Micro Devices, Inc. or its affiliates

# The LLVM Compiler Infrastructure

[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/llvm/llvm-project/badge)](https://securityscorecards.dev/viewer/?uri=github.com/llvm/llvm-project)

Welcome to the LLVM project!

This repository contains the source code for LLVM, a toolkit for the
construction of highly optimized compilers, optimizers, and run-time
environments.

The LLVM project has multiple components. The core of the project is
itself called "LLVM". This contains all of the tools, libraries, and header
files needed to process intermediate representations and convert them into
object files. Tools include an assembler, disassembler, bitcode analyzer, and
bitcode optimizer.

C-like languages use the [Clang](http://clang.llvm.org/) frontend. This
component compiles C, C++, Objective-C, and Objective-C++ code into LLVM bitcode
-- and from there into object files, using LLVM.

Other components include:
the [libc++ C++ standard library](https://libcxx.llvm.org),
the [LLD linker](https://lld.llvm.org), and more.

## Getting the Source Code and Building LLVM

Consult the
[Getting Started with LLVM](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)
page for information on building and running LLVM.

For information on how to contribute to the LLVM project, please take a look at
the [Contributing to LLVM](https://llvm.org/docs/Contributing.html) guide.

## Getting in touch

Join the [LLVM Discourse forums](https://discourse.llvm.org/), [Discord
chat](https://discord.gg/xS7Z362),
[LLVM Office Hours](https://llvm.org/docs/GettingInvolved.html#office-hours) or
[Regular sync-ups](https://llvm.org/docs/GettingInvolved.html#online-sync-ups).

The LLVM project has adopted a [code of conduct](https://llvm.org/docs/CodeOfConduct.html) for
participants to all modes of communication within the project.
