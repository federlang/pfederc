#include "pfederc/syntax.hpp"
using namespace pfederc;

void Parser::sanityExpect(TokenType type) noexcept {
  if (*lexer.getCurrentToken() != type) {
    fatal("syntax_error.cpp", __LINE__, "Unexpected token");
    return;
  }

  lexer.next();
}

bool Parser::expect(TokenType type) noexcept {
  if (*lexer.getCurrentToken() != type)
    return false;

  lexer.next();
  return true;
}

std::unique_ptr<ErrorExpr> Parser::generateError(
    std::unique_ptr<SyntaxError> &&syntaxError) noexcept {
  
  const Position &pos = syntaxError->getPosition();
  errors.push_back(std::move(syntaxError));

  return std::make_unique<ErrorExpr>(lexer, pos);
}

inline static LogMessage _logParserError(const Parser &parser, const SyntaxError &err) noexcept {
  const Lexer &lexer{parser.getLexer()};
  const Position &pos{err.getPosition()};
  const Level &lvl{err.getLogLevel()};
  switch (err.getErrorCode()) {
  case STX_ERR_EXPECTED_PRIMARY_EXPR:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected primary expression"));
  case STX_ERR_EXPECTED_CLOSING_BRACKET:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected ')'"));
  case STX_ERR_EXPECTED_ARR_CLOSING_BRACKET:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected ']'"));
  case STX_ERR_EXPECTED_TEMPL_CLOSING_BRACKET:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected '}'"));
  case STX_ERR_EXPECTED_FUNCTION_ID:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected identifier"));
  case STX_ERR_EXPECTED_PARAMETERS:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected function parameters"));
  case STX_ERR_INVALID_VARDECL_ID:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected variable identifier"));
  case STX_ERR_EXPECTED_VARDECL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected variable declaration"));
  case STX_ERR_EXPECTED_GUARD:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected guard"));
  case STX_ERR_EXPECTED_FN_DCL_DEF:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected either function declaration or definition"));
  case STX_ERR_EXPECTED_STMT:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected ';'"));
  case STX_ERR_EXPECTED_EOL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected end-of-line"));
  default:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos, "Unknown error"));
  }
}

bool pfederc::logParserErrors(Logger &log, const Parser &parser) noexcept {
  bool result = false;
  for (const auto &stxErr : parser.getErrors()) {
    LogMessage msg = _logParserError(parser, *stxErr);
    if (msg.getLogLevel() == LVL_ERROR)
      result = true;
    msg.log(log);
  }

  return result;
}
