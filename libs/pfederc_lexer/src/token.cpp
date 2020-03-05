#include "pfederc/token.hpp"
using namespace pfederc;

namespace pfederc {
  const std::vector<std::tuple<TokenType, std::string>> KEYWORDS[KEYWORDS_LENGTH] {
    // 2
    {KeywordTuple(TokenType::TOK_KW_IF, "if"),
     KeywordTuple(TokenType::TOK_KW_DO, "do")},
    // 3
    {KeywordTuple(TokenType::TOK_KW_USE, "use"),
     KeywordTuple(TokenType::TOK_KW_FOR, "for")},
    // 4
    {KeywordTuple(TokenType::TOK_KW_FN, "func"),
     KeywordTuple(TokenType::TOK_KW_ENUM, "enum"),
     KeywordTuple(TokenType::TOK_KW_TYPE, "type"),
     KeywordTuple(TokenType::TOK_KW_ELSE, "else"),
     KeywordTuple(TokenType::TOK_KW_SAFE, "safe"),
     KeywordTuple(TokenType::TOK_KW_TRUE, "True")},
    // 5
    {KeywordTuple(TokenType::TOK_KW_CLASS, "class"),
     KeywordTuple(TokenType::TOK_KW_TRAIT, "trait"),
     KeywordTuple(TokenType::TOK_KW_MATCH, "match"),
     KeywordTuple(TokenType::TOK_KW_BRK,   "break"),
     KeywordTuple(TokenType::TOK_KW_FALSE, "False")},
    // 6
    {KeywordTuple(TokenType::TOK_KW_RET,    "return"),
     KeywordTuple(TokenType::TOK_KW_MOD,    "module"),
     KeywordTuple(TokenType::TOK_KW_LAMBDA, "lambda"),
     KeywordTuple(TokenType::TOK_KW_ENSURE, "ensure"),
     KeywordTuple(TokenType::TOK_KW_SWITCH, "switch"),
     KeywordTuple(TokenType::TOK_KW_IMPORT, "import")},
    // 7
    {KeywordTuple(TokenType::TOK_KW_INC,     "include")},
    // 8
    {KeywordTuple(TokenType::TOK_KW_CTN,      "continue")},
  };

  const std::vector<OperatorTuple> OPERATORS[OPERATORS_LENGTH] {
    // 1
    {OperatorTuple(TokenType::TOK_STMT, ";"),

     OperatorTuple(TokenType::TOK_OP_COMMA, ","),
     OperatorTuple(TokenType::TOK_OP_ASG, "="),
     OperatorTuple(TokenType::TOK_OP_BOR, "|"),
     OperatorTuple(TokenType::TOK_OP_BXOR, "^"),
     OperatorTuple(TokenType::TOK_OP_BAND, "&"),
     OperatorTuple(TokenType::TOK_OP_GT, ">"),
     OperatorTuple(TokenType::TOK_OP_LT, "<"),
     OperatorTuple(TokenType::TOK_OP_ADD, "+"),
     OperatorTuple(TokenType::TOK_OP_SUB, "-"),
     OperatorTuple(TokenType::TOK_OP_MOD, "%"),
     OperatorTuple(TokenType::TOK_OP_MUL, "*"),
     OperatorTuple(TokenType::TOK_OP_DIV, "/"),
     OperatorTuple(TokenType::TOK_OP_DCL, ":"),
     OperatorTuple(TokenType::TOK_OP_LN, "!"),
     OperatorTuple(TokenType::TOK_OP_BN, "~"),
     OperatorTuple(TokenType::TOK_OP_MEM, "."),
     OperatorTuple(TokenType::TOK_OP_BRACKET_OPEN, "("),
     OperatorTuple(TokenType::TOK_OP_ARR_BRACKET_OPEN, "["),
     OperatorTuple(TokenType::TOK_OP_TEMPL_BRACKET_OPEN, "{")},
    // 2
    {OperatorTuple(TokenType::TOK_IMPL, "=>"),

     OperatorTuple(TokenType::TOK_OP_ASG_DCL, ":="),
     OperatorTuple(TokenType::TOK_OP_ASG_AND, "&="),
     OperatorTuple(TokenType::TOK_OP_ASG_XOR, "^="),
     OperatorTuple(TokenType::TOK_OP_ASG_OR, "|="),
     OperatorTuple(TokenType::TOK_OP_ASG_MOD, "%="),
     OperatorTuple(TokenType::TOK_OP_ASG_DIV, "/="),
     OperatorTuple(TokenType::TOK_OP_ASG_MUL, "*="),
     OperatorTuple(TokenType::TOK_OP_ASG_SUB, "-="),
     OperatorTuple(TokenType::TOK_OP_ASG_ADD, "+="),
     OperatorTuple(TokenType::TOK_OP_LAND, "&&"),
     OperatorTuple(TokenType::TOK_OP_LOR, "||"),
     OperatorTuple(TokenType::TOK_OP_ARG, "<>"),
     OperatorTuple(TokenType::TOK_OP_EQ, "=="),
     OperatorTuple(TokenType::TOK_OP_NQ, "!="),
     OperatorTuple(TokenType::TOK_OP_LEQ, "<="),
     OperatorTuple(TokenType::TOK_OP_GEQ, ">="),
     OperatorTuple(TokenType::TOK_OP_LSH, "<<"),
     OperatorTuple(TokenType::TOK_OP_RSH, ">>"),
     OperatorTuple(TokenType::TOK_OP_INC, "++"),
     OperatorTuple(TokenType::TOK_OP_DEC, "--"),
     OperatorTuple(TokenType::TOK_OP_DMEM, "->")},
    // 3
    {OperatorTuple(TokenType::TOK_OP_ASG_LSH, "<<="),
     OperatorTuple(TokenType::TOK_OP_ASG_RSH, ">>=")},
  };

