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

# set(ECHO_SSL_SERVER echo_ssl_server)
# add_executable(${ECHO_SSL_SERVER} echo_ssl_server.cpp)
# target_link_libraries(${ECHO_SSL_SERVER} PRIVATE boost-wintls)
# target_include_directories(${ECHO_SSL_SERVER} PRIVATE .)
# target_compile_features(${ECHO_SSL_SERVER} INTERFACE cxx_std_14)

# set(ECHO_SSL_CLIENT echo_ssl_client)
# add_executable(${ECHO_SSL_CLIENT} echo_ssl_client.cpp)
# target_link_libraries(${ECHO_SSL_CLIENT} PRIVATE boost-wintls)
# target_include_directories(${ECHO_SSL_CLIENT} PRIVATE .)
# target_compile_features(${ECHO_SSL_CLIENT} INTERFACE cxx_std_14)

# set(ECHO_CLIENT echo_client)
# add_executable(${ECHO_CLIENT} echo_client.cpp)
# target_link_libraries(${ECHO_CLIENT} PRIVATE boost-wintls)
# target_include_directories(${ECHO_CLIENT} PRIVATE .)
# target_compile_features(${ECHO_CLIENT} INTERFACE cxx_std_14)

# set(ECHO_SERVER echo_server)
# add_executable(${ECHO_SERVER} echo_server.cpp)
# target_link_libraries(${ECHO_SERVER} PRIVATE boost-wintls)
# target_include_directories(${ECHO_SERVER} PRIVATE .)
# target_compile_features(${ECHO_SERVER} INTERFACE cxx_std_14)

file(GLOB SOURCES
*.cpp
)

# strip file extension
foreach(test_file ${SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE boost-wintls )
    target_include_directories(${test_name} PRIVATE .)
    target_compile_features(${test_name} INTERFACE cxx_std_14)
endforeach()