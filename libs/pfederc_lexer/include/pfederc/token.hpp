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

  enum TokenType {
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

    TOK_KW_FN,     //!< func
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
    TOK_KW_IMP,    //!< import
    TOK_KW_TRUE,   //!< True
    TOK_KW_FALSE,  //!< False

    TOK_OP_COMMA,   //!< ,
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
    TOK_OP_LAND,    //!< &&
    TOK_OP_LOR,     //!< ||
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
    TOK_OP_SF,      //!< safe a
    TOK_OP_DEREF,   //!< \*a

    TOK_ANY,        //!< \_
  };

  std::string tokenTypeToString(TokenType type) noexcept;

  class Token {
    TokenType type;
    Position pos;
  public:
    Token(TokenType type, const Position &pos) noexcept;
    virtual ~Token();

    TokenType getType() const noexcept;
    const Position &getPosition() const noexcept;
  };

  class NumberToken : Token {
    uint64_t num;
  public:
    NumberToken(TokenType type, const Position &pos, uint64_t num) noexcept;
    NumberToken(TokenType type, const Position &pos, int64_t num) noexcept;
    virtual ~NumberToken();

    int8_t  i8() const noexcept;
    int16_t i16() const noexcept;
    int32_t i32() const noexcept;
    int64_t i64() const noexcept;

    uint8_t  u8() const noexcept;
    uint16_t u16() const noexcept;
    uint32_t u32() const noexcept;
    uint64_t u64() const noexcept;
  };
}

#endif /* PFEDERC_LEXER_TOKEN_HPP */
