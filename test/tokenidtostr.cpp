#include "pfederc/token.hpp"
using namespace pfederc;

int main() {
  if (TOKEN_TYPE_STRINGS.size() == static_cast<size_t>(TokenType::TOK_ANY) + 1)
    return 0;

  std::cerr << "TOKEN_TYPE_STRINGS.size(): "
    << TOKEN_TYPE_STRINGS.size() << std::endl;
  std::cerr << "TOK_ANY + 1: "
    << static_cast<size_t>(TokenType::TOK_ANY) + 1 << std::endl;

  return 1;
}
