#include "pfederc/syntax.hpp"
using namespace pfederc;

namespace pfederc {
  const std::map<TokenType /* opening bracket */,
    SyntaxErrorCode> STX_ERR_BRACKETS = {
      { TOK_OP_BRACKET_OPEN, STX_ERR_EXPECTED_CLOSING_BRACKET },
      { TOK_OP_ARR_BRACKET_OPEN, STX_ERR_EXPECTED_ARR_CLOSING_BRACKET },
      { TOK_OP_TEMPL_BRACKET_OPEN, STX_ERR_EXPECTED_TEMPL_CLOSING_BRACKET },
  };
}

// Parser
Parser::~Parser() {
}

std::unique_ptr<Expr> Parser::parseUnary() noexcept {
  const Token *tok = lexer.getCurrentToken();
  if (*tok == TOK_OP_BRACKET_OPEN)
    return parseBrackets();
  else if (*tok == TOK_OP_ARR_BRACKET_OPEN)
    return parseArray();

  
  TokenType type = tok->getType();
  if (std::get<1>(OPERATORS_INFO.at(tok->getType())) == OperatorType::BINARY) {
    auto bioptounop = TOKEN_BIOP_TO_UNOP.find(tok->getType());
    if (bioptounop == TOKEN_BIOP_TO_UNOP.end())
      return nullptr;

    type = std::get<1>(*bioptounop);
  }

  lexer.next(); // eat unary operator
  // ignore newline tekons
  while (*lexer.getCurrentToken() == TOK_EOL)
    lexer.next();

  const Precedence prec = std::get<0>(OPERATORS_INFO.at(type));
  std::unique_ptr<Expr> expr = parseExpression(prec);
  if (!expr)
    return nullptr;

  return std::make_unique<UnOpExpr>(lexer,
    tok->getPosition() + expr->getPosition(), tok, std::move(expr));
}

std::unique_ptr<Expr> Parser::parsePrimary() noexcept {
  const Token *tok = lexer.getCurrentToken();
  if (isTokenTypeOperator(tok->getType()))
    return parseUnary();

  switch (tok->getType()) {
  case TOK_ID:
  case TOK_INT8:
  case TOK_INT16:
  case TOK_INT32:
  case TOK_INT64:
  case TOK_UINT8:
  case TOK_UINT16:
  case TOK_UINT32:
  case TOK_UINT64:
  case TOK_FLT32:
  case TOK_FLT64:
  case TOK_CHAR:
  case TOK_STR:
  case TOK_KW_TRUE:
  case TOK_KW_FALSE:
    lexer.next();
    return std::make_unique<TokenExpr>(lexer, tok);
  case TOK_KW_FN:
    return parseFunction();
  case TOK_KW_LAMBDA:
    return parseLambda();
  case TOK_KW_MOD:
    return parseModule();
  case TOK_KW_CLASS:
    return parseClass();
  case TOK_KW_ENUM:
    return parseEnum();
  case TOK_KW_TRAIT:
    return parseTrait();
  case TOK_KW_TYPE:
    return parseType();
  case TOK_KW_USE:
    return parseUse();
  case TOK_KW_IF:
    return parseIf();
  case TOK_KW_ENSURE:
    return parseIf(true);
  case TOK_KW_MATCH:
    return parseMatch();
  case TOK_KW_FOR:
    return parseFor();
  case TOK_KW_DO:
    return parseFor(true);
  case TOK_KW_CTN:
    return parseContinue();
  case TOK_KW_BRK:
    return parseBreak();
  case TOK_KW_SAFE:
    return parseSafe();
  default:
    return nullptr;
  }
}

std::unique_ptr<Expr> Parser::parseBrackets() noexcept {
  sanityExpect(TOK_OP_BRACKET_OPEN);

  std::unique_ptr<Expr> result(parseExpression());
  if (!expect(TOK_BRACKET_CLOSE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_CLOSING_BRACKET,
      lexer.getCurrentToken()->getPosition()));
  }

  return result;
}

std::unique_ptr<Expr> Parser::parseUse() noexcept {
  sanityExpect(TOK_KW_USE);
  if (expect(TOK_KW_MOD)) {
    if (!expect(TOK_ID)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_ID, lexer.getCurrentToken()->getPosition()));
      return nullptr;
    }

    const Token *tok = lexer.getCurrentToken();
    lexer.next(); // eat id

    return std::make_unique<ProgNameExpr>(lexer, tok);
  }

  std::unique_ptr<Expr> expr(parseExpression());
  if (!expr)
    return nullptr;

  Position pos(expr->getPosition());

  Exprs exprs;
  while (isBiOpExpr(*expr, TOK_OP_COMMA)) {
    BiOpExpr& biopexpr = dynamic_cast<BiOpExpr&>(*expr);
    exprs.insert(exprs.begin(), biopexpr.getRightPtr());
    expr = biopexpr.getLeftPtr();
  }

  exprs.insert(exprs.begin(), std::move(expr));

  return std::make_unique<UseExpr>(lexer, pos, std::move(exprs));
}

std::unique_ptr<Expr> Parser::parseFor(bool isdo) noexcept {
  if (isdo)
    sanityExpect(TOK_KW_DO);
  else
    sanityExpect(TOK_KW_FOR);
   // TODO
}

std::unique_ptr<Expr> Parser::parseMatch() noexcept {
  sanityExpect(TOK_KW_MATCH);
   // TODO
}

std::unique_ptr<Expr> Parser::parseIf(bool isensure) noexcept {
  if (isensure)
    sanityExpect(TOK_KW_ENSURE);
  else
    sanityExpect(TOK_KW_IF);
   // TODO
}

std::unique_ptr<Expr> Parser::parseContinue() noexcept {
  sanityExpect(TOK_KW_CTN);
   // TODO
}

std::unique_ptr<Expr> Parser::parseBreak() noexcept {
  sanityExpect(TOK_KW_BRK);
   // TODO
}

std::unique_ptr<Expr> Parser::parseSafe() noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  sanityExpect(TOK_KW_SAFE);

  std::unique_ptr<Expr> expr(parseExpression());
  if (!expr)
    return nullptr;

  pos = pos + expr->getPosition();

  switch (expr->getType()) {
  case EXPR_ARRLIT:
  case EXPR_ARRCPY:
  case EXPR_ARREMPTY:
    return std::make_unique<SafeExpr>(lexer, pos, std::move(expr));
  case EXPR_UNOP:
    if (dynamic_cast<const UnOpExpr&>(*expr).getOperator() == TOK_OP_BRACKET_OPEN)
      return std::make_unique<SafeExpr>(lexer, pos, std::move(expr));
  case EXPR_BIOP:
    if (dynamic_cast<const BiOpExpr&>(*expr).getOperator() == TOK_OP_BRACKET_OPEN)
      return std::make_unique<SafeExpr>(lexer, pos, std::move(expr));
  default:
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_CONSTRUCTION,
      expr->getPosition()));
    return nullptr;
  }
}

std::unique_ptr<Expr> Parser::parseExpression(Precedence prec) noexcept {
  auto primary = parsePrimary();
  if (!primary) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_EXPR, lexer.getCurrentToken()->getPosition()));
    return nullptr; // error forwarding
  }
  
  return parseBinary(std::move(primary), prec);
}

// SyntaxError
SyntaxError::~SyntaxError() {
}
