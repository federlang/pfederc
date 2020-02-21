#include "pfederc/syntax_optimizer.hpp"
using namespace pfederc;

std::unique_ptr<Expr> pfederc::optimizeAll(std::unique_ptr<Expr> &&expr,
    size_t &reducedexpressions) noexcept {
  auto tpl = optimize(std::move(expr), reducedexpressions);
  while (std::get<1>(tpl)) {
    tpl = optimize(std::move(std::move(std::get<0>(tpl))), reducedexpressions);
  }

  return std::move(std::get<0>(tpl));
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimize(
    std::unique_ptr<Expr> &&expr, size_t &reducedexpressions) noexcept {
  Expr *pexpr = expr.release();
  switch (pexpr->getType()) {
  case EXPR_PROG:
    return optimizeProgramExpr(dynamic_cast<ProgramExpr*>(pexpr), reducedexpressions);
  case EXPR_FUNC:
    return optimizeFuncExpr(dynamic_cast<FuncExpr*>(pexpr), reducedexpressions);
  case EXPR_LAMBDA:
    return optimizeLambdaExpr(dynamic_cast<LambdaExpr*>(pexpr), reducedexpressions);
  case EXPR_CLASS:
    return optimizeClassExpr(dynamic_cast<ClassExpr*>(pexpr), reducedexpressions);
  case EXPR_TRAITIMPL:
    return optimizeTraitImplExpr(dynamic_cast<TraitImplExpr*>(pexpr), reducedexpressions);
  case EXPR_MOD:
    return optimizeModExpr(dynamic_cast<ModExpr*>(pexpr), reducedexpressions);
  case EXPR_SAFE:
    return optimizeSafeExpr(dynamic_cast<SafeExpr*>(pexpr), reducedexpressions);
  case EXPR_IF:
    return optimizeIfExpr(dynamic_cast<IfExpr*>(pexpr), reducedexpressions);
  case EXPR_LOOP_FOR:
  case EXPR_LOOP_DO:
    return optimizeLoopExpr(dynamic_cast<LoopExpr*>(pexpr), reducedexpressions);
  case EXPR_MATCH:
    return optimizeMatchExpr(dynamic_cast<MatchExpr*>(pexpr), reducedexpressions);
  case EXPR_BIOP:
    return optimizeBiOpExpr(dynamic_cast<BiOpExpr*>(pexpr), reducedexpressions);
  case EXPR_UNOP:
    return optimizeUnOpExpr(dynamic_cast<UnOpExpr*>(pexpr), reducedexpressions);
  case EXPR_BODY:
    return optimizeBodyExpr(dynamic_cast<BodyExpr*>(pexpr), reducedexpressions);
  case EXPR_ARRCPY:
    return optimizeArrayCpyExpr(dynamic_cast<ArrayCpyExpr*>(pexpr), reducedexpressions);
  case EXPR_ARRLIT:
    return optimizeArrayLitExpr(dynamic_cast<ArrayLitExpr*>(pexpr), reducedexpressions);
  case EXPR_ARREMPTY:
    return optimizeArrayEmptyExpr(dynamic_cast<ArrayEmptyExpr*>(pexpr), reducedexpressions);
  default:
    return std::tuple<std::unique_ptr<Expr>, bool>(
        std::unique_ptr<Expr>(pexpr), false);
  }
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeProgramExpr(
    ProgramExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeFuncExpr(
    FuncExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeLambdaExpr(
    LambdaExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeClassExpr(
    ClassExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeTraitImplExpr(
    TraitImplExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeModExpr(
    ModExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeSafeExpr(
    SafeExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeIfExpr(
    IfExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeLoopExpr(
    LoopExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeMatchExpr(
    MatchExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeBiOpExpr(
    BiOpExpr *expr, size_t &reducedexpressions) noexcept {

  const size_t oldreducedexpressions{reducedexpressions};
  std::unique_ptr<Expr> lhs = optimizeAll(expr->getLeftPtr(), reducedexpressions);
  std::unique_ptr<Expr> rhs = optimizeAll(expr->getRightPtr(), reducedexpressions);

  // Expression: str str
  // Target: str
  if (expr->getOperatorType() == TOK_OP_NONE
      && isTokenExpr(*lhs, TOK_STR) && isTokenExpr(*rhs, TOK_STR)) {
    const std::string strlhs = lhs->toString(), strrhs = rhs->toString();
    const std::string merged =
      strlhs.substr(0, strlhs.length() - 1) + strrhs.substr(1);
    std::unique_ptr<Expr> newexpr = std::make_unique<FakeTokenExpr>(
        expr->getLexer(), std::make_unique<StringToken>(
          dynamic_cast<TokenExpr&>(*lhs).getToken().getLast(),
          TOK_STR,
          expr->getPosition(), merged));
    delete expr;

    reducedexpressions++;
    return std::tuple<std::unique_ptr<Expr>, bool>(
        std::move(newexpr), oldreducedexpressions != reducedexpressions);
  }

  std::unique_ptr<Expr> newexpr = std::make_unique<BiOpExpr>(expr->getLexer(),
    expr->getPosition(), &(expr->getOperatorToken()), expr->getOperatorType(),
    std::move(lhs), std::move(rhs));
  delete expr;

  return std::tuple<std::unique_ptr<Expr>, bool>(
    std::move(newexpr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeUnOpExpr(
    UnOpExpr *expr, size_t &reducedexpressions) noexcept {
  std::unique_ptr<Expr> newincludedexpr = optimizeAll(expr->getExpressionPtr(), reducedexpressions);
  std::unique_ptr<Expr> newexpr = std::make_unique<UnOpExpr>(expr->getLexer(),
      expr->getPosition(), &(expr->getOperatorToken()),
        std::move(newincludedexpr));
  delete expr;

  return std::tuple<std::unique_ptr<Expr>, bool>(std::move(newexpr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeBodyExpr(
    BodyExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeArrayCpyExpr(
    ArrayCpyExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeArrayLitExpr(
    ArrayLitExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeArrayEmptyExpr(
    ArrayEmptyExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}
