#include "asio.hpp"
#include <string>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(){

    std::string name("server.sock");
    try
    {
    asio::io_context ioc{1};

    fs::path file = fs::current_path().append(name);
    fs::file_status s;
    if(fs::exists(file)){
        fs::remove(file);
    }

    asio::local::stream_protocol::endpoint ep(name);
    // do not reuse addr can fix the bug.
    asio::local::stream_protocol::acceptor acceptor(ioc, ep, false);

    asio::local::stream_protocol::socket socket(ioc);

    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // fs::path file(name);
    // if(fs::exists(file)){
    //     fs::remove(file);
    // }

    return 0;
}