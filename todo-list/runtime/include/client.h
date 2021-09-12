#pragma once

// middleware of different http clients

#include <string>
#include "request.h"
#include "response.h"

struct ClientConfig
{
    std::string Host;
    std::string Port;
    std::string BasePath;
};

class IClient
{
public:
    // TODO: change return
    IClient(ClientConfig cfg): _cfg(cfg) {};
    ClientConfig _cfg;
    // virtual void MakeRequest(const IOASClientRequest &req) = 0;
    virtual void MakeRequest(const IOASClientRequest &req, IOASClientResponse &resp) = 0;
};