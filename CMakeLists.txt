cmake_minimum_required(VERSION 3.5)

# Project
project(districomp)

# Sources
file(GLOB SOURCES src/*.c)

# Includes
include_directories(include)

# Set Env Variables
set(DISTRICOMP_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)

# Add the library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Flags
target_compile_options(districomp PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-O3 -Wall -Wextra -Werror -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wunreachable-code -Wconversion -Wno-error=maybe-uninitialized -Wno-error=unused-result -Wpedantic>
)
