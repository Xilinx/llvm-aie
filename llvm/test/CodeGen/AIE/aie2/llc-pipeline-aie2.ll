;
; This file is licensed under the Apache License v2.0 with LLVM Exceptions.
; See https://llvm.org/LICENSE.txt for license information.
; SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
;
; (c) Copyright 2023-2024 Advanced Micro Devices, Inc. or its affiliates

; When EXPENSIVE_CHECKS are enabled, the machine verifier appears between each
; pass. Ignore it with 'grep -v'.

; RUN: llc -O0 -mtriple=aie2 -disable-verify -debug-pass=Structure < %s 2>&1 \
; RUN:   | grep -v 'Verify generated machine code' | FileCheck -match-full-lines -strict-whitespace -check-prefixes=AIE-O0,AIE-O0123 %s
; RUN: llc -O1 -mtriple=aie2 -disable-verify -debug-pass=Structure < %s 2>&1 \
; RUN:   | grep -v 'Verify generated machine code' | FileCheck -match-full-lines -strict-whitespace -check-prefixes=AIE-O1,AIE-O123,AIE-O0123 %s
; RUN: llc -O2 -mtriple=aie2 -disable-verify -debug-pass=Structure < %s 2>&1 \
; RUN:   | grep -v 'Verify generated machine code' | FileCheck -match-full-lines -strict-whitespace -check-prefixes=AIE-O23,AIE-O123,AIE-O0123 %s
; RUN: llc -O3 -mtriple=aie2 -disable-verify -debug-pass=Structure < %s 2>&1 \
; RUN:   | grep -v 'Verify generated machine code' | FileCheck -match-full-lines -strict-whitespace -check-prefixes=AIE-O23,AIE-O123,AIE-O0123 %s

; REQUIRES: asserts

; AIE-O0123:Target Library Information
; AIE-O0123-NEXT:Target Pass Configuration
; AIE-O0123-NEXT:Machine Module Information
; AIE-O0123-NEXT:Target Transform Information

; AIE-O123-NEXT:AIE complex addressing modes based Alias Analysis
; AIE-O123-NEXT:External Alias Analysis
; AIE-O123-NEXT:Assumption Cache Tracker
; AIE-O123-NEXT:Type-Based Alias Analysis
; AIE-O123-NEXT:Scoped NoAlias Alias Analysis
; AIE-O123-NEXT:Profile summary info

; AIE-O0123-NEXT:Create Garbage Collector Module Metadata

; AIE-O0-NEXT:Assumption Cache Tracker

; AIE-O0123-NEXT:Machine Branch Probability Analysis

; AIE-O123-NEXT:Default Regalloc Eviction Advisor
; AIE-O123-NEXT:Default Regalloc Priority Advisor

; AIE-O0123-NEXT:  ModulePass Manager
; AIE-O0123-NEXT:    Pre-ISel Intrinsic Lowering
; AIE-O0123-NEXT:    FunctionPass Manager
; AIE-O0123-NEXT:      Expand large div/rem
; AIE-O0123-NEXT:      Expand large fp convert
; AIE-O0123-NEXT:      Expand Atomic instructions
; AIE-O123-NEXT:      Infer address spaces
; AIE-O123-NEXT:      Dominator Tree Construction
; AIE-O123-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O123-NEXT:      Natural Loop Information
; AIE-O123-NEXT:      Canonicalize natural loops
; AIE-O123-NEXT:      Scalar Evolution Analysis
; AIE-O123-NEXT:      Loop Pass Manager
; AIE-O123-NEXT:        Canonicalize Freeze Instructions in Loops
; AIE-O123-NEXT:        Induction Variable Users
; AIE-O123-NEXT:        Loop Strength Reduction
; AIE-O123-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O123-NEXT:      Function Alias Analysis Results
; AIE-O123-NEXT:      Merge contiguous icmps into a memcmp
; AIE-O123-NEXT:      Natural Loop Information
; AIE-O123-NEXT:      Lazy Branch Probability Analysis
; AIE-O123-NEXT:      Lazy Block Frequency Analysis
; AIE-O123-NEXT:      Expand memcmp() to load/stores

