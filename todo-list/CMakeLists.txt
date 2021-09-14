cmake_minimum_required(VERSION 3.14)

set (CMAKE_CXX_STANDARD 17)

# set the project name
project(todo-list)

# add dependencies
# install boost on ubuntu :  sudo apt-get install libboost-all-dev

find_package(Boost REQUIRED system thread)

add_subdirectory(runtime)

add_subdirectory(client)

add_subdirectory(json)

file(GLOB SOURCES
    include/*.h
    src/*.cpp
)

add_library(todo-list "${cxx_strict}" ${SOURCES})

target_include_directories(
    todo-list
    PUBLIC ${todo-list_SOURCE_DIR}/include
)

target_link_libraries(todo-list
  PUBLIC cpp-openapi-runtime 
  PUBLIC cpp-openapi-json-nlohmann
)

add_executable("beast_main" beast_main.cpp)
target_link_libraries("beast_main" PRIVATE ${Boost_LIBRARIES} todo-list cpp-openapi-client-beast)

add_executable("curlpp_main" curlpp_main.cpp)
target_link_libraries("curlpp_main" PRIVATE ${Boost_LIBRARIES} todo-list cpp-openapi-client-curlpp)
