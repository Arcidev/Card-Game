#pragma once
#include <cstdint>
#include "../Multiplatform/NetworkCommunication.h"

// port to connect sockets through 
constexpr auto DEFAULT_PORT = "10751";

class NetworkServices
{
    public:
        static int SendMessage(SOCKET const& curSocket, uint8_t const* message, int messageSize);
        static int ReceiveMessage(SOCKET const& curSocket, char* buffer, int bufSize);
};

