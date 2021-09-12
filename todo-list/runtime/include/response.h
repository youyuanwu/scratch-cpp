#pragma once

#include "common.h"

class IOASClientResponse
{
public:
    virtual void SetHeaderResp(std::string key, std::vector<std::string> val) = 0;
    // TODO: use stream?
    virtual void SetBodyResp(std::string body) = 0;
    virtual void SetCode(int code) = 0;

    virtual std::string GetBody() const = 0;
    virtual std::string GetHeader(std::string key) const = 0;
    virtual int GetCode() const = 0;
};

class ClientResponseImpl : public IOASClientResponse
{
    Header _header;
    Values _query;
    std::string _body;
    int _code;
public:
    void SetHeaderResp(std::string key, std::vector<std::string> val) override;
    void SetBodyResp(std::string body) override;
    void SetCode(int code) override;

    std::string GetBody() const override;
    std::string GetHeader(std::string key) const override;
    int GetCode() const override;
};