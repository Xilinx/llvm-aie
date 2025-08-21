// RUN: mlir-translate -mlir-to-cpp -constants-as-variables=false %s | FileCheck %s -check-prefix=CPP-DEFAULT

func.func @test() {
  %start = "emitc.constant"() <{value = 0 : index}> : () -> !emitc.size_t
  %stop = "emitc.constant"() <{value = 10 : index}> : () -> !emitc.size_t
  %step = "emitc.constant"() <{value = 1 : index}> : () -> !emitc.size_t

  emitc.for %iter = %start to %stop step %step {
    emitc.yield
  }

  return
}
// CPP-DEFAULT-LABEL: void test() {
// CPP-DEFAULT-NEXT:   for (size_t [[V1:[^ ]*]] = (size_t) 0; [[V1]] < (size_t) 10; [[V1]] += (size_t) 1) {
// CPP-DEFAULT-NEXT:   }
// CPP-DEFAULT-NEXT:   return;
// CPP-DEFAULT-NEXT: }

// -----

func.func @test_subscript(%arg0: !emitc.array<4xf32>) -> (!emitc.lvalue<f32>) {
  %c0 = "emitc.constant"() <{value = 0 : index}> : () -> !emitc.size_t
  %0 = emitc.subscript %arg0[%c0] : (!emitc.array<4xf32>, !emitc.size_t) -> !emitc.lvalue<f32>
  return %0 : !emitc.lvalue<f32>
}
// CPP-DEFAULT-LABEL: float test_subscript(float v1[4]) {
// CPP-DEFAULT-NEXT:  return v1[(size_t) 0];
// CPP-DEFAULT-NEXT: }

// -----

func.func @emitc_switch_ui64() {
  %0 = "emitc.constant"(){value = 1 : ui64} : () -> ui64

  emitc.switch %0 : ui64
  default {
    emitc.call_opaque "func2" (%0) : (ui64) -> ()
    emitc.yield
  }
  return
}
// CPP-DEFAULT-LABEL: void emitc_switch_ui64() {
// CPP-DEFAULT:   switch ((uint64_t) 1) {
// CPP-DEFAULT-NEXT:   default: {
// CPP-DEFAULT-NEXT:     func2((uint64_t) 1);
// CPP-DEFAULT-NEXT:     break;
// CPP-DEFAULT-NEXT:   }
// CPP-DEFAULT-NEXT:   }
// CPP-DEFAULT-NEXT:   return;
// CPP-DEFAULT-NEXT: }

// -----

func.func @negative_values() {
  %1 = "emitc.constant"() <{value = 10 : index}> : () -> !emitc.size_t
  %2 = "emitc.constant"() <{value = -3000000000 : index}> : () -> !emitc.ssize_t

  %3 = emitc.add %1, %2 : (!emitc.size_t, !emitc.ssize_t) -> !emitc.ssize_t

  return
}
// CPP-DEFAULT-LABEL: void negative_values() {
// CPP-DEFAULT-NEXT:   ssize_t v1 = (size_t) 10 + (ssize_t) -3000000000;
// CPP-DEFAULT-NEXT:   return;
// CPP-DEFAULT-NEXT: }
