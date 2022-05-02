cmake_minimum_required(VERSION 3.14)

set (CMAKE_CXX_STANDARD 17)

# set the project name
project(winio-cpp)

# add dependencies
# install boost on ubuntu :  sudo apt-get install libboost-all-dev

# set(BOOST_ROOT "C:\\Boost")
set(BOOST_ROOT "C:\\local\\boost_1_76_0")
#set(Boost_INCLUDE_DIR "C:\\Boost\\include\\boost-1_76")
#set(Boost_LIBRARY_DIRS "C:\\Boost\\lib")

# SET(BOOST_INCLUDE  "C:\\Boost\\include\\boost-1_76")
# SET(BOOST_LIB "C:\\Boost\\lib")

set(Boost_USE_STATIC_LIBS ON)
ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")

find_package(Boost REQUIRED system)
include_directories(${Boost_INCLUDE_DIR})
message(status "lib dir is " ${Boost_LIBRARY_DIRS})
message("BOOST LIBRARIES LOCATION: " ${Boost_LIBRARIES})


# add_executable("namedpipe" namedpipe.cpp)
# target_link_libraries("namedpipe" PRIVATE ${Boost_LIBRARIES})


# add_executable("client" client.cpp)
# target_link_libraries("client" PRIVATE ${Boost_LIBRARIES})

# add_executable("server" server.cpp)
# target_link_libraries("server" PRIVATE ${Boost_LIBRARIES})

add_executable("winunix" winunix.cpp)

target_link_libraries(winunix Ws2_32.lib)