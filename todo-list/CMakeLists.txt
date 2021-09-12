cmake_minimum_required(VERSION 3.14)

set (CMAKE_CXX_STANDARD 17)

# set the project name
project(todo-list)

# add dependencies
# install boost on ubuntu :  sudo apt-get install libboost-all-dev

find_package(Boost REQUIRED system thread)

add_subdirectory(runtime)

add_subdirectory(client)

include_directories(
    ${Boost_INCLUDE_DIR}
    ${todo-list_SOURCE_DIR}/include
#    ${cpp-openapi-runtime_INCLUDE_DIR}
    ${todo-list_SOURCE_DIR}/runtime/include
    ${todo-list_SOURCE_DIR}/client/beast/include
    ${cpp-openapi-client-curlpp_SOURCE_DIR}/include
)

file(GLOB SOURCES
    include/*.h
    src/*.cpp
)

add_library(todo-list "${cxx_strict}" ${SOURCES})

add_executable("beast_main" beast_main.cpp)
target_link_libraries("beast_main" PRIVATE ${Boost_LIBRARIES} todo-list cpp-openapi-runtime cpp-openapi-client-beast)

add_executable("curlpp_main" curlpp_main.cpp)
target_link_libraries("curlpp_main" PRIVATE ${Boost_LIBRARIES} todo-list cpp-openapi-runtime cpp-openapi-client-curlpp)
