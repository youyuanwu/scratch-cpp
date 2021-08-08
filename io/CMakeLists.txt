cmake_minimum_required(VERSION 3.14)

set (CMAKE_CXX_STANDARD 17)

# set the project name
project(io-cpp)

# add dependencies
# install boost on ubuntu :  sudo apt-get install libboost-all-dev

find_package(Boost REQUIRED system thread)

include_directories(${Boost_INCLUDE_DIR})

add_executable("unixsocket" unixsocket.cpp)
target_link_libraries("unixsocket" PRIVATE ${Boost_LIBRARIES})