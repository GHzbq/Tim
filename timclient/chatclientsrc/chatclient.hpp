/**
 * 客户端的声明和实现
 * */

#pragma once

#include <iostream>
#include <string>

#include "../net/SocketApi.hpp"
#include "../net/CustomProtocol.hpp"

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

        uint32_t _id;
        std::string _nickName;
        std::string _password;
    public:
        ChatClient(std::string ip)
            : _tcp_sock(-1),
              _udp_sock(-1),
              _peer_ip(ip),
              _id(0)
        {}

        ~ChatClient()
        {}

        void InitChatClient()
        {
            _udp_sock = net::SocketApi::Socket(SOCK_DGRAM);
        }

        bool ConnectToServer()
        {
            _tcp_sock = net::SocketApi::Socket(SOCK_STREAM);
            return net::SocketApi::Connect(_tcp_sock, _peer_ip, TCP_PORT);
        }

        bool Register()
        {
            bool res = false;
            if(GetRegisterInfo(&_nickName, &_password) && ConnectToServer())
            {
                customprotocol::Request req;
                req._method = "REGISTER\n";

                Json::Value value;
                value["nickname"] = _nickName;
                value["password"] = _password;
                customprotocol::util::Serialize(value, req._body);
                req._contentLength = "Content-Length: ";
                req._contentLength += customprotocol::util::IntToString((req._body).size());
                req._contentLength += '\n';

                customprotocol::util::SendRequest(_tcp_sock, req);
                ::recv(_tcp_sock, &_id, sizeof(uint32_t), 0);

                std::cout << "[DEBUG] _id = " << _id << std::endl;
                if(_id >= 10000)
                {
                    std::cout << "Register Success :) Your tim id is: " << _id << std::endl;
                    res = true;
                }
                else
                {
                    std::cout << "Register failed. Code is : " << _id << std::endl;
                    res = false;
                }
                ::close(_tcp_sock);
                _tcp_sock = -1;
            }
            return res;
        }

        bool Login()
        {
            bool res = false;
            if(GetLoginInfo(&_id, &_password) && ConnectToServer())
            {
                customprotocol::Request req;
                req._method = "LOGIN\n";

                Json::Value value;
                value["id"] = _id;
                value["password"] = _password;
                customprotocol::util::Serialize(value, req._body);
                req._contentLength = "Content-Length: ";
                req._contentLength += customprotocol::util::IntToString((req._body).size());
                req._contentLength += '\n';

                customprotocol::util::SendRequest(_tcp_sock, req);
                uint32_t key = 0;
                ::recv(_tcp_sock, &key, sizeof(uint32_t), 0);

                if(_id >= 10000)
                {
                    std::cout << "Login Success :)" << std::endl;
                    res = true;
                }
                else
                {
                    std::cout << "Login failed. Code is : " << _id << std::endl;
                }
                ::close(_tcp_sock);
                _tcp_sock = -1;
            }
            return res;
        }

        void Chat()
        {}
    private:
        static bool GetRegisterInfo(std::string * nickname, std::string * passwd)
        {
            std::cout << "Please input nickname:> ";
            std::cout.flush();
            std::cin >> *nickname;

            std::cout << "Please input password:> ";
            std::cout.flush();
            std::cin >> *passwd;

            std::cout << "Please input confirm passwd:> ";
            std::cout.flush();
            std::string confirmPasswd;
            std::cin >> confirmPasswd;

            if(*passwd != confirmPasswd)
            {
                return false;
            }
            else
            {
                return true;
            }
        }

        static bool GetLoginInfo(uint32_t * id, std::string * passwd)
        {
            std::cout << "Please input id:> " << std::endl;
            std::cin >> *id;
            std::cout << "Please input password:> " << std::endl;
            std::cin >> *passwd;

            return true;
        }
    };
}
