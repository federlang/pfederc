#include "pfederc/expr.hpp"
using namespace pfederc;

// Expr

Expr::Expr(const Lexer &lexer, ExprType type, const Position &pos) noexcept
    : lexer{lexer}, type{type}, pos(pos) {
}

Expr::~Expr() {
}

const Lexer &Expr::getLexer() const noexcept {
  return lexer;
}

ExprType Expr::getType() const noexcept {
  return type;
}

const Position &Expr::getPosition() const noexcept {
  return pos;
}
