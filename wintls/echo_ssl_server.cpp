//
// Copyright (c) 2021 Kasper Laudrup (laudrup at stacktrace dot dk)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//#include "certificate.hpp"

#include <boost/wintls.hpp>

#include <boost/asio.hpp>

#include "findcert.hpp"
#include "echo_server.hpp"
// #include "echo_client.hpp"

#include <cstdlib>
#include <functional>
#include <iostream>


using boost::asio::ip::tcp;

std::string echo_ssl_server_port = "12344";
std::string echo_server_port = "12345";

class session : public std::enable_shared_from_this<session> {
public:
  session(boost::wintls::stream<tcp::socket> stream)
    : stream_(std::move(stream)), resolver_(stream_.get_executor()), endpoints_(resolver_.resolve("localhost", echo_server_port)),socket_(stream_.get_executor()) {
  }

  void start() {
    do_handshake();
  }

private:
  void do_handshake() {
    auto self(shared_from_this());
    std::cout << "ssl: start handshake" << std::endl;
    stream_.async_handshake(boost::wintls::handshake_type::server,
                            [this, self](const boost::system::error_code& ec) {
      if (!ec) {
        do_read();
      } else {
        std::cerr << "Handshake failed: " << ec.message() << "\n";
      }
    });
  }

  void do_read() {
    auto self(shared_from_this());
    std::cout << "ssl: start read " << std::endl;
    stream_.async_read_some(boost::asio::buffer(data_),
                            [this, self](const boost::system::error_code& ec, std::size_t length) {
      if (!ec) {
        do_forward(length);
      } else {
        if (ec.value() != SEC_I_CONTEXT_EXPIRED) { // SEC_I_CONTEXT_EXPIRED means the client shutdown the TLS channel
          std::cerr << "Read failed: " << ec.message() << "\n";
        }
      }
    });
  }

  void do_forward(std::size_t length) {
    auto self(shared_from_this());

    std::cout << "ssl: start forward connect " << std::endl;
    boost::asio::async_connect(socket_, endpoints_, 
      [this, self,length](const boost::system::error_code& ec,
          const tcp::endpoint& /*endpoint*/){
        
        if(!ec){
          std::cout << "ssl: forward data: " << std::string(data_, length) << std::endl;
          boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
                                [this, self](const boost::system::error_code& ec, std::size_t /*length*/) {
            if (!ec) {
              do_forward_read();
            } else {
              std::cerr << "do forward Write failed: " << ec.message() << "\n";
            }
          });
        } else{
          std::cerr << "do forward connect failed: " << ec.message() << "\n";
        }
    });
    // boost::asio::connect(s, );
  }

  void do_forward_read() {
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(forward_data_),
                            [this, self](const boost::system::error_code& ec, std::size_t length) {
      if (!ec) {
        std::cout << "ssl: forward read data: " << length << ": " << std::string(forward_data_, length) << std::endl;
        do_write(length);
      } else {
        if (ec.value() != SEC_I_CONTEXT_EXPIRED) { // SEC_I_CONTEXT_EXPIRED means the client shutdown the TLS channel
          std::cerr << "Read failed: " << ec.message() << "\n";
        }
      }
    });
  }

  void do_write(std::size_t length) {
    auto self(shared_from_this());
    std::cout << "ssl: write data back to client: " << std::string(forward_data_,length) << std::endl;
    boost::asio::async_write(stream_, boost::asio::buffer(forward_data_, length),
                             [this, self](const boost::system::error_code& ec, std::size_t /*length*/) {
      if (!ec) {
        do_read();
      } else {
        std::cerr << "Write failed: " << ec.message() << "\n";
      }
    });
  }

  boost::wintls::stream<tcp::socket> stream_;
  char data_[1024];
  char forward_data_[1024];
  tcp::resolver resolver_;
  boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> endpoints_;
  tcp::socket socket_;
};

class server {
public:
  server(boost::asio::io_context& io_context, unsigned short port)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    , context_(boost::wintls::method::system_default)
    , private_key_name_("wintls-echo-server-example") {

    PCCERT_CONTEXT cert = findCert();

    if(!cert){
      throw std::exception("cert is not found");
    }

    // Use the certificate for encrypting TLS messages
    context_.use_certificate(cert);

    // TODO: seems verify client cert is not supported
    std::cout << "ssl server likstening on port: " << std::to_string(port) << std::endl;

    do_accept();
  }

  ~server() {

  }

private:
  void do_accept() {
    acceptor_.async_accept([this](const boost::system::error_code& ec, tcp::socket socket) {
      if (!ec) {
        std::make_shared<session>(boost::wintls::stream<tcp::socket>(std::move(socket), context_))->start();
      } else {
        std::cerr << "Read failed: " << ec.message() << "\n";
      }

      do_accept();
    });
  }

  tcp::acceptor acceptor_;
  boost::wintls::context context_;
  std::string private_key_name_;
};

int main() {
  try {
    boost::asio::io_context io_context;

    // raw echo server
    echo_server::server es(io_context, 12345);

    // ssl wrapper server
    // ssl echo server
    boost::asio::io_context io_context2;
    server s(io_context2, static_cast<unsigned short>(std::atoi(echo_ssl_server_port.c_str())));

    std::thread t2([&io_context](){
      io_context.run();
    }); 
    
   io_context2.run();
    t2.join();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

// int main(){
//       try
//   {

//     boost::asio::io_context io_context;

//     echo_server::server s(io_context, 12345);

//     io_context.run();
//   }
//   catch (std::exception& e)
//   {
//     std::cerr << "Exception: " << e.what() << "\n";
//   }
// }