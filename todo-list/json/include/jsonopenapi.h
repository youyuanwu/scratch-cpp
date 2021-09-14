#pragma once

// Because template with virtual member is not allowed. We implement interface with types specific functions. 

#include <string>
#include <memory>
#include <vector>

class Json
{
public:
    // sets the json content
    virtual void SetJson(std::string data) = 0;

    virtual bool GetInt(std::string name, int &ret) = 0;

    virtual bool GetString(std::string name, std::string &ret) = 0;

    virtual bool GetObj(std::string name, std::shared_ptr<Json> &ret) = 0;

    virtual bool ToArray(std::vector<std::shared_ptr<Json>> &ret) = 0;

    virtual std::string ToString() = 0;
};