cmake_minimum_required(VERSION 3.10)
project(pfederc)

add_subdirectory("${pfederc_SOURCE_DIR}/libs")

set(PFEDERC_LIBRARIES pfederc_core pfederc_errors pfederc_lexer
  pfederc_syntax pfederc_semantics)

add_executable(pfederc "${pfederc_SOURCE_DIR}/src/main.cpp")
target_include_directories(pfederc PUBLIC "${pfederc_SOURCE_DIR}/include")
target_link_libraries(pfederc ${PFEDERC_LIBRARIES})

add_subdirectory("${pfederc_SOURCE_DIR}/test")
