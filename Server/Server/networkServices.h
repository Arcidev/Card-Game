#pragma once
#include <cstdint>
#include "../Multiplatform/NetworkCommunication.h"

// port to connect sockets through 
#define DEFAULT_PORT "6881"

class NetworkServices
{
    public:
        static int sendMessage(SOCKET const& curSocket, uint8_t const* message, int messageSize);
        static int receiveMessage(SOCKET const& curSocket, char* buffer, int bufSize);
};

