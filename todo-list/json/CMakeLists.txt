file(GLOB SOURCES
    include/*.h
    src/*.cpp
)

project(cpp-openapi-json)

add_library(cpp-openapi-json "${cxx_strict}"
${SOURCES})

target_include_directories(
    cpp-openapi-json
    PUBLIC ${cpp-openapi-json_SOURCE_DIR}/include
)

add_subdirectory(providers)