;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2024-2026 Advanced Micro Devices, Inc. or its affiliates

; When EXPENSIVE_CHECKS are enabled, the machine verifier appears between each
; pass. Ignore it with 'grep -v'.

; RUN: llc -O0 -mtriple=aie2p -disable-verify -debug-pass=Structure < %s 2>&1 \
; RUN:   | grep -v 'Verify generated machine code' | FileCheck -match-full-lines -strict-whitespace -check-prefix=AIE-O0 %s
; RUN: llc -O1 -mtriple=aie2p -disable-verify -debug-pass=Structure < %s 2>&1 \
; RUN:   | grep -v 'Verify generated machine code' | FileCheck -match-full-lines -strict-whitespace -check-prefixes=AIE-O1 %s
; RUN: llc -O2 -mtriple=aie2p -disable-verify -debug-pass=Structure < %s 2>&1 \
; RUN:   | grep -v 'Verify generated machine code' | FileCheck -match-full-lines -strict-whitespace -check-prefixes=AIE-O23 %s
; RUN: llc -O3 -mtriple=aie2p -disable-verify -debug-pass=Structure < %s 2>&1 \
; RUN:   | grep -v 'Verify generated machine code' | FileCheck -match-full-lines -strict-whitespace -check-prefixes=AIE-O23 %s

; AIE-O0:Target Library Information
; AIE-O0-NEXT:Target Pass Configuration
; AIE-O0-NEXT:Machine Module Information
; AIE-O0-NEXT:Target Transform Information
; AIE-O0-NEXT:Create Garbage Collector Module Metadata
; AIE-O0-NEXT:Assumption Cache Tracker
; AIE-O0-NEXT:Machine Branch Probability Analysis
; AIE-O0-NEXT:  ModulePass Manager
; AIE-O0-NEXT:    Pre-ISel Intrinsic Lowering
; AIE-O0-NEXT:    FunctionPass Manager
; AIE-O0-NEXT:      Expand large div/rem
; AIE-O0-NEXT:      Expand fp
; AIE-O0-NEXT:      Expand Atomic instructions
; AIE-O0-NEXT:      Lower Garbage Collection Instructions
; AIE-O0-NEXT:      Shadow Stack GC Lowering
; AIE-O0-NEXT:      Remove unreachable blocks from the CFG
; AIE-O0-NEXT:      Instrument function entry/exit with calls to e.g. mcount() (post inlining)
; AIE-O0-NEXT:      Scalarize Masked Memory Intrinsics
; AIE-O0-NEXT:      Expand reduction intrinsics
; AIE-O0-NEXT:      Lower invoke and unwind, for unwindless code generators
; AIE-O0-NEXT:      Remove unreachable blocks from the CFG
; AIE-O0-NEXT:      AIE outline Memory GEP
; AIE-O0-NEXT:      Prepare callbr
; AIE-O0-NEXT:      Safe Stack instrumentation pass
; AIE-O0-NEXT:      Insert stack protectors
; AIE-O0-NEXT:      Analysis containing CSE Info
; AIE-O0-NEXT:      IRTranslator
; AIE-O0-NEXT:      Analysis containing CSE Info
; AIE-O0-NEXT:      AIE Address Space Flattening Pass
; AIE-O0-NEXT:      AIE Eliminate Duplicate PHI Pass
; AIE-O0-NEXT:      Analysis for ComputingKnownBits
; AIE-O0-NEXT:      Legalizer
; AIE-O0-NEXT:      RegBankSelect
; AIE-O0-NEXT:      Analysis for ComputingKnownBits
; AIE-O0-NEXT:      MachineDominator Tree Construction
; AIE-O0-NEXT:      Analysis containing CSE Info
; AIE-O0-NEXT:      AIE Pre-ISel Combiner
; AIE-O0-NEXT:      InstructionSelect
; AIE-O0-NEXT:      ResetMachineFunction
; AIE-O0-NEXT:      Finalize ISel and expand pseudo-instructions
; AIE-O0-NEXT:      Local Stack Slot Allocation
; AIE-O0-NEXT:      Eliminate PHI nodes for register allocation
; AIE-O0-NEXT:      AIE sub-reg constrainer
; AIE-O0-NEXT:      Two-Address instruction pass
; AIE-O0-NEXT:      Fast Register Allocator
; AIE-O0-NEXT:      Remove Redundant DEBUG_VALUE analysis
; AIE-O0-NEXT:      Fixup Statepoint Caller Saved
; AIE-O0-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O0-NEXT:      Machine Optimization Remark Emitter
; AIE-O0-NEXT:      Prologue/Epilogue Insertion & Frame Finalization
; AIE-O0-NEXT:      Post-RA pseudo instruction expansion pass
; AIE-O0-NEXT:      Remove dead machine instructions
; AIE-O0-NEXT:      AIE pseudo branch expansion
; AIE-O0-NEXT:      MachineDominator Tree Construction
; AIE-O0-NEXT:      Machine Natural Loop Construction
; AIE-O0-NEXT:      Dominator Tree Construction
; AIE-O0-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O0-NEXT:      Function Alias Analysis Results
; AIE-O0-NEXT:      PostRA Machine Instruction Scheduler
; AIE-O0-NEXT:      AIE Bundle Finalization
; AIE-O0-NEXT:      AIE Machine Alignment
; AIE-O0-NEXT:      Analyze Machine Code For Garbage Collection
; AIE-O0-NEXT:      Insert fentry calls
; AIE-O0-NEXT:      Insert XRay ops
; AIE-O0-NEXT:      Implement the 'patchable-function' attribute
; AIE-O0-NEXT:      Contiguously Lay Out Funclets
; AIE-O0-NEXT:      Remove Loads Into Fake Uses
; AIE-O0-NEXT:      StackMap Liveness Analysis
; AIE-O0-NEXT:      Live DEBUG_VALUE analysis
; AIE-O0-NEXT:      Machine Sanitizer Binary Metadata
; AIE-O0-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O0-NEXT:      Machine Optimization Remark Emitter
; AIE-O0-NEXT:      Stack Frame Layout Analysis
; AIE-O0-NEXT:      AIE2 Assembly Printer
; AIE-O0-NEXT:      Free MachineFunction

