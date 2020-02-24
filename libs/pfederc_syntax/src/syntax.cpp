#include "pfederc/syntax.hpp"
using namespace pfederc;

namespace pfederc {
  const std::map<TokenType /* opening bracket */,
    SyntaxErrorCode> STX_ERR_BRACKETS = {
      { TokenType::TOK_OP_BRACKET_OPEN, SyntaxErrorCode::STX_ERR_EXPECTED_CLOSING_BRACKET },
      { TokenType::TOK_OP_ARR_BRACKET_OPEN, SyntaxErrorCode::STX_ERR_EXPECTED_ARR_CLOSING_BRACKET },
      { TokenType::TOK_OP_TEMPL_BRACKET_OPEN, SyntaxErrorCode::STX_ERR_EXPECTED_TEMPL_CLOSING_BRACKET },
  };
}

// Parser
Parser::~Parser() {
}

bool Parser::skipToStmtEol() noexcept {
  while (*lexer.getCurrentToken() != TokenType::TOK_EOL
      && *lexer.getCurrentToken() != TokenType::TOK_EOF
      && *lexer.getCurrentToken() != TokenType::TOK_STMT)
    lexer.next();

  return *lexer.getCurrentToken() != TokenType::TOK_EOF;
}

bool Parser::skipToEol() noexcept {
  while (*lexer.getCurrentToken() != TokenType::TOK_EOL
      && *lexer.getCurrentToken() != TokenType::TOK_EOF)
    lexer.next();

  return *lexer.getCurrentToken() != TokenType::TOK_EOF;
}

void Parser::skipEol() noexcept {
  while (*lexer.getCurrentToken() == TokenType::TOK_EOL)
    lexer.next();
}

std::unique_ptr<Expr> Parser::parseUnary() noexcept {
  const Token *tok = lexer.getCurrentToken();
  if (*tok == TokenType::TOK_OP_BRACKET_OPEN)
    return parseBrackets();
  else if (*tok == TokenType::TOK_OP_ARR_BRACKET_OPEN)
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
  while (*lexer.getCurrentToken() == TokenType::TOK_EOL)
    lexer.next();

  const Precedence prec = std::get<0>(OPERATORS_INFO.at(type));
  std::unique_ptr<Expr> expr = parseExpression(prec);
  if (!expr)
    return nullptr;

  return std::make_unique<UnOpExpr>(lexer,
    tok->getPosition() + expr->getPosition(), tok, std::move(expr));
}

std::unique_ptr<Expr> Parser::parsePrimary(std::unique_ptr<Capabilities> &&caps) noexcept {
  Token *const tok = lexer.getCurrentToken();
  if (isTokenTypeOperator(tok->getType()))
    return parseUnary();

  switch (tok->getType()) {
  case TokenType::TOK_ID:
  case TokenType::TOK_INT8:
  case TokenType::TOK_INT16:
  case TokenType::TOK_INT32:
  case TokenType::TOK_INT64:
  case TokenType::TOK_UINT8:
  case TokenType::TOK_UINT16:
  case TokenType::TOK_UINT32:
  case TokenType::TOK_UINT64:
  case TokenType::TOK_FLT32:
  case TokenType::TOK_FLT64:
  case TokenType::TOK_CHAR:
  case TokenType::TOK_STR:
  case TokenType::TOK_KW_TRUE:
  case TokenType::TOK_KW_FALSE:
    lexer.next();
    return std::make_unique<TokenExpr>(lexer, tok);
  case TokenType::TOK_KW_FN:
    return parseFunction(std::move(caps));
  case TokenType::TOK_KW_LAMBDA:
    return parseLambda();
  case TokenType::TOK_KW_MOD:
    return parseModule();
  case TokenType::TOK_KW_CLASS:
    return parseClass(std::move(caps));
  case TokenType::TOK_KW_ENUM:
    return parseEnum();
  case TokenType::TOK_KW_TRAIT:
    return parseTrait(std::move(caps));
  case TokenType::TOK_KW_TYPE:
    return parseType(std::move(caps));
  case TokenType::TOK_KW_USE:
    return parseUse();
  case TokenType::TOK_KW_IF:
    return parseIf();
  case TokenType::TOK_KW_ENSURE:
    return parseIf(true);
  case TokenType::TOK_KW_MATCH:
    return parseMatch();
  case TokenType::TOK_KW_FOR:
    return parseFor();
  case TokenType::TOK_KW_DO:
    return parseFor(true);
  case TokenType::TOK_KW_SAFE:
    return parseSafe();

// capabilties
  case TokenType::TOK_ENSURE:
    return parseCapabilities();
  case TokenType::TOK_DIRECTIVE:
    return parseCapabilities();
  default:
    return nullptr;
  }
}

std::unique_ptr<Expr> Parser::parseBrackets() noexcept {
  sanityExpect(TokenType::TOK_OP_BRACKET_OPEN);

  std::unique_ptr<Expr> result(parseExpression());
  if (!expect(TokenType::TOK_BRACKET_CLOSE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_CLOSING_BRACKET,
      lexer.getCurrentToken()->getPosition()));
  }

  return result;
}

std::unique_ptr<Expr> Parser::parseUse() noexcept {
  sanityExpect(TokenType::TOK_KW_USE);
  if (expect(TokenType::TOK_KW_MOD)) {
    if (!expect(TokenType::TOK_ID)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        SyntaxErrorCode::STX_ERR_EXPECTED_ID, lexer.getCurrentToken()->getPosition()));
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
  while (isBiOpExpr(*expr, TokenType::TOK_OP_COMMA)) {
    BiOpExpr& biopexpr = dynamic_cast<BiOpExpr&>(*expr);
    exprs.insert(exprs.begin(), biopexpr.getRightPtr());
    expr = biopexpr.getLeftPtr();
  }

  exprs.insert(exprs.begin(), std::move(expr));

  return std::make_unique<UseExpr>(lexer, pos, std::move(exprs));
}

std::unique_ptr<Expr> Parser::parseSafe() noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  sanityExpect(TokenType::TOK_KW_SAFE);

  std::unique_ptr<Expr> expr(parseExpression());

  if (!expr)
    return nullptr;

  pos = pos + expr->getPosition();

  switch (expr->getType()) {
	case ExprType::EXPR_ARRLIT:
  case ExprType::EXPR_ARRCPY:
  case ExprType::EXPR_ARREMPTY:
    return std::make_unique<SafeExpr>(lexer, pos, std::move(expr));
	case ExprType::EXPR_UNOP:
    if (dynamic_cast<const UnOpExpr&>(*expr).getOperatorType() == TokenType::TOK_OP_BRACKET_OPEN)
      return std::make_unique<SafeExpr>(lexer, pos, std::move(expr));
    break;
	case ExprType::EXPR_BIOP:
    if (dynamic_cast<const BiOpExpr&>(*expr).getOperatorType() == TokenType::TOK_OP_BRACKET_OPEN)
      return std::make_unique<SafeExpr>(lexer, pos, std::move(expr));
    break;
  default:
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_CONSTRUCTION,
      expr->getPosition()));
    return nullptr;
  }

  return nullptr;
}

std::unique_ptr<Expr> Parser::parseExpression(Precedence prec) noexcept {
  auto primary = parsePrimary();
  if (!primary) {
    lexer.next();
    /*generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_EXPR, lexer.getCurrentToken()->getPosition()));*/
    return nullptr; // error forwarding
  }
  
  return parseBinary(std::move(primary), prec);
}

// SyntaxError
SyntaxError::~SyntaxError() {
}
