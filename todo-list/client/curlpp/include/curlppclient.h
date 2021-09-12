#pragma once
#include "client.h"

class CurlPPClient : public IClient
{
public:
    using IClient::IClient;
    void MakeRequest(const IOASClientRequest &req, IOASClientResponse &resp)  override;
};