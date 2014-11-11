#include <cstdlib>
#include "clientGame.h"
#include "cardGame.h"
#include "../Shared/Aes.h"
#include "../Shared/Rsa.h"
#include "../Shared/Sha.h"

ClientGame::ClientGame(CardGame* cardGame, QString const& playerName) : m_name(playerName), m_cardGame(cardGame)
{
    Packet packet(CMSG_INIT_PACKET);
    packet << Aes::Encrypt(playerName.toStdString());
    SendPacket(packet);
}

void ClientGame::handleInitPacket(Packet& packet)
{
    bool hasOpponent = packet.ReadBit();
    m_cardGame->ShowWaitForPlayer(!hasOpponent);

    if (hasOpponent)
    {
        uint32_t opponentID;
        std::string opponentName;
        packet >> opponentID;
        packet >> opponentName;
    }
}

void ClientGame::Update()
{
    int data_length = m_network.ReceivePackets(network_data);

    if (data_length < 2)
        return;

    Packet packet(network_data, data_length);
    while (packet.GetReadPosition() < (uint32_t)data_length)
    {
        uint16_t packetType;
        packet >> packetType;

        switch (packetType)
        {
            case SMSG_INIT_RESPONSE:
                handleInitPacket(packet);
                break;
            case SMSG_CHAT_MESSAGE:
                m_cardGame->HandleChatPacket(&packet);
                break;
            default:
                return;
        }
    }
}

void ClientGame::SendPacket(Packet const& packet) const
{
    NetworkServices::sendMessage(m_network.GetConnectionSocket(), &packet.GetStorage()[0], packet.GetStorage().size());
}
