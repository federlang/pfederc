#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseLambda() noexcept {
  sanityExpect(TOK_KW_LAMBDA);

  bool err = false;

  Exprs params;
  if (*lexer.getCurrentToken() == TOK_OP_BRACKET_OPEN) {
    lexer.next();

    std::unique_ptr<Expr> paramexpr(parseExpression());
    if (!paramexpr) {
      err = true;
    } else {
      while (isBiOpExpr(*paramexpr, TOK_OP_COMMA)) {
        BiOpExpr& biopexpr = dynamic_cast<BiOpExpr&>(*paramexpr);
        params.insert(params.begin(), biopexpr.getRightPtr());
        paramexpr = biopexpr.getLeftPtr();
      }

      params.insert(params.begin(), std::move(paramexpr));
    }

    if (!expect(TOK_BRACKET_CLOSE)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_CLOSING_BRACKET, lexer.getCurrentToken()->getPosition()));
      err = true;
    }
  }

  if (*lexer.getCurrentToken() == TOK_OP_ASG) {
    // *=* *expr*
    lexer.next(); // eat =

    std::unique_ptr<Expr> expr(parseExpression());
    if (!expr) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_EXPR, lexer.getCurrentToken()->getPosition()));
      return nullptr;
    }

    if (err)
      return nullptr;

    Position pos(expr->getPosition());

    return std::make_unique<LambdaExpr>(lexer, pos,
      std::move(params), std::make_unique<BodyExpr>(lexer,
        expr->getPosition(),
        Exprs(), std::move(expr)));
  }
  // *newline* *body* *;*
  if (!expect(TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
    err = true;
  }

  std::unique_ptr<BodyExpr> body(parseFunctionBody());
  if (!body)
    err = true;

  if (!expect(TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition()));
    err = true;
  }

  if (err)
    return nullptr;

  return std::make_unique<LambdaExpr>(lexer, body->getPosition(),
    std::move(params), std::move(body));
}
