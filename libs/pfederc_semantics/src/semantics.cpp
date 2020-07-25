#include "pfederc/semantics.hpp"
using namespace pfederc;

// CtxId
const Context *CtxId::commonParent(const CtxId &ctxid) const noexcept {
  Context *result = ctx;
  for (; !ctxid.ctx->isParent(*result); result = ctx->getParent());
  
  return result;
}

bool CtxId::operator < (const CtxId &ctxid) const noexcept {
  if (ctx == ctxid.ctx) {
    return id < ctxid.id;
  }

  const auto *cmnParent = commonParent(ctxid);
  const auto *ctx0 = cmnParent->getChildContextIdWithChild(getContext());
  const auto *ctx1 = cmnParent->getChildContextIdWithChild(ctxid.getContext());
  return ctx0->getIdentifier() < ctx1->getIdentifier();
}

std::list<Symbol*> CtxId::getSymbols(const std::string &name, bool fromtop) noexcept {
  std::lock_guard<std::recursive_mutex> guard(mtxSymbols);

  std::list<Symbol*> result{};

  auto it = symbols.find(name);
  if (it == symbols.end())
    return result;

  auto &lst = it->second;
  for (auto &e : lst) {
    if (!e->is(*this, ST_HIDDEN) || !fromtop)
      result.push_back(e.get());
  }

  return result;
}

std::list<const Symbol*> CtxId::getSymbols(const std::string &name, bool fromtop) const noexcept {
  std::lock_guard<std::recursive_mutex> guard(mtxSymbols);

  std::list<const Symbol*> result{};

  auto it = symbols.find(name);
  if (it == symbols.end())
    return result;

  const auto &lst = it->second;
  for (auto &e : lst) {
    if (!e->is(*this, ST_HIDDEN) || !fromtop)
      result.push_back(e.get());
  }

  return result;
}

std::unique_ptr<Symbol> CtxId::addSymbol(std::unique_ptr<Symbol> &&sym) noexcept {
  std::lock_guard<std::recursive_mutex> guard(mtxSymbols);

  std::list<std::unique_ptr<Symbol>> &lst = sym->getName() ?
    symbols[*sym->getName()] : unnamedSymbols;

  auto it = std::find_if(lst.begin(), lst.end(),
      [&sym] (std::unique_ptr<Symbol> &e) {
        return *e == *sym;
      });

  // Element, that is about the same already exists in the list
  if (it != lst.end())
    return std::move(sym);

  lst.push_back(std::move(sym));

  return nullptr;
}

// Context
bool Context::isParent(const Context &ctx) const noexcept {
  const Context *currentctx = getParent();
  while (currentctx) {
    if (*currentctx == ctx)
      return true;
  }

  return false;
}

const CtxId *Context::getChildContextIdWithChild(const Context &child) const noexcept {
  for (auto &currentChild : children) {
    if (!currentChild->isContext())
      continue;

    if (dynamic_cast<Context*>(currentChild.get())->isParent(child))
      return currentChild.get();
  }

  return nullptr;
}

CtxId *Context::createCtxId() noexcept {
  auto result = std::make_unique<CtxId>(this, createId());
  auto resultPtr = result.get();
  children.push_back(std::move(result));
  return resultPtr;
}

Context *Context::createContext() noexcept {
  auto result = std::unique_ptr<Context>(new Context(this, createId()));
  auto resultPtr = result.get();
  children.push_back(std::move(result));
  return resultPtr;
}

// SymbolHistoryType
uint64_t SymbolHistoryType::computeType(const CtxId &id, const uint64_t baseSymType) const noexcept {
  uint64_t result = baseSymType;
  for (const auto &ctxid : getChangesContextIds()) {
    if (*ctxid > id)
      break;

    const auto &typeChange = getChanges().at(ctxid);
    if (typeChange.invert)
      result &= ~typeChange.changeType;
    else
      result |= typeChange.changeType;
  }

  return result;
}

// Symbol
Symbol::Symbol(uint64_t type, uint64_t internalId) noexcept
  : type{type}, internalId{internalId}, symhisType() {
}

Symbol::~Symbol() {
}

bool Symbol::is(const CtxId &id, uint64_t types) const noexcept {
  return (symhisType.computeType(id, this->type) & types) == types;
}

bool Symbol::operator ==(const Symbol &sym) const noexcept {
  if (!getName() || !sym.getName())
      return this == &sym;

  return *getName() == *sym.getName();
}

Context *Symbol::getContext() noexcept {
  return nullptr;
}

const Context *Symbol::getContext() const noexcept {
  return nullptr;
}

// NullSymbol
NullSymbol::NullSymbol() noexcept
  : Symbol(ST_NULL) {
}

NullSymbol::~NullSymbol() {
}

// AliasSymbol
AliasSymbol::AliasSymbol(const std::string &name, Symbol *symtype) noexcept
  : Symbol(ST_ALIAS | ST_NAMED), name(name), symtype{symtype} {
}

AliasSymbol::~AliasSymbol() {
}

Symbol *AliasSymbol::getType() noexcept {
  return symtype;
}

