#ifndef PFEDERC_SEMANTICS_SEMANTICS_HPP
#define PFEDERC_SEMANTICS_SEMANTICS_HPP

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"
#include "pfederc/token.hpp"
#include "pfederc/lexer.hpp"
#include "pfederc/expr.hpp"
#include "pfederc/syntax.hpp"

namespace pfederc {
  class Semantic;

  enum SemanticType : uint32_t {
    ST_NAME =     0x00000001,
    ST_OBJ =      0x00000002,
    ST_VAR =      0x00000004,
    ST_FN =       0x00000008,
    ST_CLASS =    0x00000010,
    ST_TRAIT =    0x00000020,
    ST_DEF =      0x00000040,
    ST_MUT =      0x00000080,
    ST_CONST =    0x00000100,
    ST_PRIV =     0x00000200,
    ST_ENUM =     0x00000400,
    ST_ENUMVAL =  0x00000800,
    ST_CONSTMUT = 0x00001000,
    ST_TYPE =     0x00002000,
    ST_TEMPLATE = 0x00004000,
    ST_MODULE =   0x00008000,
    ST_TUPLE =    0x00010000,
    ST_TEMPLVAR = 0x00020000, //!< Template variable
    ST_INTERNAL = 0x00040000, //!< Internal types
    ST_PROGRAM =  0x00080000, //!< Program Module
  };

  inline constexpr bool hasAllSemanticType(uint32_t org, uint32_t shouldhave) {
    return (org & shouldhave) == shouldhave;
  }

  inline constexpr bool hasAnySemanticType(uint32_t org, uint32_t shouldhave) {
    return (org & shouldhave) != 0;
  }

  enum class SemanticInfoStringType {
    NAME,   //!< name of Semantic (requires ST_NAME)
    MANGLE, //!< magled name (requires ST_NAME)
  };

  enum class SemanticSingleType : uint8_t {
    SUBENV, //!< Subenv of classes, traits, functions
    RETURN, //!< Return value of function
    TYPE,   //!< Variable, object types (not tuple)
  };

  enum class SemanticMultipleType : uint8_t {
    PARAMS, //!< Parameter of functions
    IMPLEMENTS, //!< Implemented traits (classes,traits)
    ATTRIBUTES, //!< Class attributes
    FUNCTIONS, //!< Class/trait functions
    OBJS, //!< Objects/vars of tuples
  };

  class Semantic {
    uint32_t type;
    Expr *expr;
    Semantic *parent;
    std::unordered_map<SemanticInfoStringType, std::string> infoStrings;
  protected:
    std::unordered_map<std::string, Semantic*> children;
    std::unordered_map<SemanticSingleType, Semantic*> singleSemantics;
    std::unordered_map<SemanticMultipleType, std::list<Semantic*>> multipleSemantics;
  public:
    /*!\brief Initializes Semantic
     * \param type The semantics type. Returned by getType().
     * \param expr The underlying expression. Returned getExpression().
     * \param parent The optional parent (file semantics don't have one).
     * Returned by getParent().
     * \param infoStrings Used by getInfoString()
     */
    Semantic(uint32_t type, Expr *expr, Semantic *parent,
        std::unordered_map<SemanticInfoStringType, std::string> &&infoStrings = {},
				std::unordered_map<SemanticSingleType, Semantic*> &&singleSemantics = {},
				std::unordered_map<SemanticMultipleType, std::list<Semantic*>> &&multipleSemantics = {}) noexcept;
    Semantic(const Semantic &) = delete;
    virtual ~Semantic();

    inline uint32_t getType() const noexcept { return type; }

    inline bool isTypeFunction() const noexcept
    { return (type & (ST_FN | ST_TYPE)) == ST_FN; }

    inline bool isTypeFunctionType() const noexcept
    { return hasAllSemanticType(type, ST_FN | ST_TYPE); }

    inline bool isTypeVariable() const noexcept
    { return hasAllSemanticType(type, ST_VAR); }

    inline bool isTypeDefinedVariable() const noexcept
    { return hasAllSemanticType(type, ST_VAR | ST_OBJ); }

    inline bool isTypeValidObject() const noexcept
    { return hasAllSemanticType(type, ST_OBJ); }

    //!\return Returns true if object type is class/trait
    bool isTypeRegularObject() const noexcept;
    //!\return Returns true if object type is internal class/trait
    bool isTypeInternalObject() const noexcept;

    //!\return Returns true if all objs in tuple are objects
    bool isTypeTupleObjects() const noexcept;
    //!\return Returns true if all objs in tuple are variables
    bool isTypeTupleVariables() const noexcept;

