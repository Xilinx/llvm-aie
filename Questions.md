# Claude.md
please focus for now on data gathering and categorization. explain to me all the failures. 


# Missing Understanding
how does bumping of the scheduling work? 
what happens if we need more cycles than what the current schedule approves? 
what happens with fixed instructions? what happens with already scheduled free instructions? 

# conservative Scoreboard filling
can we check what instruction in the ISA has the first Resource usage at a function jump location and use that instead of a blanko block of the scoreboard? 

# Branch Delay Slots
Regression llvm/test/CodeGen/AIE/aie2/ra/tie-subregs-flow-3d.mir
loop regression: llvm/test/CodeGen/AIE/aie2/schedule/loopaware/loop-epilogue.mir

llvm/test/CodeGen/AIE/aie2/bfloat16.ll
llvm/test/CodeGen/AIE/aie2/conv2d_offset_test.ll
llvm/test/CodeGen/AIE/aie2/extract.ll
llvm/test/CodeGen/AIE/aie2/fadd.ll
llvm/test/CodeGen/AIE/aie2/float_to_bfloat16.ll
llvm/test/CodeGen/AIE/aie2/fsub.ll
llvm/test/CodeGen/AIE/aie2/intrinsics-128bit.ll

wrong test update: 
llvm/test/CodeGen/AIE/aie2/schedule/small.mir
