file(GLOB SOURCES
    include/*.h
    src/*.cpp
)

project(cpp-openapi-client-beast)

add_library(cpp-openapi-client-beast "${cxx_strict}" 
# ${Boost_LIBRARIES} 
${SOURCES})

target_include_directories(
    cpp-openapi-client-beast
    PUBLIC
    ${Boost_INCLUDE_DIR}
    ${cpp-openapi-client-beast_SOURCE_DIR}/include
)

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

target_link_libraries(cpp-openapi-client-beast PRIVATE ${Boost_LIBRARIES} cpp-openapi-runtime)