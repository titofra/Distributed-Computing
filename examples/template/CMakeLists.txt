cmake_minimum_required(VERSION 3.0)

project(TemplateDistriComp)

include_directories(../../include)
add_executable(TemplateClient client.c)
add_executable(TemplateServer server.c)

add_library(districomp STATIC IMPORTED)
set_property(TARGET districomp PROPERTY IMPORTED_LOCATION ../../../lib/libdistricomp.a)

target_link_libraries(TemplateClient districomp sfml-window sfml-system sfml-graphics)
target_link_libraries(TemplateServer districomp sfml-window sfml-system sfml-graphics)