; AIE-O0123-NEXT:      Lower Garbage Collection Instructions
; AIE-O0123-NEXT:      Shadow Stack GC Lowering
; AIE-O0123-NEXT:      Lower constant intrinsics
; AIE-O0123-NEXT:      Remove unreachable blocks from the CFG

; AIE-O123-NEXT:      Natural Loop Information
; AIE-O123-NEXT:      Post-Dominator Tree Construction
; AIE-O123-NEXT:      Branch Probability Analysis
; AIE-O123-NEXT:      Block Frequency Analysis
; AIE-O123-NEXT:      Constant Hoisting
; AIE-O123-NEXT:      Replace intrinsics with calls to vector library
; AIE-O123-NEXT:      Partially inline calls to library functions

; AIE-O0123-NEXT:      Expand vector predication intrinsics
; AIE-O0123-NEXT:      Scalarize Masked Memory Intrinsics
; AIE-O0123-NEXT:      Expand reduction intrinsics

; AIE-O123-NEXT:      Natural Loop Information
; AIE-O123-NEXT:      TLS Variable Hoist
; AIE-O123-NEXT:      CodeGen Prepare

; AIE-O0123-NEXT:      Lower invoke and unwind, for unwindless code generators
; AIE-O0123-NEXT:      Remove unreachable blocks from the CFG
; AIE-O123-NEXT:      Dominator Tree Construction
; AIE-O123-NEXT:      Natural Loop Information
; AIE-O123-NEXT:      Scalar Evolution Analysis
; AIE-O123-NEXT:      Lazy Branch Probability Analysis
; AIE-O123-NEXT:      Lazy Block Frequency Analysis
; AIE-O123-NEXT:      Optimization Remark Emitter
; AIE-O123-NEXT:      Hardware Loop Insertion

; AIE-O0123-NEXT:      Prepare callbr
; AIE-O0123-NEXT:      Safe Stack instrumentation pass
; AIE-O0123-NEXT:      Insert stack protectors
; AIE-O0123-NEXT:      Analysis containing CSE Info
; AIE-O123-NEXT:      Natural Loop Information
; AIE-O123-NEXT:      Post-Dominator Tree Construction
; AIE-O123-NEXT:      Branch Probability Analysis
; AIE-O123-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O123-NEXT:      Function Alias Analysis Results

; AIE-O0123-NEXT:      IRTranslator

; AIE-O0123-NEXT:      Analysis containing CSE Info
; AIE-O0123-NEXT:      AIE Address Space Flattening Pass

; AIE-O123-NEXT:      Analysis for ComputingKnownBits
; AIE-O123-NEXT:      MachineDominator Tree Construction

; AIE-O123-NEXT:      AIE2PreLegalizerCombiner
; AIE-O0123-NEXT:      AIE Eliminate Duplicate PHI Pass

; AIE-O0-NEXT:      Analysis for ComputingKnownBits
; AIE-O0123-NEXT:      Legalizer

; AIE-O123-NEXT:      MachineDominator Tree Construction
; AIE-O123-NEXT:      AIE2 Post Legalizer Generic Combiner
; AIE-O123-NEXT:      AIE Base Address Clustering Optimization
; AIE-O123-NEXT:      AIE2 Post Legalizer Custom Combiner

; AIE-O0123-NEXT:      RegBankSelect
; AIE-O0123-NEXT:      Analysis for ComputingKnownBits

; AIE-O123-NEXT:      Lazy Branch Probability Analysis
; AIE-O123-NEXT:      Lazy Block Frequency Analysis

; AIE-O0123-NEXT:      InstructionSelect
; AIE-O123-NEXT:      Remove dead machine instructions
; AIE-O123-NEXT:      AIE Post Select Optimizer
; AIE-O123-NEXT:      Remove dead machine instructions
; AIE-O0123-NEXT:      ResetMachineFunction
; AIE-O0123-NEXT:      Finalize ISel and expand pseudo-instructions

