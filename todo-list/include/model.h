#pragma once

#include <string>
#include <jsonopenapi.h>
#include <memory>
class Item
{
public:
    int id;
    std::string description;
    bool completed;

    void to_json() const;
    void json_deserialize(std::shared_ptr<Json> j);
};