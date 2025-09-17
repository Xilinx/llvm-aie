// RUN: mlir-opt -allow-unregistered-dialect %s -snapshot-op-locations='filename=%t print-debuginfo' -mlir-print-debuginfo -mlir-newline-after-attr=2 | FileCheck %s

// CHECK:module {
// CHECK-NEXT:  func.func @myfunc() -> i32 attributes {
// CHECK-NEXT:    attr1 = "foo",
// CHECK-NEXT:    attr2 = 42 : i32,
// CHECK-NEXT:    attr3 = true} {
// CHECK-NEXT:    %0 = "myop"() : () -> i32 loc(#loc2)
// CHECK-NEXT:    return %0 : i32 loc(#loc3)
// CHECK-NEXT:  } loc(#loc1)
// CHECK-NEXT:} loc(#loc)
// CHECK-NEXT:#loc = loc("{{.*}}":1:0)
// CHECK-NEXT:#loc1 = loc("{{.*}}":2:2)
// CHECK-NEXT:#loc2 = loc("{{.*}}":6:4)
// CHECK-NEXT:#loc3 = loc("{{.*}}":7:4)

func.func @myfunc() -> i32
attributes { attr1 = "foo", attr2 = 42 : i32, attr3 = true } {
%0 = "myop"() : () -> i32
return %0 : i32
}
