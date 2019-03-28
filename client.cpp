#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include "util.hpp"

#define BUFF_SIZE 64

int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " server_ip + port" << std::endl;
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0)
    {
        LOG(util::ERROR) << "socket() error" << std::endl;
        return -1;
    }
    sockaddr_in serverAddr;
    socklen_t   serverAddrLen = sizeof(sockaddr_in);
    bzero(&serverAddr, serverAddrLen);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    serverAddr.sin_port = htons(port);
    int retConnect = connect(sockfd, (sockaddr*)&serverAddr, serverAddrLen);
    if (retConnect < 0)
    {
        LOG(util::ERROR) << "connect() error" << std::endl;
        close(sockfd);
        return -1;
    }

    pollfd fds[2];
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN | POLLRDHUP;
    fds->revents = 0;
    
    char readBuf[BUFF_SIZE] = {0};
    int pipefd[2];
    int ret = pipe(pipefd);
    if(ret < 0)
    {
        LOG(util::ERROR) << "pipe() error" << std::endl;
        close(sockfd);
        return -1;
    }

    while(1)
    {
        ret = poll(fds, 2, -1);
        if(ret < 0)
        {
            LOG(util::ERROR) << "poll() error" << std::endl;
            break;
        }

        if(fds[1].revents & POLLRDHUP)
        {
            std::cout << "server close the connection" << std::endl;
            break;
        }
        else if(fds[1].revents & POLLIN)
        {
            memset(readBuf, 0x00, BUFF_SIZE);
            recv(fds[1].fd, readBuf, BUFF_SIZE-1, 0);

            std::cout << readBuf << std::endl;
        }

        if(fds[0].revents & POLLIN)
        {
            /* 使用splice将用户输入的数据直接写到sockfd上 */
            ret = splice(0, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
            ret = splice(pipefd[0], NULL, sockfd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        }
    }

    close(sockfd);
    return 0;
}
