#pragma once 

#include <iostream>
#include <sstream>

#include <sys/time.h>

namespace Util
{
    class TimeUtil
    {
    public:
        static int64_t getTimeStamp()
        {
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            return tv.tv_sec + tv.tv_usec / 1000;
        }
    };

    enum ERRORLEVEL
    {
        NOTE = 0,
        WARNING,
        ERROR,
    };


    static const char* ErrorLevelInfo[] = {
        "NOTE",
        "WARNING",
        "ERROR"
    };

    inline std::ostream& log(ERRORLEVEL level, std::string fileName, int fileNo)
    {
        std::stringstream ss;
        ss << '[' << ErrorLevelInfo[level] << "][" << TimeUtil::getTimeStamp() << "][" \
            << fileName << ':' << fileNo << "] ";
        return std::cout << ss;
    }

#define LOG(level) log(level, __FILE__, __LINE__)
}
