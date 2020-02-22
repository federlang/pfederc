#ifndef PFEDERC_SYNTAX_EXPR_HPP
#define PFEDERC_SYNTAX_EXPR_HPP

#include "pfederc/core.hpp"
#include "pfederc/lexer.hpp"
#include "pfederc/errors.hpp"

namespace pfederc {
  enum ExprType {
    EXPR_TOK,

    EXPR_PROGNAME,
    EXPR_USE,

    EXPR_PROG,
    EXPR_FUNC,
    EXPR_FUNCTYPE,
    EXPR_LAMBDA,
    EXPR_TRAIT,
    EXPR_CLASS,
    EXPR_TRAITIMPL,
    EXPR_ENUM,
    EXPR_TYPE,
    EXPR_MOD,
    EXPR_SAFE,
    EXPR_IF,
    EXPR_LOOP_FOR,
    EXPR_LOOP_DO,
    EXPR_MATCH,

    EXPR_BIOP,
    EXPR_UNOP,

    EXPR_BODY,

    EXPR_ARRLIT,
    EXPR_ARRCPY,
    EXPR_ARREMPTY,

    EXPR_ERR,
  };

  class Expr;
  class ProgramExpr;
  class TokenExpr;
  class FakeTokenExpr;
  class UseExpr;
  class ProgNameExpr;
  class FuncExpr;
  class LambdaExpr;
  class TraitExpr;
  class ClassExpr;
  class TraitImplExpr;
  class EnumExpr;
  class ModExpr;
  class SafeExpr;
  class IfExpr;
  class LoopExpr;
  class MatchExpr;
  class BiOpExpr;
  class UnOpExpr;
  class BodyExpr;
  class ArrayCpyExpr;
  class ArrayLitExpr;
  class ArrayEmptyExpr;
  class Capabilities;

  class Capable {
  protected:
    std::unique_ptr<Capabilities> caps;
  public:
    inline Capable(std::unique_ptr<Capabilities> &&caps) noexcept
        : caps(std::move(caps)) {}
    inline ~Capable() {}

    inline const Capabilities &getCapabilities() const noexcept { return *caps; }
    inline Capabilities &getCapabilities() noexcept { return *caps; }
  };

  class Expr {
    const Lexer &lexer;
    ExprType type;
    Position pos;
  public:
    /*!\brief Initializes Expr
     * \param lexer Return value of getLexer()
     * \param type Return value of getType()
     * \param pos Return value of getPosition()
     */
    Expr(const Lexer &lexer, ExprType type, const Position &pos) noexcept;
    Expr(const Expr &) = delete;
    virtual ~Expr();

    inline const Lexer &getLexer() const noexcept { return lexer; }
    inline ExprType getType() const noexcept { return type; }
    inline const Position &getPosition() const noexcept { return pos; }

    virtual std::string toString() const noexcept = 0;
  };
  
  enum class EnsuranceType {
    Ensures, 
    Requires,
  };

  class Capabilities final {
    bool isunused, isinline, isconstant;
    std::vector<std::unique_ptr<Expr>> required;
    std::vector<std::unique_ptr<Expr>> ensures;
  public:
    Capabilities(bool isunused, bool isinline, bool isconstant,
        std::vector<std::unique_ptr<Expr>> &&required,
        std::vector<std::unique_ptr<Expr>> &&ensures) noexcept;
    Capabilities(const Capabilities &) = delete;
    ~Capabilities();

    inline bool isUnused() const noexcept { return isunused; }
    inline bool isInline() const noexcept { return isinline; }
    inline bool isConstant() const noexcept { return isconstant; }

    inline const auto &getRequires() const noexcept { return required; }
    inline const auto &getEnsures() const noexcept { return ensures; }
  };

  typedef std::tuple<const Token * /* id */,
    std::unique_ptr<Expr>> TemplateDecl;
  typedef std::vector<TemplateDecl> TemplateDecls;

  typedef std::vector<std::unique_ptr<Expr>> Exprs;

