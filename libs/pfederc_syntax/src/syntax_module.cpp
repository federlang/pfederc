#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseModule() noexcept {
  sanityExpect(TOK_KW_MOD);
  
  bool err = false;

  const Token *tokId = lexer.getCurrentToken();
  if (!expect(TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_ID, lexer.getCurrentToken()->getPosition()));
    err = true;
  }

  if (!expect(TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
    // soft error
  }

  ModBody body = parseModBody();
  if (!expect(TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition()));
    // soft error
  }

  if (err)
    return nullptr;
 
  return std::make_unique<ModExpr>(lexer, tokId->getPosition(), tokId,
    std::move(std::get<2>(body)));
}

static const std::vector<TokenType> _TERMINATE_MOD_TOKEN = {
  TOK_STMT, TOK_KW_ELSE,
};

static const std::vector<ExprType> _ALLOWED_EXPR_MOD = {
  EXPR_MOD, EXPR_FUNC, EXPR_CLASS, EXPR_TRAIT, EXPR_TRAITIMPL, EXPR_ENUM,
};

static const std::vector<ExprType> _ALLOWED_EXPR_PROG = {
  EXPR_MOD, EXPR_FUNC, EXPR_CLASS, EXPR_TRAIT, EXPR_TRAITIMPL, EXPR_ENUM,
  EXPR_PROGNAME, EXPR_USE,
};

ModBody Parser::parseModBody(bool isprog) noexcept {
  const Token *progName;
  Exprs imports;
  Exprs defs;

  bool err = false;

  while(*lexer.getCurrentToken() != TOK_EOL) {
    while (*lexer.getCurrentToken() == TOK_EOL)
      lexer.next(); // eat eols
    if (*lexer.getCurrentToken() == TOK_EOF)
      break;

    const Token *tok = lexer.getCurrentToken();

    // terminate module on termination token
    if (!isprog && std::any_of(
        _TERMINATE_MOD_TOKEN.begin(), _TERMINATE_MOD_TOKEN.end(),
        [&tok](TokenType type) { return *tok == type; }))
      break;

    std::unique_ptr<Expr> expr(parseExpression());
    if ((isprog && *lexer.getCurrentToken() != TOK_EOF)
        || *lexer.getCurrentToken() != TOK_EOL) {
      if (isprog)
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_EOF_EOL, lexer.getCurrentToken()->getPosition()));
      else
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
      err = true;

      skipToStmtEol();
    } else if (*lexer.getCurrentToken() == TOK_EOL) {
      lexer.next(); // eat eol
    }

    if (expr) {
      const bool validexpr = (isprog ?
        std::any_of(_ALLOWED_EXPR_PROG.begin(), _ALLOWED_EXPR_PROG.end(),
          [&expr](ExprType type) { return expr->getType() == type; }) :
        std::any_of(_ALLOWED_EXPR_MOD.begin(), _ALLOWED_EXPR_MOD.end(),
          [&expr](ExprType type) { return expr->getType() == type; })) ||
      // valid global assignments
        isBiOpExpr(*expr, TOK_OP_ASG_DCL) ||
        (isBiOpExpr(*expr, TOK_OP_ASG) &&
        isBiOpExpr(dynamic_cast<BiOpExpr&>(*expr).getLeft(), TOK_OP_DCL));

      if (!validexpr) {
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_INVALID_EXPR, expr->getPosition()));
        err = true;
        continue;
      }

      switch (expr->getType()) {
      case EXPR_PROGNAME:
        if (progName) {
          generateError(std::make_unique<SyntaxError>(LVL_ERROR,
            STX_ERR_PROGNAME, expr->getPosition()));
          err = true;
        } else
          progName = &dynamic_cast<ProgNameExpr&>(*expr).getToken();

        break;
      case EXPR_USE:
        imports.push_back(std::move(expr));
        break;
      default:
        defs.push_back(std::move(expr));
        break;
      }
    }
  }

  if (isprog && *lexer.getCurrentToken() != TOK_EOF) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_EOF, lexer.getCurrentToken()->getPosition()));
    err = true;
  }

  return ModBody(progName, std::move(imports), std::move(defs), err);
}


