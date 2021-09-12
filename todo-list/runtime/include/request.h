#pragma once

#include <string>
#include <vector>
#include <map>

#include "common.h"
// Request for openapi
// it can convert to http request of underlying impl
//
class IOASClientRequest
{
public:
    virtual void SetHeaderParam(std::string key, std::vector<std::string> val) = 0;
    virtual void SetQueryParam(std::string key, std::vector<std::string> val) = 0;
    virtual void SetPathParam(std::string key, std::string val) = 0;
    // TODO: use stream?
    virtual void SetBodyParam(std::string body) = 0;
    virtual void SetMethod(std::string method) = 0;

    virtual std::string GetPath() const = 0;
    virtual std::string GetBody() const = 0;
    virtual Header GetHeaderParam() const = 0;
    virtual Values GetQueryParam() const = 0;
    virtual std::string GetMethod() const = 0;
};

class ClientRequestImpl : public IOASClientRequest
{
    std::string _method;
    std::string _pathPattern; // path for the api after base path
    Header _header;
    Values _query;
    std::string _body;
    std::map<std::string, std::string> _pathParam;

public:
    void SetHeaderParam(std::string key, std::vector<std::string> val) override;
    void SetQueryParam(std::string key, std::vector<std::string> val) override;
    void SetPathParam(std::string key, std::string val) override;
    // TODO: use stream?
    void SetBodyParam(std::string body) override;
    void SetMethod(std::string method) override;

    std::string GetPath() const override;
    std::string GetBody() const override;
    Header GetHeaderParam() const override;
    Values GetQueryParam() const override;
    std::string GetMethod() const override;
};

// generated param needs to implement this
class IClientRequestWriter
{
public:
    virtual void WriteToClientRequest(IOASClientRequest *req);
};