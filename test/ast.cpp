#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/lexer.hpp"
#include "pfederc/syntax.hpp"
#include <sstream>
using namespace pfederc;

int main(int argc, char * argsv[]) {
  if (argc != 2 || strlen(argsv[1]) == 0) {
    std::cerr << "Expected expression (Single argument)" << std::endl;
    return 1;
  }

  std::istringstream input(argsv[1]);
  LanguageConfiguration cfg = createDefaultLanguageConfiguration();
  Lexer lex(cfg, input, "<args[1]>");

  Logger log;
  
  lex.next();
  Parser parser(lex);
  auto expr = parser.parseExpression();

  if (logLexerErrors(log, lex))
    return 1;

  if (!expr) {
    std::cout << "ERR" << std::endl;
    logParserErrors(log, parser);
    return 1;
  }

  if (logParserErrors(log, parser))
    return 1;

  std::cout << expr->toString() << std::endl;

  return 0;
}
