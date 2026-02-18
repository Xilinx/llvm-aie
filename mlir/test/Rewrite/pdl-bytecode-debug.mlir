// Modifications Copyright (C) 2025 Advanced Micro Devices, Inc.
// These tests are copied from mlir/test/Rewrite/pdl-bytecode.mlir

// RUN: mlir-opt %s -test-pdl-bytecode-pass -split-input-file --debug 2>&1 | FileCheck %s

//===----------------------------------------------------------------------===//
// pdl_interp::ApplyConstraintOp
//===----------------------------------------------------------------------===//

module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    pdl_interp.apply_constraint "multi_entity_constraint"(%root, %root : !pdl.operation, !pdl.operation) -> ^pat, ^end

  ^pat:
    pdl_interp.apply_constraint "single_entity_constraint"(%root : !pdl.operation) -> ^pat2, ^end

  ^pat2:
    pdl_interp.record_match @rewriters::@success(%root : !pdl.operation) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation) {
      %op = pdl_interp.create_operation "test.replaced_by_pattern"
      pdl_interp.erase %root
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.op'
// CHECK: Executing ApplyConstraint multi_entity_constraint
// CHECK: Constraint succeeded
// CHECK: Executing ApplyConstraint single_entity_constraint
// CHECK: Constraint succeeded
module @ir attributes { test.apply_constraint_1 } {
  "test.op"() { test_attr } : () -> ()
}

// // -----

module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    %results = pdl_interp.get_results of %root : !pdl.range<value>
    %types = pdl_interp.get_value_type of %results : !pdl.range<type>
    pdl_interp.apply_constraint "multi_entity_var_constraint"(%results, %types : !pdl.range<value>, !pdl.range<type>) -> ^pat, ^end

  ^pat:
    pdl_interp.record_match @rewriters::@success(%root : !pdl.operation) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation) {
      %op = pdl_interp.create_operation "test.replaced_by_pattern"
      pdl_interp.erase %root
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.success_op'
// CHECK: Executing ApplyConstraint multi_entity_var_constraint
// CHECK: Constraint succeeded

// CHECK-LABEL: Processing operation : 'test.failure_op'
// CHECK: Executing ApplyConstraint multi_entity_var_constraint
// CHECK: Constraint failed
module @ir attributes { test.apply_constraint_2 } {
  "test.failure_op"() { test_attr } : () -> ()
  "test.success_op"() : () -> (i32, i64)
}

// -----

// Test support for negated constraints.
module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    %test_attr = pdl_interp.create_attribute unit
    %attr = pdl_interp.get_attribute "test_attr" of %root
    pdl_interp.are_equal %test_attr, %attr : !pdl.attribute -> ^pat, ^end

  ^pat:
    pdl_interp.apply_constraint "single_entity_constraint"(%root : !pdl.operation) {isNegated = true} -> ^pat1, ^end

  ^pat1:
    pdl_interp.record_match @rewriters::@success(%root : !pdl.operation) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation) {
      %op = pdl_interp.create_operation "test.replaced_by_pattern"
      pdl_interp.erase %root
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.op'
// CHECK: Executing ApplyConstraint single_entity_constraint
// CHECK: Constraint succeeded

// CHECK-LABEL: Processing operation : 'test.foo'
// CHECK: Executing ApplyConstraint single_entity_constraint
// CHECK: Constraint failed
module @ir attributes { test.apply_constraint_3 } {
  "test.foo"() { test_attr } : () -> ()
  "test.op"() { test_attr } : () -> ()
}

// -----

// Test returning a type from a native constraint.
module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    pdl_interp.check_operation_name of %root is "test.success_op" -> ^pat, ^end

  ^pat:
    %new_type = pdl_interp.apply_constraint "op_constr_return_type"(%root : !pdl.operation) : !pdl.type -> ^pat2, ^end

  ^pat2:
    pdl_interp.record_match @rewriters::@success(%root, %new_type : !pdl.operation, !pdl.type) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation, %new_type : !pdl.type) {
      %op = pdl_interp.create_operation "test.replaced_by_pattern" -> (%new_type : !pdl.type)
      pdl_interp.erase %root
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.success_op'
// CHECK: Executing ApplyConstraint op_constr_return_type
// CHECK: Constraint succeeded
module @ir attributes { test.apply_constraint_4 } {
  "test.failure_op"() : () -> ()
  "test.success_op"() : () -> ()
}

// -----

// Test returning a type from a native constraint.
module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    %new_type:2 = pdl_interp.apply_constraint "op_multiple_returns_failure"(%root : !pdl.operation) : !pdl.type, !pdl.type -> ^pat2, ^end

  ^pat2:
    pdl_interp.record_match @rewriters::@success(%root, %new_type#0 : !pdl.operation, !pdl.type) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation, %new_type : !pdl.type) {
      %op = pdl_interp.create_operation "test.replaced_by_pattern" -> (%new_type : !pdl.type)
      pdl_interp.erase %root
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.success_op'
// CHECK: Executing ApplyConstraint op_multiple_returns_failure
// CHECK: Constraint failed
module @ir attributes { test.apply_constraint_multi_result_failure } {
  "test.success_op"() : () -> ()
}

// -----

// Test success and failure cases of native constraints with pdl.range results.
module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    pdl_interp.check_operation_name of %root is "test.success_op" -> ^pat, ^end
  
  ^pat:
    %num_results = pdl_interp.create_attribute 2 : i32
    %types = pdl_interp.apply_constraint "op_constr_return_type_range"(%root, %num_results : !pdl.operation, !pdl.attribute) : !pdl.range<type> -> ^pat1, ^end

  ^pat1:
    pdl_interp.record_match @rewriters::@success(%root, %types : !pdl.operation, !pdl.range<type>) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation, %types : !pdl.range<type>) {
      %op = pdl_interp.create_operation "test.replaced_by_pattern" -> (%types : !pdl.range<type>)
      pdl_interp.erase %root
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.success_op'
// CHECK: Executing ApplyConstraint op_constr_return_type_range
// CHECK: Constraint succeeded
module @ir attributes { test.apply_constraint_5 } {
  "test.failure_op"() : () -> ()
  "test.success_op"() : () -> ()
}

// -----

//===----------------------------------------------------------------------===//
// pdl_interp::ApplyRewriteOp
//===----------------------------------------------------------------------===//

module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    pdl_interp.check_operation_name of %root is "test.op" -> ^pat, ^end

  ^pat:
    pdl_interp.record_match @rewriters::@success(%root : !pdl.operation) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation) {
      %operand = pdl_interp.get_operand 0 of %root
      pdl_interp.apply_rewrite "rewriter"(%root, %operand : !pdl.operation, !pdl.value)
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.op'
// CHECK: Executing ApplyRewrite rewriter
module @ir attributes { test.apply_rewrite_1 } {
  %input = "test.op_input"() : () -> i32
  "test.op"(%input) : (i32) -> ()
}

// -----

module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    pdl_interp.check_operation_name of %root is "test.op" -> ^pat, ^end

  ^pat:
    pdl_interp.record_match @rewriters::@success(%root : !pdl.operation) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation) {
      %op = pdl_interp.apply_rewrite "creator"(%root : !pdl.operation) : !pdl.operation
      pdl_interp.erase %root
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.op'
// CHECK: Executing ApplyRewrite creator
module @ir attributes { test.apply_rewrite_2 } {
  "test.op"() : () -> ()
}

// -----

module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    pdl_interp.check_operation_name of %root is "test.op" -> ^pat, ^end

  ^pat:
    pdl_interp.record_match @rewriters::@success(%root : !pdl.operation) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation) {
      %operands, %types = pdl_interp.apply_rewrite "var_creator"(%root : !pdl.operation) : !pdl.range<value>, !pdl.range<type>
      %op = pdl_interp.create_operation "test.success"(%operands : !pdl.range<value>) -> (%types : !pdl.range<type>)
      pdl_interp.replace %root with (%operands : !pdl.range<value>)
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.op'
// CHECK: Executing ApplyRewrite var_creator
module @ir attributes { test.apply_rewrite_3 } {
  %first_operand = "test.producer"() : () -> (i32)
  %operand = "test.op"(%first_operand) : (i32) -> (i32)
  "test.consumer"(%operand) : (i32) -> ()
}

// -----

module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    pdl_interp.check_operation_name of %root is "test.op" -> ^pat, ^end

  ^pat:
    pdl_interp.record_match @rewriters::@success(%root : !pdl.operation) : benefit(1), loc([%root]) -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @success(%root : !pdl.operation) {
      %attr = pdl_interp.apply_rewrite "str_creator" : !pdl.attribute
      %type = pdl_interp.apply_rewrite "type_creator" : !pdl.type
      %newOp = pdl_interp.create_operation "test.success" {"attr" = %attr} -> (%type : !pdl.type)
      pdl_interp.erase %root
      pdl_interp.finalize
    }
  }
}

// CHECK-LABEL: Processing operation : 'test.op'
// CHECK: Executing ApplyRewrite str_creator
// CHECK: Executing ApplyRewrite type_creator
module @ir attributes { test.apply_rewrite_4 } {
  "test.op"() : () -> ()
}

// -----


module @patterns {
  pdl_interp.func @matcher(%root : !pdl.operation) {
    pdl_interp.check_operation_name of %root is "test.op" -> ^pat, ^end

  ^pat:
    pdl_interp.record_match @rewriters::@named_pattern(%root : !pdl.operation) : benefit(1), loc([%root]), root("test.op") -> ^end

  ^end:
    pdl_interp.finalize
  }

  module @rewriters {
    pdl_interp.func @named_pattern(%root : !pdl.operation) {
      %op = pdl_interp.create_operation "test.replaced_by_pattern"
      pdl_interp.erase %root
      pdl_interp.finalize
    }
  }
}

// CHECK: Pattern named_pattern
module @ir {
  "test.op"() : () -> ()
}
