# set(Boost_USE_STATIC_LIBS ON)
# ADD_DEFINITIONS("-DBOOST_ALL_NO_LIB")

# find_package(Boost REQUIRED system)
# include_directories(${Boost_INCLUDE_DIR})
# message(status "lib dir is " ${Boost_LIBRARY_DIRS})
# message("BOOST LIBRARIES LOCATION: " ${Boost_LIBRARIES})


# fetch wintls
message(STATUS "Fetching boost-wintls")
include(FetchContent)
FetchContent_Declare(boost-wintls
  GIT_REPOSITORY https://github.com/laudrup/boost-wintls.git
  GIT_TAG 372a82bfa37b157914b94a34e09dd9b84dff8f40)
FetchContent_GetProperties(boost-wintls)
set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ENABLE_DOCUMENTATION OFF CACHE BOOL "" FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)
if(NOT asio_POPULATED)
  FetchContent_Populate(boost-wintls)
  add_subdirectory(${boost-wintls_SOURCE_DIR} ${boost-wintls_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server PRIVATE boost-wintls)
target_include_directories(echo_server PRIVATE .)
target_compile_features(echo_server INTERFACE cxx_std_14)


add_executable(echo_client echo_client.cpp)
target_link_libraries(echo_client PRIVATE boost-wintls)
target_include_directories(echo_client PRIVATE .)
target_compile_features(echo_client INTERFACE cxx_std_14)