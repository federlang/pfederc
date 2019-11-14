#include "pfederc/expr.hpp"
using namespace pfederc;

// Expr
Expr::Expr(const Lexer &lexer, ExprType type, const Position &pos) noexcept
    : lexer{lexer}, type{type}, pos(pos) {

}

Expr::~Expr() {
}

// Capabilities
Capabilities::~Capabilities() {
}

// ProgramExpr
ProgramExpr::ProgramExpr(const Lexer &lexer, const Position &pos,
    const Token *progName,
    Exprs &&imports,
    Exprs &&defs) noexcept
    : Expr(lexer, EXPR_PROG, pos),
      imports(std::move(imports)),
      defs(std::move(defs)) {
}

ProgramExpr::~ProgramExpr() {
}

std::string ProgramExpr::toString() const noexcept {
  std::string result;
  if (progName)
    result += "use mod " + progName->toString(getLexer()) + "\n";

  for (auto &expr : imports)
    result += expr->toString() + "\n";

  for (auto &expr : defs)
    result += expr->toString() + "\n";

  return result;
}

// TokenExpr
TokenExpr::TokenExpr(const Lexer &lexer, const Token *tok) noexcept
    : Expr(lexer, EXPR_TOK, tok->getPosition()), tok{tok} {
}

TokenExpr::~TokenExpr() {

}

std::string TokenExpr::toString() const noexcept {
  return getToken().toString(getLexer());
}

// UseExpr
UseExpr::UseExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Expr> &&expr) noexcept
    : Expr(lexer, EXPR_USE, pos), expr(std::move(expr)) {
}

UseExpr::~UseExpr() {

}

std::string UseExpr::toString() const noexcept {
  return "use " + getExpression().toString();
}

// FuncExpr
FuncExpr::FuncExpr(const Lexer &lexer, const Position &pos,
    const Token *tokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::vector<std::unique_ptr<FuncParameter>> &&params,
    std::unique_ptr<Expr> &&returnExpr,
    std::unique_ptr<BodyExpr> &&body) noexcept
    : Expr(lexer, EXPR_FUNC, pos), tokId{tokId},
      templs(std::move(templs)), params(std::move(params)),
      returnExpr(std::move(returnExpr)), body(std::move(body)) {
}

FuncExpr::~FuncExpr() {
}

std::string FuncExpr::toString() const noexcept {
  std::string result = "func";
  // TODO template
  result += ' ';
  result += getIdentifier().toString(getLexer());
  if (!params.empty()) {
    result += '(';
    for (auto it = params.begin(); it != params.end(); ++it) {
      if (it != params.begin())
        result += ", ";
      
      const auto &tpl = **it;
      if (std::get<0>(tpl)) {
        result += std::get<0>(tpl)->toString(getLexer());
        result += ": ";
      }

      result += std::get<1>(tpl)->toString();
      if (std::get<2>(tpl)) {
        result += " | ";
        result += std::get<2>(tpl)->toString();
        if (std::get<3>(tpl)) {
          result += " = ";
          result += std::get<3>(tpl)->toString();
        }
      }
    }
    result += ')';
  }

  if (returnExpr) {
    result += ": ";
    result += returnExpr->toString();
  }

  if (!body) {
    result += ';';
    return result;
  }

  result += '\n';
  result += body->toString();
  result += ';';
  return result;
}

// FuncTypeExpr
FuncTypeExpr::FuncTypeExpr(const Lexer &lexer, const Position &pos,
    std::vector<std::unique_ptr<FuncParameter>>  &&params,
    std::unique_ptr<Expr> &&returnExpr) noexcept
    : Expr(lexer, EXPR_FUNCTYPE, pos),
      params(std::move(params)), returnExpr(std::move(returnExpr)) {
}

FuncTypeExpr::~FuncTypeExpr() {
}

std::string FuncTypeExpr::toString() const noexcept {
  // TODO
  return "";
}

// LambdaExpr
LambdaExpr::LambdaExpr(const Lexer &lexer, const Position &pos,
    Exprs &&params,
    std::unique_ptr<BodyExpr> &&body) noexcept
    : Expr(lexer, EXPR_LAMBDA, pos),
      params(std::move(params)), body(std::move(body)) {
}

LambdaExpr::~LambdaExpr() {
}

std::string LambdaExpr::toString() const noexcept {
  // TODO
  return "";
}