    inline const std::string
        &getInfoString(SemanticInfoStringType istrtype) const noexcept
      { return infoStrings.at(istrtype); }

    /*!\return Return associated expression.
     */
    inline Expr *getExpression() noexcept { return expr; }
    /*!\return Return associated expression (const).
     */
    inline const Expr *getExpression() const noexcept { return expr; }

    /*!\return Returns optional parent.
     */
    inline Semantic *getParent() noexcept { return parent; }
    /*!\return Returns optional parent (const).
     */
    inline const Semantic *getParent() const noexcept { return parent; }

    /*!\brief Set type to semantic. Type must exist before (initialized
     * by constructor).
     * \param type
     * \param semantic Not nullptr
     */
		virtual void setSemantic(SemanticSingleType type, Semantic *semantic) noexcept;

    virtual Semantic *getSemantic(SemanticSingleType type) noexcept;

    virtual const Semantic *getSemantic(SemanticSingleType type) const noexcept;

		/*!\brief Adds semantic to list type
		 * \return Return true on success, otherwise false.
		 */
		virtual bool addSemantics(SemanticMultipleType type, Semantic *semantic) noexcept;

		/*!\brief Executeds fn for each element in list with 'type'
		 */
		virtual void forEachSemantics(SemanticMultipleType type,
				const std::function<void(Semantic *semantic)> &fn) const noexcept;

    /*!\brief Searches for child called name, additionally searches
     * through children if 'enablechildren' is true.
     * \param name
     * \param enablechildren If true, search through children
     * \return Returns nullptr if nothing was found, otherwise valid semantic.
     *
     * Not thread safe.
     */
    virtual const Semantic *findChild(const std::string &name, bool enablechildren = true) const noexcept;

    /*!\brief Adds child with name
     *
     * Not thread safe.
     */
    virtual bool addChild(const std::string &name, Semantic *semantic) noexcept;
    /*!\brief Sets child with name
     *
     * Not thread safe
     */
    virtual bool setChild(const std::string &name, Semantic *semantic) noexcept;
    /*!\brief Returns nullptr if no element with key name exists,
     * otherwise the assigned value.
     *
     * Not thread safe.
     */
    virtual Semantic *getChild(const std::string &name) noexcept;
  };

  /*!\return Returns mangled name of module called name with parent
   * \param parent Can be null or valid pointer to expression.
   * \param name Name of the module
   */
  std::string mangleModule(Expr *parent, const std::string &name) noexcept;

  /*!\return Returns mangled name of function called name with parent
   * \param parent Mustn't be nullptr
   * \param name Name of the function
   * \param beginParam beginning of paremeter list (magled names)
   * \param endParam end of paremeter list (mangled names)
   * \param returnType Empty if none, otherwise mangled name of type
   */
  std::string mangleFunction(const Expr *parent, const std::string &name,
      std::iterator<std::input_iterator_tag, std::string> beginParam,
      std::iterator<std::input_iterator_tag, std::string> endParam,
      const std::string &returnType) noexcept;

  /*!\return Returns mangled name of class called name with parent
   */
  std::string mangleClass(const Expr *parent, const std::string &name) noexcept;

  /*!\return Returns mangled trait name
   */
  std::string mangleTrait(const Expr *parent, const std::string &name) noexcept;

  /*!\return Returns mangled global variable name
   */
  std::string mangleGlobalVariable(const Expr *parent, const std::string &name) noexcept;

  std::string mangleProgram(const std::string &name) noexcept;

  /*!\brief Semantic with thread safety
   */
  class SafeSemantic : public Semantic {
    mutable std::mutex mtxChildren, mtxSingle, mtxMultiple;
  public:
    SafeSemantic(uint32_t type, Expr *expr, Semantic *parent,
        std::unordered_map<SemanticInfoStringType, std::string> &&infoStrings = {},
				std::unordered_map<SemanticSingleType, Semantic*> &&singleSemantics = {},
				std::unordered_map<SemanticMultipleType, std::list<Semantic*>> &&multipleSemantics = {}) noexcept;
    SafeSemantic(const Semantic &) = delete;
    virtual ~SafeSemantic();

