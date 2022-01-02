#pragma once
#include <vector>
#include "Friend.h"
#include "Achievements/AchievementMgr.h"
#include "../Multiplatform/NetworkCommunication.h"

class Packet;
class Player;
class ServerNetwork;

typedef std::map<uint32_t, std::string> BlockedUsersMap;
struct AesEncryptor
{
    std::vector<uint8_t> Key;
    std::vector<uint8_t> IVec;
};

class ConnectedUser
{
    private:
        uint32_t m_serverId;
        uint32_t m_databaseId;
        std::string m_name;
        bool m_disconnected;
        AchievementMap m_achievements;

        SOCKET m_socket;
        ServerNetwork* m_network;
        Player* m_player;
        AesEncryptor m_AesEncryptor;
        AchievementManager* m_achievementMgr;
        std::list<Friend> m_friends;
        BlockedUsersMap m_blockedUsers;

        void handleAchievementCriteria(CriteriaTypes type, std::function<void (AchievementCriteria&)> func);
    public:
        ConnectedUser(uint32_t serverId, SOCKET socket, ServerNetwork* network, AchievementManager* achievementMgr);
        ~ConnectedUser();

        void OnLoggedIn(uint32_t id);
        void OnGameEnded();
        void OnGameWon();

        void SetName(std::string_view name) { m_name = name; }
        void SetAesEncryptor(std::vector<uint8_t> const& key, std::vector<uint8_t> const& iVec) { m_AesEncryptor.Key = key; m_AesEncryptor.IVec = iVec; }

        void CreatePlayer();
        void UnsetPlayer() { m_player = nullptr; }
        void Disconnect();
        void ReceivePacket(uint32_t length, char const* packetData);
        void SendPacket(Packet const& packet) const;
        void SendChatWhisperResponse(std::string_view message, std::string_view receiver, bool success) const;
        void SendInitResponse() const;

        uint32_t GetId() const { return m_serverId; }
        uint32_t GetDatabaseId() const { return m_databaseId; }
        std::string_view GetName() const { return m_name; }
        SOCKET GetSocket() const { return m_socket; }
        ServerNetwork* GetNetwork() const { return m_network; }
        AesEncryptor const& GetAesEncryptor() const { return m_AesEncryptor; }
        bool IsDisconnected() const { return m_disconnected; }
        Player* GetPlayer() const { return m_player; }
        AchievementMap const& GetAchievements() { return m_databaseId != 0 && m_achievements.empty() ? (m_achievements = m_achievementMgr->GetPlayerAchievements(m_databaseId)) : m_achievements; }
        bool IsUserBlocked(uint32_t userId) const { return m_blockedUsers.contains(userId); }
        std::list<Friend> const& GetFriends() const { return m_friends; }
        BlockedUsersMap const& GetBlockedUsers() const { return m_blockedUsers; }
        void AddFriend(uint32_t id, std::string_view name) { m_friends.push_back(Friend(id, name, m_network)); }
        void BlockUser(uint32_t id, std::string_view name) { m_blockedUsers[id] = name; }
        void RemoveFriend(uint32_t userId) { m_friends.remove_if([&userId](Friend const& userFriend) { return userFriend.GetId() == userId; }); }
        void RemoveBlockedUser(uint32_t userId) { m_blockedUsers.erase(userId); }
};
