#include "echo_server.hpp"

int main(){
      try
  {

    boost::asio::io_context io_context;

    echo_server::server s(io_context, 12345);

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}