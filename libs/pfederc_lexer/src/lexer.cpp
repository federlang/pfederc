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