  class ProgramExpr final : public Expr {
    const Token * progName;
    Exprs imports;
    Exprs defs;
  public:
    /*!\brief Initializes ProgramExpr
     * \param lexer
     * \param pos Return value getPosition()
     * \param progName Optional program name, return  value of getProgramName()
     * \param includes Return value of getIncludes()
     * \param imports Return value of getImports()
     * \param defs Return value of getDefinitions()
     */
    ProgramExpr(const Lexer &lexer, const Position &pos,
        const Token *progName,
        Exprs &&imports,
        Exprs &&defs) noexcept;
    ProgramExpr(const ProgramExpr &) = delete;
    virtual ~ProgramExpr();

    inline const Token *getProgramName() const noexcept { return progName; }

    inline const auto &getImports() const noexcept { return imports; }

    /*!\return Returns vector of *def* expressions. The expressions
     * are ordered by their corresponding position in Feder code.
     */
    inline const auto &getDefinitions() const noexcept { return defs; }

    virtual std::string toString() const noexcept;
  };

  class TokenExpr : public Expr {
    Token *tok;
  public:
    /*!\brief Initializes TokenExpr
     * \param lexer
     * \param tok Return value of getToken(), Position of token is lexer input position
     */
    TokenExpr(const Lexer &lexer, Token *tok) noexcept;
    TokenExpr(const TokenExpr &) = delete;
    virtual ~TokenExpr();

    inline Token &getToken() noexcept { return *tok; }
    inline Token *getTokenPtr() noexcept { return tok; }
    inline const Token &getToken() const noexcept { return *tok; }
    inline const Token *getTokenPtr() const noexcept { return tok; }

    virtual std::string toString() const noexcept;
  };

  class FakeTokenExpr final : public TokenExpr {
    std::unique_ptr<Token> tokunique;
  public:
    inline FakeTokenExpr(const Lexer &lexer, std::unique_ptr<Token> &&tok) noexcept
        : TokenExpr(lexer, tok.get()), tokunique() {
      tokunique = std::move(tok);
    }

    inline virtual ~FakeTokenExpr() {}
  };

  inline bool isTokenExpr(const Expr &expr, TokenType type) {
    return expr.getType() == EXPR_TOK &&
      dynamic_cast<const TokenExpr&>(expr).getToken() == type;
  }

  class UseExpr final : public Expr {
    Exprs exprs;
  public:
    /*!\brief Initializes UseExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param exprs Return value of getExpressions(), getExpressionsPtr()
     */
    UseExpr(const Lexer &lexer, const Position &pos,
        Exprs &&exprs) noexcept;
    UseExpr(const UseExpr &) = delete;
    virtual ~UseExpr();

    inline const Exprs &getExpressions() const noexcept { return exprs; }
    inline Exprs getExpressionsPtr() noexcept
    { return std::move(exprs); }

    virtual std::string toString() const noexcept;
  };

  class ProgNameExpr final : public Expr {
    const Token *tok;
  public:
    /*!\brief Initializes ProgNameExpr
     * \param lexer
     * \param tok Return value of getToken()
     */
    ProgNameExpr(const Lexer &lexer, const Token *tok) noexcept;
    virtual ~ProgNameExpr();

    inline const Token &getToken() const noexcept { return *tok; }

    virtual std::string toString() const noexcept;
  };

  typedef std::tuple<const Token* /* opt varname */,
    bool /* is mutable, default: false */,
      std::unique_ptr<Expr> /* type */,
      std::unique_ptr<Expr> /* opt guard */,
      std::unique_ptr<Expr> /* opt guard result */> FuncParameter;

