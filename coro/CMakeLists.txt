set (CMAKE_CXX_STANDARD 20)

add_executable("coro1" coro1.cpp)
# target_link_libraries("unixsocket" PRIVATE ${Boost_LIBRARIES})