cmake_minimum_required(VERSION 3.10)
project(pfederc_core)

add_library(pfederc_core "${pfederc_core_SOURCE_DIR}/src/core.cpp")
target_include_directories(pfederc_core PUBLIC
	"${pfederc_core_SOURCE_DIR}/include")
add_lto_support(pfederc_core)
