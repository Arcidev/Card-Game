#pragma once
#include "ServerNetwork.h"

class ServerGame
{
    private:
        bool m_isShuttingDown;
        // Ids for the clients connecting for table in ServerNetwork 
        static unsigned int m_clientId;
        // The ServerNetwork object 
        ServerNetwork m_network;

        void update();

    public:
        ServerGame();
        void Loop();
};

