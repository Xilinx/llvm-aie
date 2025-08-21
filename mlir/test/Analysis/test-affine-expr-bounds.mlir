// RUN: mlir-opt -test-affine-expr-bounds --mlir-print-local-scope --allow-unregistered-dialect --verify-diagnostics %s | FileCheck %s

func.func @test_compute_affine_expr_bounds() {
  // Add

  // CHECK: "test.add"()
  // CHECK-SAME: expr_lb = 1
  // CHECK-SAME: expr_ub = 3
  "test.add"() {affine_map = affine_map<(d0) -> (d0 + 1)>, lbs = [0], ubs = [2]} : () -> ()

  // CHECK: "test.sub_const"()
  // CHECK-SAME: expr_lb = -1
  // CHECK-SAME: expr_ub = 1
  "test.sub_const"() {affine_map = affine_map<(d0) -> (d0 - 1)>, lbs = [0], ubs = [2]} : () -> ()

  // CHECK: "test.sub_dim"()
  // CHECK-SAME: expr_lb = -1
  // CHECK-SAME: expr_ub = 1
  "test.sub_dim"() {affine_map = affine_map<(d0) -> (1 - d0)>, lbs = [0], ubs = [2]} : () -> ()

  // Mul

  // CHECK: "test.mul"()
  // CHECK-SAME: expr_lb = 10
  // CHECK-SAME: expr_ub = 15
  "test.mul"() {affine_map = affine_map<(d0) -> (5 * d0)>, lbs = [2], ubs = [3]} : () -> ()

  // CHECK: "test.mul_neg"()
  // CHECK-SAME: expr_lb = -15
  // CHECK-SAME: expr_ub = -10
  "test.mul_neg"() {affine_map = affine_map<(d0) -> (-5 * d0)>, lbs = [2], ubs = [3]} : () -> ()

  // Mod

  // CHECK: "test.mod_basic"()
  // CHECK-SAME: expr_lb = 0
  // CHECK-SAME: expr_ub = 2
  "test.mod_basic"() {affine_map = affine_map<(d0) -> (d0 mod 5)>, lbs = [0], ubs = [2]} : () -> ()

  // CHECK: "test.mod_wrap_around_by_range"()
  // CHECK-SAME: expr_lb = 0
  // CHECK-SAME: expr_ub = 4
  "test.mod_wrap_around_by_range"() {affine_map = affine_map<(d0) -> (d0 mod 5)>, lbs = [0], ubs = [7]} : () -> ()

  // CHECK: "test.mod_wrap_around_by_sum"()
  // CHECK-SAME: expr_lb = 0
  // CHECK-SAME: expr_ub = 4
  "test.mod_wrap_around_by_sum"() {affine_map = affine_map<(d0) -> ((d0 + 3) mod 5)>, lbs = [0], ubs = [3]} : () -> ()

  // CHECK: "test.mod_not_wrapping_around"()
  // CHECK-SAME: expr_lb = 1
  // CHECK-SAME: expr_ub = 3
  "test.mod_not_wrapping_around"() {affine_map = affine_map<(d0) -> (((d0 + 12) mod 11) mod 5)>, lbs = [0], ubs = [2]} : () -> ()

  // CHECK: "test.mod_neg"()
  // CHECK-SAME: expr_lb = 1
  // CHECK-SAME: expr_ub = 3
  "test.mod_neg"() {affine_map = affine_map<(d0) -> (d0 mod 5)>, lbs = [-4], ubs = [-2]} : () -> ()

  // CHECK: "test.mod_wrapping_by_zero"()
  // CHECK-SAME: expr_lb = 0
  // CHECK-SAME: expr_ub = 4
  "test.mod_wrapping_by_zero"() {affine_map = affine_map<(d0) -> (d0 mod 5)>, lbs = [-2], ubs = [1]} : () -> ()

  // FloorDiv

  // CHECK: "test.floordiv_basic"()
  // CHECK-SAME: expr_lb = 0
  // CHECK-SAME: expr_ub = 1
  "test.floordiv_basic"() {affine_map = affine_map<(d0) -> (d0 floordiv 16)>, lbs = [0], ubs = [31]} : () -> ()

  // CHECK: "test.floordiv_not_stepping"()
  // CHECK-SAME: expr_lb = 1
  // CHECK-SAME: expr_ub = 1
  "test.floordiv_not_stepping"() {affine_map = affine_map<(d0) -> (d0 floordiv 16)>, lbs = [16], ubs = [31]} : () -> ()

  // CHECK: "test.floordiv_stepping_by_sum"()
  // CHECK-SAME: expr_lb = 1
  // CHECK-SAME: expr_ub = 2
  "test.floordiv_stepping_by_sum"() {affine_map = affine_map<(d0) -> ((d0 + 1) floordiv 16)>, lbs = [16], ubs = [31]} : () -> ()

  // CHECK: "test.floordiv_neg_factor"()
  // CHECK-SAME: expr_lb = -1
  // CHECK-SAME: expr_ub = 0
  "test.floordiv_neg_factor"() {affine_map = affine_map<(d0) -> (d0 floordiv -8)>, lbs = [0], ubs = [8]} : () -> ()

  // CHECK: "test.floordiv_neg_factor_not_stepping"()
  // CHECK-SAME: expr_lb = -1
  // CHECK-SAME: expr_ub = -1
  "test.floordiv_neg_factor_not_stepping"() {affine_map = affine_map<(d0) -> (d0 floordiv -8)>, lbs = [1], ubs = [8]} : () -> ()

  // CHECK: "test.floordiv_neg_range"()
  // CHECK-SAME: expr_lb = -1
  // CHECK-SAME: expr_ub = -1
  "test.floordiv_neg_range"() {affine_map = affine_map<(d0) -> (d0 floordiv 8)>, lbs = [-8], ubs = [-1]} : () -> ()

  // CeilDiv

  // CHECK: "test.ceildiv_basic"()
  // CHECK-SAME: expr_lb = 0
  // CHECK-SAME: expr_ub = 1
  "test.ceildiv_basic"() {affine_map = affine_map<(d0) -> (d0 ceildiv 16)>, lbs = [0], ubs = [16]} : () -> ()

  // CHECK: "test.ceildiv_not_stepping"()
  // CHECK-SAME: expr_lb = 1
  // CHECK-SAME: expr_ub = 1
  "test.ceildiv_not_stepping"() {affine_map = affine_map<(d0) -> (d0 ceildiv 16)>, lbs = [1], ubs = [16]} : () -> ()

  // CHECK: "test.ceildiv_stepping_by_sum"()
  // CHECK-SAME: expr_lb = 1
  // CHECK-SAME: expr_ub = 2
  "test.ceildiv_stepping_by_sum"() {affine_map = affine_map<(d0) -> ((d0 + 1) ceildiv 16)>, lbs = [1], ubs = [16]} : () -> ()

  // CHECK: "test.ceildiv_neg_factor"()
  // CHECK-SAME: expr_lb = -1
  // CHECK-SAME: expr_ub = 0
  "test.ceildiv_neg_factor"() {affine_map = affine_map<(d0) -> (d0 ceildiv -16)>, lbs = [1], ubs = [16]} : () -> ()

  // CHECK: "test.ceildiv_neg_factor_not_stepping"()
  // CHECK-SAME: expr_lb = 0
  // CHECK-SAME: expr_ub = 0
  "test.ceildiv_neg_factor_not_stepping"() {affine_map = affine_map<(d0) -> (d0 ceildiv -16)>, lbs = [0], ubs = [15]} : () -> ()

  // CHECK: "test.ceildiv_neg_range"()
  // CHECK-SAME: expr_lb = -1
  // CHECK-SAME: expr_ub = 0
  "test.ceildiv_neg_range"() {affine_map = affine_map<(d0) -> (d0 ceildiv 16)>, lbs = [-16], ubs = [-1]} : () -> ()

  return
}

