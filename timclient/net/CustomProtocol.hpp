/**
 * 自定制协议
 * */
#pragma once

#include <iostream>
#include <string>
#include <sstream>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <jsoncpp/json/json.h>

namespace customprotocol
{
    struct Request
    {
    public:
        std::string _method;        // 请求方法：REGISTER LOGIN LOGOUT
        std::string _contentLength; // body部分的长度 格式："Content-Length: 123"
        std::string _nullString;    // 空行
        std::string _body;          // 正文
    public:
        Request()
            : _nullString("\n")
        {}

        ~Request()
        {}
    };

    class util
    {
    public:
        static void Serialize(const Json::Value & value, std::string &outString)
        {
            Json::FastWriter writer;
            outString = writer.write(value);
        }

        static void DeSerialize(const std::string &inString, Json::Value & value)
        {
            Json::Reader reader;
            reader.parse(inString, value, false);
        }

        static std::string IntToString(uint32_t number)
        {
            std::stringstream ss;
            ss << number;
            return ss.str();
        }

        static int32_t StringToInt32_t(const std::string& inString)
        {
            std::stringstream ss(inString);
            int32_t ret = 0;
            ss >> ret;
            return ret;
        }

        static void RecvOneLine(int sock, std::string * outString)
        {
            if (outString == nullptr)
                return;

            int8_t c= 0;
            while(1)
            {
                int32_t s = ::recv(sock, &c, 1, 0);
                if(s > 0)
                {
                    if( c ==  '\n')
                    {
                        break;
                    }
                    outString->push_back(c);
                }
            }
        }

        static void RecvRequest(int sock, Request* req)
        {
            if(req == nullptr)
                return;
            RecvOneLine(sock, &(req->_method));
            RecvOneLine(sock, &(req->_contentLength));
            RecvOneLine(sock, &(req->_nullString));
            // RecvOneLine(sock, &(req->_body)); //直接这么做是有问题的
            std::string &contentlength = req->_contentLength; // Conten-Length: 123
            auto pos = contentlength.find(": ");
            if(pos == std::string::npos)
            {
                return;
            }

            std::string sub = contentlength.substr(pos+2);
            int32_t size = StringToInt32_t(sub);
            int8_t c = 0;
            for(int32_t i = 0; i < size; ++i)
            {
                ::recv(sock, &c, 1, 0);
                (req->_body).push_back(c);
            }
        }

        static void SendRequest(const int sock, const Request& req)
        {
            ::send(sock, (req._method).c_str(), (req._method).size(), 0);
            ::send(sock, (req._contentLength).c_str(), (req._contentLength).size(), 0);
            ::send(sock, (req._nullString).c_str(), (req._nullString).size(), 0);
            ::send(sock, (req._body).c_str(), (req._body).size(), 0);
        }
    };
}
