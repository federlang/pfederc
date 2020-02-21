#include "pfederc/core.hpp"
#include "pfederc/token.hpp"
#include "pfederc/lexer.hpp"
#include <sstream>
using namespace pfederc;

int main(int argsc, char * argsv[]) {
  if (argsc <= 1) {
    std::cout << "Expected 1st argument to be token" << std::endl;
    return 1;
  }

  std::istringstream input(argsv[1]);
  LanguageConfiguration cfg = createDefaultLanguageConfiguration();
  Lexer lex(cfg, input, "<args[1]>");
  const Token &tok = lex.next();

  std::cout << tok.toString(lex) << std::endl;
  Logger log;
  logLexerErrors(log, lex);
  return tok != TOK_ERR ? 0 : 1;
}
