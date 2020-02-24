#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseFor(bool isdo) noexcept {
  if (isdo)
    sanityExpect(TokenType::TOK_KW_DO);
  else
    sanityExpect(TokenType::TOK_KW_FOR);

  std::unique_ptr<Expr> initExpr, condExpr, itExpr;

  if (isdo) {
    if (*lexer.getCurrentToken() != TokenType::TOK_EOL
        && *lexer.getCurrentToken() != TokenType::TOK_EOF) {
      initExpr = parseExpression();
    }
  } else {
    condExpr = parseExpression();
    if (expect(TokenType::TOK_STMT)) {
      skipEol();
      itExpr = parseExpression();
      if (expect(TokenType::TOK_STMT)) {
        skipEol();
        // switch expressions
        initExpr = std::move(condExpr);
        condExpr = std::move(itExpr);
        itExpr = parseExpression();
      }
    }
  }

  if (!expect(TokenType::TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_EOL,
          lexer.getCurrentToken()->getPosition()));
    skipToEol();
  }
  // parse body
  std::unique_ptr<BodyExpr> bodyExpr(parseFunctionBody());
  if (!expect(TokenType::TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_EOL,
          lexer.getCurrentToken()->getPosition()));
  }

  if (isdo) {
    if (!expect(TokenType::TOK_KW_FOR)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
            SyntaxErrorCode::STX_ERR_EXPECTED_FOR,
            lexer.getCurrentToken()->getPosition()));
      skipToEol();
    } else {
      condExpr = parseExpression();
      if (expect(TokenType::TOK_STMT)) {
        skipEol();
        itExpr = parseExpression();
      }
    }
  }

  if (!condExpr || !bodyExpr)
    return nullptr;

  Position pos = condExpr->getPosition();

  return std::make_unique<LoopExpr>(lexer,
      isdo ? ExprType::EXPR_LOOP_DO : ExprType::EXPR_LOOP_FOR, pos,
      std::move(initExpr), std::move(condExpr), std::move(itExpr),
      std::move(bodyExpr));
}
