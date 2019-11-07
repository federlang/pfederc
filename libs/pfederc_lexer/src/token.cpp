#include "pfederc/token.hpp"
using namespace pfederc;

namespace pfederc {
  const std::vector<std::tuple<TokenType, std::string>> KEYWORDS[KEYWORDS_LENGTH] = {
    // 2
    {KeywordTuple(TOK_KW_IF, "if"),
     KeywordTuple(TOK_KW_DO, "do")},
    // 3
    {KeywordTuple(TOK_KW_MOD, "mod"),
     KeywordTuple(TOK_KW_USE, "use"),
     KeywordTuple(TOK_KW_FOR, "for")},
    // 4
    {KeywordTuple(TOK_KW_FN, "func"),
     KeywordTuple(TOK_KW_ENUM, "enum"),
     KeywordTuple(TOK_KW_TYPE, "type"),
     KeywordTuple(TOK_KW_ELSE, "else"),
     KeywordTuple(TOK_KW_SAFE, "safe"),
     KeywordTuple(TOK_KW_TRUE, "True")},
    // 5
    {KeywordTuple(TOK_KW_CLASS, "class"),
     KeywordTuple(TOK_KW_TRAIT, "trait"),
     KeywordTuple(TOK_KW_MATCH, "match"),
     KeywordTuple(TOK_KW_BRK,   "break"),
     KeywordTuple(TOK_KW_FALSE, "False")},
    // 6
    {KeywordTuple(TOK_KW_RET,    "return"),
     KeywordTuple(TOK_KW_LAMBDA, "lambda"),
     KeywordTuple(TOK_KW_SWITCH, "switch"),
     KeywordTuple(TOK_KW_IMPORT, "import")},
    // 7
    {KeywordTuple(TOK_KW_INC,     "include")},
    // 8
    {KeywordTuple(TOK_KW_CTN,      "continue")},
  };

  const std::vector<OperatorTuple> OPERATORS[OPERATORS_LENGTH] = {
    // 1
    {OperatorTuple(TOK_OP_STMT, ";"),
     OperatorTuple(TOK_OP_COMMA, ","),
     OperatorTuple(TOK_OP_ASG, "="),
     OperatorTuple(TOK_OP_BOR, "|"),
     OperatorTuple(TOK_OP_BXOR, "^"),
     OperatorTuple(TOK_OP_BAND, "&"),
     OperatorTuple(TOK_OP_GT, ">"),
     OperatorTuple(TOK_OP_LT, "<"),
     OperatorTuple(TOK_OP_ADD, "+"),
     OperatorTuple(TOK_OP_SUB, "-"),
     OperatorTuple(TOK_OP_MOD, "%"),
     OperatorTuple(TOK_OP_MUL, "*"),
     OperatorTuple(TOK_OP_DIV, "/"),
     OperatorTuple(TOK_OP_DCL, ":"),
     OperatorTuple(TOK_OP_LN, "!"),
     OperatorTuple(TOK_OP_BN, "~"),
     OperatorTuple(TOK_OP_MEM, "."),
     OperatorTuple(TOK_OP_BRACKET_OPEN, "("),
     OperatorTuple(TOK_OP_ARR_BRACKET_OPEN, "["),
     OperatorTuple(TOK_OP_TEMPL_BRACKET_OPEN, "{")},
    // 2
    {OperatorTuple(TOK_OP_ASG_DCL, ":="),
     OperatorTuple(TOK_OP_ASG_AND, "&="),
     OperatorTuple(TOK_OP_ASG_XOR, "^="),
     OperatorTuple(TOK_OP_ASG_OR, "|="),
     OperatorTuple(TOK_OP_ASG_MOD, "%="),
     OperatorTuple(TOK_OP_ASG_DIV, "/="),
     OperatorTuple(TOK_OP_ASG_MUL, "*="),
     OperatorTuple(TOK_OP_ASG_SUB, "-="),
     OperatorTuple(TOK_OP_ASG_ADD, "+="),
     OperatorTuple(TOK_OP_LAND, "&&"),
     OperatorTuple(TOK_OP_LOR, "||"),
     OperatorTuple(TOK_OP_ARG, "<>"),
     OperatorTuple(TOK_OP_EQ, "=="),
     OperatorTuple(TOK_OP_NQ, "!="),
     OperatorTuple(TOK_OP_LEQ, "<="),
     OperatorTuple(TOK_OP_GEQ, ">="),
     OperatorTuple(TOK_OP_LSH, "<<"),
     OperatorTuple(TOK_OP_RSH, ">>"),
     OperatorTuple(TOK_OP_INC, "++"),
     OperatorTuple(TOK_OP_DEC, "--"),
     OperatorTuple(TOK_OP_DMEM, "->")},
    // 3
    {OperatorTuple(TOK_OP_ASG_LSH, "<<="),
     OperatorTuple(TOK_OP_ASG_RSH, ">>=")},
  };

