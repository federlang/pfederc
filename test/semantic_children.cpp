#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/lexer.hpp"
#include "pfederc/syntax.hpp"
#include "pfederc/semantics.hpp"
#include <sstream>
using namespace pfederc;

int main() {
  std::istringstream input("requiredinputtobuildexpr");
  LanguageConfiguration cfg = createDefaultLanguageConfiguration();
  Lexer lex(cfg, input, "<input>");

  Logger log;
  lex.next();
  Parser parser(lex);
  auto expr = parser.parseExpression(); // TokenExpr, TokenType: TOK_ID

  Semantic semantic(0u, expr.get(), nullptr);
  Semantic semantic0(0u, expr.get(), nullptr);
  Semantic semantic1(0u, expr.get(), nullptr);

  if (!semantic.addChild("child0", &semantic0))
    return 1;

  if (semantic.addChild("child0", &semantic1))
    return 1;

  if (!semantic.addChild("child1", &semantic1))
    return 1;

  return 0;
}
