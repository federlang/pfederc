#ifndef PFEDERC_SYNTAX_SYNTAX_OPTIMIZER
#define PFEDERC_SYNTAX_SYNTAX_OPTIMIZER

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/lexer.hpp"
#include "pfederc/expr.hpp"

namespace pfederc {
  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimize(
      std::unique_ptr<Expr> &&expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeProgramExpr(
      ProgramExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeFuncExpr(
      FuncExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeLambdaExpr(
      LambdaExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeClassExpr(
      ClassExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeTraitImplExpr(
      TraitImplExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeModExpr(
      ModExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeSafeExpr(
      SafeExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeIfExpr(
      IfExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeLoopExpr(
      LoopExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeMatchExpr(
      MatchExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeBiOpExpr(
      BiOpExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeUnOpExpr(
      UnOpExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeBodyExpr(
      BodyExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeArrayCpyExpr(
      ArrayCpyExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeArrayLitExpr(
      ArrayLitExpr *expr, size_t &reducedexpressions) noexcept;

  std::tuple<std::unique_ptr<Expr>, bool /* changed */> optimizeArrayEmptyExpr(
      ArrayEmptyExpr *expr, size_t &reducedexpressions) noexcept;

  /*\brief Call optmize as long as _2 is true
   */
  std::unique_ptr<Expr> optimizeAll(std::unique_ptr<Expr> &&expr,
      size_t &reducedexpressions) noexcept;
}

#endif /* PFEDERC_SYNTAX_SYNTAX_OPTIMIZER */