; AIE-O123-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O123-NEXT:      Early Tail Duplication
; AIE-O123-NEXT:      Optimize machine instruction PHIs
; AIE-O123-NEXT:      Slot index numbering
; AIE-O123-NEXT:      Merge disjoint stack slots

; AIE-O0123-NEXT:      Local Stack Slot Allocation

; AIE-O123-NEXT:      Remove dead machine instructions
; AIE-O123-NEXT:      MachineDominator Tree Construction
; AIE-O123-NEXT:      Machine Natural Loop Construction
; AIE-O123-NEXT:      Machine Block Frequency Analysis
; AIE-O123-NEXT:      Function Alias Analysis Results
; AIE-O123-NEXT:      Early Machine Loop Invariant Code Motion
; AIE-O123-NEXT:      Machine LICM for reserved regs
; AIE-O123-NEXT:      MachineDominator Tree Construction
; AIE-O123-NEXT:      Machine Block Frequency Analysis
; AIE-O123-NEXT:      Machine Common Subexpression Elimination
; AIE-O123-NEXT:      MachinePostDominator Tree Construction
; AIE-O123-NEXT:      Machine Cycle Info Analysis
; AIE-O123-NEXT:      Machine code sinking
; AIE-O123-NEXT:      Peephole Optimizations
; AIE-O123-NEXT:      Remove dead machine instructions

; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Slot index numbering
; AIE-O23-NEXT:      Live Interval Analysis
; AIE-O23-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O23-NEXT:      Machine Optimization Remark Emitter
; AIE-O23-NEXT:      Modulo Software Pipelining
; AIE-O23-NEXT:      Remove dead machine instructions

; AIE-O123-NEXT:      Detect Dead Lanes
; AIE-O123-NEXT:      Init Undef Pass
; AIE-O123-NEXT:      Process Implicit Definitions
; AIE-O123-NEXT:      Remove unreachable machine basic blocks
; AIE-O123-NEXT:      Live Variable Analysis

; AIE-O23-NEXT:      MachineDominator Tree Construction
; AIE-O23-NEXT:      Machine Natural Loop Construction

; AIE-O0123-NEXT:      Eliminate PHI nodes for register allocation
; AIE-O0123-NEXT:      AIE sub-reg constrainer
; AIE-O0123-NEXT:      Two-Address instruction pass

; AIE-O0-NEXT:      Fast Register Allocator
; AIE-O0-NEXT:      Remove Redundant DEBUG_VALUE analysis
; AIE-O0-NEXT:      Fixup Statepoint Caller Saved
; AIE-O0-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O0-NEXT:      Machine Optimization Remark Emitter

; AIE-O1-NEXT:      MachineDominator Tree Construction
; AIE-O123-NEXT:      Slot index numbering
; AIE-O123-NEXT:      Live Interval Analysis
; AIE-O123-NEXT:      Register Coalescer
; AIE-O123-NEXT:      Machine Instruction Scheduler
; AIE-O123-NEXT:      Register Coalescer
; AIE-O123-NEXT:      AIE 2D/3D operand splitter
; AIE-O123-NEXT:      Machine Block Frequency Analysis
; AIE-O123-NEXT:      Live Interval Analysis
; AIE-O123-NEXT:      Debug Variable Analysis
; AIE-O123-NEXT:      Live Stack Slot Analysis
; AIE-O123-NEXT:      Virtual Register Map
; AIE-O123-NEXT:      Live Register Matrix
; AIE-O123-NEXT:      Bundle Machine CFG Edges
; AIE-O123-NEXT:      Spill Code Placement Analysis
; AIE-O123-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O123-NEXT:      Machine Optimization Remark Emitter
; AIE-O123-NEXT:      Greedy Register Allocator
; AIE-O123-NEXT:      AIE super-reg rewrite
; AIE-O123-NEXT:      Greedy Register Allocator
; AIE-O123-NEXT:      AIE super-reg rewrite
; AIE-O123-NEXT:      Greedy Register Allocator
; AIE-O123-NEXT:      AIE waw-reg rewrite
; AIE-O123-NEXT:      Virtual Register Rewriter
; AIE-O123-NEXT:      Stack Slot Coloring
; AIE-O123-NEXT:      AIE 1D operands to 2D/3D rewriter
; AIE-O123-NEXT:      Machine Copy Propagation Pass
; AIE-O123-NEXT:      Machine Loop Invariant Code Motion
; AIE-O123-NEXT:      Remove Redundant DEBUG_VALUE analysis
; AIE-O123-NEXT:      Fixup Statepoint Caller Saved
; AIE-O123-NEXT:      PostRA Machine Sink
; AIE-O123-NEXT:      Machine Block Frequency Analysis
; AIE-O123-NEXT:      MachineDominator Tree Construction
; AIE-O123-NEXT:      MachinePostDominator Tree Construction
; AIE-O123-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O123-NEXT:      Machine Optimization Remark Emitter
; AIE-O123-NEXT:      Shrink Wrapping analysis

