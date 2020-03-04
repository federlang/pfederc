#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<ProgramExpr> Parser::parseProgram() noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  ModBody body = parseModBody(true);
  return std::make_unique<ProgramExpr>(lexer, pos,
      std::get<0>(body),
      std::move(std::get<1>(body)), std::move(std::get<2>(body)));
}
