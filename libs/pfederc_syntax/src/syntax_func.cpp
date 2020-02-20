#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<FuncParameter> Parser::fromExprDeclToFunctionParam(
    std::unique_ptr<Expr> &&expr, std::unique_ptr<Expr> &&guard,
    std::unique_ptr<Expr> &&guardResult) noexcept {
  // '&'id ':' expr | id ':' expr | '&' expr | expr
  if (isBiOpExpr(*expr, TOK_OP_DCL)) {
    // '&'id ':' expr | id ':' expr
    BiOpExpr &biopdcl = dynamic_cast<BiOpExpr&>(*expr);

    std::unique_ptr<Expr> idexpr;
    bool ismutable;
    if (isUnOpExpr(biopdcl.getLeft(), TOK_OP_BAND)) {
      std::unique_ptr<UnOpExpr> unopexpr =
          std::unique_ptr<UnOpExpr>(dynamic_cast<UnOpExpr*>(biopdcl.getLeftPtr().release()));
      ismutable = true;
      idexpr = unopexpr->getExpressionPtr();
    } else {
      idexpr = biopdcl.getLeftPtr();
      ismutable = false;
    }

    if (!isTokenExpr(*idexpr, TOK_ID)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_INVALID_VARDECL_ID, idexpr->getPosition()));
      return nullptr;
    }

    if (isUnOpExpr(biopdcl.getRight(), TOK_OP_BAND)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_INVALID_VARDECL, biopdcl.getRight().getPosition()));
      return nullptr;
    }

    return std::make_unique<FuncParameter>(
      &dynamic_cast<const TokenExpr&>(*idexpr).getToken(),
      ismutable,
      biopdcl.getRightPtr(),
      std::move(guard), std::move(guardResult));
  }
  // '&' expr | expr
  std::unique_ptr<Expr> typeexpr;
  bool ismutable;
  if (isUnOpExpr(*expr, TOK_OP_MUT)) {
    std::unique_ptr<UnOpExpr> unopexpr =
      std::unique_ptr<UnOpExpr>(dynamic_cast<UnOpExpr*>(expr.release()));
    typeexpr = unopexpr->getExpressionPtr();
    ismutable = true;
  } else {
    typeexpr = std::move(expr);
    ismutable = false;
  }

  return std::make_unique<FuncParameter>(
    nullptr, ismutable, std::move(typeexpr),
    std::move(guard), std::move(guardResult));
}

std::unique_ptr<FuncParameter> Parser::fromExprGuardToFunctionParam(
    std::unique_ptr<Expr> &&expr, std::unique_ptr<Expr> &&guardResult) noexcept {
  BiOpExpr& biopexpr = dynamic_cast<BiOpExpr&>(*expr);
  return fromExprDeclToFunctionParam(biopexpr.getLeftPtr(),
    biopexpr.getRightPtr(), std::move(guardResult));
}

std::unique_ptr<FuncParameter> Parser::fromExprToFunctionParam(
    std::unique_ptr<Expr> &&expr) noexcept {
  if (isBiOpExpr(*expr, TOK_OP_ASG)) {
    BiOpExpr &biopexpr = dynamic_cast<BiOpExpr&>(*expr);
    // guard result
    if (!isBiOpExpr(biopexpr.getLeft(), TOK_OP_BOR)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_GUARD, biopexpr.getLeft().getPosition()));
      return nullptr;
    }

    return fromExprGuardToFunctionParam(
      biopexpr.getLeftPtr(), biopexpr.getRightPtr());
  } else if (isBiOpExpr(*expr, TOK_OP_BOR)) {
    // guard (requires)
    return fromExprGuardToFunctionParam(std::move(expr), nullptr);
  }

  return fromExprDeclToFunctionParam(std::move(expr), nullptr, nullptr);
}

std::vector<std::unique_ptr<FuncParameter>> Parser::parseFuncParameters() noexcept {
  sanityExpect(TOK_OP_BRACKET_OPEN);

  if (*lexer.getCurrentToken() == TOK_BRACKET_CLOSE) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_PARAMETERS, lexer.getCurrentToken()->getPosition()));
    return std::vector<std::unique_ptr<FuncParameter>>();
  }

  bool err = false;
  std::vector<std::unique_ptr<FuncParameter>> parameters;

  std::unique_ptr<Expr> expr(parseExpression());
  while (isBiOpExpr(*expr, TOK_OP_COMMA)) {
    BiOpExpr &biopexpr = dynamic_cast<BiOpExpr&>(*expr);
    auto funcparam = fromExprToFunctionParam(biopexpr.getRightPtr());
    if (!funcparam) {
      err = true;
      break;
    }

    parameters.insert(parameters.begin(), std::move(funcparam));
    // reduce
    expr = biopexpr.getLeftPtr();
  }

  auto funcparam = fromExprToFunctionParam(std::move(expr));
  if (!funcparam)
    err = true;
  else
    parameters.insert(parameters.begin(), std::move(funcparam));

  if (!expect(TOK_BRACKET_CLOSE)) {
    err = true;
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_CLOSING_BRACKET,
      lexer.getCurrentToken()->getPosition()));
  }

  if (err)
    return std::vector<std::unique_ptr<FuncParameter>>();

  return parameters;
}

