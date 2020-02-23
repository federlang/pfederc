#include "pfederc/token.hpp"
using namespace pfederc;

namespace pfederc {
  const std::vector<std::tuple<TokenType, std::string>> KEYWORDS[KEYWORDS_LENGTH] {
    // 2
    {KeywordTuple(TOK_KW_IF, "if"),
     KeywordTuple(TOK_KW_DO, "do")},
    // 3
    {KeywordTuple(TOK_KW_USE, "use"),
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
     KeywordTuple(TOK_KW_MOD,    "module"),
     KeywordTuple(TOK_KW_LAMBDA, "lambda"),
     KeywordTuple(TOK_KW_ENSURE, "ensure"),
     KeywordTuple(TOK_KW_SWITCH, "switch"),
     KeywordTuple(TOK_KW_IMPORT, "import")},
    // 7
    {KeywordTuple(TOK_KW_INC,     "include")},
    // 8
    {KeywordTuple(TOK_KW_CTN,      "continue")},
  };

  const std::vector<OperatorTuple> OPERATORS[OPERATORS_LENGTH] {
    // 1
    {OperatorTuple(TOK_STMT, ";"),

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
    {OperatorTuple(TOK_IMPL, "=>"),

     OperatorTuple(TOK_OP_ASG_DCL, ":="),
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

  const std::map<TokenType, std::string> TOKEN_TYPE_STRINGS {
    { TOK_ERR, "TOK_ERR" },
    { TOK_EOL, "TOK_EOL" },
    { TOK_EOF, "TOK_EOF" },
    { TOK_DIRECTIVE, "TOK_DIRECTIVE" },
    { TOK_ENSURE, "TOK_ENSURE" },
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
    { TOK_KW_ENSURE, "TOK_KW_ENSURE" },
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

    { TOK_STMT, "TOK_STMT" },
    { TOK_IMPL, "TOK_IMPL" },

    { TOK_OP_COMMA, "TOK_OP_COMMA" },
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
    { TOK_OP_NONE, "TOK_OP_NONE" },
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
    { TOK_OP_MUT, "TOK_OP_MUT" },
    { TOK_OP_MEM, "TOK_OP_MEM" },
    { TOK_OP_DMEM, "TOK_OP_DMEM" },
    { TOK_ANY, "TOK_ANY" },
  };

  const std::map<TokenType, const OperatorInfoTuple> OPERATORS_INFO {
    { TOK_OP_COMMA,   OperatorInfoTuple( 1, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_ASG_DCL, OperatorInfoTuple( 2, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_AND, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_XOR, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_OR,  OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_LSH, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_RSH, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_MOD, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_DIV, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_MUL, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_SUB, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG_ADD, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_ASG,     OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_NULL,    OperatorInfoTuple( 4, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_LOR,     OperatorInfoTuple( 5, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_LAND,    OperatorInfoTuple( 6, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_ARG,     OperatorInfoTuple( 7, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_NONE,    OperatorInfoTuple( 7, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_BOR,     OperatorInfoTuple( 8, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_BXOR,    OperatorInfoTuple( 9, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_BAND,    OperatorInfoTuple(10, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_EQ,      OperatorInfoTuple(11, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_NQ,      OperatorInfoTuple(11, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_LT,      OperatorInfoTuple(12, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_LEQ,     OperatorInfoTuple(12, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_GT,      OperatorInfoTuple(12, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_GEQ,     OperatorInfoTuple(12, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_LSH,     OperatorInfoTuple(13, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_RSH,     OperatorInfoTuple(13, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_ADD,     OperatorInfoTuple(14, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_SUB,     OperatorInfoTuple(14, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_MOD,     OperatorInfoTuple(14, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_MUL,     OperatorInfoTuple(15, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_DIV,     OperatorInfoTuple(15, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_DCL,     OperatorInfoTuple(16, OperatorType::BINARY, Associativity::RIGHT) },
    { TOK_OP_INC,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TOK_OP_DEC,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TOK_OP_POS,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TOK_OP_NEG,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TOK_OP_LN,      OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TOK_OP_BN,      OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TOK_OP_DEREF,   OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TOK_OP_MUT,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TOK_OP_BRACKET_OPEN, OperatorInfoTuple(18,       OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_ARR_BRACKET_OPEN, OperatorInfoTuple(18,   OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_TEMPL_BRACKET_OPEN, OperatorInfoTuple(18, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_MEM,     OperatorInfoTuple(18, OperatorType::BINARY, Associativity::LEFT) },
    { TOK_OP_DMEM,    OperatorInfoTuple(18, OperatorType::BINARY, Associativity::LEFT) },
  };

  const std::map<TokenType, TokenType> TOKEN_BIOP_TO_UNOP {
    { TOK_OP_ADD, TOK_OP_POS },
    { TOK_OP_SUB, TOK_OP_NEG },
    { TOK_OP_MUL, TOK_OP_DEREF },
    { TOK_OP_BAND, TOK_OP_MUT },
  };


  const std::map<TokenType /* open bracket */,
    TokenType /* closing bracket */> TOKEN_BRACKETS = {
      { TOK_OP_BRACKET_OPEN, TOK_BRACKET_CLOSE },
      { TOK_OP_ARR_BRACKET_OPEN, TOK_ARR_BRACKET_CLOSE },
      { TOK_OP_TEMPL_BRACKET_OPEN, TOK_TEMPL_BRACKET_CLOSE },
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

bool Token::operator !=(TokenType type) const noexcept {
  return getType() != type;
}

bool Token::operator ==(TokenType type) const noexcept {
  return getType() == type;
}

// NumberToken
NumberToken::NumberToken(Token *last, TokenType type,
    const Position &pos, uint64_t num) noexcept
    : Token(last, type, pos) {
	this->num.u64 = num;
}

NumberToken::NumberToken(Token *last, TokenType type,
    const Position &pos, int64_t num) noexcept
    : Token(last, type, pos) {
	this->num.i64 = num;
}

NumberToken::NumberToken(Token *last, TokenType type,
    const Position &pos, float f32) noexcept
    : Token(last, type, pos) {
	this->num.f32 = f32;
}

NumberToken::NumberToken(Token *last, TokenType type,
    const Position &pos, double f64) noexcept
    : Token(last, type, pos) {
	this->num.f64 = f64;
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
	return num.i64;
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
  return num.u64;
}

float NumberToken::f32() const noexcept {
	return num.f32;
}

double NumberToken::f64() const noexcept {
	return num.f64;
}

std::string NumberToken::toString(const Lexer &lexer) const noexcept {
  switch(getType()) {
  case TOK_INT8:
    return std::to_string(i8()) + 's';
  case TOK_INT16:
    return std::to_string(i16()) + 'S';
  case TOK_INT32:
    return std::to_string(i32());
  case TOK_INT64:
    return std::to_string(i64()) + 'L';
  case TOK_UINT8:
    return std::to_string(u8()) + "us";
  case TOK_UINT16:
    return std::to_string(u16()) + "uS";
  case TOK_UINT32:
    return std::to_string(u32()) + 'u';
  case TOK_UINT64:
    return std::to_string(u64()) + "uL";
  case TOK_FLT32:
    return std::to_string(f32()) + 'f';
  case TOK_FLT64:
    return std::to_string(f64()) + 'F';
  default:
    fatal(__FILE__, __LINE__, "Unexpected number token type");
    return "";
  }
}

// StringToken
StringToken::StringToken(Token *last, TokenType type, const Position &pos,
            const std::string &str) noexcept
    : Token(last, type, pos), str(str) {
}

StringToken::~StringToken() {
}

std::string StringToken::toString(const Lexer &lexer) const noexcept {
  return str;
}
