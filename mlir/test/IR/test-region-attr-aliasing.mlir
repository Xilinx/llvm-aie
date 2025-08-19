// RUN: mlir-opt %s | FileCheck %s

#map = affine_map<(d0) -> (d0)>
// CHECK: {builtin.test = #map}
func.func @test_attr_alias_on_region_attr(%arg0: memref<2xf32> {builtin.test = #map}) {
  %c0 = arith.constant 0 : index
  %c1 = arith.constant 0 : index
  %2 = memref.load %arg0[%c0] : memref<2xf32>
  memref.store %2, %arg0[%c1] : memref<2xf32>
  return
}
