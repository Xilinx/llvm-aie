# Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
# affiliates

add_mlir_dialect_library(MLIREmitCTransforms
  Transforms.cpp
  FormExpressions.cpp
  EliminateLibm.cpp
  TypeConversions.cpp

  ADDITIONAL_HEADER_DIRS
  ${MLIR_MAIN_INCLUDE_DIR}/mlir/Dialect/EmitC/Transforms

  DEPENDS
  MLIREmitCTransformsIncGen
  MLIRFuncOpsIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRPass
  MLIREmitCDialect
  MLIRTransforms
)
