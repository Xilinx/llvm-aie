# Modifications (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates
get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)

set(LLVM_LINK_COMPONENTS
  Demangle
  Support
  TableGen
)

if(MLIR_INCLUDE_TESTS)
  set(test_libs
    MLIRTestDialect
    )

endif()

set(LIBS
  ${dialect_libs}
  ${test_libs}
  MLIRIR
  MLIRRegisterAllDialects
  MLIRPDLLAST
  MLIRPDLLCodeGen
  MLIRPDLLODS
  MLIRPDLLParser
  )

add_tablegen(mlir-pdll MLIR_PDLL
  DESTINATION "${MLIR_TOOLS_INSTALL_DIR}"
  EXPORT MLIR
  mlir-pdll.cpp

  DEPENDS
  ${LIBS}
  )

target_link_libraries(mlir-pdll PRIVATE ${LIBS})

mlir_check_all_link_libraries(mlir-pdll)