  const std::map<TokenType, std::string> TOKEN_TYPE_STRINGS = {
    { TOK_ERR, "TOK_ERR" },
    { TOK_EOL, "TOK_EOL" },
    { TOK_EOF, "TOK_EOF" },
    { TOK_ID, "TOK_ID" },
    { TOK_INT8, "TOK_INT8" },
    { TOK_INT16, "TOK_INT16" },
    { TOK_INT32, "TOK_INT32" },
    { TOK_INT64, "TOK_INT64" },
    { TOK_UINT8, "TOK_UINT8" },
    { TOK_UINT16, "TOK_UINT16" },
    { TOK_UINT32, "TOK_UINT32" },
    { TOK_UINT64, "TOK_UINT64" },
    { TOK_FLT32, "TOK_FLT32" },
    { TOK_FLT64, "TOK_FLT64" },
    { TOK_CHAR, "TOK_CHAR" },
    { TOK_STR, "TOK_STR" },

    { TOK_OP_BRACKET_OPEN, "TOK_OP_BRACKET_OPEN" },
    { TOK_BRACKET_CLOSE, "TOK_BRACKET_CLOSE" },
    { TOK_OP_ARR_BRACKET_OPEN, "TOK_OP_ARR_BRACKET_OPEN" },
    { TOK_ARR_BRACKET_CLOSE, "TOK_ARR_BRACKET_CLOSE" },
    { TOK_OP_TEMPL_BRACKET_OPEN, "TOK_OP_TEMPL_BRACKET_OPEN" },
    { TOK_TEMPL_BRACKET_CLOSE, "TOK_TEMPL_BRACKET_CLOSE" },

    { TOK_KW_FN, "TOK_KW_FN" },
    { TOK_KW_LAMBDA, "TOK_KW_LAMBDA" },
    { TOK_KW_MOD, "TOK_KW_MOD" },
    { TOK_KW_CLASS, "TOK_KW_CLASS" },
    { TOK_KW_ENUM, "TOK_KW_ENUM" },
    { TOK_KW_TRAIT, "TOK_KW_TRAIT" },
    { TOK_KW_TYPE, "TOK_KW_TYPE" },
    { TOK_KW_RET, "TOK_KW_RET" },
    { TOK_KW_USE, "TOK_KW_USE" },
    { TOK_KW_IF, "TOK_KW_IF" },
    { TOK_KW_ELSE, "TOK_KW_ELSE" },
    { TOK_KW_MATCH, "TOK_KW_MATCH" },
    { TOK_KW_SWITCH, "TOK_KW_SWITCH" },
    { TOK_KW_FOR, "TOK_KW_FOR" },
    { TOK_KW_DO, "TOK_KW_DO" },
    { TOK_KW_CTN, "TOK_KW_CTN" },
    { TOK_KW_BRK, "TOK_KW_BRK" },
    { TOK_KW_INC, "TOK_KW_INC" },
    { TOK_KW_IMPORT, "TOK_KW_IMPORT" },
    { TOK_KW_TRUE, "TOK_KW_TRUE" },
    { TOK_KW_FALSE, "TOK_KW_FALSE" },

    { TOK_OP_COMMA, "TOK_OP_COMMA" },
    { TOK_OP_STMT, "TOK_OP_STMT" },
    { TOK_OP_ASG_DCL, "TOK_OP_ASG_DCL" },
    { TOK_OP_ASG_AND, "TOK_OP_ASG_AND" },
    { TOK_OP_ASG_XOR, "TOK_OP_ASG_XOR" },
    { TOK_OP_ASG_OR, "TOK_OP_ASG_OR" },
    { TOK_OP_ASG_LSH, "TOK_OP_ASG_LSH" },
    { TOK_OP_ASG_RSH, "TOK_OP_ASG_RSH" },
    { TOK_OP_ASG_MOD, "TOK_OP_ASG_MOD" },
    { TOK_OP_ASG_DIV, "TOK_OP_ASG_DIV" },
    { TOK_OP_ASG_MUL, "TOK_OP_ASG_MUL" },
    { TOK_OP_ASG_SUB, "TOK_OP_ASG_SUB" },
    { TOK_OP_ASG_ADD, "TOK_OP_ASG_ADD" },
    { TOK_OP_ASG, "TOK_OP_ASG" },
    { TOK_OP_NULL, "TOK_OP_NULL" },
    { TOK_OP_LAND, "TOK_OP_LAND" },
    { TOK_OP_LOR, "TOK_OP_LOR" },
    { TOK_OP_ARG, "TOK_OP_ARG" },
    { TOK_OP_BOR, "TOK_OP_BOR" },
    { TOK_OP_BXOR, "TOK_OP_BXOR" },
    { TOK_OP_BAND, "TOK_OP_BAND" },
    { TOK_OP_EQ, "TOK_OP_EQ" },
    { TOK_OP_NQ, "TOK_OP_NQ" },
    { TOK_OP_LT, "TOK_OP_LT" },
    { TOK_OP_LEQ, "TOK_OP_LEQ" },
    { TOK_OP_GT, "TOK_OP_GT" },
    { TOK_OP_GEQ, "TOK_OP_GEQ" },
    { TOK_OP_LSH, "TOK_OP_LSH" },
    { TOK_OP_RSH, "TOK_OP_RSH" },
    { TOK_OP_ADD, "TOK_OP_ADD" },
    { TOK_OP_SUB, "TOK_OP_SUB" },
    { TOK_OP_MOD, "TOK_OP_MOD" },
    { TOK_OP_MUL, "TOK_OP_MUL" },
    { TOK_OP_DIV, "TOK_OP_DIV" },
    { TOK_OP_DCL, "TOK_OP_DCL" },
    { TOK_OP_INC, "TOK_OP_INC" },
    { TOK_OP_DEC, "TOK_OP_DEC" },
    { TOK_OP_POS, "TOK_OP_POS" },
    { TOK_OP_NEG, "TOK_OP_NEG" },
    { TOK_OP_LN, "TOK_OP_LN" },
    { TOK_OP_BN, "TOK_OP_BN" },
    { TOK_KW_SAFE, "TOK_KW_SAFE" },
    { TOK_OP_DEREF, "TOK_OP_DEREF" },
    { TOK_OP_MEM, "TOK_OP_MEM" },
    { TOK_OP_DMEM, "TOK_OP_DMEM" },
    { TOK_ANY, "TOK_ANY" },
  };

