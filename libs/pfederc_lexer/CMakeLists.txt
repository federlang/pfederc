cmake_minimum_required(VERSION 3.10)
project(pfederc_lexer)

add_library(pfederc_lexer
  "${pfederc_lexer_SOURCE_DIR}/src/lexer.cpp"
  "${pfederc_lexer_SOURCE_DIR}/src/lexer_next.cpp"
  "${pfederc_lexer_SOURCE_DIR}/src/token.cpp")
target_include_directories(pfederc_lexer PUBLIC
  "${pfederc_lexer_SOURCE_DIR}/include")
target_link_libraries(pfederc_lexer PUBLIC pfederc_core pfederc_errors)
