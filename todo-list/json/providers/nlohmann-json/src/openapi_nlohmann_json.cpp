#include "openapi_nlohmann_json.h"
#include <nlohmann/json.hpp>

// #include <iostream>

void NlohmannJson::SetJson(std::string data)
{
    
    this->_j = nlohmann::json::parse(data);
}

bool NlohmannJson::GetInt(std::string name, int &ret)
{
    if (!this->_j.contains(name))
    {   
        return false;
    }
    this->_j.at(name).get_to<int>(ret);
    return true;
}

bool NlohmannJson::GetString(std::string name, std::string &ret)
{
    if (!this->_j.contains(name))
    {   
        return false;
    }
    this->_j.at(name).get_to<std::string>(ret);
    return true;
}

bool NlohmannJson::GetObj(std::string name, std::shared_ptr<Json> &ret)
{
    if (!this->_j.contains(name))
    {   
        return false;
    }
    // TODO: this is no efficient?
    nlohmann::json inner = this->_j.at(name);
    std::shared_ptr<NlohmannJson> np = std::make_shared<NlohmannJson>();
    np->setInternal(inner);
    ret = np;
    return true;
}

void NlohmannJson::setInternal(nlohmann::json j)
{
    this->_j = j;
}

bool NlohmannJson::ToArray(std::vector<std::shared_ptr<Json>> &ret) 
{
    if (!this->_j.is_array())
    {
        return false;
    }
   // nlohmann::json arr =  nlohmann::json::array(_j);

    std::vector<std::shared_ptr<Json>> res;
    for (nlohmann::json::iterator it = _j.begin(); it != _j.end(); ++it)
    {
        nlohmann::json item = *it;
        // std::cout << "json debug to array: " << item << std::endl;
        std::shared_ptr<NlohmannJson> jItem = std::make_shared<NlohmannJson>();
        jItem->setInternal(item);
        res.push_back(jItem);
    }
    ret = res;
    return true;
}

std::string NlohmannJson::ToString(){
    return this->_j.dump();
}