// -----

func.func @test_bounds_unsigned() {
  // CHECK: "test.unsigned"()
  // CHECK-SAME: expr_lb = 0 : ui8
  // CHECK-SAME: expr_ub = 255 : ui8
  "test.unsigned"() {affine_map = affine_map<(d0) -> (d0)>, lbs = [0 : ui8], ubs = [255 : ui8]} : () -> ()

  // CHECK: "test.unsigned_wrapping"()
  // CHECK-SAME: expr_lb = 0 : ui8
  // CHECK-SAME: expr_ub = 255 : ui8
  "test.unsigned_wrapping"() {affine_map = affine_map<(d0) -> (d0 + 2)>, lbs = [253 : ui8], ubs = [255 : ui8]} : () -> ()

  // CHECK: "test.unsigned_wrap_full"()
  // CHECK-SAME: expr_lb = 0 : ui8
  // CHECK-SAME: expr_ub = 4 : ui8
  "test.unsigned_wrap_full"() {affine_map = affine_map<(d0) -> (d0 + 5)>, lbs = [251 : ui8], ubs = [255 : ui8]} : () -> ()

  return
}

// -----

func.func @test_unsigned_floordiv() {
  // Result should be lb = 1, ub = 1, but we're missing an unsigned floordiv computation.
  // expected-error @+1 {{Failed to compute bounds}}
  "test.unsigned_floordiv"() {affine_map = affine_map<(d0) -> (d0 floordiv 128)>, lbs = [129 : ui8], ubs = [129 : ui8]} : () -> ()

}

