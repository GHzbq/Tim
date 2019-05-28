/**
 * 核心调度逻辑
 * */

#include <iostream>
#include <cstdlib>

#include <unistd.h>

#include "./chatclient.hpp"

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " serverip" << std::endl;
}

static void Menu(int32_t &input)
{
    std::cout << "************************************" << std::endl;
    std::cout << "**   1.Register         2.Login   **" << std::endl;
    std::cout << "**                      3.Exit    **" << std::endl;
    std::cout << "************************************" << std::endl;
    std::cout << "Please select:> ";
    std::cin >> input;
}

int main(int32_t argc, char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    chatclient::ChatClient *cp = new chatclient::ChatClient(argv[1]);
    cp->InitChatClient();

    int32_t choice = 0;
    while(1)
    {
        Menu(choice);
        switch(choice)
        {
        case 1: // Register
            cp->Register();
            break;
        case 2: // Login
            if(cp->Login())
            {
                cp->Chat();
            }
            break;
        case 3: // Exit
            exit(0);
            break;
        default:
            break;
        }
    }


    // bool ret_conn = cp->ConnectToServer();
    // if(ret_conn)
    // {
    //     std::cout << "connect success." << std::endl;
    //     // 为了方便观察，我们写一个死循环
    //     while(1){}
    // }
    // else
    // {
    //     std::cout << "connect failed." << std::endl;
    // }

    return 0;
}