  const std::unordered_map<TokenType, std::string> TOKEN_TYPE_STRINGS {
    { TokenType::TOK_ERR, "TOK_ERR" },
    { TokenType::TOK_EOL, "TOK_EOL" },
    { TokenType::TOK_EOF, "TOK_EOF" },
    { TokenType::TOK_DIRECTIVE, "TOK_DIRECTIVE" },
    { TokenType::TOK_ENSURE, "TOK_ENSURE" },
    { TokenType::TOK_ID, "TOK_ID" },
    { TokenType::TOK_INT8, "TOK_INT8" },
    { TokenType::TOK_INT16, "TOK_INT16" },
    { TokenType::TOK_INT32, "TOK_INT32" },
    { TokenType::TOK_INT64, "TOK_INT64" },
    { TokenType::TOK_UINT8, "TOK_UINT8" },
    { TokenType::TOK_UINT16, "TOK_UINT16" },
    { TokenType::TOK_UINT32, "TOK_UINT32" },
    { TokenType::TOK_UINT64, "TOK_UINT64" },
    { TokenType::TOK_FLT32, "TOK_FLT32" },
    { TokenType::TOK_FLT64, "TOK_FLT64" },
    { TokenType::TOK_CHAR, "TOK_CHAR" },
    { TokenType::TOK_STR, "TOK_STR" },

    { TokenType::TOK_OP_BRACKET_OPEN, "TOK_OP_BRACKET_OPEN" },
    { TokenType::TOK_BRACKET_CLOSE, "TOK_BRACKET_CLOSE" },
    { TokenType::TOK_OP_ARR_BRACKET_OPEN, "TOK_OP_ARR_BRACKET_OPEN" },
    { TokenType::TOK_ARR_BRACKET_CLOSE, "TOK_ARR_BRACKET_CLOSE" },
    { TokenType::TOK_OP_TEMPL_BRACKET_OPEN, "TOK_OP_TEMPL_BRACKET_OPEN" },
    { TokenType::TOK_TEMPL_BRACKET_CLOSE, "TOK_TEMPL_BRACKET_CLOSE" },

    { TokenType::TOK_KW_FN, "TOK_KW_FN" },
    { TokenType::TOK_KW_LAMBDA, "TOK_KW_LAMBDA" },
    { TokenType::TOK_KW_MOD, "TOK_KW_MOD" },
    { TokenType::TOK_KW_CLASS, "TOK_KW_CLASS" },
    { TokenType::TOK_KW_ENUM, "TOK_KW_ENUM" },
    { TokenType::TOK_KW_TRAIT, "TOK_KW_TRAIT" },
    { TokenType::TOK_KW_TYPE, "TOK_KW_TYPE" },
    { TokenType::TOK_KW_RET, "TOK_KW_RET" },
    { TokenType::TOK_KW_USE, "TOK_KW_USE" },
    { TokenType::TOK_KW_IF, "TOK_KW_IF" },
    { TokenType::TOK_KW_ENSURE, "TOK_KW_ENSURE" },
    { TokenType::TOK_KW_ELSE, "TOK_KW_ELSE" },
    { TokenType::TOK_KW_MATCH, "TOK_KW_MATCH" },
    { TokenType::TOK_KW_SWITCH, "TOK_KW_SWITCH" },
    { TokenType::TOK_KW_FOR, "TOK_KW_FOR" },
    { TokenType::TOK_KW_DO, "TOK_KW_DO" },
    { TokenType::TOK_KW_CTN, "TOK_KW_CTN" },
    { TokenType::TOK_KW_BRK, "TOK_KW_BRK" },
    { TokenType::TOK_KW_INC, "TOK_KW_INC" },
    { TokenType::TOK_KW_IMPORT, "TOK_KW_IMPORT" },
    { TokenType::TOK_KW_TRUE, "TOK_KW_TRUE" },
    { TokenType::TOK_KW_FALSE, "TOK_KW_FALSE" },

    { TokenType::TOK_STMT, "TOK_STMT" },
    { TokenType::TOK_IMPL, "TOK_IMPL" },

    { TokenType::TOK_OP_COMMA, "TOK_OP_COMMA" },
    { TokenType::TOK_OP_ASG_DCL, "TOK_OP_ASG_DCL" },
    { TokenType::TOK_OP_ASG_AND, "TOK_OP_ASG_AND" },
    { TokenType::TOK_OP_ASG_XOR, "TOK_OP_ASG_XOR" },
    { TokenType::TOK_OP_ASG_OR, "TOK_OP_ASG_OR" },
    { TokenType::TOK_OP_ASG_LSH, "TOK_OP_ASG_LSH" },
    { TokenType::TOK_OP_ASG_RSH, "TOK_OP_ASG_RSH" },
    { TokenType::TOK_OP_ASG_MOD, "TOK_OP_ASG_MOD" },
    { TokenType::TOK_OP_ASG_DIV, "TOK_OP_ASG_DIV" },
    { TokenType::TOK_OP_ASG_MUL, "TOK_OP_ASG_MUL" },
    { TokenType::TOK_OP_ASG_SUB, "TOK_OP_ASG_SUB" },
    { TokenType::TOK_OP_ASG_ADD, "TOK_OP_ASG_ADD" },
    { TokenType::TOK_OP_ASG, "TOK_OP_ASG" },
    { TokenType::TOK_OP_NULL, "TOK_OP_NULL" },
    { TokenType::TOK_OP_LAND, "TOK_OP_LAND" },
    { TokenType::TOK_OP_LOR, "TOK_OP_LOR" },
    { TokenType::TOK_OP_ARG, "TOK_OP_ARG" },
    { TokenType::TOK_OP_NONE, "TOK_OP_NONE" },
    { TokenType::TOK_OP_BOR, "TOK_OP_BOR" },
    { TokenType::TOK_OP_BXOR, "TOK_OP_BXOR" },
    { TokenType::TOK_OP_BAND, "TOK_OP_BAND" },
    { TokenType::TOK_OP_EQ, "TOK_OP_EQ" },
    { TokenType::TOK_OP_NQ, "TOK_OP_NQ" },
    { TokenType::TOK_OP_LT, "TOK_OP_LT" },
    { TokenType::TOK_OP_LEQ, "TOK_OP_LEQ" },
    { TokenType::TOK_OP_GT, "TOK_OP_GT" },
    { TokenType::TOK_OP_GEQ, "TOK_OP_GEQ" },
    { TokenType::TOK_OP_LSH, "TOK_OP_LSH" },
    { TokenType::TOK_OP_RSH, "TOK_OP_RSH" },
    { TokenType::TOK_OP_ADD, "TOK_OP_ADD" },
    { TokenType::TOK_OP_SUB, "TOK_OP_SUB" },
    { TokenType::TOK_OP_MOD, "TOK_OP_MOD" },
    { TokenType::TOK_OP_MUL, "TOK_OP_MUL" },
    { TokenType::TOK_OP_DIV, "TOK_OP_DIV" },
    { TokenType::TOK_OP_DCL, "TOK_OP_DCL" },
    { TokenType::TOK_OP_INC, "TOK_OP_INC" },
    { TokenType::TOK_OP_DEC, "TOK_OP_DEC" },
    { TokenType::TOK_OP_POS, "TOK_OP_POS" },
    { TokenType::TOK_OP_NEG, "TOK_OP_NEG" },
    { TokenType::TOK_OP_LN, "TOK_OP_LN" },
    { TokenType::TOK_OP_BN, "TOK_OP_BN" },
    { TokenType::TOK_KW_SAFE, "TOK_KW_SAFE" },
    { TokenType::TOK_OP_DEREF, "TOK_OP_DEREF" },
    { TokenType::TOK_OP_MUT, "TOK_OP_MUT" },
    { TokenType::TOK_OP_MEM, "TOK_OP_MEM" },
    { TokenType::TOK_OP_DMEM, "TOK_OP_DMEM" },
    { TokenType::TOK_ANY, "TOK_ANY" },
  };

