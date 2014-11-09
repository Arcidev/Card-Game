#include "Player.h"
#include "Game.h"
#include "serverNetwork.h"
#include "../Shared/Packet.h"
#include "../Shared/networkServices.h"
#include "../Shared/SharedDefines.h"

Player::Player(uint32_t id, SOCKET socket, Game* game, ServerNetwork* network) : m_id(id), m_currentCard(nullptr), m_game(game), m_network(network), m_socket(socket), m_name("<unknown>"){}

Player::~Player()
{
    m_game->RemovePlayer(m_id);
    m_network->OnPlayerDisconnected(this);
    shutdown(m_socket, SD_BOTH);
    printf("Client %d: Connection closed\r\n", m_id);
}

void Player::Attack(Player* victim, uint64_t victimCardGuid)
{
    Card* victimCard = victim->GetCard(victimCardGuid);

    if (!victimCard || !m_currentCard)
        return;

    uint damage = m_currentCard->GetDamage() - victimCard->GetDefense();
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
    Packet packet(data, dataLength);
    while (packet.GetReadPosition() < dataLength)
    {
        uint16_t packetType;
        packet >> packetType;

        printf("Client %d: sended packet %d\r\n", m_id, packetType);

        switch (packetType)
        {
            case CMSG_INIT_PACKET:
                handleInitPacket(packet);
                break;
            case CMSG_CHAT_MESSAGE:
                handlChatPacket(packet);
                break;
            default:
                return;
        }
    }
}

void Player::handleInitPacket(Packet& packet)
{
    packet >> m_name;
    sendInitResponse();
    if (m_game->IsFull())
        m_game->GetOpponent(this)->sendInitResponse();
}

void Player::handlChatPacket(Packet& packet)
{
    uint8_t chatId;
    std::string message;
    packet >> chatId;
    packet >> message;

    Packet pck(SMSG_CHAT_MESSAGE);
    pck << chatId;
    pck << m_name;
    pck << message;

    switch (chatId)
    {
        case CHAT_GLOBAL:
            m_network->BroadcastPacket(&pck);
            break;
        case CHAT_LOCAL:
            m_game->BroadcastPacket(&pck);
            break;
        default:
            break;
    }
}

void Player::sendInitResponse() const
{
    Packet pck(SMSG_INIT_RESPONSE);
    pck.WriteBit(m_game->IsFull());
    pck.FlushBits();
    if (m_game->IsFull())
    {
        Player* opponent = m_game->GetOpponent(this);
        pck << opponent->GetID();
        pck << opponent->GetName();
    }

    SendPacket(&pck);
}

void Player::SendPacket(Packet const* packet) const
{
    NetworkServices::sendMessage(m_socket, &packet->GetStorage()[0], packet->GetStorage().size());
}
