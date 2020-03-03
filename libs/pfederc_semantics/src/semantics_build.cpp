#include "pfederc/semantics.hpp"
using namespace pfederc;

Semantic* TypeAnalyzer::buildSemantic(size_t fileIdx, bool build,
                       Semantic *parent, Expr *program) noexcept {

}

void TypeAnalyzer::helperBuildProgramSemantic(const BuildSemanticParam &param) noexcept {
  const ProgramExpr& prog = *param.expr;
  if (prog.getProgramName()) {
    const std::string progname = prog.getProgramName()->toString(prog.getLexer());
    const std::string mangle = mangleProgram(progname);
    std::unique_ptr<Semantic> semantic = std::make_unique<SafeSemantic>(
        ST_NAME | ST_MODULE | ST_PROGRAM,
        param.expr,
        nullptr,
        std::unordered_map<SemanticInfoStringType,std::string>
          {{SemanticInfoStringType::NAME, progname},
           {SemanticInfoStringType::MANGLE, mangle}});
    Semantic *pSemantic = semantic.get();
    // add new module to store
    if (pSemantic == addSemanticAlways(mangle, std::move(semantic))) {
      // add new module to programs
      programs.insert(std::pair<std::string,Semantic*>(progname, pSemantic));
    }
  }
}

void TypeAnalyzer::helperBuildProgramSemantics(const std::list<BuildSemanticParam> &list) noexcept {
  for (auto &param : list)
    helperBuildProgramSemantic(param);
}


void TypeAnalyzer::helperBuildModuleSemantic(Expr* parent, ModExpr *expr) noexcept {
  Semantic *parentSemantic = nullptr;
  auto it = exprToSemantic.find(parent);
  if (it != exprToSemantic.end())
    parentSemantic = it->second;

  const std::string name = expr->getIdentifier().toString(expr->getLexer());
  const std::string mangle = mangleModule(parent, name);
  std::unique_ptr<Semantic> semantic = std::make_unique<SafeSemantic>(
      ST_NAME | ST_MODULE,
      expr,
      parentSemantic,
      std::unordered_map<SemanticInfoStringType,std::string>
        {{SemanticInfoStringType::NAME, name},
         {SemanticInfoStringType::MANGLE, mangle}});

  addSemanticAlways(mangle, std::move(semantic));

  for (auto &def : expr->getExpressions())
    if (def->getType() == ExprType::EXPR_MOD)
      helperBuildModuleSemantic(expr,
          dynamic_cast<ModExpr*>(def.get()));
}

void TypeAnalyzer::helperBuildModuleSemantics(const std::list<BuildSemanticParam> &list) noexcept {
  for (auto &param : list)
    for (auto &def : param.expr->getDefinitions())
      if (def->getType() == ExprType::EXPR_MOD)
        helperBuildModuleSemantic(param.expr,
            dynamic_cast<ModExpr*>(def.get()));
}

void TypeAnalyzer::buildSemantics(const std::list<BuildSemanticParam> &list) noexcept {
  // As Feder allows its programs to have an abitrary order their semantics
  // it is not feasable to read a Feder expression just one time.
  // This algorithm first builds semantics of all module expressions
  // and afterwards adds every expression, which is either a global variable,
  // class, function, trait implementation, trait, type (def), enumerations,
  // to a list. Then the list is iterated and every buildSemantic() which
  // doesn't evaluate to nullptr is removed from said list. This is done
  // as long as the list is not empty and the list is reduced eache whole
  // iteration. If the list isn't reduce during a whole iteration, it is
  // impossible to build the program. An error is created.

  helperBuildProgramSemantics(list);

  helperBuildModuleSemantics(list);
}
