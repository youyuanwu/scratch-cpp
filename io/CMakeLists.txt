cmake_minimum_required(VERSION 3.14)

set (CMAKE_CXX_STANDARD 17)

# set the project name
project(io-cpp)

# add dependencies
# install boost on ubuntu :  sudo apt-get install libboost-all-dev

if(WIN32)
set(Boost_USE_STATIC_LIBS ON)
ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")
find_package(Boost REQUIRED system)
else()
find_package(Boost REQUIRED system thread)
endif()


include_directories(${Boost_INCLUDE_DIR})

add_executable("unixsocket" unixsocket.cpp)
target_link_libraries("unixsocket" PRIVATE ${Boost_LIBRARIES})