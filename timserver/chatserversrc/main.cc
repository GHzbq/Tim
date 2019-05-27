#include <iostream>
#include <memory>

#include "../base/LogUtil.hpp"
#include "../net/SocketApi.hpp"
#include "./ServerManger.hpp"
#include "../base/Singleton.hpp"

int main(void)
{
    ServerManger* pServer = new ServerManger();
    pServer->InitServerManger();

    return 0;
}
