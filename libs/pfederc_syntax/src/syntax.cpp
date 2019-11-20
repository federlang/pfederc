#include "pfederc/syntax.hpp"
using namespace pfederc;

namespace pfederc {
  const std::map<TokenType /* opening bracket */,
    SyntaxErrorCode> STX_ERR_BRACKETS = {
      { TOK_OP_BRACKET_OPEN, STX_ERR_EXPECTED_CLOSING_BRACKET },
      { TOK_OP_ARR_BRACKET_OPEN, STX_ERR_EXPECTED_ARR_CLOSING_BRACKET },
      { TOK_OP_TEMPL_BRACKET_OPEN, STX_ERR_EXPECTED_TEMPL_CLOSING_BRACKET },
  };
}

// Parser
Parser::~Parser() {
}

std::unique_ptr<Expr> Parser::parseUnary() noexcept {
  const Token *tok = lexer.getCurrentToken();
  if (*tok == TOK_OP_BRACKET_OPEN)
    return parseBrackets();
  else if (*tok == TOK_OP_ARR_BRACKET_OPEN)
    return parseArray();

  
  TokenType type = tok->getType();
  if (std::get<1>(OPERATORS_INFO.at(tok->getType())) == OperatorType::BINARY) {
    auto bioptounop = TOKEN_BIOP_TO_UNOP.find(tok->getType());
    if (bioptounop == TOKEN_BIOP_TO_UNOP.end())
      return nullptr;

    type = std::get<1>(*bioptounop);
  }

  lexer.next(); // eat unary operator
  // ignore newline tekons
  while (*lexer.getCurrentToken() == TOK_EOL)
    lexer.next();

  const Precedence prec = std::get<0>(OPERATORS_INFO.at(type));
  std::unique_ptr<Expr> expr = parseExpression(prec);
  if (!expr)
    return nullptr;

  return std::make_unique<UnOpExpr>(lexer,
    tok->getPosition() + expr->getPosition(), tok, std::move(expr));
}

std::unique_ptr<Expr> Parser::parsePrimary() noexcept {
  const Token *tok = lexer.getCurrentToken();
  if (isTokenTypeOperator(tok->getType()))
    return parseUnary();

  switch (tok->getType()) {
  case TOK_ID:
  case TOK_INT8:
  case TOK_INT16:
  case TOK_INT32:
  case TOK_INT64:
  case TOK_UINT8:
  case TOK_UINT16:
  case TOK_UINT32:
  case TOK_UINT64:
  case TOK_FLT32:
  case TOK_FLT64:
  case TOK_CHAR:
  case TOK_STR:
  case TOK_KW_TRUE:
  case TOK_KW_FALSE:
    lexer.next();
    return std::make_unique<TokenExpr>(lexer, tok);
  case TOK_KW_FN:
    return parseFunction();
  case TOK_KW_LAMBDA:
    return parseLambda();
  case TOK_KW_MOD:
    return parseModule();
  case TOK_KW_CLASS:
    return parseClass();
  case TOK_KW_ENUM:
    return parseEnum();
  case TOK_KW_TRAIT:
    return parseTrait();
  case TOK_KW_TYPE:
    return parseType();
  case TOK_KW_USE:
    return parseUse();
  case TOK_KW_IF:
    return parseIf();
  case TOK_KW_ENSURE:
    return parseIf(true);
  case TOK_KW_MATCH:
    return parseMatch();
  case TOK_KW_FOR:
    return parseFor();
  case TOK_KW_DO:
    return parseFor(true);
  case TOK_KW_CTN:
    return parseContinue();
  case TOK_KW_BRK:
    return parseBreak();
  case TOK_KW_SAFE:
    return parseSafe();
  default:
    return nullptr;
  }
}

