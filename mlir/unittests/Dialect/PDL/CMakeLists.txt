add_mlir_unittest(MLIRSPLBuiltinTests
  BuiltinTest.cpp
)
target_link_libraries(MLIRSPLBuiltinTests
  PRIVATE
  MLIRIR
  MLIRPDLDialect
)
