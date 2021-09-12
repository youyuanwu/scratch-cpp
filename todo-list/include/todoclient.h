
#pragma once

#include <string>
#include <memory>
#include "client.h"

struct findTodosParams
{
    int limit;
    int since;
};

struct findTodoResponse
{
    std::string data;
    int code;
};

// defines api tag methods
class todoservice
{
public:
    todoservice(std::shared_ptr<IClient> cli):_cli(cli) {};
    std::shared_ptr<IClient> _cli;
    findTodoResponse findTodos(findTodosParams params);
};

// class todoclient
// {
// public:
//     todoclient(transport tr):_ts(tr){};
//     todoservice _ts;
//     //void addOne(std::string param);
// };