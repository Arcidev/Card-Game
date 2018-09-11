#pragma once
#include <map>
#include <mutex>
#include <thread>
#include "../Multiplatform/NetworkCommunication.h"
#include "NetworkServices.h"
#include "PacketHandlers/Packet.h"

class Player;
class Packet;
typedef std::pair<Player*, std::thread*> PlayerThread;
typedef std::map<unsigned int, PlayerThread> PlayerMap;

class ServerNetwork
{
    private:
        Player* m_lastPlayer;
        PlayerMap m_players;
        SOCKET m_listenSocket;
        bool m_shuttingDown;
        std::mutex m_locker;

        static void handlePlayerNetwork(Player* player);

    public:
        ServerNetwork();
        ~ServerNetwork();

        void Close() { shutdown(m_listenSocket, SD_BOTH); closesocket(m_listenSocket); }
        // accept new connections
        bool AcceptNewClient(unsigned int const& id);
        // receive incoming data
        int ReceiveData(Player const* player, char* recvbuf) const;
        void BroadcastPacket(Packet const& packet) const;
        bool SendPacketToPlayer(std::string_view playerName, Packet const& packet) const;
        bool IsShuttingDown() const { return m_shuttingDown; };

        PlayerMap& GetPlayers() { return m_players; }
        PlayerMap const& GetPlayers() const { return m_players; }
        std::mutex& GetLocker() { return m_locker; }

        void OnPlayerDisconnected(Player const* player);
};
