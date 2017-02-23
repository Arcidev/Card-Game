#include "ServerGame.h"
#include "Player.h"
#include "../Shared/SharedDefines.h"

unsigned int ServerGame::m_clientId = 1;

// Looking for new connections and data from connected clients
void ServerGame::update()
{
    // get new clients
    if (m_network.AcceptNewClient(m_clientId))
    {
        DEBUG_LOG("Client %d has been connected to the server\n", m_clientId);
        m_clientId++;
    }
}

// Infinite server pdate loop
void ServerGame::Loop()
{
    while (true)
        update();
}
