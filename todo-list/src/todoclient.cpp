
#include "todoclient.h"
#include "request.h"
#include "beastclient.h"
#include "curlppclient.h"

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
    return result;
}