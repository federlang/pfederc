#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseClassTrait(std::unique_ptr<Capabilities> &&caps) noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  sanityExpect(TOK_KW_TRAIT);

  // hard errors
  bool err = false;

  std::unique_ptr<TemplateDecls> templ;
  if (*lexer.getCurrentToken() == TOK_OP_TEMPL_BRACKET_OPEN) {
    templ = parseTemplateDecl();
  }

  const Token *const tokId = lexer.getCurrentToken();
  if (!expect(TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_ID, tokId->getPosition()));
    err = true;
  }

  if (!expect(TOK_OP_DCL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_ID, lexer.getCurrentToken()->getPosition()));
  }

  std::unique_ptr<Expr> impltrait(parseExpression());
  if (!impltrait)
    err = true;
  else if (isBiOpExpr(*impltrait, TOK_OP_COMMA)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_TRAITCLASS_IMPL, impltrait->getPosition()));
  }

  if (!expect(TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
  }

  std::list<std::unique_ptr<FuncExpr>> functions;
  parseClassTraitBody(err, functions);

  if (!expect(TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition()));
  }

  if (err)
    return nullptr;

  return std::make_unique<TraitImplExpr>(lexer,
      pos + tokId->getPosition(), std::move(caps),
      tokId, std::move(templ), std::move(impltrait), std::move(functions));
}

void Parser::parseClassTraitBody(bool &err,
    std::list<std::unique_ptr<FuncExpr>> &functions) noexcept {
  while (*lexer.getCurrentToken() != TOK_STMT
      && *lexer.getCurrentToken() != TOK_EOF) {
    if (*lexer.getCurrentToken() == TOK_EOL) {
      lexer.next();
      continue;
    }

    std::unique_ptr<Expr> expr(parseExpression());
    if (expr && expr->getType() == EXPR_FUNC) {
      std::unique_ptr<FuncExpr> funcExpr(
            dynamic_cast<FuncExpr*>(expr.release()));

      /*if (funcExpr->getTemplates()) {
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
              STX_ERR_TRAIT_SCOPE_FUNC_TEMPL,
              std::get<0>(funcExpr->getTemplates()->front())->getPosition()
              + std::get<1>(funcExpr->getTemplates()->back())->getPosition()));
      }*/

      if (!funcExpr->getBody()) {
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
              STX_ERR_TRAITCLASS_SCOPE_FUNC_BODY,
              funcExpr->getPosition()));
      }

      functions.push_back(std::move(funcExpr));
    } else if (expr) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR, 
            STX_ERR_TRAIT_SCOPE, expr->getPosition()));
      // soft error
    }

    if (!expect(TOK_EOL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
      // soft error
      skipToStmtEol();
    }
  }
}
