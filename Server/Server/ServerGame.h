#pragma once
#include "ServerNetwork.h"

class ServerGame
{
    private:
        // IDs for the clients connecting for table in ServerNetwork 
        static unsigned int m_clientId;
        // The ServerNetwork object 
        ServerNetwork m_network;

        void update();

    public:
        void Loop();
};

