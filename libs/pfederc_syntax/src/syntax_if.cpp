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
  while (expect(TOK_KW_ELSE)) {
    if (*lexer.getCurrentToken() == TOK_EOL) {
      lexer.next(); // eat eol
      elseBody = parseFunctionBody();
      break;
    }

    IfCase ifcase = parseIfStart(isensure);
    if (std::get<0>(ifcase) && std::get<1>(ifcase))
      cases.push_back(std::move(ifcase));
  }

  if (!expect(TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition()));
  }

  if (err)
    return nullptr;

  return std::make_unique<IfExpr>(lexer, pos,
    std::move(cases), std::move(elseBody), isensure);
}

IfCase Parser::parseIfStart(bool isensure) noexcept {
  bool err = false;
  // parse 'ensure' or 'if'
  if (isensure && !expect(TOK_KW_ENSURE)) {
    err = true;
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_EOL_ENSURE,
          lexer.getCurrentToken()->getPosition()));
  } else if (!isensure && !expect(TOK_KW_IF)) {
    err = true;
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_EOL_IF,
          lexer.getCurrentToken()->getPosition()));
  }
  // parse condition
  std::unique_ptr<Expr> cond(parseExpression());
  if (!expect(TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
    skipToEol();
  }
  // parse body
  std::unique_ptr<BodyExpr> bodyExpr(parseFunctionBody());
  // result
  return IfCase(std::move(cond), std::move(bodyExpr));
}
