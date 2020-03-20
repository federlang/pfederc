#include "pfederc/lexer.hpp"
using namespace pfederc;

// static methods

inline static bool _isNewLineCharacter(char ch) noexcept {
  return std::strchr("\n\r", ch);
}

// global

Token& Lexer::next() noexcept {
  if  (!currentToken) {
    lineIndices.push_back(0);
    nextChar();
    currentEndIndex = 0;
  } else if (*currentToken == TokenType::TOK_EOF)
    return *currentToken;


  std::unique_ptr<Token> result(nextToken());
  Token &tok = *result;
  currentToken = result.get();
  tokens.push_back(std::move(result));
  return tok;
}

void Lexer::skipSpace() noexcept {
  while (currentChar == ' ' || currentChar == '\t' || currentChar == '\v')
    nextChar();
}

std::unique_ptr<Token> Lexer::generateError(std::unique_ptr<LexerError> &&err) noexcept {
  errors.push_back(std::move(err));
  while (currentChar != EOF
      && currentChar != '\n' && currentChar != '\r')
    nextChar();
  return std::make_unique<Token>(currentToken, TokenType::TOK_ERR, getCurrentCursor());
}

int Lexer::nextChar() noexcept {
  currentChar = input.get();
  if (currentChar != std::char_traits<char>::eof()) {
    fileContent += static_cast<char>(currentChar);
  } else
    fileContent += ' ';
  ++currentEndIndex;
  return currentChar;
}

std::unique_ptr<Token> Lexer::nextToken() noexcept {
  // Ignore spaces
  skipSpace();
  // set token starting point
  currentStartIndex = currentEndIndex;

  if (std::isalpha(currentChar) || currentChar == '_')
    return nextTokenId();

  if (_isNewLineCharacter(currentChar))
    return nextTokenLine();

  if (std::strchr(")]}", currentChar))
    return nextTokenBracket();

  if (currentChar == '"')
    return nextTokenString();

  if (currentChar == '\'')
    return nextTokenChar();

  if (std::isdigit(currentChar))
    return nextTokenNum();

  std::unique_ptr<Token> result(nextTokenOperator());
  if (result)
    return result;

  if (currentChar == '#')
    return nextTokenCapability();

  if (currentChar == std::char_traits<char>::eof())
    return nextTokenEOF();

  return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_GENERAL_INVALID_CHARACTER,
      getCurrentCursor()));
}

std::unique_ptr<Token> Lexer::nextTokenCapability() noexcept {
  nextChar(); // eat #
  if (currentChar == '!') {
    nextChar(); // eat !
    return std::make_unique<Token>(currentToken,
        TokenType::TOK_ENSURE, getCurrentCursor());
  }

  return std::make_unique<Token>(currentToken,
      TokenType::TOK_DIRECTIVE, getCurrentCursor());
}

std::unique_ptr<Token> Lexer::nextTokenLine() noexcept {
  char c = currentChar;
  nextChar(); // eat newline char

  // eat again if newline character (but not equal to the previous one)
  if (c != currentChar && _isNewLineCharacter(currentChar))
    nextChar();

  lineIndices.push_back(currentEndIndex);
  return std::make_unique<Token>(currentToken, TokenType::TOK_EOL, getCurrentCursor());
}

std::unique_ptr<Token> Lexer::nextTokenEOF() noexcept {
  return std::make_unique<Token>(currentToken, TokenType::TOK_EOF, 
    Position{getCurrentCursor().line, getFileContent().length() - 1,
      getFileContent().length() - 1});
}

std::unique_ptr<Token> Lexer::nextTokenId() noexcept {
  std::string id;
  id.reserve(16);
  // eat _ (important for checking if followed by alphabetic char)
  while (currentChar == '_') {
    id += currentChar;
    nextChar();
  }
  // must be followed by alpha character or just valid _ (TokenType::TOK_ANY)
  if (isdigit(currentChar)) {
    return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_ID_NO_DIGIT_AFTER_ANYS, Position{getCurrentCursor().line,
        currentEndIndex, currentEndIndex}));
  } else if (!isalpha(currentChar)) {
    // check for any
    if (id == "_")
      return std::make_unique<Token>(currentToken, TokenType::TOK_ANY, getCurrentCursor());
    // __+ is not allowed
    return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_ID_NOT_JUST_ANYS, getCurrentCursor()));
  }

  while (currentChar == '_' || isalnum(currentChar)) {
    id += currentChar;
    nextChar();
  }

  // check if keword
  if (id.size() >= KEYWORDS_MIN_STRING_LENGTH
    && id.size() < KEYWORDS_MIN_STRING_LENGTH + KEYWORDS_LENGTH) {
    auto keywords = KEYWORDS[id.size() - KEYWORDS_MIN_STRING_LENGTH];
    for (const auto &tpl : keywords) {
      if (std::get<1>(tpl) == id)
        return std::make_unique<Token>(currentToken, std::get<0>(tpl), getCurrentCursor());
    }
  }
  // check if operator
  if (id == "null")
    return std::make_unique<Token>(currentToken, TokenType::TOK_OP_NULL, getCurrentCursor());

  return std::make_unique<Token>(currentToken, TokenType::TOK_ID, getCurrentCursor());
}