; AIE-O1:Target Library Information
; AIE-O1-NEXT:Target Pass Configuration
; AIE-O1-NEXT:Machine Module Information
; AIE-O1-NEXT:Target Transform Information
; AIE-O1-NEXT:AIE complex addressing modes based Alias Analysis
; AIE-O1-NEXT:External Alias Analysis
; AIE-O1-NEXT:Assumption Cache Tracker
; AIE-O1-NEXT:Type-Based Alias Analysis
; AIE-O1-NEXT:Scoped NoAlias Alias Analysis
; AIE-O1-NEXT:Profile summary info
; AIE-O1-NEXT:Create Garbage Collector Module Metadata
; AIE-O1-NEXT:Machine Branch Probability Analysis
; AIE-O1-NEXT:Default Regalloc Eviction Advisor
; AIE-O1-NEXT:Default Regalloc Priority Advisor
; AIE-O1-NEXT:  ModulePass Manager
; AIE-O1-NEXT:    Pre-ISel Intrinsic Lowering
; AIE-O1-NEXT:    FunctionPass Manager
; AIE-O1-NEXT:      Expand large div/rem
; AIE-O1-NEXT:      Expand fp
; AIE-O1-NEXT:      Expand Atomic instructions
; AIE-O1-NEXT:      Infer address spaces
; AIE-O1-NEXT:      Dominator Tree Construction
; AIE-O1-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O1-NEXT:      Natural Loop Information
; AIE-O1-NEXT:      Canonicalize natural loops
; AIE-O1-NEXT:      Scalar Evolution Analysis
; AIE-O1-NEXT:      Loop Pass Manager
; AIE-O1-NEXT:        Canonicalize Freeze Instructions in Loops
; AIE-O1-NEXT:        Induction Variable Users
; AIE-O1-NEXT:        Loop Strength Reduction
; AIE-O1-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O1-NEXT:      Function Alias Analysis Results
; AIE-O1-NEXT:      Merge contiguous icmps into a memcmp
; AIE-O1-NEXT:      Natural Loop Information
; AIE-O1-NEXT:      Lazy Branch Probability Analysis
; AIE-O1-NEXT:      Lazy Block Frequency Analysis
; AIE-O1-NEXT:      Expand memcmp() to load/stores
; AIE-O1-NEXT:      Lower Garbage Collection Instructions
; AIE-O1-NEXT:      Shadow Stack GC Lowering
; AIE-O1-NEXT:      Remove unreachable blocks from the CFG
; AIE-O1-NEXT:      Natural Loop Information
; AIE-O1-NEXT:      Post-Dominator Tree Construction
; AIE-O1-NEXT:      Branch Probability Analysis
; AIE-O1-NEXT:      Block Frequency Analysis
; AIE-O1-NEXT:      Constant Hoisting
; AIE-O1-NEXT:      Replace intrinsics with calls to vector library
; AIE-O1-NEXT:      Lazy Branch Probability Analysis
; AIE-O1-NEXT:      Lazy Block Frequency Analysis
; AIE-O1-NEXT:      Optimization Remark Emitter
; AIE-O1-NEXT:      Partially inline calls to library functions
; AIE-O1-NEXT:      Instrument function entry/exit with calls to e.g. mcount() (post inlining)
; AIE-O1-NEXT:      Scalarize Masked Memory Intrinsics
; AIE-O1-NEXT:      Expand reduction intrinsics
; AIE-O1-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O1-NEXT:      Function Alias Analysis Results
; AIE-O1-NEXT:      Natural Loop Information
; AIE-O1-NEXT:      Scalar Evolution Analysis
; AIE-O1-NEXT:      GPU Load and Store Vectorizer
; AIE-O1-NEXT:      CodeGen Prepare
; AIE-O1-NEXT:      Lower invoke and unwind, for unwindless code generators
; AIE-O1-NEXT:      Remove unreachable blocks from the CFG
; AIE-O1-NEXT:      AIE outline Memory GEP
; AIE-O1-NEXT:      Dominator Tree Construction
; AIE-O1-NEXT:      Natural Loop Information
; AIE-O1-NEXT:      Scalar Evolution Analysis
; AIE-O1-NEXT:      Lazy Branch Probability Analysis
; AIE-O1-NEXT:      Lazy Block Frequency Analysis
; AIE-O1-NEXT:      Optimization Remark Emitter
; AIE-O1-NEXT:      AIE Inner Loop Versioner
; AIE-O1-NEXT:      Dominator Tree Construction
; AIE-O1-NEXT:      Natural Loop Information
; AIE-O1-NEXT:      Scalar Evolution Analysis
; AIE-O1-NEXT:      Lazy Branch Probability Analysis
; AIE-O1-NEXT:      Lazy Block Frequency Analysis
; AIE-O1-NEXT:      Optimization Remark Emitter
; AIE-O1-NEXT:      Hardware Loop Insertion
; AIE-O1-NEXT:      AIE Outer Loop Pointer Optimizer
; AIE-O1-NEXT:      Scalar Evolution Analysis
; AIE-O1-NEXT:      AIE Outer Loop Pipeliner
; AIE-O1-NEXT:      Dominator Tree Construction
; AIE-O1-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O1-NEXT:      Function Alias Analysis Results
; AIE-O1-NEXT:      ObjC ARC contraction
; AIE-O1-NEXT:      Prepare callbr
; AIE-O1-NEXT:      Safe Stack instrumentation pass
; AIE-O1-NEXT:      Insert stack protectors
; AIE-O1-NEXT:      Analysis containing CSE Info
; AIE-O1-NEXT:      Natural Loop Information
; AIE-O1-NEXT:      Post-Dominator Tree Construction
; AIE-O1-NEXT:      Branch Probability Analysis
; AIE-O1-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O1-NEXT:      Function Alias Analysis Results
; AIE-O1-NEXT:      IRTranslator
; AIE-O1-NEXT:      Analysis containing CSE Info
; AIE-O1-NEXT:      AIE Address Space Flattening Pass
; AIE-O1-NEXT:      Analysis for ComputingKnownBits
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      AIE PreLegalizer Combiner
; AIE-O1-NEXT:      AIE Eliminate Duplicate PHI Pass
; AIE-O1-NEXT:      Legalizer
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      AIE Post Legalizer Generic Combiner
; AIE-O1-NEXT:      AIE Base Address Clustering Optimization
; AIE-O1-NEXT:      Machine Natural Loop Construction
; AIE-O1-NEXT:      AIE Pointer Modifier Optimization
; AIE-O1-NEXT:      AIE Post Legalizer Custom Combiner
; AIE-O1-NEXT:      RegBankSelect
; AIE-O1-NEXT:      Analysis for ComputingKnownBits
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      Analysis containing CSE Info
; AIE-O1-NEXT:      AIE Pre-ISel Combiner
; AIE-O1-NEXT:      Lazy Branch Probability Analysis
; AIE-O1-NEXT:      Lazy Block Frequency Analysis
; AIE-O1-NEXT:      InstructionSelect
; AIE-O1-NEXT:      Remove dead machine instructions
; AIE-O1-NEXT:      AIE Post Select Optimizer
; AIE-O1-NEXT:      Remove dead machine instructions
; AIE-O1-NEXT:      ResetMachineFunction
; AIE-O1-NEXT:      Finalize ISel and expand pseudo-instructions
; AIE-O1-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O1-NEXT:      Early Tail Duplication
; AIE-O1-NEXT:      Optimize machine instruction PHIs
; AIE-O1-NEXT:      Slot index numbering
; AIE-O1-NEXT:      Merge disjoint stack slots
; AIE-O1-NEXT:      Local Stack Slot Allocation
; AIE-O1-NEXT:      Remove dead machine instructions
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      Machine Natural Loop Construction
; AIE-O1-NEXT:      Machine Trace Metrics
; AIE-O1-NEXT:      Early If-Conversion
; AIE-O1-NEXT:      Machine Block Frequency Analysis
; AIE-O1-NEXT:      Function Alias Analysis Results
; AIE-O1-NEXT:      Early Machine Loop Invariant Code Motion
; AIE-O1-NEXT:      Machine LICM for reserved regs
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      Machine Block Frequency Analysis
; AIE-O1-NEXT:      Machine Common Subexpression Elimination
; AIE-O1-NEXT:      MachinePostDominator Tree Construction
; AIE-O1-NEXT:      Machine Cycle Info Analysis
; AIE-O1-NEXT:      Machine code sinking
; AIE-O1-NEXT:      Peephole Optimizations
; AIE-O1-NEXT:      Remove dead machine instructions
; AIE-O1-NEXT:      Detect Dead Lanes
; AIE-O1-NEXT:      Init Undef Pass
; AIE-O1-NEXT:      Process Implicit Definitions
; AIE-O1-NEXT:      Remove unreachable machine basic blocks
; AIE-O1-NEXT:      Live Variable Analysis
; AIE-O1-NEXT:      Eliminate PHI nodes for register allocation
; AIE-O1-NEXT:      AIE sub-reg constrainer
; AIE-O1-NEXT:      Two-Address instruction pass
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      Slot index numbering
; AIE-O1-NEXT:      Live Interval Analysis
; AIE-O1-NEXT:      Register Coalescer
; AIE-O1-NEXT:      Machine Instruction Scheduler
; AIE-O1-NEXT:      Register Coalescer
; AIE-O1-NEXT:      AIE 2D/3D operand splitter
; AIE-O1-NEXT:      Machine Block Frequency Analysis
; AIE-O1-NEXT:      Live Interval Analysis
; AIE-O1-NEXT:      Debug Variable Analysis
; AIE-O1-NEXT:      Live Stack Slot Analysis
; AIE-O1-NEXT:      Virtual Register Map
; AIE-O1-NEXT:      Live Register Matrix
; AIE-O1-NEXT:      Bundle Machine CFG Edges
; AIE-O1-NEXT:      Spill Code Placement Analysis
; AIE-O1-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O1-NEXT:      Machine Optimization Remark Emitter
; AIE-O1-NEXT:      Greedy Register Allocator
; AIE-O1-NEXT:      AIE super-reg rewrite
; AIE-O1-NEXT:      Greedy Register Allocator
; AIE-O1-NEXT:      AIE super-reg rewrite
; AIE-O1-NEXT:      AIE unallocated super-reg rewrite
; AIE-O1-NEXT:      Greedy Register Allocator
; AIE-O1-NEXT:      AIE waw-reg rewrite
; AIE-O1-NEXT:      AIE epilogue register rewrite
; AIE-O1-NEXT:      Greedy Register Allocator
; AIE-O1-NEXT:      Virtual Register Rewriter
; AIE-O1-NEXT:      Stack Slot Coloring
; AIE-O1-NEXT:      AIE Spill Slot Optimization
; AIE-O1-NEXT:      AIE 1D operands to 2D/3D rewriter
; AIE-O1-NEXT:      Machine Copy Propagation Pass
; AIE-O1-NEXT:      Machine Loop Invariant Code Motion
; AIE-O1-NEXT:      Remove Redundant DEBUG_VALUE analysis
; AIE-O1-NEXT:      Fixup Statepoint Caller Saved
; AIE-O1-NEXT:      PostRA Machine Sink
; AIE-O1-NEXT:      Machine Block Frequency Analysis
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      MachinePostDominator Tree Construction
; AIE-O1-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O1-NEXT:      Machine Optimization Remark Emitter
; AIE-O1-NEXT:      Shrink Wrapping analysis
; AIE-O1-NEXT:      Prologue/Epilogue Insertion & Frame Finalization
; AIE-O1-NEXT:      Machine Late Instructions Cleanup Pass
; AIE-O1-NEXT:      Control Flow Optimizer
; AIE-O1-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O1-NEXT:      Tail Duplication
; AIE-O1-NEXT:      Machine Copy Propagation Pass
; AIE-O1-NEXT:      Machine Copy Propagation Pass
; AIE-O1-NEXT:      Post-RA pseudo instruction expansion pass
; AIE-O1-NEXT:      Remove dead machine instructions
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      Machine Natural Loop Construction
; AIE-O1-NEXT:      Machine Block Frequency Analysis
; AIE-O1-NEXT:      MachinePostDominator Tree Construction
; AIE-O1-NEXT:      Branch Probability Basic Block Placement
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      Machine Natural Loop Construction
; AIE-O1-NEXT:      ReachingDefAnalysis
; AIE-O1-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O1-NEXT:      Machine Optimization Remark Emitter
; AIE-O1-NEXT:      AIE Hardware Loops pass
; AIE-O1-NEXT:      AIE pseudo branch expansion
; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O1-NEXT:      Machine Natural Loop Construction
; AIE-O1-NEXT:      PostRA Machine Instruction Scheduler
; AIE-O1-NEXT:      AIE Bundle Finalization
; AIE-O1-NEXT:      AIE Machine Alignment
; AIE-O1-NEXT:      Analyze Machine Code For Garbage Collection
; AIE-O1-NEXT:      Insert fentry calls
; AIE-O1-NEXT:      Insert XRay ops
; AIE-O1-NEXT:      Implement the 'patchable-function' attribute
; AIE-O1-NEXT:      Contiguously Lay Out Funclets
; AIE-O1-NEXT:      Remove Loads Into Fake Uses
; AIE-O1-NEXT:      StackMap Liveness Analysis
; AIE-O1-NEXT:      Live DEBUG_VALUE analysis
; AIE-O1-NEXT:      Machine Sanitizer Binary Metadata
; AIE-O1-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O1-NEXT:      Machine Optimization Remark Emitter
; AIE-O1-NEXT:      Stack Frame Layout Analysis
; AIE-O1-NEXT:      AIE2 Assembly Printer
; AIE-O1-NEXT:      Free MachineFunction

