#include "pfederc/lexer.hpp"
using namespace pfederc;

// Lexer
Lexer::Lexer(const LanguageConfiguration &cfg,
    std::istream &input, const std::string &filePath) noexcept
    : cfg(cfg), input{input}, filePath(filePath),
      fileContent(""), tokens(), lineIndices(), errors(),
      currentStartIndex{0}, currentEndIndex{0},
      currentChar{EOF}, currentToken{nullptr}, lastComment() {
  // 1 KiB should cover most programs
  constexpr size_t FILE_CONTENT_RESERVE = 1024 * 1024;
  fileContent.reserve(FILE_CONTENT_RESERVE); 
  constexpr size_t FILE_CONTENT_LINES = 512;
  lineIndices.reserve(FILE_CONTENT_LINES);
}

Lexer::~Lexer() {
}

Position Lexer::getCurrentCursor() const noexcept {
  return Position{lineIndices.size() - 1, currentStartIndex,
    currentEndIndex > 0 ? currentEndIndex - 1 : 0};
}

std::string Lexer::getLineFromIndex(size_t index) const noexcept {
  if (index > currentEndIndex) {
    fatal("lexer.cpp", __LINE__, "Out of bounds: " + std::to_string(index));
    return "";
  }

  size_t lineIndex = getLineNumber(index);

  size_t lineStartIndex = lineIndices[lineIndex];
  size_t lineEndIndex = lineIndex == lineIndices.size() - 1
    ? currentEndIndex : lineIndices[lineIndex + 1] - 1;
  if (lineEndIndex > lineStartIndex
        && (fileContent[lineEndIndex] == '\r'
          || fileContent[lineEndIndex] == '\n')) {
    --lineEndIndex;
  }

  std::string result(fileContent.substr(lineStartIndex,
    lineEndIndex - lineStartIndex));
  return result;
}

std::string Lexer::getLineAt(size_t lineIndex) const noexcept {
  if (lineIndex >= getLineIndices().size()) {
    fatal("lexer.cpp", __LINE__, "Out of bounds: " + std::to_string(lineIndex));
    return "";
  }

  size_t lineStartIndex = lineIndices[lineIndex];
  size_t lineEndIndex = lineIndex == lineIndices.size() - 1
    ? currentEndIndex : lineIndices[lineIndex + 1] - 1;
  if (lineEndIndex > lineStartIndex
        && (fileContent[lineEndIndex] == '\r'
          || fileContent[lineEndIndex] == '\n')) {
    --lineEndIndex;
  }

  std::string result(fileContent.substr(lineStartIndex,
    lineEndIndex - lineStartIndex));
  return result;
}

size_t Lexer::getLineNumber(size_t index) const noexcept {
  if (index > currentEndIndex) {
    fatal("lexer.cpp", __LINE__, "Out of bounds: " + std::to_string(index));
    return 0;
  }

  size_t lineIndex = lineIndices.size() - 1;
  for (; index < lineIndices[lineIndex]; --lineIndex);

  return lineIndex;
}

// error reporting

inline static std::string _logLexerErrorBase(const Lexer &lexer, const Position &pos) noexcept {
  return lexer.getFilePath() + ":"
    + std::to_string((pos.line + 1)) + ":" 
    + std::to_string(pos.startIndex - lexer.getLineIndices()[pos.line] + 1)
    + ": error: ";
}

inline static std::string _logLexerErrorMark(const Lexer &lexer, const Position &pos) noexcept {
  std::string result = lexer.getLineAt(pos.line) + '\n';
  // if end position not in same line
  const size_t lastLine = lexer.getLineNumber(pos.endIndex);
  if (lastLine > pos.line) {
    for (size_t i = pos.line; i <= lastLine; ++i) {
      result += lexer.getLineAt(i) + '\n';
    }
  }
  // print space till start
  for (size_t i = 0; i < pos.startIndex && i < lexer.getFileContent().size(); ++i)  {
    const char c = lexer.getFileContent()[i];
    const char d = c & 0xF0;
    if (c == '\t')
      result += '\t';
    else if ((~c & 0x80) == 0x80 // ascii
        || d == 0xC0 || d == 0xE0 || d == 0xF0) // utf-8 compatible
      result += ' ';
  }
  // print marks till end
  for (size_t i = pos.startIndex; i <= pos.endIndex && i < lexer.getFileContent().size(); ++i)  {
    const char c = lexer.getFileContent()[i];
    const char d = c & 0xF0;
    if ((~c & 0x80) == 0x80 // ascii
        || d == 0xC0 || d == 0xE0 || d == 0xF0) // utf-8 compatible
      result += '^';
  }
  return result;
}

