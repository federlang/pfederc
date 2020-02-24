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
  Position pos{err.getPosition()};
  const Level &lvl{err.getLogLevel()};
  switch (err.getErrorCode()) {
  case SyntaxErrorCode::STX_ERR_EXPECTED_PRIMARY_EXPR:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected primary expression"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_CLOSING_BRACKET:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected ')'"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_ARR_CLOSING_BRACKET:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected ']'"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_TEMPL_CLOSING_BRACKET:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected '}'"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_FUNCTION_ID:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected identifier"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_PARAMETERS:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected function parameters"));
  case SyntaxErrorCode::STX_ERR_INVALID_VARDECL_ID:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected variable identifier"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_VARDECL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected variable declaration"));
  case SyntaxErrorCode::STX_ERR_INVALID_VARDECL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Invalid variable declaration"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_ID:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected identifier"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_GUARD:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected guard"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_FN_DCL_DEF:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected either function declaration or definition"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_STMT:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected ';'"));
  case SyntaxErrorCode::STX_ERR_FUNC_VAR_NO_TEMPL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Function variable mustn't have be a template."));
  case SyntaxErrorCode::STX_ERR_EXPECTED_EOL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected end-of-line"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_EOF:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected end-of-file"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_EOF_EOL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Expected end-of-file or end-of-line"));
  case SyntaxErrorCode::STX_ERR_INVALID_EXPR:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
      "Invalid expression"));
  case SyntaxErrorCode::STX_ERR_PROGNAME:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected program name"));
  case SyntaxErrorCode::STX_ERR_CLASS_SCOPE:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Invalid in class scope"));
  case SyntaxErrorCode::STX_ERR_TRAIT_SCOPE:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Invalid in trait scope"));
  case SyntaxErrorCode::STX_ERR_TRAIT_SCOPE_FUNC_TEMPL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Invalid template function in trait scope"));
  case SyntaxErrorCode::STX_ERR_TRAIT_SCOPE_FUNC_BODY:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "No function body in trait scope"));
  case SyntaxErrorCode::STX_ERR_TRAITCLASS_SCOPE_FUNC_BODY:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected function body in class trait scope"));
  case SyntaxErrorCode::STX_ERR_TRAITCLASS_IMPL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected single reference to trait type"));
  case SyntaxErrorCode::STX_ERR_ENUM_SCOPE:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Invalid in enum scope"));
  case SyntaxErrorCode::STX_ERR_ENUM_BODY:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected enum body"));
  case SyntaxErrorCode::STX_ERR_CLASS_TRAIT_SCOPE:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Invalid in class trait scope"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_CONSTRUCTION:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected constructor"));
  case SyntaxErrorCode::STX_ERR_INVALID_TYPE_EXPR:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Invalid type expression"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_FOR:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected 'for'"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_STMT_ELSE:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected ';' or 'else'"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_EOL_IF:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected either end-of-line or 'if'"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_EOL_ENSURE:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected either end-of-line or 'ensure'"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_ID_NUM_CHAR_BOOL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected either identifier, number, character or boolean token"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_ID_ANY:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected either identifier or '_'"));
  case SyntaxErrorCode::STX_ERR_EXPECTED_OP_IMPL:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Expected '=>'"));
  case SyntaxErrorCode::STX_ERR_INVALID_CAPS_ENSURE:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Invalid ensurance capability"));
  case SyntaxErrorCode::STX_ERR_INVALID_CAPS_DIRECTIVE:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Invalid directive capability"));
  case SyntaxErrorCode::STX_ERR_INVALID_CAPS_FOLLOWUP:
    return LogMessage(lvl, logCreateErrorMessage(lexer, pos,
          "Invalid expression following capability list"));
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
