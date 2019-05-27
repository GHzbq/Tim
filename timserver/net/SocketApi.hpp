#pragma once

#include <iostream>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../base/LogUtil.hpp"

namespace net
{
    class SocketApi
    {
    public:
        static const int32_t LISTENBLOCK = 5;
    public:
        static int32_t Socket(int32_t type)
        {
            int32_t sock_ = ::socket(AF_INET, type, 0);
            if(sock_ < 0)
            {
                LOG(Util::ERROR) << "::socket() error." << std::endl;
                exit(2);
            }

            return sock_;
        }

        static int32_t Bind(int32_t sockfd, std::string ip, int32_t port)
        {
            struct sockaddr_in local;
            local.sin_family = AF_INET;
            local.sin_addr.s_addr = inet_addr(ip.c_str());
            local.sin_port = htons(port);
            socklen_t addrLen = sizeof(struct sockaddr_in);
            int32_t ret = ::bind(sockfd, (struct sockaddr*)&local, addrLen);
            if(ret < 0)
            {
                LOG(Util::ERROR) << "::bind() error." << std::endl;
                exit(3);
            }

            return ret;
        }

        static int32_t Listen(int32_t sockfd)
        {
            int32_t ret = ::listen(sockfd, LISTENBLOCK);
            if(ret < 0)
            {
                LOG(Util::ERROR) << "::listen() error." << std::endl;
                exit(4);
            }

            return ret;
        }

        static int32_t Accept(int32_t sockfd, struct sockaddr_in& peer)
        {
            socklen_t addrLen = sizeof(struct sockaddr_in);
            int32_t ret = ::accept(sockfd, (struct sockaddr*)&peer, &addrLen);
            if(ret < 0)
            {
                LOG(Util::ERROR) << "::accept() error." << std::endl;
            }

            return ret;
        }
    };
}