std::string pfederc::logCreateErrorMessage(const Lexer &lexer, const Position &pos, const std::string &msg) noexcept {
  return _logLexerErrorBase(lexer, pos) + msg + "\n" + _logLexerErrorMark(lexer, pos);
}

inline static LogMessage _logLexerErrorLeadingZero(const Lexer &lexer, const LexerError &err) noexcept {
  return LogMessage(LVL_NOTE, "Fix: Remove zero\n"
        + lexer.getLineFromIndex(err.getPosition().startIndex)
          .erase(err.getPosition().startIndex - lexer.getLineIndices()[err.getPosition().line], 1));
}

inline static LogMessage _logLexerErrorCharInvalidEnd(const Lexer &lexer, const LexerError &err) noexcept {
  return LogMessage(LVL_NOTE, "Fix: Insert '\n"
    + lexer.getLineFromIndex(err.getPosition().startIndex)
      .insert(err.getPosition().startIndex - lexer.getLineIndices()[err.getPosition().line], "'"));
}

inline static LogMessage _logLexerErrorStrInvalidEnd(const Lexer &lexer, const LexerError &err) noexcept {
  return LogMessage(LVL_NOTE, "Fix: Insert \"\n"
    + lexer.getLineFromIndex(err.getPosition().startIndex)
      .insert(err.getPosition().startIndex - lexer.getLineIndices()[err.getPosition().line], "\""));
}

// global
LogMessage pfederc::logLexerError(const Lexer &lexer, const LexerError &err) noexcept {
  const Position &pos = err.getPosition();
  switch (err.getErrorCode()) {
  case LEX_ERR_GENERAL_INVALID_CHARACTER:
    return LogMessage(err.getLogLevel(),
      logCreateErrorMessage(lexer, pos, "Invalid character"));  
  case LEX_ERR_ID_NO_DIGIT_AFTER_ANYS:
    return LogMessage(err.getLogLevel(),
      logCreateErrorMessage(lexer, pos, "Leading _ must not be followed by digit"));  
  case LEX_ERR_ID_NOT_JUST_ANYS:
    return LogMessage(err.getLogLevel(),
      logCreateErrorMessage(lexer, pos, "Identifier must not consist of just _"));  
  case LEX_ERR_STR_INVALID_ESCAPE_CODE:
    return LogMessage(err.getLogLevel(),
      logCreateErrorMessage(lexer, pos, "Invalid escape sequence"));  
  case LEX_ERR_CHAR_INVALID_END:
    return LogMessage(err.getLogLevel(),
      logCreateErrorMessage(lexer, pos, "Expected '"),
      { _logLexerErrorCharInvalidEnd(lexer, err) });  
  case LEX_ERR_STR_INVALID_END:
    return LogMessage(err.getLogLevel(),
      logCreateErrorMessage(lexer, pos, "Expected \""),
      { _logLexerErrorStrInvalidEnd(lexer, err) });  
  case LEX_ERR_STR_HEXADECIMAL_CHAR:
    return LogMessage(err.getLogLevel(),
      logCreateErrorMessage(lexer, pos, "Expected hexadecimal character"));
  case LEX_ERR_NUM_LEADING_ZERO:
    return LogMessage(err.getLogLevel(),
      logCreateErrorMessage(lexer, pos, "Zero must not be the first character of decimal number"),
      { _logLexerErrorLeadingZero(lexer, err) });  
  case LEX_ERR_NUM_UNEXPECTED_CHAR_DIGIT:
    return LogMessage(err.getLogLevel(),
      logCreateErrorMessage(lexer, pos, "Unexpected digit"));  
  case LEX_ERR_NUM_UNEXPECTED_CHAR:
    return LogMessage(err.getLogLevel(), logCreateErrorMessage(lexer, pos, "Unexpected character"));  
  case LEX_ERR_REGION_COMMENT_END:
    return LogMessage(err.getLogLevel(), logCreateErrorMessage(lexer, pos, "Expected '*/'"));
  default:
    return LogMessage(err.getLogLevel(), logCreateErrorMessage(lexer, pos, "Unknown error"));
  }
}

bool pfederc::logLexerErrors(Logger &log, const Lexer &lex) noexcept {
  bool result = false;
  for (const auto &lexErr : lex.getErrors()) {
    LogMessage msg = logLexerError(lex, *lexErr);
    if (msg.getLogLevel() == LVL_ERROR)
      result = true;
    msg.log(log);
  }

  return result;
}
