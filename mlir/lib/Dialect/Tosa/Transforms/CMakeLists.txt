# Modifications (c) Copyright 2025 Advanced Micro Devices, Inc. or its affiliates

add_mlir_dialect_library(MLIRTosaTransforms
  TosaDecomposeTransposeConv.cpp
  TosaDecomposeDepthwise.cpp
  TosaFolders.cpp
  TosaInferShapes.cpp
  TosaLayerwiseConstantFoldPass.cpp
  TosaMakeBroadcastable.cpp
  TosaOptionalDecompositions.cpp
  TosaReduceTransposes.cpp
  TosaTypeConverters.cpp
  TosaValidation.cpp
  SinkInputOpsThroughConcat.cpp

  ADDITIONAL_HEADER_DIRS
  ${MLIR_MAIN_INCLUDE_DIR}/mlir/Dialect/Tosa/Transforms

  DEPENDS
  MLIRTosaPassIncGen

  LINK_LIBS PUBLIC
  MLIRFuncDialect
  MLIRPass
  MLIRTosaDialect
  MLIRTransformUtils
  )