  class FuncExpr final : public Expr, public Capable {
    const Token *tokId; //!< function identifier
    std::unique_ptr<TemplateDecls> templs;
    std::vector<std::unique_ptr<FuncParameter>> params;
    std::unique_ptr<Expr> returnExpr; // optional
    std::unique_ptr<BodyExpr> body; // optional
    bool autoDetectReturnType;
  public:
    /*!\brief Initializes FuncExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param tokId Return value of getIdentifier()
     * \param templs Return value of getTemplates()
     * \param params Return value of getParameters()
     * \param returnExpr Return value of getReturn()
     * \param body Return value of getBody()
     * \param autoDetectReturnType Return value of isAutoReturnType()
     */
    FuncExpr(const Lexer &lexer, const Position &pos,
      std::unique_ptr<Capabilities> &&caps,
      const Token *tokId,
      std::unique_ptr<TemplateDecls> &&templs,
      std::vector<std::unique_ptr<FuncParameter>> &&params,
      std::unique_ptr<Expr> &&returnExpr,
      std::unique_ptr<BodyExpr> &&body,
      bool autoDetectReturnType = false) noexcept;
    FuncExpr(const FuncExpr &) = delete;
    virtual ~FuncExpr();

    inline const Token &getIdentifier() const noexcept { return *tokId; }
    inline const auto &getTemplates() const noexcept { return templs; }
    inline const auto &getParameters() const noexcept
    { return params; }

    /*!\return Returns optional return expression (return type).
     */
    inline const Expr *getReturn() const noexcept { return returnExpr.get(); }

    /*!\return Returns optional function body.
     */
    inline const BodyExpr *getBody() const noexcept { return body.get(); }

    /*!\return Returns true, if function has auto-detect return type (infered from
     * return statement), otherwise false.
     *
     * If true is returned, getReturn must return nullptr.
     */
    inline bool isAutoReturnType() const noexcept
    { return autoDetectReturnType; }

    virtual std::string toString() const noexcept;
  };

  class FuncTypeExpr final : public Expr {
    std::vector<std::unique_ptr<FuncParameter>> params; // optional
    std::unique_ptr<Expr> returnExpr; // optional
  public:
    /*!\brief Initializes FuncTypeExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param params Return value of getParameters()
     * \param returnExpr Return value of getReturn()
     */
    FuncTypeExpr(const Lexer &lexer, const Position &pos,
      std::vector<std::unique_ptr<FuncParameter>> &&params,
      std::unique_ptr<Expr> &&returnExpr) noexcept;
    virtual ~FuncTypeExpr();

    inline const auto &getParameters() const noexcept
    { return params; }

    inline const Expr *getReturn() const noexcept { return returnExpr.get(); }

    virtual std::string toString() const noexcept;
  };

  class LambdaExpr final : public Expr {
    Exprs params;
    std::unique_ptr<BodyExpr> body;
  public:
    /*!\brief Initializes LambdaExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param params Return value of getParameters()
     * \param body Return value of getBody()
     */
    LambdaExpr(const Lexer &lexer, const Position &pos,
      Exprs &&params,
      std::unique_ptr<BodyExpr> &&body) noexcept;
    LambdaExpr(const LambdaExpr &) = delete;
    virtual ~LambdaExpr();

    inline const auto &getParameters() const noexcept
    { return params; }
    inline const BodyExpr &getBody() const noexcept
    { return *body; }

    virtual std::string toString() const noexcept;
  };


  class TraitExpr final : public Expr, public Capable {
    const Token *tokId; //!< trait identifier
    std::vector<std::unique_ptr<Expr>> impltraits;
    std::unique_ptr<TemplateDecls> templs;
    std::list<std::unique_ptr<FuncExpr>> functions;
  public:
    /*!\brief Initializes TraitExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param tokId Return value of getIdentifier()
     * \param templs Return value of getTemplates()
     * \param impltraits Return value of getInheritedTraits()
     * \param functions Return value of getFunctions()
     */
    TraitExpr(const Lexer &lexer, const Position &pos,
      std::unique_ptr<Capabilities> &&caps,
      const Token *tokId,
      std::unique_ptr<TemplateDecls> &&templs,
      std::vector<std::unique_ptr<Expr>> &&impltraits,
      std::list<std::unique_ptr<FuncExpr>> &&functions) noexcept;
    TraitExpr(const TraitExpr &) = delete;
    virtual ~TraitExpr();

    inline const Token &getIdentifier() const noexcept { return *tokId; }
    inline const auto &getTemplates() const noexcept { return templs; }
    inline const auto &getInheritedTraits() const noexcept
    { return impltraits; }
    inline const auto &getFunctions() const noexcept
    { return functions; }

