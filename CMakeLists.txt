cmake_minimum_required(VERSION 3.10)
project(pfederc)

find_package(Threads REQUIRED)

include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT output)

macro(add_lto_support name)
	if (lto_supported)
		set_property(TARGET ${name} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	else()
		message(STATUS "LTO not supported: <${error}>")
	endif()
endmacro()

add_subdirectory("${pfederc_SOURCE_DIR}/libs")

set(PFEDERC_LIBRARIES pfederc_core pfederc_errors pfederc_lexer
  pfederc_syntax pfederc_semantics
  ${CMAKE_THREAD_LIBS_INIT})

add_executable(pfederc "${pfederc_SOURCE_DIR}/src/main.cpp")
add_lto_support(pfederc)
target_include_directories(pfederc PUBLIC "${pfederc_SOURCE_DIR}/include")
target_link_libraries(pfederc ${PFEDERC_LIBRARIES})

add_subdirectory("${pfederc_SOURCE_DIR}/test")
