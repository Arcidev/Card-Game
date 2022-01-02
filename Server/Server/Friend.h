#pragma once
#include <cstdint>
#include <string>

class ConnectedUser;
class ServerNetwork;

class Friend
{
    private:
        uint32_t m_userId;
        std::string m_userName;
        ServerNetwork const* m_network;

        ConnectedUser const* getUser() const;

    public:
        Friend(uint32_t userId, std::string_view userName, ServerNetwork const* network);

        uint32_t GetId() const { return m_userId; }
        std::string_view GetName() const { return m_userName; }
        bool IsOnline() const;
        void SendOwnersOnlineStatus(std::string_view name, bool isOnline) const;
};
