#include <cstdlib>
#include "clientGame.h"
#include "cardGame.h"
#include "../Crypto/Aes.h"
#include "../Crypto/Rsa.h"
#include "../Crypto/Keys/RSAPublicKey.h"

ClientGame::ClientGame(CardGame* cardGame, QString const& playerName) : m_name(playerName), m_cardGame(cardGame), m_AesKey(Aes::GenerateKey())
{
    Packet packet(CMSG_INIT_PACKET);
    packet << Rsa::Encrypt(m_AesKey, publicKey, true);
    packet << Aes::Encrypt(playerName.toStdString(), m_AesKey);
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
