cmake_minimum_required(VERSION 3.14)

set (CMAKE_CXX_STANDARD 17)

project(cpp-openapi-runtime)

file(GLOB SOURCES
    include/*.h
    src/*.cpp
)

add_library(cpp-openapi-runtime "${cxx_strict}" 
${SOURCES})

target_include_directories(
    cpp-openapi-runtime
    PUBLIC
    ${cpp-openapi-runtime_SOURCE_DIR}/include
)