inline static bool _hasOperatorStr(const std::string &op, TokenType &type) noexcept {
  if (op.size() < OPERATORS_MIN_STRING_LENGTH)
    return false;
  if (op.size() >= OPERATORS_MIN_STRING_LENGTH + OPERATORS_LENGTH)
    return false;

  auto &operators = OPERATORS[op.size() - 1];
  for (const auto &tpl : operators) {
    if (std::get<1>(tpl) == op) {
      type = std::get<0>(tpl);

      return true;
    }
  }

  return false;
}

std::unique_ptr<Token> Lexer::nextTokenOperator() noexcept {
  std::string op;
  TokenType operatorType = TokenType::TOK_ERR;
  while (_hasOperatorStr(op + (char) currentChar, operatorType)) {
    op += (char) currentChar;
    nextChar();
  }
  // no match
  if (op.empty())
    return nullptr;

  // if match check if comment (starting with '/')
  if (operatorType == TokenType::TOK_OP_DIV) {
    if (currentChar == '*') // region comment
      return nextRegionComment();
    else if (currentChar == '/') // one line comment
      return nextLineComment();
  }

  return std::make_unique<Token>(currentToken, operatorType, getCurrentCursor());
}

std::unique_ptr<Token> Lexer::nextRegionCommentDoc() noexcept {
  nextChar();
  lastComment.clear();

  while (currentChar != EOF) {
    if (currentChar == '*') {
      nextChar(); // eat *
      if (currentChar == '/') {
        nextChar(); // eat /
        return nextToken();
      }

      lastComment += '*';
      continue;
    }

    lastComment += (char) currentChar;

    nextChar();
  }

  return generateError(std::make_unique<LexerError>(LVL_ERROR, LexerErrorCode::LEX_ERR_REGION_COMMENT_END,
    getCurrentCursor()));
}

std::unique_ptr<Token> Lexer::nextRegionComment() noexcept {
  nextChar(); // eat *
  if (currentChar == '*' || currentChar == '!')
    return nextRegionCommentDoc();

  while (currentChar != EOF) {
    if (currentChar == '*') {
      nextChar(); // eat *
      if (currentChar == '/') {
        nextChar(); // eat /
        return nextToken();
      }

      continue;
    }

    nextChar();
  }

  return generateError(std::make_unique<LexerError>(LVL_ERROR, LexerErrorCode::LEX_ERR_REGION_COMMENT_END,
    getCurrentCursor()));
}

std::unique_ptr<Token> Lexer::nextLineCommentDoc() noexcept {
  nextChar();
  if (!lastComment.empty())
    lastComment += '\n';

  while (currentChar != EOF && currentChar != '\n' && currentChar != '\r') {
    lastComment += (char) currentChar;
    nextChar();
  }

  return nextToken();
}

std::unique_ptr<Token> Lexer::nextLineComment() noexcept {
  nextChar(); // eat /
  if (currentChar == '*' || currentChar == '!')
    return nextLineCommentDoc();

  while (currentChar != EOF && currentChar != '\n' && currentChar != '\r')
    nextChar();

  return nextToken();
}

