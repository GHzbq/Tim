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
            LOG(util::ERROR) << "poll() error" << std::endl;
            break;
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
                fds[userCount].events = POLLIN | POLLRDHUP | POLLERR;
                fds[userCount].revents = 0;
            }
            else if(fds[i].revents & POLLERR)
            {
                std::cout << "get an error from " << fds[i].fd << std::endl;
                char errors[100] = {0};
                socklen_t len = sizeof(errors)/sizeof(errors[0]);
                memset(errors, 0x00, len);
                if(getsockopt(fds[i].fd, SOL_SOCKET, SO_ERROR, errors, &len) < 0)
                {
                    std::cout << "get socket option failed" << std::endl;
                }
                continue;
            }
            else if(fds[i].revents & POLLRDHUP)
            {
                // 客户端关闭连接，服务器也关闭连接，并且客户数量-1
                client[fds[i].fd] = client[fds[userCount].fd];
                close(fds[i].fd);
                fds[i] = fds[userCount];
                --i;
                --userCount;
                std::cout << "a client left" << std::endl;
            }
            else if(fds[i].revents & POLLIN)
            {
                int& clientSock = fds[i].fd;
                memset(client[clientSock].buf, 0x00, BUFF_SIZE);
                ret = recv(clientSock, client[clientSock].buf, BUFF_SIZE-1, 0);
                if(ret < 0)
                {
                    if(errno != EAGAIN)
                    {
                        close(clientSock);
                        client[fds[i].fd] = client[fds[userCount].fd];
                        fds[i] = fds[userCount];
                        --i;
                        --userCount;
                    }
                }
                else if(ret == 0)
                {
                }
                else 
                {
                    client[clientSock].buf[ret] = 0;
                    std::cout << "get " << ret << " bytes of client data [" 
                        << client[clientSock].buf << "] from " << clientSock << std::endl;
                    for(int j = 1; j <= userCount; ++j)
                    {
                        if(fds[i].fd == clientSock)
                        {
                            continue;
                        }

                        fds[j].events |= ~POLLIN;
                        fds[j].events |= POLLOUT;
                        client[fds[j].fd].writeBuf = client[clientSock].buf;
                    }
                }
            }
            else if(fds[i].events & POLLOUT)
            {
                int& clientSock = fds[i].fd;
                if(!client[clientSock].writeBuf)
                {
                    continue;
                }
                int len = strlen(client[clientSock].writeBuf);
                ret = send(clientSock, client[clientSock].writeBuf, len, 0);
                if(ret < 0 || ret != len)
                {
                    LOG(util::ERROR) << "send() error" << std::endl;
                    continue;
                }
                client[clientSock].writeBuf = NULL;
                fds[i].events |= POLLOUT;
                fds[i].events |= POLLIN;
            }
        }// end of for(int i)
    }

    delete[] client;
    close(listenSock);

    return 0;
}
