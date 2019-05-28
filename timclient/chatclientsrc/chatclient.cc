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

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    chatclient::ChatClient *cp = new chatclient::ChatClient(argv[1]);
    cp->InitChatClient();

    bool ret_conn = cp->ConnectToServer();
    if(ret_conn)
    {
        std::cout << "connect success." << std::endl;
        // 为了方便观察，我们写一个死循环
        while(1){}
    }
    else
    {
        std::cout << "connect failed." << std::endl;
    }

    return 0;
}
