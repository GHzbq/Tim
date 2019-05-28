/**
 * filename: SocketApi.hpp
 * author:   GHzbq
 * date:     2019.05.28
 * function: 简单封装网络API
 * */
#pragma once

#include <iostream>
#include <cstdlib>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "../base/LogUtil.hpp"
#include "./CustomProtocol.hpp"

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

        /**
         * 对服务器来说，其实没必要指定 绑定的IP地址的
         * 只需要绑定 INADDR_AY 就好了
         * 但是为了接口的统一，我们将 IP 作为这个函数的第三个参数
         * 并且将 INADDR_ANY 作为默认参数 
         * 考虑到 INADDR_ANY 其实就是 0 (可以通过 grep -ER 'INADDR_ANY' /usr/include 查看)
         * /usr/include/netinet/in.h:#define INADDR_ANY      ((in_addr_t) 0x00000000)
         * ip 的类型本应该是 string 类型(可以理解为 “点分十进制”)
         * 所以我们把 ip 的默认参数给为 "0.0.0.0"
         * */
        static int32_t Bind(int32_t sockfd, int32_t port, std::string ip = "0.0.0.0")
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
            /**
             * 关于 listen 第二个参数 
             * 是底层链接队列的长度 +1 
             * 不是最大链接数
             * */
            int32_t ret = ::listen(sockfd, LISTENBLOCK);
            if(ret < 0)
            {
                LOG(Util::ERROR) << "::listen() error." << std::endl;
                exit(4);
            }

            return ret;
        }

        /**
         * 关于这个对端的地址信息
         * 本来想给引用参数的
         * 后来觉得能指针更合适一点，也许我们不想要对端地址信息
         * 直接给一个 nullptr 指针不是挺好吗？
         * 所以给 peer 一个默认参数 nullptr
         * */
        static int32_t Accept(int32_t sockfd, struct sockaddr_in*peer)
        {
            socklen_t addrLen = sizeof(struct sockaddr_in);
            /**
             * 考虑到 peer 指针可能是 nullptr
             * 是需要处理一下的
             * */
            if (peer == nullptr)
            {
                struct sockaddr_in addr;
                peer = &addr;
            }

            /**
             * 关于 ::accept函数的返回值，我们可能需要讨论一下
             * 返回值是一个套接字
             *
             * accept 失败，并一定是服务器失败了，就是说这个错误不算致命
             * 但是我们得记录一下 打一个 WARNING 警告好了
             * */
            int32_t ret = ::accept(sockfd, (struct sockaddr*)peer, &addrLen);
            if(ret < 0)
            {
                LOG(Util::WARNING) << "::accept() error." << std::endl;
                return -1;
            }

            return ret;
        }

        static bool Connect(const int32_t& sock, std::string peer_ip, const uint32_t& port)
        {
            struct sockaddr_in peer;
            peer.sin_family = AF_INET;
            peer.sin_addr.s_addr = inet_addr(peer_ip.c_str());
            peer.sin_port = htons(port);

            int32_t ret = ::connect(sock, (struct sockaddr*)&peer, sizeof(struct sockaddr_in));
            if(ret < 0)
            {
                LOG(Util::WARNING) << "::connect() error." << std::endl;
                return false;
            }

            return true;
        }

        static void Send(int sock, customprotocol::Request& req)
        {
            std::string &method = req._method;
            std::string &contentLength = req._contentLength;
            std::string &nullString = req._nullString; 
            std::string &body = req._body;
            ::send(sock, method.c_str(), method.size(), 0);
            ::send(sock, contentLength.c_str(), contentLength.size(), 0);
            ::send(sock, nullString.c_str(), nullString.size(), 0);
            ::send(sock, body.c_str(), body.size(), 0);
        }
    };
}
