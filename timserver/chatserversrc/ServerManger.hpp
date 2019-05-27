#include <iostream>
#include <string>

#include <pthread.h>

#include "../base/LogUtil.hpp"
#include "../net/SocketApi.hpp"

namespace ServerManger
{
    // 单个用户
    struct User
    {
    public:
        int32_t     _userID;
        std::string _userName;
        std::string _nickName;
        std::string _passWord;
    };

    struct FriendInfo
    {
    public:
        int32_t     _friendID; // 朋友的ID
        std::string _markName; // 对该朋友的备注
        std::string _teamName; // 该朋友属于哪一个分组
    };

    class ServerManger
    {
    private:
        int32_t _listen_sock;  // 这是一个 SOCK_STREAM 类型的socket，负责登录、注册、登出等逻辑
        int32_t _message_sock; // 这是一个 SOCK_DGRAM 类型的socket，负责收发数据，放到数据池中
    public:
        ServerManger()
            : _listen_sock(-1),
              _message_sock(-1)
        {}
    
        ~ServerManger()
        {}
    
        bool InitServerManger()
        {
            _listen_sock = net::SocketApi::Socket(SOCK_STREAM);
            _message_sock = net::SocketApi::Socket(SOCK_DGRAM);
    
            if ( (_listen_sock < 0) || (_message_sock < 0) )
            {
                LOG(Util::ERROR) << "ServerManger::InitServerManger() error." << std::endl;
                return false;
            }
    
            return true;
        }

        static void * Register(void *)
        {
            // TODO 负责注册逻辑

            return nullptr;
        }
    
        bool Start()
        {
            pthread_t tid = 0;
            pthread_create(&tid, nullptr, Register, nullptr);
            pthread_detach(tid);
    
            return true;
        }
    };
}