  const std::map<TokenType, OperatorInfoTuple> OPERATOR_INFOS = {
    { TOK_OP_STMT,    OperatorInfoTuple( 0, BINARY, LEFT) },
    { TOK_OP_COMMA,   OperatorInfoTuple( 1, BINARY, LEFT) },
    { TOK_OP_ASG_DCL, OperatorInfoTuple( 2, BINARY, RIGHT) },
    { TOK_OP_ASG_AND, OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG_XOR, OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG_OR,  OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG_LSH, OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG_RSH, OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG_MOD, OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG_DIV, OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG_MUL, OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG_SUB, OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG_ADD, OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_ASG,     OperatorInfoTuple( 3, BINARY, RIGHT) },
    { TOK_OP_NULL,    OperatorInfoTuple( 4, BINARY, RIGHT) },
    { TOK_OP_LOR,     OperatorInfoTuple( 5, BINARY, LEFT) },
    { TOK_OP_LAND,    OperatorInfoTuple( 6, BINARY, LEFT) },
    { TOK_OP_ARG,     OperatorInfoTuple( 7, BINARY, LEFT) },
    { TOK_OP_BOR,     OperatorInfoTuple( 8, BINARY, LEFT) },
    { TOK_OP_BXOR,    OperatorInfoTuple( 9, BINARY, LEFT) },
    { TOK_OP_BAND,    OperatorInfoTuple(10, BINARY, LEFT) },
    { TOK_OP_EQ,      OperatorInfoTuple(11, BINARY, LEFT) },
    { TOK_OP_NQ,      OperatorInfoTuple(11, BINARY, LEFT) },
    { TOK_OP_LT,      OperatorInfoTuple(12, BINARY, LEFT) },
    { TOK_OP_LEQ,     OperatorInfoTuple(12, BINARY, LEFT) },
    { TOK_OP_GT,      OperatorInfoTuple(12, BINARY, LEFT) },
    { TOK_OP_GEQ,     OperatorInfoTuple(12, BINARY, LEFT) },
    { TOK_OP_LSH,     OperatorInfoTuple(13, BINARY, LEFT) },
    { TOK_OP_RSH,     OperatorInfoTuple(13, BINARY, LEFT) },
    { TOK_OP_ADD,     OperatorInfoTuple(14, BINARY, LEFT) },
    { TOK_OP_SUB,     OperatorInfoTuple(14, BINARY, LEFT) },
    { TOK_OP_MOD,     OperatorInfoTuple(14, BINARY, LEFT) },
    { TOK_OP_MUL,     OperatorInfoTuple(15, BINARY, LEFT) },
    { TOK_OP_DIV,     OperatorInfoTuple(15, BINARY, LEFT) },
    { TOK_OP_DCL,     OperatorInfoTuple(16, BINARY, RIGHT) },
    { TOK_OP_INC,     OperatorInfoTuple(17, UNARY, RIGHT) },
    { TOK_OP_DEC,     OperatorInfoTuple(17, UNARY, RIGHT) },
    { TOK_OP_POS,     OperatorInfoTuple(17, UNARY, RIGHT) },
    { TOK_OP_NEG,     OperatorInfoTuple(17, UNARY, RIGHT) },
    { TOK_OP_LN,      OperatorInfoTuple(17, UNARY, RIGHT) },
    { TOK_OP_BN,      OperatorInfoTuple(17, UNARY, RIGHT) },
    { TOK_OP_DEREF,   OperatorInfoTuple(17, UNARY, RIGHT) },
    { TOK_OP_BRACKET_OPEN, OperatorInfoTuple(18, BINARY, LEFT) },
    { TOK_OP_ARR_BRACKET_OPEN, OperatorInfoTuple(18, BINARY, LEFT) },
    { TOK_OP_TEMPL_BRACKET_OPEN, OperatorInfoTuple(18, BINARY, LEFT) },
    { TOK_OP_MEM,     OperatorInfoTuple(18, BINARY, LEFT) },
    { TOK_OP_DMEM,    OperatorInfoTuple(18, BINARY, LEFT) },
  };
}