    virtual std::string toString() const noexcept;
  };

  class ClassExpr final : public Expr, public Capable {
    const Token *tokId;
    std::unique_ptr<TemplateDecls> templs;
    std::list<std::unique_ptr<BiOpExpr>> constructAttributes;
    std::list<std::unique_ptr<BiOpExpr>> attributes;
    std::list<std::unique_ptr<FuncExpr>> functions;
  public:
    /*!\brief Initializes ClassExpr
     * \param lexer
     * \param pos Position in lexer
     * \param tokId Return value of getIdentifier()
     * \param templs Return value of getTemplates()
     * \param constructAttributes Return value of getConstructorAttributes()
     * \param attributes Return value of getAttributes()
     * \param functions Return value of getFunctions()
     */
    ClassExpr(const Lexer &lexer, const Position &pos,
      std::unique_ptr<Capabilities> &&caps,
      const Token *tokId,
      std::unique_ptr<TemplateDecls> &&templs,
      std::list<std::unique_ptr<BiOpExpr>> &&constructAttributes,
      std::list<std::unique_ptr<BiOpExpr>> &&attributes,
      std::list<std::unique_ptr<FuncExpr>> &&functions) noexcept;
    ClassExpr(const ClassExpr &) = delete;
    virtual ~ClassExpr();

    inline const Token &getIdentifier() const noexcept { return *tokId; }
    inline const auto &getTemplates() const noexcept { return templs; }

    inline const auto &getConstructorAttributes() const noexcept
    { return constructAttributes; }

    inline const auto &getAttributes() const noexcept
    { return attributes; }

    inline const auto &getFunctions() const noexcept
    { return functions; }

    virtual std::string toString() const noexcept;
  };

  class TraitImplExpr final : public Expr, public Capable {
    const Token *classTokId;
    std::unique_ptr<TemplateDecls> templs;
    std::unique_ptr<Expr> implTrait;
    std::list<std::unique_ptr<FuncExpr>> functions;
  public:
    /*!\brief Initializes TraitImplExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param classTokId Return value of getIdentifier()
     * \param templs Return value of getTemplates()
     * \param implTrait Return value of getImplementedTrait()
     * \param functions Return value of getFunctions()
     */
    TraitImplExpr(const Lexer &lexer, const Position &pos,
        std::unique_ptr<Capabilities> &&caps,
        const Token *classTokId, std::unique_ptr<TemplateDecls> &&templs,
        std::unique_ptr<Expr> &&implTrait,
        std::list<std::unique_ptr<FuncExpr>> &&functions) noexcept;
    TraitImplExpr(const TraitImplExpr &) = delete;
    virtual ~TraitImplExpr();

    inline const Token &getIdentifier() const noexcept { return *classTokId; }
    inline const auto &getTemplates() const noexcept
    { return templs; }
    inline const Expr &getImplementedTrait() const noexcept
    { return *implTrait; }
    inline const auto &getFunctions() const noexcept
    { return functions; }

    virtual std::string toString() const noexcept;
  };

  typedef std::tuple<const Token*, std::vector<std::unique_ptr<Expr>>> EnumConstructor;

  class EnumExpr final : public Expr {
    const Token *tokId;
    std::unique_ptr<TemplateDecls> templs;
    std::vector<EnumConstructor> constructors;
  public:
    /*!\brief Initializes EnumExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param tokId Return value of getIdentifier()
     * \param templs Return value of getTemplates()
     * \param constructors Return value of getConstructors()
     */
    EnumExpr(const Lexer &lexer, const Position &pos,
        const Token *tokId,
        std::unique_ptr<TemplateDecls> &&templs,
        std::vector<EnumConstructor> &&constructors) noexcept;
    EnumExpr(const EnumExpr &) = delete;
    virtual ~EnumExpr();

    inline const Token &getIdentifier() const noexcept { return *tokId; }
    inline const auto &getTemplates() const noexcept { return templs; }
    inline const auto &getConstructors() const noexcept
    { return constructors; }

    virtual std::string toString() const noexcept;
  };

