#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<TemplateDecl> Parser::fromDeclExprToTemplateDecl(BiOpExpr &bioprhs) noexcept {
  if (!isTokenExpr(bioprhs.getLeft(), TokenType::TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_INVALID_VARDECL_ID, bioprhs.getPosition()));
    return nullptr;
  }
  
  return std::make_unique<TemplateDecl>(
    &(dynamic_cast<const TokenExpr&>(bioprhs.getLeft()).getToken()),
    bioprhs.getRightPtr());
}

std::unique_ptr<TemplateDecls> Parser::parseTemplateDecl() noexcept {
  sanityExpect(TokenType::TOK_OP_TEMPL_BRACKET_OPEN);

  bool err = false;

  std::unique_ptr<Expr> expr(parseExpression());
  std::unique_ptr<TemplateDecls> result(new TemplateDecls());

  while (expr && isBiOpExpr(*expr, TokenType::TOK_OP_COMMA)) {
    BiOpExpr &biopexpr = dynamic_cast<BiOpExpr&>(*expr);
    std::unique_ptr<Expr> rhs(biopexpr.getRightPtr());
    if (!isBiOpExpr(*rhs, TokenType::TOK_OP_DCL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        SyntaxErrorCode::STX_ERR_EXPECTED_VARDECL, rhs->getPosition()));

      // next iterator step
      expr = biopexpr.getLeftPtr();

      continue;
    }

    BiOpExpr &bioprhs = dynamic_cast<BiOpExpr&>(*rhs);
    std::unique_ptr<TemplateDecl> templdecl(fromDeclExprToTemplateDecl(bioprhs));
    if (!templdecl)
      err = true;
    else
    // success, push on result
      result->push_back(TemplateDecl(std::get<0>(*templdecl), std::get<1>(*templdecl).release()));
    // next iterator step
    expr = biopexpr.getLeftPtr();
  }

  if (expr && !isBiOpExpr(*expr, TokenType::TOK_OP_DCL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_VARDECL, expr->getPosition()));
    return nullptr;
  }
  
  if (expr) {
    BiOpExpr &bioprhs = dynamic_cast<BiOpExpr&>(*expr);
    std::unique_ptr<TemplateDecl> templdecl(fromDeclExprToTemplateDecl(bioprhs));
    if (!templdecl)
      err = true;
    // success, push on result
    else
      result->push_back(TemplateDecl(std::get<0>(*templdecl), std::get<1>(*templdecl).release()));
  }

  if (!expect(TokenType::TOK_TEMPL_BRACKET_CLOSE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_TEMPL_CLOSING_BRACKET,
          lexer.getCurrentToken()->getPosition()));
    err = true;
  }

  if (err)
    return nullptr;

  return result;
}

std::unique_ptr<Expr> Parser::parseTemplate() noexcept {
  sanityExpect(TokenType::TOK_OP_TEMPL_BRACKET_OPEN);

  // TODO
}
