// Modifications (c) Copyright 2026 Advanced Micro Devices, Inc. or its
// affiliates

// RUN: mlir-tblgen -gen-pass-decls -name Test -I %S/../../include %s | FileCheck %s

include "mlir/Pass/PassBase.td"

def TestPassWithOptions : Pass<"test-pass-with-options"> {
  let summary = "Test pass with options";
  let genOptionsParser = 1;

  let options = [
    Option<"testOption", "test-option", "int", "0", "Test option">,
    ListOption<"testListOption", "test-list-option", "int64_t",
               "Test list option">
  ];
}

def TestPassWithoutOptionsParser : Pass<"test-pass-without-options-parser"> {
  let summary = "Test pass without options parser";

  let options = [
    Option<"testOption", "test-option", "int", "0", "Test option">,
  ];
}

// CHECK-LABEL: struct TestPassWithOptionsOptions {
// CHECK: int testOption = 0;
// CHECK: ::llvm::SmallVector<int64_t> testListOption;

// CHECK: ::mlir::FailureOr<TestPassWithOptionsOptions> parseTestPassWithOptionsOptions(
// CHECK-NEXT: ::llvm::StringRef options,
// CHECK-NEXT: ::llvm::function_ref<::mlir::LogicalResult(const ::llvm::Twine &)> errorHandler);

// CHECK-LABEL: class TestPassWithOptionsBase
// CHECK: protected:
// CHECK: ::mlir::Pass::Option<int> testOption
// CHECK-SAME: "test-option"
// CHECK-SAME: ::llvm::cl::init(0)
// CHECK: ::mlir::Pass::ListOption<int64_t> testListOption
// CHECK-SAME: "test-list-option"

// CHECK: ::mlir::FailureOr<TestPassWithOptionsOptions> parseTestPassWithOptionsOptions(
// CHECK-NEXT: ::llvm::StringRef options,
// CHECK-NEXT: ::llvm::function_ref<::mlir::LogicalResult(const ::llvm::Twine &)> errorHandler) {
// CHECK: struct OptionsParser : public ::mlir::detail::PassOptions {
// CHECK: ::mlir::detail::PassOptions::Option<int> testOption
// CHECK-SAME: "test-option"
// CHECK-SAME: ::llvm::cl::init(0)
// CHECK: ::mlir::detail::PassOptions::ListOption<int64_t> testListOption
// CHECK-SAME: "test-list-option"
// CHECK: if (::mlir::failed(parser.parseFromString(options, errorStream))) {
// CHECK: return errorHandler(error);
// CHECK: TestPassWithOptionsOptions parsedOptions;
// CHECK: parsedOptions.testOption = parser.testOption.getValue();
// CHECK: parsedOptions.testListOption.assign((*parser.testListOption).begin(), (*parser.testListOption).end());
// CHECK: return parsedOptions;

// CHECK-LABEL: struct TestPassWithoutOptionsParserOptions {
// CHECK: int testOption = 0;
// CHECK-NOT: parseTestPassWithoutOptionsParserOptions
// CHECK-LABEL: class TestPassWithoutOptionsParserBase
// CHECK-NOT: parseTestPassWithoutOptionsParserOptions
// CHECK: std::unique_ptr<::mlir::Pass> createTestPassWithoutOptionsParser()
