#include "pfederc/expr.hpp"
using namespace pfederc;

template<class T>
inline static void _setParent(std::unique_ptr<T> &expr, Expr *parent) noexcept {
  if (!!expr)
    expr->setParent(parent);
}

template<class T>
static void _setParents(std::list<std::unique_ptr<T>> &exprs, Expr *parent) noexcept {
  for (std::unique_ptr<T> &expr : exprs) {
    expr->setParent(parent);
  }
}

template<class T>
static void _setParents(std::vector<std::unique_ptr<T>> &exprs, Expr *parent) noexcept {
  for (std::unique_ptr<T> &expr : exprs) {
    expr->setParent(parent);
  }
}

static void _setParameterParents(std::vector<std::unique_ptr<FuncParameter>> &params, Expr *parent) noexcept {
  for (auto &param : params) {
    std::get<2>(*param)->setParent(parent);
    _setParent(std::get<3>(*param), parent);
    _setParent(std::get<4>(*param), parent);
  }
}

static void _setTemplateParents(std::unique_ptr<TemplateDecls> &templs, Expr *parent) noexcept {
  if (!templs)
    return;

  for (auto &templ : *(templs)) {
    _setParent(templ.expr, parent);
  }
}

static std::string _templateToString(const TemplateDecls &tmpls) {
  std::string result;
  result += '{';

  bool first = true;
  for (const TemplateDecl &tmpl : tmpls) {
    if (!first)
      result += ", ";
    else
      first = false;

    const auto &snd = *tmpl.expr;
    result += tmpl.id->toString(snd.getLexer());
    result += ": ";
    result += snd.toString();
  }

  result += '}';
  return result;
}

static std::string _capsToString(const Capabilities &caps) noexcept {
  std::string result;
  if (caps.isUnused())
    result += "#Unused\n";
  if (caps.isInline())
    result += "#Inline\n";
  if (caps.isConstant())
    result += "#Constant\n";

  for (auto &require : caps.getRequires()) {
    result += "#!requires ";
    result += require->toString();
    result += '\n';
  }

  for (auto &ensure : caps.getEnsures()) {
    result += "#!ensures ";
    result += ensure->toString();
    result += '\n';
  }

  return result;
}

// Expr
Expr::Expr(const Lexer &lexer, ExprType type, const Position &pos) noexcept
    : parent{nullptr}, lexer{lexer}, type{type}, pos(pos) {

}

Expr::~Expr() {
}

// Capabilities
Capabilities::Capabilities(bool isunused, bool isinline, bool isconstant,
    std::vector<std::unique_ptr<Expr>> &&required,
    std::vector<std::unique_ptr<Expr>> &&ensures) noexcept
    : isunused{isunused}, isinline{isinline}, isconstant{isconstant},
      required(std::move(required)), ensures(std::move(ensures)) {}

Capabilities::~Capabilities() {
}

// ProgramExpr
ProgramExpr::ProgramExpr(const Lexer &lexer, const Position &pos,
    const Token *progName,
    Exprs &&imports,
    Exprs &&defs) noexcept
    : Expr(lexer, ExprType::EXPR_PROG, pos),
      progName{progName},
      imports(std::move(imports)),
      defs(std::move(defs)) {
  
  _setParents(imports, this);
  _setParents(defs, this);
}

ProgramExpr::~ProgramExpr() {
}

std::string ProgramExpr::toString() const noexcept {
  std::string result;
  if (progName)
    result += "use mod " + progName->toString(getLexer()) + '\n';

  for (auto &expr : imports)
    result += expr->toString() + '\n';

  for (auto &expr : defs)
    result += expr->toString() + '\n';

  return result;
}

// TokenExpr
TokenExpr::TokenExpr(const Lexer &lexer, Token *tok) noexcept
    : Expr(lexer, ExprType::EXPR_TOK, tok->getPosition()), tok{tok} {
  assert(this->tok);
}

TokenExpr::~TokenExpr() {

}

std::string TokenExpr::toString() const noexcept {
  return getToken().toString(getLexer());
}

// UseExpr
UseExpr::UseExpr(const Lexer &lexer, const Position &pos,
    Exprs &&exprs) noexcept
    : Expr(lexer, ExprType::EXPR_USE, pos), exprs(std::move(exprs)) {
  assert(!this->exprs.empty());

  _setParents(this->exprs, this);
}

