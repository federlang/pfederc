#ifndef PFEDERC_SEMANTICS_SEMANTICS_HPP
#define PFEDERC_SEMANTICS_SEMANTICS_HPP

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/token.hpp"
#include "pfederc/semantics_internals.hpp"

namespace pfederc {
  class Context;
  class CtxId;
  class CtxIdHash;
  class Symbol;
  class TemplateSymbol;
  class NamespaceSymbol;
  class LambdaSymbol;
  class FunctionSymbol;
  class ClassSymbol;
  class EnumSymbol;
  class TraitSymbol;
  class TupleSymbol;
  class ObjectSymbol;
  class VariableSymbol;
  class FunctionParamSymbol;
  class AliasSymbol;
  class NamespaceSymbol;

  enum SymbolType : uint64_t {
    ST_MODULE   = 0x00000001ull,
    ST_TRAIT    = 0x00000002ull,
    ST_CLASS    = 0x00000004ull,
    ST_OBJECT   = 0x00000008ull,
    ST_FUNCTION = 0x00000010ull,
    ST_LAMBDA   = 0x00000020ull,
    ST_VARIABLE = 0x00000040ull,
    ST_TYPEDEF  = 0x00000080ull,
    ST_ALIAS    = 0x00000100ull,
    ST_LITERAL  = 0x00000200ull,
    ST_NAMED    = 0x00000400ull,
    ST_PRIVATE  = 0x00000800ull,
    ST_INTERNAL = 0x00001000ull,
    ST_CONSTANT = 0x00002000ull,
    ST_MUTABLE  = 0x00004000ull,
    ST_DEFINED  = 0x00008000ull,
    //! Hidden type, not accessable from outside contexts
    ST_HIDDEN   = 0x00010000ull,
    ST_TUPLE    = 0x00020000ull,
    ST_TEMPLATE = 0x00040000ull,
    ST_INLINE   = 0x00080000ull,
    ST_ENUM     = 0x00100000ull,
    ST_NULL     = 0x00200000ull,
  };

  class CtxId {
    Context *ctx;
    uint32_t id;
    bool flagContext;

    std::list<std::unique_ptr<Symbol>> unnamedSymbols;
    std::map<std::string, std::list<std::unique_ptr<Symbol>>> symbols;
    mutable std::recursive_mutex mtxSymbols;

    const Context *commonParent(const CtxId &ctxid) const noexcept;
  public:
    /*!
     * \param ctx Context CtxId was created in
     * \param id Unique it in ctx
     * \param flagContext
     */
    inline CtxId(Context *ctx, uint32_t id, bool flagContext = false) noexcept
      : ctx{ctx}, id{id}, flagContext{flagContext}, unnamedSymbols() {
      assert(this->ctx);
    }
    virtual ~CtxId() {}

    inline bool operator == (const CtxId &ctxid) const noexcept {
      return ctx == ctxid.ctx && id == ctxid.id;
    }

    /*!\return Returns closest context.
     */
    inline const Context &getContext() const noexcept {
      return *ctx;
    }

    /*!\return Returns true, if this is of type Context.
     */
    bool isContext() const noexcept {
      return flagContext;
    }

    /*!\return Returns false, if ctxid is executed prior to this
     * (translates roughly to: the position of the code is above this),
     * otherwise true is returned.
     */
    bool operator < (const CtxId &ctxid) const noexcept;

    inline bool operator > (const CtxId &ctxid) const noexcept {
      return ctxid < *this;
    }

    inline  uint32_t getIdentifier() const noexcept {
      return id;
    }

    std::list<Symbol*> getSymbols(const std::string &name, bool fromtop) noexcept;
    std::list<const Symbol*> getSymbols(const std::string &name, bool fromtop) const noexcept;

    /*!\brief Adds sym to symbols map
     * \param sym
     * \return Returns nullptr, if sym does already exists in map. Otherwise
     * the parameter sym is returned.
     */
    std::unique_ptr<Symbol> addSymbol(std::unique_ptr<Symbol> &&sym) noexcept;
  };

  /**
   * Type for referencing the current context
   */
  class Context : public CtxId {
    Context *parent;
    std::list<std::unique_ptr<CtxId>> children;
    uint32_t id;
    uint32_t lastId;

    mutable std::recursive_mutex mtxId;

