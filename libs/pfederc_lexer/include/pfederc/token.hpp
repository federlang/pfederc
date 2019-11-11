#ifndef PFEDERC_LEXER_TOKEN_HPP
#define PFEDERC_LEXER_TOKEN_HPP

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"

namespace pfederc {
  class Token;

  struct Position {
    size_t line;
    size_t startIndex;
    size_t endIndex;

    /*!\brief Merge this and pos
     * \param pos
     * \return Returns starting line, starting index and end index of merged
     * positions.
     */
    Position operator +(const Position &pos) const noexcept;
  };

  enum TokenType : uint16_t {
    TOK_ERR,       //!< error
    TOK_EOL,       //!< end-of-line
    TOK_EOF,       //!< end-of-file

    TOK_ID,
    TOK_INT8,
    TOK_INT16,
    TOK_INT32,
    TOK_INT64,
    TOK_UINT8,
    TOK_UINT16,
    TOK_UINT32,
    TOK_UINT64,
    TOK_FLT32,
    TOK_FLT64,
    TOK_CHAR,
    TOK_STR,

    TOK_BRACKET_CLOSE,       //!< )
    TOK_ARR_BRACKET_CLOSE,   //!< ]
    TOK_TEMPL_BRACKET_CLOSE, //!< }

    TOK_KW_FN,     //!< func
    TOK_KW_LAMBDA, //!< lambda
    TOK_KW_MOD,    //!< mod
    TOK_KW_CLASS,  //!< class
    TOK_KW_ENUM,   //!< enum
    TOK_KW_TRAIT,  //!< trait
    TOK_KW_TYPE,   //!< type
    TOK_KW_RET,    //!< return
    TOK_KW_USE,    //!< use
    TOK_KW_IF,     //!< if
    TOK_KW_ELSE,   //!< else
    TOK_KW_MATCH,  //!< match
    TOK_KW_SWITCH, //!< switch
    TOK_KW_FOR,    //!< for
    TOK_KW_DO,     //!< do
    TOK_KW_CTN,    //!< continue
    TOK_KW_BRK,    //!< break
    TOK_KW_INC,    //!< include
    TOK_KW_IMPORT, //!< import
    TOK_KW_SAFE,   //!< safe
    TOK_KW_TRUE,   //!< True
    TOK_KW_FALSE,  //!< False

    TOK_STMT,    //!< ;
    TOK_IMPL, //!< =>

    TOK_OP_COMMA,   //!< ,
    TOK_OP_BRACKET_OPEN,        //!< (
    TOK_OP_ARR_BRACKET_OPEN,    //!< [
    TOK_OP_TEMPL_BRACKET_OPEN,  //!< {
    TOK_OP_ASG_DCL, //!< :=
    TOK_OP_ASG_AND, //!< &=
    TOK_OP_ASG_XOR, //!< ^=
    TOK_OP_ASG_OR,  //!< |=
    TOK_OP_ASG_LSH, //!< \<\<=
    TOK_OP_ASG_RSH, //!< \>\>=
    TOK_OP_ASG_MOD, //!< %=
    TOK_OP_ASG_DIV, //!< /=
    TOK_OP_ASG_MUL, //!< \*=
    TOK_OP_ASG_SUB, //!< -=
    TOK_OP_ASG_ADD, //!< +=
    TOK_OP_ASG,     //!< =
    TOK_OP_NULL,    //!< null a
    TOK_OP_LOR,     //!< ||
    TOK_OP_LAND,    //!< &&
    TOK_OP_ARG,     //!< \<\>
    TOK_OP_BOR,     //!< |
    TOK_OP_BXOR,    //!< ^
    TOK_OP_BAND,    //!< &
    TOK_OP_EQ,      //!< ==
    TOK_OP_NQ,      //!< !=
    TOK_OP_LT,      //!< <
    TOK_OP_LEQ,     //!< <=
    TOK_OP_GT,      //!< >
    TOK_OP_GEQ,     //!< >=
    TOK_OP_LSH,     //!< <<
    TOK_OP_RSH,     //!< >>
    TOK_OP_ADD,     //!< +
    TOK_OP_SUB,     //!< -
    TOK_OP_MOD,     //!< %
    TOK_OP_MUL,     //!< \*
    TOK_OP_DIV,     //!< /
    TOK_OP_DCL,     //!< :
    TOK_OP_INC,     //!< ++
    TOK_OP_DEC,     //!< --
    TOK_OP_POS,     //!< +a
    TOK_OP_NEG,     //!< -a
    TOK_OP_LN,      //!< !
    TOK_OP_BN,      //!< ~
    TOK_OP_DEREF,   //!< \*a
    TOK_OP_MUT,     //!< &

