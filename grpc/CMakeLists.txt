
message(STATUS "Fetching grpc")
include(FetchContent)

FetchContent_Declare(
  gRPC
  GIT_REPOSITORY https://github.com/grpc/grpc
  GIT_TAG        v1.28.0
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(gRPC)

add_executable(main main.cpp)
target_link_libraries(main grpc++)