    inline Context(Context *parent, uint32_t id) noexcept
      : CtxId(this, id, true), lastId{0}, parent{parent},
        children(),
        mtxId() {
      assert(id == 0 || parent);
    }

    /*!
     * \return Returns new Context id
     *
     * This function is thread-safe.
     */
    inline uint32_t createId() noexcept {
      std::lock_guard<std::recursive_mutex> guard(mtxId);
      return lastId++;
    }
  public:
    /*!\brief Creates the origin context
     */
    inline Context() noexcept
      : Context(nullptr, 0) {}

    virtual ~Context();

    /*!\return Returns new context id, which is a child of this context.
     * The returned pointer is deleted on destruction of this.
     */
    CtxId *createCtxId() noexcept;

    /*!\return Returns new context, which is a child of this context.
     * The returned pointer is deleted on destruction of this.
     */
    Context *createContext() noexcept;

    /*!\return Returns  this contenxt's parent. Can be nullptr.
     */
    inline Context *getParent() noexcept
    { return parent; }

    /*!\return Returns this contenxt's parent. Can be nullptr.
     */
    inline const Context *getParent() const noexcept
    { return parent; }

    /*!\return Returns direct child context, which is (indirect) parent of
     * child.  Returns nullptr, if there's no direct child, was has 'child' as
     * indirect child.
     */
    const CtxId *getChildContextIdWithChild(const Context &child) const noexcept;

    inline bool operator == (const Context &ctx) const noexcept
    { return this == &ctx; }

    /*!\return Returns true, if ctx is a parent of this.
     * \param ctx Context
     */
    bool isParent(const Context &ctx) const noexcept;
  };

  class CtxIdHash final {
  public:
    constexpr CtxIdHash() noexcept {}
    inline ~CtxIdHash() {}

    inline size_t operator ()(const CtxId &ctxid) const noexcept {
      return pfederc::combineHashes(
          std::hash<uint32_t>{}(ctxid.getIdentifier()),
          std::hash<const void*>{}(&(ctxid.getContext())));
    }
  };

  template<class T>
  class SymbolHistory {
    std::set<CtxId*> ctxTypeChangeList;
    std::unordered_map<const CtxId*, T> ctxTypeChanges;
  public:
    inline SymbolHistory() noexcept :
      ctxTypeChangeList(), ctxTypeChanges() {}
    virtual ~SymbolHistory();

    const auto getChangesContextIds() const noexcept
    { return ctxTypeChangeList; }

    const auto &getChanges() const noexcept
    { return ctxTypeChanges; }

    /**
     * \param id id must not be element of getChangesContextIds()
     * \param t The associated change
     */
    void addChange(CtxId &id, T&& t) noexcept {
      CtxId *ptrId = &id;
      assert(ctxTypeChangeList.find(ptrId) != ctxTypeChangeList.end());
      // insert into set and map
      ctxTypeChangeList.emplace(ptrId);
      ctxTypeChanges.emplace(std::move(ptrId), std::move(t));
    }
  };

  struct CtxTypeChange {
    bool invert;
    uint64_t changeType;
  };
  
  class SymbolHistoryType : public SymbolHistory<CtxTypeChange> {
  public:
    inline SymbolHistoryType() noexcept :
      SymbolHistory<CtxTypeChange>() {}
    virtual ~SymbolHistoryType();

    uint64_t computeType(const CtxId &id, uint64_t baseSymType) const noexcept;
  };

  class Symbol {
    uint64_t internalId;
    SymbolHistoryType symhisType;
  protected:
    uint64_t type;
  public:
    /*!\brief Initializes a symbol
     * \param type The symbol type (bitmask)
     * \param internalId The internal id (used for internals like u8)
     */
    Symbol(uint64_t type, uint64_t internalId = ITID_NOT_INTERNAL) noexcept;
    virtual ~Symbol();

    virtual Symbol *getType() noexcept;
    virtual const Symbol *getType() const noexcept;

    virtual Context *getContext() noexcept;
    virtual const Context *getContext() const noexcept;

    /*!\return Returns nullptr, if this symbol doesn't have a name, otherwise
     * an existing pointer to a string is returned.
     */
    virtual const std::string *getName() const noexcept { return nullptr; }

