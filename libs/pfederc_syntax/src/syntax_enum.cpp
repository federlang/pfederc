#include "pfederc/syntax.hpp"
using namespace pfederc;

std::unique_ptr<Expr> Parser::parseEnum() noexcept {
  sanityExpect(TOK_KW_ENUM);

  bool err = false; // hard errors

  std::unique_ptr<TemplateDecls> templ;
  if (*lexer.getCurrentToken() == TOK_OP_TEMPL_BRACKET_OPEN) {
    templ = parseTemplateDecl();
    // maybe soft error
  }

  const Token *const tokId = lexer.getCurrentToken();
  if (!expect(TOK_ID)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_ID, tokId->getPosition()));
    err = true;
  }

  if (!expect(TOK_EOL)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_EOL, lexer.getCurrentToken()->getPosition()));
  }

  while (*lexer.getCurrentToken() != TOK_STMT
      && *lexer.getCurrentToken() != TOK_EOF) {
    if (*lexer.getCurrentToken() == TOK_EOL) {
      lexer.next(); // skip eol
      continue;
    }

    const Token *const tokConstructorId = lexer.getCurrentToken();
    if (!expect(TOK_ID)) {
      generateError(std::make_unique<SyntaxError>(LVL_ERROR,
        STX_ERR_EXPECTED_ID, tokConstructorId->getPosition()));
      err = true;
    }
  }

  if (!expect(TOK_STMT)) {
    generateError(std::make_unique<SyntaxError>(LVL_ERROR,
      STX_ERR_EXPECTED_STMT, lexer.getCurrentToken()->getPosition(),
      std::vector<Position> { tokId->getPosition() }));
    err = true;
  }

  if (err)
    return nullptr;
}

std::unique_ptr<Expr> Parser::parseTrait() noexcept {
  sanityExpect(TOK_KW_TRAIT);
   // TODO
}

std::unique_ptr<Expr> Parser::parseType() noexcept {
  sanityExpect(TOK_KW_TYPE);
   // TODO
}
