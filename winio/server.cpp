#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include <boost/stacktrace.hpp>
#include <boost/process/async_pipe.hpp>

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

#define BUFSIZE 512

void handleRequest(net::windows::stream_handle &pipe, boost::asio::io_context &ios)
{
    // read request from pipe into req.
    http::request<http::dynamic_body> request_;

    http::response<http::dynamic_body> response_;
    beast::flat_buffer buffer_{8192};

    http::async_read(
        pipe,
        buffer_,
        request_,
        [&request_, &response_](beast::error_code ec,
                                std::size_t bytes_transferred)
        {
            boost::ignore_unused(bytes_transferred);
            if (!ec)
            {
                std::cout << "debug method:" << request_.method() << std::endl;
                switch (request_.method())
                {
                case http::verb::get:
                    response_.result(http::status::ok);
                    response_.set(http::field::server, "Beast");
                    if (request_.target() == "/count")
                    {
                        response_.set(http::field::content_type, "text/html");
                        beast::ostream(response_.body())
                            << "<html>\n"
                            << "<head><title>Request count</title></head>\n"
                            << "<body>\n"
                            << "<h1>Request count</h1>\n"
                            << "<p>There have been "
                            << "dummy count "
                            << " requests so far.</p>\n"
                            << "</body>\n"
                            << "</html>\n";
                    }
                    else if (request_.target() == "/time")
                    {
                        response_.set(http::field::content_type, "text/html");
                        beast::ostream(response_.body())
                            << "<html>\n"
                            << "<head><title>Current time</title></head>\n"
                            << "<body>\n"
                            << "<h1>Current time</h1>\n"
                            << "<p>The current time is "
                            << "dummy time "
                            << " seconds since the epoch.</p>\n"
                            << "</body>\n"
                            << "</html>\n";
                    }
                    else
                    {
                        response_.result(http::status::not_found);
                        response_.set(http::field::content_type, "text/plain");
                        beast::ostream(response_.body()) << "File not found\r\n";
                    }
                    break;

                default:
                    // We return responses indicating an error if
                    // we do not recognize the request method.
                    response_.result(http::status::bad_request);
                    response_.set(http::field::content_type, "text/plain");
                    beast::ostream(response_.body())
                        << "Invalid request-method '"
                        << std::string(request_.method_string())
                        << "'";
                    break;
                }
                response_.content_length(response_.body().size());
            }
            else
            {
                std::cout << "Server: boost error when read request: " << ec << " " << ec.message() << std::endl;
            }
        });

    ios.run();
    std::cout << "Server Read: \n"
              << request_ << std::endl;
    http::write(pipe, response_);
    ios.run();
    std::cout << "Server Write: \n"
              << response_ << std::endl;
}

void ServerWaitToRead()
{
    HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
    LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

    hPipe = CreateNamedPipe(
        lpszPipename,        // pipe name
        PIPE_ACCESS_DUPLEX | // read/write access
            FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE |         // message type pipe
            PIPE_READMODE_MESSAGE | // message-read mode
            PIPE_WAIT,              // blocking mode
        PIPE_UNLIMITED_INSTANCES,   // max. instances
        BUFSIZE,                    // output buffer size
        BUFSIZE,                    // input buffer size
        0,                          // client time-out
        NULL);                      // default security attribute

    if (hPipe == INVALID_HANDLE_VALUE)
    {
        std::string errStr("CreateNamedPipe failed, GLE=%d.\n", GetLastError());
        throw new std::runtime_error(errStr);
    }

    std::cout << "Pipe created successfully." << std::endl;

    bool fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!fConnected)
    {
        std::string errStr("Pipe client connect error, GLE=%d.\n", GetLastError());
        throw new std::runtime_error(errStr);
    }
    boost::asio::io_context ios;
    net::windows::stream_handle pipe(ios);
    pipe.assign(hPipe);
    handleRequest(pipe, ios);
}

int main(int argc, char *argv[])
{
    try
    {
        ServerWaitToRead();
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cout << boost::stacktrace::stacktrace();
        return EXIT_FAILURE;
    }
}