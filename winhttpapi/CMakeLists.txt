
add_executable(main main.cpp)
target_link_libraries(main PRIVATE Crypt32.lib Ws2_32.lib Bcrypt.lib)