#ifndef PFEDERC_SEMANTICS_SEMANTICS_HPP
#define PFEDERC_SEMANTICS_SEMANTICS_HPP

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/token.hpp"
#include "pfederc/lexer.hpp"
#include "pfederc/expr.hpp"
#include "pfederc/syntax.hpp"

namespace pfederc {
	class Symbol;
	class ObjectSymbol;
	class VariableSymbol;
	class ClassSymbol;
	class TraitSymbol;
	class FunctionSymbol;

	typedef std::unordered_map<std::string, Symbol*> SymbolMap;

	typedef std::uint32_t SymbolType;
	constexpr SymbolType SYM_OBJ     = 0x00000001;
	constexpr SymbolType SYM_NAME    = 0x0000002;
	//! Variables don't always have a name (tuple members)
	constexpr SymbolType SYM_VAR     = 0x00000004 | SYM_OBJ;
	constexpr SymbolType SYM_TYPE    = 0x0000008;
	constexpr SymbolType SYM_CLASS   = 0x0000010 | SYM_TYPE;
	constexpr SymbolType SYM_TRAIT   = 0x0000020 | SYM_TYPE;
	//!< Is it a function (type). Without SYM_TYPE it's a callable function.
	constexpr SymbolType SYM_FN      = 0x0000040;
	//!< Is it a function type
	constexpr SymbolType SYM_FN_TYPE = SYM_FN | SYM_TYPE;
	//!< Is it a module
	constexpr SymbolType SYM_MOD     = 0x0000080;
	//! allow implicit convertion at compile time (e.g. integer expressions)
	constexpr SymbolType SYM_CONV    = 0x0000100;
	//! Empty type is not object, because edge-case
	constexpr SymbolType SYM_NULL    = 0x0000200;
	constexpr SymbolType SYM_ANY     = 0x0000400;
	//! Private semantic
	constexpr SymbolType SYM_PRIV    = 0x0000800;

	typedef std::uint16_t ObjectModifiers;
	//! Constant lookup access
	constexpr ObjectModifiers OBJMOD_CONST     = 0x0001;
	//! Object's value is known at compile time
	constexpr ObjectModifiers OBJMOD_DIRECT    = 0x0002;

	typedef std::uint16_t VariableModifiers;
	//! Pointer (stack-value) cannot be modified
	constexpr VariableModifiers VARMOD_MUTABLE  = 0x0001;
	//! Variable is global
	constexpr VariableModifiers VARMOD_GLOBAL   = 0x0002;
	//! Class attribute can even be modified when This is constant
	constexpr VariableModifiers VARMOD_CONSTMOD = 0x0004;

	enum class SymbolStringType {
		NAME,
		MANGLE,
	};

	class Symbol {
		SymbolType type;
		std::unordered_map<SymbolStringType, std::string> strings;
	public:
		Symbol(SymbolType type,
				std::unordered_map<SymbolStringType, std::string> &&strings = {}) noexcept;
		virtual ~Symbol();

		inline SymbolType getType() const noexcept { return type; }
		/*!\return Returns string associated to sstype
		 * \param type
		 * 
		 * Undefined behaviour if sstype isn't mapped to string
		 */
		inline const std::string &getString(SymbolStringType sstype) const noexcept
		{ return strings.at(sstype); }

		virtual bool operator ==(const std::string&) const noexcept;
	};

	class Ensurance {
		Symbol *sym;
	public:
		Ensurance(Symbol *const sym) noexcept;
		virtual ~Ensurance();

		inline const Symbol *getSymbol() const noexcept
		{ return sym; }
	};

	class ObjectSymbol : public Symbol {
		ObjectModifiers objmod;
		std::vector<Symbol*> types;
	public:
		/*!\brief Intialize ObjectSymbol
		 * \param type additional SYMs to SYM_OBJ
		 * \param objmod
		 * \param types
		 * \param strings
		 */
		ObjectSymbol(SymbolType type,
				ObjectModifiers objmod, std::vector<Symbol*> &&types,
				std::unordered_map<SymbolStringType, std::string> &&strings = {}) noexcept;
		virtual ~ObjectSymbol();

		inline ObjectModifiers getObjectModifiers() const noexcept
		{ return objmod; }
	
		/*!\brief When isTuple() == true, every type in getTypes() is another
		 * object, not type. Otherwise, if getTypes().size() == 1, the returned
		 * Symbol is a type.
		 */
		inline const std::vector<Symbol*> &getTypes() const noexcept
		{ return types; }

		/*!\return Returns getTypes().size() > 1.
		 */
		inline bool isTuple() const noexcept { return types.size() > 1; }

		/*!\return Returns symbol at index 'idx'
		 * \param idx Must be less than getTypes().size()
		 *
		 * This function is undefined for isTuple() == false
		 */
		Symbol *getTupleType(const std::size_t idx) const noexcept;
	};

	class VariableSymbol final : public ObjectSymbol {
		VariableModifiers varmod;
	public:
		/*!\brief Initialize VariableSymbol
		 * \param type Additional SYMs to SYM_VAR | SYM_VAR
		 * \param varmod
		 * \param objmod
		 * \param strings
		 */
		VariableSymbol(SymbolType type, VariableModifiers varmod,
				ObjectModifiers objmod,
				std::vector<Symbol*> &&types,
				std::unordered_map<SymbolStringType, std::string> &&strings = {}) noexcept;
		virtual ~VariableSymbol();

		inline VariableModifiers getVariableModifiers() const noexcept
		{ return varmod; }
	};

	class ObjectEnvSymbol : public Symbol {
		mutable std::mutex objenvsymsMtx;
		std::unordered_map<std::string, std::list<Symbol*>> objenvsyms;
	public:
		ObjectEnvSymbol(SymbolType type,
				std::unordered_map<SymbolStringType, std::string> &&strings = {}) noexcept;
		virtual ~ObjectEnvSymbol();
		
		void addSymbol(const std::string &name, Symbol *const sym) noexcept;

		virtual std::list<Symbol*> lookupInObject(const std::string &result) const noexcept;
	};

	class TraitSymbol final : public ObjectEnvSymbol {
	public:
		TraitSymbol(SymbolType type,
				std::unordered_map<SymbolStringType, std::string> &&strings = {}) noexcept;
		virtual ~TraitSymbol();

		virtual std::list<Symbol*> lookupInObject(const std::string &result) const noexcept override;
	};

	Symbol *convertTraitToClassFunction(FunctionSymbol *const sym) noexcept;

	class ClassSymbol final : public ObjectEnvSymbol {
		std::list<TraitSymbol*> implTraits;
	public:
		ClassSymbol(SymbolType type,
				std::unordered_map<SymbolStringType, std::string> &&strings = {}) noexcept;
		virtual ~ClassSymbol();

		inline auto &getTraits() noexcept
		{ return implTraits; }

		inline const auto &getTraits() const noexcept
		{ return implTraits; }

		virtual std::list<Symbol*> lookupInObject(const std::string &result) const noexcept override;
	};

	class FunctionSymbol final : public Symbol {
		Symbol *returnType;
		std::list<Symbol*> params;
	public:
		FunctionSymbol(SymbolType type,
				Symbol *const returnType, std::list<Symbol*> &&params,
				std::unordered_map<SymbolStringType, std::string> &&strings = {}) noexcept;
		virtual ~FunctionSymbol();

		inline const Symbol *getReturnType() const noexcept
		{ return returnType; }

		inline const auto &getParameter() const noexcept
		{ return params; }
	};
}

#endif /* PFEDERC_SEMANTICS_SEMANTICS_HPP */
