#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseIf(bool isensure) noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  bool err = false;

  std::vector<IfCase> cases;
  IfCase startifcase = parseIfStart(isensure);
  if (std::get<0>(startifcase) && std::get<1>(startifcase))
    cases.push_back(std::move(startifcase));

  std::unique_ptr<BodyExpr> elseBody;
  while (expect(TokenType::TOK_KW_ELSE)) {
    if (*lexer.getCurrentToken() == TokenType::TOK_EOL) {
      lexer.next(); // eat eol
      elseBody = parseFunctionBody();
      break;
    }

    IfCase ifcase = parseIfStart(isensure);
    if (std::get<0>(ifcase) && std::get<1>(ifcase))
      cases.push_back(std::move(ifcase));
  }

  if (!expect(TokenType::TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition()));
  }

  if (err)
    return nullptr;

  return std::make_unique<IfExpr>(lexer, pos,
    std::move(cases), std::move(elseBody), isensure);
}

IfCase Parser::parseIfStart(bool isensure) noexcept {
  // parse 'ensure' or 'if'
  if (isensure && !expect(TokenType::TOK_KW_ENSURE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_EOL_ENSURE,
          lexer.getCurrentToken()->getPosition()));
  } else if (!isensure && !expect(TokenType::TOK_KW_IF)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_EOL_IF,
          lexer.getCurrentToken()->getPosition()));
  }
  // parse condition
  std::unique_ptr<Expr> cond(parseExpression());
  if (!expect(TokenType::TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
    skipToEol();
  }
  // parse body
  std::unique_ptr<BodyExpr> bodyExpr(parseFunctionBody());
  // result
  return IfCase(std::move(cond), std::move(bodyExpr));
}
