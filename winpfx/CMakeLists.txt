
add_executable(export export.cpp)

target_link_libraries(export PRIVATE Crypt32.lib)

add_executable(generate generate.cpp)

target_link_libraries(generate PRIVATE Crypt32.lib)


add_executable(copied copied.c)
target_link_libraries(copied PRIVATE Crypt32.lib)


find_package(OpenSSL REQUIRED COMPONENTS SSL Crypto)

add_executable(opensslgen opensslgen.cpp)
target_link_libraries(opensslgen PRIVATE OpenSSL::Crypto Crypt32.lib)