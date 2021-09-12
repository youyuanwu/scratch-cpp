#include "curlppclient.h"

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Infos.hpp>
#include <sstream>

curlpp::OptionBase* makeMethodOpt(std::string method)
{
  if(method == "GET")
  {
    return new curlpp::options::HttpGet(true);
  }
  if(method == "POST")
  {
    return new curlpp::options::Post(true);
  }
  if(method == "PUT")
  {
    return new curlpp::options::Put(true);
  }
  throw std::invalid_argument("Method: " + method + " is not recognized");
}

void CurlPPClient::MakeRequest(const IOASClientRequest &req, IOASClientResponse &resp){
        
        std::string url = "http://" + this->_cfg.Host + ":" + this->_cfg.Port + this->_cfg.BasePath;

        curlpp::Cleanup cleaner;
		curlpp::Easy request;

		std::list<std::string> headers;
        const Header & h = req.GetHeaderParam();
        for (auto const& [key, val] : h)
        {
          // use the first val. TODO: fix this
          if (val.size() == 0)
          {
            continue;
          }
          headers.push_back(key + ": " + val[0]);
        }
		// sprintf(buf, "Content-Length: %d", size); 
		//headers.push_back(buf);

		using namespace curlpp::Options;
    request.setOpt(makeMethodOpt(req.GetMethod()));

		// request.setOpt(new Verbose(true));
    const std::string & body = req.GetBody();
    if (body.size() != 0)
    {
		  request.setOpt(new curlpp::options::PostFields(req.GetBody()));
      request.setOpt(new curlpp::options::PostFieldSize(req.GetBody().length()));
    }
    request.setOpt(new HttpHeader(headers));
		request.setOpt(new Url(url));

        std::stringstream os;
		curlpp::options::WriteStream ws(&os);
		request.setOpt(ws);
		request.perform();

        resp.SetBodyResp(os.str());
        long http_code = curlpp::infos::ResponseCode::get(request);
        resp.SetCode(http_code);
        // TODO set header
}