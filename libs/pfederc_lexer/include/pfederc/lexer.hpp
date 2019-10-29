#ifndef PFEDERC_LEXER_LEXER_HPP
#define PFEDERC_LEXER_LEXER_HPP

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/token.hpp"

namespace pfederc {
  class Lexer;

  class Lexer {
    LanguageConfiguration cfg;
    std::string fileContent;
    std::string filePath;
    std::istream &input;
    std::vector<Token*> tokens; //!< All tokens read by next
    std::vector<size_t> lineIndices; //!< Indices of line beginnings
    // tmps for lexical analysis
    size_t currentStartIndex, currentEndIndex;
    char currentChar; //!< Current character invalid if currentToken == nullptr
    Token *currentToken; //!< Current token. nullptr before first next call.
    std::string lastComment; //!< Important for generating some documentation
    // METHODS for next()
    //! Reads nextChar. Sets currentChar to read character.
    char nextChar() noexcept;
    //! Reads ids, keywords and any
    Token *nextTokenId() noexcept;
    //! Reads next number
    Token *nextTokenNum() noexcept;
    Token *nextTokenBinNum() noexcept;
    Token *nextTokenOctNum() noexcept;
    Token *nextTokenHexNum() noexcept;
    Token *nextTokenDecNum() noexcept;
    //! Reads number type (non, or s,S,l,L with optional u). *num* is number.
    Token *nextTokenNumType(size_t num) noexcept;
    //! Next floating point number. *num* is left to '.'
    Token *nextTokenFltNum(size_t num) noexcept;
    Token *nextTokenChar() noexcept;
    Token *nextTokenString() noexcept;
    //! Can return nullptr
    Token *nextTokenOperator() noexcept;
  public:
    Lexer(const LanguageConfiguration &cfg,
        std::istream &input, const std::string &filePath) noexcept;
    virtual ~Lexer();

    const LanguageConfiguration &getLanguageConfiguration() const noexcept;
    const std::string &getFileContent() const noexcept;
    const std::string &getFilePath() const noexcept;
    const std::vector<Token*> &getTokens() const noexcept;
    const std::vector<size_t> &getLineIndices() const noexcept;

    /*!\brief Aquire next token from input stream
     *
     * \return Never returns nullptr (except out-of-memory)
     */
    Token *next() noexcept;
  };
}

#endif /* PFEDERC_LEXER_LEXER_HPP */