const Symbol *AliasSymbol::getType() const noexcept {
  return symtype;
}

const std::string *AliasSymbol::getName() const noexcept {
  return &name;
}

bool AliasSymbol::operator ==(const Symbol &sym) const noexcept {
  return getType() == sym.getType();
}

// TemplateSymbol
TemplateSymbol::TemplateSymbol(uint64_t type,
    std::list<AliasSymbol*> &&templSyms,
    uint64_t internalId) noexcept
  : Symbol(type | (templSyms.empty() ? 0x00 : ST_TEMPLATE), internalId),
    templSyms(std::move(templSyms)) {
}

TemplateSymbol::~TemplateSymbol() {
}

// LambdaSymbol
LambdaSymbol::LambdaSymbol(uint64_t type,
        std::list<std::unique_ptr<FunctionParamSymbol>> &&params,
        Symbol *returnValue,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId) noexcept
  : TemplateSymbol(type, std::move(templSyms), internalId),
    params(std::move(params)), returnValue{returnValue} {
}

LambdaSymbol::~LambdaSymbol() {
}

Symbol *LambdaSymbol::getType() noexcept {
  return returnValue;
}

const Symbol *LambdaSymbol::getType() const noexcept {
  return returnValue;
}

// FunctionSymbol
FunctionSymbol::FunctionSymbol(uint64_t type,
    const std::string &name,
    std::list<std::unique_ptr<FunctionParamSymbol>> &&params,
    Symbol *returnValue,
    std::list<AliasSymbol*> &&templSyms,
    uint64_t internalId) noexcept 
  : LambdaSymbol(type | ST_FUNCTION | ST_NAMED, std::move(params), returnValue,
                 std::move(templSyms), internalId),
    name(name) {
}

FunctionSymbol::FunctionSymbol(uint64_t type,
    std::string &&name,
    std::list<std::unique_ptr<FunctionParamSymbol>> &&params,
    Symbol *returnValue,
    std::list<AliasSymbol*> &&templSyms,
    uint64_t internalId) noexcept
  : LambdaSymbol(type | ST_FUNCTION | ST_NAMED, std::move(params), returnValue,
                 std::move(templSyms), internalId),
    name(std::move(name)) {
}

FunctionSymbol::FunctionSymbol(uint64_t type,
    std::string &&name,
    std::list<std::unique_ptr<FunctionParamSymbol>> &&params,
    Symbol *returnValue,
    uint64_t internalId) noexcept
  : LambdaSymbol(type | ST_FUNCTION | ST_NAMED, std::move(params), returnValue,
                 std::list<AliasSymbol*>(), internalId),
    name(std::move(name)) {
}

FunctionSymbol::~FunctionSymbol() {
}

Symbol *FunctionSymbol::getType() noexcept {
  return this;
}

const Symbol *FunctionSymbol::getType() const noexcept {
  return this;
}

const std::string *FunctionSymbol::getName() const noexcept {
  return &name;
}

Symbol *FunctionSymbol::resolveTemplate(std::list<AliasSymbol*> &&templSyms) noexcept {
  // TODO
  return nullptr;
}

// ClassSymbol
ClassSymbol::ClassSymbol(uint64_t type,
    Context *ctx,
    const std::string *name,
    std::list<AliasSymbol*> &&templSyms,
    uint64_t internalId) noexcept
  : TemplateSymbol(type | ST_CLASS, std::move(templSyms), internalId),
    ctx{ctx},
    name(name ? std::make_unique<std::string>(*name) : nullptr) {
  if (this->getName())
    this->type |= ST_NAMED;
}

ClassSymbol::ClassSymbol(uint64_t type,
    Context *ctx,
    std::string &&name,
    std::list<AliasSymbol*> &&templSyms,
    uint64_t internalId) noexcept
  : TemplateSymbol(type | ST_CLASS | ST_NAMED, std::move(templSyms), internalId),
    ctx{ctx},
    name(std::make_unique<std::string>(std::move(name))) {
}

ClassSymbol::ClassSymbol(uint64_t type,
    Context *ctx,
    std::string &&name,
    uint64_t internalId) noexcept
  : TemplateSymbol(type | ST_CLASS | ST_NAMED, std::list<AliasSymbol*>(), internalId),
    ctx{ctx},
    name(std::make_unique<std::string>(std::move(name))) {
}

ClassSymbol::~ClassSymbol() {
}

Symbol *ClassSymbol::getType() noexcept {
  return this;
}

const Symbol *ClassSymbol::getType() const noexcept {
  return this;
}

const std::string *ClassSymbol::getName() const noexcept {
  return name.get();
}

Context *ClassSymbol::getContext() noexcept {
  return ctx;
}

const Context *ClassSymbol::getContext() const noexcept {
  return ctx;
}


Context *ClassSymbol::getObjectContext() noexcept {
  return objctx;
}

const Context *ClassSymbol::getObjectContext() const noexcept {
  return objctx;
}

Symbol *ClassSymbol::resolveTemplate(std::list<AliasSymbol*> &&templSyms) noexcept {
  // TODO
  return nullptr;
}

