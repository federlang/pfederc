cmake_minimum_required(VERSION 3.10)
project(pfederc_test)

include(CTest)
enable_testing()

find_program(VALGRIND "valgrind")
if (VALGRIND)
	set(VALGRIND "${VALGRIND}" --tool=memcheck --leak-check=yes)
endif()

macro(build_test name)
	add_executable("${name}"
		"${pfederc_test_SOURCE_DIR}/${name}.cpp")
	target_link_libraries("${name}" ${PFEDERC_LIBRARIES})
endmacro()

macro(status_test name)
	build_test("${name}")
	add_test("${name}" "${name}")
endmacro()

macro(status_test_arg testname name args)
	add_test(NAME "${testname}" COMMAND "${name}" ${args})
endmacro()

macro(match_test testname name args match)
	add_test(NAME "${testname}" COMMAND "${name}" ${args})
	set_property(TEST "${testname}" PROPERTY PASS_REGULAR_EXPRESSION "${match}")
endmacro()

macro(valgrind_test testname name args)
	if (VALGRIND)
		add_test(NAME "${testname}" COMMAND ${VALGRIND} "${name}" ${args})
	endif()
endmacro()

status_test(tokenidtostr)

build_test(tokenid)
build_test(tokenidmem)
build_test(ast)
build_test(astmem)

