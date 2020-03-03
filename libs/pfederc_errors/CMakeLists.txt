cmake_minimum_required(VERSION 3.10)
project(pfederc_errors)

add_library(pfederc_errors STATIC "${pfederc_errors_SOURCE_DIR}/src/errors.cpp")
target_include_directories(pfederc_errors PUBLIC
	"${pfederc_errors_SOURCE_DIR}/include")
target_link_libraries(pfederc_errors PUBLIC pfederc_core)
add_lto_support(pfederc_errors)
