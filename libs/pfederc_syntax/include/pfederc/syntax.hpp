#ifndef PFEDERC_SYNTAX_SYNTAX
#define PFEDERC_SYNTAX_SYNTAX

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/lexer.hpp"
#include "pfederc/expr.hpp"

namespace pfederc {
  ProgramExpr* parseProgram(Lexer &lex) noexcept;
}

#endif /* PFEDERC_SYNTAX_SYNTAX */
