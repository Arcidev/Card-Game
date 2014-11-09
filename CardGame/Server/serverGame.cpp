#include "serverGame.h"
#include "Player.h"

unsigned int ServerGame::m_clientId = 0;

void ServerGame::update()
{
    // get new clients
    if (m_network.AcceptNewClient(m_clientId))
    {
        printf("Client %d has been connected to the server\n", m_clientId);

        m_clientId++;
    }

    receiveFromClients();
}

void ServerGame::receiveFromClients()
{
    for (PlayerMap::iterator iter = m_network.GetPlayers().begin(); iter != m_network.GetPlayers().end();)
    {
        // get data for that client
        int data_length = m_network.ReceiveData(iter->second, m_networkData);
        if (!data_length)
        {
            delete iter->second;
            iter = m_network.GetPlayers().erase(iter);
            continue;
        }

        // invalid packet sended
        if (data_length < 2)
        {
            iter++;
            continue;
        }

        iter->second->ReceivePacket(data_length, m_networkData);
        iter++;
    }
}

void ServerGame::Loop()
{
    while (true)
        update();
}
