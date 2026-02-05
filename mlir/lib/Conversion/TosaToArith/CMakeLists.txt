# Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
add_mlir_conversion_library(MLIRTosaToArith
  TosaToArith.cpp
  TosaToArithPass.cpp

  ADDITIONAL_HEADER_DIRS
  ${MLIR_MAIN_INCLUDE_DIR}/mlir/Dialect/Tosa
  ${MLIR_MAIN_INCLUDE_DIR}/mlir/IR

  DEPENDS
  MLIRConversionPassIncGen

  LINK_LIBS PUBLIC
  MLIRArithDialect
  MLIRIR
  MLIRPass
  MLIRTosaDialect
  MLIRTosaToLinalg
  MLIRTosaTransforms
  MLIRSupport
  )