// TraitExpr
TraitExpr::TraitExpr(const Lexer &lexer, const Position &pos,
    const Token *tokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept
    : Expr(lexer, EXPR_TRAIT, pos), tokId{tokId},
      templs(std::move(templs)), functions(std::move(functions)) {
}

TraitExpr::~TraitExpr() {
}

std::string TraitExpr::toString() const noexcept {
  std::string result("trait ");

  result += getIdentifier().toString(getLexer());

  // TODO

  return result;
}

// ClassExpr
ClassExpr::ClassExpr(const Lexer &lexer, const Position &pos,
    const Token *tokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::vector<std::unique_ptr<BiOpExpr>> &&constructAttributes,
    std::vector<std::unique_ptr<BiOpExpr>> &&attributes,
    std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept
    : Expr(lexer, EXPR_CLASS, pos), tokId{tokId},
      templs(std::move(templs)),
      constructAttributes(std::move(constructAttributes)),
      attributes(std::move(attributes)),
      functions(std::move(functions)) {
}

ClassExpr::~ClassExpr() {
}

std::string ClassExpr::toString() const noexcept {
  // TODO
  return "";
}

// TraitImplExpr
TraitImplExpr::TraitImplExpr(const Lexer &lexer, const Position &pos,
    const Token *classTokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::unique_ptr<Expr> &&implTrait,
    std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept
    : Expr(lexer, EXPR_TRAITIMPL, pos), classTokId{classTokId},
      templs(std::move(templs)),
      implTrait(std::move(implTrait)), functions(std::move(functions)) {
}

TraitImplExpr::~TraitImplExpr() {
}

std::string TraitImplExpr::toString() const noexcept {
  return "";
}

// EnumExpr
EnumExpr::EnumExpr(const Lexer &lexer, const Position &pos,
    const Token *tokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::vector<EnumConstructor> &&constructors) noexcept
    : Expr(lexer, EXPR_ENUM, pos), tokId{tokId},
      templs(std::move(templs)), constructors(std::move(constructors)) {
}

EnumExpr::~EnumExpr() {
}

std::string EnumExpr::toString() const noexcept {
  // TODO
  return "";
}

// ModExpr
ModExpr::ModExpr(const Lexer &lexer, const Position &pos,
    const Token *tokId, Exprs &&exprs) noexcept
    : Expr(lexer, EXPR_MOD, pos), tokId{tokId}, exprs(std::move(exprs)) {
}

ModExpr::~ModExpr() {
}

std::string ModExpr::toString() const noexcept {
  return "mod " + getIdentifier().toString(getLexer()) + ";";
}

// SafeExpr
SafeExpr::SafeExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Expr> &&expr) noexcept
    : Expr(lexer, EXPR_SAFE, pos), expr(std::move(expr)) {
}

SafeExpr::~SafeExpr() {
}

std::string SafeExpr::toString() const noexcept {
  return "safe " + getExpression().toString();
}

// IfExpr
IfExpr::IfExpr(const Lexer &lexer, const Position &pos,
    std::vector<IfCase> &&ifCases,
    std::unique_ptr<BodyExpr> &&elseExpr, bool isensure) noexcept
    : Expr(lexer, EXPR_IF, pos), ifCases(std::move(ifCases)),
      elseExpr(std::move(elseExpr)), isensure{isensure} {
}

IfExpr::~IfExpr() {
}

std::string IfExpr::toString() const noexcept {
  std::string result;

  if (isEnsure())
    result += "ensure ";
  else
    result += "if ";

  result += std::get<0>(ifCases[0])->toString() + "\n";
  result += std::get<1>(ifCases[0])->toString();

  for (size_t i = 1; i < ifCases.size(); ++i) {
    result += "else ";
    if (isEnsure())
      result += "ensure ";
    else
      result += "if ";

    result += std::get<0>(ifCases[i])->toString() + "\n";
    result += std::get<1>(ifCases[i])->toString();
  }

  if (getElse()) {
    result += "else\n";
    result += getElse()->toString();
  }

  return result;
}

// LoopExpr
LoopExpr::LoopExpr(const Lexer &lexer, ExprType type, const Position &pos,
    std::unique_ptr<Expr> &&initExpr,
    std::unique_ptr<Expr> &&condExpr,
    std::unique_ptr<Expr> &&itExpr) noexcept
    : Expr(lexer, type, pos), initExpr(std::move(initExpr)),
      condExpr(std::move(condExpr)), itExpr(std::move(itExpr)) {
}

LoopExpr::~LoopExpr() {
}

std::string LoopExpr::toString() const noexcept {
  // TODO
  return "";
}

// MatchExpr
MatchExpr::MatchExpr(const Lexer &lexer, const Position &pos,
    std::vector<MatchPattern> &&cases,
    std::unique_ptr<BodyExpr> &&anyCase) noexcept
    : Expr(lexer, EXPR_MATCH, pos),
      cases(std::move(cases)),
      anyCase(std::move(anyCase)) {
}

MatchExpr::~MatchExpr() {
}

std::string MatchExpr::toString() const noexcept {
  // TODO
  return "";
}

// BiOpExpr
BiOpExpr::BiOpExpr(const Lexer &lexer, const Position &pos,
     const Token *tokOp,
     std::unique_ptr<Expr> &&lhs, std::unique_ptr<Expr> &&rhs) noexcept
     : Expr(lexer, EXPR_BIOP, pos), tokOp{tokOp},
       lhs(std::move(lhs)), rhs(std::move(rhs)) {
}

BiOpExpr::~BiOpExpr() {
}

std::string BiOpExpr::toString() const noexcept {
  switch (getOperator().getType()) {
  case TOK_OP_BRACKET_OPEN:
  case TOK_OP_ARR_BRACKET_OPEN:
  case TOK_OP_TEMPL_BRACKET_OPEN: {
    std::string result;
    switch (getOperator().getType()) {
      case TOK_OP_BRACKET_OPEN:
        result += '(';
        break;
      case TOK_OP_ARR_BRACKET_OPEN:
        result += '[';
        break;
      case TOK_OP_TEMPL_BRACKET_OPEN:
        result += '{';
        break;
      default:
        fatal("expr.cpp", __LINE__, "Unexpected branch reached");
        break;
    }
    result += getLeft().toString();
    result += ' ';
    std::string args;
    const Expr * expr = &getRight();
    while (isBiOpExpr(*expr, TOK_OP_COMMA)) {
      const BiOpExpr * biopexpr = dynamic_cast<const BiOpExpr*>(expr);
      args = ' ' + biopexpr->getRight().toString() + args;
      expr = &biopexpr->getLeft();
    }

    args = expr->toString() + args;
    result += args;
    switch (getOperator().getType()) {
      case TOK_OP_BRACKET_OPEN:
        result += ')';
        break;
      case TOK_OP_ARR_BRACKET_OPEN:
        result += ']';
        break;
      case TOK_OP_TEMPL_BRACKET_OPEN:
        result += '}';
        break;
      default:
        fatal("expr.cpp", __LINE__, "Unexpected branch reached");
        break;
    }

    return result;
  }
  default:
    return "(" + getOperator().toString(getLexer()) + " " +
      getLeft().toString() + " " + getRight().toString() + ")";
  }
}

// UnOpExpr
UnOpExpr::UnOpExpr(const Lexer &lexer, const Position &pos,
     const Token *tokOp, std::unique_ptr<Expr> &&expr) noexcept
     : Expr(lexer, EXPR_UNOP, pos), tokOp{tokOp}, expr(std::move(expr)) {
}

UnOpExpr::~UnOpExpr() {
}

std::string UnOpExpr::toString() const noexcept {
  if (getOperator().getType() == TOK_OP_BRACKET_OPEN) {
    return "(" + getExpression().toString() + ")";
  }

  return "(" + getOperator().toString(getLexer()) +
    " " + getExpression().toString() + ")";
}

// BodyExpr
BodyExpr::BodyExpr(const Lexer &lex, const Position &pos,
     Exprs &&exprs, std::unique_ptr<Expr> &&retExpr) noexcept
     : Expr(lex, EXPR_BODY, pos), exprs(std::move(exprs)),
       retExpr(std::move(retExpr)) {
}

BodyExpr::~BodyExpr() {
}

std::string BodyExpr::toString() const noexcept {
  std::string result;
  for (auto &expr : exprs) {
    result += expr->toString();
    result += '\n';
  }

  if (retExpr) {
    result += "return ";
    result += retExpr->toString();
    result += '\n';
  }

  return result;
}

// ArrayCpyExpr
ArrayCpyExpr::ArrayCpyExpr(const Lexer &lexer, const Position &pos,
     std::unique_ptr<Expr> &&valuExpr,
     std::unique_ptr<Expr> &&lengthExpr) noexcept
     : Expr(lexer, EXPR_ARRCPY, pos), valueExpr(std::move(valuExpr)),
       lengthExpr(std::move(lengthExpr)) {
}

 ArrayCpyExpr::~ArrayCpyExpr() {
}

std::string ArrayCpyExpr::toString() const noexcept {
  // TODO
  return "";
}

// ArrayLitExpr
ArrayLitExpr::ArrayLitExpr(const Lexer &lexer, const Position &pos,
     std::unique_ptr<Expr> &&expr0,
     std::unique_ptr<Expr> &&expr1,
     Exprs &&exprs) noexcept
     : Expr(lexer, EXPR_ARRLIT, pos),
       exprs(std::move(exprs)) {
  exprs.insert(exprs.begin(), std::move(expr1));
  exprs.insert(exprs.begin(), std::move(expr0));
}

ArrayLitExpr::~ArrayLitExpr() {
}

std::string ArrayLitExpr::toString() const noexcept {
  // TODO
  return "";
}

// ArrayEmptyExpr
ArrayEmptyExpr::ArrayEmptyExpr(const Lexer &lexer, const Position &pos,
     std::unique_ptr<Expr> &&typeExpr) noexcept
     : Expr(lexer, EXPR_ARREMPTY, pos), typeExpr(std::move(typeExpr)) {
}

ArrayEmptyExpr::~ArrayEmptyExpr() {
}


std::string ArrayEmptyExpr::toString() const noexcept {
  // TODO
  return "";
}

// ErrorExpr
std::string ErrorExpr::toString() const noexcept {
  // TODO
  return "";
}
