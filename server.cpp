#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " ip + port" << std::endl;
        return 1;
    }

    return 0;
}
