#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include "../Multiplatform/NetworkCommunication.h"

class Packet;
class Player;
class ServerNetwork;

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

        SOCKET m_socket;
        ServerNetwork* m_network;
        Player* m_player;
        AesEncryptor m_AesEncryptor;

    public:
        ConnectedUser(uint32_t serverId, SOCKET socket, ServerNetwork* network);
        ~ConnectedUser();

        void SetDatabaseUserId(uint32_t id) { m_databaseId = id; }
        void SetName(std::string_view name) { m_name = name; }
        void SetAesEncryptor(std::vector<uint8_t> const& key, std::vector<uint8_t> const& iVec) { m_AesEncryptor.Key = key; m_AesEncryptor.IVec = iVec; }

        void CreatePlayer();
        void Disconnect();
        void ReceivePacket(uint32_t length, char const* packetData);
        void SendPacket(Packet const& packet) const;
        void SendChatWhisperResponse(std::string_view message, std::string_view receiver, bool success) const;
        void SendInitResponse() const;

        uint32_t GetId() const { return m_serverId; }
        bool IsLoggedIn() const { return m_serverId != 0; }
        std::string_view GetName() const { return m_name; }
        SOCKET GetSocket() const { return m_socket; }
        ServerNetwork* GetNetwork() const { return m_network; }
        AesEncryptor const& GetAesEncryptor() const { return m_AesEncryptor; }
        bool IsDisconnected() const { return m_disconnected; }
        Player* GetPlayer() const { return m_player; }
};

