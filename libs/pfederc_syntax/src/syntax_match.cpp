#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseMatch() noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  sanityExpect(TokenType::TOK_KW_MATCH);

  bool err = false;

  std::unique_ptr<Expr> expr(parseExpression());
  if (!expect(TokenType::TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
  }

  std::vector<MatchPattern> cases;
  parseMatchCases(err, cases);

  std::unique_ptr<BodyExpr> anyCase;
  if (expect(TokenType::TOK_ANY)) {
    if (!expect(TokenType::TOK_IMPL)) {
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
              SyntaxErrorCode::STX_ERR_EXPECTED_OP_IMPL,
              lexer.getCurrentToken()->getPosition()));
    }

    anyCase = parseFunctionBody();
    if (!expect(TokenType::TOK_STMT)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
            SyntaxErrorCode::STX_ERR_EXPECTED_STMT,
            lexer.getCurrentToken()->getPosition()));
    }

    skipEol();
  }

  if (!expect(TokenType::TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_STMT,
          lexer.getCurrentToken()->getPosition()));
  }

  if (!expr || cases.empty() || err)
    return nullptr;

  return std::make_unique<MatchExpr>(lexer, pos, std::move(expr),
      std::move(cases), std::move(anyCase));
}

void Parser::parseMatchCases(bool &err,
    std::vector<MatchPattern> &cases) noexcept {

  while (*lexer.getCurrentToken() != TokenType::TOK_STMT
      && *lexer.getCurrentToken() != TokenType::TOK_EOF
      && *lexer.getCurrentToken() != TokenType::TOK_ANY) {
    skipEol();

    MatchPattern matchPattern = parseMatchCase(err);
    if (std::get<0>(matchPattern) && std::get<2>(matchPattern))
      cases.push_back(std::move(matchPattern));
  }
}

MatchPattern Parser::parseMatchCase(bool &err) noexcept {
  const Token *tokId = lexer.getCurrentToken();
  if (*lexer.getCurrentToken() != TokenType::TOK_ID
      && !isNumberType(lexer.getCurrentToken()->getType())
      && *lexer.getCurrentToken() != TokenType::TOK_CHAR
      && *lexer.getCurrentToken() != TokenType::TOK_KW_TRUE
      && *lexer.getCurrentToken() != TokenType::TOK_KW_FALSE) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_ID_NUM_CHAR_BOOL,
          lexer.getCurrentToken()->getPosition()));
    if (*lexer.getCurrentToken()  != TokenType::TOK_OP_BRACKET_OPEN
        && *lexer.getCurrentToken() != TokenType::TOK_IMPL)
      lexer.next(); // skip
    tokId = nullptr;
  } else {
    lexer.next();
  }

  std::vector<const Token *> vars;
  if (expect(TokenType::TOK_OP_BRACKET_OPEN)) {
    do {
      skipEol();
      const Token *const tokVarId = lexer.getCurrentToken();
      if (*tokVarId != TokenType::TOK_ID && *tokVarId != TokenType::TOK_ANY) {
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
              SyntaxErrorCode::STX_ERR_EXPECTED_ID_ANY,
              lexer.getCurrentToken()->getPosition()));
        if (*tokVarId != TokenType::TOK_OP_COMMA)
          lexer.next();
      } else {
        lexer.next(); // eat id|any

        vars.push_back(tokVarId);
      }
    } while (expect(TokenType::TOK_OP_COMMA));

    if (!expect(TokenType::TOK_BRACKET_CLOSE)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
            SyntaxErrorCode::STX_ERR_EXPECTED_CLOSING_BRACKET,
            lexer.getCurrentToken()->getPosition()));
    }
  }

  if (!expect(TokenType::TOK_IMPL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
            SyntaxErrorCode::STX_ERR_EXPECTED_OP_IMPL,
            lexer.getCurrentToken()->getPosition()));
  }

  skipEol();

  std::unique_ptr<BodyExpr> bodyExpr(parseFunctionBody());
  if (!expect(TokenType::TOK_STMT)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
            SyntaxErrorCode::STX_ERR_EXPECTED_STMT,
            lexer.getCurrentToken()->getPosition()));
  }

  skipEol();

  return MatchPattern(tokId, std::move(vars), std::move(bodyExpr));
}
