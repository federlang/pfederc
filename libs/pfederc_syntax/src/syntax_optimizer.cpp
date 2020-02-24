#include "pfederc/syntax_optimizer.hpp"
using namespace pfederc;

std::unique_ptr<Expr> pfederc::optimizeAll(std::unique_ptr<Expr> &&expr,
    size_t &reducedexpressions) noexcept {
  auto tpl = optimize(std::move(expr), reducedexpressions);
  while (std::get<1>(tpl)) {
    tpl = optimize(std::move(std::move(std::get<0>(tpl))), reducedexpressions);
  }

  return std::move(std::get<0>(tpl));
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimize(
    std::unique_ptr<Expr> &&expr, size_t &reducedexpressions) noexcept {
  Expr *pexpr = expr.release();
  switch (pexpr->getType()) {
  case ExprType::EXPR_PROG:
    return optimizeProgramExpr(dynamic_cast<ProgramExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_FUNC:
    return optimizeFuncExpr(dynamic_cast<FuncExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_LAMBDA:
    return optimizeLambdaExpr(dynamic_cast<LambdaExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_CLASS:
    return optimizeClassExpr(dynamic_cast<ClassExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_TRAITIMPL:
    return optimizeTraitImplExpr(dynamic_cast<TraitImplExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_MOD:
    return optimizeModExpr(dynamic_cast<ModExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_SAFE:
    return optimizeSafeExpr(dynamic_cast<SafeExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_IF:
    return optimizeIfExpr(dynamic_cast<IfExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_LOOP_FOR:
  case ExprType::EXPR_LOOP_DO:
    return optimizeLoopExpr(dynamic_cast<LoopExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_MATCH:
    return optimizeMatchExpr(dynamic_cast<MatchExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_BIOP:
    return optimizeBiOpExpr(dynamic_cast<BiOpExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_UNOP:
    return optimizeUnOpExpr(dynamic_cast<UnOpExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_BODY:
    return optimizeBodyExpr(dynamic_cast<BodyExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_ARRCPY:
    return optimizeArrayCpyExpr(dynamic_cast<ArrayCpyExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_ARRLIT:
    return optimizeArrayLitExpr(dynamic_cast<ArrayLitExpr*>(pexpr), reducedexpressions);
  case ExprType::EXPR_ARREMPTY:
    return optimizeArrayEmptyExpr(dynamic_cast<ArrayEmptyExpr*>(pexpr), reducedexpressions);
  default:
    return std::tuple<std::unique_ptr<Expr>, bool>(
        std::unique_ptr<Expr>(pexpr), false);
  }
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeProgramExpr(
    ProgramExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeFuncExpr(
    FuncExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeLambdaExpr(
    LambdaExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeClassExpr(
    ClassExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeTraitImplExpr(
    TraitImplExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeModExpr(
    ModExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeSafeExpr(
    SafeExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeIfExpr(
    IfExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeLoopExpr(
    LoopExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeMatchExpr(
    MatchExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

template<class R, class T>
static R _computeNumberOperation(TokenType type, T x, T y) noexcept {
	switch (type) {
	case TokenType::TOK_OP_ADD:
		return static_cast<R>(x + y);
  case TokenType::TOK_OP_SUB:
		return static_cast<R>(x - y);
  case TokenType::TOK_OP_MUL:
		return static_cast<R>(x * y);
  case TokenType::TOK_OP_DIV:
		return static_cast<R>(x / y);
	default:
		fatal(__FILE__, __LINE__, "Unknown operation");
		return 0;
	}
}

template<class R, class T>
static R _computeIntegerOperation(TokenType type, T x, T y) noexcept {
	switch (type) {
	case TokenType::TOK_OP_ADD:
		return static_cast<R>(x + y);
  case TokenType::TOK_OP_SUB:
		return static_cast<R>(x - y);
  case TokenType::TOK_OP_MUL:
		return static_cast<R>(x * y);
  case TokenType::TOK_OP_DIV:
		return static_cast<R>(x / y);
  case TokenType::TOK_OP_MOD:
		return static_cast<R>(x % y);
	default:
		fatal(__FILE__, __LINE__, "Unknown operation");
		return 0;
	}
}

template<typename R>
static R _computeNumberBoolOperation(TokenType type, const NumberToken &lhs, const NumberToken &rhs) {
	switch (type) {
	case TokenType::TOK_OP_LT:
		return lhs.getNumber<R>() < rhs.getNumber<R>();
	case TokenType::TOK_OP_GT:
		return lhs.getNumber<R>() > rhs.getNumber<R>();
	case TokenType::TOK_OP_LEQ:
		return lhs.getNumber<R>() <= rhs.getNumber<R>();
	case TokenType::TOK_OP_GEQ:
		return lhs.getNumber<R>() >= rhs.getNumber<R>();
	case TokenType::TOK_OP_EQ:
		return lhs.getNumber<R>() == rhs.getNumber<R>();
	case TokenType::TOK_OP_NQ:
		return lhs.getNumber<R>() != rhs.getNumber<R>();
	default:
		fatal(__FILE__, __LINE__, "Unknown operation");
		return 0;
	}
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeBiOpExpr(
    BiOpExpr *expr, size_t &reducedexpressions) noexcept {

  const size_t oldreducedexpressions{reducedexpressions};
  std::unique_ptr<Expr> lhs = optimizeAll(expr->getLeftPtr(), reducedexpressions);
  std::unique_ptr<Expr> rhs = optimizeAll(expr->getRightPtr(), reducedexpressions);

  // Expression: str str
  // Target: str
  if (expr->getOperatorType() == TokenType::TOK_OP_NONE
      && isTokenExpr(*lhs, TokenType::TOK_STR) && isTokenExpr(*rhs, TokenType::TOK_STR)) {
    const std::string strlhs = lhs->toString(), strrhs = rhs->toString();
    const std::string merged =
      strlhs.substr(0, strlhs.length() - 1) + strrhs.substr(1);
    std::unique_ptr<Expr> newexpr = std::make_unique<FakeTokenExpr>(
        expr->getLexer(), std::make_unique<StringToken>(
          dynamic_cast<TokenExpr&>(*lhs).getToken().getLast(),
          TokenType::TOK_STR,
          expr->getPosition(), merged));
    delete expr;

    reducedexpressions++;
    return std::tuple<std::unique_ptr<Expr>, bool>(
        std::move(newexpr), oldreducedexpressions != reducedexpressions);
  }

	// num `op` num
	if (lhs->getType() == ExprType::EXPR_TOK && rhs->getType() == ExprType::EXPR_TOK
			&& isNumberType(dynamic_cast<TokenExpr&>(*lhs).getToken().getType())
			&& dynamic_cast<TokenExpr&>(*lhs).getToken().getType() == dynamic_cast<TokenExpr&>(*rhs).getToken().getType()
			&& (expr->getOperatorType() == TokenType::TOK_OP_ADD
				|| expr->getOperatorType() == TokenType::TOK_OP_SUB
				|| expr->getOperatorType() == TokenType::TOK_OP_MUL
				|| expr->getOperatorType() == TokenType::TOK_OP_DIV)) {
		NumberToken &lhsTok = dynamic_cast<NumberToken&>(dynamic_cast<TokenExpr&>(*lhs).getToken());
		NumberToken &rhsTok = dynamic_cast<NumberToken&>(dynamic_cast<TokenExpr&>(*rhs).getToken());

		std::unique_ptr<NumberToken> tok(nullptr);

		switch (lhsTok.getType()) {
    case TokenType::TOK_INT8:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<int64_t, int8_t>(expr->getOperatorType(),
						lhsTok.i8(), rhsTok.i8()));
			break;
    case TokenType::TOK_INT16:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<int64_t, int16_t>(expr->getOperatorType(),
						lhsTok.i16(), rhsTok.i16()));
			break;
    case TokenType::TOK_INT32:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<int64_t, int32_t>(expr->getOperatorType(),
						lhsTok.i32(), rhsTok.i32()));
			break;
    case TokenType::TOK_INT64:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<int64_t, int64_t>(expr->getOperatorType(),
						lhsTok.i64(), rhsTok.i64()));
			break;
    case TokenType::TOK_UINT8:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<uint64_t, uint8_t>(expr->getOperatorType(),
						lhsTok.u8(), rhsTok.u8()));
			break;
    case TokenType::TOK_UINT16:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<uint64_t, uint16_t>(expr->getOperatorType(),
						lhsTok.u16(), rhsTok.u16()));
			break;
    case TokenType::TOK_UINT32:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<uint64_t, uint32_t>(expr->getOperatorType(),
						lhsTok.u32(), rhsTok.u32()));
			break;
    case TokenType::TOK_UINT64:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<uint64_t, uint64_t>(expr->getOperatorType(),
						lhsTok.u64(), rhsTok.u64()));
			break;
    case TokenType::TOK_FLT32:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<float, float>(expr->getOperatorType(),
						lhsTok.f32(), rhsTok.f32()));
			break;
    case TokenType::TOK_FLT64:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeNumberOperation<double, double>(expr->getOperatorType(),
						lhsTok.f64(), rhsTok.f64()));
			break;
    default:
			fatal(__FILE__, __LINE__, "Unknown number type");
			break;
		}

		std::unique_ptr<Expr> newexpr = std::make_unique<FakeTokenExpr>(
				expr->getLexer(), std::move(tok));
		delete expr;

		reducedexpressions++;

		return std::tuple<std::unique_ptr<Expr>, bool>(std::move(newexpr), true);
	}

	// num % num
	if (lhs->getType() == ExprType::EXPR_TOK && rhs->getType() == ExprType::EXPR_TOK
			&& isNumberType(dynamic_cast<TokenExpr&>(*lhs).getToken().getType())
			&& dynamic_cast<TokenExpr&>(*lhs).getToken().getType() == dynamic_cast<TokenExpr&>(*rhs).getToken().getType()
			&& (expr->getOperatorType() == TokenType::TOK_OP_MOD)) {
		NumberToken &lhsTok = dynamic_cast<NumberToken&>(dynamic_cast<TokenExpr&>(*lhs).getToken());
		NumberToken &rhsTok = dynamic_cast<NumberToken&>(dynamic_cast<TokenExpr&>(*rhs).getToken());

		std::unique_ptr<NumberToken> tok(nullptr);

		switch (lhsTok.getType()) {
    case TokenType::TOK_INT8:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeIntegerOperation<int64_t, int8_t>(expr->getOperatorType(),
						lhsTok.i8(), rhsTok.i8()));
			break;
    case TokenType::TOK_INT16:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeIntegerOperation<int64_t, int16_t>(expr->getOperatorType(),
						lhsTok.i16(), rhsTok.i16()));
			break;
    case TokenType::TOK_INT32:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeIntegerOperation<int64_t, int32_t>(expr->getOperatorType(),
						lhsTok.i32(), rhsTok.i32()));
			break;
    case TokenType::TOK_INT64:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeIntegerOperation<int64_t, int64_t>(expr->getOperatorType(),
						lhsTok.i64(), rhsTok.i64()));
			break;
    case TokenType::TOK_UINT8:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeIntegerOperation<uint64_t, uint8_t>(expr->getOperatorType(),
						lhsTok.u8(), rhsTok.u8()));
			break;
    case TokenType::TOK_UINT16:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeIntegerOperation<uint64_t, uint16_t>(expr->getOperatorType(),
						lhsTok.u16(), rhsTok.u16()));
			break;
    case TokenType::TOK_UINT32:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeIntegerOperation<uint64_t, uint32_t>(expr->getOperatorType(),
						lhsTok.u32(), rhsTok.u32()));
			break;
    case TokenType::TOK_UINT64:
			tok = std::make_unique<NumberToken>(lhsTok.getLast(),
					lhsTok.getType(), expr->getPosition(),
					_computeIntegerOperation<uint64_t, uint64_t>(expr->getOperatorType(),
						lhsTok.u64(), rhsTok.u64()));
			break;
		default:
			fatal(__FILE__, __LINE__, "Unknown number type");
			break;
		}

		std::unique_ptr<Expr> newexpr = std::make_unique<FakeTokenExpr>(
				expr->getLexer(), std::move(tok));
		delete expr;

		reducedexpressions++;

		return std::tuple<std::unique_ptr<Expr>, bool>(std::move(newexpr), true);
	}

	// typeof(num `op` num) == bool
	if (lhs->getType() == ExprType::EXPR_TOK && rhs->getType() == ExprType::EXPR_TOK
			&& isNumberType(dynamic_cast<TokenExpr&>(*lhs).getToken().getType())
			&& dynamic_cast<TokenExpr&>(*lhs).getToken().getType() == dynamic_cast<TokenExpr&>(*rhs).getToken().getType()
			&& (expr->getOperatorType() == TokenType::TOK_OP_LT
				|| expr->getOperatorType() == TokenType::TOK_OP_GT
				|| expr->getOperatorType() == TokenType::TOK_OP_LEQ
				|| expr->getOperatorType() == TokenType::TOK_OP_GEQ
				|| expr->getOperatorType() == TokenType::TOK_OP_EQ
				|| expr->getOperatorType() == TokenType::TOK_OP_NQ)) {
		NumberToken &lhsTok = dynamic_cast<NumberToken&>(dynamic_cast<TokenExpr&>(*lhs).getToken());
		NumberToken &rhsTok = dynamic_cast<NumberToken&>(dynamic_cast<TokenExpr&>(*rhs).getToken());

		std::unique_ptr<Token> tok(nullptr);
		switch (lhsTok.getType()) {
			case TokenType::TOK_INT8:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<int8_t>(expr->getOperatorType(), lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
			case TokenType::TOK_INT16:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<int16_t>(expr->getOperatorType(),
							lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
			case TokenType::TOK_INT32:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<int32_t>(expr->getOperatorType(),
							lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
			case TokenType::TOK_INT64:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<int64_t>(expr->getOperatorType(),
							lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
			case TokenType::TOK_UINT8:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<uint8_t>(expr->getOperatorType(),
							lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
			case TokenType::TOK_UINT16:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<uint16_t>(expr->getOperatorType(),
							lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
			case TokenType::TOK_UINT32:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<uint32_t>(expr->getOperatorType(),
							lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
			case TokenType::TOK_UINT64:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<uint64_t>(expr->getOperatorType(),
							lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
			case TokenType::TOK_FLT32:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<float>(expr->getOperatorType(),
							lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
			case TokenType::TOK_FLT64:
				tok = std::make_unique<Token>(lhsTok.getLast(),
						_computeNumberBoolOperation<double>(expr->getOperatorType(),
							lhsTok, rhsTok) ? TokenType::TOK_KW_TRUE : TokenType::TOK_KW_FALSE,
							expr->getPosition());
				break;
		}

		std::unique_ptr<Expr> newexpr = std::make_unique<FakeTokenExpr>(
				expr->getLexer(), std::move(tok));
		delete expr;

		reducedexpressions++;

		return std::tuple<std::unique_ptr<Expr>, bool>(std::move(newexpr), true);
	}

  std::unique_ptr<Expr> newexpr = std::make_unique<BiOpExpr>(expr->getLexer(),
    expr->getPosition(), &(expr->getOperatorToken()), expr->getOperatorType(),
    std::move(lhs), std::move(rhs));
  delete expr;

  return std::tuple<std::unique_ptr<Expr>, bool>(
    std::move(newexpr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeUnOpExpr(
    UnOpExpr *expr, size_t &reducedexpressions) noexcept {
  std::unique_ptr<Expr> newincludedexpr = optimizeAll(expr->getExpressionPtr(), reducedexpressions);
  std::unique_ptr<Expr> newexpr = std::make_unique<UnOpExpr>(expr->getLexer(),
      expr->getPosition(), &(expr->getOperatorToken()),
        std::move(newincludedexpr));
  delete expr;

  return std::tuple<std::unique_ptr<Expr>, bool>(std::move(newexpr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeBodyExpr(
    BodyExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeArrayCpyExpr(
    ArrayCpyExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeArrayLitExpr(
    ArrayLitExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}

std::tuple<std::unique_ptr<Expr>, bool /* changed */> pfederc::optimizeArrayEmptyExpr(
    ArrayEmptyExpr *expr, size_t &reducedexpressions) noexcept {

  return std::tuple<std::unique_ptr<Expr>, bool>(
      std::unique_ptr<Expr>(expr), false);
}
