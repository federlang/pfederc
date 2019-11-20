#include "pfederc/syntax.hpp"
using namespace pfederc;

inline static bool _binary_continue_condition(const Token &lookahead,
    const Precedence minPrecedence, Precedence &prec) noexcept {
  if (!isTokenTypeOperator(lookahead.getType()))
    return false;

  const auto &info = OPERATORS_INFO.at(lookahead.getType());
  prec = std::get<0>(info);
  return std::get<1>(info) == OperatorType::BINARY
    && prec >= minPrecedence;
}

inline static bool _binary_inner_continue_condition(const Token &lookahead,
    const Precedence opPrecedence, Precedence &prec) noexcept {
  if (!isTokenTypeOperator(lookahead.getType()))
    return false;

  const auto &info = OPERATORS_INFO.at(lookahead.getType());
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
    lexer.next(); // advance to next unprocessed token
    
    // ignore newline tekons
    while (*lexer.getCurrentToken() == TOK_EOL)
      lexer.next();

    std::unique_ptr<Expr> rhs;
    if (TOKEN_BRACKETS.find(op->getType()) != TOKEN_BRACKETS.end()) {
      if (*op == TOK_OP_BRACKET_OPEN
          && *lexer.getCurrentToken() == TOK_BRACKET_CLOSE) {
        const Token *const closingBracket = lexer.getCurrentToken();
        // empty function call
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

        if (!expect(TOKEN_BRACKETS.at(op->getType()))) {
          generateError(std::make_unique<SyntaxError>(LVL_ERROR,
            STX_ERR_BRACKETS.at(op->getType()),
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
          STX_ERR_EXPECTED_PRIMARY_EXPR, op->getPosition()));
          
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
      op, std::move(lhs), std::move(rhs));
  }

  return lhs;
}