; AIE-O0123-NEXT:      Prologue/Epilogue Insertion & Frame Finalization

; AIE-O123-NEXT:      Machine Late Instructions Cleanup Pass
; AIE-O123-NEXT:      Control Flow Optimizer
; AIE-O123-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O123-NEXT:      Tail Duplication
; AIE-O123-NEXT:      Machine Copy Propagation Pass
; AIE-O123-NEXT:      Machine Copy Propagation Pass

; AIE-O0123-NEXT:      Post-RA pseudo instruction expansion pass
; AIE-O0123-NEXT:      Remove dead machine instructions

; AIE-O123-NEXT:      MachineDominator Tree Construction
; AIE-O123-NEXT:      Machine Natural Loop Construction
; AIE-O123-NEXT:      Machine Block Frequency Analysis
; AIE-O123-NEXT:      MachinePostDominator Tree Construction
; AIE-O123-NEXT:      Branch Probability Basic Block Placement

; AIE-O0123-NEXT:      AIE Machine Block Alignment

; AIE-O123-NEXT:      MachineDominator Tree Construction
; AIE-O123-NEXT:      Machine Natural Loop Construction
; AIE-O123-NEXT:      ReachingDefAnalysis
; AIE-O123-NEXT:      AIE Hardware Loops pass

; AIE-O0123-NEXT:      AIE pseudo branch expansion
; AIE-O0123-NEXT:      MachineDominator Tree Construction
; AIE-O0123-NEXT:      Machine Natural Loop Construction

; AIE-O0-NEXT:      Dominator Tree Construction
; AIE-O0-NEXT:      Basic Alias Analysis (stateless AA impl)
; AIE-O0-NEXT:      Function Alias Analysis Results

; AIE-O0123-NEXT:      PostRA Machine Instruction Scheduler
; AIE-O0123-NEXT:      AIE Bundle Finalization
; AIE-O0123-NEXT:      AIE Machine Alignment
; AIE-O0123-NEXT:      Analyze Machine Code For Garbage Collection
; AIE-O0123-NEXT:      Insert fentry calls
; AIE-O0123-NEXT:      Insert XRay ops
; AIE-O0123-NEXT:      Implement the 'patchable-function' attribute
; AIE-O0123-NEXT:      Contiguously Lay Out Funclets
; AIE-O0123-NEXT:      StackMap Liveness Analysis
; AIE-O0123-NEXT:      Live DEBUG_VALUE analysis
; AIE-O0123-NEXT:      Machine Sanitizer Binary Metadata
; AIE-O0123-NEXT:      Lazy Machine Block Frequency Analysis
; AIE-O0123-NEXT:      Machine Optimization Remark Emitter
; AIE-O0123-NEXT:      Stack Frame Layout Analysis
; AIE-O0123-NEXT:      AIE2 Assembly Printer
; AIE-O0123-NEXT:      Free MachineFunction



define void @empty() {
  ret void
}