// TraitSymbol
TraitSymbol::TraitSymbol(uint64_t type,
    Context *ctx,
    const std::string &name,
    std::list<AliasSymbol*> &&templSyms,
    uint64_t internalId) noexcept
  : TemplateSymbol(type | ST_TRAIT | ST_NAMED, std::move(templSyms), internalId),
      ctx{ctx}, name(name) {
}

TraitSymbol::TraitSymbol(uint64_t type,
    Context *ctx,
    std::string &&name,
    std::list<AliasSymbol*> &&templSyms,
    uint64_t internalId) noexcept
  : TemplateSymbol(type | ST_TRAIT | ST_NAMED, std::move(templSyms), internalId),
      ctx{ctx}, name(std::move(name)) {
}

TraitSymbol::TraitSymbol(uint64_t type,
    Context *ctx,
    std::string &&name,
    uint64_t internalId) noexcept
  : TemplateSymbol(type | ST_TRAIT | ST_NAMED, std::list<AliasSymbol*>(), internalId),
      ctx{ctx}, name(std::move(name)) {
}

TraitSymbol::~TraitSymbol() {
}

Symbol *TraitSymbol::getType() noexcept {
  return this;
}

const Symbol *TraitSymbol::getType() const noexcept {
  return this;
}

const std::string *TraitSymbol::getName() const noexcept {
  return &name;
}

Context *TraitSymbol::getContext() noexcept {
  return ctx;
}

const Context *TraitSymbol::getContext() const noexcept {
  return ctx;
}

Symbol *TraitSymbol::resolveTemplate(std::list<AliasSymbol*> &&templSyms) noexcept {
  // TODO
  return nullptr;
}

// EnumSymbol
EnumSymbol::EnumSymbol(uint64_t type,
    const std::string &name,
    Context *ctx,
    std::list<AliasSymbol*> &&templSyms,
    uint64_t internalId) noexcept
  : TemplateSymbol(type | ST_ENUM | ST_NAMED, std::move(templSyms), internalId),
    name(name), ctx{ctx} {
}

EnumSymbol::EnumSymbol(uint64_t type,
   std::string &&name,
    Context *ctx,
    std::list<AliasSymbol*> &&templSyms,
    uint64_t internalId) noexcept
  : TemplateSymbol(type | ST_ENUM, std::move(templSyms), internalId),
    name(std::move(name)), ctx{ctx} {
}

EnumSymbol::EnumSymbol(uint64_t type, std::string &&name,
    Context *ctx,
    uint64_t internalId) noexcept
  : TemplateSymbol(type | ST_ENUM, std::list<AliasSymbol*>(), internalId),
    name(std::move(name)), ctx{ctx} {
}

EnumSymbol::~EnumSymbol() {
}

Symbol *EnumSymbol::getType() noexcept {
  return this;
}

const Symbol *EnumSymbol::getType() const noexcept {
  return this;
}

const std::string *EnumSymbol::getName() const noexcept {
  return &name;
}

Symbol *EnumSymbol::resolveTemplate(std::list<AliasSymbol*> &&templSyms) noexcept {
  // TODO
  return nullptr;
}

// ObjectSymbol
ObjectSymbol::ObjectSymbol(uint64_t type, Symbol *objType,
    uint64_t internalId) noexcept
  : Symbol(type | ST_OBJECT, internalId),
    objType{objType} {
}

ObjectSymbol::~ObjectSymbol() {
}

Symbol *ObjectSymbol::getType() noexcept {
  return objType;
}

const Symbol *ObjectSymbol::getType() const noexcept {
  return objType;
}

Context *ObjectSymbol::getContext() noexcept {
  return objType->getContext();
}

const Context *ObjectSymbol::getContext() const noexcept {
  return objType->getContext();
}

// VariableSymbol
VariableSymbol::VariableSymbol(uint64_t type,
    const std::string &name, Symbol *objType,
    uint64_t internalId) noexcept
  : ObjectSymbol(type | ST_VARIABLE | ST_NAMED, objType, internalId),
    name(name) {
}

VariableSymbol::VariableSymbol(uint64_t type,
    std::string &&name, Symbol *objType,
    uint64_t internalId) noexcept
  : ObjectSymbol(type | ST_VARIABLE | ST_NAMED, objType, internalId),
    name(std::move(name)) {
}

VariableSymbol::~VariableSymbol() {
}

const std::string *VariableSymbol::getName() const noexcept {
  return &name;
}

// NamespaceSymbol
NamespaceSymbol::NamespaceSymbol(uint64_t type,
    const std::string &name, Context *ctx) noexcept
  : Symbol(type | ST_MODULE | ST_NAMED), name(name), ctx{ctx} {
}

NamespaceSymbol::~NamespaceSymbol() {
}

const std::string *NamespaceSymbol::getName() const noexcept {
  return &name;
}

Context *NamespaceSymbol::getContext() noexcept {
  return ctx;
}

const Context *NamespaceSymbol::getContext() const noexcept {
  return ctx;
}
