#include "asio.hpp"
#include <string>
#include <iostream>

int main(){
  try
    {
    asio::io_context ioc{1};

    std::string name("server.sock");
        // bool d = DeleteFileA(name.c_str());
        // if(!d){
        //     DWORD err = GetLastError();
        //     if(err != ERROR_FILE_NOT_FOUND){
        //         std::cerr << "LastError: " << err << std::endl;
        //         return EXIT_FAILURE;
        //     }
        // }
    asio::local::stream_protocol::endpoint ep(name);
    asio::local::stream_protocol::acceptor acceptor(ioc, ep);
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}