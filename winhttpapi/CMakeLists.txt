
# add_executable(main main.cpp)
# target_link_libraries(main PRIVATE Crypt32.lib Ws2_32.lib Bcrypt.lib)

# # for find cert hpp
# target_include_directories(main
#     PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../wintls
# )

add_executable(client client.cpp)
target_link_libraries(client PRIVATE Winhttp.lib)

add_executable(client2 client2.cpp)
target_link_libraries(client2 PRIVATE Winhttp.lib)