std::unique_ptr<Expr> Parser::parseArray() noexcept {
  const Token *const startToken = lexer.getCurrentToken();
  sanityExpect(TOK_OP_ARR_BRACKET_OPEN);

  std::unique_ptr<Expr> expr0(parseExpression());
  if (!expr0)
    return nullptr;

  if (expect(TOK_STMT)) {
    std::unique_ptr<Expr> expr1(parseExpression());
    if (!expr1)
      return nullptr;

    const Token *const endToken = lexer.getCurrentToken();
    if (!expect(TOK_ARR_BRACKET_CLOSE)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_ARR_CLOSING_BRACKET,
        lexer.getCurrentToken()->getPosition(),
        std::vector<Position> { startToken->getPosition() }));
    }

    const Position pos(startToken->getPosition() +
      expr0->getPosition() + expr1->getPosition() + endToken->getPosition());

    return std::make_unique<ArrayCpyExpr>(lexer, pos,
      std::move(expr0), std::move(expr1));
  }
  // now it's either an ArrayEmptyExpr or ArrayLitExpr
  if (isBiOpExpr(*expr0, TOK_OP_COMMA)) {
    Position pos(expr0->getPosition());

    std::list<std::unique_ptr<Expr>> exprs;
    while (isBiOpExpr(*expr0, TOK_OP_COMMA)) {
      BiOpExpr &biopexpr = dynamic_cast<BiOpExpr&>(*expr0);
      exprs.insert(exprs.begin(), biopexpr.getRightPtr());
      expr0 = biopexpr.getLeftPtr();
    }

    exprs.insert(exprs.begin(), std::move(expr0));

    const Token *const endToken = lexer.getCurrentToken();
    if (!expect(TOK_ARR_BRACKET_CLOSE)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_ARR_CLOSING_BRACKET,
        lexer.getCurrentToken()->getPosition(),
        std::vector<Position> { startToken->getPosition() }));
    }

    pos = pos + startToken->getPosition() + endToken->getPosition();

    return std::make_unique<ArrayLitExpr>(lexer, pos, std::move(exprs));
  }

  const Token *const endToken = lexer.getCurrentToken();
  if (!expect(TOK_ARR_BRACKET_CLOSE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_ARR_CLOSING_BRACKET,
      lexer.getCurrentToken()->getPosition(),
      std::vector<Position> { startToken->getPosition() }));
  }

  const Position pos(startToken->getPosition() +
    expr0->getPosition() + endToken->getPosition());

  return std::make_unique<ArrayEmptyExpr>(lexer, pos,
    std::move(expr0));
}

std::unique_ptr<Expr> Parser::parseBrackets() noexcept {
  sanityExpect(TOK_OP_BRACKET_OPEN);

  std::unique_ptr<Expr> result(parseExpression());
  if (!expect(TOK_BRACKET_CLOSE)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_CLOSING_BRACKET,
      lexer.getCurrentToken()->getPosition()));
  }

  return result;
}

std::unique_ptr<FuncParameter> Parser::fromExprDeclToFunctionParam(
    std::unique_ptr<Expr> &&expr, std::unique_ptr<Expr> &&guard,
    std::unique_ptr<Expr> &&guardResult) noexcept {
  if (isBiOpExpr(*expr, TOK_OP_DCL)) {
    BiOpExpr &biopdcl = dynamic_cast<BiOpExpr&>(*expr);
    if (!isTokenExpr(biopdcl.getLeft(), TOK_ID)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_INVALID_VARDECL_ID, biopdcl.getLeft().getPosition()));
      return nullptr;
    }

    return std::make_unique<FuncParameter>(
      &dynamic_cast<const TokenExpr&>(biopdcl.getLeft()).getToken(),
      biopdcl.getRightPtr(),
      std::move(guard), std::move(guardResult));
  }

  return std::make_unique<FuncParameter>(
    nullptr, std::move(expr), std::move(guard), std::move(guardResult));
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

std::unique_ptr<Expr> Parser::parseFunction() noexcept {
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
        STX_ERR_FUNC_VAR_NO_TEMPL, std::get<1>(*templ->at(0))->getPosition()));
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

    return std::make_unique<FuncExpr>(lexer, tok->getPosition(),
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

  return std::make_unique<FuncExpr>(lexer, tok->getPosition(),
    tok, std::move(templ), std::move(parameters),
    std::move(returnExpr), std::move(body), autoDetect);
}

std::unique_ptr<BodyExpr> Parser::parseFunctionBody() noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  bool err = false;
  Exprs exprs;
  while (*lexer.getCurrentToken() != TOK_KW_RET
      && *lexer.getCurrentToken() != TOK_STMT
      && *lexer.getCurrentToken() != TOK_EOF) {
    if (*lexer.getCurrentToken() == TOK_EOL) {
      lexer.next();
      continue;
    }

    std::unique_ptr<Expr> expr(parseExpression());
    if (!expect(TOK_EOL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
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
    if (!returnExpr)
      return nullptr;
    pos = pos + returnExpr->getPosition();
  }

  while (*lexer.getCurrentToken() == TOK_EOL)
    lexer.next();

  if (err)
    return nullptr;

  return std::make_unique<BodyExpr>(lexer,
    pos, std::move(exprs), std::move(returnExpr));
}

std::unique_ptr<Expr> Parser::parseLambda() noexcept {
  sanityExpect(TOK_KW_LAMBDA);

  bool err = false;

  Exprs params;
  if (*lexer.getCurrentToken() == TOK_OP_BRACKET_OPEN) {
    lexer.next();

    std::unique_ptr<Expr> paramexpr(parseExpression());
    if (!paramexpr) {
      err = true;
    } else {
      while (isBiOpExpr(*paramexpr, TOK_OP_COMMA)) {
        BiOpExpr& biopexpr = dynamic_cast<BiOpExpr&>(*paramexpr);
        params.insert(params.begin(), biopexpr.getRightPtr());
        paramexpr = biopexpr.getLeftPtr();
      }

      params.insert(params.begin(), std::move(paramexpr));
    }

    if (!expect(TOK_BRACKET_CLOSE)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_CLOSING_BRACKET, lexer.getCurrentToken()->getPosition()));
      err = true;
    }
  }

  if (*lexer.getCurrentToken() == TOK_OP_ASG) {
    // *=* *expr*
    lexer.next(); // eat =

    std::unique_ptr<Expr> expr(parseExpression());
    if (!expr) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_EXPR, lexer.getCurrentToken()->getPosition()));
      return nullptr;
    }

    if (err)
      return nullptr;

    Position pos(expr->getPosition());

    return std::make_unique<LambdaExpr>(lexer, pos,
      std::move(params), std::make_unique<BodyExpr>(lexer,
        expr->getPosition(),
        Exprs(), std::move(expr)));
  }
  // *newline* *body* *;*
  if (!expect(TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
    err = true;
  }

  std::unique_ptr<BodyExpr> body(parseFunctionBody());
  if (!body)
    err = true;

  if (!expect(TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition()));
    err = true;
  }

  if (err)
    return nullptr;

  return std::make_unique<LambdaExpr>(lexer, body->getPosition(),
    std::move(params), std::move(body));
}

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

