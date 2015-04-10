#include <stdexcept>
#include <algorithm>
#include <random>
#include "Player.h"
#include "serverNetwork.h"
#include "DataHolder.h"
#include "PacketHandler.h"
#include "../Crypto/Aes.h"
#include "Packet.h"
#include "../Shared/SharedDefines.h"

Player::Player(uint32_t id, SOCKET socket, Game* game, ServerNetwork* network) : m_isPrepared(false), m_isDisconnected(false), m_id(id), m_currentCard(nullptr), m_game(game), m_network(network), m_socket(socket), m_name("<unknown>"){}

void Player::Disconnect()
{
    m_isDisconnected = true;
    m_game->DisconnectPlayer(m_id);
    m_network->OnPlayerDisconnected(this);
    shutdown(m_socket, SD_BOTH);
    DEBUG_LOG("Client %d: Connection closed\r\n", m_id);
}

void Player::Attack(Player* victim, uint64_t victimCardGuid)
{
    Card* victimCard = victim->GetCard(victimCardGuid);

    if (!victimCard || !m_currentCard)
        return;

    uint32_t damage = m_currentCard->GetDamage() - victimCard->GetDefense();
    victimCard->DealDamage(damage);

    if (!victimCard->IsAlive())
        victim->DestroyCard(victimCardGuid);
}

void Player::DestroyCard(uint64_t cardGuid)
{
    CardsMap::iterator iter = m_cards.find(cardGuid);
    if (iter == m_cards.end())
        return;

    m_cards.erase(iter);
}

inline Card* Player::GetCard(uint64_t cardGuid)
{
    CardsMap::iterator iter = m_cards.find(cardGuid);
    return (iter == m_cards.end()) ? nullptr : &iter->second;
}

void Player::ReceivePacket(uint32_t dataLength, char const* data)
{
    uint32_t readedData = 0;
    
    while (readedData < dataLength)
    {
        uint16_t packetLength;
        std::memcpy(&packetLength, data + readedData, sizeof(uint16_t));
        std::string networkData;
        networkData.resize(packetLength);
        std::memcpy(&networkData[0], data + sizeof(uint16_t) + readedData, packetLength);

        Packet packet(Aes::Decrypt(networkData, m_AesKey));
        try
        {
            uint16_t packetType;
            packet >> packetType;

            PacketHandlerFunc packetHandler = PacketHandler::GetPacketHandler(packetType);
            if (packetHandler)
                packetHandler(this, &packet);
        }
        catch (std::out_of_range ex)
        {
            printf("Error occured while reading packet: %s\n", ex.what());
        }

        readedData += dataLength + sizeof(uint16_t);
    }
}

void Player::SendAvailableCards() const
{
    // can be static, always sending the same
    CardsDataMap cards = DataHolder::GetCards();
    Packet packet(SMSG_AVAILABLE_CARDS);
    packet << (uint16_t)cards.size();
    for (CardsDataMap::const_iterator iter = cards.begin(); iter != cards.end(); ++iter)
    {
        packet << iter->second.GetId();
        packet << iter->second.GetType();
        packet << iter->second.GetHealth();
        packet << iter->second.GetDamage();
        packet << iter->second.GetMana();
        packet << iter->second.GetDefense();
    }

    SendPacket(&packet);
}

void Player::SendChatWhisperResponse(std::string const& message, std::string const& receiver, bool success) const
{
    Packet pck(success ? SMSG_CHAT_MESSAGE : SMSG_WHISPER_FAILED);

    if (success)
    {
        pck << (uint8_t)CHAT_WHISPER_RESPONSE;
        pck << receiver;
        pck << message;
    }
    else
        pck << receiver;
    
    SendPacket(&pck);
}

void Player::SendSelectCardsFailed(uint8_t failReason) const
{
    Packet packet(SMSG_SELECT_CARDS_FAILED);
    packet << failReason;

    SendPacket(&packet);
}

void Player::SendInitResponse() const
{
    Packet pck(SMSG_INIT_RESPONSE);
    pck.WriteBit(m_game->IsFull());
    pck.FlushBits();

    pck << m_id;
    if (m_game->IsFull())
    {
        pck << GetOpponent()->GetId();
        pck << GetOpponent()->GetName();
    }

    SendPacket(&pck);
}

void Player::SendPlayerDisconnected() const
{
    Packet packet(SMSG_PLAYER_DISCONNECTED);
    SendPacket(&packet);
}

void Player::SendPacket(Packet const* packet) const
{
    std::string encrypted = Aes::Encrypt(std::string(packet->GetStorage().begin(), packet->GetStorage().end()), m_AesKey);
    uint16_t size = encrypted.length();
    std::vector<uint8_t> toSend(sizeof(uint16_t) + size);
    std::memcpy(&toSend[0], (uint8_t *)&size, sizeof(uint16_t));
    std::memcpy(&toSend[0] + sizeof(uint16_t), encrypted.c_str(), size);

    NetworkServices::sendMessage(m_socket, &toSend[0], toSend.size());
}

void Player::CreateCard(Card const& cardTemplate)
{
    Card card(cardTemplate.GetId(), cardTemplate.GetType(), cardTemplate.GetHealth(), cardTemplate.GetDamage(), cardTemplate.GetMana(), cardTemplate.GetDefense());
    card.SetGuid(m_game->GetNextCardGuid());

    m_cards.insert(std::make_pair(card.GetGuid(), card));
    m_cardOrder.push_back(GetCard(card.GetGuid()));
}

void Player::Prepare()
{
    m_isPrepared = true;
    std::shuffle(m_cardOrder.begin(), m_cardOrder.end(), std::default_random_engine(rand()));
}

void Player::HandleDeckCards(bool addCard)
{
    if (addCard && (!m_cardOrder.empty() || (m_currentCards.size() < MAX_CARDS_ON_DECK)))
    {
        m_currentCards.push_back(m_cardOrder.front());
        m_cardOrder.erase(m_cardOrder.begin());
    }
    
    uint8_t cardsCount = m_currentCards.size();
    Packet packet(SMSG_DECK_CARDS);
    packet << cardsCount;
    for (uint8_t i = 0; i < cardsCount; i++)
        packet.WriteGuidBitStreamInOrder(m_currentCards[i]->GetGuid(), std::vector<uint8_t> { 7, 2, 1, 4, 5, 0, 6, 3 });
    packet.FlushBits();

    packet << m_id;
    for (uint8_t i = 0; i < cardsCount; i++)
        packet.WriteGuidByteStreamInOrder(m_currentCards[i]->GetGuid(), std::vector<uint8_t> { 2, 1, 7, 6, 0, 5, 3, 4 });
    
    m_game->BroadcastPacket(&packet);
}