UseExpr::~UseExpr() {

}

std::string UseExpr::toString() const noexcept {
  std::string result = "use ";
  for (auto it = exprs.begin(); it != exprs.end(); ++it) {
    if (it != exprs.begin())
      result += ", ";
      
    result += (*it)->toString();
  }

  return result;
}

// ProgNameExpr
ProgNameExpr::ProgNameExpr(const Lexer &lexer, const Token *tok) noexcept
    : Expr(lexer, ExprType::EXPR_PROGNAME, tok->getPosition()), tok{tok} {
}

ProgNameExpr::~ProgNameExpr() {
}

std::string ProgNameExpr::toString() const noexcept {
  return "use module " + getToken().toString(getLexer());
}

// FuncExpr
FuncExpr::FuncExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Capabilities> &&caps,
    const Token *tokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::vector<std::unique_ptr<FuncParameter>> &&params,
    std::unique_ptr<Expr> &&returnExpr,
    std::unique_ptr<BodyExpr> &&body,
    bool autoDetectReturnType) noexcept
    : Expr(lexer, ExprType::EXPR_FUNC, pos), Capable(std::move(caps)), tokId{tokId},
      templs(std::move(templs)), params(std::move(params)),
      returnExpr(std::move(returnExpr)), body(std::move(body)),
      autoDetectReturnType{autoDetectReturnType} {
  assert(this->tokId);
  assert((this->autoDetectReturnType && !this->returnExpr) || !this->autoDetectReturnType);

  _setTemplateParents(this->templs, this);
  _setParameterParents(this->params, this);
  _setParent(this->returnExpr, this);
  _setParent(this->body, this);
}

FuncExpr::~FuncExpr() {
}

