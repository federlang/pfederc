#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseClass(std::unique_ptr<Capabilities> &&caps) noexcept {
  sanityExpect(TOK_KW_CLASS);
  // maybe it's a class trait
  if (*lexer.getCurrentToken() == TOK_KW_TRAIT)
    return parseClassTrait(std::move(caps));

  bool err = false;

  // class templ?
  std::unique_ptr<TemplateDecls> templ;
  if (*lexer.getCurrentToken() == TOK_OP_TEMPL_BRACKET_OPEN) {
    templ = parseTemplateDecl();
    err = true;
  }

  // class templ? id
  const Token *const tokId = lexer.getCurrentToken();
  if (!expect(TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_ID, tokId->getPosition()));
    err = true;
  }

  std::list<std::unique_ptr<BiOpExpr>> constructAttributes;
  // class templ? id constructor?
  if (*lexer.getCurrentToken() == TOK_OP_BRACKET_OPEN) {
    parseClassConstructor(err, constructAttributes);
  }
  // class templ? id constructor? newline
  if (!expect(TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
  }

  std::list<std::unique_ptr<BiOpExpr>> attrs;
  std::list<std::unique_ptr<FuncExpr>> functions;
	parseClassBody(tokId, err, attrs, functions);

  return std::make_unique<ClassExpr>(lexer, tokId->getPosition(), std::move(caps),
    tokId, std::move(templ), std::move(constructAttributes),
    std::move(attrs), std::move(functions));
}

void Parser::parseClassConstructor(bool &err,
    std::list<std::unique_ptr<BiOpExpr>> &constructAttributes) noexcept {
  const Token *const constructStart = lexer.getCurrentToken();
  lexer.next();

  std::unique_ptr<Expr> expr(parseExpression());
  while (isBiOpExpr(*expr, TOK_OP_COMMA)) {
    BiOpExpr &biopexpr = dynamic_cast<BiOpExpr&>(*expr);
    if (!isBiOpExpr(biopexpr.getRight(), TOK_OP_DCL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_VARDECL, biopexpr.getRight().getPosition()));
      // advance to next left expression
      expr = biopexpr.getLeftPtr();
      continue;
    }

    constructAttributes.insert(constructAttributes.begin(),
      std::unique_ptr<BiOpExpr>(
        dynamic_cast<BiOpExpr*>(biopexpr.getRightPtr().release())));
    // advance to next left expression
    expr = biopexpr.getLeftPtr();
  }

  if (!isBiOpExpr(*expr, TOK_OP_DCL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_VARDECL, expr->getPosition()));
    // soft error
  } else {
    constructAttributes.insert(constructAttributes.begin(),
      std::unique_ptr<BiOpExpr>(
        dynamic_cast<BiOpExpr*>(expr.release())));
  }

  if (!expect(TOK_BRACKET_CLOSE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_CLOSING_BRACKET,
      lexer.getCurrentToken()->getPosition(),
      std::vector<Position>{ constructStart->getPosition() }));
  }
}

void Parser::parseClassBody(const Token *const tokId, bool &err,
    std::list<std::unique_ptr<BiOpExpr>> &attributes,
    std::list<std::unique_ptr<FuncExpr>> &functions) noexcept {

  // classbody
  while (*lexer.getCurrentToken() != TOK_STMT
      && *lexer.getCurrentToken() != TOK_EOF) {
    if (*lexer.getCurrentToken() == TOK_EOL) {
      lexer.next();
      continue;
    }

    std::unique_ptr<Expr> expr(parseExpression());
    if (expr && expr->getType() == EXPR_FUNC) {
      functions.push_back(
        std::unique_ptr<FuncExpr>(
          dynamic_cast<FuncExpr*>(expr.release())));
    } else if (expr && isBiOpExpr(*expr, TOK_OP_DCL)) {
      attributes.push_back(
        std::unique_ptr<BiOpExpr>(
          dynamic_cast<BiOpExpr*>(expr.release())));
    } else if (expr) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_CLASS_SCOPE, expr->getPosition()));
      // soft error
    }

    if (!expect(TOK_EOL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
      // soft error
      skipToStmtEol();
    }
  }
  // end of class body
  if (!expect(TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition(),
      std::vector<Position> { tokId->getPosition() }));
    err = true;
  }
}
