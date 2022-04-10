project(example)

cmake_minimum_required(VERSION 3.16)

message(STATUS "Fetching runtime")
include(FetchContent)
FetchContent_Declare(openapi_runtime
  GIT_REPOSITORY https://github.com/cpp-openapi/runtime.git
  GIT_TAG a2375d84ad1bdb05510ba86f333c59de64dc4006)
FetchContent_GetProperties(openapi_runtime)
if(NOT openapi_runtime_POPULATED)
  FetchContent_Populate(openapi_runtime)
  add_subdirectory(${openapi_runtime_SOURCE_DIR} ${openapi_runtime_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