std::string FuncExpr::toString() const noexcept {
  std::string result;
  if (caps)
    result += _capsToString(*caps);

  result += "func";
  if (templs)
    result += _templateToString(*templs);

  result += ' ';
  result += getIdentifier().toString(getLexer());
  if (!params.empty()) {
    result += '(';
    for (auto it = params.begin(); it != params.end(); ++it) {
      if (it != params.begin())
        result += ", ";
      
      const auto &tpl = **it;
			if (std::get<1>(tpl))
				result += '&';

      if (std::get<0>(tpl)) {
        result += std::get<0>(tpl)->toString(getLexer());
        result += ": ";
      }

      result += std::get<2>(tpl)->toString();
      if (std::get<3>(tpl)) {
        result += " | ";
        result += std::get<2>(tpl)->toString();
        if (std::get<4>(tpl)) {
          result += " = ";
          result += std::get<4>(tpl)->toString();
        }
      }
    }
    result += ')';
  }

  if (returnExpr) {
    result += ": ";
    result += returnExpr->toString();
  } else if (autoDetectReturnType) {
    result += ':';
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
    : Expr(lexer, ExprType::EXPR_FUNCTYPE, pos),
      params(std::move(params)), returnExpr(std::move(returnExpr)) {

  _setParameterParents(this->params, this);
  _setParent(returnExpr, this);
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
    : Expr(lexer, ExprType::EXPR_LAMBDA, pos),
      params(std::move(params)), body(std::move(body)) {
  assert(!!this->body);

  _setParents(this->params, this);
  _setParent(this->body, this);
}

LambdaExpr::~LambdaExpr() {
}

std::string LambdaExpr::toString() const noexcept {
  std::string result = "lambda ";
  if (!params.empty()) {
    result += '(';
    for (auto it = params.begin(); it != params.end(); ++it) {
      if (it != params.begin())
        result += ", ";
      result += (*it)->toString();
    }
    result += ')';
  }

  result += '\n';

  result += body->toString();
  result += ';';
  return result;
}

// TraitExpr
TraitExpr::TraitExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Capabilities> &&caps,
    const Token *tokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::vector<std::unique_ptr<Expr>> &&impltraits,
    std::list<std::unique_ptr<FuncExpr>> &&functions) noexcept
    : Expr(lexer, ExprType::EXPR_TRAIT, pos), Capable(std::move(caps)), tokId{tokId},
      templs(std::move(templs)),
      impltraits(std::move(impltraits)), functions(std::move(functions)) {
  assert(this->tokId);

  _setTemplateParents(this->templs, this);
  _setParents(this->impltraits, this);
  _setParents(this->functions, this);
}

TraitExpr::~TraitExpr() {
}

std::string TraitExpr::toString() const noexcept {
  std::string result;
  if (caps)
    result += _capsToString(*caps);

  result += "trait";
  if (templs) {
    result += _templateToString(*templs);
  }

  result += ' ';
  result += getIdentifier().toString(getLexer());

  if (!impltraits.empty()) {
    result += ": ";
    bool first = true;
    for (auto &impltrait : impltraits) {
      if (!first)
        result += ", ";
      else
        first = false;

      result += impltrait->toString();
    }
  }

  result += '\n';
  for (auto &func : functions) {
    result += func->toString();
    result += '\n';
  }

  result += ';';

  return result;
}

// ClassExpr
ClassExpr::ClassExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Capabilities> &&caps,
    const Token *tokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::list<std::unique_ptr<BiOpExpr>> &&constructAttributes,
    std::list<std::unique_ptr<BiOpExpr>> &&attributes,
    std::list<std::unique_ptr<FuncExpr>> &&functions) noexcept
    : Expr(lexer, ExprType::EXPR_CLASS, pos), Capable(std::move(caps)), tokId{tokId},
      templs(std::move(templs)),
      constructAttributes(std::move(constructAttributes)),
      attributes(std::move(attributes)),
      functions(std::move(functions)) {
  assert(this->tokId);

  _setTemplateParents(this->templs, this);
  _setParents(this->constructAttributes, this);
  _setParents(this->attributes, this);
  _setParents(this->functions, this);
}

ClassExpr::~ClassExpr() {
}

std::string ClassExpr::toString() const noexcept {
  std::string result;
  if (caps)
    result += _capsToString(*caps);

  result += "class";
  if (templs) {
    result += _templateToString(*templs);
  }

  result += ' ';
  result += tokId->toString(getLexer());
  for (auto it = constructAttributes.begin();
      it != constructAttributes.end(); ++it) {
    if (it == constructAttributes.begin())
      result += '(';
    else
      result += ", ";

    result += (*it)->toString();
  }

  if (!constructAttributes.empty())
    result += ')';

  result += '\n';

  for (const auto &biopexpr : attributes) {
    result += biopexpr->toString();
    result += '\n';
  }

  for (const auto &function : functions) {
    result += function->toString();
    result += '\n';
  }

  result += ';';

  return result;
}

// TraitImplExpr
TraitImplExpr::TraitImplExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Capabilities> &&caps,
    const Token *classTokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::unique_ptr<Expr> &&implTrait,
    std::list<std::unique_ptr<FuncExpr>> &&functions) noexcept
    : Expr(lexer, ExprType::EXPR_TRAITIMPL, pos), Capable(std::move(caps)), classTokId{classTokId},
      templs(std::move(templs)),
      implTrait(std::move(implTrait)), functions(std::move(functions)) {
  assert(this->classTokId);
  assert(!!this->implTrait);

  _setTemplateParents(this->templs, this);
  this->implTrait->setParent(this);
  _setParents(this->functions, this);
}

TraitImplExpr::~TraitImplExpr() {
}

std::string TraitImplExpr::toString() const noexcept {
  std::string result;
  if (caps)
    result += _capsToString(*caps);

  result += "class trait";
  if (templs)
    result += _templateToString(*templs);

  result += ' ';

  result += classTokId->toString(getLexer());
  result += ": ";
  result += implTrait->toString();
  result += '\n';

  for (auto &func : functions) {
    result += func->toString();
    result += '\n';
  }

  result += ';';

  return result;
}

// EnumExpr
EnumExpr::EnumExpr(const Lexer &lexer, const Position &pos,
    const Token *tokId,
    std::unique_ptr<TemplateDecls> &&templs,
    std::vector<EnumConstructor> &&constructors) noexcept
    : Expr(lexer, ExprType::EXPR_ENUM, pos), tokId{tokId},
      templs(std::move(templs)), constructors(std::move(constructors)) {
  assert(this->tokId);

  _setTemplateParents(this->templs, this);

  for (auto &construct : this->constructors) {
    _setParents(std::get<1>(construct), this);
  }
}

EnumExpr::~EnumExpr() {
}

