cmake_minimum_required(VERSION 3.10)
project(pfederc_syntax)

add_library(pfederc_syntax
	"${pfederc_syntax_SOURCE_DIR}/src/expr.cpp"
	"${pfederc_syntax_SOURCE_DIR}/src/syntax.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_optimizer.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_array.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_binary.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_capabilities.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_class.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_classtrait.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_enum.cpp"
	"${pfederc_syntax_SOURCE_DIR}/src/syntax_error.cpp"
	"${pfederc_syntax_SOURCE_DIR}/src/syntax_func.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_if.cpp"
	"${pfederc_syntax_SOURCE_DIR}/src/syntax_lambda.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_loop.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_match.cpp"
	"${pfederc_syntax_SOURCE_DIR}/src/syntax_module.cpp"
	"${pfederc_syntax_SOURCE_DIR}/src/syntax_program.cpp"
	"${pfederc_syntax_SOURCE_DIR}/src/syntax_template.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_trait.cpp"
  "${pfederc_syntax_SOURCE_DIR}/src/syntax_type.cpp")
target_include_directories(pfederc_syntax PUBLIC
	"${pfederc_syntax_SOURCE_DIR}/include")
target_link_libraries(pfederc_syntax PUBLIC pfederc_core pfederc_errors
  pfederc_lexer)
add_lto_support(pfederc_syntax)
