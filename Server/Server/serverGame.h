#pragma once
#include "serverNetwork.h"

class ServerGame
{
    private:
        // IDs for the clients connecting for table in ServerNetwork 
        static unsigned int m_clientId;
        // The ServerNetwork object 
        ServerNetwork m_network;
        // data buffer
        char m_networkData[MAX_PACKET_SIZE];

        void update();
        void receiveFromClients();

    public:
        void Loop();
};

