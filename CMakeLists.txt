cmake_minimum_required(VERSION 3.0)

project(districomp)

file(GLOB SOURCES src/*.c)

include_directories(./include)

add_library(districomp STATIC ${SOURCES})

target_include_directories(districomp PUBLIC include/)