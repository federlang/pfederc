#ifndef PFEDERC_SYNTAX_EXPR_HPP
#define PFEDERC_SYNTAX_EXPR_HPP

#include "pfederc/core.hpp"
#include "pfederc/errors.hpp"

namespace pfederc {
  enum ExprType {
    EXPR_TOKEN,

    EXPR_PROGRAM,
    EXPR_USE,
    EXPR_FUNC,
    EXPR_LAMBDA,
    EXPR_TRAIT,
    EXPR_CLASS,
    EXPR_TRAITIMPL,
    EXPR_ENUM,
    EXPR_MOD,
    EXPR_SAFE,
    EXPR_IF,
    EXPR_LOOP,
    EXPR_MATCH,

    EXPR_BIOP,
    EXPR_UNOP,

    EXPR_BODY,
  };

  class Expr;
  class ProgramExpr;
  class TokenExpr;
  class UseExpr;
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
    virtual ~Expr();

    const Lexer &getLexer() const noexcept;
    ExprType getType() const noexcept;
    const Position &getPosition() const noexcept;
  };

  typedef std::vector<std::unique_ptr<Expr>> Exprs;

  class ProgramExpr : public Expr {
    std::vector<std::string> includes;
    Exprs imports;
    Exprs defs;
  public:
    /*!\brief Initializes ProgramExpr
     * \param pos Return value getPosition()
     * \param includes Return value of getIncludes()
     * \param imports Return value of getImports()
     * \param defs Return value of getDefinitions()
     */
    ProgramExpr(const Lexer &lexer, const Position &pos,
        std::vector<std::string> &&includes,
        Exprs &&imports,
        Exprs &&defs) noexcept;
    virtual ~ProgramExpr();

    const std::vector<std::string> &getIncludes() const noexcept;
    const Exprs &getImports() const noexcept;

    /*!\return Returns vector of *def* expressions. The expressions
     * are ordered by their corresponding position in Feder code.
     */
    const Exprs &getDefinitions() const noexcept;
  };

  class TokenExpr : public Expr {
    const Token &tok;
  public:
    TokenExpr(const Lexer &lexer, const Token &tok) noexcept;
    virtual ~TokenExpr();

    const Token &getToken() const noexcept;
  };

  class UseExpr : public Expr {
    std::unique_ptr<Expr> expr;
  public:
    UseExpr(const Lexer &lexer, std::unique_ptr<Expr> &&expr) noexcept;
    virtual ~UseExpr();

    Expr &getExpression() const noexcept;
  };

  typedef std::tuple<const Token* /* opt varname */,
      std::unique_ptr<Expr> /* type */,
      std::unique_ptr<Expr> /* opt guard */> FuncParameter;

  class FuncExpr : public Expr {
    const Token &tokId; //!< function identifier
    Exprs templs;
    std::vector<FuncParameter> params;
    std::unique_ptr<Expr> returnExpr; // optional
    std::unique_ptr<BodyExpr> body; // optional
  public:
    FuncExpr(const Lexer &lexer, const Position &pos,
      const Token &tokId,
      Exprs &&templ,
      std::vector<FuncParameter> &&params,
      std::unique_ptr<Expr> &&returnExpr,
      std::unique_ptr<BodyExpr> &&body) noexcept;
    virtual ~FuncExpr();

    const Token &getIdentifier() const noexcept;
    const Exprs &getTemplates() const noexcept;
    const std::vector<FuncParameter> &getParameters() const noexcept;

    /*!\return Returns optional return expression (return type).
     */
    const Expr *getReturn() const noexcept;

    /*!\return Returns optional function body.
     */
    const Expr *getBody() const noexcept;
  };

  class LambdaExpr : public Expr {
    Exprs params;
    std::unique_ptr<BodyExpr> body;
  public:
    LambdaExpr(const Lexer &lexer, const Position &pos,
      Exprs &&params,
      std::unique_ptr<BodyExpr> &&body) noexcept;
    virtual ~LambdaExpr();

    const Exprs &getParameters() const noexcept;
    const BodyExpr &getBody() const noexcept;
  };

  class TraitExpr : public Expr {
    const Token &tokId; //!< trait identifier
    Exprs templs;
    std::vector<std::unique_ptr<FuncExpr>> functions;
  public:
    TraitExpr(const Lexer &lexer, const Position &pos,
      const Token &tokId,
      Exprs &&templs,
      std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept;
    virtual ~TraitExpr();

    const Token &getIdentifier() const noexcept;
    const Exprs &getTemplates() const noexcept;
    const std::vector<std::unique_ptr<FuncExpr>> &getFunctions() const noexcept;
  };

  class ClassExpr : public Expr {
    const Token &tokId;
    Exprs templs;
    std::vector<std::unique_ptr<BiOpExpr>> constructAttributes;
    std::vector<std::unique_ptr<BiOpExpr>> attributes;
    std::vector<std::unique_ptr<FuncExpr>> functions;
  public:
    ClassExpr(const Lexer &lexer, const Position &pos,
      const Token &tokId,
      Exprs &&templs,
      std::vector<std::unique_ptr<BiOpExpr>> &&constructAttributes,
      std::vector<std::unique_ptr<BiOpExpr>> &&attributes,
      std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept;
    virtual ~ClassExpr();

    const Token &getIdentifier() const noexcept;
    const Exprs &getTemplates() const noexcept;
    const std::vector<std::unique_ptr<BiOpExpr>> &getConstructorAttributes() const noexcept;
    const std::vector<std::unique_ptr<BiOpExpr>> &getAttributes() const noexcept;
    const std::vector<std::unique_ptr<FuncExpr>> &getFunctions() const noexcept;
  };

  class TraitImplExpr : public Expr {
    const Token &classTokId;
    Exprs templs;
    std::unique_ptr<Expr> implTrait;
    std::vector<std::unique_ptr<FuncExpr>> functions;
  public:
    TraitImplExpr(const Lexer &lexer, const Position &pos,
        const Token &classTokId, Exprs &&templs,
        std::unique_ptr<Expr> &&implTrait,
        std::vector<std::unique_ptr<FuncExpr>> &&functions) noexcept;
    virtual ~TraitImplExpr();

    const Token &getIdentifier() const noexcept;
    const std::vector<std::unique_ptr<Expr>> &getTemplates() const noexcept;
    const Expr &getImplementedTrait() const noexcept;
    const std::vector<std::unique_ptr<FuncExpr>> &getFunctions() const noexcept;
  };

  typedef std::tuple<const Token&, std::vector<std::unique_ptr<Expr>>> EnumConstructor;

  class EnumExpr : public Expr {
    const Token &tokId;
    Exprs templs;
    std::vector<EnumConstructor> constructors;
  public:
    EnumExpr(const Lexer &lexer, const Position &pos,
        const Token &tokId,
        Exprs &&templs,
        EnumConstructor &&contructors) noexcept;
    virtual ~EnumExpr();

    const Token &getIdentifier() const noexcept;
    const Exprs &getTemplates() const noexcept;
    const std::vector<EnumConstructor> &getConstructors() const noexcept;
  };
}

#endif /* PFEDERC_SYNTAX_EXPR_HPP */