  class TypeExpr final : public Expr, public Capable {
    const Token *tokId;
    std::unique_ptr<Expr> expr;
  public:
    /*!\brief Initializes TypeExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param TokId Return value of getIdentifier()
     * \param expr Return value of getExpression()
     */
    TypeExpr(const Lexer &lexer, const Position &pos,
        std::unique_ptr<Capabilities> &&caps,
        const Token *tokId, std::unique_ptr<Expr> &&expr) noexcept;
    TypeExpr(const TypeExpr &) = delete;
    virtual ~TypeExpr();

    inline const auto &getIdentifier() const noexcept { return *tokId; }
    inline const Expr &getExpresion() const noexcept { return *expr; }

    virtual std::string toString() const noexcept;
  };

  class ModExpr final : public Expr {
    const Token *tokId;
    Exprs exprs;
  public:
    /*!\brief Initializes ModExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param tokId Return value of getidentifier()
     * \param exprs Return value of getExpressions()
     */
    ModExpr(const Lexer &lexer, const Position &pos,
        const Token *tokId, Exprs &&exprs) noexcept;
    ModExpr(const ModExpr &) = delete;
    virtual ~ModExpr();

    inline const Token &getIdentifier() const noexcept { return *tokId; }
    inline const auto &getExpressions() const noexcept { return exprs; }

    virtual std::string toString() const noexcept;
  };

  class SafeExpr final : public Expr {
    std::unique_ptr<Expr> expr;
  public:
    /*!\brief Initializes SafeExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param expr Return vlaue of getExpression()
     */
    SafeExpr(const Lexer &lexer, const Position &pos,
        std::unique_ptr<Expr> &&expr) noexcept;
    SafeExpr(const SafeExpr &) = delete;
    virtual ~SafeExpr();

    /*!\return Returns memory allocation expression
     * (array, class)
     */
    inline const Expr &getExpression() const noexcept { return *expr; }

    virtual std::string toString() const noexcept;
  };

  typedef std::tuple<std::unique_ptr<Expr> /* cond */,
      std::unique_ptr<BodyExpr>> IfCase;

  class IfExpr final : public Expr {
    std::vector<IfCase> ifCases;
    std::unique_ptr<BodyExpr> elseExpr;
    bool isensure;
  public:
    /*!\brief Initializes IfExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param ifCases Return value of getCases()
     * \param elseExpr Return value of getElse()
     */
    IfExpr(const Lexer &lexer, const Position &pos,
        std::vector<IfCase> &&ifCases,
        std::unique_ptr<BodyExpr> &&elseExpr, bool isensure = false) noexcept;
    IfExpr(const IfExpr &) = delete;
    virtual ~IfExpr();

    inline bool isEnsure() const noexcept { return isensure; }

    inline const auto &getCases() const noexcept
    { return ifCases; }

    /*!\return Returns optional else clause
     */
    inline const BodyExpr *getElse() const noexcept { return elseExpr.get(); }

    virtual std::string toString() const noexcept;
  };

  class LoopExpr final : public Expr {
    std::unique_ptr<Expr> initExpr, condExpr, itExpr;
    std::unique_ptr<BodyExpr> bodyExpr;
  public:
    /*!\brief Initializes LoopExpr
     * \param lexer
     * \param type Either EXPR_LOOP_DO or EXPR_LOOP_FOR
     * \param pos Position in lexer input
     * \param initExpr Return value of getInitialization()
     * \param condExpr Return value of getCondition()
     * \param itExpr Return value of getIterator()
     */
    LoopExpr(const Lexer &lexer, ExprType type, const Position &pos,
        std::unique_ptr<Expr> &&initExpr,
        std::unique_ptr<Expr> &&condExpr,
        std::unique_ptr<Expr> &&itExpr,
        std::unique_ptr<BodyExpr> &&bodyExpr) noexcept;
    LoopExpr(const LoopExpr &) = delete;
    virtual ~LoopExpr();

    /*!\return Returns loop body
     */
    inline const BodyExpr &getBody() const noexcept
    { return *bodyExpr; }

