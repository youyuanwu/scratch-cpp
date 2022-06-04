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

#include <cstdlib>
#include <functional>
#include <iostream>


using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
  session(boost::wintls::stream<tcp::socket> stream)
    : stream_(std::move(stream)) {
  }

  void start() {
    do_handshake();
  }

private:
  void do_handshake() {
    auto self(shared_from_this());
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
    stream_.async_read_some(boost::asio::buffer(data_),
                            [this, self](const boost::system::error_code& ec, std::size_t length) {
      if (!ec) {
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
    boost::asio::async_write(stream_, boost::asio::buffer(data_, length),
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

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: server <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;
    server s(io_context, static_cast<std::uint16_t>(std::atoi(argv[1])));

    io_context.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}