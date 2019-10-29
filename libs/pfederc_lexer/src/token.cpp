#include "pfederc/token.hpp"
using namespace pfederc;

// Position

Position Position::operator +(const Position &pos) const noexcept {
  return Position{std::min(line, pos.line),
    std::min(startIndex, pos.startIndex),
    std::max(endIndex, pos.endIndex)};
}

// global

std::string pfederc::tokenTypeToString(TokenType type) noexcept {
  switch (type) {
  case TOK_ID:         return "TOK_ID";
  case TOK_INT8:       return "TOK_INT8";
  case TOK_INT16:      return "TOK_INT16";
  case TOK_INT32:      return "TOK_INT32";
  case TOK_INT64:      return "TOK_INT64";
  case TOK_UINT8:      return "TOK_UINT8";
  case TOK_UINT16:     return "TOK_UINT16";
  case TOK_UINT32:     return "TOK_UINT32";
  case TOK_UINT64:     return "TOK_UINT64";
  case TOK_FLT32:      return "TOK_FLT32";
  case TOK_FLT64:      return "TOK_FLT64";
  case TOK_CHAR:       return "TOK_CHAR";
  case TOK_STR:        return "TOK_STR";

  case TOK_KW_FN:      return "TOK_KW_FN";
  case TOK_KW_MOD:     return "TOK_KW_MOD";
  case TOK_KW_CLASS:   return "TOK_KW_CLASS";
  case TOK_KW_ENUM:    return "TOK_KW_ENUM";
  case TOK_KW_TRAIT:   return "TOK_KW_TRAIT";
  case TOK_KW_TYPE:    return "TOK_KW_TYPE";
  case TOK_KW_RET:     return "TOK_KW_RET";
  case TOK_KW_USE:     return "TOK_KW_USE";
  case TOK_KW_IF:      return "TOK_KW_IF";
  case TOK_KW_ELSE:    return "TOK_KW_ELSE";
  case TOK_KW_MATCH:   return "TOK_KW_MATCH";
  case TOK_KW_SWITCH:  return "TOK_KW_SWITCH";
  case TOK_KW_FOR:     return "TOK_KW_FOR";
  case TOK_KW_DO:      return "TOK_KW_DO";
  case TOK_KW_CTN:     return "TOK_KW_CTN";
  case TOK_KW_BRK:     return "TOK_KW_BRK";
  case TOK_KW_INC:     return "TOK_KW_INC";
  case TOK_KW_IMP:     return "TOK_KW_IMP";
  case TOK_KW_TRUE:    return "TOK_KW_TRUE";
  case TOK_KW_FALSE:   return "TOK_KW_FALSE";

  case TOK_OP_COMMA:   return "TOK_OP_COMMA";
  case TOK_OP_ASG_DCL: return "TOK_OP_ASG_DCL";
  case TOK_OP_ASG_AND: return "TOK_OP_ASG_AND";
  case TOK_OP_ASG_XOR: return "TOK_OP_ASG_XOR";
  case TOK_OP_ASG_OR:  return "TOK_OP_ASG_OR";
  case TOK_OP_ASG_LSH: return "TOK_OP_ASG_LSH";
  case TOK_OP_ASG_RSH: return "TOK_OP_ASG_RSH";
  case TOK_OP_ASG_MOD: return "TOK_OP_ASG_MOD";
  case TOK_OP_ASG_DIV: return "TOK_OP_ASG_DIV";
  case TOK_OP_ASG_MUL: return "TOK_OP_ASG_MUL";
  case TOK_OP_ASG_SUB: return "TOK_OP_ASG_SUB";
  case TOK_OP_ASG_ADD: return "TOK_OP_ASG_ADD";
  case TOK_OP_ASG:     return "TOK_OP_ASG";
  case TOK_OP_NULL:    return "TOK_OP_NULL";
  case TOK_OP_LAND:    return "TOK_OP_LAND";
  case TOK_OP_LOR:     return "TOK_OP_LOR";
  case TOK_OP_ARG:     return "TOK_OP_ARG";
  case TOK_OP_BOR:     return "TOK_OP_BOR";
  case TOK_OP_BXOR:    return "TOK_OP_BXOR";
  case TOK_OP_BAND:    return "TOK_OP_BAND";
  case TOK_OP_EQ:      return "TOK_OP_EQ";
  case TOK_OP_NQ:      return "TOK_OP_NQ";
  case TOK_OP_LT:      return "TOK_OP_LT";
  case TOK_OP_LEQ:     return "TOK_OP_LEQ";
  case TOK_OP_GT:      return "TOK_OP_GT";
  case TOK_OP_GEQ:     return "TOK_OP_GEQ";
  case TOK_OP_LSH:     return "TOK_OP_LSH";
  case TOK_OP_RSH:     return "TOK_OP_RSH";
  case TOK_OP_ADD:     return "TOK_OP_ADD";
  case TOK_OP_SUB:     return "TOK_OP_SUB";
  case TOK_OP_MOD:     return "TOK_OP_MOD";
  case TOK_OP_MUL:     return "TOK_OP_MUL";
  case TOK_OP_DIV:     return "TOK_OP_DIV";
  case TOK_OP_DCL:     return "TOK_OP_DCL";
  case TOK_OP_INC:     return "TOK_OP_INC";
  case TOK_OP_DEC:     return "TOK_OP_DEC";
  case TOK_OP_POS:     return "TOK_OP_POS";
  case TOK_OP_NEG:     return "TOK_OP_NEG";
  case TOK_OP_LN:      return "TOK_OP_LN";
  case TOK_OP_BN:      return "TOK_OP_BN";
  case TOK_OP_SF:      return "TOK_OP_SF";
  case TOK_OP_DEREF:   return "TOK_OP_DEREF";
  default:
    fatal(__FILE__, __LINE__, "Invalid branch");
    return "";
  }
}

// Token
Token::Token(TokenType type, const Position &pos) noexcept
    : type{type}, pos(pos) {
}

Token::~Token() {
}

TokenType Token::getType() const noexcept {
  return type;
}

const Position &Token::getPosition() const noexcept {
  return pos;
}

// NumberToken
NumberToken::NumberToken(TokenType type,
    const Position &pos, uint64_t num) noexcept
    : Token(type, pos), num{num} {
}

NumberToken::NumberToken(TokenType type,
    const Position &pos, int64_t num) noexcept
    : Token(type, pos), num{reinterpret_cast<const uint64_t&>(num)} {
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
