#include "PacketHandler.h"
#include "../Shared/Packet.h"
#include "Player.h"
#include "Game.h"
#include "serverNetwork.h"
#include "DataHolder.h"
#include "../Shared/SharedDefines.h"
#include "../Crypto/Aes.h"
#include "../Crypto/Rsa.h"
#include "../Crypto/Keys/RSAPrivateKey.h"

PacketHandlerMap const PacketHandler::packetHandlers =
{
    { CMSG_INIT_PACKET, handleInitPacket },
    { CMSG_CHAT_MESSAGE, handleChatPacket },
    { CMSG_SELECTED_CARDS, handleSelectedCardsPacket }
};

void PacketHandler::handleInitPacket(Player* player, Packet* packet)
{
    std::string aesKey;
    *packet >> aesKey;
    aesKey = Rsa::Decrypt(aesKey, privateKey, false);
    player->SetEncryptionKey(aesKey);

    std::string name;
    *packet >> name;
    player->SetName(Aes::Decrypt(name, player->GetEncryptionKey()).c_str() /*to trim \0 at the end*/);
    player->SendInitResponse();
    player->SendAvailableCards();

    DEBUG_LOG("CMSG_INIT_PACKET:\n\tName: %s\n", player->GetName().c_str());
    if (player->GetGame()->IsFull())
        player->GetGame()->GetOpponent(player)->SendInitResponse();
}

void PacketHandler::handleChatPacket(Player* player, Packet* packet)
{
    uint8_t chatId;
    std::string message;
    *packet >> chatId;
    *packet >> message;

    DEBUG_LOG("CMSG_CHAT_MESSAGE:\n\tChatId: %d\n\tMessage: %s\n", chatId, message.c_str());
    Packet pck(SMSG_CHAT_MESSAGE);
    pck << chatId;
    pck << player->GetName();
    pck << message;

    switch (chatId)
    {
        case CHAT_GLOBAL:
            player->GetNetwork()->BroadcastPacket(&pck);
            break;
        case CHAT_LOCAL:
            player->GetGame()->BroadcastPacket(&pck);
            break;
        case CHAT_WHISPER:
        {
            std::string receiverName;
            *packet >> receiverName;
            player->SendChatWhisperResponse(message, receiverName, player->GetNetwork()->SendPacketToPlayer(receiverName, &pck));
            break;
        }
        default:
            break;
    }
}

void PacketHandler::handleSelectedCardsPacket(Player* player, Packet* packet)
{
    uint32_t cardId;
    uint8_t cardCount;
    *packet >> cardCount;
    if (cardCount != MAX_CARDS_COUNT)
        player->SendSelectCardsFailed(INVALID_CARD_COUNT);

    DEBUG_LOG("CMSG_SELECTED_CARDS:\n\tCardCount: %d\n", cardCount);
    CardsDataMap const& cards = DataHolder::GetCards();

    for (uint8_t i = 0; i < MAX_CARDS_COUNT; i++)
    {
        *packet >> cardId;
        DEBUG_LOG("\tCardId: %d\n", cardId);

        if (cards.find(cardId) == cards.end())
        {
            // Rest of packet is ignored
            player->SendSelectCardsFailed(INVALID_CARD_ID);
            return;
        }
    }
}

PacketHandlerFunc PacketHandler::GetPacketHandler(uint16_t packetId)
{
    PacketHandlerMap::const_iterator iter = packetHandlers.find(packetId);
    return (iter != packetHandlers.end()) ? iter->second : nullptr;
}
