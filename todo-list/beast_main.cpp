#include "todoclient.h"
#include "beastclient.h"
#include <iostream>

int main()
{
    try
    {
        ClientConfig cfg = {
            "localhost",
            "12345",
            "/",
            };
        
        std::shared_ptr<IClient> cli =  std::make_shared<BeastClient>(cfg);

        todoservice ts(cli);

        findTodosParams p;
        findTodoResponse r = ts.findTodos(p);
        std::cout << r.code << " " << r.data << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}