std::unique_ptr<Token> Lexer::nextTokenStringEscapeCode() noexcept {
  nextChar(); // eat '\'
  switch (currentChar) {
  case '\'':
  case '\"':
  case '\\':
  case 'a':
  case 'b':
  case 'f':
  case 'n':
  case 'r':
  case 't':
  case 'v':
    return nullptr;
  case 'x':
    nextChar(); // eat x
    if (!std::isxdigit(currentChar)) {
      return generateError(std::make_unique<LexerError>(LVL_ERROR,
        LexerErrorCode::LEX_ERR_STR_HEXADECIMAL_CHAR, Position{getCurrentCursor().line,
          currentEndIndex, currentEndIndex}));
    }
    nextChar(); // eat hex
    if (!std::isxdigit(currentChar)) {
      return generateError(std::make_unique<LexerError>(LVL_ERROR,
        LexerErrorCode::LEX_ERR_STR_HEXADECIMAL_CHAR, Position{getCurrentCursor().line,
          currentEndIndex, currentEndIndex}));
    }
    return nullptr;
  default:
    return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_STR_INVALID_ESCAPE_CODE, getCurrentCursor()));
  }
}

std::unique_ptr<Token> Lexer::nextTokenString() noexcept {
  nextChar(); // eat "

  while (currentChar != '\"' && currentChar != EOF
      && (cfg.multiLineString || (currentChar != '\r' && currentChar != '\n'))) {
    if (currentChar == '\\') {
      std::unique_ptr<Token> tok(nextTokenStringEscapeCode());
      if (tok)
        return tok; // error forwarding
    }

    nextChar();
  }

  if (currentChar != '\"')
    return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_STR_INVALID_END, Position{getCurrentCursor().line,
        currentEndIndex, currentEndIndex}));

  nextChar(); // eat "

  return std::make_unique<Token>(currentToken, TokenType::TOK_STR, getCurrentCursor());
}

std::unique_ptr<Token> Lexer::nextTokenChar() noexcept {
  nextChar(); //eat '
  
  if (currentChar == '\\') {
    std::unique_ptr<Token> tok = nextTokenStringEscapeCode();
    if (tok)
      return tok; // error forwarding
  }

  nextChar();
  if (currentChar != '\'')
    return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_CHAR_INVALID_END, Position{getCurrentCursor().line,
        currentEndIndex, currentEndIndex}));

  nextChar(); // eat '

  return std::make_unique<Token>(currentToken, TokenType::TOK_CHAR, getCurrentCursor());
}

std::unique_ptr<Token> Lexer::nextTokenNum() noexcept {
  if (currentChar == '0') {
    nextChar(); // eat 0
    switch (currentChar) {
    case 'b':
      return nextTokenBinNum();
    case 'o':
      return nextTokenOctNum();
    case 'x':
      return nextTokenHexNum();
    default:
      if (isdigit(currentChar)) {
        return generateError(std::make_unique<LexerError>(LVL_ERROR,
          LexerErrorCode::LEX_ERR_NUM_LEADING_ZERO, Position{getCurrentCursor().line,
            currentStartIndex, currentStartIndex}));
      } else if (currentChar == '.') {
        return nextTokenFltNum(0);
      }

      return nextTokenNumType(0);
    }
  }

  return nextTokenDecNum();
}

std::unique_ptr<Token> Lexer::nextTokenBinNum() noexcept {
  nextChar(); // eat b

  size_t num = 0;
  while (currentChar == '0' || currentChar == '1') {
    num *= 2;
    num += currentChar - '0';
    nextChar(); // eat binary digit
  }

  return nextTokenNumType(num);
}

std::unique_ptr<Token> Lexer::nextTokenOctNum() noexcept {
  nextChar(); // eat o

  size_t num = 0;
  while (currentChar >= '0' && currentChar <= '7') {
    num *= 8;
    num += currentChar - '0';
    nextChar(); // eat octal digit
  }

  return nextTokenNumType(num);
}

std::unique_ptr<Token> Lexer::nextTokenHexNum() noexcept {
  nextChar(); // eat x

  size_t num = 0;
  while (std::isxdigit(currentChar)) {
    num *= 16;
    if (std::isdigit(currentChar))
      num += currentChar - '0';
    else
      num += currentChar - 'A' + 10;
    nextChar(); // eat hexadecimal digit
  }

  return nextTokenNumType(num);
}

std::unique_ptr<Token> Lexer::nextTokenDecNum() noexcept {
  size_t num = 0;
  while (std::isdigit(currentChar)) {
    num *= 10;
    num += currentChar - '0';
    nextChar(); // eat decimal digit
  }

  if (currentChar == '.')
    return nextTokenFltNum(num);

  return nextTokenNumType(num);
}