# Identifiers
# good
match_test(tokenid_id00 tokenid "variable" "^TOK_ID\n$")
match_test(tokenid_id01 tokenid "_Variable" "^TOK_ID\n$")
match_test(tokenid_id02 tokenid "_Var0123456789" "^TOK_ID\n$")
match_test(tokenid_id03 tokenid "snake_cases_used" "^TOK_ID\n$")
match_test(tokenid_id04 tokenid "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "^TOK_ID\n$")
match_test(tokenid_id05 tokenid "abcdefghijklmnopqrtstuvwyz" "^TOK_ID\n$")
## bad
match_test(tokenid_id50 tokenid "__" "^TOK_ERR\n")
match_test(tokenid_id51 tokenid "_0" "^TOK_ERR\n")
# Keywords
match_test(tokenid_kw00 tokenid "func" "^TOK_KW_FN\n$")
match_test(tokenid_kw01 tokenid "mod" "^TOK_KW_MOD\n$")
match_test(tokenid_kw02 tokenid "class" "^TOK_KW_CLASS\n$")
match_test(tokenid_kw03 tokenid "enum" "^TOK_KW_ENUM\n$")
match_test(tokenid_kw04 tokenid "trait" "^TOK_KW_TRAIT\n$")
match_test(tokenid_kw05 tokenid "type" "^TOK_KW_TYPE\n$")
match_test(tokenid_kw06 tokenid "return" "^TOK_KW_RET\n$")
match_test(tokenid_kw07 tokenid "use" "^TOK_KW_USE\n$")
match_test(tokenid_kw08 tokenid "if" "^TOK_KW_IF\n$")
match_test(tokenid_kw09 tokenid "else" "^TOK_KW_ELSE\n$")
match_test(tokenid_kw10 tokenid "match" "^TOK_KW_MATCH\n$")
match_test(tokenid_kw11 tokenid "switch" "^TOK_KW_SWITCH\n$")
match_test(tokenid_kw12 tokenid "for" "^TOK_KW_FOR\n$")
match_test(tokenid_kw13 tokenid "do" "^TOK_KW_DO\n$")
match_test(tokenid_kw14 tokenid "continue" "^TOK_KW_CTN\n$")
match_test(tokenid_kw15 tokenid "break" "^TOK_KW_BRK\n$")
match_test(tokenid_kw16 tokenid "include" "^TOK_KW_INC\n$")
match_test(tokenid_kw17 tokenid "import" "^TOK_KW_IMPORT\n$")
match_test(tokenid_kw18 tokenid "safe" "^TOK_KW_SAFE\n$")
match_test(tokenid_kw19 tokenid "True" "^TOK_KW_TRUE\n$")
match_test(tokenid_kw20 tokenid "False" "^TOK_KW_FALSE\n$")
match_test(tokenid_kw21 tokenid "lambda" "^TOK_KW_LAMBDA\n$")
match_test(tokenid_kw22 tokenid "ensure" "^TOK_KW_ENSURE\n$")
# Operators
match_test(tokenid_op00 tokenid "," "^TOK_OP_COMMA\n$")
match_test(tokenid_op01 tokenid ":=" "^TOK_OP_ASG_DCL\n$")
match_test(tokenid_op02 tokenid "&=" "^TOK_OP_ASG_AND\n$")
match_test(tokenid_op03 tokenid "^=" "^TOK_OP_ASG_XOR\n$")
match_test(tokenid_op04 tokenid "|=" "^TOK_OP_ASG_OR\n$")
match_test(tokenid_op05 tokenid "<<=" "^TOK_OP_ASG_LSH\n$")
match_test(tokenid_op06 tokenid ">>=" "^TOK_OP_ASG_RSH\n$")
match_test(tokenid_op07 tokenid "%=" "^TOK_OP_ASG_MOD\n$")
match_test(tokenid_op08 tokenid "/=" "^TOK_OP_ASG_DIV\n$")
match_test(tokenid_op09 tokenid "*=" "^TOK_OP_ASG_MUL\n$")
match_test(tokenid_op10 tokenid "-=" "^TOK_OP_ASG_SUB\n$")
match_test(tokenid_op11 tokenid "+=" "^TOK_OP_ASG_ADD\n$")
match_test(tokenid_op12 tokenid "=" "^TOK_OP_ASG\n$")
match_test(tokenid_op13 tokenid "null" "^TOK_OP_NULL\n$")
match_test(tokenid_op14 tokenid "||" "^TOK_OP_LOR\n$")
match_test(tokenid_op15 tokenid "&&" "^TOK_OP_LAND\n$")
match_test(tokenid_op16 tokenid "<>" "^TOK_OP_ARG\n$")
match_test(tokenid_op17 tokenid "|" "^TOK_OP_BOR\n$")
match_test(tokenid_op18 tokenid "^" "^TOK_OP_BXOR\n$")
match_test(tokenid_op19 tokenid "&" "^TOK_OP_BAND\n$")
match_test(tokenid_op20 tokenid "==" "^TOK_OP_EQ\n$")
match_test(tokenid_op21 tokenid "!=" "^TOK_OP_NQ\n$")
match_test(tokenid_op22 tokenid "<" "^TOK_OP_LT\n$")
match_test(tokenid_op23 tokenid "<=" "^TOK_OP_LEQ\n$")
match_test(tokenid_op24 tokenid ">" "^TOK_OP_GT\n$")
match_test(tokenid_op25 tokenid ">=" "^TOK_OP_GEQ\n$")
match_test(tokenid_op26 tokenid "<<" "^TOK_OP_LSH\n$")
match_test(tokenid_op27 tokenid ">>" "^TOK_OP_RSH\n$")
match_test(tokenid_op28 tokenid "+" "^TOK_OP_ADD\n$")
match_test(tokenid_op29 tokenid "-" "^TOK_OP_SUB\n$")
match_test(tokenid_op30 tokenid "%" "^TOK_OP_MOD\n$")
match_test(tokenid_op31 tokenid "*" "^TOK_OP_MUL\n$")
match_test(tokenid_op32 tokenid "/" "^TOK_OP_DIV\n$")
match_test(tokenid_op33 tokenid ":" "^TOK_OP_DCL\n$")
match_test(tokenid_op34 tokenid "++" "^TOK_OP_INC\n$")
match_test(tokenid_op35 tokenid "--" "^TOK_OP_DEC\n$")
match_test(tokenid_op36 tokenid "!" "^TOK_OP_LN\n$")
match_test(tokenid_op37 tokenid "~" "^TOK_OP_BN\n$")
match_test(tokenid_op38 tokenid "." "^TOK_OP_MEM\n$")
match_test(tokenid_op39 tokenid "->" "^TOK_OP_DMEM\n$")
# STMT
match_test(tokenid_stmt00 tokenid "\;" "^TOK_STMT\n$")
# IMPL
match_test(Tokenid_impl00 tokenid "=>" "^TOK_IMPL\n$")
# Brackets
match_test(tokenid_br00 tokenid "(" "^TOK_OP_BRACKET_OPEN\n$")
match_test(tokenid_br01 tokenid ")" "^TOK_BRACKET_CLOSE\n$")
match_test(tokenid_br02 tokenid "[" "^TOK_OP_ARR_BRACKET_OPEN\n$")
match_test(tokenid_br03 tokenid "]" "^TOK_ARR_BRACKET_CLOSE\n$")
match_test(tokenid_br04 tokenid "{" "^TOK_OP_TEMPL_BRACKET_OPEN\n$")
match_test(tokenid_br05 tokenid "}" "^TOK_TEMPL_BRACKET_CLOSE\n$")
# Integers
# good
match_test(tokenid_int00 tokenid "0" "^TOK_INT32\n$")
match_test(tokenid_int01 tokenid "0u" "^TOK_UINT32\n$")
match_test(tokenid_int02 tokenid "0s" "^TOK_INT8\n$")
match_test(tokenid_int03 tokenid "0S" "^TOK_INT16\n$")
match_test(tokenid_int04 tokenid "0l" "^TOK_INT32\n$")
match_test(tokenid_int05 tokenid "0L" "^TOK_INT64\n$")
match_test(tokenid_int06 tokenid "0us" "^TOK_UINT8\n$")
match_test(tokenid_int07 tokenid "0uS" "^TOK_UINT16\n$")
match_test(tokenid_int08 tokenid "0ul" "^TOK_UINT32\n$")
match_test(tokenid_int09 tokenid "0uL" "^TOK_UINT64\n$")
match_test(tokenid_int10 tokenid "1234567890" "^TOK_INT32\n$")
match_test(tokenid_int11 tokenid "1234567890u" "^TOK_UINT32\n$")
match_test(tokenid_int12 tokenid "0b01010101" "^TOK_INT32\n$")
match_test(tokenid_int13 tokenid "0b01010101us" "^TOK_UINT8\n$")
match_test(tokenid_int14 tokenid "0o01234567" "^TOK_INT32\n$")
match_test(tokenid_int15 tokenid "0o01234567u" "^TOK_UINT32\n$")
match_test(tokenid_int16 tokenid "0x01234567" "^TOK_INT32\n")
match_test(tokenid_int17 tokenid "0x89ABCDEF" "^TOK_INT32\n")
match_test(tokenid_int18 tokenid "0x89ABCDEFu" "^TOK_UINT32\n")
# bad
match_test(tokenid_int50 tokenid "0b012" "^TOK_ERR\n")
match_test(tokenid_int51 tokenid "0o178" "^TOK_ERR\n")
match_test(tokenid_int52 tokenid "0xEFG" "^TOK_ERR\n")
match_test(tokenid_int53 tokenid "123E" "^TOK_ERR\n")
match_test(tokenid_int54 tokenid "00" "^TOK_ERR\n")
match_test(tokenid_int55 tokenid "01" "^TOK_ERR\n")
# Floats
# good
match_test(tokenid_flt00 tokenid "0.0" "^TOK_FLT64\n$")
match_test(tokenid_flt01 tokenid "0.0f" "^TOK_FLT32\n$")
match_test(tokenid_flt02 tokenid "0.0F" "^TOK_FLT64\n$")
match_test(tokenid_flt03 tokenid "1234567890.0123456789" "^TOK_FLT64\n$")
# bad
match_test(tokenid_flt04 tokenid "0.0u" "^TOK_ERR\n")
match_test(tokenid_flt05 tokenid "1.f" "^TOK_ERR\n")
# Char
# good
match_test(tokenid_char00 tokenid "'a'" "^TOK_CHAR\n$")
match_test(tokenid_char01 tokenid "'\\''" "^TOK_CHAR\n$")
match_test(tokenid_char02 tokenid "'\\\"'" "^TOK_CHAR\n$")
#match_test(tokenid_char03 tokenid "'\\\\'" "^TOK_CHAR\n$")
match_test(tokenid_char04 tokenid "'\\\\a'" "^TOK_CHAR\n$")
match_test(tokenid_char05 tokenid "'\\\\b'" "^TOK_CHAR\n$")
match_test(tokenid_char06 tokenid "'\\\\f'" "^TOK_CHAR\n$")
match_test(tokenid_char07 tokenid "'\\\\n'" "^TOK_CHAR\n$")
match_test(tokenid_char08 tokenid "'\\\\r'" "^TOK_CHAR\n$")
match_test(tokenid_char09 tokenid "'\\\\t'" "^TOK_CHAR\n$")
match_test(tokenid_char10 tokenid "'\\\\v'" "^TOK_CHAR\n$")
match_test(tokenid_char11 tokenid "'\\\\xAA'" "^TOK_CHAR\n$")
match_test(tokenid_char12 tokenid "'\\\\x0A'" "^TOK_CHAR\n$")
# bad
match_test(tokenid_char50 tokenid "'aa'" "^TOK_ERR\n")
match_test(tokenid_char51 tokenid "'\\\\xFG'" "^TOK_ERR\n")
match_test(tokenid_char52 tokenid "'\\\\xGF'" "^TOK_ERR\n")
# String
# good
match_test(tokenid_str00 tokenid "\"hello, world!\"" "^TOK_STR\n$")
# bad
match_test(tokenid_str50 tokenid "\"" "^TOK_ERR\n")
match_test(tokenid_str51 tokenid "\"abcd" "^TOK_ERR\n")
# EOL
match_test(tokenid_eol tokenid "\n" "^TOK_EOL\n$")
# EOF
match_test(tokenid_eof tokenid " " "^TOK_EOF\n$")
# Any
match_test(tokeid_any tokenid "_" "^TOK_ANY\n$")

# comments and spaces
# good
match_test(tokenid_sp00 tokenid "// comment\n123" "^TOK_EOL\n$")
match_test(tokenid_sp01 tokenid "/* comment */ 123" "^TOK_INT32\n$")
match_test(tokenid_sp02 tokenid "    123" "^TOK_INT32\n$")
match_test(tokenid_sp03 tokenid "/* comment\nanother comment*/ 123"
  "^TOK_INT32\n$")
# bad
match_test(tokenid_sp50 tokenid "/* comment" "^TOK_ERR\n")

# kill tests
status_test_arg(tokenid_kill00 tokenidmem "ka\;jsdlkfa\;h")
status_test_arg(tokenid_kill01 tokenidmem "[as]123786!@#~")
status_test_arg(tokenid_kill02 tokenidmem "\\\\")
status_test_arg(tokenid_kill03 tokenidmem "\n")
status_test_arg(tokenid_kill04 tokenidmem "         akl\;sdjfa123")

# memory tests
valgrind_test(tokenid_memstr00 $<TARGET_FILE:tokenidmem> "\"hello, world!\"")
valgrind_test(tokenid_memstr01 $<TARGET_FILE:tokenidmem> "\"")
valgrind_test(tokenid_memstr02 $<TARGET_FILE:tokenidmem> "\"abcd")
valgrind_test(tokenid_memid00 $<TARGET_FILE:tokenidmem> "abcd")
valgrind_test(tokenid_memflt00 $<TARGET_FILE:tokenidmem> "1.0")
valgrind_test(tokenid_memch00 $<TARGET_FILE:tokenidmem> "'h")
valgrind_test(tokenid_memint00 $<TARGET_FILE:tokenidmem> "1")
valgrind_test(tokenid_memint01 $<TARGET_FILE:tokenidmem> "0x0")
valgrind_test(tokenid_memint02 $<TARGET_FILE:tokenidmem> "0o0")
valgrind_test(tokenid_memint03 $<TARGET_FILE:tokenidmem> "0b0")
valgrind_test(tokenid_memint14 $<TARGET_FILE:tokenidmem> "(")

# -------------------------------- syntax ------------------------------------
# binary operators
# good
match_test(ast_biop00 ast "a == b" "^\\\\(== a b\\\\)\n$")
match_test(ast_biop01 ast "a != b" "^\\\\(!= a b\\\\)\n$")
match_test(ast_biop02 ast "a + b" "^\\\\(\\\\+ a b\\\\)\n$")
match_test(ast_biop03 ast "a - b" "^\\\\(- a b\\\\)\n$")
# bad
match_test(ast_biop50 ast "a -" "^ERR\n")
match_test(ast_biop51 ast "/ a" "^ERR\n")
match_test(ast_biop52 ast "a *" "^ERR\n")
match_test(ast_biop53 ast "a %" "^ERR\n")
match_test(ast_biop54 ast "a ==" "^ERR\n")
# precedence/associativity
match_test(ast_prec00 ast "a + b * c"
  "^\\\\(\\\\+ a \\\\(\\\\* b c\\\\)\\\\)\n$")
match_test(ast_prec01 ast "a + b + c"
  "^\\\\(\\\\+ \\\\(\\\\+ a b\\\\) c\\\\)\n$")
match_test(ast_prec02 ast "a * b * c"
  "^\\\\(\\\\* \\\\(\\\\* a b\\\\) c\\\\)\n$")
match_test(ast_prec03 ast "a : b : c"
  "^\\\\(: a \\\\(: b c\\\\)\\\\)\n$")
match_test(ast_prec04 ast "a = b = c"
  "^\\\\(= a \\\\(= b c\\\\)\\\\)\n$")
match_test(ast_prec05 ast "a + b - c"
  "^\\\\(- \\\\(\\\\+ a b\\\\) c\\\\)\n$")
# unary operators
match_test(ast_unop00 ast "++a" "^\\\\(\\\\+\\\\+ a\\\\)\n$")
match_test(ast_unop01 ast "+a" "^\\\\(\\\\+ a\\\\)\n$")
match_test(ast_unop02 ast "-a" "^\\\\(- a\\\\)\n$")
match_test(ast_unop03 ast "&a" "^\\\\(& a\\\\)\n$")
match_test(ast_unop04 ast "*a" "^\\\\(\\\\* a\\\\)\n")
# unayr/binary operator precedence
match_test(ast_biunop00 ast "+a.b"
  "^\\\\(\\\\+ \\\\(\\\\. a b\\\\)\\\\)\n$")
match_test(ast_biunop01 ast "+a-b"
  "^\\\\(- \\\\(\\\\+ a\\\\) b\\\\)\n$")
match_test(ast_biunop02 ast "+a+b"
  "^\\\\(\\\\+ \\\\(\\\\+ a\\\\) b\\\\)\n$")
match_test(ast_biunop03 ast "+a.b-c.d.e/f"
  "^\\\\(- \\\\(\\\\+ \\\\(\\\\. a b\\\\)\\\\) \\\\(/ \\\\(\\\\. \\\\(\\\\. c d\\\\) e\\\\) f\\\\)\\\\)")

# fn, arr, templ calls
# fn
# good
match_test(ast_fncall00 ast "a()" "^\\\\(a\\\\)\n$")
match_test(ast_fncall01 ast "a(b)" "^\\\\(a b\\\\)\n$")
match_test(ast_fncall02 ast "a(b, c)" "^\\\\(a b c\\\\)\n$")
match_test(ast_fncall03 ast "a.b(c, d)" "^\\\\(\\\\(\\\\. a b\\\\) c d\\\\)\n$")
match_test(ast_fncall04 ast "a + b(c, d)" "^\\\\(\\\\+ a \\\\(b c d\\\\)\\\\)\n$")
match_test(ast_fncall05 ast "a - b.c(d, e)" "^\\\\(- a \\\\(\\\\(\\\\. b c\\\\) d e\\\\)\\\\)\n$")
# bad
match_test(ast_fncall50 ast "a(" "^ERR\n")
match_test(ast_fncall51 ast "a(b" "^ERR\n")
match_test(ast_fncall52 ast "a(b, c" "^ERR\n")

# memory tests
valgrind_test(ast_mem00 $<TARGET_FILE:astmem> "a + b")
valgrind_test(ast_mem01 $<TARGET_FILE:astmem> "a + b + c * d * e / hello")
valgrind_test(ast_mem02 $<TARGET_FILE:astmem> "\"Hello\" + \"World\"")
valgrind_test(ast_mem03 $<TARGET_FILE:astmem> "\"Hello\" + ")
valgrind_test(ast_mem04 $<TARGET_FILE:astmem> "abcde + ")
valgrind_test(ast_mem05 $<TARGET_FILE:astmem> "++ ")
