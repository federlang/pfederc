#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<TemplateDecl> Parser::fromDeclExprToTemplateDecl(BiOpExpr &bioprhs) noexcept {
  if (!isTokenExpr(bioprhs.getLeft(), TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_INVALID_VARDECL_ID, bioprhs.getPosition()));
    return nullptr;
  }
  
  return std::make_unique<TemplateDecl>(
    &(dynamic_cast<const TokenExpr&>(bioprhs.getLeft()).getToken()),
    bioprhs.getRightPtr());
}

std::unique_ptr<TemplateDecls> Parser::parseTemplateDecl() noexcept {
  sanityExpect(TOK_OP_TEMPL_BRACKET_OPEN);

  std::unique_ptr<Expr> expr(parseExpression());
  std::unique_ptr<TemplateDecls> result(new TemplateDecls());

  while (isBiOpExpr(*expr, TOK_OP_COMMA)) {
    BiOpExpr &biopexpr = dynamic_cast<BiOpExpr&>(*expr);
    std::unique_ptr<Expr> rhs(biopexpr.getRightPtr());
    if (!isBiOpExpr(*rhs, TOK_OP_DCL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_VARDECL, rhs->getPosition()));
      return nullptr;
    }

    BiOpExpr &bioprhs = dynamic_cast<BiOpExpr&>(*rhs);
    std::unique_ptr<TemplateDecl> templdecl(fromDeclExprToTemplateDecl(bioprhs));
    if (!templdecl)
      return nullptr;
    // success, push on result
    result->push_back(std::move(templdecl));
    // next iterator step
    expr = biopexpr.getLeftPtr();
  }

  if (!isBiOpExpr(*expr, TOK_OP_DCL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_VARDECL, expr->getPosition()));
    return nullptr;
  }
  
  BiOpExpr &bioprhs = dynamic_cast<BiOpExpr&>(*expr);
  std::unique_ptr<TemplateDecl> templdecl(fromDeclExprToTemplateDecl(bioprhs));
  if (!templdecl)
    return nullptr;
  // success, push on result
  result->push_back(std::move(templdecl));

  return result;
}

std::unique_ptr<Expr> Parser::parseTemplate() noexcept {
  sanityExpect(TOK_OP_TEMPL_BRACKET_OPEN);

  // TODO
}
