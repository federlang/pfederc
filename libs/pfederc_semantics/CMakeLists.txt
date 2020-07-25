cmake_minimum_required(VERSION 3.10)
project(pfederc_semantics)

add_library(pfederc_semantics
  "${pfederc_semantics_SOURCE_DIR}/src/semantics.cpp"
  "${pfederc_semantics_SOURCE_DIR}/src/semantics_stdlib.cpp")
target_include_directories(pfederc_semantics PUBLIC
  "${pfederc_semantics_SOURCE_DIR}/include")
target_link_libraries(pfederc_semantics PUBLIC pfederc_core pfederc_errors
  pfederc_lexer)
add_lto_support(pfederc_semantics)