    /*!\brief Searches for child called name, additionally searches
     * through children if 'enablechildren' is true.
     * \param name
     * \param enablechildren If true, search through children
     * \return Returns nullptr if nothing was found, otherwise valid semantic.
     *
     * Thread safe.
     */
    virtual const Semantic *findChild(const std::string &name, bool enablechildren = true) const noexcept;
    /*!\brief Adds child with name
     *
     * Thread safe.
     */
    virtual bool addChild(const std::string &name, Semantic *semantic) noexcept;
    /*!\brief Sets child with name
     *
     * Thread safe
     */
    virtual bool setChild(const std::string &name, Semantic *semantic) noexcept;
    /*!\brief Returns nullptr if no element with key name exists,
     * otherwise the assigned value.
     *
     * Thread safe.
     */
    virtual Semantic *getChild(const std::string &name) noexcept;

    /*!\brief Set type to semantic. Type must exist before (initialized
     * by constructor).
     * \param type
     * \param semantic Not nullptr
     */
		virtual void setSemantic(SemanticSingleType type, Semantic *semantic) noexcept;

    virtual Semantic *getSemantic(SemanticSingleType type) noexcept;

    virtual const Semantic *getSemantic(SemanticSingleType type) const noexcept;
		
		/*!\brief Adds semantic to semantics of 'type'
		 * \param type To which list the semantic should be added. List must
		 * have been added by at construction
		 * \param semantic
		 * \return Returns true, if semantic already part of list
		 */
		virtual bool addSemantics(SemanticMultipleType type, Semantic *semantic) noexcept;

		/*!\brief Executeds fn for each element in list with 'type'
		 */
		virtual void forEachSemantics(SemanticMultipleType type,
				const std::function<void(Semantic *semantic)> &fn) const noexcept;
  };

  struct BuildSemanticParam {
    std::string filepath;
    bool build;
    ProgramExpr *expr;
  };

	class TypeAnalyzer final {
		const size_t maxthreads;

		std::mutex mtxSemantics;
		std::unordered_map<std::string /* mangle */, std::unique_ptr<Semantic>> semantics;

    std::mutex mtxBuildSemantics;
    //!\brief Which semantics the target has to build
    std::unordered_map<std::string, std::list<Semantic*>> targetBuildSemantics;

    std::unordered_map<std::string, Semantic*> programs;

    std::unordered_map<Expr*,Semantic*> exprToSemantic;

    void addSemanticToBuilder(const std::string &name, Semantic *semantic) noexcept;

    void helperBuildProgramSemantic(const BuildSemanticParam &param) noexcept;
    void helperBuildProgramSemantics(const std::list<BuildSemanticParam> &list) noexcept;
    void helperBuildModuleSemantic(Expr* parent, ModExpr *expr) noexcept;
    void helperBuildModuleSemantics(const std::list<BuildSemanticParam> &list) noexcept;
	public:
		/*!\brief Initializes TypeAnalyzer
		 * \param maxthreads How many simultanous threads should be opened.
		 * Returned by getMaxThreads().
		 */
		TypeAnalyzer(const size_t maxthreads) noexcept;
		~TypeAnalyzer();

		size_t getMaxThreads() const noexcept { return maxthreads; }

		/*!\brief Adds mangle+semantic to semantics
		 * \param mangle
		 * \param semantics
		 * \return Returns pointer of added semantic if added to map,
     * otherwise a pointer to already added object is returned.
		 *
		 * Thread-safe
     *
     * Only binds semantic to expression if binding between mangle and semantic
     * wasn't established before.
     */
		Semantic *addSemantic(const std::string &mangle,
        std::unique_ptr<Semantic> &&semantic) noexcept;

    /*!\brief Nearly the same as addSemantic() but adds semantic to expression
     * binding regardless wether or not the semantic was already bound to the
     * mangle
     */
    Semantic *addSemanticAlways(const std::string &mangle,
        std::unique_ptr<Semantic> &&semantic) noexcept;

		/*!\return Returns nullptr on failure (mangle not found),
		 * otherwise not nullptr/valid semantic
		 * \param mangle !mangle.empty()
		 *
		 * Thread-safe
		 */
		Semantic *getSemantic(const std::string &mangle) noexcept;

    /*!\brief Build single semantic
     * \param fileIdx 0 = Input for interpreter. Everything else are files.
     * \return Returns nullptr if failed to build semantic
     *
     * Note: Only use for interpreter purposes externally
     */
		Semantic* buildSemantic(size_t fileIdx, bool build,
                           Semantic *parent, Expr *program) noexcept;

    /*!\brief Build semantics
     */
		void buildSemantics(const std::list<BuildSemanticParam> &list) noexcept;
	};
}

#endif /* PFEDERC_SEMANTICS_SEMANTICS_HPP */
