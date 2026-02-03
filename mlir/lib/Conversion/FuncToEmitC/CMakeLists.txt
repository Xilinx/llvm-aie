# Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
add_mlir_conversion_library(MLIRFuncToEmitC
  FuncToEmitC.cpp
  FuncToEmitCPass.cpp

  ADDITIONAL_HEADER_DIRS
  ${MLIR_MAIN_INCLUDE_DIR}/mlir/Conversion/FuncToEmitC

  DEPENDS
  MLIRConversionPassIncGen

  LINK_LIBS PUBLIC
  MLIREmitCDialect
  MLIREmitCTransforms
  MLIRFuncDialect
  MLIRPass
  MLIRTransformUtils
  )
