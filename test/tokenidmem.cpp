#include "pfederc/lexer.hpp"
#include <sstream>
using namespace pfederc;

int main(int argsc, char * argsv[]) {
  if (argsc == 0)
    return 1;

  std::istringstream input(argsv[1]);
  LanguageConfiguration cfg = createDefaultLanguageConfiguration();
  Lexer lex(cfg, input, "<args[1]>");
  Token *tok = lex.next();
  std::cout << TOKEN_TYPE_STRINGS.at(tok->getType()) << std::endl;
  Logger log;
  logLexerErrors(log, lex);
  return 0;
}
