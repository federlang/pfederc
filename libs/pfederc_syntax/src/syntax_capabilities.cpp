#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseCapabilities() noexcept {
  bool isunused = false, isinline = false, isconstant = false;
  std::vector<std::unique_ptr<Expr>> requires;
  std::vector<std::unique_ptr<Expr>> ensures;

  while ((*lexer.getCurrentToken() == TokenType::TOK_ENSURE
      || *lexer.getCurrentToken() == TokenType::TOK_DIRECTIVE)
      && *lexer.getCurrentToken() != TokenType::TOK_EOF) {

    if (*lexer.getCurrentToken() == TokenType::TOK_ENSURE)
      parseCapabilityEnsure(requires, ensures);
    else
      parseCapabilityDirective(isunused, isinline, isconstant);

    if (!expect(TokenType::TOK_EOL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
      skipToEol();
    }

    // skip new lines
    while (*lexer.getCurrentToken() == TokenType::TOK_EOL)
      lexer.next();
  }

  switch (lexer.getCurrentToken()->getType()) {
  case TokenType::TOK_KW_CLASS:
  case TokenType::TOK_KW_FN:
  case TokenType::TOK_KW_TRAIT:
  case TokenType::TOK_KW_TYPE:
    return parsePrimary(std::make_unique<Capabilities>(
          isunused, isinline, isconstant,
          std::move(requires), std::move(ensures)));
  default:
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_INVALID_CAPS_FOLLOWUP,
          lexer.getCurrentToken()->getPosition()));
    return nullptr;
  }
}

static constexpr uint8_t _CAPS_ENSURE_INVALID = 0;
static constexpr uint8_t _CAPS_ENSURE_ENSURES = 1;
static constexpr uint8_t _CAPS_ENSURE_REQUIRES = 2;

static uint8_t _getCapsEnsure(const Token *const tokCmd, const Lexer &lexer) {
  if (*tokCmd != TokenType::TOK_ID)
    return _CAPS_ENSURE_INVALID;

  std::string str = tokCmd->toString(lexer);
  if (str == "ensures")
    return _CAPS_ENSURE_ENSURES;
  if (str == "requires")
    return _CAPS_ENSURE_REQUIRES;
  // else
  return _CAPS_ENSURE_INVALID;
}

void Parser::parseCapabilityEnsure(std::vector<std::unique_ptr<Expr>> &requires,
    std::vector<std::unique_ptr<Expr>> &ensures) noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  sanityExpect(TokenType::TOK_ENSURE);

  bool err = false;

  const Token *const tokCmd = lexer.getCurrentToken();
  uint8_t capstype = _CAPS_ENSURE_INVALID;
  if ((capstype = _getCapsEnsure(tokCmd, getLexer())) == _CAPS_ENSURE_INVALID) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_INVALID_CAPS_ENSURE, lexer.getCurrentToken()->getPosition()));
    switch (lexer.getCurrentToken()->getType()) {
    case TokenType::TOK_EOF:
    case TokenType::TOK_EOL:
    case TokenType::TOK_STMT:
      break;
    default:
      lexer.next();
      break;
    }

    err = true;
  } else {
    lexer.next();
  }

  std::unique_ptr<Expr> expr(parseExpression());
  if (!expr)
    err = true;

  if (err)
    return;

  if (capstype == _CAPS_ENSURE_ENSURES)
    ensures.push_back(std::move(expr));
  else
    requires.push_back(std::move(expr));
}

void Parser::parseCapabilityDirective(bool &isunused, bool &isinline, bool &isconstant) noexcept {
  sanityExpect(TokenType::TOK_DIRECTIVE);

  const Token *const tokId = lexer.getCurrentToken();
  if (!expect(TokenType::TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_INVALID_CAPS_DIRECTIVE,
          lexer.getCurrentToken()->getPosition()));
    return;
  }

  std::string str = tokId->toString(getLexer());
  if (str == "Unused" && !isunused) {
    isunused = true;
  } else if (str == "Inline" && !isinline) {
    isinline = true;
  } else if (str == "Constant" && !isconstant) {
    isconstant = true;
  } else {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_INVALID_CAPS_DIRECTIVE,
          lexer.getCurrentToken()->getPosition()));
  }
}