std::unique_ptr<Expr> Parser::parseClass() noexcept {
  sanityExpect(TOK_KW_CLASS);
   // TODO
}

std::unique_ptr<Expr> Parser::parseEnum() noexcept {
  sanityExpect(TOK_KW_ENUM);
   // TODO
}

std::unique_ptr<Expr> Parser::parseTrait() noexcept {
  sanityExpect(TOK_KW_TRAIT);
   // TODO
}

std::unique_ptr<Expr> Parser::parseType() noexcept {
  sanityExpect(TOK_KW_TYPE);
   // TODO
}

std::unique_ptr<Expr> Parser::parseUse() noexcept {
  sanityExpect(TOK_KW_USE);
  if (expect(TOK_KW_MOD)) {
    if (!expect(TOK_ID)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_ID, lexer.getCurrentToken()->getPosition()));
      return nullptr;
    }

    const Token *tok = lexer.getCurrentToken();
    lexer.next(); // eat id

    return std::make_unique<ProgNameExpr>(lexer, tok);
  }

  std::unique_ptr<Expr> expr(parseExpression());
  if (!expr)
    return nullptr;

  Position pos(expr->getPosition());

  Exprs exprs;
  while (isBiOpExpr(*expr, TOK_OP_COMMA)) {
    BiOpExpr& biopexpr = dynamic_cast<BiOpExpr&>(*expr);
    exprs.insert(exprs.begin(), biopexpr.getRightPtr());
    expr = biopexpr.getLeftPtr();
  }

  exprs.insert(exprs.begin(), std::move(expr));

  return std::make_unique<UseExpr>(lexer, pos, std::move(exprs));
}

std::unique_ptr<Expr> Parser::parseFor(bool isdo) noexcept {
  if (isdo)
    sanityExpect(TOK_KW_DO);
  else
    sanityExpect(TOK_KW_FOR);
   // TODO
}

std::unique_ptr<Expr> Parser::parseMatch() noexcept {
  sanityExpect(TOK_KW_MATCH);
   // TODO
}

std::unique_ptr<Expr> Parser::parseIf(bool isensure) noexcept {
  if (isensure)
    sanityExpect(TOK_KW_ENSURE);
  else
    sanityExpect(TOK_KW_IF);
   // TODO
}

std::unique_ptr<Expr> Parser::parseContinue() noexcept {
  sanityExpect(TOK_KW_CTN);
   // TODO
}

std::unique_ptr<Expr> Parser::parseBreak() noexcept {
  sanityExpect(TOK_KW_BRK);
   // TODO
}

std::unique_ptr<Expr> Parser::parseSafe() noexcept {
  sanityExpect(TOK_KW_SAFE);
   // TODO
}

std::unique_ptr<Expr> Parser::parseTemplate() noexcept {
  sanityExpect(TOK_OP_TEMPL_BRACKET_OPEN);
   // TODO
}

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
        && *lexer.getCurrentToken() != TOK_EOL) {
      if (isprog)
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_EOF_EOL, lexer.getCurrentToken()->getPosition()));
      else
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
      err = true;
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

std::unique_ptr<Expr> Parser::parseExpression(Precedence prec) noexcept {
  auto primary = parsePrimary();
  if (!primary) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_EXPR, lexer.getCurrentToken()->getPosition()));
    return nullptr; // error forwarding
  }
  
  return parseBinary(std::move(primary), prec);
}

// SyntaxError
SyntaxError::~SyntaxError() {
}
