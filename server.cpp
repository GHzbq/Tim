#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include "util.hpp"

#define  USER_LIMIT    5
#define  BUFF_SIZE     64
#define  FD_LIMIT      65535

struct clientData
{
    sockaddr_in address;
    char*       writeBuf;
    char        buf[BUFF_SIZE];
};

int setNonBlocking(int fd)
{
    int oldOption = fcntl(fd, F_GETFL);
    int newOption = oldOption | O_NONBLOCK;
    fcntl(fd, F_GETFL, newOption);
    return oldOption;
}

int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " server_ip + port" << std::endl;
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenSock < 0)
    {
        LOG(util::ERROR) << "socket() error" << std::endl;
        return -1;
    }

    int opt = 1;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr;
    socklen_t serverAddrLen = sizeof(sockaddr_in);
    bzero(&serverAddr, serverAddrLen);
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(port);
    int ret = bind(listenSock, (sockaddr*)&serverAddr, serverAddrLen);
    if(ret < 0)
    {
        LOG(util::ERROR) << "bind() error" << std::endl;
        close(listenSock);
        return -1;
    }

    ret = listen(listenSock, 5);
    if(ret < 0)
    {
        LOG(util::ERROR) << 
            "listen() error" << std::endl;
        close(listenSock);
        return -1;
    }

    clientData* client = new clientData[FD_LIMIT];
    pollfd fds[USER_LIMIT+1];

    while(1)
    {
        ;
    }

    close(listenSock);

    return 0;
}
