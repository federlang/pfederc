#include "pfederc/semantics.hpp"
using namespace pfederc;

std::string pfederc::mangleModule(Expr *parent, const std::string &name) noexcept {
  std::string result = "m";
  std::string tmp;
  while (parent) {
    if (*parent == ExprType::EXPR_MOD) {
      const std::string name = dynamic_cast<ModExpr*>(parent)->getIdentifier().toString(parent->getLexer());
      tmp.insert(0, std::to_string(name.length()) + name);
      tmp.insert(tmp.begin(), 1, '_');
    } else if (*parent == ExprType::EXPR_PROG) {
      const Token *progExpr = dynamic_cast<ProgramExpr*>(parent)->getProgramName();
      if (progExpr) {
        const std::string name = progExpr->toString(parent->getLexer());
        tmp.insert(0, std::to_string(name.length()) + name);
        tmp.insert(tmp.begin(), 1, '_');
      }
    }
    // iterator step
    parent = parent->getParent();
  }

  result += tmp;
  result += '_';
  result += std::to_string(name.length());
  result += name;

  return result;
}

std::string pfederc::mangleProgram(const std::string &name) noexcept {
  return "p_" + std::to_string(name.length()) + name;
}

std::string pfederc::mangleGlobalVariable(const Expr *parent, const std::string &name) noexcept;

std::string pfederc::mangleTrait(const Expr *parent, const std::string &name) noexcept;

std::string pfederc::mangleClass(const Expr *parent, const std::string &name) noexcept;