bool pfederc::isTokenTypeKeyword(TokenType type) noexcept {
  return type >= TOK_KW_START && type <= TOK_KW_END;
}

bool pfederc::isTokenTypeOperator(TokenType type) noexcept {
  return type >= TOK_OP_START && type <= TOK_OP_END;
}

// Position

Position Position::operator +(const Position &pos) const noexcept {
  return Position{std::min(line, pos.line),
    std::min(startIndex, pos.startIndex),
    std::max(endIndex, pos.endIndex)};
}

// Token
Token::Token(Token *last, TokenType type, const Position &pos) noexcept
    : last{last}, type{type}, pos(pos) {
}

Token::~Token() {
}

Token *Token::getLast() const noexcept {
  return last;
}

TokenType Token::getType() const noexcept {
  return type;
}

const Position &Token::getPosition() const noexcept {
  return pos;
}

bool Token::operator !=(TokenType type) const noexcept {
  return getType() != type;
}

bool Token::operator ==(TokenType type) const noexcept {
  return getType() == type;
}

// NumberToken
NumberToken::NumberToken(Token *last, TokenType type,
    const Position &pos, uint64_t num) noexcept
    : Token(last, type, pos), num{num} {
}

NumberToken::NumberToken(Token *last, TokenType type,
    const Position &pos, int64_t num) noexcept
    : Token(last, type, pos), num{reinterpret_cast<const uint64_t&>(num)} {
}

NumberToken::NumberToken(Token *last, TokenType type,
    const Position &pos, float f32) noexcept
    : Token(last, type, pos), num{reinterpret_cast<const uint64_t&>(num)} {
}

NumberToken::NumberToken(Token *last, TokenType type,
    const Position &pos, double f64) noexcept
    : Token(last, type, pos), num{reinterpret_cast<const uint64_t&>(num)} {
}

NumberToken::~NumberToken() {
}

int8_t NumberToken::i8() const noexcept {
  return static_cast<int8_t>(i64());
}

int16_t NumberToken::i16() const noexcept {
  return static_cast<int16_t>(i64());
}

int32_t NumberToken::i32() const noexcept {
  return static_cast<int32_t>(i64());
}

int64_t NumberToken::i64() const noexcept {
  return reinterpret_cast<const int64_t&>(num);
}

uint8_t NumberToken::u8() const noexcept {
  return static_cast<uint8_t>(u64());
}

uint16_t NumberToken::u16() const noexcept {
  return static_cast<uint16_t>(u64());
}

uint32_t NumberToken::u32() const noexcept {
  return static_cast<uint32_t>(u64());
}

uint64_t NumberToken::u64() const noexcept {
  return num;
}

float NumberToken::f32() const noexcept {
  return reinterpret_cast<const float&>(num);
}

double NumberToken::f64() const noexcept {
  return reinterpret_cast<const double&>(num);
}
