#pragma once
#include <map>
#include "../Multiplatform/NetworkCommunication.h"
#include "../Shared/networkServices.h"
#include "../Shared/Packet.h"

class Player;
class Packet;
typedef std::map<unsigned int, Player*> PlayerMap;

class ServerNetwork
{
    private:
        Player* m_lastPlayer;
        PlayerMap m_players;
        int iResult;
        SOCKET ListenSocket;
        SOCKET ClientSocket;

    public:
        ServerNetwork();
        ~ServerNetwork();

        // accept new connections
        bool AcceptNewClient(unsigned int & id);
        // receive incoming data
        int ReceiveData(Player* player, char* recvbuf);
        void BroadcastPacket(Packet const* packet) const;
        bool SendPacketToPlayer(std::string const& playerName, Packet const* packet) const;

        PlayerMap& GetPlayers() { return m_players; }
        PlayerMap const& GetPlayers() const { return m_players; }

        void OnPlayerDisconnected(Player* player);
};

