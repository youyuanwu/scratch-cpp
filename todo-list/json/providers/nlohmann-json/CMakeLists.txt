# json dependency
message(STATUS "Fetching nlohmann json")
include(FetchContent)
FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.7.3)
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
# json dep end

# target_link_libraries(foo PRIVATE nlohmann_json::nlohmann_json)

file(GLOB SOURCES
    include/*.h
    src/*.cpp
)

project(cpp-openapi-json-nlohmann)

add_library(cpp-openapi-json-nlohmann SHARED "${cxx_strict}" 
${SOURCES})

target_include_directories(
    cpp-openapi-json-nlohmann
    PUBLIC ${cpp-openapi-json-nlohmann_SOURCE_DIR}/include
)

target_link_libraries(cpp-openapi-json-nlohmann PUBLIC cpp-openapi-json)
target_link_libraries(cpp-openapi-json-nlohmann PUBLIC nlohmann_json::nlohmann_json)