    /*!
     * \param id The modifiers of a Symbol can change depending on the context
     * \param The SymbolType(s) to check
     * \return Returns true, if every SymbolType in types is a modifier in this
     * Symbol.
     */
    bool is(const CtxId &id, uint64_t types) const noexcept;

    /*!\return Returns true, if getName() exists for both objects and they are
     * equal, or if either of the two objects return nullptr for getName(),
     * then the reference of both objects is compared.
     */
    virtual bool operator ==(const Symbol &sym) const noexcept;

    inline uint64_t getInternalId() const noexcept
    { return internalId; }
  };

  class NullSymbol final : public Symbol {
  public:
    NullSymbol() noexcept;
    virtual ~NullSymbol();
  };

  class AliasSymbol final : public Symbol {
    Symbol *symtype;
    std::string name;
  public:
    AliasSymbol(const std::string &name, Symbol *symtype) noexcept;
    virtual ~AliasSymbol();

    virtual Symbol *getType() noexcept override;
    virtual const Symbol *getType() const noexcept override;

    virtual const std::string *getName() const noexcept override;
    
    virtual bool operator ==(const Symbol &sym) const noexcept override;
  };

  class TemplateSymbol : public Symbol {
    std::list<AliasSymbol*> templSyms;
  public:
    TemplateSymbol(uint64_t type,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId = 0) noexcept;
    virtual ~TemplateSymbol();

    inline std::list<AliasSymbol*> getTemplateSymbols() noexcept {
      return templSyms;
    }

    inline const std::list<AliasSymbol*> getTemplateSymbols() const noexcept {
      return templSyms;
    }

    virtual Symbol *resolveTemplate(std::list<AliasSymbol*> &&templSyms) noexcept = 0;
  };

  class LambdaSymbol : public TemplateSymbol {
    std::list<std::unique_ptr<FunctionParamSymbol>> params;
    Symbol *returnValue;
  public:
    LambdaSymbol(uint64_t type,
        std::list<std::unique_ptr<FunctionParamSymbol>> &&params,
        Symbol *returnValue,
        std::list<AliasSymbol*> &&templSyms = std::list<AliasSymbol*>(),
        uint64_t internal = 0) noexcept;
    virtual ~LambdaSymbol();

    virtual Symbol *getType() noexcept override;
    virtual const Symbol *getType() const noexcept override;

    inline auto &getResult() noexcept
    { return *returnValue; }

    inline const auto &getResult() const noexcept
    { return *returnValue; }

    inline const auto &getParameters() const noexcept
    { return params; }
  };

  class FunctionSymbol final : public LambdaSymbol {
    std::string name;
  public:
    FunctionSymbol(uint64_t type,
        const std::string &name,
        std::list<std::unique_ptr<FunctionParamSymbol>> &&params,
        Symbol *returnValue,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId = 0) noexcept;
    FunctionSymbol(uint64_t type,
        std::string &&name,
        std::list<std::unique_ptr<FunctionParamSymbol>> &&params,
        Symbol *returnValue,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId = 0) noexcept;
    FunctionSymbol(uint64_t type,
        std::string &&name,
        std::list<std::unique_ptr<FunctionParamSymbol>> &&params,
        Symbol *returnValue,
        uint64_t internalId = 0) noexcept;
    virtual ~FunctionSymbol();

    virtual Symbol *getType() noexcept override;
    virtual const Symbol *getType() const noexcept override;

    virtual const std::string *getName() const noexcept override;

    virtual Symbol *resolveTemplate(std::list<AliasSymbol*> &&templSyms) noexcept override;
  };

  class ClassSymbol final : public TemplateSymbol {
    std::unique_ptr<std::string> name;
    Context *objctx;
    Context *ctx;
  public:
    ClassSymbol(uint64_t type,
        Context *ctx,
        const std::string *name,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId = 0) noexcept;
    ClassSymbol(uint64_t type,
        Context *ctx,
        std::string &&name,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId = 0) noexcept;
    ClassSymbol(uint64_t type,
        Context *ctx,
        std::string &&name,
        uint64_t internalId = 0) noexcept;
    virtual ~ClassSymbol();

    virtual Symbol *getType() noexcept override;
    virtual const Symbol *getType() const noexcept override;

    virtual const std::string *getName() const noexcept override;

