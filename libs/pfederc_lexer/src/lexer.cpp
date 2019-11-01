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
  for (Token *tok : tokens)
    delete tok;
  for (LexerError *err : errors)
    delete err;
}

const LanguageConfiguration &Lexer::getLanguageConfiguration() const noexcept {
  return cfg;
}

const std::string &Lexer::getFileContent() const noexcept {
  return fileContent;
}

const std::string &Lexer::getFilePath() const noexcept {
  return filePath;
}

const std::vector<Token*> &Lexer::getTokens() const noexcept {
  return tokens;
}

const std::vector<size_t> &Lexer::getLineIndices() const noexcept {
  return lineIndices;
}

const std::vector<LexerError*> &Lexer::getErrors() const noexcept {
  return errors;
}

Position Lexer::getCurrentCursor() const noexcept {
  return Position{lineIndices.size() - 1, currentStartIndex, currentEndIndex - 1};
}

std::string Lexer::getLine(size_t index) const noexcept {
  if (index > currentEndIndex) {
    fatal(__FILE__, __LINE__, "Out of bounds");
    return "";
  }

  size_t lineIndex = lineIndices.size() - 1;
  for (; index < lineIndices[lineIndex]; --lineIndex);

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

// LexerError

LexerError::LexerError(Level logLevel, LexerErrorCode err,
    const Position &pos) noexcept
    : logLevel{logLevel}, err{err}, pos(pos) {
}

LexerError::~LexerError() {
}

Level LexerError::getLogLevel() const noexcept {
  return logLevel;
}

LexerErrorCode LexerError::getErrorCode() const noexcept {
  return err;
}

const Position &LexerError::getPosition() const noexcept {
  return pos;
}

// error reporting

inline static std::string _logLexerErrorBase(const Lexer &lexer, const LexerError &err) noexcept {
  return lexer.getFilePath() + ":"
    + std::to_string((err.getPosition().line + 1)) + ":" 
    + std::to_string(err.getPosition().startIndex - lexer.getLineIndices()[err.getPosition().line] + 1)
    + ": error: ";
}

inline static std::string _logLexerErrorMark(const Lexer &lexer, const LexerError &err) noexcept {
  std::string result = lexer.getLine(err.getPosition().startIndex) + '\n';
  // print space till start
  for (size_t i = 0; i < err.getPosition().startIndex && i < lexer.getFileContent().size(); ++i)  {
    const char c = lexer.getFileContent()[i];
    const char d = c & 0xF0;
    if (c == '\t')
      result += '\t';
    else if ((~c & 0x80) == 0x80 // ascii
        || d == 0xC0 || d == 0xE0 || d == 0xF0) // utf-8 compatible
      result += ' ';
  }
  // print marks till end
  for (size_t i = err.getPosition().startIndex; i <= err.getPosition().endIndex && i < lexer.getFileContent().size(); ++i)  {
    const char c = lexer.getFileContent()[i];
    const char d = c & 0xF0;
    if ((~c & 0x80) == 0x80 // ascii
        || d == 0xC0 || d == 0xE0 || d == 0xF0) // utf-8 compatible
      result += '^';
  }
  return result;
}

inline static std::string _logLexerErrorMsg(const Lexer &lexer, const LexerError &err, const std::string &msg) noexcept {
  return _logLexerErrorBase(lexer, err) + msg + "\n" + _logLexerErrorMark(lexer, err);
}

inline static LogMessage _logLexerErrorLeadingZero(const Lexer &lexer, const LexerError &err) noexcept {
  return LogMessage(LVL_NOTE, "Fix: Remove zero\n"
        + lexer.getLine(err.getPosition().startIndex)
          .erase(err.getPosition().startIndex - lexer.getLineIndices()[err.getPosition().line], 1));
}

inline static LogMessage _logLexerErrorCharInvalidEnd(const Lexer &lexer, const LexerError &err) noexcept {
  return LogMessage(LVL_NOTE, "Fix: Insert '\n"
    + lexer.getLine(err.getPosition().startIndex)
      .insert(err.getPosition().startIndex - lexer.getLineIndices()[err.getPosition().line], "'"));
}

inline static LogMessage _logLexerErrorStrInvalidEnd(const Lexer &lexer, const LexerError &err) noexcept {
  return LogMessage(LVL_NOTE, "Fix: Insert \"\n"
    + lexer.getLine(err.getPosition().startIndex)
      .insert(err.getPosition().startIndex - lexer.getLineIndices()[err.getPosition().line], "\""));
}

// global
LogMessage pfederc::logLexerError(const Lexer &lexer, const LexerError &err) noexcept {
  switch (err.getErrorCode()) {
  case LEX_ERR_GENERAL_INVALID_CHARACTER:
    return LogMessage(err.getLogLevel(),
      _logLexerErrorMsg(lexer, err, "Invalid character"));  
  case LEX_ERR_ID_NO_DIGIT_AFTER_ANYS:
    return LogMessage(err.getLogLevel(),
      _logLexerErrorMsg(lexer, err, "Leading _ must not be followed by digit"));  
  case LEX_ERR_ID_NOT_JUST_ANYS:
    return LogMessage(err.getLogLevel(),
      _logLexerErrorMsg(lexer, err, "Identifier must not consist of just _"));  
  case LEX_ERR_STR_INVALID_ESCAPE_CODE:
    return LogMessage(err.getLogLevel(),
      _logLexerErrorMsg(lexer, err, "Invalid escape sequence"));  
  case LEX_ERR_CHAR_INVALID_END:
    return LogMessage(err.getLogLevel(),
      _logLexerErrorMsg(lexer, err, "Expected '"),
      { _logLexerErrorCharInvalidEnd(lexer, err) });  
  case LEX_ERR_STR_INVALID_END:
    return LogMessage(err.getLogLevel(),
      _logLexerErrorMsg(lexer, err, "Expected \""),
      { _logLexerErrorStrInvalidEnd(lexer, err) });  
  case LEX_ERR_STR_HEXADECIMAL_CHAR:
    return LogMessage(err.getLogLevel(),
      _logLexerErrorMsg(lexer, err, "Expected hexadecimal character"));
  case LEX_ERR_NUM_LEADING_ZERO:
    return LogMessage(err.getLogLevel(),
      _logLexerErrorMsg(lexer, err, "Zero must not be the first character of decimal number"),
      { _logLexerErrorLeadingZero(lexer, err) });  
  case LEX_ERR_NUM_UNEXPECTED_CHAR_DIGIT:
    return LogMessage(err.getLogLevel(),
      _logLexerErrorMsg(lexer, err, "Unexpected digit"));  
  case LEX_ERR_NUM_UNEXPECTED_CHAR:
    return LogMessage(err.getLogLevel(), _logLexerErrorMsg(lexer, err, "Unexpected character"));  
  default:
    return LogMessage(err.getLogLevel(), _logLexerErrorMsg(lexer, err, "Unknown error"));
  }
}

bool pfederc::logLexerErrors(Logger &log, const Lexer &lex) noexcept {
  bool result = false;
  for (const auto lexErr : lex.getErrors()) {
    LogMessage msg = logLexerError(lex, *lexErr);
    if (msg.getLogLevel() == LVL_ERROR)
      result = true;
    msg.log(log);
  }

  return result;
}
