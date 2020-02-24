#ifndef PFEDERC_LEXER_LEXER_HPP
#define PFEDERC_LEXER_LEXER_HPP

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/token.hpp"

namespace pfederc {
  class Lexer;
  class LexerError;

  enum class LexerErrorCode {
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
    std::vector<std::unique_ptr<Token>> tokens; //!< All tokens read by next
    std::vector<size_t> lineIndices; //!< Indices of line beginnings
    std::vector<std::unique_ptr<LexerError>> errors; //!< Generated errors
    // tmps for lexical analysis
    size_t currentStartIndex, currentEndIndex;
    int currentChar; //!< Current character invalid if currentToken == nullptr
    Token *currentToken; //!< Current token. nullptr before first next call.
    std::string lastComment; //!< Important for generating some documentation

    // METHODS for next()
    //! Reads nextChar. Sets currentChar to read character.
    int nextChar() noexcept;
    std::unique_ptr<Token> nextToken() noexcept;
    void skipSpace() noexcept;
    //! Reads ids, keywords and any
    std::unique_ptr<Token> nextTokenId() noexcept;
    std::unique_ptr<Token> nextTokenLine() noexcept;
    std::unique_ptr<Token> nextTokenEOF() noexcept;
    //! Reads next number
    std::unique_ptr<Token> nextTokenNum() noexcept;
    std::unique_ptr<Token> nextTokenBinNum() noexcept;
    std::unique_ptr<Token> nextTokenOctNum() noexcept;
    std::unique_ptr<Token> nextTokenHexNum() noexcept;
    std::unique_ptr<Token> nextTokenDecNum() noexcept;
    //! Reads number type (non, or s,S,l,L with optional u). *num* is number.
    std::unique_ptr<Token> nextTokenNumType(size_t num) noexcept;
    //! Next floating point number. *num* is left to '.'
    std::unique_ptr<Token> nextTokenFltNum(size_t num) noexcept;
    std::unique_ptr<Token> nextTokenChar() noexcept;
    std::unique_ptr<Token> nextTokenString() noexcept;
    //! Returns nullptr if successfull otherwise error token
    std::unique_ptr<Token> nextTokenStringEscapeCode() noexcept;
    //! Can return nullptr
    std::unique_ptr<Token> nextTokenOperator() noexcept;
    std::unique_ptr<Token> nextTokenBracket() noexcept;
    // comments
    std::unique_ptr<Token> nextRegionComment() noexcept;
    std::unique_ptr<Token> nextLineComment() noexcept;
    std::unique_ptr<Token> nextRegionCommentDoc() noexcept;
    std::unique_ptr<Token> nextLineCommentDoc() noexcept;
    // capabilities
    std::unique_ptr<Token> nextTokenCapability() noexcept;

    std::unique_ptr<Token> generateError(std::unique_ptr<LexerError> &&err) noexcept;
  public:
    Lexer(const LanguageConfiguration &cfg,
        std::istream &input, const std::string &filePath) noexcept;
    virtual ~Lexer();

    inline const auto &getLanguageConfiguration() const noexcept {
      return cfg;
    }
    
    inline const auto &getFileContent() const noexcept {
      return fileContent;
    }
    
    inline const auto &getFilePath() const noexcept {
      return filePath;
    }
    
    inline const auto &getTokens() const noexcept {
      return tokens;
    }
    
    inline const auto &getLineIndices() const noexcept {
      return lineIndices;
    }
    
    inline const auto &getErrors() const noexcept {
      return errors;
    }

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
    Token &next() noexcept;

    /*!\return Returns last token read with next
     *
     * Undefined behaviour if next wasn't called before
     */
    inline Token *getCurrentToken() noexcept {
      return currentToken;
    }
  };

  class LexerError final {
    Level logLevel;
    LexerErrorCode err;
    Position pos;
  public:
    /*!\brief Initializes LexerError
     * \param logLevel
     * \param err
     * \param pos Error position
     */
    constexpr LexerError(Level logLevel, LexerErrorCode err,
        const Position &pos) noexcept
        : logLevel{logLevel}, err{err}, pos(pos) {
    }
    inline ~LexerError() {}

    inline Level getLogLevel() const noexcept {
      return logLevel;
    }
    
    inline LexerErrorCode getErrorCode() const noexcept {
      return err;
    }

    /*!\return Returns where the error occured
     */
    inline const Position &getPosition() const noexcept {
      return pos;
    }
  };

  LogMessage logLexerError(const Lexer &lexer, const LexerError &err) noexcept;

  std::string logCreateErrorMessage(const Lexer &lexer,
    const Position &pos, const std::string &msg) noexcept;

  /*!\return Returns true if an error occured
   */
  bool logLexerErrors(Logger &log, const Lexer &lex) noexcept;
}

#endif /* PFEDERC_LEXER_LEXER_HPP */
