#include "pfederc/expr.hpp"
using namespace pfederc;

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

// TokenExpr
TokenExpr::TokenExpr(const Lexer &lexer, const Token &tok) noexcept
    : Expr(lexer, EXPR_TOK, tok.getPosition()), tok{tok} {
}

TokenExpr::~TokenExpr() {

}

// UseExpr
UseExpr::UseExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Expr> &&expr) noexcept
    : Expr(lexer, EXPR_USE, pos), expr(std::move(expr)) {
}

UseExpr::~UseExpr() {

}

// FuncExpr
FuncExpr::FuncExpr(const Lexer &lexer, const Position &pos,
    const Token &tokId,
    Exprs &&templ,
    std::vector<FuncParameter> &&params,
    std::unique_ptr<Expr> &&returnExpr,
    std::unique_ptr<BodyExpr> &&body) noexcept
    : Expr(lexer, EXPR_FUNC, pos), tokId{tokId},
      templs(std::move(templs)), params(std::move(params)),
      returnExpr(std::move(returnExpr)), body(std::move(body)) {
}

 FuncExpr::~FuncExpr() {
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

// TraitExpr
TraitExpr::TraitExpr(const Lexer &lexer, const Position &pos,
    const Token &tokId,
    Exprs &&templs,
    std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept
    : Expr(lexer, EXPR_TRAIT, pos), tokId{tokId},
      templs(std::move(templs)), functions(std::move(functions)) {
}

 TraitExpr::~TraitExpr() {
}

// ClassExpr
ClassExpr::ClassExpr(const Lexer &lexer, const Position &pos,
    const Token &tokId,
    Exprs &&templs,
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

// TraitImplExpr
TraitImplExpr::TraitImplExpr(const Lexer &lexer, const Position &pos,
    const Token &classTokId, Exprs &&templs,
    std::unique_ptr<Expr> &&implTrait,
    std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept
    : Expr(lexer, EXPR_TRAITIMPL, pos), classTokId{classTokId},
      templs(std::move(templs)),
      implTrait(std::move(implTrait)), functions(std::move(functions)) {
}

 TraitImplExpr::~TraitImplExpr() {
}

// EnumExpr
EnumExpr::EnumExpr(const Lexer &lexer, const Position &pos,
    const Token &tokId,
    Exprs &&templs,
    EnumConstructor &&contructors) noexcept
    : Expr(lexer, EXPR_ENUM, pos), tokId{tokId},
      templs(std::move(templs)), constructors(std::move(constructors)) {
}

 EnumExpr::~EnumExpr() {
}

// ModExpr
ModExpr::ModExpr(const Lexer &lexer, const Position &pos,
    const Token &tokId, Exprs &&exprs) noexcept
    : Expr(lexer, EXPR_MOD, pos), tokId{tokId}, exprs(std::move(exprs)) {
}

ModExpr::~ModExpr() {
}

// SafeExpr
SafeExpr::SafeExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Expr> &&expr) noexcept
    : Expr(lexer, EXPR_SAFE, pos), expr(std::move(expr)) {
}

SafeExpr::~SafeExpr() {
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

// BiOpExpr
BiOpExpr::BiOpExpr(const Lexer &lexer, const Position &pos,
     const Token &tokOp,
     std::unique_ptr<Expr> &&lhs, std::unique_ptr<Expr> &&rhs) noexcept
     : Expr(lexer, EXPR_BIOP, pos), tokOp{tokOp},
       lhs(std::move(lhs)), rhs(std::move(rhs)) {
}

BiOpExpr::~BiOpExpr() {
}

// UnOpExpr
UnOpExpr::UnOpExpr(const Lexer &lexer, const Position &pos,
     const Token &tokOp, std::unique_ptr<Expr> &&expr) noexcept
     : Expr(lexer, EXPR_UNOP, pos), tokOp{tokOp}, expr(std::move(expr)) {
}

UnOpExpr::~UnOpExpr() {
}

// BodyExpr
BodyExpr::BodyExpr(const Lexer &lex, const Position &pos,
     Exprs &&exprs, std::unique_ptr<Expr> &&retExpr) noexcept
     : Expr(lex, EXPR_BODY, pos), exprs(std::move(exprs)),
       retExpr(std::move(retExpr)) {
}

BodyExpr::~BodyExpr() {
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

// ArrayEmptyExpr
ArrayEmptyExpr::ArrayEmptyExpr(const Lexer &lexer, const Position &pos,
     std::unique_ptr<Expr> &&typeExpr) noexcept
     : Expr(lexer, EXPR_ARREMPTY, pos), typeExpr(std::move(typeExpr)) {
}

 ArrayEmptyExpr::~ArrayEmptyExpr() {
 }