  const std::unordered_map<TokenType, const OperatorInfoTuple> OPERATORS_INFO {
    { TokenType::TOK_OP_COMMA,   OperatorInfoTuple( 1, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_ASG_DCL, OperatorInfoTuple( 2, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_AND, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_XOR, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_OR,  OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_LSH, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_RSH, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_MOD, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_DIV, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_MUL, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_SUB, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG_ADD, OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_ASG,     OperatorInfoTuple( 3, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_NULL,    OperatorInfoTuple( 4, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_LOR,     OperatorInfoTuple( 5, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_LAND,    OperatorInfoTuple( 6, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_ARG,     OperatorInfoTuple( 7, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_NONE,    OperatorInfoTuple( 7, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_BOR,     OperatorInfoTuple( 8, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_BXOR,    OperatorInfoTuple( 9, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_BAND,    OperatorInfoTuple(10, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_EQ,      OperatorInfoTuple(11, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_NQ,      OperatorInfoTuple(11, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_LT,      OperatorInfoTuple(12, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_LEQ,     OperatorInfoTuple(12, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_GT,      OperatorInfoTuple(12, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_GEQ,     OperatorInfoTuple(12, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_LSH,     OperatorInfoTuple(13, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_RSH,     OperatorInfoTuple(13, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_ADD,     OperatorInfoTuple(14, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_SUB,     OperatorInfoTuple(14, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_MOD,     OperatorInfoTuple(14, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_MUL,     OperatorInfoTuple(15, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_DIV,     OperatorInfoTuple(15, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_DCL,     OperatorInfoTuple(16, OperatorType::BINARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_INC,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_DEC,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_POS,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_NEG,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_LN,      OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_BN,      OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_DEREF,   OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_MUT,     OperatorInfoTuple(17, OperatorType::UNARY, Associativity::RIGHT) },
    { TokenType::TOK_OP_BRACKET_OPEN, OperatorInfoTuple(18,       OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_ARR_BRACKET_OPEN, OperatorInfoTuple(18,   OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_TEMPL_BRACKET_OPEN, OperatorInfoTuple(18, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_MEM,     OperatorInfoTuple(18, OperatorType::BINARY, Associativity::LEFT) },
    { TokenType::TOK_OP_DMEM,    OperatorInfoTuple(18, OperatorType::BINARY, Associativity::LEFT) },
  };

  const std::unordered_map<TokenType, TokenType> TOKEN_BIOP_TO_UNOP {
    { TokenType::TOK_OP_ADD, TokenType::TOK_OP_POS },
    { TokenType::TOK_OP_SUB, TokenType::TOK_OP_NEG },
    { TokenType::TOK_OP_MUL, TokenType::TOK_OP_DEREF },
    { TokenType::TOK_OP_BAND, TokenType::TOK_OP_MUT },
  };


  const std::unordered_map<TokenType /* open bracket */,
    TokenType /* closing bracket */> TOKEN_BRACKETS = {
      { TokenType::TOK_OP_BRACKET_OPEN, TokenType::TOK_BRACKET_CLOSE },
      { TokenType::TOK_OP_ARR_BRACKET_OPEN, TokenType::TOK_ARR_BRACKET_CLOSE },
      { TokenType::TOK_OP_TEMPL_BRACKET_OPEN, TokenType::TOK_TEMPL_BRACKET_CLOSE },
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
  case TokenType::TOK_INT8:
    return std::to_string(i8()) + 's';
  case TokenType::TOK_INT16:
    return std::to_string(i16()) + 'S';
  case TokenType::TOK_INT32:
    return std::to_string(i32());
  case TokenType::TOK_INT64:
    return std::to_string(i64()) + 'L';
  case TokenType::TOK_UINT8:
    return std::to_string(u8()) + "us";
  case TokenType::TOK_UINT16:
    return std::to_string(u16()) + "uS";
  case TokenType::TOK_UINT32:
    return std::to_string(u32()) + 'u';
  case TokenType::TOK_UINT64:
    return std::to_string(u64()) + "uL";
  case TokenType::TOK_FLT32:
    return std::to_string(f32()) + 'f';
  case TokenType::TOK_FLT64:
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
