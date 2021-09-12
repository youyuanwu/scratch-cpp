#pragma once
#include "client.h"

class BeastClient : public IClient
{
public:
    using IClient::IClient;
    // void MakeRequest(const IOASClientRequest &req) override;
    void MakeRequest(const IOASClientRequest &req, IOASClientResponse &resp)  override;
};