#include "pfederc/syntax.hpp"
using namespace pfederc;

inline static bool couldBeOperator(TokenType type) noexcept {
  return type != TokenType::TOK_EOL && type != TokenType::TOK_EOF && type != TokenType::TOK_STMT
    && type != TokenType::TOK_BRACKET_CLOSE
    && type != TokenType::TOK_ARR_BRACKET_CLOSE
    && type != TokenType::TOK_TEMPL_BRACKET_CLOSE;
}

inline TokenType _getOperatorType(const Token &tok) {
  return isTokenTypeOperator(tok.getType()) ?
    tok.getType() : TokenType::TOK_OP_NONE;
}

inline static bool _binary_continue_condition(const Token &lookahead,
    const Precedence minPrecedence, Precedence &prec) noexcept {
  if (!couldBeOperator(lookahead.getType()))
    return false;

  const auto &info = OPERATORS_INFO.at(_getOperatorType(lookahead));
  prec = std::get<0>(info);
  return std::get<1>(info) == OperatorType::BINARY
    && prec >= minPrecedence;
}

inline static bool _binary_inner_continue_condition(const Token &lookahead,
    const Precedence opPrecedence, Precedence &prec) noexcept {
  if (!couldBeOperator(lookahead.getType()))
    return false;

  const auto &info = OPERATORS_INFO.at(_getOperatorType(lookahead));
  prec = std::get<0>(info);
  return std::get<1>(info) == OperatorType::BINARY &&
    (std::get<0>(info) > opPrecedence ||
    (std::get<0>(info) == opPrecedence &&
    std::get<2>(info) == Associativity::RIGHT));
}

std::unique_ptr<Expr> Parser::parseBinary(std::unique_ptr<Expr> lhs,
    Precedence minPrecedence) noexcept {
  Precedence prec{0};
  Token *lookahead = lexer.getCurrentToken();
  while (_binary_continue_condition(*lookahead, minPrecedence, prec)) {
    const Token *op = lexer.getCurrentToken();
    TokenType optype = _getOperatorType(*op);
    if (optype != TokenType::TOK_OP_NONE)
      lexer.next(); // advance to next unprocessed token
    
    // ignore newline tokens
    while (*lexer.getCurrentToken() == TokenType::TOK_EOL)
      lexer.next();

    std::unique_ptr<Expr> rhs;
    if (TOKEN_BRACKETS.find(optype) != TOKEN_BRACKETS.end()) {
      if (optype == TokenType::TOK_OP_BRACKET_OPEN
          && *lexer.getCurrentToken() == TokenType::TOK_BRACKET_CLOSE) {
        // empty function call, because ()
        const Token *const closingBracket = lexer.getCurrentToken();
        lexer.next(); // eat )
        lhs = std::make_unique<UnOpExpr>(lexer,
          lhs->getPosition() + op->getPosition() + closingBracket->getPosition(),
          op, std::move(lhs));

        lookahead = lexer.getCurrentToken();

        continue;
      } else {
        rhs = parseExpression();
        if (!rhs)
          return nullptr;

        if (!expect(TOKEN_BRACKETS.at(optype))) {
          generateError(std::make_unique<SyntaxError>(LVL_ERROR,
            STX_ERR_BRACKETS.at(optype),
            op->getPosition() + rhs->getPosition() +
            lexer.getCurrentToken()->getPosition()));
          return nullptr;
        }
      }

      lookahead = lexer.getCurrentToken();
    } else {
      rhs = parsePrimary();
      if (!rhs) {
        generateError(std::make_unique<SyntaxError>(LVL_ERROR,
          SyntaxErrorCode::STX_ERR_EXPECTED_PRIMARY_EXPR, op->getPosition()));
          
        return nullptr;
      }

      // lexer already on unprocessed token
      lookahead = lexer.getCurrentToken();
      Precedence innerPrec{0};
      while (_binary_inner_continue_condition(*lookahead, prec, innerPrec)) {
        rhs = parseBinary(std::move(rhs), innerPrec);
        lookahead = lexer.getCurrentToken();
      }
    }

    lhs = std::make_unique<BiOpExpr>(lexer,
      lhs->getPosition() + rhs->getPosition(),
      op, optype, std::move(lhs), std::move(rhs));
  }

  return lhs;
}
