#include <request.h>

void ClientRequestImpl::SetHeaderParam(std::string key, std::vector<std::string> val)
{
    this->_header[key] = val;
}


void ClientRequestImpl::SetQueryParam(std::string key, std::vector<std::string> val)
{
    this->_query[key] = val;
}

void ClientRequestImpl::SetPathParam(std::string key, std::string val)
{
    this->_pathParam[key] = val;
}

void ClientRequestImpl::SetBodyParam(std::string body)
{
    this->_body = body;
}

void ClientRequestImpl::SetMethod(std::string method) 
{
    this->_method = method;    
}

std::string ClientRequestImpl::GetPath() const {
    // TODO: match and replace params
    return this->_pathPattern;
}

std::string ClientRequestImpl::GetBody() const {
    return this->_body;
}
Header ClientRequestImpl::GetHeaderParam() const {
    return this->_header;
}
Values ClientRequestImpl::GetQueryParam() const {
    return this->_query;
}

std::string ClientRequestImpl::GetMethod() const
{
    return this->_method;
}