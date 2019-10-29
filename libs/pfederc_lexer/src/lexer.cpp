#include "pfederc/lexer.hpp"
using namespace pfederc;

Lexer::Lexer(const LanguageConfiguration &cfg,
    std::istream &input, const std::string &filePath) noexcept
    : cfg(cfg), input{input}, filePath(filePath) {
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
