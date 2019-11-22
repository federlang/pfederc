#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseArray() noexcept {
  const Token *const startToken = lexer.getCurrentToken();
  sanityExpect(TOK_OP_ARR_BRACKET_OPEN);

  std::unique_ptr<Expr> expr0(parseExpression());
  if (!expr0)
    return nullptr;

  if (expect(TOK_STMT)) {
    std::unique_ptr<Expr> expr1(parseExpression());
    if (!expr1)
      return nullptr;

    const Token *const endToken = lexer.getCurrentToken();
    if (!expect(TOK_ARR_BRACKET_CLOSE)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_ARR_CLOSING_BRACKET,
        lexer.getCurrentToken()->getPosition(),
        std::vector<Position> { startToken->getPosition() }));
    }

    const Position pos(startToken->getPosition() +
      expr0->getPosition() + expr1->getPosition() + endToken->getPosition());

    return std::make_unique<ArrayCpyExpr>(lexer, pos,
      std::move(expr0), std::move(expr1));
  }
  // now it's either an ArrayEmptyExpr or ArrayLitExpr
  if (isBiOpExpr(*expr0, TOK_OP_COMMA)) {
    Position pos(expr0->getPosition());

    std::list<std::unique_ptr<Expr>> exprs;
    while (isBiOpExpr(*expr0, TOK_OP_COMMA)) {
      BiOpExpr &biopexpr = dynamic_cast<BiOpExpr&>(*expr0);
      exprs.insert(exprs.begin(), biopexpr.getRightPtr());
      expr0 = biopexpr.getLeftPtr();
    }

    exprs.insert(exprs.begin(), std::move(expr0));

    const Token *const endToken = lexer.getCurrentToken();
    if (!expect(TOK_ARR_BRACKET_CLOSE)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_ARR_CLOSING_BRACKET,
        lexer.getCurrentToken()->getPosition(),
        std::vector<Position> { startToken->getPosition() }));
    }

    pos = pos + startToken->getPosition() + endToken->getPosition();

    return std::make_unique<ArrayLitExpr>(lexer, pos, std::move(exprs));
  }

  const Token *const endToken = lexer.getCurrentToken();
  if (!expect(TOK_ARR_BRACKET_CLOSE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_ARR_CLOSING_BRACKET,
      lexer.getCurrentToken()->getPosition(),
      std::vector<Position> { startToken->getPosition() }));
  }

  const Position pos(startToken->getPosition() +
    expr0->getPosition() + endToken->getPosition());

  return std::make_unique<ArrayEmptyExpr>(lexer, pos,
    std::move(expr0));
}
