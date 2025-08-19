// RUN: mlir-opt -allow-unregistered-dialect %s -pass-pipeline='builtin.module(func.func(cse))' -mlir-print-debuginfo | FileCheck %s

// CHECK-LABEL: @many
func.func @many(f32, f32) -> (f32, f32) {
^bb0(%a : f32, %b : f32):
  // All operations have locations. Must have locations of Add0, Add1, Add2, Add3.
  %c = arith.addf %a, %b : f32 loc(#loc0)
  %d = arith.addf %a, %b : f32 loc(#loc1)
  %e = arith.addf %a, %b : f32 loc(#loc2)
  %f = arith.addf %a, %b : f32 loc(#loc3)
  // CHECK-NEXT: %[[VAR_0:[0-9a-zA-Z_]+]] = arith.addf %{{.*}}, %{{.*}} : f32 loc([[LOC_ABCD:.*]])

  // First operation has unknown location. Must have locations of Add0, Add1, Add2.
  %g = arith.addf %c, %d : f32 loc(#loc)
  %h = arith.addf %e, %f : f32 loc(#loc0)
  %i = arith.addf %c, %e : f32 loc(#fused_loc0)
  // CHECK-NEXT: %[[VAR_1:[0-9a-zA-Z_]+]] = arith.addf %[[VAR_0]], %[[VAR_0]] : f32 loc([[LOC_ABC:.*]])

  // Last operation has unknown location. Must have locations of Add2, Add3.
  %j = arith.addf %g, %h : f32 loc(#fused_loc1)
  %k = arith.addf %h, %i : f32 loc(#loc)
  // CHECK-NEXT: %[[VAR_2:[0-9a-zA-Z_]+]] = arith.addf %[[VAR_1]], %[[VAR_1]] : f32 loc([[LOC_CD:.*]])

  // Two operations with fused locations. Must have locations of Add1, Add2, Add3.
  %l = arith.addf %j, %k : f32 loc(#fused_loc0)
  %m = arith.addf %j, %k : f32 loc(#fused_loc1)
  // CHECK-NEXT: %[[VAR_3:[0-9a-zA-Z_]+]] = arith.addf %[[VAR_2]], %[[VAR_2]] : f32 loc([[LOC_BCD:.*]])

  // CHECK-NEXT: return %[[VAR_3]], %[[VAR_3]] : f32, f32
  return %l, %m : f32, f32
}
#loc = loc(unknown)
#loc0 = loc("Add0")
#loc1 = loc("Add1")
#loc2 = loc("Add2")
#loc3 = loc("Add3")

#fused_loc0 = loc(fused[#loc1, #loc2])
#fused_loc1 = loc(fused[#loc2, #loc3])

// CHECK-DAG: #[[LOC_A:.*]] = loc("Add0")
// CHECK-DAG: #[[LOC_B:.*]] = loc("Add1")
// CHECK-DAG: #[[LOC_C:.*]] = loc("Add2")
// CHECK-DAG: #[[LOC_D:.*]] = loc("Add3")
// CHECK-DAG: [[LOC_ABCD]] = loc(fused[#[[LOC_A]], #[[LOC_B]], #[[LOC_C]], #[[LOC_D]]])
// CHECK-DAG: [[LOC_ABC]] = loc(fused[#[[LOC_A]], #[[LOC_B]], #[[LOC_C]]])
// CHECK-DAG: [[LOC_BCD]] = loc(fused[#[[LOC_B]], #[[LOC_C]], #[[LOC_D]]])
// CHECK-DAG: [[LOC_CD]] = loc(fused[#[[LOC_C]], #[[LOC_D]]])