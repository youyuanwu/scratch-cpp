
#include "response.h"

void ClientResponseImpl::SetHeaderResp(std::string key, std::vector<std::string> val)
{
    this->_header[key] = val;
}


void ClientResponseImpl::SetBodyResp(std::string body)
{
    this->_body = body;
}

void ClientResponseImpl::SetCode(int code)
{
    this->_code = code;
}

std::string ClientResponseImpl::GetBody() const {
    return this->_body;
}


std::string ClientResponseImpl::GetHeader(std::string key) const 
{
    // TODO: error handling
    std::vector<std::string> h = this->_header.at(key);
    if (h.size() != 0)
    {
        return h[0];
    }
    return "";
}

int ClientResponseImpl::GetCode() const
{
    return this->_code;
}