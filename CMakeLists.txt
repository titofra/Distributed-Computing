cmake_minimum_required(VERSION 3.0)

project(districomp)

file(GLOB SOURCES src/*.c)

include_directories(./include)

add_library(districomp STATIC ${SOURCES})

target_compile_options(districomp PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -O3 -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wunreachable-code -Wconversion -Wno-error=maybe-uninitialized -Wpedantic -Wcast-align>
)

target_include_directories(districomp PUBLIC include/)
