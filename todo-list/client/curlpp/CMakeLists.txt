


# message(STATUS "Looking for CURL")
# find_package(CURL)

# if(CURL_FOUND)
#   message(STATUS "Found CURL version: ${CURL_VERSION_STRING}")
#   message(STATUS "Using CURL include dir(s): ${CURL_INCLUDE_DIRS}")
#   message(STATUS "Using CURL lib(s): ${CURL_LIBRARIES}")
# else()
#   message(STATUS "Fetching CURL")
#   include(FetchContent)
#   FetchContent_Declare(CURL GIT_REPOSITORY https://github.com/curl/curl.git)
#   message(STATUS "New CURL version: ${CURL_VERSION_STRING}")
#   message(STATUS "New CURL include dir(s): ${CURL_INCLUDE_DIRS}")
#   message(STATUS "New CURL lib(s): ${CURL_LIBRARIES}")
# endif()

# install curl and ssl 
# sudo apt-get install libssl-dev libcurl4-openssl-dev

message(STATUS "Fetching curlpp")
include(FetchContent)
FetchContent_Declare(
    curlpp 
    GIT_REPOSITORY https://github.com/jpbarrette/curlpp.git
    GIT_TAG a67c73dde2c5d50b3dd3f898b8c185c23de827f3
    )

FetchContent_MakeAvailable(curlpp)


file(GLOB SOURCES
    include/*.h
    src/*.cpp
)

project(cpp-openapi-client-curlpp)

add_library(cpp-openapi-client-curlpp "${cxx_strict}" 
# ${Boost_LIBRARIES} 
${SOURCES})

target_include_directories(
    cpp-openapi-client-curlpp
    PUBLIC
    ${cpp-openapi-client-curlpp_SOURCE_DIR}/include
    "${curlpp_SOURCE_DIR}/include"
)

target_link_libraries(cpp-openapi-client-curlpp PRIVATE ${Boost_LIBRARIES} cpp-openapi-runtime curlpp)