std::unique_ptr<Token> Lexer::nextTokenNumType(size_t num) noexcept {
  if (isdigit(currentChar))
    return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_NUM_UNEXPECTED_CHAR_DIGIT, Position{getCurrentCursor().line,
        currentEndIndex, currentEndIndex}));

  std::unique_ptr<Token> tok(nullptr);
  switch (currentChar) {
  case 'u':
    switch (nextChar()) {
      case 's':
        nextChar();
        tok = std::make_unique<NumberToken>(currentToken, TokenType::TOK_UINT8,
          getCurrentCursor(), num);
        break;
      case 'S':
        nextChar();
        tok = std::make_unique<NumberToken>(currentToken, TokenType::TOK_UINT16,
          getCurrentCursor(), num);
        break;
      case 'l':
        nextChar();
        tok = std::make_unique<NumberToken>(currentToken, TokenType::TOK_UINT32,
          getCurrentCursor(), num);
        break;
      case 'L':
        nextChar();
        tok = std::make_unique<NumberToken>(currentToken, TokenType::TOK_UINT64,
          getCurrentCursor(), num);
        break;
      default:
        if (!isalnum(currentChar))
          tok = std::make_unique<NumberToken>(currentToken, TokenType::TOK_UINT32,
            getCurrentCursor(), num);
        break;
    }
    break;
  case 's':
    nextChar();
    tok = std::make_unique<NumberToken>(currentToken, TokenType::TOK_INT8,
      getCurrentCursor(), num);
    break;
  case 'S':
    nextChar();
    tok = std::make_unique<NumberToken>(currentToken, TokenType::TOK_INT16,
      getCurrentCursor(), num);
    break;
  case 'l':
    nextChar();
    break;
  case 'L':
    nextChar();
    tok = std::make_unique<NumberToken>(currentToken, TokenType::TOK_INT64,
      getCurrentCursor(), num);
    break;
  default:
    break;
  }

  if (isalnum(currentChar))
    return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_NUM_UNEXPECTED_CHAR, Position{getCurrentCursor().line,
        currentEndIndex, currentEndIndex}));

  if (!tok)
    return std::make_unique<NumberToken>(currentToken, TokenType::TOK_INT32,
      getCurrentCursor(), num);

  return tok;
}

std::unique_ptr<Token> Lexer::nextTokenFltNum(size_t num) noexcept {
  nextChar(); // eat .

  float f32 = static_cast<float>(num);
  double f64 = static_cast<double>(num);
  
  if (!isdigit(currentChar))
    return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_NUM_UNEXPECTED_CHAR, Position{getCurrentCursor().line,
        currentEndIndex, currentEndIndex}));

  float po32 = 10.0f;
  double po64 = 10.0;

  do {
    f32 += static_cast<float>(currentChar - '0') / po32;
    f64 += static_cast<double>(currentChar - '0') / po64;
    po32 *= 10.0f;
    po64 *= 10.0;
  } while (isdigit(nextChar()));

  std::unique_ptr<Token> tok(nullptr);
  switch (currentChar) {
  case 'f':
    nextChar(); // eat f
    tok = std::make_unique<NumberToken>(currentToken, TokenType::TOK_FLT32, getCurrentCursor(), f32);
    break;
  case 'F':
    nextChar(); // eat F
    break;
  default:
    break;
  }

  if (isalnum(currentChar))
    return generateError(std::make_unique<LexerError>(LVL_ERROR,
      LexerErrorCode::LEX_ERR_NUM_UNEXPECTED_CHAR, Position{getCurrentCursor().line,
        currentEndIndex, currentEndIndex}));

  if (tok)
    return tok;

  return std::make_unique<NumberToken>(currentToken, TokenType::TOK_FLT64, getCurrentCursor(), f64);
}

std::unique_ptr<Token> Lexer::nextTokenBracket() noexcept {
  const char c = currentChar;
  nextChar(); // eat bracket
  switch (c) {
  case ')':
    return std::make_unique<Token>(currentToken, TokenType::TOK_BRACKET_CLOSE, getCurrentCursor());
  case ']':
    return std::make_unique<Token>(currentToken, TokenType::TOK_ARR_BRACKET_CLOSE, getCurrentCursor());
  case '}':
    return std::make_unique<Token>(currentToken, TokenType::TOK_TEMPL_BRACKET_CLOSE, getCurrentCursor());
  default:
    fatal(__FILE__, __LINE__, "Unexpected branch reached");
    return nullptr;
  }
}
