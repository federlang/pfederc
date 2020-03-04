#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/lexer.hpp"
#include "pfederc/syntax.hpp"
#include "pfederc/semantics.hpp"
#include <sstream>
using namespace pfederc;

int main(int argsc, char *argsv[]) {
  if (argsc < 2 || strlen(argsv[1]) == 0)
    return 1;

  std::istringstream input(argsv[1]);
  LanguageConfiguration cfg = createDefaultLanguageConfiguration();
  Lexer lex(cfg, input, "<args[1]>");

  Logger log;
  
  lex.next();
  Parser parser(lex);

  auto program = parser.parseProgram();

  if (logLexerErrors(log, lex))
    return 1;

  if (!program) {
    std::cout << "ERR" << std::endl;
    logParserErrors(log, parser);
    return 1;
  }

  if (logParserErrors(log, parser))
    return 1;

  TypeAnalyzer semanticAna(1);
  semanticAna.buildSemantics(std::list<BuildSemanticParam>{
      BuildSemanticParam{"<args[1]>", true, program.get()}});

  std::vector<const std::string*> mangles;
  semanticAna.forEachSemantic([&mangles](const std::string &mangle, Semantic *semantic) {
      mangles.push_back(&mangle);
  });

  std::sort(mangles.begin(), mangles.end(),
      [](const std::string *pstr0, const std::string *pstr1) {
        const std::string &str0 = *pstr0;
        const std::string &str1 = *pstr1;
        size_t i = 0;
        for (; i < str0.length() && i < str1.length(); ++i) {
          if (str0[i] < str1[i])
            return -1;
          else if (str0[i] != str1[i])
            return 1;
        }

        if (i == str0.length() && i == str1.length())
          return 0;
        if (i == str0.length())
          return 1;

        return -1;
      });

  for (auto &str : mangles)
    std::cout << *str << std::endl;

  return 0;
}