    /*!\return Returns optional initialization expression
     */
    inline const Expr *getInitialization() const noexcept
    { return initExpr.get(); }

    /*!\return Returns loop continuation condition
     */
    inline const Expr &getCondition() const noexcept
    { return *condExpr; }

    /*!\return Returns optional iterate step expression
     */
    inline const Expr *getIterator() const noexcept
    { return itExpr.get(); }

    virtual std::string toString() const noexcept;
  };

  typedef std::tuple<const Token * /* constructor */,
        std::vector<const Token *> /* variables */,
        std::unique_ptr<BodyExpr>> MatchPattern;

  class MatchExpr final : public Expr {
    std::unique_ptr<Expr> expr;
    std::vector<MatchPattern> cases;
    std::unique_ptr<BodyExpr> anyCase;
  public:
    /*!\brief Initializes MatchExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param cases Return vlaue of getCases()
     * \param anyCase Return value of getAnCase()
     */
    MatchExpr(const Lexer &lexer, const Position &pos,
        std::unique_ptr<Expr> &&expr,
        std::vector<MatchPattern> &&cases,
        std::unique_ptr<BodyExpr> &&anyCase) noexcept;
    MatchExpr(const MatchExpr &) = delete;
    virtual ~MatchExpr();

    inline const auto &getExpression() const noexcept { return *expr; }
    inline auto &getExpression() noexcept { return *expr; }

    inline const auto &getCases() const noexcept
    { return cases; }

    /*!\return Returns optional any case
     */
    const BodyExpr *getAnyCase() const noexcept
    { return anyCase.get(); }

    virtual std::string toString() const noexcept;
  };

  class BiOpExpr final : public Expr {
    const Token *tokOp;
    TokenType opType;
    std::unique_ptr<Expr> lhs, rhs;
  public:
    /*!\brief Initializes BiOpExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param tokOp Return value of getOperatorToken()
     * \param opType Return value of getOperatorType()
     * \param lhs Return value of getLeft(), getLeftPtr()
     * \param rhs Return value of getRight(), getRightPtr()
     */
    BiOpExpr(const Lexer &lexer, const Position &pos,
        const Token *tokOp, TokenType opType,
        std::unique_ptr<Expr> &&lhs, std::unique_ptr<Expr> &&rhs) noexcept;
    BiOpExpr(const BiOpExpr &) = delete;
    virtual ~BiOpExpr();

    inline const Token &getOperatorToken() const noexcept { return *tokOp; }
    inline TokenType getOperatorType() const noexcept { return opType; }

    inline std::unique_ptr<Expr> getRightPtr() noexcept
    { return std::move(rhs); }
    inline std::unique_ptr<Expr> getLeftPtr() noexcept
    { return std::move(lhs); }

    inline const Expr &getRight() const noexcept { return *rhs; }
    inline const Expr &getLeft() const noexcept { return *lhs; }

    virtual std::string toString() const noexcept;
  };

  /*!\return Returns true, if the expr is binary operator with the
   * operator 'type', otherwise false is returned.
   */
  inline bool isBiOpExpr(const Expr &expr, TokenType type) noexcept {
    return expr.getType() == EXPR_BIOP &&
      dynamic_cast<const BiOpExpr&>(expr).getOperatorType() == type;
  }

  class UnOpExpr final : public Expr {
    const Token *tokOp;
    std::unique_ptr<Expr> expr;
  public:
    /*!\brief Initializes UnOpExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param tokOp Return value of getOperatorToken()
     * \param opType Return value of getOperatorType()
     * \param expr Return vlaue of getExpression(), getExpresionPtr()
     */
    UnOpExpr(const Lexer &lexer, const Position &pos,
        const Token *tokOp, std::unique_ptr<Expr> &&expr) noexcept;
    UnOpExpr(const UnOpExpr &) = delete;
    virtual ~UnOpExpr();

    inline const Token &getOperatorToken() const noexcept { return *tokOp; }
    inline TokenType getOperatorType() const noexcept { return tokOp->getType(); }

    inline const Expr &getExpression() const noexcept { return *expr; }
    inline std::unique_ptr<Expr> getExpressionPtr() noexcept
    { return std::move(expr); }

