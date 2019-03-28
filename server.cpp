#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>
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
    int userCount = 0;
    for(int i = 1; i <= USER_LIMIT; ++i)
    {
        fds[i].fd = -1;
        fds[i].events = 0;
    }
    fds[0].fd = listenSock;
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;

    while(1)
    {
        ret = poll(fds, userCount+1, -1);
        if(ret < 0)
        {
            ;
        }

        for(int i = 0; i < userCount+1; ++i)
        {
            if((fds[i].fd == listenSock) && (fds[i].revents & POLLIN))
            {
                // 有新连接
                sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(sockaddr_in);
                bzero(&clientAddr, clientAddrLen);
                int clientSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen);
                if(clientSock < 0)
                {
                    // err
                    LOG(util::ERROR) << "accept() error" << std::endl;
                    continue;
                }

                if(userCount >= USER_LIMIT)
                {
                    // 连接过多
                    const std::string info = "too many users";
                    std::cout << info << std::endl;
                    ret = send(clientSock, info.c_str(), info.size(), 0);
                    if(ret < 0 || (size_t)ret != info.size())
                    {
                        LOG(util::ERROR) << "send() error" << std::endl;
                    }
                    close(clientSock);
                    continue;
                }

                // 新连接：
                ++userCount;
                client[clientSock].address = clientAddr;
                setNonBlocking(clientSock);
                fds[userCount].fd = clientSock;

            }
        }
    }

    close(listenSock);

    return 0;
}
