#ifndef PFEDERC_LEXER_LEXER_HPP
#define PFEDERC_LEXER_LEXER_HPP

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/token.hpp"

namespace pfederc {
  class Lexer;
  class LexerError;

  enum LexerErrorCode {
    LEX_ERR_GENERAL_INVALID_CHARACTER,
    LEX_ERR_ID_NO_DIGIT_AFTER_ANYS,
    LEX_ERR_ID_NOT_JUST_ANYS,
    LEX_ERR_STR_INVALID_ESCAPE_CODE,
    LEX_ERR_CHAR_INVALID_END,
    LEX_ERR_STR_INVALID_END,
    LEX_ERR_STR_HEXADECIMAL_CHAR,
    LEX_ERR_NUM_LEADING_ZERO,
    LEX_ERR_NUM_UNEXPECTED_CHAR_DIGIT,
    LEX_ERR_NUM_UNEXPECTED_CHAR,
    LEX_ERR_REGION_COMMENT_END,
  };

  class Lexer {
    LanguageConfiguration cfg;
    std::istream &input;
    std::string filePath;
    std::string fileContent;
    std::vector<Token*> tokens; //!< All tokens read by next
    std::vector<size_t> lineIndices; //!< Indices of line beginnings
    std::vector<LexerError*> errors; //!< Generated errors
    // tmps for lexical analysis
    size_t currentStartIndex, currentEndIndex;
    int currentChar; //!< Current character invalid if currentToken == nullptr
    Token *currentToken; //!< Current token. nullptr before first next call.
    std::string lastComment; //!< Important for generating some documentation
    // METHODS for next()
    //! Reads nextChar. Sets currentChar to read character.
    int nextChar() noexcept;
    Token *nextToken() noexcept;
    void skipSpace() noexcept;
    //! Reads ids, keywords and any
    Token *nextTokenId() noexcept;
    Token *nextTokenLine() noexcept;
    Token *nextTokenEOF() noexcept;
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
    //! Returns nullptr if successfull otherwise error token
    Token *nextTokenStringEscapeCode() noexcept;
    //! Can return nullptr
    Token *nextTokenOperator() noexcept;
    Token *nextTokenBracket() noexcept;
    // comments
    Token *nextRegionComment() noexcept;
    Token *nextLineComment() noexcept;
    Token *nextRegionCommentDoc() noexcept;
    Token *nextLineCommentDoc() noexcept;

    Token *generateError(LexerError *err) noexcept;
  public:
    Lexer(const LanguageConfiguration &cfg,
        std::istream &input, const std::string &filePath) noexcept;
    virtual ~Lexer();

    const LanguageConfiguration &getLanguageConfiguration() const noexcept;
    const std::string &getFileContent() const noexcept;
    const std::string &getFilePath() const noexcept;
    const std::vector<Token*> &getTokens() const noexcept;
    const std::vector<size_t> &getLineIndices() const noexcept;
    const std::vector<LexerError*> &getErrors() const noexcept;

    /*!\return Returns current token position (last next call)
     */
    Position getCurrentCursor() const noexcept;

    /*!\return Returns line at position index
     *
     * If index is out-of-bounds a fatal occurs 
     */
    std::string getLineFromIndex(size_t index) const noexcept;


    /*!\return Returns 'index' line
     *
     * If index is out-of-bounds a fatal occurs
     */
    std::string getLineAt(size_t lineIndex) const noexcept;

    /*!\return Returns line's number starting from 0 which contains the
     * character at position 'index'
     *
     * If index is out-of-bounds a fatal occurs
     */
    size_t getLineNumber(size_t index) const noexcept;

    /*!\brief Aquire next token from input stream
     *
     * \return Never returns nullptr (except out-of-memory)
     */
    Token *next() noexcept;
  };

  class LexerError {
    Level logLevel;
    LexerErrorCode err;
    Position pos;
  public:
    /*!\brief Initializes LexerError
     * \param logLevel
     * \param err
     * \param pos Error position
     */
    LexerError(Level logLevel, LexerErrorCode err, const Position &pos) noexcept;
    virtual ~LexerError();

    Level getLogLevel() const noexcept;
    LexerErrorCode getErrorCode() const noexcept;

    /*!\return Returns where the error occured
     */
    const Position &getPosition() const noexcept;
  };

  LogMessage logLexerError(const Lexer &lexer, const LexerError &err) noexcept;
  /*!\return Returns true if an error occured
   */
  bool logLexerErrors(Logger &log, const Lexer &lex) noexcept;
}

#endif /* PFEDERC_LEXER_LEXER_HPP */
