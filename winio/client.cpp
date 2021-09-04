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

void ClientWrite()
{
   HANDLE hPipe;
   BOOL fSuccess = FALSE;
   DWORD dwMode;
   LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

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

   std::cout << "Client pipe connected successfully" << std::endl;
   boost::asio::io_context ios;
   net::windows::stream_handle pipe(ios);
   pipe.assign(hPipe);

   http::request<http::string_body> req{http::verb::post, "/count", 11};
   req.set(http::field::host, "localhost");
   req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
   req.set(http::field::body, "hello");
   req.prepare_payload();
   // std::cout << "client preview: " << req << std::endl;
   // Send the HTTP request to the remote host
   http::write(pipe, req);

   std::cout << "Client request sent \n"
             << std::endl;

   // This buffer is used for reading and must be persisted
   beast::flat_buffer buffer_;

   // Declare a container to hold the response
   http::response<http::dynamic_body> resp;

   std::cout << "Client reading resp \n " << std::endl;
   if (!pipe.is_open())
   {
      std::cout << "pipe is closed before read" << std::endl;
   }

   //Receive the HTTP response
   try
   {
      http::read(pipe, buffer_, resp);
   }
   catch (std::exception const &e)
   {
      std::cerr << "Http read: " << e.what() << std::endl;
      std::cout << boost::stacktrace::stacktrace();
   }
   ios.run();
   std::cout << "Client resp read: \n " << resp << std::endl;
}

int main(int argc, char *argv[])
{
   try
   {
      ClientWrite();
   }
   catch (std::exception const &e)
   {
      std::cerr << "Error: " << e.what() << std::endl;
      std::cout << boost::stacktrace::stacktrace();
      return EXIT_FAILURE;
   }
}