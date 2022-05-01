#include "asio.hpp"
#include <iostream>

static const char write_data[]
  = "SomeDummyData";

void handle_write(const asio::error_code& err,
    size_t bytes_transferred, bool* called)
{
  *called = true;
  //ASIO_CHECK(!err);
  //ASIO_CHECK(bytes_transferred == sizeof(write_data));
}

void handle_read(const asio::error_code& err,
    size_t bytes_transferred, bool* called)
{
  *called = true;
  //ASIO_CHECK(!err);
  //ASIO_CHECK(bytes_transferred == sizeof(write_data));
}

int main()
{
    using namespace std; // For memcmp.
    using namespace asio;
    
    asio::io_context io_context;
    asio::error_code ec1;
    asio::error_code ec2;

    readable_pipe p1(io_context);
    writable_pipe p2(io_context);
    connect_pipe(p1, p2);

    std::string data1 = write_data;
    asio::write(p2, asio::buffer(data1));

    std::string data2;
    data2.resize(data1.size());
    asio::read(p1, asio::buffer(data2));

    //ASIO_CHECK(data1 == data2);

    char read_buffer[sizeof(write_data)];
    bool read_completed = false;
    using std::placeholders::_1;
    using std::placeholders::_2;
    asio::async_read(p1,
        asio::buffer(read_buffer),
        std::bind(handle_read,
          _1, _2, &read_completed));

    bool write_completed = false;
    asio::async_write(p2,
        asio::buffer(write_data),
        std::bind(handle_write,
          _1, _2, &write_completed));

    io_context.run();

    cout << read_buffer << endl;

    p1.close();
    p2.close();
}