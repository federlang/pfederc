#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<TemplateDecl> Parser::fromExprToTemplateDecl(std::unique_ptr<Expr> &&rhs) noexcept {
  if (!rhs) {
    return nullptr;
  }

  if (isTokenExpr(*rhs, TokenType::TOK_ID)) {
    return std::make_unique<TemplateDecl>(std::unique_ptr<TokenExpr>(
          dynamic_cast<TokenExpr*>(rhs.release())));
  }

  if (!isBiOpExpr(*rhs, TokenType::TOK_OP_DCL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_VARDECL, rhs->getPosition()));
    return nullptr;
  }

  BiOpExpr& bioprhs = dynamic_cast<BiOpExpr&>(*rhs);

  if (!isTokenExpr(bioprhs.getLeft(), TokenType::TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR, SyntaxErrorCode::STX_ERR_INVALID_VARDECL_ID, bioprhs.getPosition()));
    return nullptr;
  }
  
  return std::make_unique<TemplateDecl>(
    std::unique_ptr<TokenExpr>(dynamic_cast<TokenExpr*>(bioprhs.getLeftPtr().release())),
    bioprhs.getRightPtr());
}

TemplateDecls Parser::parseTemplateDecl() noexcept {
  sanityExpect(TokenType::TOK_OP_TEMPL_BRACKET_OPEN);

  bool err = false;

  std::unique_ptr<Expr> expr(parseExpression());
  TemplateDecls result;

  while (expr && isBiOpExpr(*expr, TokenType::TOK_OP_COMMA)) {
    BiOpExpr &biopexpr = dynamic_cast<BiOpExpr&>(*expr);
    std::unique_ptr<Expr> rhs(biopexpr.getRightPtr());

    std::unique_ptr<TemplateDecl> templdecl(fromExprToTemplateDecl(std::move(rhs)));
    if (!templdecl)
      err = true;
    else
      // success, push on result
      result.insert(result.begin(), std::move(templdecl));

    // next iterator step
    expr = biopexpr.getLeftPtr();
  }
  
  std::unique_ptr<TemplateDecl> templdecl(fromExprToTemplateDecl(std::move(expr)));
  if (!templdecl)
    err = true;
  // success, push on result
  else
    result.insert(result.begin(), std::move(templdecl));

  if (!expect(TokenType::TOK_TEMPL_BRACKET_CLOSE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_TEMPL_CLOSING_BRACKET,
          lexer.getCurrentToken()->getPosition()));
    err = true;
  }

  if (err)
    return TemplateDecls();

  return result;
}

std::unique_ptr<Expr> Parser::parseTemplate() noexcept {
  sanityExpect(TokenType::TOK_OP_TEMPL_BRACKET_OPEN);

  // TODO
}
