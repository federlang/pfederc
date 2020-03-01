#include "pfederc/semantics.hpp"
using namespace pfederc;

std::string pfederc::mangleModule(Expr *parent, const std::string &name) noexcept;

std::string pfederc::mangleFunction(Expr *parent, const std::string &name,
    std::iterator<std::input_iterator_tag, std::string> beginParam,
    std::iterator<std::input_iterator_tag, std::string> endParam,
    const std::string &returnType) noexcept;

std::string pfederc::mangleClass(Expr *parent, const std::string &name) noexcept;

std::string mangleTrait(Expr *parent, const std::string &name) noexcept;
