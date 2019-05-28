/**
 * 关于服务端类的声明和实现
 * */
#pragma once
#include <iostream>

#include <unistd.h>

#include <pthread.h>

#include "../net/SocketApi.hpp"
#include "../net/CustomProtocol.hpp"
#include "./UserManger.hpp"

namespace chatserver
{
    class ChatServer;

    struct param_t
    {
        ChatServer* _sp;
        int32_t     _sock;
    public:
        param_t(ChatServer* sp, int32_t sock)
            : _sp(sp),
              _sock(sock)
        {}

        ~param_t()
        {}
    };

    class ChatServer
    {
    private:
        /**
         * 这个套接字信息 
         * 作为注册、登录和退出逻辑的套接字
         * */
        int32_t _listen_sock;
        uint16_t _listen_port;

        /**
         * 负责消息的收发
         * */
        int32_t _message_sock;
        uint16_t _message_port;

        usermanger::UserManger um;

    public:
        ChatServer(uint16_t listen_port = 30001, uint16_t message_port = 30002)
            : _listen_sock(-1),
              _listen_port(listen_port),
              _message_sock(-1),
              _message_port(message_port)
        {}

        ~ChatServer()
        {}

        void InitChatServer()
        {
            _listen_sock = net::SocketApi::Socket(SOCK_STREAM); 
            _message_sock = net::SocketApi::Socket(SOCK_DGRAM);

            net::SocketApi::Bind(_listen_sock, _listen_port);
            net::SocketApi::Bind(_message_sock, _message_port);

            net::SocketApi::Listen(_listen_sock);
        }

        void Start()
        {
            pthread_t tid = 0;
            pthread_create(&tid, nullptr, UserManger, nullptr);
            pthread_create(&tid, nullptr, UserManger, nullptr);

            struct sockaddr_in peer;
            for(; ; )
            {
                int32_t sock = net::SocketApi::Accept(_listen_sock, &peer);
                if(sock >= 0)
                {
                    std::cout << "get a new client " << inet_ntoa(peer.sin_addr) << ":" << ntohs(peer.sin_port)<< std::endl;

                    param_t* p = new param_t(this, sock);
                    pthread_t tid = 0;
                    pthread_create(&tid, nullptr, HandlerRequest, p);
                }
            }
        }

    private:
        static uint32_t UserRegister(const std::string& nickName, const std::string& passWord);

        static uint32_t UserLogin(const uint32_t id, const std::string& password);

        static void* UserManger(void*)
        {
            return nullptr;
        }

        static void* HandlerRequest(void* arg)
        {
            param_t* p = static_cast<param_t*>(arg);
            int sock = p->_sock;
            ChatServer* sp = p->_sp;
            delete p;

            pthread_detach(pthread_self());

            customprotocol::Request req;
            customprotocol::util::RecvRequest(sock, &req);

            std::cout << "[DEBUG]" << " req._method = " << req._method << std::endl;
            std::cout << "[DEBUG]" << " req._contentLength = " << req._contentLength << std::endl;
            std::cout << "[DEBUG]" << " req = " << req._body << std::endl;

            Json::Value value;
            customprotocol::util::DeSerialize(req._body, value);

            // REGISTER
            if(req._method == "REGISTER")
            {
                std::string nickName = value["nickname"].asString();
                std::string passWord = value["password"].asString();

                uint32_t id = 10001; // sp->UserRegister(nickName, passWord);
                std::cout << "[DEBUG]" << "id = " << id << std::endl;
                int32_t ret = ::send(sock, &id, sizeof(uint32_t), 0);
                if(ret < 0)
                {
                    std::cerr <<  __FILE__ << ":" << __LINE__ << "::send() error." << std::endl;
                }
            }
            else if(req._method == "LOGIN")
            {
                uint32_t id = value["id"].asUInt();
                std::string passWord = value["password"].asString();

                uint32_t result = 10001; // sp->UserLogin(id, passWord);
                ::send(sock, &result, sizeof(uint32_t), 0);
            }
            else
            {
                // LOGOUT TODO
            }

            close(sock);
            return nullptr;
        }
    };
}
