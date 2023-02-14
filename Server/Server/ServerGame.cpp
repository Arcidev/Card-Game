#include <csignal>
#include "ServerGame.h"
#include "SignalHandler.h"
#include "Player.h"
#include "../Shared/SharedDefines.h"

unsigned int ServerGame::m_clientId = 1;

ServerGame::ServerGame() : m_isShuttingDown(false) { }

// Looking for new connections and data from connected clients
void ServerGame::update()
{
    // get new clients
    if (m_network.AcceptNewClient(m_clientId, &m_achievementMgr))
    {
        DEBUG_LOG("Client %d has been connected to the server\r\n", m_clientId);
        m_clientId++;
    }
}

// Infinite server update loop
void ServerGame::Loop()
{
    SignalHandler::RegisterMethod(SIGINT, [&]() { m_isShuttingDown = true; m_network.Close(); return true; });
    while (!m_isShuttingDown)
        update();
}