std::unique_ptr<Expr> Parser::parseFuncType() noexcept {
  const Token *tokBegin = lexer.getCurrentToken();
  sanityExpect(TOK_KW_TYPE);

  bool err = false;

  std::vector<std::unique_ptr<FuncParameter>> parameters;
  if (*lexer.getCurrentToken() == TOK_OP_BRACKET_OPEN) {
    parameters = parseFuncParameters();
    if (parameters.empty())
      err = true;
  }

  std::unique_ptr<Expr> returnExpr;
  if (*lexer.getCurrentToken() == TOK_OP_DCL) {
    lexer.next();
    returnExpr = parseExpression(16);
    if (!returnExpr)
      return nullptr;
  }

  return std::make_unique<FuncTypeExpr>(lexer,
    tokBegin->getPosition(), std::move(parameters), std::move(returnExpr));
}

std::unique_ptr<Expr> Parser::parseFuncReturnType() noexcept {
  std::unique_ptr<Expr> returnExpr = parseExpression();
  if (!returnExpr)
    return nullptr;

  return returnExpr;
}

std::unique_ptr<Expr> Parser::parseFunction(std::unique_ptr<Capabilities> &&caps) noexcept {
  sanityExpect(TOK_KW_FN);

  bool err = false;

  std::unique_ptr<TemplateDecls> templ;
  if (*lexer.getCurrentToken() == TOK_OP_TEMPL_BRACKET_OPEN) {
    templ = parseTemplateDecl();
    if (!templ)
      err = true;
    else if (!expect(TOK_TEMPL_BRACKET_CLOSE) && !err) {
      err = true;
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_TEMPL_CLOSING_BRACKET,
        lexer.getCurrentToken()->getPosition()));
    }
  }

  if (*lexer.getCurrentToken() == TOK_KW_TYPE) {
    if (!templ->empty()) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_FUNC_VAR_NO_TEMPL, std::get<1>(templ->at(0))->getPosition()));
      err = true;
    }

    auto result = parseFuncType();
    if (err)
      return nullptr;

    return result;
  }

  // function decl./def.
  const Token *tok = lexer.getCurrentToken();
  if (!expect(TOK_ID) && !err) {
    err = true;
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_FUNCTION_ID, lexer.getCurrentToken()->getPosition()));
  }

  std::vector<std::unique_ptr<FuncParameter>> parameters;
  if (*lexer.getCurrentToken() == TOK_OP_BRACKET_OPEN) {
    parameters = parseFuncParameters();
    if (parameters.empty())
      err = true;
  }
  // assign to expression
  if (*lexer.getCurrentToken() == TOK_OP_ASG) {
    lexer.next(); // eat =

    std::unique_ptr<Expr> returnExprPos(parseExpression());
    if (!returnExprPos)
      err = true;

    if (*lexer.getCurrentToken() != TOK_EOF
        && *lexer.getCurrentToken() != TOK_EOL) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
      err = true;
    }

    lexer.next();

    if (err)
      return nullptr;

    return std::make_unique<FuncExpr>(lexer, tok->getPosition(), std::move(caps),
      tok, std::move(templ), std::move(parameters),
      nullptr,
      std::make_unique<BodyExpr>(lexer, returnExprPos->getPosition(),
      Exprs(), std::move(returnExprPos)), true);
  }

  std::unique_ptr<Expr> returnExpr;
  bool autoDetect = false;
  if (*lexer.getCurrentToken() == TOK_OP_DCL) {
    lexer.next();
    if (*lexer.getCurrentToken() == TOK_EOL) {
      autoDetect = true;
    } else {
      // return type
      returnExpr = parseFuncReturnType();
      if (!returnExpr)
        err = true;
    }
  }
  // declaration
  if (!autoDetect && *lexer.getCurrentToken() == TOK_STMT) {
    lexer.next(); // eat ;
    if (err)
      return nullptr;

    // function declaration
    return std::make_unique<FuncExpr>(lexer, tok->getPosition(),
      std::move(caps),
      tok, std::move(templ), std::move(parameters),
      std::move(returnExpr), nullptr, false);
  }

  // body
  if (!expect(TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_FN_DCL_DEF, lexer.getCurrentToken()->getPosition()));
    return nullptr;
  }

  std::unique_ptr<BodyExpr> body(parseFunctionBody());
  if (!body) {
    if (!expect(TOK_STMT)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition()));
      return nullptr;
    }

    return nullptr;
  }

  if (!expect(TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition()));
    return nullptr;
  } 

  if (err)
    return nullptr;

  return std::make_unique<FuncExpr>(lexer, tok->getPosition(), std::move(caps),
    tok, std::move(templ), std::move(parameters),
    std::move(returnExpr), std::move(body), autoDetect);
}

std::unique_ptr<BodyExpr> Parser::parseFunctionBody() noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  bool err = false;
  Exprs exprs;
  while (*lexer.getCurrentToken() != TOK_KW_RET
      && *lexer.getCurrentToken() != TOK_STMT
      && *lexer.getCurrentToken() != TOK_EOF
      && *lexer.getCurrentToken() != TOK_KW_ELSE) {
    if (*lexer.getCurrentToken() == TOK_EOL) {
      lexer.next();
      continue;
    }

    std::unique_ptr<Expr> expr(parseExpression());
    if (!expect(TOK_EOL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
      skipToStmtEol();
      err = true;
    }

    if (!expr)
      err = true;
    else {
      pos = pos + expr->getPosition();
      exprs.push_back(std::move(expr));
    }
  }

  std::unique_ptr<Expr> returnExpr;
  if (*lexer.getCurrentToken() == TOK_KW_RET) {
    pos = pos + lexer.getCurrentToken()->getPosition();
    lexer.next(); // eat return
    returnExpr = parseExpression();
    if (returnExpr)
      pos = pos + returnExpr->getPosition();
    else
      err = true;
  }

  skipEol();

  if (err)
    return nullptr;

  return std::make_unique<BodyExpr>(lexer,
    pos, std::move(exprs), std::move(returnExpr));
}