    virtual std::string toString() const noexcept;
  };

  inline bool isUnOpExpr(const Expr &expr, TokenType type) noexcept {
    return expr.getType() == EXPR_UNOP &&
      dynamic_cast<const UnOpExpr&>(expr).getOperatorType() == type;
  }

  /*!\brief The type used to jump to different code position
   */
  enum class ReturnControlType {
    NONE,
    RETURN,
    CONTINUE,
    BREAK,
  };

  class BodyExpr final : public Expr {
    Exprs exprs;
    std::unique_ptr<Expr> retExpr;
    ReturnControlType rct;
  public:
    /*!\brief Initializes BodyExpr
     * \param lex
     * \param pos Position in lexer input
     * \param exprs Return value of getExpressions()
     * \param retExpr Return value of getReturn()
     */
    BodyExpr(const Lexer &lex, const Position &pos,
        Exprs &&exprs, std::unique_ptr<Expr> &&retExpr,
        ReturnControlType rct) noexcept;
    BodyExpr(const BodyExpr &) = delete;
    virtual ~BodyExpr();

    inline const auto &getExpressions() const noexcept { return exprs; }

    /*!\brief Optional return statement
     */
    inline const Expr *getReturn() const noexcept { return retExpr.get(); }

    inline ReturnControlType getReturnType() const noexcept { return rct; }

    virtual std::string toString() const noexcept;
  };

  class ArrayCpyExpr final : public Expr {
    std::unique_ptr<Expr> valueExpr, lengthExpr;
  public:
    /*!\brief Initializes ArrayCpyExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param valueexpr Return value of getValue()
     * \param lengthExpr Return value of getLength()
     */
    ArrayCpyExpr(const Lexer &lexer, const Position &pos,
        std::unique_ptr<Expr> &&valuexpr,
        std::unique_ptr<Expr> &&lengthExpr) noexcept;
    ArrayCpyExpr(const ArrayCpyExpr &) = delete;
    virtual ~ArrayCpyExpr();

    inline const Expr &getValue() const noexcept { return *valueExpr; }
    inline const Expr &getLength() const noexcept { return *lengthExpr; }

    virtual std::string toString() const noexcept;
  };

  class ArrayLitExpr final : public Expr {
    std::list<std::unique_ptr<Expr>> exprs;
  public:
    /*!\brief Initializes ArrayLitExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param exprs Return vlaue of getValues().
     */
    ArrayLitExpr(const Lexer &lexer, const Position &pos,
        std::list<std::unique_ptr<Expr>> &&exprs) noexcept;
    ArrayLitExpr(const ArrayLitExpr &) = delete;
    virtual ~ArrayLitExpr();

    /*!\return Returns the array's values. Length is at least 2.
     */
    inline const auto &getValues() const noexcept { return exprs; }

    virtual std::string toString() const noexcept;
  };

  class ArrayEmptyExpr final : public Expr {
    std::unique_ptr<Expr> typeExpr;
  public:
    /*!\brief Initializes ArrayEmptyExpr
     * \param lexer
     * \param pos Position in lexer input
     * \param typeExpr Return value of getType
     */
    ArrayEmptyExpr(const Lexer &lexer, const Position &pos,
        std::unique_ptr<Expr> &&typeExpr) noexcept;
    ArrayEmptyExpr(const ArrayEmptyExpr &) = delete;
    virtual ~ArrayEmptyExpr();

    /*!\return Returns type of every element in the array
     */
    inline const Expr &getType() const noexcept { return *typeExpr; }

    virtual std::string toString() const noexcept;
  };

  class ErrorExpr final : public Expr {
  public:
    inline ErrorExpr(const Lexer &lexer, const Position &pos) noexcept
        : Expr(lexer, EXPR_ERR, pos) {}
    ErrorExpr(const ErrorExpr &) = delete;
    inline virtual ~ErrorExpr() {}

    virtual std::string toString() const noexcept;
  };
}

#endif /* PFEDERC_SYNTAX_EXPR_HPP */
