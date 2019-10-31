cmake_minimum_required(VERSION 3.10)
project(pfederc)

add_subdirectory("${pfederc_SOURCE_DIR}/libs")

set(PFEDERC_LIBRARIES pfederc_core pfederc_errors pfederc_lexer
  pfederc_syntax)

add_subdirectory("${pfederc_SOURCE_DIR}/test")
