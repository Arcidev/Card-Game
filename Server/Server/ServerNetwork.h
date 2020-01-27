#pragma once
#include <map>
#include <mutex>
#include <thread>
#include "../Multiplatform/NetworkCommunication.h"

class ConnectedUser;
class Game;
class Packet;
class Player;

typedef std::pair<ConnectedUser*, std::thread*> UserThread;
typedef std::map<unsigned int, UserThread> UserMap;

class ServerNetwork
{
    private:
        Player const* m_lastPlayer;
        UserMap m_users;
        SOCKET m_listenSocket;
        bool m_shuttingDown;
        std::mutex m_locker;

        static void handlePlayerNetwork(ConnectedUser* user);

    public:
        ServerNetwork();
        ~ServerNetwork();

        void Close() { shutdown(m_listenSocket, SD_BOTH); closesocket(m_listenSocket); }
        // accept new connections
        bool AcceptNewClient(unsigned int const& id);
        // receive incoming data
        int ReceiveData(ConnectedUser const* player, char* recvbuf) const;
        void BroadcastPacket(Packet const& packet) const;
        bool SendPacketToPlayer(std::string_view playerName, Packet const& packet) const;
        bool IsShuttingDown() const { return m_shuttingDown; };

        UserMap& GetUsers() { return m_users; }
        UserMap const& GetUsers() const { return m_users; }
        std::mutex& GetLocker() { return m_locker; }
        void SetLastPlayer(Player const* user) { m_lastPlayer = user; }

        void OnPlayerDisconnected(ConnectedUser const* user);
        Game* GetGameForPlayer();
};
