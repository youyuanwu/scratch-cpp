
#include "todoclient.h"
#include "request.h"

#include "openapi_nlohmann_json.h"
#include <iostream>

findTodoResponse todoservice::findTodos(findTodosParams params)
{
    findTodoResponse result;
    ClientRequestImpl cri;
    cri.SetHeaderParam("x-todolist-token",{"example token"});
    cri.SetMethod("GET");

    ClientResponseImpl respRet;

    _cli->MakeRequest(cri, respRet);

    result.data = respRet.GetBody();
    result.code = respRet.GetCode();

    // Try convert payload
    std::shared_ptr<Json> j = std::make_shared<NlohmannJson>();
    j->SetJson(result.data);

    std::vector<std::shared_ptr<Json>> res;
    bool isArray = j->ToArray(res);
    // std::cout <<"is array: "<< isArray << " debug size: "<< res.size() << std::endl;
    for (auto& e : res) {
        Item i;
        i.json_deserialize(e);
        result.payload.push_back(i);
        std::cout <<"item: "<< i.id << " description: " << i.description << "string:" <<e->ToString()<<std::endl;
    }
    //debug
    //nlohmann::json debugJ("[\"hi\"]");
    //debugJ.
    //std::cout<<"debug is array"<< debugJ.is_array() << debugJ.is_number() << debugJ.is_string() << std::endl;

    return result;
}