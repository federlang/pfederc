#include "pfederc/semantics.hpp"
using namespace pfederc;

std::string pfederc::mangleModule(Expr *parent, const std::string &name) noexcept {
  std::string result = "m_";
  while (parent) {
  }
  return result;
}

std::string pfederc::mangleProgram(const std::string &name) noexcept {
  return "p_" + name.length() + name;
}

std::string pfederc::mangleGlobalVariable(const Expr *parent, const std::string &name) noexcept;

std::string pfederc::mangleTrait(const Expr *parent, const std::string &name) noexcept;

std::string pfederc::mangleClass(const Expr *parent, const std::string &name) noexcept;
