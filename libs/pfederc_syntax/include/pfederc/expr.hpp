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

    inline virtual std::string toString() const noexcept = 0;
  };

  typedef std::tuple<std::vector<const Token*> /* idcall */,
    const Token * /* operator */,
    std::unique_ptr<Expr> /* comparison */> CapabilityEnsure;

  class Capabilities final {
    bool isunused, isinline, isconstant;
    std::vector<CapabilityEnsure> required;
    std::vector<CapabilityEnsure> ensures;
  public:
    inline Capabilities(bool isunused, bool isinline, bool isconstant,
        std::vector<CapabilityEnsure> &&required,
        std::vector<CapabilityEnsure> &&ensures) noexcept
        : isunused{isunused}, isinline{isinline}, isconstant{isconstant},
          required(std::move(required)), ensures(std::move(ensures)) {}
    ~Capabilities();

    bool isUnused() const noexcept { return isunused; }
    bool isInline() const noexcept { return isinline; }
    bool isConstant() const noexcept { return isconstant; }

    const auto &getRequired() const noexcept { return required; }
    const auto &getEnsures() const noexcept { return ensures; }
  };

  typedef std::tuple<const Token * /* id */,
    std::unique_ptr<Expr>> TemplateDecl;
  typedef std::vector<std::unique_ptr<TemplateDecl>> TemplateDecls;

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

  class TokenExpr final : public Expr {
    const Token *tok;
  public:
    TokenExpr(const Lexer &lexer, const Token *tok) noexcept;
    TokenExpr(const TokenExpr &) = delete;
    virtual ~TokenExpr();

    inline const Token &getToken() const noexcept { return *tok; }

    virtual std::string toString() const noexcept;
  };

  inline bool isTokenExpr(const Expr &expr, TokenType type) {
    return expr.getType() == EXPR_TOK &&
      dynamic_cast<const TokenExpr&>(expr).getToken() == type;
  }

  class UseExpr final : public Expr {
    Exprs exprs;
  public:
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
    ProgNameExpr(const Lexer &lexer, const Token *tok) noexcept;
    virtual ~ProgNameExpr();

    inline const Token &getToken() const noexcept { return *tok; }

    virtual std::string toString() const noexcept;
  };

  typedef std::tuple<const Token* /* opt varname */,
      std::unique_ptr<Expr> /* type */,
      std::unique_ptr<Expr> /* opt guard */,
      std::unique_ptr<Expr> /* opt guard result */> FuncParameter;

  class FuncExpr final : public Expr {
    const Token *tokId; //!< function identifier
    std::unique_ptr<TemplateDecls> templs;
    std::vector<std::unique_ptr<FuncParameter>> params;
    std::unique_ptr<Expr> returnExpr; // optional
    std::unique_ptr<BodyExpr> body; // optional
    bool autoDetectReturnType;
  public:
    FuncExpr(const Lexer &lexer, const Position &pos,
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

    inline bool isAutoReturnType() const noexcept
    { return autoDetectReturnType; }

    virtual std::string toString() const noexcept;
  };

  class FuncTypeExpr final : public Expr {
    std::vector<std::unique_ptr<FuncParameter>> params; // optional
    std::unique_ptr<Expr> returnExpr; // optional
  public:
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

  class TraitExpr final : public Expr {
    const Token *tokId; //!< trait identifier
    std::unique_ptr<TemplateDecls> templs;
    std::vector<std::unique_ptr<FuncExpr>> functions;
  public:
    TraitExpr(const Lexer &lexer, const Position &pos,
      const Token *tokId,
      std::unique_ptr<TemplateDecls> &&templs,
      std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept;
    TraitExpr(const TraitExpr &) = delete;
    virtual ~TraitExpr();

    inline const Token &getIdentifier() const noexcept { return *tokId; }
    inline const auto &getTemplates() const noexcept { return templs; }
    inline const auto &getFunctions() const noexcept
    { return functions; }

    virtual std::string toString() const noexcept;
  };

  class ClassExpr final : public Expr {
    const Token *tokId;
    std::unique_ptr<TemplateDecls> templs;
    std::vector<std::unique_ptr<BiOpExpr>> constructAttributes;
    std::vector<std::unique_ptr<BiOpExpr>> attributes;
    std::vector<std::unique_ptr<FuncExpr>> functions;
  public:
    ClassExpr(const Lexer &lexer, const Position &pos,
      const Token *tokId,
      std::unique_ptr<TemplateDecls> &&templs,
      std::vector<std::unique_ptr<BiOpExpr>> &&constructAttributes,
      std::vector<std::unique_ptr<BiOpExpr>> &&attributes,
      std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept;
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

  class TraitImplExpr final : public Expr {
    const Token *classTokId;
    std::unique_ptr<TemplateDecls> templs;
    std::unique_ptr<Expr> implTrait;
    std::vector<std::unique_ptr<FuncExpr>> functions;
  public:
    TraitImplExpr(const Lexer &lexer, const Position &pos,
        const Token *classTokId, std::unique_ptr<TemplateDecls> &&templs,
        std::unique_ptr<Expr> &&implTrait,
        std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept;
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

  typedef std::tuple<const Token&, std::vector<std::unique_ptr<Expr>>> EnumConstructor;

  class EnumExpr final : public Expr {
    const Token *tokId;
    std::unique_ptr<TemplateDecls> templs;
    std::vector<EnumConstructor> constructors;
  public:
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

  class ModExpr final : public Expr {
    const Token *tokId;
    Exprs exprs;
  public:
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
     * \param pos
     * \param ifCases size() >= 1
     * \param elseExpr optional else expression
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
  public:
    /*!\brief Initializes LoopExpr
     * \param lexer
     * \param type Either EXPR_LOOP_DO or EXPR_LOOP_FOR
     * \param pos
     * \param initExpr Optional init expression (statement)
     * \param condExpr Loop continuation condition
     * \param itExpr Optional iterator step expression
     */
    LoopExpr(const Lexer &lexer, ExprType type, const Position &pos,
        std::unique_ptr<Expr> &&initExpr,
        std::unique_ptr<Expr> &&condExpr,
        std::unique_ptr<Expr> &&itExpr) noexcept;
    LoopExpr(const LoopExpr &) = delete;
    virtual ~LoopExpr();

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

  typedef std::tuple<const std::vector<Token *> /* constructors */,
      const std::vector<const Token * /* variables */>,
      std::unique_ptr<BodyExpr>> MatchPattern;

  class MatchExpr final : public Expr {
    std::vector<MatchPattern> cases;
    std::unique_ptr<BodyExpr> anyCase;
  public:
    /*!\brief Initializes MatchExpr
     * \param lexer
     * \param pos
     * \param cases
     * \param anyCase Optional any case
     */
    MatchExpr(const Lexer &lexer, const Position &pos,
        std::vector<MatchPattern> &&cases,
        std::unique_ptr<BodyExpr> &&anyCase) noexcept;
    MatchExpr(const MatchExpr &) = delete;
    virtual ~MatchExpr();

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
    std::unique_ptr<Expr> lhs, rhs;
  public:
    BiOpExpr(const Lexer &lexer, const Position &pos,
        const Token *tokOp,
        std::unique_ptr<Expr> &&lhs, std::unique_ptr<Expr> &&rhs) noexcept;
    BiOpExpr(const BiOpExpr &) = delete;
    virtual ~BiOpExpr();

    inline const Token &getOperator() const noexcept { return *tokOp; }

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
      dynamic_cast<const BiOpExpr&>(expr).getOperator() == type;
  }

  class UnOpExpr final : public Expr {
    const Token *tokOp;
    std::unique_ptr<Expr> expr;
  public:
    UnOpExpr(const Lexer &lexer, const Position &pos,
        const Token *tokOp, std::unique_ptr<Expr> &&expr) noexcept;
    UnOpExpr(const UnOpExpr &) = delete;
    virtual ~UnOpExpr();

    inline const Token &getOperator() const noexcept { return *tokOp; }

    inline const Expr &getExpression() const noexcept { return *expr; }

    virtual std::string toString() const noexcept;
  };

  class BodyExpr final : public Expr {
    Exprs exprs;
    std::unique_ptr<Expr> retExpr;
  public:
    BodyExpr(const Lexer &lex, const Position &pos,
        Exprs &&exprs, std::unique_ptr<Expr> &&retExpr) noexcept;
    BodyExpr(const BodyExpr &) = delete;
    virtual ~BodyExpr();

    inline const auto &getExpressions() const noexcept { return exprs; }

    /*!\brief Optional return statement
     */
    inline const Expr *getReturn() const noexcept { return retExpr.get(); }

    virtual std::string toString() const noexcept;
  };

  class ArrayCpyExpr final : public Expr {
    std::unique_ptr<Expr> valueExpr, lengthExpr;
  public:
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
     * \param pos
     * \param exprs Length is at least 2
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
