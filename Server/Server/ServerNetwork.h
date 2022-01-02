#pragma once
#include <cstdint>
#include <map>
#include <mutex>
#include <thread>
#include "../Multiplatform/NetworkCommunication.h"

class AchievementManager;
class ConnectedUser;
class Game;
class Packet;
class Player;

typedef std::pair<ConnectedUser*, std::thread*> UserThread;
typedef std::map<uint32_t, ConnectedUser*> DbUserMap;
typedef std::map<uint32_t, UserThread> UserMap;
typedef std::map<std::string, ConnectedUser*> UserNameMap;

class ServerNetwork
{
    private:
        Player const* m_lastPlayer;
        UserMap m_users;
        UserNameMap m_userNames;
        DbUserMap m_dbUsers;
        SOCKET m_listenSocket;
        bool m_shuttingDown;
        std::mutex m_locker;

        static void handlePlayerNetwork(ConnectedUser* user);

    public:
        ServerNetwork();
        ~ServerNetwork();

        void Close() { shutdown(m_listenSocket, SD_BOTH); closesocket(m_listenSocket); }
        // accept new connections
        bool AcceptNewClient(uint32_t id, AchievementManager* achievementMgr);
        // receive incoming data
        int ReceiveData(ConnectedUser const* player, char* recvbuf) const;
        void BroadcastPacket(Packet const& packet, ConnectedUser const* sender = nullptr) const;
        bool IsShuttingDown() const { return m_shuttingDown; };

        UserMap& GetUsers() { return m_users; }
        UserMap const& GetUsers() const { return m_users; }
        DbUserMap& GetDbUsers() { return m_dbUsers; }
        DbUserMap const& GetDbUsers() const { return m_dbUsers; }
        UserNameMap& GetUserNames() { return m_userNames; }
        UserNameMap const& GetUserNames() const { return m_userNames; }
        ConnectedUser* GetUser(uint32_t dbId) const;
        ConnectedUser* GetUser(std::string_view name) const;
        std::mutex& GetLocker() { return m_locker; }
        void SetLastPlayer(Player const* user) { m_lastPlayer = user; }

        void OnPlayerDisconnected(Player const* user);
        Game* GetGameForPlayer();
};
