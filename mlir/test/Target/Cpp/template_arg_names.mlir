// RUN: mlir-translate -mlir-to-cpp %s | FileCheck %s -check-prefix=CPP-DEFAULT

// CPP-DEFAULT-LABEL: void basic
func.func @basic(%arg0: i32, %arg1: f32) {
  emitc.call_opaque "kernel3"(%arg0, %arg1)  : (i32, f32) -> ()
// CPP-DEFAULT:   kernel3(
  emitc.call_opaque "kernel4"(%arg0, %arg1)  {template_arg_names = ["N", "P"], template_args = [42 : i32, 56]} : (i32, f32) -> ()
// CPP-DEFAULT:   kernel4</*N=*/42, /*P=*/56>(
  emitc.call_opaque "kernel4"(%arg0, %arg1)  {template_arg_names = ["N"], template_args = [#emitc.opaque<"42">]} : (i32, f32) -> ()
// CPP-DEFAULT:   kernel4</*N=*/42>(
  return
}