std::string EnumExpr::toString() const noexcept {
  std::string result = "enum";
  if (templs)
    result += _templateToString(*templs);

  result += ' ';
  result += tokId->toString(getLexer());
  result += '\n';

  for (auto &constructor : constructors) {
    result += std::get<0>(constructor)->toString(getLexer());
    auto &params = std::get<1>(constructor);
    if (!params.empty()){
      result += '(';
      bool first = true;
      for (auto &param : params) {
        if (!first)
          result += ", ";
        else
          first = false;

        result += param->toString();
      }

      result += ')';
    }

    result += '\n';
  }

  result += ';';

  return result;
}

// TypeExpr
TypeExpr::TypeExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Capabilities> &&caps,
    const Token *tokId, std::unique_ptr<Expr> &&expr) noexcept
  : Expr(lexer, ExprType::EXPR_TYPE, pos), Capable(std::move(caps)),
    tokId{tokId}, expr(std::move(expr)) {
  assert(this->tokId);
  assert(!!this->expr);

  this->expr->setParent(this);
}

TypeExpr::~TypeExpr() {
}

std::string TypeExpr::toString() const noexcept {
  std::string result;
  if (caps)
    result += _capsToString(*caps);
  result += "type " + expr->toString() + ' ' + tokId->toString(expr->getLexer());
  return result;
}

// ModExpr
ModExpr::ModExpr(const Lexer &lexer, const Position &pos,
    const Token *tokId, Exprs &&exprs) noexcept
    : Expr(lexer, ExprType::EXPR_MOD, pos), tokId{tokId}, exprs(std::move(exprs)) {
  assert(this->tokId);

  _setParents(this->exprs, this);
}

ModExpr::~ModExpr() {
}

std::string ModExpr::toString() const noexcept {
  std::string result = "module " + getIdentifier().toString(getLexer()) + '\n';
  for (const auto &expr : exprs) {
    result += expr->toString();
    result += '\n';
  }
  result += ';';
  return result;
}

