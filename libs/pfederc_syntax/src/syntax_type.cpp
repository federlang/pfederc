#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseType(std::unique_ptr<Capabilities> &&caps) noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  sanityExpect(TOK_KW_TYPE);

  std::unique_ptr<Expr> expr(parseExpression());
  if (!expr) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_INVALID_TYPE_EXPR, lexer.getCurrentToken()->getPosition()));
    return nullptr;
  }

  if (!isBiOpExpr(*expr, TOK_OP_NONE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_INVALID_TYPE_EXPR, expr->getPosition()));
    return nullptr;
  }

  BiOpExpr &biopexpr = dynamic_cast<BiOpExpr&>(*expr);

  if (!isTokenExpr(biopexpr.getRight(), TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_ID, biopexpr.getRight().getPosition()));
    return nullptr;
  }

  pos = pos + biopexpr.getPosition();

  return std::make_unique<TypeExpr>(lexer, pos, std::move(caps),
      dynamic_cast<const TokenExpr&>(biopexpr.getRight()).getTokenPtr(),
      biopexpr.getLeftPtr());
}
