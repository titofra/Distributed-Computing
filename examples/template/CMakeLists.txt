cmake_minimum_required(VERSION 3.0)

project(TemplateDistriComp)

include_directories(../../include)
add_executable(TemplateClient client.c)
add_executable(TemplateServer server.c)

add_library(districomp STATIC IMPORTED)
set_property(TARGET districomp PROPERTY IMPORTED_LOCATION ../../../lib/libdistricomp.a)

target_compile_options(TemplateClient PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -O3 -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wunreachable-code -Wconversion -Wno-error=maybe-uninitialized -Wpedantic -Wcast-align>
)

target_compile_options(TemplateServer PRIVATE
	$<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Werror -O3 -Wfloat-equal -Wundef -Wcast-align -Wwrite-strings -Wunreachable-code -Wconversion -Wno-error=maybe-uninitialized -Wpedantic -Wcast-align>
)

target_link_libraries(TemplateClient districomp sfml-window sfml-system sfml-graphics)
target_link_libraries(TemplateServer districomp sfml-window sfml-system sfml-graphics)