    virtual Context *getContext() noexcept override;
    virtual const Context *getContext() const noexcept override;

    virtual Context *getObjectContext() noexcept;
    virtual const Context *getObjectContext() const noexcept;

    virtual Symbol *resolveTemplate(std::list<AliasSymbol*> &&templSyms) noexcept override;
  };

  class TraitSymbol final : public TemplateSymbol {
    std::string name;
    Context *ctx;
  public:
    TraitSymbol(uint64_t type,
        Context *ctx,
        const std::string &name,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId = 0) noexcept;
    TraitSymbol(uint64_t type,
        Context *ctx,
        std::string &&name,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId = 0) noexcept;
    TraitSymbol(uint64_t type,
        Context *ctx,
        std::string &&name,
        uint64_t internalId = 0) noexcept;
    virtual ~TraitSymbol();

    virtual Symbol *getType() noexcept override;
    virtual const Symbol *getType() const noexcept override;

    virtual const std::string *getName() const noexcept override;

    virtual Context *getContext() noexcept override;
    virtual const Context *getContext() const noexcept override;

    virtual Symbol *resolveTemplate(std::list<AliasSymbol*> &&templSyms) noexcept override;
  };

  class EnumSymbol final : public TemplateSymbol {
    std::string name;
    Context *ctx;
  public:
    EnumSymbol(uint64_t type, const std::string &name,
        Context *ctx,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId = 0) noexcept;
    EnumSymbol(uint64_t type, std::string &&name,
        Context *ctx,
        std::list<AliasSymbol*> &&templSyms,
        uint64_t internalId = 0) noexcept;
    EnumSymbol(uint64_t type, std::string &&name,
        Context *ctx,
        uint64_t internalId = 0) noexcept;
    virtual ~EnumSymbol();

    virtual Symbol *getType() noexcept override;
    virtual const Symbol *getType() const noexcept override;

    virtual const std::string *getName() const noexcept override;

    virtual Context *getContext() noexcept override;
    virtual const Context *getContext() const noexcept override;

    virtual Symbol *resolveTemplate(std::list<AliasSymbol*> &&templSyms) noexcept override;
  };

  class TupleSymbol final : public Symbol {
    std::unique_ptr<std::string> name;
    Context *ctx;
  public:
    inline TupleSymbol(uint64_t type,
        const std::string *name, Context *ctx,
        uint64_t internalId = 0) noexcept
      : Symbol(type | ST_TUPLE),
        name(name ? new std::string(*name) : nullptr), ctx{ctx} {}
    virtual ~TupleSymbol();

    virtual const std::string *getName() const noexcept override;

    virtual Context *getContext() noexcept override;
    virtual const Context *getContext() const noexcept override;
  };

  class ObjectSymbol : public Symbol {
    Symbol *objType;
  public:
    ObjectSymbol(uint64_t type, Symbol *objType,
        uint64_t internalId = 0) noexcept;
    virtual ~ObjectSymbol();

    virtual Symbol *getType() noexcept override;
    virtual const Symbol *getType() const noexcept override;

    virtual Context *getContext() noexcept override;
    virtual const Context *getContext() const noexcept override;
  };

  class VariableSymbol : public ObjectSymbol {
    std::string name;
  public:
    VariableSymbol(uint64_t type,
        const std::string &name, Symbol *objType,
        uint64_t internalId = 0) noexcept;
    VariableSymbol(uint64_t type,
        std::string &&name, Symbol *objType,
        uint64_t internalId = 0) noexcept;
    virtual ~VariableSymbol();

    virtual const std::string *getName() const noexcept override;
  };

  class FunctionParamSymbol final : public VariableSymbol {
    std::unique_ptr<std::string> name;
  public:
    virtual const std::string *getName() const noexcept override;
  };

  class NamespaceSymbol final : public Symbol {
    std::string name;
    Context *ctx;
  public:
    NamespaceSymbol(uint64_t type,
        const std::string &name, Context *ctx) noexcept;
    virtual ~NamespaceSymbol();

    virtual Context *getContext() noexcept override;
    virtual const Context *getContext() const noexcept override;

    const std::string *getName() const noexcept override;
  };

  void addStandardLibrary(Context &mainContext) noexcept;
}

#endif /* PFEDERC_SEMANTICS_SEMANTICS_HPP */
