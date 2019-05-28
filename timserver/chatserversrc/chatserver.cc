/**
 *
 * */
#include "chatserver.hpp"

static void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " tcp_port udp_port" << std::endl;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }

    uint16_t tcp_port = atoi(argv[1]);
    uint16_t udp_port = atoi(argv[2]);

    chatserver::ChatServer *sp = new chatserver::ChatServer(tcp_port, udp_port);
    sp->InitChatServer();
    sp->Start();

    return 0;
}
