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

void ServerWaitToRead()
{
    HANDLE hPipe = INVALID_HANDLE_VALUE, hThread = NULL;
    LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

    try
    {
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

        boost::asio::io_context ios;
        net::windows::stream_handle pipe(ios);
        pipe.assign(hPipe);

        std::cout << "Pipe created successfully." << std::endl;

        bool fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (!fConnected)
        {
            std::string errStr("Pipe client connect error, GLE=%d.\n", GetLastError());
            throw new std::runtime_error(errStr);
        }

        char data[BUFSIZE];
        pipe.async_read_some(boost::asio::buffer(data, BUFSIZE), [](const boost::system::error_code &err,
                                                                    size_t bytes_transferred)
                             {
                                 std::cout << "read part:" << bytes_transferred << std::endl;
                                 if (err)
                                 {
                                     std::cout << "err:" << err << std::endl;
                                 }
                             });

        ios.run();
        std::cout << "Server Read: " << data << std::endl;
    }
    catch (std::exception const &e)
    {
        std::cerr << "Server Error: " << e.what() << std::endl;
        std::cout << boost::stacktrace::stacktrace();
    }
}

void ClientWrite()
{
    HANDLE hPipe;
    BOOL fSuccess = FALSE;
    DWORD dwMode;
    LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

    try
    {
        // Try to open a named pipe; wait for it, if necessary.

        while (1)
        {
            hPipe = CreateFile(
                lpszPipename,  // pipe name
                GENERIC_READ | // read and write access
                    GENERIC_WRITE,
                0,                    // no sharing
                NULL,                 // default security attributes
                OPEN_EXISTING,        // opens existing pipe
                FILE_FLAG_OVERLAPPED, // default attributes
                NULL);                // no template file

            // Break if the pipe handle is valid.

            if (hPipe != INVALID_HANDLE_VALUE)
                break;

            // Exit if an error other than ERROR_PIPE_BUSY occurs.

            if (GetLastError() != ERROR_PIPE_BUSY)
            {
                _tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
                //return -1;
            }

            // All pipe instances are busy, so wait for 20 seconds.

            if (!WaitNamedPipe(lpszPipename, 20000))
            {
                printf("Could not open pipe: 20 second wait timed out.");
                //return -1;
            }
        }

        // The pipe connected; change to message-read mode.

        dwMode = PIPE_READMODE_MESSAGE;
        fSuccess = SetNamedPipeHandleState(
            hPipe,   // pipe handle
            &dwMode, // new pipe mode
            NULL,    // don't set maximum bytes
            NULL);   // don't set maximum time
        if (!fSuccess)
        {
            _tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
            //return -1;
        }

        boost::asio::io_context ios;
        net::windows::stream_handle pipe(ios);
        pipe.assign(hPipe);

        char data[BUFSIZE] = "hello from client";
        pipe.async_write_some(boost::asio::buffer(data), [](const boost::system::error_code &err,
                                                            size_t bytes_transferred)
                              {
                                  std::cout << "write part:" << bytes_transferred << std::endl;
                                  if (err)
                                  {
                                      std::cout << "err:" << err << std::endl;
                                  }
                              });

        ios.run();

        CloseHandle(hPipe);
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cout << boost::stacktrace::stacktrace();
    }
}

int main(int argc, char *argv[])
{
    try
    {
        std::thread server(ServerWaitToRead);
        std::thread client(ClientWrite);

        server.join();
        client.join();
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cout << boost::stacktrace::stacktrace();
        return EXIT_FAILURE;
    }
}