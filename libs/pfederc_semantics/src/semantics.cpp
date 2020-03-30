#include "pfederc/semantics.hpp"
using namespace pfederc;

// Symbol

Symbol::Symbol(SymbolType type,
		std::unordered_map<SymbolStringType, std::string> &&strings) noexcept
	: type{type}, strings(std::move(strings)) {
	assert((bitmap::all(this->type, SYM_NAME) && !this->strings.empty())
			|| bitmap::none(this->type, SYM_NAME));
}

Symbol::~Symbol() {
}

bool Symbol::operator ==(const std::string& str) const noexcept {
	auto it = strings.find(SymbolStringType::NAME);
	if (it == strings.end())
		return false;

	return it->second == str;
}

// ObjectSymbol

ObjectSymbol::ObjectSymbol(SymbolType type, ObjectModifiers objmod,
		std::vector<Symbol*> &&types,
		std::unordered_map<SymbolStringType, std::string> &&strings) noexcept
	: Symbol(SYM_OBJ | type, std::move(strings)),
	  objmod{objmod}, types(std::move(types)) {
	assert(!this->types.empty());
}

ObjectSymbol::~ObjectSymbol() {
}

Symbol *ObjectSymbol::getTupleType(const std::size_t idx) const noexcept {
	assert(isTuple());
	assert(idx < getTypes().size());
	return getTypes().at(idx);
}

// VariableSymbol
VariableSymbol::VariableSymbol(SymbolType type,
		VariableModifiers varmod, ObjectModifiers objmod,
		std::vector<Symbol*> &&types,
		std::unordered_map<SymbolStringType, std::string> &&strings) noexcept
	: ObjectSymbol(SYM_VAR | type, objmod, std::move(types), std::move(strings)),
	  varmod{varmod} {
}

VariableSymbol::~VariableSymbol() {
}

// ObjectEnvSymbol
ObjectEnvSymbol::ObjectEnvSymbol(SymbolType type,
		std::unordered_map<SymbolStringType, std::string> &&strings) noexcept
	: Symbol(type, std::move(strings)) {
}

ObjectEnvSymbol::~ObjectEnvSymbol() {
}

void ObjectEnvSymbol::addSymbol(const std::string &name, Symbol *const sym) noexcept {
	std::lock_guard<std::mutex> lck(objenvsymsMtx);
	auto it = objenvsyms.find(name);
	if (it == objenvsyms.end())
		objenvsyms.insert(std::pair<std::string,std::list<Symbol*>>(
					name, std::list<Symbol*>{sym}));

	it->second.push_back(sym);
}

std::list<Symbol*> ObjectEnvSymbol::lookupInObject(const std::string &name) const noexcept {
	std::lock_guard<std::mutex> lck(objenvsymsMtx);
	auto it = objenvsyms.find(name);
	if (it == objenvsyms.end())
		return std::list<Symbol*>();

	return it->second;
}

// ClassSymbol
ClassSymbol::ClassSymbol(SymbolType type,
		std::unordered_map<SymbolStringType, std::string> &&strings) noexcept
		: ObjectEnvSymbol(type, std::move(strings)) {
}

ClassSymbol::~ClassSymbol() {
}

// TraitSymbol
TraitSymbol::TraitSymbol(SymbolType type,
		std::unordered_map<SymbolStringType, std::string> &&strings) noexcept
		: ObjectEnvSymbol(type, std::move(strings)) {
}

TraitSymbol::~TraitSymbol() {
}