; AIE-O23:Target Library Information
; AIE-O23-NEXT:Target Pass Configuration
; AIE-O23-NEXT:Machine Module Information
; AIE-O23-NEXT:Target Transform Information
; AIE-O23-NEXT:AIE complex addressing modes based Alias Analysis
; AIE-O23-NEXT:External Alias Analysis
; AIE-O23-NEXT:Assumption Cache Tracker
; AIE-O23-NEXT:Type-Based Alias Analysis
; AIE-O23-NEXT:Scoped NoAlias Alias Analysis
; AIE-O23-NEXT:Profile summary info
; AIE-O23-NEXT:Create Garbage Collector Module Metadata
; AIE-O23-NEXT:Machine Branch Probability Analysis
; AIE-O23-NEXT:Default Regalloc Eviction Advisor
; AIE-O23-NEXT:Default Regalloc Priority Advisor
; AIE-O23-NEXT:  ModulePass Manager
; AIE-O23-NEXT:    Pre-ISel Intrinsic Lowering
; AIE-O23-NEXT:    FunctionPass Manager
; AIE-O23-NEXT:      Expand large div/rem
; AIE-O23-NEXT:      Expand fp
; AIE-O23-NEXT:      Expand Atomic instructions
; AIE-O23-NEXT:      Infer address spaces
; AIE-O23-NEXT:      Dominator Tree Construction
; AIE-O23-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O23-NEXT:      Natural Loop Information
; AIE-O23-NEXT:      Canonicalize natural loops
; AIE-O23-NEXT:      Scalar Evolution Analysis
; AIE-O23-NEXT:      Loop Pass Manager
; AIE-O23-NEXT:        Canonicalize Freeze Instructions in Loops
; AIE-O23-NEXT:        Induction Variable Users
; AIE-O23-NEXT:        Loop Strength Reduction
; AIE-O23-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O23-NEXT:      Function Alias Analysis Results
; AIE-O23-NEXT:      Merge contiguous icmps into a memcmp
; AIE-O23-NEXT:      Natural Loop Information
; AIE-O23-NEXT:      Lazy Branch Probability Analysis
; AIE-O23-NEXT:      Lazy Block Frequency Analysis
; AIE-O23-NEXT:      Expand memcmp() to load/stores
; AIE-O23-NEXT:      Lower Garbage Collection Instructions
; AIE-O23-NEXT:      Shadow Stack GC Lowering
; AIE-O3-NEXT:      Lower constant intrinsics
; AIE-O23-NEXT:      Remove unreachable blocks from the CFG
; AIE-O23-NEXT:      Natural Loop Information
; AIE-O23-NEXT:      Post-Dominator Tree Construction
; AIE-O23-NEXT:      Branch Probability Analysis
; AIE-O23-NEXT:      Block Frequency Analysis
; AIE-O23-NEXT:      Constant Hoisting
; AIE-O23-NEXT:      Replace intrinsics with calls to vector library
; AIE-O23-NEXT:      Lazy Branch Probability Analysis
; AIE-O23-NEXT:      Lazy Block Frequency Analysis
; AIE-O23-NEXT:      Optimization Remark Emitter
; AIE-O23-NEXT:      Partially inline calls to library functions
; AIE-O3-NEXT:      Expand vector predication intrinsics
; AIE-O23-NEXT:      Instrument function entry/exit with calls to e.g. mcount() (post inlining)
; AIE-O23-NEXT:      Scalarize Masked Memory Intrinsics
; AIE-O23-NEXT:      Expand reduction intrinsics
; AIE-O23-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O23-NEXT:      Function Alias Analysis Results
; AIE-O23-NEXT:      Natural Loop Information
; AIE-O23-NEXT:      Scalar Evolution Analysis
; AIE-O23-NEXT:      GPU Load and Store Vectorizer
; AIE-O3-NEXT:      TLS Variable Hoist
; AIE-O23-NEXT:      CodeGen Prepare
; AIE-O23-NEXT:      Lower invoke and unwind, for unwindless code generators
; AIE-O23-NEXT:      Remove unreachable blocks from the CFG
; AIE-O23-NEXT:      AIE outline Memory GEP
; AIE-O23-NEXT:      Dominator Tree Construction
; AIE-O23-NEXT:      Natural Loop Information
; AIE-O23-NEXT:      Scalar Evolution Analysis
; AIE-O23-NEXT:      Lazy Branch Probability Analysis
; AIE-O23-NEXT:      Lazy Block Frequency Analysis
; AIE-O23-NEXT:      Optimization Remark Emitter
; AIE-O23-NEXT:      AIE Inner Loop Versioner
; AIE-O23-NEXT:      Dominator Tree Construction
; AIE-O23-NEXT:      Natural Loop Information
; AIE-O23-NEXT:      Scalar Evolution Analysis
; AIE-O23-NEXT:      Lazy Branch Probability Analysis
; AIE-O23-NEXT:      Lazy Block Frequency Analysis
; AIE-O23-NEXT:      Optimization Remark Emitter
; AIE-O23-NEXT:      Hardware Loop Insertion
; AIE-O23-NEXT:      AIE Outer Loop Pointer Optimizer
; AIE-O23-NEXT:      Scalar Evolution Analysis
; AIE-O23-NEXT:      AIE Outer Loop Pipeliner
; AIE-O23-NEXT:      Dominator Tree Construction
; AIE-O23-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O23-NEXT:      Function Alias Analysis Results
; AIE-O23-NEXT:      ObjC ARC contraction
; AIE-O23-NEXT:      Prepare callbr
; AIE-O23-NEXT:      Safe Stack instrumentation pass
; AIE-O23-NEXT:      Insert stack protectors
; AIE-O23-NEXT:      Analysis containing CSE Info
; AIE-O23-NEXT:      Natural Loop Information
; AIE-O23-NEXT:      Post-Dominator Tree Construction
; AIE-O23-NEXT:      Branch Probability Analysis
; AIE-O23-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O23-NEXT:      Function Alias Analysis Results
; AIE-O23-NEXT:      IRTranslator
; AIE-O23-NEXT:      Analysis containing CSE Info
; AIE-O23-NEXT:      AIE Address Space Flattening Pass
; AIE-O23-NEXT:      Analysis for ComputingKnownBits
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      AIE PreLegalizer Combiner
; AIE-O23-NEXT:      AIE Eliminate Duplicate PHI Pass
; AIE-O23-NEXT:      Legalizer
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      AIE Post Legalizer Generic Combiner
; AIE-O23-NEXT:      AIE Base Address Clustering Optimization
; AIE-O23-NEXT:      Machine Natural Loop Construction
; AIE-O23-NEXT:      AIE Pointer Modifier Optimization
; AIE-O23-NEXT:      AIE Post Legalizer Custom Combiner
; AIE-O23-NEXT:      RegBankSelect
; AIE-O23-NEXT:      Analysis for ComputingKnownBits
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Analysis containing CSE Info
; AIE-O23-NEXT:      AIE Pre-ISel Combiner
; AIE-O23-NEXT:      Lazy Branch Probability Analysis
; AIE-O23-NEXT:      Lazy Block Frequency Analysis
; AIE-O23-NEXT:      InstructionSelect
; AIE-O23-NEXT:      Remove dead machine instructions
; AIE-O23-NEXT:      AIE Post Select Optimizer
; AIE-O23-NEXT:      Remove dead machine instructions
; AIE-O23-NEXT:      ResetMachineFunction
; AIE-O23-NEXT:      Finalize ISel and expand pseudo-instructions
; AIE-O23-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O23-NEXT:      Early Tail Duplication
; AIE-O23-NEXT:      Optimize machine instruction PHIs
; AIE-O23-NEXT:      Slot index numbering
; AIE-O23-NEXT:      Merge disjoint stack slots
; AIE-O23-NEXT:      Local Stack Slot Allocation
; AIE-O23-NEXT:      Remove dead machine instructions
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Machine Natural Loop Construction
; AIE-O23-NEXT:      Machine Trace Metrics
; AIE-O23-NEXT:      Early If-Conversion
; AIE-O23-NEXT:      Machine Block Frequency Analysis
; AIE-O23-NEXT:      Function Alias Analysis Results
; AIE-O23-NEXT:      Early Machine Loop Invariant Code Motion
; AIE-O23-NEXT:      Machine LICM for reserved regs
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Machine Block Frequency Analysis
; AIE-O23-NEXT:      Machine Common Subexpression Elimination
; AIE-O23-NEXT:      MachinePostDominator Tree Construction
; AIE-O23-NEXT:      Machine Cycle Info Analysis
; AIE-O23-NEXT:      Machine code sinking
; AIE-O23-NEXT:      Peephole Optimizations
; AIE-O23-NEXT:      Remove dead machine instructions
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Slot index numbering
; AIE-O23-NEXT:      Live Interval Analysis
; AIE-O23-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O23-NEXT:      Machine Optimization Remark Emitter
; AIE-O23-NEXT:      Modulo Software Pipelining
; AIE-O23-NEXT:      Remove dead machine instructions
; AIE-O23-NEXT:      Detect Dead Lanes
; AIE-O23-NEXT:      Init Undef Pass
; AIE-O23-NEXT:      Process Implicit Definitions
; AIE-O23-NEXT:      Remove unreachable machine basic blocks
; AIE-O23-NEXT:      Live Variable Analysis
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Machine Natural Loop Construction
; AIE-O23-NEXT:      Eliminate PHI nodes for register allocation
; AIE-O23-NEXT:      AIE sub-reg constrainer
; AIE-O23-NEXT:      Two-Address instruction pass
; AIE-O23-NEXT:      Slot index numbering
; AIE-O23-NEXT:      Live Interval Analysis
; AIE-O23-NEXT:      Register Coalescer
; AIE-O23-NEXT:      Machine Instruction Scheduler
; AIE-O23-NEXT:      Register Coalescer
; AIE-O23-NEXT:      AIE 2D/3D operand splitter
; AIE-O23-NEXT:      Machine Block Frequency Analysis
; AIE-O23-NEXT:      Live Interval Analysis
; AIE-O23-NEXT:      Debug Variable Analysis
; AIE-O23-NEXT:      Live Stack Slot Analysis
; AIE-O23-NEXT:      Virtual Register Map
; AIE-O23-NEXT:      Live Register Matrix
; AIE-O23-NEXT:      Bundle Machine CFG Edges
; AIE-O23-NEXT:      Spill Code Placement Analysis
; AIE-O23-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O23-NEXT:      Machine Optimization Remark Emitter
; AIE-O23-NEXT:      Greedy Register Allocator
; AIE-O23-NEXT:      AIE super-reg rewrite
; AIE-O23-NEXT:      Greedy Register Allocator
; AIE-O23-NEXT:      AIE super-reg rewrite
; AIE-O23-NEXT:      AIE unallocated super-reg rewrite
; AIE-O23-NEXT:      Greedy Register Allocator
; AIE-O23-NEXT:      AIE waw-reg rewrite
; AIE-O23-NEXT:      AIE epilogue register rewrite
; AIE-O23-NEXT:      Greedy Register Allocator
; AIE-O23-NEXT:      Virtual Register Rewriter
; AIE-O23-NEXT:      Stack Slot Coloring
; AIE-O23-NEXT:      AIE Spill Slot Optimization
; AIE-O23-NEXT:      AIE 1D operands to 2D/3D rewriter
; AIE-O23-NEXT:      Machine Copy Propagation Pass
; AIE-O23-NEXT:      Machine Loop Invariant Code Motion
; AIE-O23-NEXT:      Remove Redundant DEBUG_VALUE analysis
; AIE-O23-NEXT:      Fixup Statepoint Caller Saved
; AIE-O23-NEXT:      PostRA Machine Sink
; AIE-O23-NEXT:      Machine Block Frequency Analysis
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      MachinePostDominator Tree Construction
; AIE-O23-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O23-NEXT:      Machine Optimization Remark Emitter
; AIE-O23-NEXT:      Shrink Wrapping analysis
; AIE-O23-NEXT:      Prologue/Epilogue Insertion & Frame Finalization
; AIE-O23-NEXT:      Machine Late Instructions Cleanup Pass
; AIE-O23-NEXT:      Control Flow Optimizer
; AIE-O23-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O23-NEXT:      Tail Duplication
; AIE-O23-NEXT:      Machine Copy Propagation Pass
; AIE-O23-NEXT:      Machine Copy Propagation Pass
; AIE-O23-NEXT:      Post-RA pseudo instruction expansion pass
; AIE-O23-NEXT:      Remove dead machine instructions
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Machine Natural Loop Construction
; AIE-O23-NEXT:      Machine Block Frequency Analysis
; AIE-O23-NEXT:      MachinePostDominator Tree Construction
; AIE-O23-NEXT:      Branch Probability Basic Block Placement
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Machine Natural Loop Construction
; AIE-O23-NEXT:      ReachingDefAnalysis
; AIE-O23-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O23-NEXT:      Machine Optimization Remark Emitter
; AIE-O23-NEXT:      AIE Hardware Loops pass
; AIE-O23-NEXT:      AIE pseudo branch expansion
; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Machine Natural Loop Construction
; AIE-O23-NEXT:      PostRA Machine Instruction Scheduler
; AIE-O23-NEXT:      AIE Bundle Finalization
; AIE-O23-NEXT:      AIE Machine Alignment
; AIE-O23-NEXT:      Analyze Machine Code For Garbage Collection
; AIE-O23-NEXT:      Insert fentry calls
; AIE-O23-NEXT:      Insert XRay ops
; AIE-O23-NEXT:      Implement the 'patchable-function' attribute
; AIE-O23-NEXT:      Contiguously Lay Out Funclets
; AIE-O23-NEXT:      Remove Loads Into Fake Uses
; AIE-O23-NEXT:      StackMap Liveness Analysis
; AIE-O23-NEXT:      Live DEBUG_VALUE analysis
; AIE-O23-NEXT:      Machine Sanitizer Binary Metadata
; AIE-O23-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O23-NEXT:      Machine Optimization Remark Emitter
; AIE-O23-NEXT:      Stack Frame Layout Analysis
; AIE-O23-NEXT:      AIE2 Assembly Printer
; AIE-O23-NEXT:      Free MachineFunction

define void @empty() {
  ret void
}
