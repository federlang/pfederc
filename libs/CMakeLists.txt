cmake_minimum_required(VERSION 3.10)
project(pfederc_libs)

add_subdirectory("${pfederc_libs_SOURCE_DIR}/pfederc_core")
add_subdirectory("${pfederc_libs_SOURCE_DIR}/pfederc_errors")
add_subdirectory("${pfederc_libs_SOURCE_DIR}/pfederc_lexer")
add_subdirectory("${pfederc_libs_SOURCE_DIR}/pfederc_syntax")
add_subdirectory("${pfederc_libs_SOURCE_DIR}/pfederc_semantics")
