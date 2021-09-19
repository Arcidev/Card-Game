#include <iostream>
#include "ConnectedUser.h"
#include "Player.h"
#include "NetworkServices.h"
#include "ServerNetwork.h"
#include "PacketHandlers/PacketHandler.h"
#include "../Crypto/Aes.h"
#include "../Database/DatabaseInstance.h"
#include "../Shared/SharedDefines.h"

ConnectedUser::ConnectedUser(uint32_t serverId, SOCKET socket, ServerNetwork* network, AchievementManager* achievementMgr) : m_serverId(serverId), m_databaseId(0), m_name(""), m_disconnected(false), m_socket(socket), m_network(network), m_player(nullptr), m_achievementMgr(achievementMgr) { }

ConnectedUser::~ConnectedUser()
{
    if (m_player)
        delete m_player;
}

void ConnectedUser::OnLoggedIn(uint32_t databaseId)
{
    if (m_databaseId)
        return;

    m_databaseId = databaseId;
    handleAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_LOGIN, [](AchievementCriteria& criteria) { criteria.SetProgress(criteria.GetProgress() + 1); });
}

void ConnectedUser::OnGameEnded()
{
    handleAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_GAME_PLAYED, [](AchievementCriteria& criteria) { criteria.SetProgress(criteria.GetProgress() + 1); });
}

void ConnectedUser::OnGameWon()
{
    OnGameEnded();
    handleAchievementCriteria(CriteriaTypes::CRITERIA_TYPE_GAME_WON, [](AchievementCriteria& criteria) { criteria.SetProgress(criteria.GetProgress() + 1); });
}

void ConnectedUser::CreatePlayer()
{
    m_network->GetLocker().lock();

    Game* game = m_network->GetGameForPlayer();
    m_player = new Player(game, this);
    m_network->SetLastPlayer(m_player);
    game->AddPlayer(m_player);

    m_network->GetLocker().unlock();
}

void ConnectedUser::Disconnect()
{
    if (m_disconnected)
        return;

    m_disconnected = true;
    if (m_player)
        m_player->Disconnect();

    shutdown(m_socket, SD_BOTH);
    closesocket(m_socket);

    DatabaseInstance::GetDbCommandHandler().UpdateUserLastLoginTime(m_databaseId);
    DEBUG_LOG("Client %d: Connection closed\r\n", m_databaseId);
}

// Receive encrypted packet from client
void ConnectedUser::ReceivePacket(uint32_t dataLength, char const* data)
{
    uint32_t readedData = 0;

    while (readedData < dataLength)
    {
        uint16_t packetLength;
        std::memcpy(&packetLength, data + readedData, sizeof(uint16_t));
        std::vector<uint8_t> networkData;
        networkData.resize(packetLength);
        std::memcpy(&networkData[0], data + sizeof(uint16_t) + readedData, packetLength);

        // Inicializes readable packet with decrypted data
        Packet packet(Aes::Decrypt(networkData, m_AesEncryptor.Key, m_AesEncryptor.IVec));
        try
        {
            uint16_t packetType;
            packet >> packetType;

            PacketHandlerFuncWrapper packetHandler = PacketHandler::GetPacketHandler(packetType);
            packetHandler.Invoke(this, packet);
        }
        catch (std::out_of_range const& ex)
        {
            std::cerr << "Error occured while reading packet: " << ex.what() << std::endl;
        }

        readedData += dataLength + sizeof(uint16_t);
    }
}

// Sends encrypted packet to client
void ConnectedUser::SendPacket(Packet const& packet) const
{
    std::vector<uint8_t> encrypted = Aes::Encrypt(packet.GetStorage(), m_AesEncryptor.Key, m_AesEncryptor.IVec);
    uint16_t size = (uint16_t)encrypted.size();
    std::vector<uint8_t> toSend(sizeof(uint16_t) + size);
    std::memcpy(&toSend[0], (uint8_t*)&size, sizeof(uint16_t));
    std::memcpy(&toSend[0] + sizeof(uint16_t), &encrypted[0], size);

    NetworkServices::SendMessage(m_socket, &toSend[0], (int)toSend.size());
}

// Sends whisper response to sender
void ConnectedUser::SendChatWhisperResponse(std::string_view message, std::string_view receiver, bool success) const
{
    Packet pck(success ? SMSGPackets::SMSG_CHAT_MESSAGE : SMSGPackets::SMSG_WHISPER_FAILED);

    if (success)
    {
        pck << (uint8_t)CHAT_WHISPER_RESPONSE;
        pck << receiver;
        pck << message;
    }
    else
        pck << receiver;

    SendPacket(pck);
}

// Sends response to init packet
void ConnectedUser::SendInitResponse() const
{
    Packet pck(SMSGPackets::SMSG_INIT_RESPONSE);
    SendPacket(pck);
}

void ConnectedUser::handleAchievementCriteria(CriteriaTypes type, std::function<void(AchievementCriteria&)> func)
{
    AchievementMap& achievements = m_databaseId != 0 && m_achievements.empty() ? (m_achievements = m_achievementMgr->GetPlayerAchievements(m_databaseId)) : m_achievements;
    for (auto& achievement : achievements)
    {
        if (achievement.second.IsCompleted())
            continue;

        for (auto& criteria : achievement.second.GetCriterias())
        {
            if (criteria.IsMet())
                continue;

            if (criteria.GetType() == type)
                func(criteria);
        }

        if (achievement.second.IsCompleted())
        {
            Packet pck(SMSGPackets::SMSG_ACHIEVEMENT_EARNED);
            pck << achievement.first;
            SendPacket(pck);
        }
    }
}