// SafeExpr
SafeExpr::SafeExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Expr> &&expr) noexcept
    : Expr(lexer, ExprType::EXPR_SAFE, pos), expr(std::move(expr)) {
  assert(!!this->expr);

  this->expr->setParent(this);
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
    : Expr(lexer, ExprType::EXPR_IF, pos), ifCases(std::move(ifCases)),
      elseExpr(std::move(elseExpr)), isensure{isensure} {
  assert(!this->ifCases.empty());

  for (auto &ifcase : this->ifCases) {
    std::get<0>(ifcase)->setParent(this);
    std::get<1>(ifcase)->setParent(this);
  }

  _setParent(this->elseExpr, this);
}

IfExpr::~IfExpr() {
}

std::string IfExpr::toString() const noexcept {
  std::string result;

  if (isEnsure())
    result += "ensure ";
  else
    result += "if ";

  result += std::get<0>(ifCases[0])->toString() + '\n';
  result += std::get<1>(ifCases[0])->toString();

  for (size_t i = 1; i < ifCases.size(); ++i) {
    result += "else ";
    if (isEnsure())
      result += "ensure ";
    else
      result += "if ";

    result += std::get<0>(ifCases[i])->toString() + '\n';
    result += std::get<1>(ifCases[i])->toString();
  }

  if (getElse()) {
    result += "else\n";
    result += getElse()->toString();
  }

  result += ';';

  return result;
}

// LoopExpr
LoopExpr::LoopExpr(const Lexer &lexer, ExprType type, const Position &pos,
    std::unique_ptr<Expr> &&initExpr,
    std::unique_ptr<Expr> &&condExpr,
    std::unique_ptr<Expr> &&itExpr,
    std::unique_ptr<BodyExpr> &&bodyExpr) noexcept
    : Expr(lexer, type, pos), initExpr(std::move(initExpr)),
      condExpr(std::move(condExpr)), itExpr(std::move(itExpr)),
      bodyExpr(std::move(bodyExpr)) {
  assert(this->condExpr);
  assert((!!this->initExpr && !!this->itExpr) || !this->initExpr);
  assert(!!this->bodyExpr);

  _setParent(this->initExpr, this);
  this->condExpr->setParent(this);
  _setParent(this->itExpr, this);

  _setParent(this->bodyExpr, this);
}

LoopExpr::~LoopExpr() {
}

std::string LoopExpr::toString() const noexcept {
  std::string result;
  if (getType() == ExprType::EXPR_LOOP_DO) {
    result += "do";
    if (initExpr) {
      result += ' ';
      result += initExpr->toString();
    }
  } else {
    result += "for ";
    if (initExpr) {
      result += initExpr->toString();
      result += "; ";
    }
    result += condExpr->toString();
    if (itExpr) {
      result += "; ";
      result += itExpr->toString();
    }
  }

  result += '\n';
  result += bodyExpr->toString();
  result += ';';

  if (getType() == ExprType::EXPR_LOOP_DO) {
    result += " for ";
    result += condExpr->toString();
    if (itExpr) {
      result += "; ";
      result += itExpr->toString();
    }
  }

  return result;
}

// MatchExpr
MatchExpr::MatchExpr(const Lexer &lexer, const Position &pos,
    std::unique_ptr<Expr> &&expr,
    std::vector<MatchPattern> &&cases,
    std::unique_ptr<BodyExpr> &&anyCase) noexcept
    : Expr(lexer, ExprType::EXPR_MATCH, pos), expr(std::move(expr)),
      cases(std::move(cases)),
      anyCase(std::move(anyCase)) {
  assert(!!this->expr);

  this->expr->setParent(this);
  for (auto &matchp : this->cases) {
    std::get<2>(matchp)->setParent(this);
  }

  _setParent(this->anyCase, this);
}

MatchExpr::~MatchExpr() {
}

std::string MatchExpr::toString() const noexcept {
  std::string result = "match ";
  result += getExpression().toString();
  result += '\n';
  for (auto &matchcase : cases) {
    result += std::get<0>(matchcase)->toString(getLexer());
    if (!std::get<1>(matchcase).empty()) {
      result += '(';
      bool first = true;
      for (auto &var : std::get<1>(matchcase)) {
        if (!first)
          result += ", ";
        else
          first = false;

        result += var->toString(getLexer());
      }
      result += ')';
    }

    result += " => ";

    result += std::get<2>(matchcase)->toString();
    result += ";\n";
  }

  if (anyCase) {
    result += "_ => ";
    result += anyCase->toString();
    result += ";\n";
  }

  result += ';';

  return result;
}


// BiOpExpr
BiOpExpr::BiOpExpr(const Lexer &lexer, const Position &pos,
     const Token *tokOp,
     TokenType opType,
     std::unique_ptr<Expr> &&lhs, std::unique_ptr<Expr> &&rhs) noexcept
     : Expr(lexer, ExprType::EXPR_BIOP, pos), tokOp{tokOp}, opType{opType},
       lhs(std::move(lhs)), rhs(std::move(rhs)) {
  assert(this->tokOp);
  assert(isTokenTypeOperator(this->opType));
  assert(!!this->lhs);
  assert(!!this->rhs);

  this->lhs->setParent(this);
  this->rhs->setParent(this);
}

BiOpExpr::~BiOpExpr() {
}

std::string BiOpExpr::toString() const noexcept {
  switch (getOperatorType()) {
  case TokenType::TOK_OP_BRACKET_OPEN:
  case TokenType::TOK_OP_ARR_BRACKET_OPEN:
  case TokenType::TOK_OP_TEMPL_BRACKET_OPEN: {
    std::string result;
    switch (getOperatorType()) {
      case TokenType::TOK_OP_BRACKET_OPEN:
        result += '(';
        break;
      case TokenType::TOK_OP_ARR_BRACKET_OPEN:
        result += '[';
        break;
      case TokenType::TOK_OP_TEMPL_BRACKET_OPEN:
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
    while (isBiOpExpr(*expr, TokenType::TOK_OP_COMMA)) {
      const BiOpExpr * biopexpr = dynamic_cast<const BiOpExpr*>(expr);
      args = ' ' + biopexpr->getRight().toString() + args;
      expr = &biopexpr->getLeft();
    }

    args = expr->toString() + args;
    result += args;
    switch (getOperatorType()) {
      case TokenType::TOK_OP_BRACKET_OPEN:
        result += ')';
        break;
      case TokenType::TOK_OP_ARR_BRACKET_OPEN:
        result += ']';
        break;
      case TokenType::TOK_OP_TEMPL_BRACKET_OPEN:
        result += '}';
        break;
      default:
        fatal("expr.cpp", __LINE__, "Unexpected branch reached");
        break;
    }

    return result;
  }
  case TokenType::TOK_OP_NONE:
    return "( " + getLeft().toString() + ' ' + getRight().toString() + ')';
  default:
    return '(' + getOperatorToken().toString(getLexer()) + ' ' +
      getLeft().toString() + ' ' + getRight().toString() + ')';
  }
}

// UnOpExpr
UnOpExpr::UnOpExpr(const Lexer &lexer, const Position &pos,
     const Token *tokOp, std::unique_ptr<Expr> &&expr) noexcept
     : Expr(lexer, ExprType::EXPR_UNOP, pos), tokOp{tokOp}, expr(std::move(expr)) {
  assert(this->tokOp);
  assert(isTokenTypeOperator(this->tokOp->getType()));
  assert(!!this->expr);

  this->expr->setParent(this);
}

UnOpExpr::~UnOpExpr() {
}

std::string UnOpExpr::toString() const noexcept {
  if (getOperatorType() == TokenType::TOK_OP_BRACKET_OPEN) {
    return '(' + getExpression().toString() + ')';
  }

  return '(' + getOperatorToken().toString(getLexer()) +
    ' ' + getExpression().toString() + ')';
}

// BodyExpr
BodyExpr::BodyExpr(const Lexer &lex, const Position &pos,
     Exprs &&exprs, std::unique_ptr<Expr> &&retExpr,
     ReturnControlType rct) noexcept
     : Expr(lex, ExprType::EXPR_BODY, pos), exprs(std::move(exprs)),
       retExpr(std::move(retExpr)), rct{rct} {
  
  _setParents(this->exprs, this);
  _setParent(this->retExpr, this);
}

BodyExpr::~BodyExpr() {
}

std::string BodyExpr::toString() const noexcept {
  std::string result;
  for (auto &expr : exprs) {
    result += expr->toString();
    result += '\n';
  }

  switch (rct) {
  case ReturnControlType::RETURN:
      result += "return";
      break;
  case ReturnControlType::CONTINUE:
      result += "continue";
      break;
  case ReturnControlType::BREAK:
      result += "break";
      break;
  default:
      break;
  };

  if (retExpr) {
    result += ' ';
    result += retExpr->toString();
  }

  if (rct != ReturnControlType::NONE)
    result += '\n';

  return result;
}

// ArrayCpyExpr
ArrayCpyExpr::ArrayCpyExpr(const Lexer &lexer, const Position &pos,
     std::unique_ptr<Expr> &&valueExpr,
     std::unique_ptr<Expr> &&lengthExpr) noexcept
     : Expr(lexer, ExprType::EXPR_ARRCPY, pos), valueExpr(std::move(valueExpr)),
       lengthExpr(std::move(lengthExpr)) {
  assert(!!this->valueExpr);
  assert(!!this->lengthExpr);

  this->valueExpr->setParent(this);
  this->lengthExpr->setParent(this);
}

ArrayCpyExpr::~ArrayCpyExpr() {
}

std::string ArrayCpyExpr::toString() const noexcept {
  return '[' + valueExpr->toString() + "; " + lengthExpr->toString() + ']';
}

// ArrayLitExpr
ArrayLitExpr::ArrayLitExpr(const Lexer &lexer, const Position &pos,
     std::list<std::unique_ptr<Expr>> &&exprs) noexcept
     : Expr(lexer, ExprType::EXPR_ARRLIT, pos),
       exprs(std::move(exprs)) {
  assert(!this->exprs.empty());

  _setParents(this->exprs, this);
}

ArrayLitExpr::~ArrayLitExpr() {
}

std::string ArrayLitExpr::toString() const noexcept {
  std::string result = "[";
  for (auto it = exprs.begin(); it != exprs.end(); ++it) {
    if (it != exprs.begin())
      result += ", ";

    result += (*it)->toString();
  }

  result += ']';

  return result;
}

// ArrayEmptyExpr
ArrayEmptyExpr::ArrayEmptyExpr(const Lexer &lexer, const Position &pos,
     std::unique_ptr<Expr> &&typeExpr) noexcept
     : Expr(lexer, ExprType::EXPR_ARREMPTY, pos), typeExpr(std::move(typeExpr)) {
  assert(!!this->typeExpr);

  this->typeExpr->setParent(this);
}

ArrayEmptyExpr::~ArrayEmptyExpr() {
}


std::string ArrayEmptyExpr::toString() const noexcept {
  return '[' + typeExpr->toString() + ']';
}

// ErrorExpr
std::string ErrorExpr::toString() const noexcept {
  // TODO
  return "";
}
