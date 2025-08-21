add_mlir_conversion_library(MLIRUBToEmitC
  UBToEmitC.cpp

  ADDITIONAL_HEADER_DIRS
  ${MLIR_MAIN_INCLUDE_DIR}/mlir/Conversion/UBToEmitC

  DEPENDS
  MLIRConversionPassIncGen

  LINK_COMPONENTS
  Core

  LINK_LIBS PUBLIC
  MLIRLLVMCommonConversion
  MLIREmitCDialect
  MLIRUBDialect
  )
