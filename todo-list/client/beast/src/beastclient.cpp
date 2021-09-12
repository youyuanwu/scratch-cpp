#include "beastclient.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <iostream> // debug
namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

void BeastClient::MakeRequest(const IOASClientRequest &req, IOASClientResponse &resp) 
{
        // The io_context is required for all I/O
        net::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);

        // Look up the domain name
        auto const results = resolver.resolve(this->_cfg.Host, this->_cfg.Port);

        // Make the connection on the IP address we get from a lookup
        stream.connect(results);

        std::string temppath = this->_cfg.BasePath + req.GetPath();

        // Set up an HTTP GET request message
        http::verb method = http::string_to_verb(req.GetMethod());
        http::request<http::string_body> bReq{method, temppath, 11};
        bReq.set(http::field::host, this->_cfg.Host);
        bReq.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // add request headers
        const Header & h = req.GetHeaderParam();
        for (auto const& [key, val] : h)
        {
          // use the first val. TODO: fix this
          if (val.size() == 0)
          {
            continue;
          }
          bReq.set(key, val[0]);
        }

        // Send the HTTP request to the remote host
        http::write(stream, bReq);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Translate to respose to return
        resp.SetBodyResp(beast::buffers_to_string(res.body().data()));
        for (auto it = res.begin(); it != res.end(); it++)
        {

            std::string key(it->name_string().begin(), it->name_string().end());
            std::string val(it->value().begin(), it->value().end());
            resp.SetHeaderResp(key, {val});
        }
        resp.SetCode(res.result_int());

        // Gracefully close the socket
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if(ec && ec != beast::errc::not_connected)
            throw beast::system_error{ec};

        // If we get here then the connection is closed gracefully

        // TODO: response
        // std::cout <<"debug resp:"<< beast::buffers_to_string(res.body().data()) << std::endl;
}