    TOK_OP_MEM,     //!< '.'
    TOK_OP_DMEM,    //!< -\>

    TOK_ANY,        //!< \_
  };

  constexpr TokenType TOK_KW_START = TOK_KW_FN;
  constexpr TokenType TOK_KW_END = TOK_KW_FALSE;
  /*!\return Returns true if type is a keyword, otherwise false.
   */
  bool isTokenTypeKeyword(TokenType type) noexcept;

  constexpr TokenType TOK_OP_START = TOK_OP_COMMA;
  constexpr TokenType TOK_OP_END = TOK_OP_DMEM;
  /*!\return Returns true if type is an operator, otherwise false.
   */
  bool isTokenTypeOperator(TokenType type) noexcept;

  constexpr size_t KEYWORDS_MIN_STRING_LENGTH = 2;
  constexpr size_t KEYWORDS_LENGTH = 7;
  typedef std::tuple<TokenType, std::string> KeywordTuple;
  /*!\brief Keywords with corresponding string (Feder code)
   *
   * KEYWORDS[0] are operators with length KEYWORDS_MIN_STRING_LENGTH.
   * KEYWORDS[n] are operators with length KEYWORDS_MIN_STRING_LENGTH + n;
   */
  extern const std::vector<KeywordTuple> KEYWORDS[KEYWORDS_LENGTH];

  constexpr size_t OPERATORS_MIN_STRING_LENGTH = 1;
  constexpr size_t OPERATORS_LENGTH = 3;
  typedef std::tuple<TokenType, std::string> OperatorTuple;
  /*!\brief Operators with corresponding string (Feder code)
   *
   * OPERATORS[0] are operators with length OPERATORS_MIN_STRING_LENGTH.
   * OPERATORS[n] are operators with length OPERATORS_MIN_STRING_LENGTH + n;
   */
  extern const std::vector<OperatorTuple> OPERATORS[OPERATORS_LENGTH];

  /*!\brief String of TokenType (exactly the same as TokenType)
   */
  extern const std::map<TokenType, std::string> TOKEN_TYPE_STRINGS;

  /*!\brief Contains all binary operator to unary operator convertions
   * (operators which can double has binary- and unary operators)
   */
  extern const std::map<TokenType /* biop */,
      TokenType /* unop */> TOKEN_BIOP_TO_UNOP;

  /*!\brief Operator associativity
   */
  enum class Associativity {
    LEFT,
    RIGHT,
  };

  enum class OperatorType {
    UNARY,
    BINARY
  };

  /*!\brief Operator precedence
   */
  typedef uint8_t Precedence;

  typedef std::tuple<Precedence, OperatorType, Associativity> OperatorInfoTuple;
  /*!\brief Information about operators
   */
  extern const std::map<TokenType, const OperatorInfoTuple> OPERATORS_INFO;

  class Token {
    Token *last;
    TokenType type;
    Position pos;
  public:
    /*!\brief Initializes Token
     * \param last If first token in file nullptr, otherwise not
     * \param type
     * \param pos
     */
    Token(Token *last, TokenType type, const Position &pos) noexcept;
    Token(const Token &) = delete;
    virtual ~Token();

    /*!\return Returns previous read token. If there isn't any previous token
     * (0th token in file), then *nullptr* is returned.
     */
    Token *getLast() const noexcept;
    TokenType getType() const noexcept;
    const Position &getPosition() const noexcept;

    bool operator !=(TokenType type) const noexcept;
    bool operator ==(TokenType type) const noexcept;
  };

  class NumberToken : public Token {
    uint64_t num;
  public:
    NumberToken(Token *last, TokenType type, const Position &pos, uint64_t num) noexcept;
    NumberToken(Token *last, TokenType type, const Position &pos, int64_t num) noexcept;
    NumberToken(Token *last, TokenType type, const Position &pos, float num) noexcept;
    NumberToken(Token *last, TokenType type, const Position &pos, double num) noexcept;
    virtual ~NumberToken();

    int8_t  i8() const noexcept;
    int16_t i16() const noexcept;
    int32_t i32() const noexcept;
    int64_t i64() const noexcept;

    uint8_t  u8() const noexcept;
    uint16_t u16() const noexcept;
    uint32_t u32() const noexcept;
    uint64_t u64() const noexcept;

    float f32() const noexcept;
    double f64() const noexcept;
  };
}

#endif /* PFEDERC_LEXER_TOKEN_HPP */
