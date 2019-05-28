/**
 * 客户端的声明和实现
 * */

#pragma once

#include <iostream>
#include <string>

#include "../net/SocketApi.hpp"

namespace chatclient
{
    const static uint16_t TCP_PORT = 30001;
    const static uint16_t UDP_PORT = 30002;

    class ChatClient
    {
    private:
        int32_t _tcp_sock; // 负责注册、登录、退出逻辑
        int32_t _udp_sock; // 负责收发消息
        std::string _peer_ip; // 服务端 IP 地址
    public:
        ChatClient(std::string ip)
            : _peer_ip(ip)
        {}

        ~ChatClient()
        {}

        void InitChatClient()
        {
            _tcp_sock = net::SocketApi::Socket(SOCK_STREAM);
            _udp_sock = net::SocketApi::Socket(SOCK_DGRAM);
        }

        bool ConnectToServer()
        {
            return net::SocketApi::Connect(_tcp_sock, _peer_ip, TCP_PORT);
        }
    };
}
