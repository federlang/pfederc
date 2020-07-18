#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseEnum() noexcept {
  Position pos(lexer.getCurrentToken()->getPosition());
  sanityExpect(TokenType::TOK_KW_ENUM);

  bool err = false; // hard errors

  TemplateDecls templ;
  if (*lexer.getCurrentToken() == TokenType::TOK_OP_TEMPL_BRACKET_OPEN) {
    templ = parseTemplateDecl();
    // maybe soft error
  }

  const Token *const tokId = lexer.getCurrentToken();
  if (!expect(TokenType::TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_ID, tokId->getPosition()));
    err = true;
  }

  if (!expect(TokenType::TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
  }

  std::vector<EnumConstructor> constructors;
  parseEnumBody(err, constructors);

  if (!expect(TokenType::TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      SyntaxErrorCode::STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition(),
      std::vector<Position> { tokId->getPosition() }));
    err = true;
  }

  if (err)
    return nullptr;

  return std::make_unique<EnumExpr>(lexer, pos + tokId->getPosition(),
      tokId, std::move(templ), std::move(constructors));
}

void Parser::parseEnumBody(bool &err,
    std::vector<EnumConstructor> &constructors) noexcept {
  while (*lexer.getCurrentToken() != TokenType::TOK_STMT
      && *lexer.getCurrentToken() != TokenType::TOK_EOF) {
    if (*lexer.getCurrentToken() == TokenType::TOK_EOL) {
      lexer.next(); // skip eol
      continue;
    }

    const Token *const tokConstructorId = lexer.getCurrentToken();
    if (!expect(TokenType::TOK_ID)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        SyntaxErrorCode::STX_ERR_EXPECTED_ID, tokConstructorId->getPosition()));
      err = true;
      skipToStmtEol();
      continue;
    }

    std::vector<std::unique_ptr<Expr>> args;
    if (*lexer.getCurrentToken() == TokenType::TOK_OP_BRACKET_OPEN) {
      lexer.next(); // eat (

      parseTraitImpl(err, args);
      if (!expect(TokenType::TOK_BRACKET_CLOSE)) {
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_CLOSING_BRACKET, lexer.getCurrentToken()->getPosition()));
      }
    }

    constructors.push_back(EnumConstructor(tokConstructorId, std::move(args)));

    if (!expect(TokenType::TOK_EOL)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        SyntaxErrorCode::STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
      skipToStmtEol();
    }
  }
}
