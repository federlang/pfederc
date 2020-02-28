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
    ST_INTERNAL = 0x00040000, // !< Internal types
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
    inline Expr &getExpression() noexcept { return *expr; }
    /*!\return Return associated expression (const).
     */
    inline const Expr &getExpression() const noexcept { return *expr; }

    /*!\return Returns optional parent.
     */
    inline Semantic *getParent() noexcept { return parent; }
    /*!\return Returns optional parent (const).
     */
    inline const Semantic *getParent() const noexcept { return parent; }

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

		virtual void setSemantic(SemanticSingleType type, Semantic *semantic) noexcept;
		
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

	class TypeAnalyzer final {
		const size_t maxthreads;

		std::mutex mtxSemantics;
		std::unordered_map<std::string /* mangle */, std::unique_ptr<Semantic>> semantics;

		Semantic* buildSemantic(Expr *parent, Expr *program) noexcept;
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
		 * \return Returns true on success, otherwise false
		 *
		 * Thread-safe
		 */
		bool addSemantic(const std::string &mangle, std::unique_ptr<Semantic> &&semantic) noexcept;

		/*!\return Returns nullptr on failure (mangle not found),
		 * otherwise not nullptr/valid semantic
		 * \param mangle !mangle.empty()
		 *
		 * Thread-safe
		 */
		Semantic *getSemantic(const std::string &mangle) noexcept;

		/*!\brief
		 * \param expressions Top-level expressions. Every expression must
		 * be either a module. The string is the module name, Expr is the program.
		 */
		Semantic* buildSemantics(std::list<std::tuple<std::string, Expr*>> &expressions) noexcept;
	};
}

#endif /* PFEDERC_SEMANTICS_SEMANTICS_HPP */