// -----

func.func @test_bounds_signed() {
  // CHECK: "test.signed"()
  // CHECK-SAME: expr_lb = -1 : i8
  // CHECK-SAME: expr_ub = 0 : i8
  "test.signed"() {affine_map = affine_map<(d0) -> (d0 floordiv 16)>, lbs = [-1 : i8], ubs = [0 : i8]} : () -> ()

  // CHECK: "test.signed_wrapping"()
  // CHECK-SAME: expr_lb = -128 : i8
  // CHECK-SAME: expr_ub = 127 : i8
  "test.signed_wrapping"() {affine_map = affine_map<(d0) -> (d0 + 3)>, lbs = [124 : i8], ubs = [127 : i8]} : () -> ()

  // CHECK: "test.signed_wrap_full"()
  // CHECK-SAME: expr_lb = -128 : i8
  // CHECK-SAME: expr_ub = -125 : i8
  "test.signed_wrap_full"() {affine_map = affine_map<(d0) -> (d0 + 4)>, lbs = [124 : i8], ubs = [127 : i8]} : () -> ()

  return
}

// -----

func.func @test_dynamic_lb_basic() {
  // expected-error @+1 {{Failed to compute bounds}}
  "test.dynamic_lb_basic"() {affine_map = affine_map<(d0) -> (d0)>, lbs = ["?"], ubs = [1]} : () -> ()
  return
}

// -----

func.func @test_dynamic_ub_basic() {
  // expected-error @+1 {{Failed to compute bounds}}
  "test.dynamic_ub_basic"() {affine_map = affine_map<(d0) -> (d0)>, lbs = [0], ubs = ["?"]} : () -> ()
  return
}

// -----

func.func @test_dynamic_lb_unused() {
  // CHECK: "test.dynamic_lb_unused"()
  // CHECK-SAME: expr_lb = 14
  // CHECK-SAME: expr_ub = 16
  "test.dynamic_lb_unused"() {affine_map = affine_map<(d0, d1) -> (d1 + 2)>, lbs = ["?", 12], ubs = [1, 14]} : () -> ()
  return
}

// -----

func.func @test_dynamic_ub_unused() {
  // CHECK: "test.dynamic_ub_unused"()
  // CHECK-SAME: expr_lb = 14
  // CHECK-SAME: expr_ub = 16
  "test.dynamic_ub_unused"() {affine_map = affine_map<(d0, d1) -> (d1 + 2)>, lbs = [0, 12], ubs = ["?", 14]} : () -> ()
  return
}
