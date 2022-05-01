set (CMAKE_CXX_STANDARD 20)

project(example)

cmake_minimum_required(VERSION 3.16)

message(STATUS "Fetching asio")
include(FetchContent)
FetchContent_Declare(asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG f0a1e1c7c0387ad16358c81eb52528f190df625c)
FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
  FetchContent_Populate(asio)
endif()

# header only lib

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE
    ${asio_SOURCE_DIR}/asio/include
)

#add_subdirectory(${openapi_runtime_SOURCE_DIR} ${openapi_runtime_BINARY_DIR} EXCLUDE_FROM_ALL)

file(GLOB SOURCES
*.cpp
)

foreach(exe_file ${SOURCES})
    get_filename_component(exe_name ${exe_file} NAME_WE)
    add_executable(${exe_name} ${exe_file})
    target_link_libraries(${exe_name} PRIVATE asio)
endforeach()