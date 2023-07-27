#ifndef MLIR_IR_PATTERNMATCHACTION_H
#define MLIR_IR_PATTERNMATCHACTION_H

#include "mlir/IR/Action.h"

namespace mlir {
struct ReplaceOpAction : public tracing::ActionImpl<ReplaceOpAction> {
  using Base = tracing::ActionImpl<ReplaceOpAction>;
  ReplaceOpAction(ArrayRef<IRUnit> irUnits, ValueRange replacement);
  static constexpr StringLiteral tag = "op-replacement";
  void print(raw_ostream &os) const override;

  Operation *getOp() const;

public:
  ValueRange replacement;
};
} // namespace mlir

#endif
